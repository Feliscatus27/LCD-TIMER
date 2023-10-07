#include <avr/io.h>
#include <util/delay.h>
#define LCD_Port PORTD			//Define LCD Port (PORTA, PORTB, PORTC, PORTD)
#define LCD_DPin  DDRD			//Define 4-Bit Pins (PD4-PD7 at PORT D)
#define RSPIN PD0			//RS Pin
#define ENPIN PD1 			//E Pin

void lampu1(); // PB0
void kipas();  // PB1
void lampu2(); // PC0

int seconds = 0;
int minutes = 0;
int hours = 0;
int milsecond = 0 ;
int count = 0;

void TimerInit() {
   TCCR1A = 0x00;
   TCCR1B = 0x03;
   TCCR1C = 0x00;

   TIMSK1 = 0x01;
}

int main()
{ 
  DDRB = 0XFF;
  PORTB =0XFF;

	LCD_Init();  //Activate LCD
  TimerInit();
  sei();

	while(1) {
       
		char detik[3], menit[3], jam[3] ,counter[3],mil[3]; // Maksimum 2 digit + null-terminator
    sprintf(mil, "%02d", milsecond);
    sprintf(detik, "%02d", seconds);
    sprintf(menit, "%02d", minutes);
    sprintf(jam, "%02d", hours);

    // Tampilkan jam:menit:detik di LCD
    LCD_Printpos(1,0,jam);
    LCD_Print(":");
    LCD_Print(menit);
    LCD_Print(":");
    LCD_Print(detik);
    LCD_Print(":");
    LCD_Print(mil);
  }
}

ISR(TIMER1_OVF_vect) {
  TCNT1H = 0XF6;
  TCNT1L = 0X3C;
  milsecond++;
  if (milsecond >=100) {
    milsecond = 0 ; 
      seconds++;
      if (seconds >= 60) {
        seconds = 0;
        minutes++;
        
        if (minutes >= 60) {
            minutes = 0;
            hours++;
        }
    } 
  }
  lampu1();
  lampu2();
  kipas();

  return seconds , minutes ,hours,milsecond;
}

void lampu1(){
  if(hours ==0){
    if ( minutes ==0){
      if(seconds >= 5){
        PORTB &= ~(1<<1); // Lampu nyala pada pukul 00:00:5
      }
    }    
  } 
  if(hours ==0){
    if ( minutes ==0){
      if(seconds >= 8){
        PORTB |=(1<<1);  // Lampu mati pada pukul 00:00:8
      }
    }    
  } 
}
void kipas(){
  if(hours ==0){
    if ( minutes ==0){
      if(seconds >= 3){
        PORTB &= ~(1<<2);  // kipas nyala pada pukul 00:00:3
      }
    }    
  } 
  if(hours ==0){
    if ( minutes ==0){
      if(seconds >= 10){
        PORTB |=(1<<2);   // kipas nyala pada pukul 00:00:10
      }
    }    
  } 
}

void lampu2(){
  if(hours ==0){
    if ( minutes ==0){
      if(seconds >= 7){
        PORTB &= ~(1<<3);  // kipas nyala pada pukul 00:00:7
      }
    }    
  } 
  if(hours ==0){
    if ( minutes ==0){
      if(seconds >= 15){
        PORTB |=(1<<3);   // kipas nyala pada pukul 00:00:15
      }
    }    
  } 
}


void LCD_Init (void)
{
	LCD_DPin = 0xFF;		//Control LCD Pins (D4-D7)
	_delay_ms(15);		//Wait before LCD activation
	LCD_Action(0x02);	//4-Bit Control
	LCD_Action(0x28);       //Control Matrix @ 4-Bit
	LCD_Action(0x0c);       //Disable Cursor
	LCD_Action(0x06);       //Move Cursor
	LCD_Action(0x01);       //Clean LCD
	_delay_ms(2);
}

void LCD_Action( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0);
	LCD_Port &= ~ (1<<RSPIN);
	LCD_Port |= (1<<ENPIN);
	_delay_us(1);
	LCD_Port &= ~ (1<<ENPIN);
	_delay_us(200);
	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);
	LCD_Port |= (1<<ENPIN);
	_delay_us(1);
	LCD_Port &= ~ (1<<ENPIN);
	_delay_ms(2);
}
void LCD_Clear()
{
	LCD_Action (0x01);		//Clear LCD
	_delay_ms(2);			//Wait to clean LCD
	LCD_Action (0x80);		//Move to Position Line 1, Position 1
}
void LCD_Print (char *str)
{
	int i;
	for(i=0; str[i]!=0; i++)
	{
		LCD_Port = (LCD_Port & 0x0F) | (str[i] & 0xF0);
		LCD_Port |= (1<<RSPIN);
		LCD_Port|= (1<<ENPIN);
		_delay_us(1);
		LCD_Port &= ~ (1<<ENPIN);
		_delay_us(200);
		LCD_Port = (LCD_Port & 0x0F) | (str[i] << 4);
		LCD_Port |= (1<<ENPIN);
		_delay_us(1);
		LCD_Port &= ~ (1<<ENPIN);
		_delay_ms(2);
	}
}
//Write on a specific location
void LCD_Printpos (char row, char pos, char *str)
{
	if (row == 0 && pos<16)
	LCD_Action((pos & 0x0F)|0x80);
	else if (row == 1 && pos<16)
	LCD_Action((pos & 0x0F)|0xC0);
	LCD_Print(str);
}