/***********************************************************************
 * Copyright � 2003-2004 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.     *
 **********************************************************************/

#if !defined(__childnode_h)
#define __childnode_h 1

#include <string>

#include "mrnet/src/Message.h"
#include "mrnet/src/RemoteNode.h"

namespace MRN
{

class ChildNode: public Error{
 protected:
    RemoteNode * upstream_node;

 private:
    std::string hostname;
    Port port;
    bool threaded;

 public:
    ChildNode(bool, std::string, Port);
    virtual ~ChildNode(void);
    virtual int proc_PacketsFromUpStream(std::list <Packet> &)=0;
    virtual int proc_DataFromUpStream(Packet&)=0;

    int recv_PacketsFromUpStream(std::list <Packet> &packet_list);
    int send_PacketUpStream(Packet& packet);
    int flush_PacketsUpStream();
    int send_Events( );
    bool has_data( );

    std::string get_HostName();
    Port get_Port();

    int getConnections( int** conns, unsigned int* nConns );
    virtual void error( ErrorCode, const char *, ... );
};

inline bool ChildNode::has_data( )
{
    return upstream_node->has_data();
}

inline int ChildNode::recv_PacketsFromUpStream(std::list <Packet>
                                               &packet_list)
{
  return upstream_node->recv(packet_list);
}

inline int ChildNode::send_PacketUpStream(Packet& packet)
{
  return upstream_node->send(packet);
}

inline int ChildNode::flush_PacketsUpStream()
{
  return upstream_node->flush();
}

inline std::string ChildNode::get_HostName()
{
  return hostname;
}

inline Port ChildNode::get_Port()
{
  return port;
}


} // namespace MRN

#endif /* __childnode_h */
