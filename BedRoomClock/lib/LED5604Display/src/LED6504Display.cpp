#include "LED5604Display.h"
//Slave select for the LED5604 module
const int slaveSelectPin = 5;
const uint8_t digit_bin [10]={192,249,164,176,153,146,131,248,128,152}; //Array of binary equivalents of digits 0 through 9
const uint8_t all_blank = 0b11111111; //All LED segments off
const uint8_t char_s = 146; //S and 5 are the same for display
const uint8_t char_u = 0b11000001;
const uint8_t char_n = 0b11001000;
const uint8_t char_m = 0b11101010;
const uint8_t char_o = 192; //o and 0 are the same for display
const uint8_t char_tee = 0b10000111;
const uint8_t char_e = 0b10000110;
const uint8_t char_w = 0b11010101;
const uint8_t char_d = 0b10100001;
const uint8_t char_h = 0b10001011;
const uint8_t char_f = 0b10001110;
const uint8_t char_r = 0b11001100;
const uint8_t char_i = 0b11001111;
const uint8_t char_a = 0b10001000;
const uint8_t char_p = 0b10001100;
const uint8_t day_bin [7] [3] =  {
                                   { char_s, char_u, char_n},      /*Sunday*/
                                   { char_m, char_o, char_n},      /*Monday*/
                                   { char_tee, char_u, char_e},    /*Tuesday*/
                                   { char_w, char_e, char_d},      /*Wednesday*/
                                   { char_tee, char_h, char_u},    /*Thursday*/
                                   { char_f, char_r, char_i},      /*Friday*/
                                   { char_s, char_a, char_tee}     /*Saturday*/
                                  };

const uint8_t decPoint = 0b01111111; //Left most bit is used to turn the decimal point on '0'

uint8_t digitsDisplayed; //count of how many digits have been displayed so far

void LED5604Display::init(){
    // set the slaveSelectPin as an output:
    pinMode(slaveSelectPin, OUTPUT);
    // initialize SPI for LED5604 modules:
    SPI.begin(LED5604_SCK, LED5604_MISO, LED5604_MOSI, LED5604_SS);
}

//Low Level Function to actually write a single digit using SPI
void LED5604Display::showDigit(int value) {
    // take the SS pin low to select the chip:
        digitalWrite(slaveSelectPin, LOW);
    //  send in the address and value via SPI:
        SPI.transfer(value);
    // take the SS pin high to de-select the chip:
        digitalWrite(slaveSelectPin, HIGH);
}

int LED5604Display::addDecimalPoint(int inputChar){
    return inputChar & decPoint;
}

void LED5604Display::displayDigits(int originalNumber, bool addUnitPlaceDecimal){
    int displayDig = originalNumber % 10;
    int displayCode = digit_bin[displayDig];  
    showDigit( addUnitPlaceDecimal ? addDecimalPoint(displayCode) : displayCode);
    digitsDisplayed++;
    originalNumber = originalNumber / 10;
    if(originalNumber > 0) displayDigits(originalNumber,false); // Recurse into this function 
                                                                // to show the next digit in 
                                                                // line to the left untill all 
                                                                // digits are exhausted
}

void LED5604Display::displayNumber(int inputNumber, int maxDigits, bool addUnitPlaceDecimal){
    digitsDisplayed = 0;
    displayDigits(inputNumber, addUnitPlaceDecimal);
    for (int i=digitsDisplayed; i<maxDigits; i++) { //Pad leading zeroes to fill up max digit display expectation
            showDigit(digit_bin[0]);
  }   
  
}

void LED5604Display::displayTime(uint8_t hour, uint8_t minute, uint8_t secs){
    displayNumber(minute, 2, false);
    displayNumber(hour, 2, secs % 2 == 0); //Blink seconds dot every other second
}

void LED5604Display::displayDay(uint8_t day_of_week){
    uint8_t day_bin_offset = day_of_week - 1;
    // Read day array row backward as display is populated last char first
    for (int day_char_pos=2; day_char_pos>=0; day_char_pos--) 
        showDigit(day_bin[day_bin_offset][day_char_pos]);
  
    showDigit(all_blank);
}

void LED5604Display::displayDate(uint8_t month_val, uint8_t day_val){
    displayNumber(day_val, 2, false);
    displayNumber(month_val, 2, true); //Display a dot after month to separate the month and day values clearly
}

void LED5604Display::displayYear(uint16_t year_val){
    if(year_val < 100) year_val = 2000 + year_val;
    displayNumber(year_val, 4, false);
}

void LED5604Display::displayError(uint8_t captionChar){
    showDigit(char_r); 
    showDigit(char_r);
    showDigit(char_e);  
    showDigit(captionChar);
}

void LED5604Display::displayInit(){
    //Show a "wifiinit" message
    showDigit(char_tee); 
    showDigit(char_i);
    showDigit(char_n);  
    showDigit(char_i);
    showDigit(char_i); 
    showDigit(char_f);
    showDigit(char_i);  
    showDigit(char_w);
}