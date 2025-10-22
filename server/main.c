// Läs in grundläggande städer
// Börja lyssna efter förfrågan från klienter
// Hitta stad, hämta väder, returnera svar

#include "HTTP.h"
#include <stdio.h>

const char* demoRequest = "GET /this/is/a/test/request HTTP/1.1\r\nHost: example.com\r\nX-Test: hello\r\n\r\n";

int main()
{
    printf("\n");
    HTTPRequest* req = ParseRequest(demoRequest);

    printf("Request valid: %i\n",req->valid);
    printf("Invalid reason: %i\n",req->reason);
    if(req->valid)
    {
        printf("Method: %i\n",req->method);
        printf("Protocol: %i\n",req->protocol);
        printf("URL: %s\n\n",req->URL);

        printf("Total headers (%lu):\n",req->Headers->size);
        Node* item = req->Headers->head;
        while(item)
        {
            HTTPHeader* hdr = (HTTPHeader*)item->item;
            printf("%s = %s\n", hdr->Name, hdr->Value);
            
            item = item->front;
        }
    }

    HTTPRequest_Dispose(&req);

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