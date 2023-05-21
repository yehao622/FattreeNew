#include "General.h"

int comp(cObject* r1, cObject* r2){
    Request* t1 = check_and_cast<Request*>(r1);
    Request* t2 = check_and_cast<Request*>(r2);
    int ans = t1->getArriveModule_time() > t2->getArriveModule_time()?1:0; // FIFO order
    return ans;
}

bool checkPortWithTransCable(cGate* g) {
    auto channel = g->getChannel();
    return channel && channel->isTransmissionChannel();
}

simtime_t transTimestampByCable(cGate* g) {
    simtime_t trans_time = simTime();
    if(checkPortWithTransCable(g))
        trans_time = std::max(trans_time, g->getTransmissionChannel()->getTransmissionFinishTime());

    return trans_time;
}

bool compareStrVec(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    if(a.size() < b.size())
        return true;
    else if(a.size() == b.size())
        return a[0]<b[0] || a.back()<b.back();
    return false;
}

std::string popPath(Request* req, char direction) {
    if(direction != 's' && direction != 'b')
        cRuntimeError("Unknown sent/back direction of a message!\n");

    std::string cur_path, new_path;
    size_t pos_comma;

    if(direction == 's'){
        cur_path = req->getSendPath();
    }else if(direction == 'b'){
        cur_path = req->getBackPath();
    }
    pos_comma = cur_path.find(',', 0);

    if(pos_comma != std::string::npos)
        new_path = cur_path.substr(pos_comma+1, std::string::npos);

    direction=='s' ? req->setSendPath(new_path.c_str()) : req->setBackPath(new_path.c_str());

    return cur_path.substr(0, pos_comma);
}
