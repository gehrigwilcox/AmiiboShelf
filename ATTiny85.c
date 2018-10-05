/*
  Amiibo Slot Controller
  Author: Gehrig Wilcox
  06/04/18

  ATTiny85
  Switches 2 16-Channel MUXs, reads value of photodiode, and alerts raspberry pi
    via I2C if slot needs to be investigated


    Pin 0:  N/C
    Pin 1:  N/C
    Pin 2:  Analog Input
    Pin 3:  I2C Data
    Pin 4:  I2C Clock
    Pin 5:  Reset


*/

#include <avr/io.h> //GPIO Library
#include <avr/interrupt.h> //Interrupt Library
#include <core_adc.h> //ADC Library
#include <Wire.h> //I2C Library


#define SHELVES 16 //How many shelves exist
#define SLOTS 16  //Slots on each shelf


enum Device {RPI=0x00,NFC=0x01,MUX1=0x02,MUX2=0x03};



/*****    I2C Functions   *****/


void I2C_On_Recieve(){

}

void I2C_On_Request(){

}

void I2C_Communicate(enum Device reciever, char[] data){
  Wire.beginTransmission(reciever);
  for(int i = 0; i < sizeof(data)/sizeof(data[0]); i++){
    //Do I need to check the clock to make sure slave is ready for next byte?
    Wire.write(data[i]);
  }
  Wire.endTransmission();
}

void I2C_Set_Clock_Speed(int hertz){
  Wire.setClock(hertz);
}




int main(){



  /*****    VARIABLES   *****/


    /*
      16 x 16 slots (2 bytes = 16 bits)

      Keeps the last known status of each slot
      bit is set to 1 if it is occupied, set to 0 otherwise
    */
    char spaces[SHELVES][SLOTS/8];



  /*****   CONFIG    *****/

    DDRB = 0x04;  //Configure portB pin 2 as input, rest as output


  /*****    LOGIC     *****/

    //Infinite loop
    while(1){

      //For every shelf
      for(int i = 0; i < SHELVES; i++){
        //For every slot byte
        for(int j = 0; j < SLOTS/8; j++){
          //For every slot
          for(int k = 0; k < 8; k++){

            /*

              Use I2C to tell MUX 1 to address i
              Use I2C to tell MUX 2 to address 8*j + k

            */

            I2C_Communicate(Device.MUX1, {i});
            I2C_Communicate(Device.MUX2, {8*j+k});


            char currentSlot = /* Read Pin 2 */;

            //If the slot has changed states
            if( (spaces[i][j] >> k & 1) ^ (currentSlot & 1) ){

              //Update Spaces (Flip current bit)
              spaces[i][j] ^= 1 << k;


              /*
                Tell RPi through I2C to read shelf i, slot 8*j + k
              */
              I2C_Communicate(Device.RPI, {i,8*j+k});

              //Wait for response from RPi
              while(1){
                if(/* UPDATE FROM PI */) break;
              }
            }
          }
        }
      }



    }

    //Inaccessable
    return 0;

}
