#include <Adafruit_NeoPixel.h>

#define DEBUG
#ifdef __AVR__
#include <avr/power.h>
#endif

#ifdef DEBUG
 #define DEBUG_PRINTLN(x)  Serial.println (x) 
 #define DEBUG_PRINT(x)  Serial.print (x)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
#endif

// time between display updates
#define WAIT 5000
// Pixels across 
#define WIDTH 20
// Pixels up/down
#define HEIGHT 6
// Arduino pin for display data
#define PIN 6

#define TOTALLED 60

// max number of letters to display
#define SENTENCELENGTH 20
// max # pixels wide in letter
#define LETTERWIDTH 5

// character in alphabet
typedef struct {
    uint32_t pixelData;
    byte width;    
    byte special;
    char val; // for debug
} character;

/// CHANGE TO ARRAY
character charA, charB, charC, charD, charE, charH, charL, charO, charR, charT, charN, charM, charAMPER, charSPACE, charCOMMA, charPERIOD, charEXCLAIM, charEOL;
// character alphabet[70]; 
// A-Z 0-26
// a-z 27-52
// 0-9 53-62
// space, period, exclaim, comma, hashtag, dash, colon, slash, at? 63-70

// led display driver
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTALLED, PIN, NEO_GRB + NEO_KHZ800);

// default for LED ON / off
uint32_t ON = strip.Color(255, 0, 0);   // pixel on = red (eventually allow words to have different colors)
uint32_t OFF = strip.Color(0, 0, 0);    // pixel off 

// alphabet converted into on/off matrix
bool ledData[HEIGHT][(SENTENCELENGTH * LETTERWIDTH)];

// current index in ledData going left to right
uint8_t currentIndex; 
// last piece of parsed data in array (array might be longer than configured data
uint8_t maxIndex; 

uint8_t lettersInSentence; 

// holds characters to show in display
character sentence[SENTENCELENGTH];

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  // startup serial for debugging
  #ifdef DEBUG
  Serial.begin(115200);             
  // wait for serial port to open on native usb devices
  while (!Serial) {
    delay(1);
  }
  #endif
  
  DEBUG_PRINTLN("build alphabet");
  buildChars();
  
  DEBUG_PRINTLN("load sentence");
  sentence[0] = charH;
  sentence[1] = charSPACE;
  sentence[2] = charE;
  sentence[3] = charSPACE;
  sentence[4] = charL;
  sentence[5] = charSPACE;
  sentence[6] = charL;
  sentence[7] = charSPACE;
  sentence[8] = charO;
  sentence[9] = charEOL;

  lettersInSentence = 10;
  /*
  sentence[11] = charSPACE;
  sentence[12] = charC;
  sentence[13] = charSPACE;
  sentence[14] = charA;
  sentence[15] = charSPACE;
  sentence[16] = charR;
  sentence[17] = charSPACE;
  sentence[18] = charO;
  sentence[19] = charSPACE;
  sentence[20] = charL;
  sentence[21] = charEOL;     
  */
  
  DEBUG_PRINTLN("Parse Sentence");
  // parseSentence();

  DEBUG_PRINTLN("start strip");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  currentIndex = 0;
  DEBUG_PRINTLN("Ready to go");
  DEBUG_PRINT("WIDTH: ");
  DEBUG_PRINTLN(WIDTH);
  DEBUG_PRINT("HEIGHT: ");
  DEBUG_PRINTLN(HEIGHT);

  DEBUG_PRINT("Total leds: ");
  DEBUG_PRINTLN(TOTALLED);
}

void loop() {
  // flash each character in sentence
  DEBUG_PRINT("loop -> current of max: ");
  DEBUG_PRINT(currentIndex);
  DEBUG_PRINT(" of ");
  DEBUG_PRINTLN(lettersInSentence);
  
  // while there's more data to show 
  if (currentIndex <= lettersInSentence) {
    
    character cur = sentence[currentIndex];
    
    DEBUG_PRINT("Character: ");
    DEBUG_PRINT(cur.val);
    
    for (int r = 0; r < HEIGHT * WIDTH; r++) 
    {
      if (r < TOTALLED)
        strip.setPixelColor(r, OFF);
    }
    
    for (int r = 0; r < HEIGHT; r++) 
    {
      DEBUG_PRINTLN("");
      DEBUG_PRINT("r: ");
      DEBUG_PRINT(r);
      DEBUG_PRINT(" ");
      
      for (int c = 0; c < cur.width ; c++) 
      {
        DEBUG_PRINTLN("");
        DEBUG_PRINT("c: ");
        DEBUG_PRINT(c);

        uint32_t shift1 = (r * cur.width);
        uint32_t shifted = shift1 + c;
        unsigned long offset = (unsigned long)1 << shifted;
        
        DEBUG_PRINT(" ("); 
        DEBUG_PRINT(shift1);
        DEBUG_PRINT(" + ");
        DEBUG_PRINT(c);
        DEBUG_PRINT("=");
        
        DEBUG_PRINT(shifted);       
        DEBUG_PRINT(") offset: ");
        DEBUG_PRINT(offset);
        
        bool isOn = (cur.pixelData & offset) == offset;
        DEBUG_PRINT(" - on");
        DEBUG_PRINT(isOn);        
        DEBUG_PRINT(" ; pos = ");
        int pos = (r * WIDTH) + c;
        DEBUG_PRINT(pos);
        if (isOn) {          
          //if (shifted < TOTALLED) 
            strip.setPixelColor(pos, ON);
         } 
      }
    }
    
    strip.show();    
  }
  DEBUG_PRINTLN("");

  if ((currentIndex % 2) == 0) 
    strip.setPixelColor(20, OFF);
  else 
    strip.setPixelColor(20, ON);

  currentIndex++;
  
  delay(WAIT);
}
/////////////////////////////////////////////////////////////////////

// Take the letters and build he pixel maps
void parseSentence() {
  uint8_t idx = 0;
  uint8_t row = HEIGHT; 
  uint8_t comp = 0;

  DEBUG_PRINTLN("..parse");  
  
  // skip the width of output to prepend blanks, so text will scroll in from the right
  maxIndex = WIDTH;

  // got through character by character until hit limit or the EOL 
  while (idx < SENTENCELENGTH && sentence[idx].special != 2) {    
    
    DEBUG_PRINT("....");
    DEBUG_PRINT(idx);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(sentence[idx].val);

    // start with bottom row      
    row = HEIGHT;
    
    // column by column - starting at left most, move right for .width pixels
    for (int col = 5; col > (5 - sentence[idx].width); col--)
    { 
      // increate max width (for when to stop (or restart) the scroll)     
      maxIndex++;      
            
      // for each row in the pixel matrix
      for (int dataRow = 0; dataRow < 31; dataRow+=5)
      {
        // shift the number for comparison below to see if pixel is on or off
        comp = 1 << (col + dataRow);   

        // set LED bit to correspond to the character pixel status
        if ((sentence[idx].pixelData & comp) == comp)
          ledData[row][col] = 1 << maxIndex;
      }
      row--;
    } 
    idx++;   
  }  

  DEBUG_PRINT("..max index = ");
  DEBUG_PRINTLN(maxIndex);
}


/////////////////////////////////////////////////////////////////////
void buildChars() {
 // charA.pixelData = 9747750;
  charA.pixelData = 630678;
  charA.width = 4;
  charA.val = 'A';  
  
  charB.pixelData = 7642407;
  charB.width = 4;
  charB.val = 'B';
  
  charC.pixelData = 14713902;
  charC.width = 4;
  charC.val = 'C';

  charE.pixelData = 988959;
  // charE.pixelData = 15768623;
  charE.width = 4;
  charE.val = 'E';

  charH.pixelData = 630681;
  // charH.pixelData = 9747753;
  charH.width = 4;
  charH.val = 'H';

  charL.pixelData = 29257;
  // charL.pixelData = 15762465;
  // charL.width = 4;
  charL.width = 3;
  charL.val = 'L';

  charO.pixelData = 432534;
  // charO.pixelData = 6595878;
  charO.width = 4;
  charO.val = 'O';
        
/*
  bool tmpR[6][5] = {
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 0, 0, 0}
    };
  memcpy(tmpR, charR.pixelData, sizeOf);
  charO.width = 4;

  bool tmpT[6][5] = {
        {1, 1, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
  memcpy(tmpT, charT.pixelData, sizeOf);
  charT.width = 3;

  bool tmpN[6][5] = {
        {1, 0, 0, 0, 1},
        {1, 1, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0}
    };
  memcpy(tmpN, charN.pixelData, sizeOf);
  charN.width = 5;
  
  bool tmpM[6][5] = {
        {1, 0, 0, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0}
    };
  memcpy(tmpM, charM.pixelData, sizeOf);
  charM.width = 5;
*/
////// 

  charSPACE.pixelData = 0;
  charSPACE.width = 2;
  charSPACE.val = '_';
        // {0, 0, 0, 0, 0},
        // {0, 0, 0, 0, 0},
        // {0, 0, 0, 0, 0},
        // {0, 0, 0, 0, 0},
        // {0, 0, 0, 0, 0},
        // {0, 0, 0, 0, 0}

  charEOL.pixelData = 0;
  charEOL.val = '^';
  charEOL.special = 1; 
  
/*
  bool tmpAMPER[6][5] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0}
    };
  memcpy(tmpAMPER, charAMPER.pixelData, sizeOf);
  charAMPER.width = 5;

  bool tmpEXCLAIM[6][5] = {
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
  memcpy(tmpEXCLAIM, charEXCLAIM.pixelData, sizeOf);
  charEXCLAIM.width = 3;
  
  bool tmpPERIOD[6][5] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
  memcpy(tmpPERIOD, charPERIOD.pixelData, sizeOf);
  charPERIOD.width = 3;
  
  bool tmpCOMMA[6][5] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0}
    };
  memcpy(tmpCOMMA, charCOMMA.pixelData, sizeOf);
  charCOMMA.width = 3;
  */      
}


// code from here:
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both#_=_
