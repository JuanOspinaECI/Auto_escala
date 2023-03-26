/*
 * File:   led_main.c
 * Author: Usuario
 *
 * Created on March 20, 2023, 12:18 AM
 */

// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = XT    // Oscillator Selection bits (RC oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

//#pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <math.h>
#include <stdio.h>
//#include "math24f.h"
//#include "math24lb.h"
#define _XTAL_FREQ 4000000
#define SBIT_ADON     0
#define SBIT_CHS0     3
#define SBIT_ADFM     7
#define rs RC0
#define rw RC1
#define en RC2

char cadena[15];

void lcd_init();
void cmd(unsigned char a);
void dat(unsigned char b);
void show(unsigned char *s);

unsigned char num_s(int val, char flag){
    switch(val){
        case 1:
            return '1';
            break;
        case 2:
            return '2';
            break;
        case 3:
            return '3';
            break;
        case 4:
            return '4';
            break;
        case 5:
            return '5';
            break;
        case 6:
            return '6';
            break;
        case 7:
            return '7';
            break;
        case 8:
            return '8';
            break;
        case 9:
            return '9';
            break;
        default:
            if (flag == 0){return ' ';}
            else{return '0';}    
    }
}

void float_LCD(float value){
    char flag = 0;
    int v_temp;
    v_temp = (int)(value/1000);
    cadena[0] = num_s(v_temp,flag);
    if(v_temp != 0){flag = 1;}
    value -= (float)v_temp*1000;
    
    v_temp = (int)(value/100);
    cadena[1] = num_s(v_temp,flag);
    if(v_temp != 0){flag = 1;}
    value -= (float)v_temp*100;
    
    v_temp = (int)(value/10);
    cadena[2] = num_s(v_temp,flag);
    if(v_temp != 0){flag = 1;}
    value -= (float)v_temp*10;
    
    v_temp = (int)(value);
    cadena[3] = num_s(v_temp,1);
    value -= (float)v_temp;
    
    cadena[4] = '.';
    value = value *1000;
    
    v_temp = (int)(value/100);
    cadena[5] = num_s(v_temp,1);
    value -= (float)v_temp*100;
    
    v_temp = (int)(value/10);
    if (v_temp > 9){return;}
    cadena[6] = num_s(v_temp,1);
    value -= (float)v_temp*10;
    
    v_temp = (int)(value);
    cadena[7] = num_s(v_temp,1);
    value -= (float)v_temp;
}

void delay(int cnt)
{
    while(cnt--);
}
void lcd_init()
{
    cmd(0x38);
    cmd(0x0c);
    cmd(0x06);
    cmd(0x80);
}

void cmd(unsigned char a)
{
    PORTB=a;
    rs=0;
    rw=0;
    en=1;
    delay(1000);
    en=0;
}

void dat(unsigned char b)
{
    PORTB=b;
    rs=1;
    rw=0;
    en=1;
    delay(1000);
    en=0;
}

void show(unsigned char *s)
{
    while(*s) {
        dat(*s++);
    }
}

void ADC_Init()
 {
   ADCON0=0x00;  // sampling freq=osc_freq/2,ADC off initially
   ADCON1=(1<<SBIT_ADFM);  // All pins are configured as Analog pins and ADC result is right justified  
}


int ADC_Read(int adcChannel)
 {  
    ADCON0 = (1<<SBIT_ADON) | (adcChannel<SBIT_CHS0);  //select required channel and turn ON adc

    delay(1000);                   //Acquisition Time(Wait for Charge Hold Capacitor to get charged )
   
    GO=1;                           // Start ADC conversion
    while(GO_DONE==1);              // Wait for the conversion to complete
                                    // GO_DONE bit will be cleared once conversion is complete

    return((ADRESH<<8) + ADRESL);   // return right justified 10-bit result
 }


int main()
{
    int adcValue=0;
    
    TRISB = 0x00; // Configure PORTB and PORTD as output to display the ADC values on LEDs
    TRISD = 0x00;
    TRISC0=TRISC1=TRISC2=0;
    

    ADC_Init();             //Initialize the ADC module
    char count = 0;
    char c2 = 31;
    float suma = 0;
    float val=0;
    int m = 1;
    float prom_1;
    float total = 0;
    unsigned int i;
    lcd_init();
    cmd(0x8A); //forcing the cursor at 0x8A position
    show("IDIE");
    while(1)
    {
        __delay_ms(0.416);
        //cmd(0x18);
        //cmd(0x01);
        //cmd(0x80);
        //float a = 1010.999;
        //sprintf(c, "%f", a);
        //float_LCD(a);
        //show(cadena);
        adcValue = ADC_Read(0);       // Read the ADC value of channel zero
        count++;
        //for (int j = 0; j<10; j++){
        //        int temp = adcValue & m;
        //        m = m*2;
        //        val += temp;
        //    }
        val = (((float)adcValue*5/1024)-2.5);
        val = val*val*0.000416;
        //float_LCD(adcValue*5/1024);
        //float_LCD(val);
        //show(cadena);
        suma += val;
        
        if (count == 40){
            count = 0;
            val = 0;
            m = 1;
            
            total = suma*60;
            total = sqrt(total)*70.71;
            prom_1 += total;
            if(c2 == 31){
                cmd(0x01);
                cmd(0x80);
                float_LCD(total);
                show(cadena);
                c2 = 0;
            }
            if (c2 == 30 ){
                cmd(0x01);
                cmd(0x80);
                prom_1 = prom_1/30;
                float_LCD(total);
                show(cadena);
                c2 = 0;
            }
            c2++;
            suma = 0;    
            __delay_ms(0.0266666);
        }
        
        //PORTB = (adcValue & 0xff);    //Adc value displayed on LEDs connected to PORTB,PORTD
        //PORTD = (adcValue>>8) & 0x03; // PORTB will display lower 8-bits and PORTD higher 2-bits
    }
}