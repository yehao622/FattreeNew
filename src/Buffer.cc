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

#include "Buffer.h"
#include "StorageDevice.h"

namespace fattreenew {

Define_Module(Buffer);

void Buffer::initialize()
{
//    buffer_full = false;

    if(strcmp(getName(), "flashBuffer") == 0){
        avail_buffer_size = par("flash_buffer").doubleValue();
        read_bw = par("read_storage_flash_bw").doubleValue();
        write_bw = par("write_storage_flash_bw").doubleValue();
    }else if(strcmp(getName(), "oss_memory")==0 || strcmp(getName(), "cn_memory")==0){
        avail_buffer_size = par("DRAM_buffer").doubleValue();
        read_bw = par("read_DRAM_buffer_bw").doubleValue();
        write_bw = par("write_DRAM_buffer_bw").doubleValue();
    }else if(strcmp(getName(), "hcaBuffer")==0 || strcmp(getName(), "hbaBuffer")==0 || strcmp(getName(), "core")==0){
        avail_buffer_size = par("SRAM_buffer").doubleValue();
        read_bw = par("read_SRAM_buffer_bw").doubleValue();
        write_bw = par("write_SRAM_buffer_bw").doubleValue();
    }else if(strcmp(getName(), "aggr")==0 || strcmp(getName(), "edge")==0){
        avail_buffer_size = par("switch_buffer").doubleValue();
        read_bw = par("read_switch_bw").doubleValue();
        write_bw = par("write_switch_bw").doubleValue();
    }else{
        cRuntimeError("Need to define/rename a new buffer!\n");
    }

    std::string q_name = getName();
    q_name.append("Queue");
    buffer_queue = new cQueue(q_name.c_str());
    buffer_queue->setup(comp);

    qLenSignal = registerSignal("queueLength");
}

void Buffer::handleMessage(cMessage *msg)
{
    Request* req = check_and_cast<Request*>(msg);

    if(strcmp(getName(), "flashBuffer") == 0){ // if at OST's flash buffer
        if(!msg->isSelfMessage()){
            emit(qLenSignal, buffer_queue->getLength());
            req->setArriveModule_time(simTime());

            if((!req->getFinished() && !checkDiskStatus()) ||
                    avail_buffer_size < (double)req->getByteLength()/MB){
                buffer_queue->insert(req);
            }else{
                avail_buffer_size -= (double)req->getByteLength() / MB;
                simtime_t later_time = calcSendDelay(req);
                scheduleAt(later_time, req);
            }
        }else{
            avail_buffer_size += (double)req->getByteLength() / MB;
            if(req->getFinished()){
                send(req, "port$o", getGateTo("port$o", "payloadOST"));
            }else{
                send(req, "port$o", getGateTo("port$o", "storageDevice")); // Assume one flash memory coonected with only 1 disk drive!
            }
            sendFromBuffer();
        }
    }

    else if(strcmp(getName(), "hcaBuffer")==0 || strcmp(getName(), "hbaBuffer")==0){
        if(!msg->isSelfMessage()){
            req->setArriveModule_time(simTime());

            if(avail_buffer_size < (double)MTU/MB){
                buffer_queue->insert(req);
            }else{
                avail_buffer_size -= (double)req->getByteLength() / MB;
                simtime_t later_time = calcSendDelay(req);
                scheduleAt(later_time, req);
            }
        }else{
            avail_buffer_size += (double)req->getByteLength() / MB;
            if(strcmp(getName(), "hcaBuffer") == 0)
                send(req, "port$o", getGateTo("port$o", "hca_payload"));
            else if(strcmp(getName(), "hbaBuffer") == 0)
                send(req, "port$o", getGateTo("port$o", "hba_payload"));
            sendFromBuffer();
        }

    }

    else if(strcmp(getName(), "oss_memory") == 0){
        emit(qLenSignal, buffer_queue->getLength());
        if(!msg->isSelfMessage()){
            req->setArriveModule_time(simTime());

            if(avail_buffer_size < (double)req->getByteLength() / MB){
                buffer_queue->insert(req);
            }else{
                avail_buffer_size -= (double)req->getByteLength() / MB;
                simtime_t later_time = calcSendDelay(req);
                if(strcmp(req->getSenderModule()->getName(), "oss_hub_mem_hca") == 0){
                    req->setNext_hop_addr("oss_hub_mem_hba");
                }else if(strcmp(req->getSenderModule()->getName(), "oss_hub_hba_ost") == 0){
                    req->setNext_hop_addr("oss_hub_mem_hca");
                }
                scheduleAt(later_time, req);
            }
        }else{
            avail_buffer_size += (double)req->getByteLength() / MB;
            send(req, "port$o", getGateTo("port$o", req->getNext_hop_addr()));
            sendFromBuffer();
        }
    }

    else if(strcmp(getName(), "cn_memory") == 0){
            if(!msg->isSelfMessage()){
                req->setArriveModule_time(simTime());

                if(avail_buffer_size < (double)req->getByteLength() / MB){
                    buffer_queue->insert(req);
                }else{
                    avail_buffer_size -= (double)req->getByteLength() / MB;
                    simtime_t later_time = calcSendDelay(req);
                    scheduleAt(later_time, req);
                }
            }else{
                avail_buffer_size += (double)req->getByteLength() / MB;
                send(req, "port$o", getGateTo("port$o", "cn_memory_hca"));
                sendFromBuffer();
            }
        }

    else if(strcmp(getName(), "core")==0 || strcmp(getName(), "aggr")==0 || strcmp(getName(), "edge")==0){
            if(!msg->isSelfMessage()){
                req->setArriveModule_time(simTime());
                std::string cur_path = req->getSendPath();
                cur_path.empty() ? req->setNext_hop_addr(popPath(req, 'b').c_str()) : req->setNext_hop_addr(popPath(req, 's').c_str());

                if(avail_buffer_size < (double)MTU/MB){
                    buffer_queue->insert(req);
                }else{
                    avail_buffer_size -= (double)req->getByteLength() / MB;
                    simtime_t later_time = calcSendDelay(req);
                    scheduleAt(later_time, req);
                }

            }else{
                avail_buffer_size += (double)req->getByteLength() / MB;
                send(req, "port$o", getGateTo("port$o", req->getNext_hop_addr()));
                sendFromBuffer();
            }

        }

}

simtime_t Buffer::calcSendDelay(Request* req) {
    double proc_time;
    if(req->getWork_type() == 'r'){
        proc_time = 8.0 / read_bw;
        proc_time = proc_time * (req->getByteLength() / (double)MB );
    }else{
        proc_time = 8.0 / write_bw;
        proc_time = proc_time * (req->getByteLength() / (double)MB );
    }

    return simTime() + proc_time;
}

const bool Buffer::checkDiskStatus() {
    for(int i=0; i<gateSize("port$o"); i++) {
        cGate* g = gate("port$o", i);
        if(strcmp(g->getNextGate()->getOwnerModule()->getName(), "storageDevice") == 0){
            StorageDevice* dev = check_and_cast<StorageDevice*>(g->getNextGate()->getOwnerModule());
            return dev->isFree();
        }
    }
    return false;
}

void Buffer::sendFromBuffer() {
    if(buffer_queue->isEmpty()) return;
    if(strcmp(getName(), "flashBuffer")==0 && !checkDiskStatus()) return;

    Request* req_in_queue = check_and_cast<Request*>(buffer_queue->pop());
    avail_buffer_size -= (double)req_in_queue->getByteLength() / MB;
    simtime_t later_time = calcSendDelay(req_in_queue);
    scheduleAt(later_time, req_in_queue);
}

int Buffer::getGateTo(const char* gate_type, const char* dest) {
    std::string cur_mod_name = getFullName();
    if(system_layout.count(cur_mod_name) && system_layout[cur_mod_name].count(dest))
        return system_layout[cur_mod_name][dest].second;

    std::vector<int> gate_vec;
    for(int i=0; i<gateSize(gate_type); i++){
        cGate* g = gate(gate_type, i);
        if(strcmp(g->getNextGate()->getOwnerModule()->getName(), dest) == 0){
            gate_vec.push_back(i);
        }
    }
    return gate_vec[intuniform(0, gate_vec.size()-1, par("rng").intValue())];
}

} //namespace
