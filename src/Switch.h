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

#ifndef __FATTREENEW_TCX_H
#define __FATTREENEW_TCX_H

#include <omnetpp.h>
#include "General.h"

using namespace omnetpp;

namespace fattreenew {

class Switch : public cSimpleModule
{
  public:
    Switch();
    virtual ~Switch();
    bool checkPort(std::string);
//    bool queueIsFull;
    uint64_t getDataSizeInQueue();
  protected:
    std::map<int, int64_t> queue_data_size; // <port, data_size_in_queue>
    std::unordered_map<std::string, int> conn_map;
    cQueue* switch_buffer;
    simsignal_t qLenSignal;
    simsignal_t staySignal;
    double waitingSignal;
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual int randChoose(std::string);
    virtual int findCN(std::string, std::string);
    virtual int findAggr(std::string);
    int geatRealQueueLength();
};

}; // namespace

#endif
