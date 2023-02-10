#ifndef LED5604DISPLAY_H
#define LED5604DISPLAY_H
#include <SPI.h>
#include <Arduino.h>
#define LED5604_SCK 18
#define LED5604_MISO 19
#define LED5604_MOSI 23
#define LED5604_SS 5

class LED5604Display
{
    private:
        void showDigit(int value);
        int  addDecimalPoint(int inputChar);
        void displayDigits(int originalNumber, bool addUnitPlaceDecimal);
        void displayNumber(int inputNumber, int maxDigits, bool addUnitPlaceDecimal);
    
    public:
        void init();
        void displayInit();
        void displayDay(uint8_t day_of_week);
        void displayDate(uint8_t month_val, uint8_t day_val);
        void displayYear(uint16_t year_val);
        void displayTime(uint8_t hour, uint8_t minute, uint8_t secs);
        void displayError(uint8_t captionChar);
};

#endif //LED5604DISPLAY_H