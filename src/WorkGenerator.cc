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

#include "WorkGenerator.h"

namespace fattreenew {

Define_Module(WorkGenerator);

void WorkGenerator::initialize()
{
    if(par("sendInitialMessage").boolValue()){
        Request* req = new Request();
        scheduleAt(simTime(), req);
    }

    id = 1;
}

void WorkGenerator::initMsg(Request* req) {
    req->setMaster_id(id);
    req->setId(id);
    req->setData_size(par("data_size").doubleValue() * MB);
    req->setFrag_size(par("data_size").doubleValue() * MB);

    double read_prob(uniform(0, 1.0, par("rng").intValue()));
    if(read_prob < par("read_probability").doubleValue()){
        req->setWork_type('r');
    }else{
        req->setWork_type('w');
        req->setByteLength(req->getData_size());
    }

    req->setGenerate_time(simTime());
    req->setSrc_addr(getParentModule()->getFullName());
    std::string des(all_oss[intuniform(0, all_oss.size()-1, par("rng").intValue())]);
//    std::string des(all_cn[intuniform(0, all_cn.size()-1, par("rng").intValue())]);
    req->setDes_addr(des.c_str());

    cModule* cm = findModuleByPath(("Fattreenew." + des).c_str());
    int num_ost = cm->getSubmoduleVectorSize("ost");
    req->setTarget_ost(intuniform(0, num_ost-1, par("rng").intValue()));

    auto avail_paths = all_paths[req->getSrc_addr()][req->getDes_addr()];
    auto chosen_send_path = avail_paths[intuniform(0, avail_paths.size()-1, par("rng").intValue())];
    auto chosen_back_path = avail_paths[intuniform(0, avail_paths.size()-1, par("rng").intValue())];
    for(std::string s:chosen_send_path){
        std::string cur_p = req->getSendPath();
        req->setSendPath((cur_p + s + ",").c_str());
    }
    for(auto it=chosen_back_path.rbegin(); it!=chosen_back_path.rend(); it++){
        std::string cur_p = req->getBackPath();
        req->setBackPath((cur_p + *it + ",").c_str());
    }

    send(req, "port$o");
}

unsigned int WorkGenerator::fetchID() {
    return id;
}

void WorkGenerator::handleMessage(cMessage *msg)
{
    Request* req = check_and_cast<Request*>(msg);

    if(msg->isSelfMessage()){
        initMsg(req);
        simtime_t delay = par("sendInterval").doubleValue();
        Request* req = new Request();
        id++;
        scheduleAt(simTime()+delay, req);
    }else{
        cRuntimeError("Messages come into workload generator!\n");
    }
}

} //namespace
