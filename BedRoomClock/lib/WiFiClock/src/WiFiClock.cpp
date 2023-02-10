#include "WiFiClock.h"

const char* ssid     = "Snappy";
const char* password = "Wholetthedogsout?";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = SEC_OFFSET_US_EASTERN;
const int   daylightOffset_sec = 3600;
const int max_drift_factor = 1;
//DS1302RTC Pin assignments 
const int kCePin   = 27; //Chip Enable (also known as RST)
const int kSclkPin = 12; // Serial Clock
const int kIoPin   = 14; // Input/Output


//Create a DS1302 RTC object.
Ds1302 rtc(kCePin, kSclkPin, kIoPin);

void WifiClock::init(){
    rtc.init();
    setDateTimeFromNTP();
}
//Return Date Time information from the DS1302 RTC
WifiClock::WifiClockInfo WifiClock::getWifiClockInfo(){
    //First compare both internal and external RTCs to find out if a resync with NTP is needed (check if there is drift in the RTCs)
    compareIntExtRTCs();
    Ds1302::DateTime rtcTime;
    rtc.getDateTime(&rtcTime);
    WifiClock::WifiClockInfo retVal;
    memcpy(&retVal,&rtcTime,sizeof(rtcTime));
    return retVal;   
}
//Generic function to extract a number from a specified part of a string
int WifiClock::extractNumFromString(char * inpStr,int firstChar, int numChar){
      char subbuff[numChar+1]; //need extra space at the end [+1] to hold the null terminator for the C String
      memcpy( subbuff, &inpStr[firstChar], numChar ); // string starts at first offset 0
      subbuff[numChar] = '\0'; // null terminator at end
      return atoi(subbuff);
    }
//Push date-time from internal/native RTC clock to external DS1302 RTC. Sync both RTCs
boolean WifiClock::convertNativeRTCDt2DS1302Dt(Ds1302::DateTime* DS1302TimeStruct){
        struct tm timeinfo;
        if(!getLocalTime(&timeinfo)){
                    return false;
        }
        char tempTime[50];
        strftime(tempTime,50,"%D %T %w",&timeinfo);

        DS1302TimeStruct->day = extractNumFromString(tempTime,3,2) ;
        DS1302TimeStruct->dow = extractNumFromString(tempTime,18,1) + 1;
        DS1302TimeStruct->hour = extractNumFromString(tempTime,9,2);
        DS1302TimeStruct->minute = extractNumFromString(tempTime,12,2);
        DS1302TimeStruct->month=extractNumFromString(tempTime,0,2);
        DS1302TimeStruct->second=extractNumFromString(tempTime,15,2) + 1;
        DS1302TimeStruct->year= extractNumFromString(tempTime,6,2);
        return true;
}

boolean WifiClock::setDateTimeFromNTP(){
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
            }
            // Init  time
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            struct tm timeinfo;
            
            if(!getLocalTime(&timeinfo)){
                    return false;
            }

            //disconnect WiFi as it's no longer needed
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            Ds1302::DateTime newDT;
            if(!convertNativeRTCDt2DS1302Dt(&newDT)) return false;
            rtc.halt();
            rtc.setDateTime(&newDT);     
            return true;
}
//Compare internal and external RTCs to detect drift in time keeping. If drift noticed, resync both from NTP
boolean WifiClock::compareIntExtRTCs(){
    Ds1302::DateTime extDT;
    Ds1302::DateTime intDT;
    if(!convertNativeRTCDt2DS1302Dt(&intDT)) return false;
    rtc.getDateTime(&extDT);
    if(!(intDT.year == extDT.year && 
         intDT.month == extDT.month && 
         intDT.day == extDT.day && 
         intDT.hour == extDT.hour && 
         abs(intDT.minute - extDT.minute) <= max_drift_factor))  //if internal and external RTC clocks 
                                                                 //diverge by more than pre defined 
                                                                 //maximum drift factor then reset time from NTP
            return setDateTimeFromNTP();
    return true;
}


