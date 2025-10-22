#include "HTTP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* substr(const char* start, const char* end) {
    if (!start || !end || end < start)
        return NULL;
    size_t len = end - start;
    char* out = malloc(len + 1);
    if (!out)
        return NULL;
    memcpy(out, start, len);
    out[len] = '\0';
    return out;
}

RequestMethod Enum_Method(const char* method)
{
    if(!method) return Method_Unknown;

    if (strcmp(method, "GET") == 0) return GET;
    if (strcmp(method, "POST") == 0) return POST;

    return Method_Unknown;
}

ProtocolVersion Enum_Protocol(const char* protocol)
{
    if(!protocol) return Protocol_Unknown;

    if (strcmp(protocol, "HTTP/0.9") == 0) return HTTP_0_9;
    if (strcmp(protocol, "HTTP/1.0") == 0) return HTTP_1_0;
    if (strcmp(protocol, "HTTP/1.1") == 0) return HTTP_1_1;
    if (strcmp(protocol, "HTTP/2.0") == 0) return HTTP_2_0;
    if (strcmp(protocol, "HTTP/3.0") == 0) return HTTP_3_0;

    return Protocol_Unknown;
}

HTTPRequest* ParseRequest(const char* message)
{
    HTTPRequest* request = calloc(1, sizeof(HTTPRequest));
    request->reason = MalformedRequest;

    int state = 0;

    const char* start = message;
    while(start && *start)
    {
        // Find end of line
        const char* end = strstr(start, "\r\n");
        if(!end)
        {
            printf("Did not find another line separator.\n");
            break;
        }
        // Check length with pointer math
        int length = end - start;
        if(length == 0)
        {
            printf("Reached end of request.\n");
            break;
        }
        // Allocate memory for current line
        char* current_line = substr(start, end);
        if(!current_line) // horrible problem
            break;

        if(state == 0)
        {
            // Count spaces in request, should match 2
            int count = 0;
            char* scan = current_line;
            for(; *scan; scan++)
            {
                if(*scan == ' ')
                    count++;
            }
            if(count != 2)
            {
                printf("INVALID: Request is not formatted with 2 spaces.\n");
                free(current_line);
                break;
            }

            const char* space1 = strchr(current_line, ' ');
            const char* space2 = strchr(space1 + 1, ' ');

            if(space2 - (space1 + 1) >= MAX_URL_LEN)
            {
                printf("INVALID: Request URL is too long\n");
                request->reason = URLTooLong;
                free(current_line);
                break;
            }

            char* method = substr(current_line, space1);
            if(!method) { free(current_line); request->reason = OutOfMemory; break; }
            char* path = substr(space1 + 1, space2);
            if(!path) { free(current_line); request->reason = OutOfMemory; break; }
            char* protocol = substr(space2 + 1, current_line + length);
            if(!protocol) { free(current_line); request->reason = OutOfMemory; break; }

            request->method = Enum_Method(method);
            request->protocol = Enum_Protocol(protocol);

            snprintf(request->URL, MAX_URL_LEN, "%s", path);

            free(method);
            free(path);
            free(protocol);

            request->valid = 1;
            state = 1; // jump to header parsing
        } else {
            printf("Header support incomplete.\n\n");
            free(current_line);
            break;
        }

        start = end + 2;
        free(current_line);
    }

    return request;
}