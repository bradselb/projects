#if !defined HTTP_REQUEST_H
#define HTTP_REQUEST_H

struct HttpRequest;

struct HttpRequest* http_alloc_request(const char* host, const char* resource);

void http_free_request(struct HttpRequest*);

int http_set_host(struct HttpRequest*, const char* host);
int http_set_port(struct HttpRequest*, int port);
int http_set_resource(struct HttpRequest*, const char* resource);

int http_set_user_agent(struct HttpRequest*, const char* user_agent);
int http_set_basic_auth(struct HttpRequest*, const char* auth);

int http_to_string(struct HttpRequest* request, char* buf, unsigned int bufsize);

#endif // !defined HTTP_REQUEST_H
