#include <WiFiClock.h>
#include <LED5604Display.h>

//How many different types of unique displays ?
const int numUniqueDisplays = 3;

const uint8_t secondaryDisplayMaxPersistence=5; //Items on secondary display stay there for 5 seconds
uint8_t secondaryDisplayPersistenceCounter=1;
uint8_t displaySequence=1; //Initialize displaySequence to first display

//Initialize LED5604Display
LED5604Display myLED5604Display;
//Initialize WifiClock
WifiClock myClock;

void setup() {
    myLED5604Display.init();
    myLED5604Display.displayInit();
    myClock.init();
}

void loop() {
  WifiClock::WifiClockInfo myClockInfo;
  myClockInfo = myClock.getWifiClockInfo();
    
  switch(displaySequence){
      case 1: myLED5604Display.displayDay(myClockInfo.dow); break;
      case 2: myLED5604Display.displayDate(myClockInfo.month,myClockInfo.day); break;
      case 3: myLED5604Display.displayYear(myClockInfo.year); break; 
      //case 4: myLED5604Display.displayTemp(); break;
      //case 5: myLED5604Display.displayHumidity(); break;
  }

  secondaryDisplayPersistenceCounter++;
  if(secondaryDisplayPersistenceCounter > secondaryDisplayMaxPersistence) {
         displaySequence++;
         if(displaySequence > numUniqueDisplays) displaySequence=1; //Loop back to first display in sequence
         secondaryDisplayPersistenceCounter=1;
  }
  myLED5604Display.displayTime(myClockInfo.hour,myClockInfo.minute,myClockInfo.second);
  delay(1000);
}