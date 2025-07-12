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

#include "Switch.h"

namespace fattreenew {

Define_Module(Switch);

Switch::Switch(){
    switch_buffer = new cQueue(getFullName());
}

Switch::~Switch(){
//    conn_map.clear();
    while(!switch_buffer->isEmpty()){
        delete(switch_buffer->pop());
    }
}

void Switch::initialize()
{

//    queueIsFull = false;
    qLenSignal = registerSignal("queueLen");
    staySignal = registerSignal("stayTime");
    waitingSignal = registerSignal("waitingTime");
}

void Switch::handleMessage(cMessage *msg)
{
    Request* req = check_and_cast<Request*>(msg);
    int gate_id;

    if(!msg->isSelfMessage()) { // if new msg arrives here

        if(strcmp(getName(), "edge") == 0){  // At Edge layer
            if(std::regex_match(req->getMaster_id_addr(), std::regex("cn\\[[0-9]+\\]")) &&
                    (!req->getCkp_launched() || req->getFinished())){
                if(!req->getCkp_launched()){
                    if(conn_map.count(req->getDes_addr()))
                        gate_id = conn_map[req->getDes_addr()];
                    else
                        gate_id = randChoose("aggr");
                }else{
                    if(conn_map.count(req->getSrc_addr()))
                        gate_id = conn_map[req->getSrc_addr()];
                    else
                        gate_id = randChoose("aggr");
                }
            }else{
                if(std::regex_match(req->getDes_addr(), std::regex("cn\\[[0-9]+\\]"))){ // destination is another CN
                    if(req->getFinished()){ // returned from destination CN
                        if(conn_map.count(req->getSrc_addr()))
                            gate_id = conn_map[req->getSrc_addr()];
                        else
                            gate_id = randChoose("aggr");
                    }else if(conn_map.count(req->getDes_addr())){
                        gate_id = conn_map[req->getDes_addr()];
                    }else{
                        gate_id = randChoose("aggr");
                    }
                }else{ // destination is OST
                    if(req->getWork_type() == 'r'){
                        if(req->getByteLength()){ // read data from ost
                            gate_id = conn_map[req->getSrc_addr()];
                        }else{
                            gate_id = randChoose("aggr");
                        }
                    }else{
                        if(req->getByteLength()){ // write data to ost
                            gate_id = randChoose("aggr");
                        }else{                             // have written data to ost
                            gate_id = conn_map[req->getSrc_addr()];
                        }
                    }
                }
            }

        }else if(strcmp(getName(), "aggr") == 0){  // At Aggregation layer
            if(std::regex_match(req->getMaster_id_addr(), std::regex("cn\\[[0-9]+\\]")) &&
                    (!req->getCkp_launched() || req->getFinished())){
                if(!req->getCkp_launched()){
                    int forward_to_dest_id = findCN(req->getDes_addr(), "edge");
//                    if(strcmp(req->getSenderModule()->getName(), "edge") == 0 || forward_to_dest_id==-1)
                    if(forward_to_dest_id == -1)
                        gate_id = randChoose("core");
                    else
                        gate_id = forward_to_dest_id;
                }else{
                    int back_to_src_id = findCN(req->getSrc_addr(), "edge");
                    if(back_to_src_id == -1)
                        gate_id = randChoose("core");
                    else
                        gate_id = back_to_src_id;
                }
            }else{
            if(std::regex_match(req->getDes_addr(), std::regex("cn\\[[0-9]+\\]"))){ // destination is another CN
                if(strcmp(req->getSenderModule()->getName(), "edge") == 0){ // from Edge layer
                    int back_to_src_id = findCN(req->getSrc_addr(), "edge");
                    int forward_to_dest_id = findCN(req->getDes_addr(), "edge");
                    if(!req->getFinished()) { // if has not arrived target CN
                        if(forward_to_dest_id == -1)
                            gate_id = randChoose("core");
                        else
                            gate_id = forward_to_dest_id;
                    }else{
                        if(back_to_src_id == -1)
                            gate_id = randChoose("core");
                        else
                            gate_id = back_to_src_id;
                    }
                }else if(strcmp(req->getSenderModule()->getName(), "core") == 0){ // from Core layer
                    if(!req->getFinished())
                        gate_id = findCN(req->getDes_addr(), "edge");
                    else
                        gate_id = findCN(req->getSrc_addr(), "edge");
                }else{
                    throw cRuntimeError("Aggr layer connected with other unknown switches!\n");
                }
            }else{
                if(strcmp(req->getSenderModule()->getName(), "edge") == 0){ // from Edge layer
                    gate_id = randChoose("core");
                }else if(strcmp(req->getSenderModule()->getName(), "core") == 0){ // from Core layer
                    gate_id = findCN(req->getSrc_addr(), "edge");
                }else{
                    throw cRuntimeError("Aggr layer connected with other unknown switches!\n");
                }
            }
          }

        }else if(strcmp(getName(), "core") == 0){  // At Core layer
            if(std::regex_match(req->getMaster_id_addr(), std::regex("cn\\[[0-9]+\\]")) &&
                    !req->getCkp_launched()){
                gate_id = findAggr(req->getDes_addr());
            }else{
            if(std::regex_match(req->getDes_addr(), std::regex("cn\\[[0-9]+\\]"))){ // destination is another CN
                if(!req->getFinished())
                    gate_id = findAggr(req->getDes_addr());
                else
                    gate_id = findAggr(req->getSrc_addr());
            }else{
                if(strcmp(req->getSenderModule()->getName(), "aggr") == 0){
                    if(!conn_map.count("mds"))
                        throw cRuntimeError("No MDS %s exists!\n", req->getNext_hop_addr());
                    gate_id = conn_map["mds"];
                }else if(strcmp(req->getSenderModule()->getName(), "mds") == 0){
                    if(!conn_map.count(req->getNext_hop_addr()))
                        throw cRuntimeError("%s No such OSS %s exists!\n", getFullName(), req->getNext_hop_addr());
                    gate_id  = conn_map[req->getNext_hop_addr()];
                }else if(strcmp(req->getSenderModule()->getName(), "oss") == 0){
                    gate_id  = findAggr(req->getSrc_addr());
                    if(gate_id == -1)
                        throw cRuntimeError("Cannot assign gate from core switches to aggr");
                }else{
                    throw cRuntimeError("Core layer connected with other unknown switches!\n");
                }
            }
                       }

        }else{
            throw cRuntimeError("Unknown switch appears!\n");
        }

        double proc_time(0.0);
        if(strcmp(getName(), "core") || strcmp(req->getSenderModule()->getName(), "aggr") ||
                                       std::regex_match(req->getDes_addr(), std::regex("cn\\[[0-9]+\\]"))){
            emit(qLenSignal, geatRealQueueLength());
            req->setArriveModule_time(simTime());
            req->setPort_index(gate_id);
            simtime_t del_time = simTime();
//            if(gate("port$o", gate_id)->getChannel()->isTransmissionChannel())
//                del_time = std::max(gate("port$o", gate_id)->getTransmissionChannel()->getTransmissionFinishTime(), simTime());

            if(switch_buffer->getLength() < par("proc_num").intValue()){
                if(strcmp(getName(), "core") == 0) {
                    if(req->getByteLength()){
                        proc_time = par("core_latency").doubleValue();
                        req->setLeaveModule_time(del_time + proc_time);
                    }else{
                        req->setLeaveModule_time(del_time);
                    }
                }else if(strcmp(getName(), "aggr") == 0) {
                    if(req->getByteLength()){
                        proc_time = par("aggr_latency").doubleValue();
                        req->setLeaveModule_time(del_time + proc_time);
                    }else{
                        req->setLeaveModule_time(del_time);
                    }
                }else if(strcmp(getName(), "edge") == 0) {
                    if(req->getByteLength()){
                        proc_time = par("edge_latency").doubleValue();
                        req->setLeaveModule_time(del_time + proc_time);
                    }else{
                        req->setLeaveModule_time(del_time);
                    }
                }
            }else{
                auto last_req = check_and_cast<Request*>(switch_buffer->back());
                if(strcmp(getName(), "core") == 0) {
                    if(req->getByteLength()){
                        proc_time = par("core_latency").doubleValue();
                        req->setLeaveModule_time(last_req->getLeaveModule_time() + proc_time);
                    }else{
                        req->setLeaveModule_time(last_req->getLeaveModule_time());
                    }
                }else if(strcmp(getName(), "aggr") == 0) {
                    if(req->getByteLength()){
                        proc_time = par("aggr_latency").doubleValue();
                        req->setLeaveModule_time(last_req->getLeaveModule_time() + proc_time);
                    }else{
                        req->setLeaveModule_time(last_req->getLeaveModule_time());
                    }
                }else if(strcmp(getName(), "edge") == 0) {
                    if(req->getByteLength()){
                        proc_time = par("edge_latency").doubleValue();
                        req->setLeaveModule_time(last_req->getLeaveModule_time() + proc_time);
                    }else{
                        req->setLeaveModule_time(last_req->getLeaveModule_time());
                    }
                }
            }

            req->setProc_time(proc_time);
            scheduleAt(req->getLeaveModule_time(), req->dup());
            switch_buffer->insert(req);
        }else{ // core to mds and need goes to OST
            if(req->getByteLength()){
                proc_time = par("core_latency").doubleValue();
                sendDelayed(req, proc_time, "port$o", gate_id);
            }else{
                send(req, "port$o", gate_id);
            }
            req->setProc_time(proc_time);
        }

    } else {  // if self-message re-enter the queue
        delete(switch_buffer->pop()); // scheduled sending a copy above
        simtime_t new_del_time = simTime();
        if(gate("port$o", req->getPort_index())->getChannel()->isTransmissionChannel())
            new_del_time = std::max(gate("port$o", req->getPort_index())->getTransmissionChannel()->getTransmissionFinishTime(), simTime());

        std::string next_gate_name = gate("port$o", req->getPort_index())->getNextGate()->getOwnerModule()->getName();
        if(strcmp(next_gate_name.c_str(), "cn") && strcmp(next_gate_name.c_str(), "mds")){ // if not sent to CN or MDS
            if(strcmp(next_gate_name.c_str(), "oss") == 0){
                //                OSS* next_oss = check_and_cast<OSS*>(gate("port$o", req->getPort_index())->getNextGate()->getOwnerModule());
                //                if(!next_oss->queueIsFull){
                sendDelayed(req, new_del_time-simTime(), "port$o", req->getPort_index());
                emit(staySignal, (new_del_time-req->getArriveModule_time()).dbl());
                //                }else
                //                    delete req;
            }else{
                //                Switch* next_sw = check_and_cast<Switch*>(gate("port$o", req->getPort_index())->getNextGate()->getOwnerModule());
                //                if(!next_sw->queueIsFull){
                sendDelayed(req, new_del_time-simTime(), "port$o", req->getPort_index());
                emit(staySignal, (new_del_time-req->getArriveModule_time()).dbl());
                //                }else
                //                    delete req;
            }
        }else{
            sendDelayed(req, new_del_time-simTime(), "port$o", req->getPort_index());
        }

        emit(waitingSignal, (new_del_time-req->getArriveModule_time()).dbl() - req->getProc_time());
    }

}

void Switch::finish(){}

int Switch::randChoose(std::string layer){ // Randomly select a switch following uniform distribution, to core or aggr layer
    std::vector<int> all_gates;
    for(auto item:conn_map){
        if (std::regex_match(item.first, std::regex(layer+"\\[[0-9]+\\]"))){
            all_gates.push_back(item.second);
//            Switch* sw = check_and_cast<Switch*>(gate("port$o", item.second)->getNextGate()->getOwnerModule());
//            queue_data_size[item.second] = sw->getDataSizeInQueue();
        }
    }

    if(all_gates.empty())
        throw cRuntimeError("Cannot find available hop station at %s", getName());

    return all_gates[intuniform(0, all_gates.size()-1, 0)];

    // belowing part tries to select the least data size in queue
    int64_t min_data_size(INT64_MAX);
    int dest_port(INT_MIN);

    for(int port_ind:all_gates){
        if(queue_data_size[port_ind] <= min_data_size){
            min_data_size = queue_data_size[port_ind];
            dest_port = port_ind;
        }
    }

    return dest_port;
}

bool Switch::checkPort(std::string port_name){
    return conn_map.count(port_name);
}

int Switch::findCN(std::string src, std::string layer){ // find compute node from Aggr layer
    for(auto item:conn_map){
        if (std::regex_match(item.first, std::regex(layer+"\\[[0-9]+\\]"))){
            cGate* g = gate("port$o", item.second);
            if(layer == "edge"){
                Switch* sw = check_and_cast<Switch*>(g->getNextGate()->getOwnerModule()); // get edge switch
                if(sw->checkPort(src))  // check if corresponding CN connected
                    return item.second;
            }else if(layer == "oss"){
//                OSS* oss = check_and_cast<OSS*>(g->getNextGate()->getOwnerModule()); // get oss
//                if(oss->findOST(src))  // check if corresponding ost connected
//                    return item.second;
            }
        }
    }

    return -1;
}

int Switch::findAggr(std::string src){ // for core switch find OST under aggr. switch
    std::vector<int> avail_aggr;
    for(auto item:conn_map){
        if (std::regex_match(item.first, std::regex("aggr\\[[0-9]+\\]"))){
            cGate* g = gate("port$o", item.second);
            Switch* sw = check_and_cast<Switch*>(g->getNextGate()->getOwnerModule()); // get aggr switch
            if(sw->findCN(src, "edge") != -1){
                avail_aggr.push_back(item.second);
                queue_data_size[item.second] = sw->getDataSizeInQueue();
            }
        }
    }

    if(avail_aggr.empty())
        throw cRuntimeError("Cannot find available Aggr. Switch at %s", getName());

    return avail_aggr[intuniform(0, avail_aggr.size()-1, 0)];

    int64_t min_data_size(INT64_MAX);
    int dest_port(INT_MIN);//(avail_aggr[intuniform(0, avail_aggr.size()-1, 0)]);

    for(int port_ind:avail_aggr){
        if(queue_data_size[port_ind] <= min_data_size){
            min_data_size = queue_data_size[port_ind];
            dest_port = port_ind;
        }
    }

    return dest_port;
}

int Switch::geatRealQueueLength(){
    int len(0);
    for (cQueue::Iterator iter(*switch_buffer); !iter.end(); iter++) {
        Request* req = check_and_cast<Request*>(*iter);
        if(req->getByteLength())
            len++;
    }
    return len;
}

uint64_t Switch::getDataSizeInQueue(){
    uint64_t total_size(0);
    for (cQueue::Iterator iter(*switch_buffer); !iter.end(); iter++) {
        Request* req = check_and_cast<Request*>(*iter);
        total_size += req->getByteLength(); // Actual data size
    }
    return total_size;
}

}// namespace
