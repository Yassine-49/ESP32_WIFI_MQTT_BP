#if !defined(NXT_DEBUG_H)
#define NXT_DEBUG_H

#include "main.h"

void dbg(String data);
void dbgln(String data = "");
void dbg(char data, int format);
void dbgln(char data, int format);

#endif // NXT_DEBUG_H
