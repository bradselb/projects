#include "HttpRequest.h"
#include "inet.h"
#include "slist.h"

struct HttpRequest
{
    char* host;
    char* port;
    char* resource;
    char* user_agent;
    char* auth_user;
    char* auth_passwd;
};
