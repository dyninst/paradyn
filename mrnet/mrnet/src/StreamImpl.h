#if !defined(__streamimpl_h)
#define __streamimpl_h 1

#include <list>
#include <vector>

#include <map>

#include "mrnet/h/MR_Network.h"
#include "mrnet/src/Message.h"
#include "mrnet/src/CommunicatorImpl.h"

namespace MRN
{

class StreamImpl: public Stream {
  friend class Network;

 private:
  /* "Registered" streams */
  static std::map<unsigned int, StreamImpl*>* streams;
  static unsigned int cur_stream_idx, next_stream_id;
  static bool force_network_recv;

  std::list <Packet *> IncomingPacketBuffer;
  unsigned short filter_id;
  unsigned short sync_id;
  unsigned short stream_id;
  CommunicatorImpl * communicator;

 public:
  StreamImpl(Communicator *, int _filter_id = AGGR_NULL,
                int _sync_id = SYNC_DONTWAIT );
  StreamImpl(int stream_id, int * backends=0, int num_backends=-1,
                int filter_id=-AGGR_NULL, int sync_id=SYNC_DONTWAIT);
  virtual ~StreamImpl();
  static int recv(int *tag, void **buf, Stream ** stream, bool blocking=true);
  static StreamImpl * get_Stream(int stream_id);
  static void set_ForceNetworkRecv( bool force=true );

  virtual int send(int tag, const char * format_str, ...);
  virtual int flush();
  virtual int recv(int *tag, void **buf, bool blocking=true);
  virtual unsigned int get_NumEndPoints();
  void add_IncomingPacket(Packet *);
  const std::vector <EndPoint *> * get_EndPoints() const;


    int send_aux(int tag, const char * format_str, va_list arg_list ); 
    static int unpack(char* buf, const char* fmt, va_list arg_list );

    virtual Communicator* get_Communicator( void ) { return communicator; }
};

} // namespace MRN

#endif /* __streamimpl_h */
