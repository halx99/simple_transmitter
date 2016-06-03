// transmission_def.h
#ifndef _TRANSMISSION_DEF_
#define _TRANSMISSION_DEF_
#ifndef _TRANSMISSION_VERSION_
#define _TRANSMISSION_VERSION_ "1.0.1"
#endif
#include <iostream>
#include "xxsocket.h"
#include "xxbswap.h"
#include "politedef.h"
using namespace purelib::net;

/// error strings
#define ERRS_SERVER_BIND_FAIELD "Transmission Server: bind failed"
#define ERRS_SERVER_LISTEN_FAILED "Transmission Server: listen failed"
#define ERRS_CLIENT_CONNECT_FAILED "Transmission Client: connect to specific server failed" 

/// status machie
// USE phony SIZE filed(4Bytes or 8Bytes later) AS BYTE STREAM 's HEADER.
// server:
//     UNDEFINED 
//         |
//         |  <------ socket create successfully
//         V
//     ACCEPTING <----------------------------------------------+
//         |                                                    |
//         |  <------  new client connection arrived            |
//         V                                                    |
//    ESTABLISHED                                               |
//         |                                                    |
//         |  <------  transmission_request from cleint         |
//         |  -------> transmission_response to client          |
//         V                                                    |
//    TRASMITTING                                               |
//         |                                                    |
//         |  <------  sending_complete from client             |
//         |  -------> receiving_complete to client             |
//         V                                                    |
//  TRANSMITT_COMPLETE                                          |
//         |                                                    |
//         |                                                    |
//         -----------------------------------------------------+
//
// client:
//     UNDEFINED 
//         |
//         | <------ socket create successfully
//         V
//    CONNECTING 
//         |
//         | <------ connect to server successfully
//         V
//    ESTABLISHED
//         |
//         | ------> transmission_request to server
//         | <------ transmission_response from server
//         V
//    TRASMITTING 
//         | -------> send_complete to server
//         | <------- receive_complete from server
//         V
//  TRANSMITT_COMPLETE
//         |
//         |
//         V
//        EXIT
//
// 

typedef enum _status_value {
	UNDEFINED = 0,
	ACCEPTING = 1,
	CONNECTING = 2,
	ESTABLISHED = 3,
	TRASMITTING = 4,
	TRANSMITT_COMPLETE = 5
} status_value;

typedef enum _message_type {
	TRANSMISSION_REQUEST = 0,
	TRANSMISSION_RESPONSE = 1,
	TRANSMISSION_SEND_COMPLETE = 2,
	TRANSMISSION_RECEIVE_COMPLETE = 3
} message_type;

/// messages
typedef struct _transmission_request {
	uint8_t type;
} transmission_request;

typedef struct _transmission_response {
	uint8_t type;
} transmission_response;

typedef struct _send_complete {
	uint8_t type;
} send_complete;

typedef struct _receive_complete {
	uint8_t type;
} receive_complete;

typedef struct _sftp_header {
	int       namesize;
	long long datasize;
	
	static const size_t size = sizeof(int) + sizeof(long long);

	long long get_total_size() const {
		return namesize + datasize + size;
	}
} sftp_header;

static const size_t sftp_header_size = sizeof(sftp_header);

#define set_bval(ptr,val)                 ( * (uint8_t*)(ptr) = (uint8_t)(val) )
#define set_wval(ptr,val)                 ( * (uint16_t*)(ptr) = (uint16_t)(val) )
#define set_dval(ptr,val)                 ( * (uint32_t*)(ptr) = htonl((uint32_t)(val)) )
#define set_qval(ptr,val)                 ( * (uint64_t*)(ptr) = (uint64_t)(val) )

#define set_bval_step(ptr,val)            ( ( * (uint8_t*)(ptr) = (uint8_t)(val) ), ++ptr )
#define set_wval_step(ptr,val)            ( ( * (uint16_t*)(ptr) = (uint16_t)(val) ), ptr += 2 )
#define set_dval_step(ptr,val)            ( ( * (uint32_t*)(ptr) = htonl((uint32_t)(val)) ), ptr += 4 ) // ok
#define set_qval_step(ptr,val)            ( ( * (uint64_t*)(ptr) = htonll( (uint64_t)(val) ), ptr += 8 ) )

#define set_stream(ptr,bstream,size)      ( memcpy((ptr), (bstream), (size)) )
#define set_stream_step(ptr,bstream,size) ( memcpy((ptr), (bstream), (size)), ptr += size )
#define set_stream_noncopy(ptr,bstream,size) // no significance
#define set_stream_step_noncopy(ptr,bstream,size) // no significance

#define get_bval(ptr,val)                 ( * (uint8_t*)(ptr) = (uint8_t)(val) )
#define get_wval(ptr,val)                 ( * (uint16_t*)(ptr) = (uint16_t)(val) )
#define get_dval(ptr,val)                 ( * (uint32_t*)(ptr) = (uint32_t)(val) )
#define get_qval(ptr,val)                 ( * (uint64_t*)(ptr) = (uint64_t)(val) )

#define get_bval_step(ptr,val)            ( ( * (uint8_t*)(ptr) = (uint8_t)(val) ), ++ptr )
#define get_wval_step(ptr,val)            ( ( (val) = * (uint16_t*)(ptr) ), ptr += 2 )
#define get_dval_step(val,ptr)            ( ( (val) = ntohl(*(uint32_t*)(ptr)) ), ptr += 4 ) // ok
#define get_qval_step(ptr,val)            ( ( * (uint64_t*)(ptr) = (uint64_t)(val) ), ptr += 8 )

#define get_stream(bstream,ptr,size)      ( memcpy((ptr), (bstream), (size)) )
#define get_stream_step(bstream,ptr,size) ( memcpy((ptr), (bstream), (size)), ptr += size )
#define get_stream_noncopy(bstream,ptr,size) ( ((bstream) = (ptr)) )
#define get_stream_step_noncopy(bstream,ptr,size) ( ((bstream) = (ptr)), ptr += size )

// non deep copy, can't implement currently
static int encode(const sftp_header& info, char*& outbuf)
{
	//totalsize = sizeof(msg.totalsize) + 
	//	sizeof(msg.body.namesize) + sizeof(msg.body.datasize) +
	//	msg.body.namesize + msg.body.datasize;
	//try {
	//	outbuf = new char[totalsize];
	//}
	//catch(std::bad_alloc&){
	//	return -1;
	//}
	//char* ptr = outbuf;
	//set_dval_step(ptr, htonl(totalsize));
	//set_dval_step(ptr, htonl(msg.body.namesize));
	//set_stream_step(ptr, msg.body.name, msg.body.namesize);
	//set_dval_step(ptr, htonl(msg.body.datasize));
	//set_stream_step(ptr, msg.body.data, msg.body.datasize);
	return 0;
}

// non deep copy, need skip total size field.
static int decode_sftp_header(const char* in, sftp_header& out)
{
	const char* ptr = in;
	out.namesize = __bswap32(*((const long long*)ptr));
	ptr += 4;
	out.datasize = __bswap64(*((const long long*)ptr));
	return 0;
}

// non deep copy, need skip total size field.
static int encode_sftp_header(const sftp_header& in, char* out)
{
	char* ptr = out;
	*((long long*)ptr) = __bswap32(in.namesize);
	ptr += 4;
	*((long long*)ptr) = __bswap64(in.datasize);
	return 0;
}



#define BUFFER_SIZE  SZ(32,M)

static void dump_hex_i(const char* prompt, const char* hex, int len, FILE* fp = stdout, char delim = ',', int line_feed = 16)
{
	fprintf(fp, "%s\n", prompt);
	for (int i = 0; i < len; ++i)
	{
		if (i + 1 != len) {
			if ((i + 1) % line_feed != 0) {
				fprintf(fp, "0x%02x%c ", (unsigned char)hex[i], delim);
			}
			else {
				fprintf(fp, "0x%02x%c\n", (unsigned char)hex[i], delim);
			}
		}
		else {
			fprintf(fp, "0x%02x\n", (unsigned char)hex[i]);
		}
	}
	fprintf(fp, "\n");
}

#endif
/*
* Copyright (c) 2012-2019 by X.D. Guo  ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
**/

