#include "State.h"
#include "timeHelpers.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // memset()


// represent the persistent state of the DynDNS Updater
struct State
{
   int enabled; // is the updater enabeled? 
   char* dateTime; // date and time of last update
   char* ipAddress; // our IP address last time 
   char* result; // the result from last update

};

// ----------------------------------------------------------------------
State_t newState()
{
   struct State* state;

   state = malloc(sizeof(struct State));
   if (state) {
      memset(state, 0, sizeof(struct State)); 
      state->enabled = 0;
      state->dateTime = 0;
      state->ipAddress = 0;
      state->result = 0;
   }
   return state;
}

// ----------------------------------------------------------------------
void deleteState(struct State* state)
{
   if ( state ) {
      if (state->dateTime) {
         free(state->dateTime);
      }
      if (state->ipAddress) {
         free(state->ipAddress);
      }
      if (state->result) {
         free(state->result);
      }
   }
}


// ----------------------------------------------------------------------
int loadState(struct State* state, const char* filename)
{
   int rc = -1;
   const int bufsize = 50; // arbitrary but, big enough. 
   char buf[bufsize];
   memset(buf, 0, bufsize);
   FILE* file = fopen(filename, "r");

   if ( state && file ) {
      char* p = 0;
      int line = 0;
      while ( 0 != (fgets(buf, bufsize, file)) ) {
         ++line;
         // fgets() copies the newline char to the buffer. 
         // remove newline at end of line by replacing it with nul
         if ( 0 != (p = strpbrk(buf, "\n\r")) ) {
            *p = 0; // just terminate string at newline
         }

         if ( 1 == line ) {
            state->enabled = strtol(buf, 0, 0);
         } else if ( 2 == line ) {
            // date-time
            setDateTime(state, buf);
         } else if ( 3 == line ) {
            // ip address
            setIp(state, buf);
         } else if ( 4 == line ) {
            // result
            setResult(state, buf);
            rc = 0;
         } else {
            // not sure what to do here. 
            // could be that we have extra lines at end of file. 
            // so, not necessarily an error. 
         }
      } // while
      fclose(file);
   } // if (file) 
   return rc;
}


// ----------------------------------------------------------------------
int saveState(const struct State* state, const char* filename)
{
   int rc = -1;
   FILE* file = fopen(filename, "w");
   
   if ( state && file ) {
      fprintf(file, "%d\n", state->enabled);
      fprintf(file, "%s\n", state->dateTime);
      fprintf(file, "%s\n", state->ipAddress);
      fprintf(file, "%s\n", state->result);
      fclose(file);
      rc = 0;
   }
   return rc;
}


// ----------------------------------------------------------------------
int isEnabled(const struct State* state)
{
   return (state ? state->enabled : 0);
}

// ----------------------------------------------------------------------
const char* getUpdateDateTimeString(const struct State* state)
{
   return (state ? state->dateTime : 0);
}

// ----------------------------------------------------------------------
const char* getPrevIp(const struct State* state)
{
   return (state ? state->ipAddress : 0);
}

// ----------------------------------------------------------------------
const char* getPrevResult(const struct State* state)
{
   return (state ? state->result : 0);
}


// ----------------------------------------------------------------------
int setEnabled(struct State* state, int isEnabled)
{
   int rc = -1;
   if ( state ) {
      state->enabled = (isEnabled ? 1 : 0);
      rc = 0;
   }
   return rc;
}


// ----------------------------------------------------------------------
int setUpdateTimeNow(struct State* state)
{
   int rc = -1;

   if ( !state ) {
      rc = -1;
   } else {
      if ( state->dateTime ) {
         free(state->dateTime);
         state->dateTime = 0;
      }

      const int len = 32;
      state->dateTime = (char*)malloc(len);
      memset(state->dateTime, 0, len);
      getTimeNow(state->dateTime, len-1);
      rc = 0;
   }

   return rc;
}

// ----------------------------------------------------------------------
int setIp(struct State* state, const char* ip)
{
   int rc = -1;
   if ( state && ip ) {
      if ( state->ipAddress ) {
         free(state->ipAddress);
      }

      int size = 1 + strlen(ip);
      state->ipAddress = (char*)malloc(size);
      memset(state->ipAddress, 0, size);
      strncpy(state->ipAddress, ip, size);
      rc = 0;
   }
   return rc;
}

// ----------------------------------------------------------------------
int setDateTime(struct State* state, const char* dateTime)
{
   int rc = -1;
   if ( state && dateTime ) {
      if ( state->dateTime ) {
         free(state->dateTime);
      }

      int size = 1 + strlen(dateTime);
      state->dateTime = malloc(size);
      memset(state->dateTime, 0, size);
      strncpy(state->dateTime, dateTime, size);
      rc = 0;
   }
   return rc;
}


// ----------------------------------------------------------------------
int setResult(struct State* state, const char* result)
{
   int rc = -1;
   if ( state && result ) {
      if ( state->result ) {
         free(state->result);
      }

      unsigned int length = strlen(result);
      state->result = malloc(length +1);
      strncpy(state->result, result, length);
      rc = 0;
   }
   return rc;
}

