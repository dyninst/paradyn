#if !defined(__parentnode_h)
#define __parentnode_h 1

#include <map>
#include <list>
#include <string>

#include "mrnet/src/Message.h"
#include "mrnet/src/StreamManager.h"
#include "mrnet/src/Filter.h"
#include "mrnet/src/RemoteNode.h"
#include "mrnet/src/pthread_sync.h"

namespace MRN
{

class ParentNode {
    friend class Aggregator;
    friend class Synchronizer;
    friend class RemoteNode;
 private:

    std::string hostname;
    unsigned short port;
    unsigned short config_port;
    int listening_sock_fd;

    bool threaded;

    bool isLeaf_;           // am I a leaf in the MRNet tree?
    std::vector < Packet * >childLeafInfoResponses;
    pthread_sync childLeafInfoResponsesLock;
    std::vector < Packet * >childConnectedLeafResponses;
    pthread_sync childConnectedLeafResponsesLock;

    int wait_for_SubTreeReports(  );

 protected:
    enum
        { MRN_ALLNODESREPORTED };
    std::list < RemoteNode * >children_nodes;
    std::list < int >backend_descendant_nodes;
    pthread_sync subtreereport_sync;
    unsigned int num_descendants, num_descendants_reported;

    std::map < unsigned int, RemoteNode * >ChildNodeByBackendId;
    pthread_sync childnodebybackendid_sync;

    std::map < unsigned int, StreamManager * >StreamManagerById;
    pthread_sync streammanagerbyid_sync;


    virtual int deliverLeafInfoResponse( Packet * pkt ) = NULL;
    virtual int deliverConnectLeavesResponse( Packet * pkt ) = NULL;

    bool isLeaf( void ) const
        {
            return isLeaf_;
        }

 public:
    ParentNode( bool _threaded, std::string, unsigned short );
    virtual ~ ParentNode( void );

    virtual int proc_PacketsFromDownStream( std::list < Packet * >& ) = 0;
    virtual int proc_DataFromDownStream( Packet * ) = 0;

    int recv_PacketsFromDownStream( std::list < Packet * >&packet_list,
                                    std::list < RemoteNode * >*rmt_nodes =
                                    NULL, bool blocking = true );
    int send_PacketDownStream( Packet * packet, bool internal_only =
                               false );
    int flush_PacketsDownStream( unsigned int stream_id );
    int flush_PacketsDownStream( void );

    int proc_newSubTree( Packet * );
    int proc_delSubTree( Packet * );

    int proc_newSubTreeReport( Packet * );

    StreamManager *proc_newStream( Packet * );
    int send_newStream( Packet *, StreamManager * );
    int proc_delStream( Packet * );
    int proc_newApplication( Packet * );
    int proc_delApplication( Packet * );

    int proc_newFilter( Packet * );

    int proc_getLeafInfo( Packet * );
    int proc_getLeafInfoResponse( Packet * );

    int proc_connectLeaves( Packet * );
    int proc_connectLeavesResponse( Packet * );

    std::string get_HostName(  );
    unsigned short get_Port(  );
    int getConnections( int **conns, unsigned int *nConns );
};

bool lt_RemoteNodePtr( RemoteNode * p1, RemoteNode * p2 );
bool equal_RemoteNodePtr( RemoteNode * p1, RemoteNode * p2 );

inline std::string ParentNode::get_HostName(  ) {
    return hostname;
}

inline unsigned short ParentNode::get_Port(  )
{
    return port;
}

}                               // namespace MRN

#endif                          /* __parentnode_h */
