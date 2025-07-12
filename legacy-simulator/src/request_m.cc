//
// Generated file, do not edit! Created by opp_msgtool 6.0 from request.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "request_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace fattreenew {

Register_Class(Request)

Request::Request(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Request::Request(const Request& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Request::~Request()
{
}

Request& Request::operator=(const Request& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Request::copy(const Request& other)
{
    this->work_type = other.work_type;
    this->finished = other.finished;
    this->ckp_launched = other.ckp_launched;
    this->port_index = other.port_index;
    this->target_ost = other.target_ost;
    this->id = other.id;
    this->master_id = other.master_id;
    this->num_proc = other.num_proc;
    this->frag_size = other.frag_size;
    this->data_size = other.data_size;
    this->proc_time = other.proc_time;
    this->src_addr = other.src_addr;
    this->des_addr = other.des_addr;
    this->master_id_addr = other.master_id_addr;
    this->next_hop_addr = other.next_hop_addr;
    this->sendPath = other.sendPath;
    this->backPath = other.backPath;
    this->generate_time = other.generate_time;
    this->arriveModule_time = other.arriveModule_time;
    this->leaveModule_time = other.leaveModule_time;
}

void Request::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->work_type);
    doParsimPacking(b,this->finished);
    doParsimPacking(b,this->ckp_launched);
    doParsimPacking(b,this->port_index);
    doParsimPacking(b,this->target_ost);
    doParsimPacking(b,this->id);
    doParsimPacking(b,this->master_id);
    doParsimPacking(b,this->num_proc);
    doParsimPacking(b,this->frag_size);
    doParsimPacking(b,this->data_size);
    doParsimPacking(b,this->proc_time);
    doParsimPacking(b,this->src_addr);
    doParsimPacking(b,this->des_addr);
    doParsimPacking(b,this->master_id_addr);
    doParsimPacking(b,this->next_hop_addr);
    doParsimPacking(b,this->sendPath);
    doParsimPacking(b,this->backPath);
    doParsimPacking(b,this->generate_time);
    doParsimPacking(b,this->arriveModule_time);
    doParsimPacking(b,this->leaveModule_time);
}

void Request::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->work_type);
    doParsimUnpacking(b,this->finished);
    doParsimUnpacking(b,this->ckp_launched);
    doParsimUnpacking(b,this->port_index);
    doParsimUnpacking(b,this->target_ost);
    doParsimUnpacking(b,this->id);
    doParsimUnpacking(b,this->master_id);
    doParsimUnpacking(b,this->num_proc);
    doParsimUnpacking(b,this->frag_size);
    doParsimUnpacking(b,this->data_size);
    doParsimUnpacking(b,this->proc_time);
    doParsimUnpacking(b,this->src_addr);
    doParsimUnpacking(b,this->des_addr);
    doParsimUnpacking(b,this->master_id_addr);
    doParsimUnpacking(b,this->next_hop_addr);
    doParsimUnpacking(b,this->sendPath);
    doParsimUnpacking(b,this->backPath);
    doParsimUnpacking(b,this->generate_time);
    doParsimUnpacking(b,this->arriveModule_time);
    doParsimUnpacking(b,this->leaveModule_time);
}

char Request::getWork_type() const
{
    return this->work_type;
}

void Request::setWork_type(char work_type)
{
    this->work_type = work_type;
}

bool Request::getFinished() const
{
    return this->finished;
}

void Request::setFinished(bool finished)
{
    this->finished = finished;
}

bool Request::getCkp_launched() const
{
    return this->ckp_launched;
}

void Request::setCkp_launched(bool ckp_launched)
{
    this->ckp_launched = ckp_launched;
}

short Request::getPort_index() const
{
    return this->port_index;
}

void Request::setPort_index(short port_index)
{
    this->port_index = port_index;
}

short Request::getTarget_ost() const
{
    return this->target_ost;
}

void Request::setTarget_ost(short target_ost)
{
    this->target_ost = target_ost;
}

uint32_t Request::getId() const
{
    return this->id;
}

void Request::setId(uint32_t id)
{
    this->id = id;
}

uint32_t Request::getMaster_id() const
{
    return this->master_id;
}

void Request::setMaster_id(uint32_t master_id)
{
    this->master_id = master_id;
}

uint32_t Request::getNum_proc() const
{
    return this->num_proc;
}

void Request::setNum_proc(uint32_t num_proc)
{
    this->num_proc = num_proc;
}

uint32_t Request::getFrag_size() const
{
    return this->frag_size;
}

void Request::setFrag_size(uint32_t frag_size)
{
    this->frag_size = frag_size;
}

uint64_t Request::getData_size() const
{
    return this->data_size;
}

void Request::setData_size(uint64_t data_size)
{
    this->data_size = data_size;
}

double Request::getProc_time() const
{
    return this->proc_time;
}

void Request::setProc_time(double proc_time)
{
    this->proc_time = proc_time;
}

const char * Request::getSrc_addr() const
{
    return this->src_addr.c_str();
}

void Request::setSrc_addr(const char * src_addr)
{
    this->src_addr = src_addr;
}

const char * Request::getDes_addr() const
{
    return this->des_addr.c_str();
}

void Request::setDes_addr(const char * des_addr)
{
    this->des_addr = des_addr;
}

const char * Request::getMaster_id_addr() const
{
    return this->master_id_addr.c_str();
}

void Request::setMaster_id_addr(const char * master_id_addr)
{
    this->master_id_addr = master_id_addr;
}

const char * Request::getNext_hop_addr() const
{
    return this->next_hop_addr.c_str();
}

void Request::setNext_hop_addr(const char * next_hop_addr)
{
    this->next_hop_addr = next_hop_addr;
}

const char * Request::getSendPath() const
{
    return this->sendPath.c_str();
}

void Request::setSendPath(const char * sendPath)
{
    this->sendPath = sendPath;
}

const char * Request::getBackPath() const
{
    return this->backPath.c_str();
}

void Request::setBackPath(const char * backPath)
{
    this->backPath = backPath;
}

::omnetpp::simtime_t Request::getGenerate_time() const
{
    return this->generate_time;
}

void Request::setGenerate_time(::omnetpp::simtime_t generate_time)
{
    this->generate_time = generate_time;
}

::omnetpp::simtime_t Request::getArriveModule_time() const
{
    return this->arriveModule_time;
}

void Request::setArriveModule_time(::omnetpp::simtime_t arriveModule_time)
{
    this->arriveModule_time = arriveModule_time;
}

::omnetpp::simtime_t Request::getLeaveModule_time() const
{
    return this->leaveModule_time;
}

void Request::setLeaveModule_time(::omnetpp::simtime_t leaveModule_time)
{
    this->leaveModule_time = leaveModule_time;
}

class RequestDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_work_type,
        FIELD_finished,
        FIELD_ckp_launched,
        FIELD_port_index,
        FIELD_target_ost,
        FIELD_id,
        FIELD_master_id,
        FIELD_num_proc,
        FIELD_frag_size,
        FIELD_data_size,
        FIELD_proc_time,
        FIELD_src_addr,
        FIELD_des_addr,
        FIELD_master_id_addr,
        FIELD_next_hop_addr,
        FIELD_sendPath,
        FIELD_backPath,
        FIELD_generate_time,
        FIELD_arriveModule_time,
        FIELD_leaveModule_time,
    };
  public:
    RequestDescriptor();
    virtual ~RequestDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(RequestDescriptor)

RequestDescriptor::RequestDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(fattreenew::Request)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

RequestDescriptor::~RequestDescriptor()
{
    delete[] propertyNames;
}

bool RequestDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Request *>(obj)!=nullptr;
}

const char **RequestDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *RequestDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int RequestDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 20+base->getFieldCount() : 20;
}

unsigned int RequestDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_work_type
        FD_ISEDITABLE,    // FIELD_finished
        FD_ISEDITABLE,    // FIELD_ckp_launched
        FD_ISEDITABLE,    // FIELD_port_index
        FD_ISEDITABLE,    // FIELD_target_ost
        FD_ISEDITABLE,    // FIELD_id
        FD_ISEDITABLE,    // FIELD_master_id
        FD_ISEDITABLE,    // FIELD_num_proc
        FD_ISEDITABLE,    // FIELD_frag_size
        FD_ISEDITABLE,    // FIELD_data_size
        FD_ISEDITABLE,    // FIELD_proc_time
        FD_ISEDITABLE,    // FIELD_src_addr
        FD_ISEDITABLE,    // FIELD_des_addr
        FD_ISEDITABLE,    // FIELD_master_id_addr
        FD_ISEDITABLE,    // FIELD_next_hop_addr
        FD_ISEDITABLE,    // FIELD_sendPath
        FD_ISEDITABLE,    // FIELD_backPath
        FD_ISEDITABLE,    // FIELD_generate_time
        FD_ISEDITABLE,    // FIELD_arriveModule_time
        FD_ISEDITABLE,    // FIELD_leaveModule_time
    };
    return (field >= 0 && field < 20) ? fieldTypeFlags[field] : 0;
}

const char *RequestDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "work_type",
        "finished",
        "ckp_launched",
        "port_index",
        "target_ost",
        "id",
        "master_id",
        "num_proc",
        "frag_size",
        "data_size",
        "proc_time",
        "src_addr",
        "des_addr",
        "master_id_addr",
        "next_hop_addr",
        "sendPath",
        "backPath",
        "generate_time",
        "arriveModule_time",
        "leaveModule_time",
    };
    return (field >= 0 && field < 20) ? fieldNames[field] : nullptr;
}

int RequestDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "work_type") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "finished") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "ckp_launched") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "port_index") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "target_ost") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "id") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "master_id") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "num_proc") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "frag_size") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "data_size") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "proc_time") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "src_addr") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "des_addr") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "master_id_addr") == 0) return baseIndex + 13;
    if (strcmp(fieldName, "next_hop_addr") == 0) return baseIndex + 14;
    if (strcmp(fieldName, "sendPath") == 0) return baseIndex + 15;
    if (strcmp(fieldName, "backPath") == 0) return baseIndex + 16;
    if (strcmp(fieldName, "generate_time") == 0) return baseIndex + 17;
    if (strcmp(fieldName, "arriveModule_time") == 0) return baseIndex + 18;
    if (strcmp(fieldName, "leaveModule_time") == 0) return baseIndex + 19;
    return base ? base->findField(fieldName) : -1;
}

const char *RequestDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "char",    // FIELD_work_type
        "bool",    // FIELD_finished
        "bool",    // FIELD_ckp_launched
        "short",    // FIELD_port_index
        "short",    // FIELD_target_ost
        "uint32_t",    // FIELD_id
        "uint32_t",    // FIELD_master_id
        "uint32_t",    // FIELD_num_proc
        "uint32_t",    // FIELD_frag_size
        "uint64_t",    // FIELD_data_size
        "double",    // FIELD_proc_time
        "string",    // FIELD_src_addr
        "string",    // FIELD_des_addr
        "string",    // FIELD_master_id_addr
        "string",    // FIELD_next_hop_addr
        "string",    // FIELD_sendPath
        "string",    // FIELD_backPath
        "omnetpp::simtime_t",    // FIELD_generate_time
        "omnetpp::simtime_t",    // FIELD_arriveModule_time
        "omnetpp::simtime_t",    // FIELD_leaveModule_time
    };
    return (field >= 0 && field < 20) ? fieldTypeStrings[field] : nullptr;
}

const char **RequestDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *RequestDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int RequestDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void RequestDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Request'", field);
    }
}

const char *RequestDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RequestDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        case FIELD_work_type: return long2string(pp->getWork_type());
        case FIELD_finished: return bool2string(pp->getFinished());
        case FIELD_ckp_launched: return bool2string(pp->getCkp_launched());
        case FIELD_port_index: return long2string(pp->getPort_index());
        case FIELD_target_ost: return long2string(pp->getTarget_ost());
        case FIELD_id: return ulong2string(pp->getId());
        case FIELD_master_id: return ulong2string(pp->getMaster_id());
        case FIELD_num_proc: return ulong2string(pp->getNum_proc());
        case FIELD_frag_size: return ulong2string(pp->getFrag_size());
        case FIELD_data_size: return uint642string(pp->getData_size());
        case FIELD_proc_time: return double2string(pp->getProc_time());
        case FIELD_src_addr: return oppstring2string(pp->getSrc_addr());
        case FIELD_des_addr: return oppstring2string(pp->getDes_addr());
        case FIELD_master_id_addr: return oppstring2string(pp->getMaster_id_addr());
        case FIELD_next_hop_addr: return oppstring2string(pp->getNext_hop_addr());
        case FIELD_sendPath: return oppstring2string(pp->getSendPath());
        case FIELD_backPath: return oppstring2string(pp->getBackPath());
        case FIELD_generate_time: return simtime2string(pp->getGenerate_time());
        case FIELD_arriveModule_time: return simtime2string(pp->getArriveModule_time());
        case FIELD_leaveModule_time: return simtime2string(pp->getLeaveModule_time());
        default: return "";
    }
}

void RequestDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        case FIELD_work_type: pp->setWork_type(string2long(value)); break;
        case FIELD_finished: pp->setFinished(string2bool(value)); break;
        case FIELD_ckp_launched: pp->setCkp_launched(string2bool(value)); break;
        case FIELD_port_index: pp->setPort_index(string2long(value)); break;
        case FIELD_target_ost: pp->setTarget_ost(string2long(value)); break;
        case FIELD_id: pp->setId(string2ulong(value)); break;
        case FIELD_master_id: pp->setMaster_id(string2ulong(value)); break;
        case FIELD_num_proc: pp->setNum_proc(string2ulong(value)); break;
        case FIELD_frag_size: pp->setFrag_size(string2ulong(value)); break;
        case FIELD_data_size: pp->setData_size(string2uint64(value)); break;
        case FIELD_proc_time: pp->setProc_time(string2double(value)); break;
        case FIELD_src_addr: pp->setSrc_addr((value)); break;
        case FIELD_des_addr: pp->setDes_addr((value)); break;
        case FIELD_master_id_addr: pp->setMaster_id_addr((value)); break;
        case FIELD_next_hop_addr: pp->setNext_hop_addr((value)); break;
        case FIELD_sendPath: pp->setSendPath((value)); break;
        case FIELD_backPath: pp->setBackPath((value)); break;
        case FIELD_generate_time: pp->setGenerate_time(string2simtime(value)); break;
        case FIELD_arriveModule_time: pp->setArriveModule_time(string2simtime(value)); break;
        case FIELD_leaveModule_time: pp->setLeaveModule_time(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Request'", field);
    }
}

omnetpp::cValue RequestDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        case FIELD_work_type: return pp->getWork_type();
        case FIELD_finished: return pp->getFinished();
        case FIELD_ckp_launched: return pp->getCkp_launched();
        case FIELD_port_index: return pp->getPort_index();
        case FIELD_target_ost: return pp->getTarget_ost();
        case FIELD_id: return (omnetpp::intval_t)(pp->getId());
        case FIELD_master_id: return (omnetpp::intval_t)(pp->getMaster_id());
        case FIELD_num_proc: return (omnetpp::intval_t)(pp->getNum_proc());
        case FIELD_frag_size: return (omnetpp::intval_t)(pp->getFrag_size());
        case FIELD_data_size: return (omnetpp::intval_t)(pp->getData_size());
        case FIELD_proc_time: return pp->getProc_time();
        case FIELD_src_addr: return pp->getSrc_addr();
        case FIELD_des_addr: return pp->getDes_addr();
        case FIELD_master_id_addr: return pp->getMaster_id_addr();
        case FIELD_next_hop_addr: return pp->getNext_hop_addr();
        case FIELD_sendPath: return pp->getSendPath();
        case FIELD_backPath: return pp->getBackPath();
        case FIELD_generate_time: return pp->getGenerate_time().dbl();
        case FIELD_arriveModule_time: return pp->getArriveModule_time().dbl();
        case FIELD_leaveModule_time: return pp->getLeaveModule_time().dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Request' as cValue -- field index out of range?", field);
    }
}

void RequestDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        case FIELD_work_type: pp->setWork_type(omnetpp::checked_int_cast<char>(value.intValue())); break;
        case FIELD_finished: pp->setFinished(value.boolValue()); break;
        case FIELD_ckp_launched: pp->setCkp_launched(value.boolValue()); break;
        case FIELD_port_index: pp->setPort_index(omnetpp::checked_int_cast<short>(value.intValue())); break;
        case FIELD_target_ost: pp->setTarget_ost(omnetpp::checked_int_cast<short>(value.intValue())); break;
        case FIELD_id: pp->setId(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_master_id: pp->setMaster_id(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_num_proc: pp->setNum_proc(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_frag_size: pp->setFrag_size(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_data_size: pp->setData_size(omnetpp::checked_int_cast<uint64_t>(value.intValue())); break;
        case FIELD_proc_time: pp->setProc_time(value.doubleValue()); break;
        case FIELD_src_addr: pp->setSrc_addr(value.stringValue()); break;
        case FIELD_des_addr: pp->setDes_addr(value.stringValue()); break;
        case FIELD_master_id_addr: pp->setMaster_id_addr(value.stringValue()); break;
        case FIELD_next_hop_addr: pp->setNext_hop_addr(value.stringValue()); break;
        case FIELD_sendPath: pp->setSendPath(value.stringValue()); break;
        case FIELD_backPath: pp->setBackPath(value.stringValue()); break;
        case FIELD_generate_time: pp->setGenerate_time(value.doubleValue()); break;
        case FIELD_arriveModule_time: pp->setArriveModule_time(value.doubleValue()); break;
        case FIELD_leaveModule_time: pp->setLeaveModule_time(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Request'", field);
    }
}

const char *RequestDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr RequestDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void RequestDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Request *pp = omnetpp::fromAnyPtr<Request>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Request'", field);
    }
}

}  // namespace fattreenew

namespace omnetpp {

}  // namespace omnetpp

