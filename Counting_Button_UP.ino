#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>                  
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)
#define range_UP (adc_value  <= 575 && adc_value  >= 580)//575
#define range_DOWN (adc_value  <= 445 && adc_value  >= 450)//448
#define range_RIGHT (adc_value  >= 125 && adc_value  <= 130)//127
#define range_LEFT (adc_value  >= 765 && adc_value  <= 770) //768
#define range_ENTER (adc_value  >= 285 && adc_value  <= 290) //287
#define range_DOWN (adc_value  >= 0 && adc_value  <= 5) //0



void USART_Init() {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE);

    UCSR0B = (1 << TXEN0); // Aktifkan pengirim UART
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Pengaturan frame: 8 data bits, 1 stop bit
}

void USART_Transmit(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))); // Tunggu hingga buffer pengiriman siap
    UDR0 = data; // Kirim data
}

void ADC_Init() {
    ADMUX = (1 << REFS0); // Referensi tegangan AVCC
    ADCSRA = (1 << ADEN); // Aktifkan ADC
}

uint16_t ADC_Read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // Pilih kanal ADC yang sesuai
    ADCSRA |= (1 << ADSC); // Mulai konversi
    while (ADCSRA & (1 << ADSC)); // Tunggu hingga konversi selesai
    return ADC; // Kembalikan hasil konversi ADC
}

int main(void) {
  int count ;
  int ini = 0; // buat nge-hold
  LCD_Init();
  USART_Init();
  ADC_Init(); 

  uint16_t adc_value;
  count = 0;
      

    while (1) {

      adc_value = ADC_Read(0);
  
      if (range_UP){
        if(adc_value != 575 && ini ==0){
          count++;
           ini= 1 ;
        }
      }
      else {
        ini =0
      }
        char buffer[10];
        sprintf(buffer, "%d\r\n", count);

        for (int i = 0; buffer[i]; i++) {
            USART_Transmit(buffer[i]); // Kirim nilai ADC melalui UART       
        }
          
        _delay_ms(500); // Delay sejenak sebelum membaca lagi 
        
    }    
}