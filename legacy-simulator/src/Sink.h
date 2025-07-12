
#ifndef SINK_H_
#define SINK_H_

#include <omnetpp.h>
#include "General.h"

std::unordered_map<std::string, std::unordered_map<std::string, std::pair<std::string, int>>> system_layout;
std::vector<std::string> all_oss, all_cn;
std::vector<std::vector<std::string>> path_cn_cn, path_cn_oss;
std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::vector<std::string>>>> all_paths;

using namespace omnetpp;

namespace fattreenew {

/**
 * Implements the Txc simple module. See the NED file for more information.
 */
class Sink : public cSimpleModule
{
  public:
    Sink();
    virtual ~Sink();
  protected:
    uint64_t total_data_size;
    uint64_t total_read_size;
    uint64_t total_write_size;
    simsignal_t ThroughputSignal;
    simsignal_t rThroughputSignal;
    simsignal_t wThroughputSignal;
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
  private:
    void findPathCNtoCN(std::string, std::string, std::string, std::vector<std::string>&);
    void findPathCNtoOSS(std::string, std::string, std::string, std::vector<std::string>&);
    bool checkPath(const std::vector<std::string>&);
    void generateShortPaths(std::vector<std::vector<std::string>>&);
};

}; // namespace

#endif /* SINK_H_ */
