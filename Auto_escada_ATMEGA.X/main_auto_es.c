/*
 * File:   main_auto_es.c
 * Author: Usuario
 *
 * Created on 16 de abril de 2023, 07:15 PM
 */


#include <xc.h>
//#include <ADC.h>
#include <math.h>
#include <stdio.h>
#define F_CPU 16000000UL
//#define _XTAL_FREQ 16000000
//CLKPR = 0x00;
#include <avr/io.h>
#include <util/delay.h>
#define LCD_Port PORTB			//Define LCD Port (PORTA, PORTB, PORTC, PORTD)
#define LCD_DPin  DDRC			//Define 4-Bit Pins (PD4-PD7 at PORT D)
#define RSPIN PC0			//RS Pin
#define ENPIN PC1 			//E Pin
#define rs PC0
#define rw PC1
#define en PC2

//Global variables
char cadena[15]; //Variable for conversion float to string


//Functions
unsigned char num_s(int val, char flag);
void float_LCD(float value);
void delay(int cnt);
void lcd_init();
void cmd(unsigned char a);
void dat(unsigned char b);
void show(const char *s);
void ADC_Init();
int ADC_Read(int adcChannel);


void ADC_init_0(void)
{

    ADMUX = 0b00000000;
    ADCSRA = 0b10000000;
}
void ADC_init_1(void)
{

    ADMUX = 0b00000001;
    ADCSRA = 0b10000000;
}

uint16_t ADC_read(uint8_t canal)
{
    //ADCSRA = 0b01000000;
	canal&=0b00001111;				//Limitar la entrada a 5
	ADMUX = (ADMUX & 0xF0)|canal;  //Limpiar los últimos 4 bits de ADMUX, OR con ch
	ADCSRA|=(1<<ADSC);				//Inicia la conversión
	while((ADCSRA)&(1<<ADSC));		//Hasta que se complete la coversión
	//_delay_ms(0.1);
    //return(ADCH<<2 | ADCL>>6);	
    return(ADC);	//devuelve el valor del adc
}
/*uint16_t ADC_read(uint8_t canal)
{
	canal&=0b00001111;				//Limitar la entrada a 5
	ADMUX = (ADMUX & 0xF0)|canal;  //Limpiar los últimos 4 bits de ADMUX, OR con ch
	ADCSRA|=(1<<ADSC);				//Inicia la conversión
	while((ADCSRA)&(1<<ADSC));		//Hasta que se complete la coversión
	return(ADC);					//devuelve el valor del adc
}
 */
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
    int v_temp =0;
    int pow_10 = 1000;
    for (int i = 0; i<8; i++){
        if(i == 4)
        {
            cadena[4] = '.';
            value = value *1000;
            pow_10 = 100;
            continue;
        }
        v_temp = (int)(value/pow_10);
        cadena[i] = num_s(v_temp,flag);
        if(v_temp != 0){flag = 1;}
        value = value - ((float)v_temp*pow_10);
        pow_10 = pow_10/10;
    }
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
//#define rs RC3
//#define rw RC4
//#define en RC5
void cmd(unsigned char a)
{
    PORTD=a;
    PORTB = (PORTB & 0b11111000) | 0b00001100;
    //PORTC = (PORTC & 0b11110111);//rs=0;
    //PORTC = (PORTC & 0b11101111);//rw=0;
    //PORTC = (PORTC | 0b00100000);//en=1;
    _delay_ms(1);
    //PORTC = (PORTC & 0b11011111);//en=0;
    PORTB = (PORTB & 0b11111000);// | 0b00000000;
}

void dat(unsigned char b)
{
    PORTD=b;
    PORTB = (PORTB & 0b11111000) | 0b00001101;
    //PORTB = 0b11111111;
    //PORTC = (PORTC | 0b00001000);//rs=1;
    //PORTC = (PORTC & 0b11101111);//rw=0;
    //PORTC = (PORTC | 0b00100000);//en=1;
    //delay(1000);
    _delay_ms(1);
    //PORTC = (PORTC & 0b11011111);//en=0;
    PORTB = (PORTB & 0b11111000) | 0b00001001;
}

void show(const char *s)
{
    while(*s) {
        dat(*s++);
    }
}



int main()
{
    //int adcValue=0;
    
    DDRD = 0xFF; // Configure PORTB and PORTD as output to display the ADC values on LEDs
    DDRB = 0x0F;
    //TRISC4 = 0;//=TRISC1=TRISC2=0;
    //TRISC3 = 0;
    
    //channel 0 variables
    float suma_ch0 = 0;
    float val_ch0 =0;
    float total_ch0 = 0;
    //Channel 1 variables
    float suma_ch1 = 0;
    float val_ch1 =0;
    float total_ch1 = 0;
    uint16_t array_ch0 [120];
    uint16_t array_ch1 [120];
    float offset;
    uint16_t max;
    uint16_t min;
    uint16_t count = 1;
    //unsigned int i;
    lcd_init();
    cmd(0x8A); //forcing the cursor at 0x8A position
    show("IDIE");
    while(1)
    {
        //ADC_init();
        for(int i = 0; i<120; i++){
            //ADC_Init_CH0();
            //ADC_0_measurement = ADC_0_get_conversion(0);
            ADC_init_0();
            array_ch0[i] = ADC_read(0);
            //ADC_Init_CH1();
            //ADC_0_measurement = ADC_0_get_conversion(1);
            ADC_init_1();
            array_ch1[i] = ADC_read(1);
            _delay_ms(0.1376);
        }
        
        max = array_ch0[0];
        min = array_ch0[0];
        for(int i = 0; i<120; i++){
            if(array_ch0[i]>max){max = array_ch0[i];}
            if(array_ch0[i]<min){min = array_ch0[i];}
        }
        unsigned int offset_0 = (max+min);
        max = array_ch1[0];
        min = array_ch1[0];
        for(int i = 0; i<120; i++){
            if(array_ch1[i]>max){max = array_ch1[i];}
            if(array_ch1[i]<min){min = array_ch1[i];}
        }
        unsigned int offset_1 = (max+min);
        suma_ch0 = 0;
        suma_ch1 = 0;
        for(int i = 0; i<120; i++){
            val_ch0 = (((float)array_ch0[i]-(offset_0/2))*5/1024);
            val_ch0 = val_ch0*val_ch0*0.0001376;
            suma_ch0 += val_ch0;
            val_ch1 = (((float)array_ch1[i]-(offset_1/2))*5/1024);
            val_ch1 = val_ch1*val_ch1*0.0001376;
            suma_ch1 += val_ch1;
        }
        val_ch0 = 0;
        val_ch1 = 0;
        if (count == 2){
            count = 1;
            suma_ch0 = suma_ch0*60;
            suma_ch0 = sqrt(suma_ch0);//*70.71;
            
            total_ch0 = (suma_ch0+total_ch0)/2;
            
            
            cmd(0x01);
            cmd(0x80);
            //float_LCD(total_ch0);
            sprintf(cadena, "%g", total_ch0*82.76);//74.405
            show("Vrms = ");
            show(cadena);
            
            suma_ch1 = suma_ch1*60;
            suma_ch1 = sqrt(suma_ch1);//*70.71;
            
            total_ch1 = (suma_ch1+total_ch1)/2;
        

            //float_LCD(total_ch1);
            sprintf(cadena, "%g", total_ch1/2.571);
            cmd(0xC0);
            show("Irms = ");
            show(cadena);
        
            suma_ch0 = suma_ch1 = 0;
        }else {
            count++;
            total_ch0 = suma_ch0*60;
            total_ch0 = sqrt(total_ch0);//*70.71;
            
            total_ch1 = suma_ch1*60;
            total_ch1 = sqrt(total_ch1);//*70.71;
        }
        DDRC = 0b11111100;
        //DDRB = 0x0F;
        if ((PINB & 0b0010000) == 0b0000000){
        }
        else if ((PINB & 0b0010000) == 0b0010000){
            PORTC = 0b0000000; //Decremento PC2=0=UD 
            PORTB = 0b00000000;// Habilitar cs = 0
            PORTC = 0b0000100; // Increment/decremento   toogle
            _delay_ms(100);
            PORTC = 0b0000000;
            PORTB = 0b00001000;
        }
        //DDRD = 0xFF; // Configure PORTB and PORTD as output to display the ADC values on LEDs
        //DDRB = 0x0F;
    }
}
