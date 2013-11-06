#if !defined DYNDNSUPDATERSTATE_H
#define DYNDNSUPDATERSTATE_H

// represent the persistent state of the DynDNS Updater
struct State;
typedef struct State* State_t;

State_t newState();
int loadState(State_t, const char* filename);
int saveState(State_t, const char* filename);
int deleteState(State_t);

int isEnabled(State_t);
const char* getUpdateDateTimeString(State_t);
const char* getPrevIp(State_t);
const char* getPrevResult(State_t);

int setEnabled(State_t, int isEnabled);
int setUpdateTimeNow(State_t);
int setIp(State_t, const char* ip);
int setDateTime(State_t, const char* dateTime);
int setResult(State_t, const char* result);


#endif //!defined DYNDNSUPDATERSTATE_H

