//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "StorageDevice.h"

namespace fattreenew {

Define_Module(StorageDevice);

StorageDevice::StorageDevice(){}

StorageDevice::~StorageDevice(){
    while(!storage_queue->isEmpty()){
        delete(storage_queue->pop());
    }
}

void StorageDevice::initialize()
{
    queue_full = false;
    storage_queue = new cQueue("storageQueue");
    storage_queue->setup(comp);

    qLenSignal = registerSignal("queueLength");
}

void StorageDevice::handleMessage(cMessage *msg)
{
    // if msg tranfer to storage device, it means the queue at this moment is not full;
    Request* req = check_and_cast<Request*>(msg);

    if(!msg->isSelfMessage()){
        emit(qLenSignal, storage_queue->getLength());
        int gate_id(intuniform(0, gateSize("port$o")-1, 0));  // randomly select a channel
        req->setPort_index(gate_id);
        req->setArriveModule_time(simTime());
        updateMsgProcTime(req);
        scheduleAt(req->getLeaveModule_time(), req->dup());
        storage_queue->insert(req);
    }else{
        delete(storage_queue->pop()); // send the copy of scheduled msg above, so pop one in queue.
        cGate* g = gate("port$o", req->getPort_index());
        simtime_t new_del_time = transTimestampByCable(g);
        sendDelayed(req, new_del_time-simTime(), "port$o", req->getPort_index());
    }

    if(storage_queue->getLength() == par("max_queue_len").intValue())
        queue_full = true;
    else if(storage_queue->getLength() < par("max_queue_len").intValue())
        queue_full = false;
//    else
//        cRuntimeError("queue length out of control in %s\n", getFullName());
}

void StorageDevice::updateMsgProcTime(Request* req) {
    if(req->getWork_type() == 'r'){
        req->setByteLength(req->getFrag_size());
    }else if(req->getWork_type() == 'w'){
        req->setByteLength(0);
    }else{
        throw cRuntimeError("Need define new rules for type: %c !\n", req->getWork_type());
    }

    double proc_time;
    if(req->getWork_type() == 'r'){
        proc_time = 8.0 / par("read_bw").doubleValue();
        proc_time = proc_time * (req->getFrag_size() / (double)MB );
    }else{
        proc_time = 8.0 / par("write_bw").doubleValue();
        proc_time = proc_time * (req->getFrag_size() / (double)MB );
    }

    if(storage_queue->getLength() < par("parallel_level").intValue()){
        req->setLeaveModule_time(req->getArriveModule_time() + proc_time);
    }else{
        auto last_req = check_and_cast<Request*>(storage_queue->back());
        req->setLeaveModule_time(last_req->getLeaveModule_time() + proc_time);
    }

    req->setFinished(true);
    req->setProc_time(proc_time);
}

const bool StorageDevice::isFree() {
    return !queue_full;
}

} //namespace
