// Läs in grundläggande städer
// Börja lyssna efter förfrågan från klienter
// Hitta stad, hämta väder, returnera svar

#include "HTTP.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("\nCLIENT: HTTP request generation\n\n");

    HTTPRequest* request = HTTPRequest_new(GET, "/v1/test");
    HTTPRequest_add_header(request, "User-Agent", "VädretUnderBordet/1.0");
    const char* string_req = HTTPRequest_tostring(request);
    printf("%s\n", string_req);
    HTTPRequest_Dispose(&request);

    printf("\nSERVER: HTTP request parsing\n\n");

    HTTPRequest* req = HTTPRequest_fromstring(string_req);
    free((void*)string_req);

    printf("Request valid: %i\n",req->valid);
    printf("Reason invalid: %i\n",req->reason);
    if(req->valid)
    {
        printf("Method: %i\n",req->method);
        printf("Protocol: %i\n",req->protocol);
        printf("URL: %s\n\n",req->URL);

        printf("Total headers (%lu):\n",req->headers->size);
        LinkedList_foreach(req->headers, item) {
            HTTPHeader* hdr = (HTTPHeader*)item->item;
            printf("%s = %s\n", hdr->Name, hdr->Value);
        };
    }
    HTTPRequest_Dispose(&req);

    printf("\nSERVER: HTTP response generation\n\n");

    HTTPResponse* resp = HTTPResponse_new(OK, "test body text");
    HTTPResponse_add_header(resp, "X-Test","test header");
    const char* string_resp = HTTPResponse_tostring(resp);
    printf("%s\n", string_resp);
    HTTPResponse_Dispose(&resp);

    printf("\nCLIENT: HTTP response parsing\n\n");

    HTTPResponse* response = HTTPResponse_fromstring(string_resp);
    free((void*)string_resp);
    printf("Response valid: %i\n",response->valid);
    printf("Reason invalid: %i\n",response->reason);
    if(response->valid)
    {
        printf("Protocol: %i\n",response->protocol);
        printf("Code: %i\n",response->responseCode);

        printf("Total headers (%lu):\n",response->headers->size);
        LinkedList_foreach(response->headers, item) {
            HTTPHeader* hdr = (HTTPHeader*)item->item;
            printf("%s = %s\n", hdr->Name, hdr->Value);
        };

        printf("Body:\n%s\n",response->body);
    }
    HTTPResponse_Dispose(&response);

    // LoadCities();
    // TCP_INIT();
    // while {
    //  listen();
    //  HTTP_parse();
    //  switch {
    //    case func1:
    //      func1();
    //    case func2:
    //      func2();
    //    case func3:
    //      func3();
    //   }
    // Skicka tillbaks http formaterat meddelande
    // }

    return 0;
}