#include "debug.h"

void dbg(String data)
{
    if(xSemaphoreTake(xdebug, 1000))
    {
        Serial.print(data);
        xSemaphoreGive(xdebug);
    }
}

void dbgln(String data)
{
    if(xSemaphoreTake(xdebug, 1000))
    {
        Serial.println(data);
        xSemaphoreGive(xdebug);
    }
}

void dbg(char data, int format)
{
    if(xSemaphoreTake(xdebug, 1000))
    {
        Serial.print(data, format);
        xSemaphoreGive(xdebug);
    }
}

void dbgln(char data, int format)
{
    if(xSemaphoreTake(xdebug, 1000))
    {
        Serial.println(data, format);
        xSemaphoreGive(xdebug);
    }
}
