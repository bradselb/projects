#include "findReply.h"

// returns a pointer to the reply in a buffer containing an HTTP response

char* findReply(const char* buf, unsigned int bufsize)
{
    char* p;
    unsigned int i;
    char ch;

    enum State {NONE, LF, CR, CRLF, CRLFCR, DONE} state;
    state = NONE;

    for (i=0, p=buf; i<bufsize && p!=0; ++i, ++p) {
        ch = *p;
        if (state == NONE)
            if (ch == '\r')
                state = CR;
            else if (ch == '\n')
                state = LF;
            else
                state = NONE;

        else if (state == LF)
            if (ch == '\n')
                state = DONE;
            else
                state = NONE;

        else if (state == CR)
            if (ch == '\n')
                state = CRLF;
            else if (ch == '\r')
                state = DONE;
            else
                state = NONE;

        else if (state == CRLF)
            if (ch == '\r')
                state = CRLFCR;
            else
                state = NONE;

        else if (state == CRLFCR)
            if (ch == '\n')
                state = DONE;
            else
                state = NONE;

        else if (state == DONE)
            // p now points to the start of the reply.
            break;

    }

    return p;
}
