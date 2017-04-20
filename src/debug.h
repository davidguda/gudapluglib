//
//  debug.h
//  EuterpeXL
//
//  Created by David Gunnarsson on 5/12/12.
//  Copyright 2012 guda audio. All rights reserved.
//

#ifndef EUTERPE_DEBUG_H
#define EUTERPE_DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <vector>
#include <string>
using namespace std;

extern void DBG2(const char*, ...);
extern void DBG2FILE(const char*, ...);
extern void DEPRECATEDFUNC(const char*, ...);

extern void debugToVector(const char*, ...);

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

extern const char* gFUNC__;
extern const char* gFILE__;
extern int gLINE__;

extern vector<string> g_debugOutVector;
// enable DEBUG_TO_VECTOR to debug to vector as well as to stdout

extern void everyThousandName(int id, const char * name);
extern void everyThousandStart(int id, const char* name = 0);
extern void everyThousandEnd(int id);

class DBGCLASS {
public:
    DBGCLASS() : func (gFUNC__), file(gFILE__), line(gLINE__) {
        text = "";
        DBG2("start");
    };

    DBGCLASS(string text_in) : func (gFUNC__), file(gFILE__), line(gLINE__) {
        text = text_in;
        DBG2(string(string("start") + text).c_str());
    };
    
    ~DBGCLASS() {
        gFUNC__ = func;
        gFILE__ = file;
        gLINE__ = line;
        DBG2(string(string("end") + text).c_str());
    }
private:
    string text;
    const char* func;
    const char* file;
    int line = 0;
};

class TIMEDDBGCLASS {
public:
    TIMEDDBGCLASS() : func (gFUNC__), file(gFILE__), line(gLINE__) {
        text = "";
        DBG2("start");
        everyThousandStart(id, text.c_str());
    };
    
    TIMEDDBGCLASS(string text_in, const int id_in) : func (gFUNC__), file(gFILE__), line(gLINE__), id(id_in) {
        text = text_in;
        DBG2(string(string("start") + text).c_str());
        everyThousandStart(id, text.c_str());
    };
    
    ~TIMEDDBGCLASS() {
        gFUNC__ = func;
        gFILE__ = file;
        gLINE__ = line;
        DBG2(string(string("end") + text).c_str());
        everyThousandEnd(id);
    }
private:
    string text;
    const char* func;
    const char* file;
    int line = 0;
    int id = 0;
};

#ifdef DEBUG_BUILD
  #define DEPRECATED gFUNC__ = __func__ ; gFILE__ = __FILE__ ; gLINE__ = __LINE__ ; DEPRECATEDFUNC;
  #define DBUG(x) gFUNC__ = __func__ ; gFILE__ = __FILE__ ; gLINE__ = __LINE__ ; DBG2 x ;
  #define DBUG2FILE(x) gFUNC__ = __func__ ; gFILE__ = __FILE__ ; gLINE__ = __LINE__ ; DBG2 x ;
  #define DBGF DBUG((""));
  #define DBGRAII gFUNC__ = __func__ ; gFILE__ = __FILE__ ; gLINE__ = __LINE__ ; DBGCLASS DBGOBJ;
  #define NAMEDRAII gFUNC__ = __func__ ; gFILE__ = __FILE__ ; gLINE__ = __LINE__ ; DBGCLASS DBGOBJ x;
  #define TIMEDRAII gFUNC__ = __func__ ; gFILE__ = __FILE__ ; gLINE__ = __LINE__ ; TIMEDDBGCLASS DBGOBJ;
  #define NAMEDTIMEDRAII gFUNC__ = __func__ ; gFILE__ = __FILE__ ; gLINE__ = __LINE__ ; TIMEDDBGCLASS DBGOBJ x;
  #define DEBUG_OVER_NETWORK
#else
  #define DEPRECATED ;
  #define DBUG(x) ;
  #define DBUG2FILE(x) ;
  #define DBGF ;
  #define DBGRAII ;
  #define NAMEDRAII(x) ;
  #define TIMEDRAII ;
#endif


#define EVERYTHOUSANDDUMMY {everyThousandStart(7); everyThousandEnd(7);}

class DBGClass
{
public:
    const char* func;
    const char* file;
    int line;
    
    DBGClass(const char* func_in, const char* file_in, int line_in) {
        func = func_in;
        file = file_in;
        line = line_in;
    }
};

#endif //EUTERPE_DEBUG_H
