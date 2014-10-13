#if !defined HTTP_REQUEST_H
#define HTTP_REQUEST_H

struct HttpRequest;

struct HttpRequest* http_alloc_request(void);

void http_free_request(struct HttpRequest*);

int http_set_host(struct HttpRequest*, const char* host);
int http_set_port(struct HttpRequest*, const char* port);
int http_set_resource(struct HttpRequest*, const char* resource);
int http_set_user_agent(struct HttpRequest*, const char* user_agent);
int http_set_basic_auth(struct HttpRequest*, const char* user, const char* pass);

int http_to_string(struct HttpRequest*, char* buf, int bufsize);

int http_send(struct HttpRequest* request, int fd, int timeout);

int http_wait_reply(struct HttpRequest*, char* buf, int bufsize, int fd, int timeout);


#endif // !defined HTTP_REQUEST_H
