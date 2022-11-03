#include <xc.h>
#include "LCD.h"

/************************************
 * Function to toggle LCD enable bit on then off
 * when this function is called the LCD screen reads the data lines
************************************/
void LCD_E_TOG(void)
{
    //turn the LCD enable bit on
    LATCbits.LATC2 = 1;
    __delay_us(2); //wait a short delay
    //turn the LCD enable bit off again
    LATCbits.LATC2 = 0;
 
}
 
/************************************
 * Function to set the 4-bit data line levels for the LCD
************************************/
void LCD_sendnibble(unsigned char number)
{
    
    //set the data lines here (think back to LED array output)
    if(number & 0b0001){ LCD_DB4 = 1;}else{LCD_DB4 = 0;}
    if(number & 0b0010){ LCD_DB5 = 1;}else{LCD_DB5 = 0;}
    if(number & 0b0100){ LCD_DB6 = 1;}else{LCD_DB6 = 0;}
    if(number & 0b1000){ LCD_DB7 = 1;}else{LCD_DB7 = 0;}
        
    LCD_E_TOG();            //toggle the enable bit to instruct the LCD to read the data lines
    __delay_us(5);      //Delay 5uS
}
 
 
/************************************
 * Function to send full 8-bit commands/data over the 4-bit interface
 * high nibble (4 most significant bits) are sent first, then low nibble sent
************************************/
void LCD_sendbyte(unsigned char Byte, char type)
{
    
    LCD_RS = type; // set RS pin whether it is a Command (0) or Data/Char (1) using type argument
    LCD_sendnibble(Byte >> 4); // send high bits of Byte using LCDout function
    LCD_sendnibble(Byte);// send low bits of Byte using LCDout function
 
    __delay_us(50);               //delay 50uS (minimum for command to execute)
}
 
/************************************
 * Function to initialise the LCD after power on
************************************/
void LCD_Init(void)
{
 
    //Define LCD Pins as Outputs and
    //set all pins low (might be random values on start up, fixes lots of issues)
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC6 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB2 = 0;
    TRISEbits.TRISE3 = 0;
    TRISEbits.TRISE1 = 0;
    
    // initialise LCD output -> set ports below to 0
    LCD_RS = 0;
    LCD_E = 0;
    LCD_DB4 = 0;
    LCD_DB5 = 0;
    LCD_DB6 = 0;
    LCD_DB7 = 0;
    
    __delay_ms(60);      //Delay 60mS
     
    LCD_sendnibble(0b0011);
    __delay_us(40);      //Delay 40uS
 
    LCD_sendnibble(0b0010);
    __delay_us(40);      //Delay 40uS
 
    LCD_sendbyte(0b00101000,0); //turn on DB5
    __delay_us(40);      //Delay 40uS
 
    LCD_sendbyte(0b00101000,0); //enable DB5
    __delay_us(40);      //Delay 40uS
    
    LCD_sendbyte(0b00000001,0); //Set DB4-7 to 0 -> Clear screen
    __delay_ms(2);      //Delay 2ms
    
    LCD_sendbyte(0b00001111,0); //DB6=1; I/D=1; SH=1 -> Enable shifting of entire display
    __delay_ms(2);      //Delay 2ms
 
    //remember to turn the LCD display back on at the end of the initialisation (not in the data sheet)
}
 
/************************************
 * Function to set the cursor to beginning of line 1 or 2
************************************/
void LCD_setline (char line)
{
    if(line == 1){
        LCD_sendbyte(0x80,0);   //Send 0x80 to set line to 1 (0x00 ddram address)
    }else{
        LCD_sendbyte(0xC0,0);    //Send 0xC0 to set line to 2 (0x40 ddram address)
    }
}
 
/************************************
 * Function to send string to LCD screen
************************************/
void LCD_sendstring(char *string1, char *string2) //input two strings
{
    LCD_setline(1); //write first input in the first line
    while(*string1 !=0){
        LCD_sendbyte(*string1++,1);
    }    
    LCD_setline(2); //write second input in the second line
    while(*string2 !=0){
        LCD_sendbyte(*string2++,1);
    }
}
 
/************************************
 * Function to send string to LCD screen
************************************/
void LCD_scroll(int max) //scrolls back and forth; input maximum length among the two strings
{
    max -= 16; //LCD has 16 bits length 
    int i; //declare the integer
    for (i=0;i<max;i++){ //repeat the loop 
    LCD_sendbyte(0b00011000,0); //Turn on DB3
    //code here to scroll the text on the LCD screen
    __delay_ms(500);
    }
    for (i=0;i<max;i++){
    LCD_sendbyte(0b00011100,0); //Turn on DB3
    //code here to scroll the text on the LCD screen
    __delay_ms(500);
    }
    __delay_ms(200); //call initial delay function
}
 
/************************************
 * Function takes a ADC value and works out the voltage to 2 dp
 * the result is stored in buf as ascii text ready for display on LCD
 * Note result is stored in a buffer using pointers, it is not sent to the LCD
************************************/
void ADC2String(char *buf, unsigned int ADC_val){
    //code to calculate the inegeter and fractions part of a ADC value
    // and format as a string using sprintf (see GitHub readme)
}
