//
//  debug.cpp
//  EuterpeXL
//
//  Created by David Gunnarsson on 5/12/12.
//  Copyright 2012-2013 guda audio. All rights reserved.
//

#include "../JuceLibraryCode/JuceHeader.h"

#include "debug.h"
#include <cstring>
#include <ctime>
//#include <sys/time.h>
#include "timeHelper.h"
#include <cstdio>
//#include "globalStuff.h"
//#if OSX
//#include <unistd.h>
//#endif

#define OSX 1

//#include "windowshacks.h"
//#define IGNORE_EVERY_THOUSAND 1

vector<string> g_debugOutVector;


#ifdef OSX
 #ifdef DEBUG_OVER_NETWORK
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
 #endif
#endif

#ifndef DEBUG_BUILD
  #define IGNORE_EVERY_THOUSAND 1
#endif

#ifndef IGNORE_EVERY_THOUSAND
//static clock_t startTime[8];
static clock_t accumulatedTime[8] = {0,0,0,0,0,0,0,0};
static clock_t avarageTime[8] = {0,0,0,0,0,0,0,0};
static int timingCounts[8] = {0,0,0,0,0,0,0,0};
static int isRunning[8] = {0,0,0,0,0,0,0,0};
static clock_t lastAddTime;
static const char* everyThousantNames[8] = {0,0,0,0,0,0,0,0};

#endif //! IGNORE_EVERY_THOUSAND

static CriticalSection debugLock;


void sendDebugOverNetwork(const char* str) {
    if(str[0] == '\n') {
        return;
    }
#ifdef OSX
#ifdef DEBUG_BUILD
    static bool socketCreated = false;
    static sockaddr_in myaddr;
    static int sock;

    if(!socketCreated) {
        socketCreated = true;
    
        memset(&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family=AF_INET;
        myaddr.sin_addr.s_addr=htonl(INADDR_ANY);
        
        myaddr.sin_port=htons(0);
        
        if((sock=socket(AF_INET, SOCK_DGRAM, 0))<0) {
            perror("Failed to create socket");
            exit(EXIT_FAILURE);
        }
        
        int res = ::bind(sock,(sockaddr *) &myaddr, sizeof(myaddr));
        
        if(res<0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        
        std::string to("blitz.cs.niu.edu");
        inet_pton(AF_INET,"127.0.0.1",&myaddr.sin_addr.s_addr);
        myaddr.sin_port=htons(9999);
        
    }
    int bytesSent = sendto(sock, str, strlen(str), 0, (struct sockaddr *)&myaddr, sizeof(myaddr));
    if (bytesSent != strlen(str)) {
        perror("Mismatch in number of bytes sent");
        exit(EXIT_FAILURE);
    }

   //close(sock);
#endif //DEBUG_BUILD
#endif //OSX
}

void everyThousandName(int id, const char* name) {
#ifndef IGNORE_EVERY_THOUSAND
    everyThousantNames[id] = name;
#endif
}

void everyThousandStart(int id, const char* name) {
#ifndef IGNORE_EVERY_THOUSAND
    if(name) {
        everyThousantNames[id] = name;
    }
    if(id > 7) {
        DBUG(("too big id",0));
        return;
    }

    isRunning[id] = 1;
    
    //startTime[id] = clock();
    //        if(timingCounts[id]>=999) {
    //            DBUG(("last run on %i", id));
    //        }
    clock();
#endif
}

#ifdef DEBUG_BUILD
const int everyNth = 1000;
#endif
void everyThousandEnd(int id) {
#ifndef IGNORE_EVERY_THOUSAND
    if(id > 7) {
        DBUG(("too big id",0));
        return;
    }

    //if(!isRunning[id]) {
    //    DBUG(("WARNING, %i not running", id));
    //}
    isRunning[id] = 0;
    clock_t addTime = clock();
    clock_t tmp = addTime;
    addTime = addTime - lastAddTime;
    lastAddTime = tmp;
    
    //DBUG(("addTime %i", addTime));
    accumulatedTime[id] += addTime;
    if(++timingCounts[id] >= everyNth) {
        if(id!=7) {
            if(avarageTime[id] == 0) {
                avarageTime[id] = accumulatedTime[id];
            } else {
                avarageTime[id] = avarageTime[id]*0.9 + accumulatedTime[id]*0.1;
            }
            if(everyThousantNames[id]) {
                DBUG(("%s, %ul milliseconds, avg %ul", everyThousantNames[id], accumulatedTime[id]/((everyNth/1000)*CLOCKS_PER_SEC), avarageTime[id]/((everyNth/1000)*CLOCKS_PER_SEC) ));
            } else {
                DBUG(("id %i, %ul milliseconds, avg %ul", id, accumulatedTime[id]/(everyNth*CLOCKS_PER_SEC), avarageTime[id]/((everyNth/1000)*CLOCKS_PER_SEC)));
            }
        }

        timingCounts[id] = 0;
        accumulatedTime[id] = 0;
        //for(int i = 0 ; i < 8 ; i++) {
        //    DBUG(("id %i, 1000 repeats to total of %ul milliseconds, timingCounts[id] %i", i, accumulatedTime[i], timingCounts[i]));
        //}
    }
#endif
}

const char* gFUNC__ = 0;
const char* gFILE__ = 0;
int gLINE__;

void output(const char* str) {
#ifdef DEBUG_TO_VECTOR
    g_debugOutVector.push_back(str);
#endif
#ifdef OSX
    cout << str;
//    puts(str);
#else
    OutputDebugStringA(str);
#endif
#ifdef DEBUG_OVER_NETWORK
    sendDebugOverNetwork(str);
#endif
}

void outputf(const char* str, ...) {
        va_list ap;
        va_start(ap, str);
        char tmp[2048];
        for(int i = 0 ; i < 2048; ++i) {
            tmp[i]=0;
        }

        vsnprintf(tmp, 2047, str, ap);
        output(tmp);
    va_end(ap);
}

static const bool global_debug_print_time = true;

void DBG2FILE(const char* str, ...) {
    DBG2("file logging not supported");
}

void DBG2(const char* str, ...) {
#ifdef DEBUG_BUILD
    const ScopedLock sl(debugLock);
    static volatile int lock = 0;
    
start:
    
    if(!lock) {
        lock=1;
#ifdef OSX
        const char * file = strrchr ( gFILE__,'/') + 1;
        //file += 1;
        const char * func = gFUNC__;
#else
        const char * file = strrchr ( gFILE__,'\\') + 1;
        char * func_ = (char*)strrchr ( gFUNC__,':');
        if(func_) {
            func_++;
        } else if(gFUNC__) {
            func_ = (char*)gFUNC__;
        } else {
            func_ = "?";
        }

        const char* func = func_;
#endif

        const int line = gLINE__;

        if(!(gFUNC__ && gLINE__ && gFILE__)) {
            output("error in DBG2\n");
            return;
        }
        
        va_list ap;
        va_start(ap, str);
        char tmp[2048];
        for(int i = 0 ; i < 2048; ++i) {
            tmp[i]=0;
        }
        
        if(global_debug_print_time) {
            //print time
            time_t t = time(0);   // get time now
            struct tm * now = localtime( & t );
            timeval tid;
            gettimeofday(&tid, NULL);
            outputf("%i:%i:%i.%02d ", now->tm_hour, now->tm_min, now->tm_sec, (tid.tv_usec/10000));
            //end print time
        }
        
        if(file && line && func) {
            //printf("%s:%i:%s - ", file, line, func);
            outputf("%s:%i:%s - ", file, line, func);
        } else {
            output("bad file gLINE or gFUNC\n");
        }        
        
        vsnprintf(tmp, 2047, str, ap);
        output(tmp);
        output("\n");

        va_end(ap);
        fflush(stdout);
        lock=0;
    } else {
        output("no output, lock is on, retry\n");
        for (int i  = 0 ; i < 20 ; i++) {
            if(!lock) {
                //output("goto start\n");
                goto start;
            } else {
                usleep(10 + i);
                //if(i > 15) {
                    output("sleeping\n");
                //}
            }
        }
        output("WARNING, debug printing failed after 15 attempts, forced print lock off\n");
        lock=0;
        goto start;

    }
#endif //DEBUG_BUILD
}

void DEPRECATEDFUNC() {
#ifdef DEBUG_BUILD
    const char * file = strrchr ( gFILE__,'/') + 1;
    //file += 1;
    
    if(!(gFUNC__ && gLINE__ && gFILE__)) {
        output("error in WARNINGFUNC");
        return;
    }
    
    outputf("deprecated function %s in %s:%in\n", gFUNC__, file, gLINE__);
#endif //DEBUG_BUILD
}










