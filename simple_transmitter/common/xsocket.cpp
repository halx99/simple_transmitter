#include "xsocket.h"

using namespace simplepp_1_0_2012_9::net;

xsocket::xsocket(void) : fd(bad_sock)
{
}

xsocket::xsocket(int af, int type, int protocol) : fd(bad_sock)
{
    open(af, type, protocol);
}

xsocket::~xsocket(void)
{
    this->fd = bad_sock;
}

bool xsocket::open(int af, int type, int protocol)
{
    if(bad_sock == this->fd) 
    {
        this->fd = ::socket(af, type, protocol);
    }
    return is_open();
}

void xsocket::close(void)
{
    if(this->fd != bad_sock)
    {
        ::closesocket(this->fd);
        this->fd = bad_sock;
    }
}

xsocket* xsocket::release(void)
{
    close();
    delete this;
    return nullptr;
}

bool xsocket::is_open(void) const
{
    return this->fd != bad_sock;
}

socket_t xsocket::get_handle(void) const
{
    return this->fd;
}

int xsocket::set_mode(u_long mode)
{
    return ::ioctlsocket(this->fd, FIONBIO, &mode);
}

int xsocket::shutdown(int how)
{
    return ::shutdown(this->fd, how);
}

int xsocket::bind(const char* addr, unsigned short port)
{
    this->addr.operating.sin_family = AF_INET;
    this->addr.operating.sin_addr.s_addr = addr ? inet_addr(addr) : 0;
    this->addr.operating.sin_port = htons(port);

    return ::bind(this->fd, &this->addr.kernel, sizeof(this->addr.kernel));
}

int xsocket::listen(int backlog)
{
    return ::listen(this->fd, backlog);
}

xsocket* xsocket::accept(socklen_t addrlen)
{
    xsocket* ns = new xsocket;

    ns->fd = ::accept(this->fd, &ns->addr.kernel, &addrlen);
    if(!ns->is_open()) {
        delete ns;
        ns = nullptr;
    }

    return ns;
}

xsocket* xsocket::accept_n(long sec, long usec)
{
    set_mode(nonblocking);

    fd_set fds_rd; 
    FD_ZERO(&fds_rd);
    FD_SET(this->fd, &fds_rd);

    timeval timeo;
    timeo.tv_sec = sec; 
    timeo.tv_usec = usec; 

    xsocket* ns = nullptr;
    if(::select((int)this->fd + 1, &fds_rd, 0, 0, &timeo) > 0 && FD_ISSET(this->fd, &fds_rd)) 
    {
        ns = this->accept();
    }

    set_mode(blocking);

    return ns;

    // epoll
    /*this->set_mode();
    int nfds, epfd = epoll_create(4096);

    epoll_event e;
    e.data.fd = this->h;
    e.events = EPOLLIN | EPOLLET;

    epoll_ctl(epfd, EPOLL_CTL_ADD, this->h, &e);


    int e_nr = epoll_wait(epfd, &e, 4096, sec * 1000);
    if(e_nr > 0) {
    this->accept(s);
    }

    close(epfd);
    this->set_mode(BLOCKING);
    return BAD_SOCK != s.h;*/

    // poll 
    //   this->set_mode();
    //   pollfd pfd;
    //   pfd.fd = this->h;
    //   pfd.events = POLLIN;

    //   int nr = poll(&pfd, this->h + 1, 5000);
    //   if(nr > 0 )
    //   {
    //this->accept(s);
    //   }
    //   this->set_mode(BLOCKING);

    //   return BAD_SOCK != s.h;

}

int xsocket::connect(const char* addr, u_short port)
{
    this->addr.operating.sin_family = AF_INET;
    this->addr.operating.sin_port = htons(port);
    this->addr.operating.sin_addr.s_addr = inet_addr(addr);

    return ::connect(this->fd,
        &this->addr.kernel, 
        sizeof(this->addr)
        );
}

int xsocket::connect_n(const char* addr, u_short port, long sec, long usec)
{
    this->set_mode(nonblocking);

    this->connect(addr, port);

    fd_set fds_wr; 
    FD_ZERO(&fds_wr);
    FD_SET(this->fd, &fds_wr);

    timeval timeo;
    timeo.tv_sec = sec; 
    timeo.tv_usec = usec;

    if(::select((int)this->fd + 1, nullptr, &fds_wr, nullptr, &timeo) > 0)
    { // connect successfully
        set_mode(blocking);
        return 0;
    }
    else
    { // connect timeout or socket error
        close();
        return -1;
    }
}

int xsocket::connect_n_i(const char* addr, u_short port, long sec, long usec)
{
#ifdef __cxx11
    int result = -1;

    set_mode(nonblocking);

    connect(addr, port);

    timeval timeo;
    timeo.tv_sec = sec;
    timeo.tv_usec = usec;

    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(this->fd, &wfds);

    if( (result = ::select(this->fd + 1, nullptr, &wfds, nullptr, (sec || usec) ? &timeo : nullptr)) > 0 )
    { // the socket h is already writeable
        this->get_optval(SOL_SOCKET, SO_ERROR, result);
    }
    else if(0 == result)
    { // connect timeout, result = ETIMEDOUT, but not 0
        result = ETIMEDOUT;
    }
    else
    { // connect error. result < 0
        ;
    }

    // done
    if(0 == result)
    { // connect successfully, resume the io mode
        set_mode(blocking);
    }
    else
    { // connect failed, close the socket fd
        close();
    }
    return result;
#else
    return this->connect_n(addr, port, sec, usec);
#endif
}

int xsocket::send(const void* buf, int len, int flags)
{
    int bytes_transferred = 0;
    int n = 0;
    do 
    {
        bytes_transferred += 
            ( n = ::send(this->fd,
            (char*)buf + bytes_transferred,
            len - bytes_transferred,
            flags
            ) );
    } while(bytes_transferred < len && n > 0);
    return bytes_transferred;
}

int xsocket::recv(void* buf, int len, int flags)
{
    int bytes_transfrred = 0;
    int n = 0;
    do
    {
        bytes_transfrred += 
            ( n = ::recv(this->fd,
            (char*)buf + bytes_transfrred,
            len - bytes_transfrred,
            flags
            ) );

    } while(bytes_transfrred < len && n > 0);
    return bytes_transfrred;
}

int xsocket::send(void* buf, int len, int slicelen, int flags)
{
    char* ptr = (char*)buf;
    int bytes_leavings = len;
    while(bytes_leavings > 0)
    {
        if(bytes_leavings > slicelen)
        {
            bytes_leavings -= this->send(ptr, slicelen);
            ptr += slicelen;
        }
        else
        {
            bytes_leavings -= this->send(ptr, bytes_leavings);
        }
    }
    return len - bytes_leavings;
}

int xsocket::recv(void* buf, int len, int slicelen, int flags)
{
    char* ptr = (char*)buf;
    int bytes_leavings = len;
    while(bytes_leavings > 0)
    {
        if(bytes_leavings > slicelen)
        {
            bytes_leavings -= this->recv(ptr, slicelen);
            ptr += slicelen;
        }
        else
        {
            bytes_leavings -= this->recv(ptr, bytes_leavings);
        }
    }
    return len - bytes_leavings;
}

int xsocket::send_i(const void* buf, int len, int flags)
{
    return ::send(
        this->fd,
        (const char*)buf,
        len,
        flags);
}

int xsocket::recv_i(void* buf, int len, int flags)
{
    return ::recv(
        this->fd,
        (char*)buf,
        len,
        flags);
}

int xsocket::recvfrom_i(void* buf, int len, sockaddr_t& peer_addr, int flags)
{
    socklen_t addrlen = sizeof(sockaddr_t);
    return ::recvfrom(this->fd,
        (char*)buf,
        len,
        flags,
        &peer_addr.kernel,
        &addrlen
        );
}

int xsocket::sendto_i(const void* buf, int len, int flags)
{
    return ::sendto(this->fd,
        (const char*)buf,
        len,
        flags,
        &this->addr.kernel,
        sizeof(sockaddr_t)
        );
}

std::string xsocket::get_addr(void)
{
    return std::string(inet_ntoa(this->addr.operating.sin_addr));
}

u_short xsocket::get_port(void)
{
    return ntohs(this->addr.operating.sin_port);
}

std::string xsocket::get_addr_full(void)
{
    std::stringstream ss;
    ss << get_addr() << ":" << get_port();
    return ss.str();
}

int xsocket::set_keepalive(int flag, int idle, int interval, int probes)
{
#ifdef _WIN32
    tcp_keepalive buffer_in;
    buffer_in.onoff = flag;
    buffer_in.keepalivetime = idle;
    buffer_in.keepaliveinterval = interval;
    
    return WSAIoctl(this->fd,
        SIO_KEEPALIVE_VALS,
        &buffer_in,
        sizeof(buffer_in),
        nullptr,
        0,
        (DWORD*)&probes,
        nullptr,
        nullptr);
#else
    int errcnt = 0;
    errcnt += this->set_optval(SOL_SOCKET, SO_KEEPALIVE, flag);
    errcnt += this->set_optval(SOL_TCP, TCP_KEEPIDLE, idle);
    errcnt += this->set_optval(SOL_TCP, TCP_KEEPINTVL, interval);
    errcnt += this->set_optval(SOL_TCP, TCP_KEEPCNT, probes);
    return errcnt;
#endif
}

int xsocket::ioctl(long cmd, u_long* argp)
{
    return ::ioctlsocket(this->fd, cmd, argp);
}

void _naked_mark xsocket::init_ws32_lib(void)
{
#if defined(_WIN32) && !defined(_WIN64)
    _asm ret;
#else
    return;
#endif
}

// initialize win32 socket library
#ifdef _WIN32
namespace {

    struct ws2_32_gc
    {
        ws2_32_gc(void)
        {
            WSADATA dat;
            WSAStartup(0x0202, &dat);
        }
        ~ws2_32_gc(void)
        {
            WSACleanup();
        }
    };

    ws2_32_gc __ws32_lib_gc;
};
#endif

/* select usage:
char dat;
fd_set fds_rd;
FD_ZERO(&fds_rd);
FD_SET(fd, &fds_rd);
timeval timeo;
timeo.sec = 5;
timeo.usec = 500000;
switch( ::select(fd + 1, &fds_rd, nullptr, nullptr, &timeo) )
{
case -1:  // select error
    break;
case 0:   // select timeout
    break;
default:  // can read
    if(sock.recv_i(&dat, sizeof(dat), MSG_PEEK) < 0)
    {
        return -1;
    }
    ;
}
*/

