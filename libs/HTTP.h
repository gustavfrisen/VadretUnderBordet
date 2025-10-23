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

    LinkedList* headers;
} HTTPRequest;

HTTPRequest* ParseRequest(const char* request);
void HTTPRequest_Dispose(HTTPRequest** request);

typedef enum
{
    ResponseCode_Unknown = 0,

    OK = 200,

    Moved_Permanently = 301,
    Found = 302,
    Not_Modified = 304,
    Temporary_Redirect = 307,
    Permanent_Redirect = 308,

    Bad_Request = 400,
    Unauthorized = 401,
    Forbidden = 403,
    Not_Found = 404,
    Method_Not_Allowed = 405,
    Request_Timeout = 408,
    Gone = 410,
    Length_Required = 411,
    Content_Too_Large = 413,
    URI_Too_Long = 414,
    Too_Many_Requests = 429,

    Internal_Server_Error = 500,
    Not_Implemented = 501,
    Bad_Gateway = 502,
    Service_Unavailable = 503,
    Gateway_Timeout = 504,
    HTTP_Version_Not_Supported = 505,
} ResponseCode;

typedef struct {
    ResponseCode responseCode;
    LinkedList* headers;
    const char* body;
} HTTPResponse;

HTTPResponse* HTTPResponse_init(ResponseCode code, const char* body);
int HTTPResponse_add_header(HTTPResponse* response, const char* name, const char* value);
const char* HTTPResponse_tostring(HTTPResponse** request);

HTTPRequest* ParseRequest(const char* request);
void HTTPRequest_Dispose(HTTPRequest** request);

#endif

// HTTP Struct
// ParseRequest();
// GET <request> HTTP/1.1