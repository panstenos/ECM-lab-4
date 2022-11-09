#include <xc.h>
#include "LCD.h"
#include <stdio.h>
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
        
    LCD_sendbyte(0b00000110,0);  // 0x06 Auto Increment cursor, shift display off

    LCD_sendbyte(0b00001110,0);  //Cursor on, blinking off
    LCD_sendbyte(0b00001100,0);  //Cursor off, blinking off
 
    //remember to turn the LCD display back on at the end of the initialisation (not in the data sheet)
    
    init_custom_chars(2);
}
 
void LCD_clear(void)
{
    __delay_us(40);
    LCD_sendbyte(0b00000001,0); //Set DB4-7 to 0 -> Clear screen
    
    __delay_ms(2);      //Delay 2ms    
    LCD_sendbyte(0b00000110,0);  // 0x06 Auto Increment cursor, shift display off
    LCD_sendbyte(0b00001110,0);  //Cursor on, blinking off
    LCD_sendbyte(0b00001100,0);  //Cursor off, blinking off
    
}

/************************************
 * Function to set the cursor to beginning of line 1 or 2
************************************/
void LCD_setline (char line)
{
    if(line == 1){
        LCD_sendbyte(0x80,0);   //Set cursor to line 1 (0x00 ddram address)
    }else{
        LCD_sendbyte(0xC0,0);    //Set cursor to line 2 (0x40 ddram address)
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
    max -= 15; //LCD has 16 bits length 
    if (max > 0){ //only if one of the two lines is more than 16 characters in length
        int i; //declare the integer
        for (i=0;i<max;i++){ //scroll right max characters; notice max -= 16
        LCD_sendbyte(0b00011000,0); //scroll right function
        //code here to scroll the text on the LCD screen
        __delay_ms(500); // delay 0.5 sec
        }
        for (i=0;i<max;i++){ //scroll back (left) max characters; notice max -= 16
        LCD_sendbyte(0b00011100,0); //scroll left function
        //code here to scroll the text on the LCD screen
        __delay_ms(500); // delay 0.5 sec
        }
    }
}
 
/************************************
 * Function takes a ADC value and works out the voltage to 2 dp
 * the result is stored in buf as ascii text ready for display on LCD
 * Note result is stored in a buffer using pointers, it is not sent to the LCD
************************************/
void ADC2String(char *buf, unsigned int ADC_val){
    int int_part;
    int frac_part;
    float num = 255/3.3;
    int_part = ADC_val/num;
    frac_part = (ADC_val*100)/num - int_part*100;
    sprintf(buf, "V = %u.%02u",int_part, frac_part);
            
}


unsigned int pos = 16;
unsigned int sprite_offset = 0;
void LCD_bbanimation(){
    for(int i = 0; i < 4; i ++){ // Show the top 3 parts of the sprite

    if(i + pos >= 0 && i + pos < 16){
        LCD_sendbyte(0x80 + i + pos,0);   //Set cursor to the right position
        if(i != 3){
            LCD_sendbyte(i,1);//Print the sprite part
        }else{
            LCD_sendbyte(6,1);//Print the sprite part
        }
    }
}

for(int i = 0;i < 4; i ++){ //Show the bottom 3 parts of the sprite
    if(i + pos >= 0 && i + pos < 16){
        LCD_sendbyte(0xC0 + i + pos,0); //Set the cursor at the right position
        if(i != 3){
        LCD_sendbyte(i + 3,1); //Print the sprite part
        }else{
            LCD_sendbyte(7,1);//Print the sprite part
        }
    }
}
    if(sprite_offset == 0){
        init_custom_chars(sprite_offset);
    }
    sprite_offset++;
    if(sprite_offset == 6){
        sprite_offset = 0;
        pos--;
    }
    if(pos == -4){
        pos = 16;
    }
    if(sprite_offset != 0){
        init_custom_chars(sprite_offset);
        __delay_ms(125);
    }
    }
    
    
    


void init_custom_chars(unsigned int offset){
    //Store 6 characters representing 6 part of the bulletbill
    
    
    // Code for first row first column bullet bill
    int line00 = 0b00000;
    int line01 = 0b00000;
    int line02 = 0b00001;
    int line03 = 0b00111;
    int line04 = 0b01110;
    int line05 = 0b01100;
    int line06 = 0b10100;
    int line07 = 0b10001;

    int lines0[] = {line00,line01,line02,line03,line04,line05,line06,line07};
    
    for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01000000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = lines0[i] >> 5 - offset;
        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    }
    
    // Code for first row second column bullet bill part

    int line10 = 0b00000;
    int line11 = 0b01111;
    int line12 = 0b11000;
    int line13 = 0b10111;
    int line14 = 0b11111;
    int line15 = 0b11111;
    int line16 = 0b11111;
    int line17 = 0b11110;

    int lines1[] = {line10,line11,line12,line13,line14,line15,line16,line17};
    
    for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01001000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = ((lines0[i] << offset) + (lines1[i] >> 5 - offset));
        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    }
    
    // Code for first row third column bullet bill part

    
    int line20 = 0b00000;
    int line21 = 0b11001;
    int line22 = 0b00110;
    int line23 = 0b11101;
    int line24 = 0b11111;
    int line25 = 0b11111;
    int line26 = 0b11111;
    int line27 = 0b01111;

    int lines2[] = {line20,line21,line22,line23,line24,line25,line26,line27};
    
    for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01010000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = ((lines1[i] << offset) + (lines2[i] >> 5 - offset));
        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    } 
    
    // Code for second row first column bullet bill part
    int line30 = 0b11111;
    int line31 = 0b11111;
    int line32 = 0b01111;
    int line33 = 0b01111;
    int line34 = 0b00111;
    int line35 = 0b00001;
    int line36 = 0b00000;
    int line37 = 0b00000;

    int lines3[] = {line30,line31,line32,line33,line34,line35,line36,line37};
    
    for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01011000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = lines3[i] >> 5 - offset;

        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    }
    
    // Code for second row second column bullet bill part
    int line40 = 0b10010;
    int line41 = 0b00000;
    int line42 = 0b00000;
    int line43 = 0b10001;
    int line44 = 0b11111;
    int line45 = 0b11111;
    int line46 = 0b00111;
    int line47 = 0b00000;

    int lines4[] = {line40,line41,line42,line43,line44,line45,line46,line47};
    
    for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01100000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = ((lines3[i] << offset) + (lines4[i] >> 5 - offset));

        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    }
    
    // Code for second row third column bullet bill part
    int line50 = 0b00111;
    int line51 = 0b00111;
    int line52 = 0b01111;
    int line53 = 0b11111;
    int line54 = 0b11111;
    int line55 = 0b11111;
    int line56 = 0b10011;
    int line57 = 0b00000;

    int lines5[] = {line50,line51,line52,line53,line54,line55,line56,line57};
    
    for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01101000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = ((lines4[i] << offset) + (lines5[i] >> 5 - offset));

        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    }
    
    
    
    
    
    for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01110000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = (lines2[i] << offset);
        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    } 
     for(unsigned int i = 0; i < 8; i++){
        unsigned int adress = 0b01111000 + i;
        LCD_sendbyte(adress,0);
        __delay_us(40);      //Delay 40uS
        int offset_line = (lines5[i] << offset);
        LCD_sendbyte(offset_line,1);
        __delay_us(40);
    } 
    
    
}


    




