#ifndef GENERAL_H_
#define GENERAL_H_

#include <string>
#include <omnetpp.h>
#include <map>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include "request_m.h"
#include <regex>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)
#define TB (1024*GB)

#define MTU 65520
#define STRIPE_SIZE (64*KB)
#define STRIPE_COUNT 3

using namespace omnetpp;
using namespace fattreenew;

int comp(cObject*, cObject*); // comparator function for cQueue

bool checkPortWithTransCable(cGate*);

bool compareStrVec(const std::vector<std::string>&, const std::vector<std::string>&);

std::string popPath(Request*, char);

simtime_t transTimestampByCable(cGate*);

extern std::unordered_map<std::string, std::unordered_map<std::string, std::pair<std::string, int>>> system_layout; // record each pair of modules with their gate name and index: <module1_name, <module2_name,<gate_name, gate_index>>>
extern std::vector<std::string> all_oss, all_cn;  // all OSSes and CNs
extern std::vector<std::vector<std::string>> path_cn_cn, path_cn_oss; // paths form CN1 to CN2; CN to OSSes
extern std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::vector<std::string>>>> all_paths;

#endif /* GENERAL_H_ */
