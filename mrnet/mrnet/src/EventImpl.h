#if !defined(eventimpl_h)
#define eventimpl_h 1

#include <string>
#include "mrnet/h/MRNet.h"
#include "mrnet/src/utils.h"

namespace MRN
{

class EventImpl: public Event{
 private:
    EventType type;
    std::string hostname;
    unsigned short port;
    std::string description;
    static std::list<EventImpl> events;
    static std::list<EventImpl> remote_events;

 public:
    static bool EventImpl::have_Event(){
        return !( events.empty() );
    }
    static bool EventImpl::have_RemoteEvent(){
        return !( remote_events.empty() );
    }

    static void EventImpl::add_Event(EventImpl &event){
        events.push_back( event );
        remote_events.push_back( event );
    }

    static EventImpl EventImpl::get_NextEvent() {
        EventImpl ret = *( events.begin() );
        events.pop_front();
        return ret;
    }

    static EventImpl EventImpl::get_NextRemoteEvent() {
        EventImpl ret = *( remote_events.begin() );
        remote_events.pop_front();
        return ret;
    }

    static unsigned int EventImpl::get_NumRemoteEvents() {
        return remote_events.size();
    }

    static unsigned int EventImpl::get_NumEvents() {
        return events.size();
    }

    EventImpl( EventType t, std::string desc="", std::string h=LocalHostName,
           unsigned short p=LocalPort )
        :type(t), hostname(h), port(p), description(desc) {}

    virtual EventImpl::~EventImpl(){}

    virtual EventType get_Type( ){
        return type;
    }

    virtual const std::string & get_HostName( ){
        return hostname;
    }

    virtual unsigned short get_Port( ){
        return port;
    }

    virtual const std::string & get_Description( ){
        return description;
    }
};

} /* namespace MRN */
#endif /* eventimpl_h */
