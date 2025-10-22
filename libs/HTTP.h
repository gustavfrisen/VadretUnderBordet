#ifndef HTTP_H
#define HTTP_H

#define HTTP_VERSION "HTTP/1.1"
#define MAX_URL_LEN 256

// If a HTTPRequest is not valid, why?
typedef enum
{
    NotInvalid = 0,

    MalformedRequest = 1,
    OutOfMemory = 2,
    URLTooLong = 3
} InvalidReason;


typedef enum
{
    GET = 0,
    POST = 1,

    Method_Unknown = 2
} RequestMethod;

typedef enum
{
    HTTP_0_9 = 1,
    HTTP_1_0 = 2,
    HTTP_1_1 = 3,
    HTTP_2_0 = 4,
    HTTP_3_0 = 5,

    Protocol_Unknown = 6
} ProtocolVersion;

// Serverside functions
typedef struct {
    int valid; // If false (0), then the request could not be parsed. Panic!
    InvalidReason reason;

    RequestMethod method;
    ProtocolVersion protocol;
    char URL[MAX_URL_LEN];
} HTTPRequest;

HTTPRequest* ParseRequest(const char* request);

#endif

// HTTP Struct
// ParseRequest();
// GET <request> HTTP/1.1