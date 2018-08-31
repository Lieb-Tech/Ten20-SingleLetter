# ten20 
A first step in a project to display text and animations across a 20x10 array of NeoPixels.

## Proof of concept
The final project will have many more moving pieces, so the is first step in that direction. This is my first time using the WS2812b pixels, so I started with just flashing 1 letter at a time. 

* Microcontroller - I had a Arduino Micro (small form factor version of the [Leonardo] (https://store.arduino.cc/usa/arduino-leonardo-with-headers) laying around so I started with that. After working with PCs with GBs of RAM, it was fun to work on shrinking my running code to fit into 2.5 KB of SRAM.

* LEDs - Just a generic WS2812B 5 meter strip with 60leds/M. I cut them down to 20 pixel lengths, and soldered on 3 pin JSTs connectors. Using thse connectors, as opposed to hardwiring the strips together, make it easier to test my soldering, as well as makes is easier to handle them. For this project, the 60/m strips are good balance of pixel spacing and cost.

* Power - To start off, the strips are being powered by the Arduino, via USB. The plan for the future is to have a seperate higher amperage power supply. I have some older 300w PC power supplies that I will use.

## Code overview
* Character definitions - I've written a simple C# console app (Gihub repo here) to generate the character defintions, allowing me to try different formats to attempt to reduce my code's SRAM footprint. My current codebase is a uint32_t representation of 6 row x variable width array. It uses up to 30 bits of the 32 bit value to store on/off settings for specific pixel location. 

* Sentence - This array holds the structs of the letters to be displayed. Due to the small memory space of the Micro, I've had to limit this to 20. Migrating this project to a MCU with greater SRAM will allow this value to increase greatly.

* Loop - Simply takes the next character in the array, uses bitwise and bitshifting to determine which led to turn on or off. This first iteration is just single color, future versions will allow for colors - that will require a MCU with more SRAM than the Micro. Using the excellent Adafruit library to handle the display functionality.
