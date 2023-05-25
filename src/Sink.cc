#include "Sink.h"
//#include"WorkGenerator.h"

namespace fattreenew {

Define_Module(Sink);

Sink::Sink(){}

Sink::~Sink(){}

void Sink::initialize()
{
    total_data_size = 0;
    total_read_size = 0;
    total_write_size = 0;

    ThroughputSignal = registerSignal("throughput");
    rThroughputSignal = registerSignal("readThroughput");
    wThroughputSignal = registerSignal("writeThroughput");

    if(strcmp(getFullName(), "sink[0]") == 0){
        for (cModule::SubmoduleIterator it(getSystemModule()); !it.end(); it++) {
            cModule *submodule = *it;
            std::string module_name = submodule->getFullName();

            if(strcmp(submodule->getName(), "cn") == 0)
                all_cn.push_back(submodule->getFullName());
            if(strcmp(submodule->getName(), "oss") == 0)
                all_oss.push_back(submodule->getFullName());

            for(int i=0; submodule->hasGate("port$o")&&i<submodule->gateSize("port$o"); i++){
                cGate* g = submodule->gate("port$o", i);
                std::string neighbor_name = g->getNextGate()->getOwnerModule()->getFullName();
                system_layout[module_name][neighbor_name].first = "port$o";
                system_layout[module_name][neighbor_name].second = i;
            }

            if(submodule->hasGate("out")){
                cGate* g;
                std::string neighbor_name;
                if(submodule->hasGateVector("out")){
                    for(int i=0; i<submodule->gateSize("out"); i++){
                        g = submodule->gate("out", i);
                        neighbor_name = g->getNextGate()->getOwnerModule()->getFullName();
                        system_layout[module_name][neighbor_name].first = "out";
                        system_layout[module_name][neighbor_name].second = i;
                    }
                }else{
                    g = submodule->gate("out");
                    neighbor_name = g->getNextGate()->getOwnerModule()->getFullName();
                    system_layout[module_name][neighbor_name].first = "out";
                    system_layout[module_name][neighbor_name].second = -1;
                }
            }
        }

        //            for(auto mod:system_layout){
        //                EV << mod.first << " connected with: \n";
        //                for(auto conn_mod:mod.second){
        //                    EV << "    " << conn_mod.first << " at gate: " << conn_mod.second.first << " index: " << conn_mod.second.second << "\n";
        //                }
        //            }
        //
//                    for(auto a: all_cn) EV << a << " "; EV << "\n";
//                    for(auto a: all_oss) EV << a << " "; EV << "\n";
        std::vector<std::string> cn_cn, cn_oss;
        for(int i=0; i<all_cn.size(); i++){
            for(int j=i+1; j<all_cn.size(); j++){
                findPathCNtoCN(all_cn[i], all_cn[j], all_cn[i], cn_cn);
            }
            for(int j=0; j<all_oss.size(); j++){
                findPathCNtoOSS(all_cn[i], all_oss[j], all_cn[i], cn_oss);
            }
        }

//        std::sort(path_cn_cn.begin(), path_cn_cn.end(), compareStrVec);
//        std::sort(path_cn_oss.begin(), path_cn_oss.end(), compareStrVec);

        generateShortPaths(path_cn_cn);
        generateShortPaths(path_cn_oss);
    }

}

void Sink::handleMessage(cMessage *msg)
{
    Request* req = check_and_cast<Request*>(msg);

    if(req->getWork_type() == 'r') {
        total_read_size += req->getFrag_size();//req->getData_size();
        emit(rThroughputSignal, total_read_size / (1024.0 * 1024.0 * simTime().dbl()));
    }else{
        total_write_size += req->getFrag_size();//req->getData_size();
        emit(wThroughputSignal, total_write_size / (1024.0 * 1024.0 * simTime().dbl()));
    }

    delete req;
}

void Sink::finish(){
    //    int s(0);
    //    for(auto cn: all_cn){
    //        std::string root_path = getParentModule()->getName();
    //        cModule* m = getModuleByPath((root_path + "." + cn + ".work_gen").c_str());
    //        WorkGenerator* gen = check_and_cast<WorkGenerator*>(m);
    //        EV << gen->fetchID() << "\n";
    //        s += gen->fetchID();
    //    }
    //    EV << s << "\n";
}

void Sink::findPathCNtoCN(std::string cn_src, std::string cn_tar, std::string mid, std::vector<std::string>& path) {
    size_t index_pos = mid.find('[');
    std::string comp_name = mid.substr(0,index_pos);

    path.push_back(mid);
    auto path_size = path.size();
    if(mid == cn_tar){
        if(checkPath(path)){
            path_cn_cn.push_back(path);
            auto rev_path = path;
            std::reverse_copy(path.begin(), path.end(), rev_path.begin());
            path_cn_cn.push_back(rev_path);
        }
        path.pop_back();
        return;
    }

    if(path_size>15 ||
            strcmp(comp_name.c_str(), "sink") == 0 ||
            strcmp(comp_name.c_str(), "oss") == 0){
        path.pop_back();
        return;
    }

    if(strcmp(comp_name.c_str(), "cn") == 0){
         if(path_size > 1){ // if passed by line above "if(mid == cn_tar)" that means other CN involved!
             path.pop_back();
             return;
         }
     }else if(strcmp(comp_name.c_str(), "inif_edge_cn") == 0){
         if(path_size!=2 && path_size!=6 && path_size!=10 && path_size!=14){
             path.pop_back();
             return;
         }
     }else if(strcmp(comp_name.c_str(), "edge_connect") == 0){
         if(path_size!=3 && path_size!=5 && path_size!=9 && path_size!=13){
             path.pop_back();
             return;
         }
     }else if(strcmp(comp_name.c_str(), "edge") == 0){
         if(path_size!=4 && path_size!=8 && path_size!=12){
             path.pop_back();
             return;
         }
     }else if(strcmp(comp_name.c_str(), "inif_aggr_edge") == 0){
         if(path_size!=5 && path_size!=7 && path_size!=11){
             path.pop_back();
             return;
         }
     }else if(strcmp(comp_name.c_str(), "aggr") == 0){
         if(path_size!=6 && path_size!=10){
             path.pop_back();
             return;
         }
     }else if(strcmp(comp_name.c_str(), "inif_core_aggr") == 0){
         if(path_size!=7 && path_size!=9){
             path.pop_back();
             return;
         }
     }else if(strcmp(comp_name.c_str(), "core") == 0){
         if(path_size!=8){
             path.pop_back();
             return;
         }
     }else{
         cRuntimeError("Unknown module name!\n");
     }

    for(auto new_src:system_layout[mid]){
        findPathCNtoCN(cn_src, cn_tar, new_src.first, path);
    }

    path.pop_back();
}

void Sink::findPathCNtoOSS(std::string cn, std::string oss, std::string mid, std::vector<std::string>& path) {
    size_t index_pos = mid.find('[');
    std::string comp_name = mid.substr(0,index_pos);

    path.push_back(mid);
    auto path_size = path.size();
    if(mid == oss){
        if(checkPath(path)){
            path_cn_oss.push_back(path);
            auto rev_path = path;
            std::reverse_copy(path.begin(), path.end(), rev_path.begin());
            path_cn_oss.push_back(rev_path);
        }
        path.pop_back();
        return;
    }

    if(path_size>15 || strcmp(comp_name.c_str(), "sink")==0 || strcmp(comp_name.c_str(), "oss")==0){
        path.pop_back();
        return;
    }

    if(strcmp(comp_name.c_str(), "cn") == 0){
        if(path_size > 1){ // CN can only be put at start position
            path.pop_back();
            return;
        }
    }else if(strcmp(comp_name.c_str(), "inif_edge_cn") == 0){
        if(path_size!=2 && path_size!=6 && path_size!=10 && path_size!=14){
            path.pop_back();
            return;
        }
    }else if(strcmp(comp_name.c_str(), "edge_connect") == 0){
        if(path_size!=3 && path_size!=5 && path_size!=9 && path_size!=13){
            path.pop_back();
            return;
        }
    }else if(strcmp(comp_name.c_str(), "edge") == 0){
        if(path_size!=4 && path_size!=8 && path_size!=12){
            path.pop_back();
            return;
        }
    }else if(strcmp(comp_name.c_str(), "inif_aggr_edge") == 0){
        if(path_size!=5 && path_size!=7 && path_size!=11){
            path.pop_back();
            return;
        }
    }else if(strcmp(comp_name.c_str(), "aggr") == 0){
        if(path_size!=6 && path_size!=10){
            path.pop_back();
            return;
        }
    }else if(strcmp(comp_name.c_str(), "inif_core_aggr") == 0){
        if(path_size!=7 && path_size!=9){
            path.pop_back();
            return;
        }
    }else if(strcmp(comp_name.c_str(), "core") == 0){
        if(path_size!=8){
            path.pop_back();
            return;
        }
    }else{
        cRuntimeError("Unknown module name!\n");
    }

    for(auto new_src:system_layout[mid]){
        findPathCNtoOSS(cn, oss, new_src.first, path);
    }

    path.pop_back();
}

bool Sink::checkPath(const std::vector<std::string> &path) {
    if(path.size()<7 || path.size()>15 || path.size()%2==0) return false;
    return true;
   /* std::map<std::string, int> switch_count;
    for(auto a:path){
        auto sw_name = a.substr(0,5);
        if(sw_name=="edge[" || sw_name=="aggr[" || sw_name=="core["){
            switch_count[sw_name]++;
            if(switch_count[sw_name] > 2){
                return false;
            }
        }
    }

    return switch_count.size();*/
}

void Sink::generateShortPaths(std::vector<std::vector<std::string>>& avail_paths) {
    for(auto p:avail_paths){
        auto src(p.front()), des(p.back());
        std::vector<std::string> mid_way;

        for(int i=1; i<p.size()-1; i++)
            mid_way.push_back(p[i]);

        if(all_paths.count(src) && all_paths[src].count(des)){
            if(all_paths[src][des][0].size() > mid_way.size())
                all_paths[src][des].clear();
            else if(all_paths[src][des][0].size() < mid_way.size())
                continue;
        }

        all_paths[src][des].push_back(mid_way);
    }
}

}; // namespace




