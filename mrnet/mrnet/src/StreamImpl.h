#if !defined(__streamimpl_h)
#define __streamimpl_h 1

#include <list>
#include <vector>

#include <map>

#include "mrnet/h/MRNet.h"
#include "mrnet/src/Message.h"
#include "mrnet/src/CommunicatorImpl.h"
#include "mrnet/src/FrontEndNode.h"
#include "mrnet/src/BackEndNode.h"
#include "mrnet/src/NetworkImpl.h"

namespace MRN
{

class StreamImpl:public Stream {
    friend class Network;

 private:
    /* "Registered" streams */
    static std::map < unsigned int, StreamImpl * >*streams;
    static unsigned int cur_stream_idx;
    static bool force_network_recv;

    std::list < Packet >IncomingPacketBuffer;
    int ds_filter_id;
    int us_filter_id;
    int sync_id;
    unsigned short stream_id;
    CommunicatorImpl *communicator;

 public:
    StreamImpl( Communicator *, int sync_id, int ds_filter_id,
                int us_filter_id );
    StreamImpl( int stream_id, int *backends = 0, int num_backends =
                -1, int sync_id = SYNC_DONTWAIT, int ds_filter_id =
                AGGR_NULL, int us_filter_id = AGGR_NULL );
    virtual ~ StreamImpl(  );
    static int recv( int *tag, void **buf, Stream ** stream, bool blocking =
                     true );
    static StreamImpl *get_Stream( int stream_id );
    static void set_ForceNetworkRecv( bool force = true );

    virtual int send( int tag, const char *format_str, ... );
    virtual int flush(  );
    virtual int recv( int *tag, void **buf, bool blocking = true );
    virtual unsigned int get_NumEndPoints(  );
    void add_IncomingPacket( Packet& );
    const std::vector < EndPoint * >*get_EndPoints(  ) const;


    int send_aux( int tag, const char *format_str, va_list arg_list );
    static int unpack( void *buf, const char *fmt, va_list arg_list );

    virtual Communicator *get_Communicator( void );
    virtual unsigned int get_Id( void ) const ;
};

inline int StreamImpl::flush()
{
    if(Network::network){
        return Network::network->front_end->flush(stream_id);
    }

    return Network::back_end->flush();
}


inline unsigned int StreamImpl::get_NumEndPoints()
{
    return communicator->size();
}

inline void StreamImpl::set_ForceNetworkRecv( bool force )
{
    force_network_recv = force;
}

inline void StreamImpl::add_IncomingPacket(Packet& packet)
{
    IncomingPacketBuffer.push_back(packet);
}

inline const std::vector <EndPoint *> * StreamImpl::get_EndPoints() const
{
    return communicator->get_EndPoints();
}

inline int StreamImpl::unpack( void* buf, const char* fmt_str,
                               va_list arg_list )
{
    Packet * packet = (Packet *)buf;
    int ret = packet->ExtractVaList(fmt_str, arg_list); 
    delete packet;

    return ret;
}

inline Communicator * StreamImpl::get_Communicator( void )
{
    return communicator;
}

inline unsigned int StreamImpl::get_Id( void ) const 
{
    return stream_id;
}
} // namespace MRN

#endif /* __streamimpl_h */
