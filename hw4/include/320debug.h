#ifndef __DEBUG__
#define __DEBUG__
#include "csapp.h"
/*Will contain macros for different debugging messages*/
#ifdef DEBUG
   #define DEBUG_MSG(msg) do{fprintf(stderr,"DEBUG: %s\n", msg);}while(0);
   #define DEBUG_MSG_TWO(msg, msg2) do{fprintf(stderr,"DEBUG: %s %s\n", msg, msg2);}while(0);
   #define DEBUG_LINE(msg) do{fprintf(stderr,"DEBUG: %s:%s:%d %s\n",__FILE__,__FUNCTION__,__LINE__,msg);}while(0);
   #define INFO(fmt, ...) do{fprintf(stderr,"INFO: %s:%s:%d \n", ftom, __FILE__,__FUNCTION__, __LINE__, ##__VA_ARGS__);}while(0);
 #else
   #define DEBUG_MSG(msg)
   #define DEBUG_LINE(msg)
   #define INFO(fmt, ...)
   #define DEBUG_MSG_TWO(msg, msg2)
 #endif

#endif /* __DEBUG__ */
