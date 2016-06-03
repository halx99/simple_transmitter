#ifndef _XSOCKET_H_
#define _XSOCKET_H_
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#include <Mstcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t; 
typedef int socklen_t;
#else
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#define closesocket close
#define ioctlsocket ioctl
typedef int socket_t;
#undef socket
#endif
#include <errno.h>

#include <sstream>
#include "simpleppdef.h"

namespace simplepp_1_0_2012_9 {

namespace net {

#define _make_value(b1,b2,b3,b4) ( ( ((uint32_t)(b4) << 24) & 0xff000000 ) | ( ((uint32_t)(b3) << 16) & 0x00ff0000 ) | ( ((uint32_t)(b2) << 8) & 0x0000ff00 ) | ( (uint32_t)(b1) & 0x000000ff ) )

#pragma pack(push,1)
// ip packet
typedef struct _RS_IP_HDR
{
    // header size; 5+
    unsigned char  header_length:4; 

    // IP version: 0100/0x04(IPv4), 0110/0x05(IPv6)
    unsigned char  version:4; 

    // type of service: 
    union {
        unsigned char  value;
        struct {
            unsigned char priority:3;
            unsigned char D:1; // delay: 0(normal), 1(as little as possible)
            unsigned char T:1; // throughput: 0(normal), 1(as big as possible)
            unsigned char R:1; // reliability: 0(normal), 1(as big as possible)
            unsigned char C:1; // transmission cost: 0(normal), 1(as little as possible)
            unsigned char reserved:1; // always be zero
        } detail;
    } TOS; 

    // total size, header + data; MAX length is: 65535
    unsigned short total_length;

    // identifier: all split small packet set as the same value.
    unsigned short identifier;

    // flags and frag
    // unsigned short flags:3;
    // unsigned short frag:13;
    unsigned short flags_and_frag;

    // time of living, decreased by route, if zero, this packet will by dropped
    // avoid foward looply.
    unsigned char  TTL;

    // protocol
    // 1: ICMP
    // 2: IGMP
    // 6: TCP
    // 0x11/17: UDP
    // 0x58/88: IGRP
    // 0x59/89: OSPF
    unsigned char  protocol;// TCP / UDP / Other

    // check header of IP-PACKET 's correctness.
    unsigned short checksum;

    
    typedef union 
    {
        unsigned int value;
        struct {
            unsigned int   B1:8, B2:8, B3:8, B4:8;
        } detail;
    } dotted_decimal_t;

    // source ip address
    dotted_decimal_t src_ip;

    // destination ip address
    dotted_decimal_t dst_ip;
} RS_IP_HDR;

typedef struct _RS_PSD_HDR
{
    unsigned long  src_addr;
    unsigned long  dst_addr;
    char           mbz;
    char           protocol;
    unsigned short tcp_length;
} RS_PSD_HDR;

typedef struct _RS_TCP_HDR
{
    unsigned short src_port;
    unsigned short dst_port;
    unsigned int   seqno;
    unsigned int   ackno;
    unsigned char  header_length:4;
    unsigned char  reserved:4;
    unsigned char  flg_fin:1, flg_syn:1, flg_rst:1, flg_psh:1, flg_ack:1, flg_urg:1, flg_reserved:2;
    unsigned short win_length;
    unsigned short checksum;
    unsigned short urp; // emergency
} RS_TCP_HDR;

typedef struct _RS_UDP_HDR
{
    unsigned short src_port;
    unsigned short dst_port;
    unsigned short length;
    unsigned short checksum;
} RS_UDP_HDR;

typedef struct _RS_ICMP_HDR
{
    unsigned char  type; // 8bit type
    unsigned char  code; // 8bit code
    unsigned short checksum; // 16bit check sum
    unsigned short id; // identifier: usually use process id
    unsigned short seqno; // message sequence NO.
    unsigned int   timestamp; // timestamp
} RS_ICMP_HDR;

typedef struct _RS_ETH_HDR
{
    unsigned dst_eth[6];
    unsigned src_eth[6];
    unsigned eth_type;
} RS_ETH_HDR;

typedef struct _RS_ARP_HDR
{
    unsigned short arp_hw; // format of hardware address
    unsigned short arp_pro; // format of protocol address
    unsigned char  arp_hlen; // length of hardware address
    unsigned char  arp_plen; // length of protocol address
    unsigned short arp_op; // arp operation
    unsigned char  arp_oha[6]; // sender hardware address
    unsigned long  arp_opa; // sender protocol address
    unsigned char  arp_tha; // target hardware address
    unsigned long  arp_tpa; // target protocol address;
} RS_ARP_HDR;

typedef struct _RS_ARP_PACKET
{
    RS_ETH_HDR ethhdr;
    RS_ARP_HDR arphdr;
} RS_ARP_PACKET;
#pragma pack(pop)

static const socket_t bad_sock = (socket_t)-1;
static const u_long blocking = 0;
static const u_long nonblocking = 1;

union sockaddr_t
{
    sockaddr kernel;
    sockaddr_in operating;
};

inline
bool operator==(const sockaddr_t& lhs, const sockaddr_t& rhs)
{
    return ( lhs.operating.sin_addr.s_addr == rhs.operating.sin_addr.s_addr &&
        lhs.operating.sin_port == rhs.operating.sin_port );
}

inline
bool operator<(const sockaddr_t& lhs, const sockaddr_t& rhs)
{
    return (lhs.operating.sin_addr.s_addr < rhs.operating.sin_addr.s_addr) || 
        ( lhs.operating.sin_addr.s_addr == rhs.operating.sin_addr.s_addr && 
        lhs.operating.sin_port < rhs.operating.sin_port );
}

inline
bool operator >(const sockaddr_t& lhs, const sockaddr_t& rhs)
{
    return !(lhs < rhs) && !(lhs == rhs);
}

class xsocket
{
    // Disable object created in stack, and make it sealed
    ~xsocket(void); 

    // Not implemented, just disable object-copied  
    xsocket(const xsocket&);        

    // Not implemented, just disable object-assignment  
    xsocket& operator=(const xsocket&); 
    
public:

    // Construct a empty socket object
    xsocket(void); 

    // See also as function: open
    xsocket(int af, int type, int protocol);


    /** Release the socket object resource **/
    xsocket* release(void);


    /* @brief: Open new socket
    ** @params: 
    **        af      : Usually is [AF_INET] 
    **        type    : [SOCK_STREAM-->TCP] and [SOCK_DGRAM-->UDP]
    **        protocol: Usually is [0]
    ** @returns: false: check reason by errno
    */
    bool open(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);


    /** Close the socket **/
    void close(void);


    /** Is this socket opened **/
    bool is_open(void) const;


    /** Get the socket fd value **/
    socket_t get_handle(void) const;


    /* @brief: Set this socket io mode
    ** @params: 
    **        mode: [nonblocking] or [blocking]
    **
    ** @returns: [0] succeed, otherwise, a value of SOCKET_ERROR is returned.
    */
    int set_mode(u_long mode = nonblocking);


    /* @brief: Disables sends or receives on this socket
    ** @params: 
    **        how: [SD_SEND] or [SD_RECEIVE] or [SD_BOTH]
    **
    ** @returns: [0] succeed, otherwise, a value of SOCKET_ERROR is returned.
    */
    int shutdown(int how = SD_BOTH);


    /* @brief: Associates a local address with this socket
    ** @params: 
    **        Ommit
    **
    ** @returns: 
    **         If no error occurs, bind returns [0]. Otherwise, it returns SOCKET_ERROR
    */
    int bind(const char* addr, unsigned short port);


    /* @brief: Places this socket in a state in which it is listening for an incoming connection
    ** @params: 
    **        Ommit
    **
    ** @returns: 
    **         If no error occurs, bind returns [0]. Otherwise, it returns SOCKET_ERROR
    */
    int listen(int backlog = SOMAXCONN);


    /* @brief: Permits an incoming connection attempt on this socket
    ** @params: 
    **        addrlen: Usually is [sizeof (sockaddr)]
    **
    ** @returns: 
    **        If no error occurs, accept returns a new socket on which 
    **        the actual connection is made. 
    **        Otherwise, a value of [nullptr] is returned
    */
    xsocket* accept(socklen_t addrlen = sizeof(sockaddr));


    /* @brief: Permits an incoming connection attempt on this socket
    ** @params: 
    **        sec : seconds of waiting for new connection
    **        usec: Usually is [0]
    **
    ** @returns: 
    **        If no error occurs, accept returns a new socket on which 
    **        the actual connection is made. 
    **        Otherwise, a value of [nullptr] is returned
    */
    xsocket* accept_n(long sec, long usec = 0);


    /* @brief: Establishes a connection to a specified this socket
    ** @params: 
    **        addr: Usually is a IPV4 address
    **        port: Server Listenning Port
    ** 
    ** @returns: 
    **         If no error occurs, returns [0]. 
    **         Otherwise, it returns SOCKET_ERROR
    */
    int connect(const char* addr, u_short port);


    /* @brief: Establishes a connection to a specified this socket with nonblocking
    ** @params: omit
    **           
    ** @returns: [0].succeed, [-1].failed
    */
    int connect_n(const char* addr, u_short port, long sec, long usec = 0);


    /* @brief: Establishes a connection to a specified this socket with nonblocking
    ** @params: omit
    **           
    ** @returns: 
    **           [0]: succeed
    **    [ETIMEOUT]: connect timeout
    **          [-1]: failed
    */
    int connect_n_i(const char* addr, u_short port, long sec, long usec = 0);


    /* @brief: Sends data on this connected socket
    ** @params: omit
    **           
    ** @returns: 
    **         If no error occurs, send returns the total number of bytes sent, 
    **         which can be less than the number requested to be sent in the len parameter. 
    **         Otherwise, a value of SOCKET_ERROR is returned.
    */
    int send(const void* buf, int len, int flags = 0);


    /* @brief: Receives data from this connected socket or a bound connectionless socket. 
    ** @params: omit
    **           
    ** @returns: 
    **         If no error occurs, recv returns the number of bytes received and 
    **         the buffer pointed to by the buf parameter will contain this data received.
    **         If the connection has been gracefully closed, the return value is [0].
    */
    int recv(void* buf, int len, int flags = 0);


    /* @brief: Sends data on this connected socket
    ** @params:
    **       slicelen: slice length;
    **           
    ** @returns: 
    **         If no error occurs, send returns the total number of bytes sent, 
    **         which can be less than the number requested to be sent in the len parameter. 
    **         Otherwise, a value of SOCKET_ERROR is returned.
    */
    int send(void* buf, int len, int slicelen, int flags = 0);


    /* @brief: Receives data from this connected socket or a bound connectionless socket. 
    ** @params: slicelen: slice length
    **           
    ** @returns: 
    **         If no error occurs, recv returns the number of bytes received and 
    **         the buffer pointed to by the buf parameter will contain this data received.
    **         If the connection has been gracefully closed, the return value is [0].
    */
    int recv(void* buf, int len, int slicelen, int flags = 0);


    /* @brief: Sends data on this connected socket
    ** @params: omit
    **           
    ** @returns: 
    **         If no error occurs, send returns the total number of bytes sent, 
    **         which can be less than the number requested to be sent in the len parameter. 
    **         Otherwise, a value of SOCKET_ERROR is returned.
    */
    int send_i(const void* buf, int len, int flags = 0);


    /* @brief: Receives data from this connected socket or a bound connectionless socket. 
    ** @params: omit
    **           
    ** @returns: 
    **         If no error occurs, recv returns the number of bytes received and 
    **         the buffer pointed to by the buf parameter will contain this data received.
    **         If the connection has been gracefully closed, the return value is [0].
    */
    int recv_i(void* buf, int len, int flags = 0);


    /* @brief: Sends data on this connected socket
    ** @params: omit
    **           
    ** @returns: 
    **         If no error occurs, send returns the total number of bytes sent, 
    **         which can be less than the number requested to be sent in the len parameter. 
    **         Otherwise, a value of SOCKET_ERROR is returned.
    */
    int sendto_i(const void* buf, int len, int flags = 0);


    /* @brief: Receives a datagram and stores the source address
    ** @params: omit
    **           
    ** @returns: 
    **         If no error occurs, recv returns the number of bytes received and 
    **         the buffer pointed to by the buf parameter will contain this data received.
    **         If the connection has been gracefully closed, the return value is [0].
    */
    int recvfrom_i(void* buf, int len, sockaddr_t& peer_addr, int flags = 0);


    /* @brief: Get the the ipv4 address corresponding with this socket
    ** @params : None
    **   
    ** @returns: Return ipv4 string, format:127.0.0.1
    */
    std::string get_addr(void);


    /* @brief: Get the the port corresponding with this socket
    ** @params : None
    **   
    ** @returns: Omit
    */
    u_short get_port(void);


    /* @brief: Get the socket address fully
    ** @params : None
    **   
    ** @returns: ipv4 address and port, format: 127.0.0.1:10086
    */
    std::string get_addr_full(void);


    /* @brief: Configure TCP keepalive
    ** @params : omit
    **   
    ** @returns: [0].successfully
    **          [<0].one or more errors occured
    */
    int set_keepalive(int flag = 0, int idle = 7200, int interval = 75, int probes = 10);


    /* @brief: Sets the socket option
    ** @params : 
    **        level: The level at which the option is defined (for example, SOL_SOCKET).
    **      optname: The socket option for which the value is to be set (for example, SO_BROADCAST). 
    **               The optname parameter must be a socket option defined within the specified level, 
    **               or behavior is undefined. 
    **       optval: The option value.
    **
    ** @returns: If no error occurs, getsockopt returns zero. Otherwise, a value of SOCKET_ERROR is returned
    */
    template<typename T>
    int set_optval(int level, int optname, const T& optval); 


    /* @brief: Retrieves a socket option
    ** @params : 
    **     level: The level at which the option is defined. Example: SOL_SOCKET. 
    **   optname: The socket option for which the value is to be retrieved. 
    **            Example: SO_ACCEPTCONN. The optname value must be a socket option defined within the specified level, or behavior is undefined. 
    **    optval: A variable to the buffer in which the value for the requested option is to be returned.
    **    
    ** @returns: If no error occurs, getsockopt returns zero. Otherwise, a value of SOCKET_ERROR is returned
    */
    template<typename T>
    int get_optval(int level, int optname, T& optval, socklen_t = sizeof(T));

    int ioctl(long cmd, u_long* argp);

	/// <summary>
	/// this function just for windows platform
	/// </summary>
    static void init_ws32_lib(void);

private:
    socket_t   fd;
    sockaddr_t addr;
};

template<typename T> inline
    int xsocket::set_optval(int level, int optname, const T& optval)
{
    return ::setsockopt(this->fd, level, optname, (char*)&optval, sizeof(T));
}

template<typename T> inline
    int xsocket::get_optval(int level, int optname, T& optval, socklen_t optlen)
{
    return ::getsockopt(this->fd, level, optname, (char*)&optval, &optlen);
}


}; /* namespace: simplepp_1_0_2012_9::net */

}; /* namespace: simplepp_1_0_2012_9 */

/***************************************************************
*
* Usage:
*   (1)TCP
*      Server: 
*          int main(int, char**)
*          {
*              xsocket* ss = new xsocket(AF_INET, SOCK_STREAM, 0);
*
*              if(!ss->is_open()) 
*              {
*                  ss = ss->release();
*                  return -1;
*              }
*
*              if(ss->bind("127.0.0.1", 10086) != 0) 
*              {
*                  ss = ss->release();
*                  return -1;
*              }
*
*              if(ss->listen() != 0)
*              {
*                  ss = ss->release();
*                  return -1;
*              }
*              
*              xsocket* cs = ss->accept(); // start to waiting for connection from client.
*              cs = cs->release();
*              ss = ss->release();
*              return 0;
*          }
* 
*       Client:
*          int main(int, char**)
*          {
*              xsocket* cs = new xsocket(AF_INET, SOCK_STREAM, 0);
*
*              if(!cs->is_open()) 
*              {
*                  cs = cs->release();
*                  return -1;
*              }
*
*              if(cs.connect("127.0.0.1", 10086) != 0) // connect the server
*              {
*                  ;
*              }
*              cs->release();
*              return 0;
*          }     
*
****************************************************************/

#endif
/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/

