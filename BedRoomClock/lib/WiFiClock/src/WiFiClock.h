#ifndef WIFICLOCK_H
#define WIFICLOCK_H
#include <WiFi.h>
#include "time.h"
#include <DS1302.h>
#define SEC_OFFSET_US_EASTERN -18000

class WifiClock
{
private:
    int extractNumFromString(char * inpStr,int firstChar, int numChar);
    boolean setDateTimeFromNTP();
    boolean compareIntExtRTCs();
    boolean convertNativeRTCDt2DS1302Dt(Ds1302::DateTime* DS1302TimeStruct);

public:
    typedef struct 
    {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t dow;
    } WifiClockInfo;
    WifiClockInfo getWifiClockInfo();
    void init();
};

#endif //WIFICLOCK_H