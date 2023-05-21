
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <omnetpp.h>
#include <vector>

using namespace omnetpp;

class Message : public cMessage
{
  public:
    Message();
    ~Message();
    std::vector<std::vector<std::string>> way;
};

#endif /* MESSAGE_H_ */
