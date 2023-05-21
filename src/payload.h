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

#ifndef __FATTREENEW_PAYLOAD_H_
#define __FATTREENEW_PAYLOAD_H_

#include <omnetpp.h>
#include "General.h"

using namespace omnetpp;

namespace fattreenew {

/**
 * TODO - Generated class
 */
class Payload : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  private:
    std::unordered_map<std::string, std::pair<std::string, int>> gate_to_neighbor;
    std::unordered_map<std::string, std::unordered_map<unsigned int, std::unordered_map<unsigned int, int64_t>>> work_arrive_status;
    void toModuleName(Request*, const std::string);
//    std::string popPath(Request*, char);

    // payload in OST network
    int getGateToExit();

    // in OSS, CN, to assemble data read from(or written to) OSTs
    void collectFromOSTs(Request*);
    const bool checkAllIdArrival(Request*); // check if all child processes arrive at the original CN
    void sendOstByStripe(Request*);

    // in fattree
    void segAndSend(Request*, int64_t, const int, const char*);
};

} //namespace

#endif
