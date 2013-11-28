#if !defined DYNDNSUPDATERSTATE_H
#define DYNDNSUPDATERSTATE_H

// represent the persistent state of the DynDNS Updater
struct State;
typedef struct State* State_t;

struct State* newState();
void deleteState(struct State*);

int loadState(struct State*, const char* filename);
int saveState(const struct State*, const char* filename);

int readStateFile(struct State*, int fd);
int writeStateFile(const struct State*, int fd);

int isEnabled(const struct State*);
const char* getUpdateDateTimeString(const struct State*);
const char* getPrevIp(const struct State*);
const char* getPrevResult(const struct State*);

int setEnabled(struct State*, int isEnabled);
int setUpdateTimeNow(struct State*);
int setIp(struct State*, const char* ip);
int setDateTime(struct State*, const char* dateTime);
int setResult(struct State*, const char* result);


#endif //!defined DYNDNSUPDATERSTATE_H

