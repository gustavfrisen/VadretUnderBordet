#ifndef HTTP_H
#define HTTP_H

#define HTTP_VERSION "HTTP/1.1"
#define MAX_URL_LEN 256

#include "linked_list.h"

// A HTTPRequest struct should only be disposed by HTTPRequest_Dispose

// If a HTTPRequest is not valid, why?
typedef enum
{
    Unknown = 0,
    NotInvalid = 1,

    MalformedRequest = 2,
    OutOfMemory = 3,
    URLTooLong = 4 // Originally existed because the URL was fixed size in the struct, but kept for extra safety
} InvalidReason;


typedef enum
{
    Method_Unknown = 0,

    GET = 1,
    POST = 2,
} RequestMethod;

typedef enum
{
    Protocol_Unknown = 0,

    HTTP_0_9 = 1,
    HTTP_1_0 = 2,
    HTTP_1_1 = 3,
    HTTP_2_0 = 4,
    HTTP_3_0 = 5
} ProtocolVersion;

typedef struct {
    const char* Name;
    const char* Value;
} HTTPHeader;

// Serverside functions
typedef struct {
    int valid; // If false (0), then the request could not be parsed. Panic!
    InvalidReason reason;

    RequestMethod method;
    ProtocolVersion protocol;
    const char* URL;

    LinkedList* Headers;
} HTTPRequest;

HTTPRequest* ParseRequest(const char* request);
void HTTPRequest_Dispose(HTTPRequest** request);

#endif

// HTTP Struct
// ParseRequest();
// GET <request> HTTP/1.1