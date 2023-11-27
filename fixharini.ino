 #include <Arduino_FreeRTOS.h>
 #include <SPI.h>
 #include <avr/io.h>
#include <LiquidCrystal.h>
#define BAUD 9600
#define BAUD_PRESCALE ((F_CPU / (BAUD * 16UL)) - 1)
// Inisialisasi objek LCD dengan parameter (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(A1, A0, 4, 5, 6, 7);

const int buttonPin = 2;
const int buttonPin1 = 3;
const int buttonPin2 = 8;
const int buttonPin3 = 9;
const int buzzer = A2;

int slaveaddres = 0 ;
int masteraddres = 0 ;
int bitdata = 0 ;

int seconds = 0;
int minutes = 0;
int hours = 0;

int day =0 ;
int tgl = 27;
int moon = 10;
int years = 2023;

int jamAlarm= 0;
int menitAlarm= 0 ;

int SETjam = 0;
int SETmenit = 0;
int SETdetik = 0;

bool readVolt = false ;
bool avo = false ;
float measuredVoltage;
const int analogInPin = A3; // Pin analog sebagai input
float voltageDividerRatio = 5.0 / (20.0 + 5.0); // Perbandingan pembagi tegangan

int Alarm_init =0;
int d_tone =0;

enum state {S0, S_jam ,S_alarm,S_setJam,jam,jam_set, menit_set ,jamSET,menitSET,detikSET ,AVO,avoMode ,komunikasi,PC,volt,proses,I2C,slaveMode,masterMode,alamatslave,prosesSlave} stateku;
enum state1 {c4,d4,e4,f4,g4,a4,b4,c5} melodiku;
enum state2 {f0,f1,f2,f3,f41} pagi;
// 1. Print ke serial monitor dengan 3 task (tugas)
 
void task_count_jam(void *param);
void task_komunikasi(void *param);
void task_menu(void *param);
void task_Alarm(void *param);
void Task_menu1(void *param);


TaskHandle_t Task_Handle1;
TaskHandle_t Task_Handle2;
TaskHandle_t Task_Handle3;
TaskHandle_t Task_Handle4;
TaskHandle_t Task_Handle5;

void setup() {

  Serial.begin(9600);
  UART_init();
  SPI.begin(); // Inisialisasi SPI sebagai master
  pinMode(10, OUTPUT); // Tetapkan pin 10 sebagai SS (Slave Select)
  lcd.begin(16, 2);
  pinMode(A3, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  xTaskCreate(task_count_jam,"Task1",64,NULL,1,&Task_Handle1);
  xTaskCreate(task_komunikasi,"Task2",64,NULL,2,&Task_Handle2);
  xTaskCreate(task_menu,"Task3",128,NULL,3,&Task_Handle3);
  xTaskCreate(task_Alarm,"Task4",64,NULL,5,&Task_Handle4);

  xTaskCreate(task_menu1,"Task5",128,NULL,4,&Task_Handle5);
}
 
void loop() {
}
 
void task_count_jam(void *param) {
  (void) param;
 
  while(1) {
      seconds++;
      if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
          minutes = 0;
          hours++;
          if(hours >= 24){
            hours=0;
            tgl++;
            day++;
            if(day >= 6){
              day = 0;
            }
            if(tgl >= 31){
              tgl = 1;
              moon ++;
              if(moon >=12){
                moon = 1;
                years++;
              }
            }
          }
        }
      }
    
    vTaskDelay(958/portTICK_PERIOD_MS); // Arduino akan melakukan print ke serial monitor tiap 2 milidetik
  }
}

void task_komunikasi(void *param) {
  (void) param;
 
  while(1) {
    // Gunakan nilai count di sini
    
      // UART_sendChar("Volt: ");
      // UART_sendChar(measuredVoltage);
      // UART_sendChar(" V/n/r"); 
    
    Serial.print("Nilai seconds: ");
    Serial.println(seconds);
    
    vTaskDelay(1000/portTICK_PERIOD_MS); // Arduino akan melakukan print ke serial monitor tiap 1 detik sekali
  }
}
 
void task_menu(void *param)
{
    (void) param;
 
  while(1)
  {
    switch(stateku)
    {
//========STATE AWAL==========================//      
      case S0:
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("==Jam Digital==");
        if(btnNext()){
          stateku = S_jam;
        }
        break;
      }
//========STATE JAM==========================// (SETELAH S0) 
      case S_jam:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Tampilkan Jam");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S_alarm;         
        }
        if(btnOK()){
          stateku=jam;        
        }
        break;
      }

      case jam:
      {
        lcd.clear();
        showJam(); 
        if(btnOK()){
          stateku=S_jam;        
        }
        break;
      }
//========STATE ALARM==========================// (SETELAH STATE JAM) 
      case S_alarm:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Setting Alarm");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S_setJam;         
        }
        if(btnOK()){
          stateku = jam_set;       
        }
        break;
      }

      case jam_set:
      {
        if(button_up()){
          jamAlarm++;
        }
        if(button_down()){
          jamAlarm--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("jam :");
          lcd.print(jamAlarm);
        if(btnOK()){
          stateku = menit_set;
        }
        break;
      }
      case menit_set:
      {
        if(button_up()){
          menitAlarm++;
        }
        if(button_down()){
          menitAlarm--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("menit :");
          lcd.print(menitAlarm);        
      
        if(btnOK()){
          Alarm_init++;
          stateku = jam;
        }
        break;
      }


//========STATE SETTING JAM==========================// (SETELAH STATE ALARM) 
      case S_setJam:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Setting Jam");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = komunikasi;         
        }
        if(btnOK()){
          stateku = jamSET;       
        }
        break;
      }

      case jamSET:
      {
        if(button_up()){
          SETjam++;
        }
        if(button_down()){
          SETjam--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("jam :");
          lcd.print(SETjam);
        if(btnOK()){
          stateku = menitSET;
        }
        break;
      }
      case menitSET:
      {
        if(button_up()){
          SETmenit++;
        }
        if(button_down()){
          SETmenit--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("menit :");
          lcd.print(SETmenit);
        if(btnOK()){
          stateku = detikSET;
        }
        break;
      }
      case detikSET:
      {
        if(button_up()){
          SETdetik++;
        }
        if(button_down()){
          SETdetik--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("detik :");
          lcd.print(SETdetik);

        if(btnOK()){
          hours=SETjam;
          minutes=SETmenit;
          seconds=SETdetik;

          stateku = jam;
        }
        break;
      }
//========KOMUNIKASI==========================// (SETELAH SET JAM) 
      case komunikasi:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("KOMUNIKASI");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = AVO;         
        }
        if(btnOK()){
          stateku = PC;       
        }
        break;
      }
      case PC:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Using DB9 to PC ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = I2C;         
        }
        if(btnOK()){
          stateku = volt;       
        }
        break;
      }
      case volt:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("kirim data volt ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = PC;         
        }
        if(btnOK()){
          avo = true ;
          stateku = proses;       
        }
        break;
      }
      case proses:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("mengirim ...");
        if(btnNext()or btnOK()){
          avo = false;
          stateku = komunikasi;
        }
        break;
      }
      case I2C:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Using I2C ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S0;         
        }
        if(btnOK()){
          stateku = slaveMode;       
        }
        break;
      }
      case slaveMode:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Slave mode ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = masterMode;         
        }
        if(btnOK()){
          stateku = alamatslave;       
        }
        break;
      }
      case alamatslave:
      {
        if(button_up()){
          slaveaddres++;
        }
        if(button_down()){
          slaveaddres--;
        }
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("alamat :");
        lcd.print(slaveaddres);
        if(btnOK()){
          stateku = prosesSlave;       
        }
        break;
      }
      case prosesSlave:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Slave Mode");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnOK()){
          stateku = prosesSlave;       
        }
        break;
      }
//========STATE AVO==========================// (SETELAH STATE KOMUNIKASI) 
      case AVO:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("MODE AVO");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S0;         
        }
        if(btnOK()){
          readVolt=true;    
          stateku = avoMode; 
        }
        break;
      }
      case avoMode:
      {
        lcd.clear();
        showVolt();
        if(btnOK()){
          readVolt=false;    
          stateku = AVO; 
        }
        break;
      }
    }
    vTaskDelay(16/portTICK_PERIOD_MS); // Arduino akan melakukan print ke serial monitor tiap 1 detik sekali
  }
}

void task_menu1(void *param)
{
    (void) param;
 
  while(1)
  {
    vTaskDelay(4/portTICK_PERIOD_MS);
    switch(stateku)
    {
//========STATE AWAL==========================//      
      case S0:
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("==Jam Digital==");
        if(btnNext()){
          stateku = S_jam;
        }
        break;
      }
//========STATE JAM==========================// (SETELAH S0) 
      case S_jam:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Tampilkan Jam");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S_alarm;         
        }
        if(btnOK()){
          stateku=jam;        
        }
        break;
      }

      case jam:
      {
        lcd.clear();
        showJam(); 
        if(btnOK()){
          stateku=S_jam;        
        }
        break;
      }
//========STATE ALARM==========================// (SETELAH STATE JAM) 
      case S_alarm:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Setting Alarm");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S_setJam;         
        }
        if(btnOK()){
          stateku = jam_set;       
        }
        break;
      }

      case jam_set:
      {
        if(button_up()){
          jamAlarm++;
        }
        if(button_down()){
          jamAlarm--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("jam :");
          lcd.print(jamAlarm);
        if(btnOK()){
          stateku = menit_set;
        }
        break;
      }
      case menit_set:
      {
        if(button_up()){
          menitAlarm++;
        }
        if(button_down()){
          menitAlarm--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("menit :");
          lcd.print(menitAlarm);        
      
        if(btnOK()){
          Alarm_init++;
          stateku = jam;
        }
        break;
      }


//========STATE SETTING JAM==========================// (SETELAH STATE ALARM) 
      case S_setJam:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Setting Jam");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = komunikasi;         
        }
        if(btnOK()){
          stateku = jamSET;       
        }
        break;
      }

      case jamSET:
      {
        if(button_up()){
          SETjam++;
        }
        if(button_down()){
          SETjam--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("jam :");
          lcd.print(SETjam);
        if(btnOK()){
          stateku = menitSET;
        }
        break;
      }
      case menitSET:
      {
        if(button_up()){
          SETmenit++;
        }
        if(button_down()){
          SETmenit--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("menit :");
          lcd.print(SETmenit);
        if(btnOK()){
          stateku = detikSET;
        }
        break;
      }
      case detikSET:
      {
        if(button_up()){
          SETdetik++;
        }
        if(button_down()){
          SETdetik--;
        }
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("detik :");
          lcd.print(SETdetik);

        if(btnOK()){
          hours=SETjam;
          minutes=SETmenit;
          seconds=SETdetik;

          stateku = jam;
        }
        break;
      }
//========KOMUNIKASI==========================// (SETELAH SET JAM) 
      case komunikasi:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("KOMUNIKASI");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = AVO;         
        }
        if(btnOK()){
          stateku = PC;       
        }
        break;
      }
      case PC:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Using DB9 to PC ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = I2C;         
        }
        if(btnOK()){
          stateku = volt;       
        }
        break;
      }
      case volt:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("kirim data volt ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = PC;         
        }
        if(btnOK()){
          avo = true ;
          stateku = proses;       
        }
        break;
      }
      case proses:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("mengirim ...");
        if(btnNext()or btnOK()){
          avo = false;
          stateku = komunikasi;
        }
        break;
      }
      case I2C:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Using I2C ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S0;         
        }
        if(btnOK()){
          stateku = slaveMode;       
        }
        break;
      }
      case slaveMode:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Slave mode ?");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = masterMode;         
        }
        if(btnOK()){
          stateku = alamatslave;       
        }
        break;
      }
      case alamatslave:
      {
        if(button_up()){
          slaveaddres++;
        }
        if(button_down()){
          slaveaddres--;
        }
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("alamat :");
        lcd.print(slaveaddres);
        if(btnOK()){
          stateku = prosesSlave;       
        }
        break;
      }
      case prosesSlave:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Slave Mode");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnOK()){
          stateku = prosesSlave;       
        }
        break;
      }
//========STATE AVO==========================// (SETELAH STATE KOMUNIKASI) 
      case AVO:
      {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("MODE AVO");
        lcd.setCursor(13, 1);
        lcd.print("==>");
        if(btnNext()){
          stateku = S0;         
        }
        if(btnOK()){
          readVolt=true;    
          stateku = avoMode; 
        }
        break;
      }
      case avoMode:
      {
        lcd.clear();
        showVolt();
        if(btnOK()){
          readVolt=false;    
          stateku = AVO; 
        }
        break;
      }
    }
    vTaskDelay(16/portTICK_PERIOD_MS); // Arduino akan melakukan print ke serial monitor tiap 1 detik sekali
  }
}


void task_Alarm(void *param) {
  (void) param;
 
  while(1) {
    if(readVolt==true)
    {
        int sensorValue = analogRead(A3); // Membaca nilai tegangan dari pin A0
        float inputVoltage = sensorValue * (5.0 / 1023.0); // Konversi nilai ADC ke tegangan (5V max, 10-bit resolution)
        measuredVoltage = inputVoltage / voltageDividerRatio; // Menghitung tegangan sesungguhnya
    }

    if(Alarm_init !=0){
      if(minutes==menitAlarm){
        if(hours==jamAlarm){
          ringtone_pagi();
        }
      }else{
          noTone(buzzer); 
          jamAlarm =0;
          menitAlarm=0;
          Alarm_init--;  //BUG
        }
    }
    
    vTaskDelay(100/portTICK_PERIOD_MS); // Arduino akan melakukan print ke serial monitor tiap 2 milidetik
  }
}

void showJam(){
    char detik[3], menit[3], jam[3] ,tanggal[3],bullan[3],tahun[5]; // Maksimum 2 digit + null-terminator

    sprintf(detik, "%02d", seconds);
    sprintf(menit, "%02d", minutes);
    sprintf(jam, "%02d", hours);
    sprintf(tanggal, "%d", tgl);
    sprintf(bullan, "%d", moon);
    sprintf(tahun, "%d", years);

    lcd.setCursor(4, 0);
    lcd.print(jam);
    lcd.print(":");
    lcd.print(menit);
    lcd.print(":");
    lcd.print(detik);


  // lcd.setCursor(0, 1);
  // lcd.print(hari[day]);   // Menampilkan nama hari
  // lcd.print(tanggal);     // Menampilkan tanggal
  // // // lcd.print(" /");
  // // lcd.print(bullan); // Menampilkan nama bulan
  // // // lcd.print(" /");
  // lcd.print(tahun);       // Menampilkan tahun

}

void showVolt(){
    char vo[6];
    dtostrf(measuredVoltage, 6, 2, vo);
    lcd.setCursor(0, 0);
    lcd.print("Volt: ");
    lcd.print(vo);
    lcd.print(" V");
}

void UART_init() {
    UBRR0H = (BAUD_PRESCALE >> 8);
    UBRR0L = BAUD_PRESCALE;

    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, 1 stop bit, no parity
}

void UART_sendString(const char *str) {
    while (*str) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *str++;
    }
}

void UART_sendFloat(float value) {
    char buffer[20];
    sprintf(buffer, "%f", value);
    UART_sendString(buffer);
}

unsigned char btnNext(){
  int buttonState = digitalRead(buttonPin);
  static unsigned char debounce=0xFF;
	unsigned char detectedFLag=0;
	if(buttonState == LOW){
		debounce=(debounce<<1);
	} else {
		debounce= (debounce<<1)|1;
	}
	if (debounce==0x03) {
		detectedFLag=1;
	}
	return detectedFLag;
}

unsigned char btnOK(){
  int buttonState = digitalRead(buttonPin1);
  static unsigned char debounce=0xFF;
	unsigned char detectedFLag=0;
	if(buttonState == LOW){
		debounce=(debounce<<1);
	} else {
		debounce= (debounce<<1)|1;
	}
	if (debounce==0x03) {
		detectedFLag=1;
	}
	return detectedFLag;
}

unsigned char button_up(){
  int buttonState = digitalRead(buttonPin2);
  static unsigned char debounce=0xFF;
	unsigned char detectedFLag=0;
	if(buttonState == LOW){
		debounce=(debounce<<1);
	} else {
		debounce= (debounce<<1)|1;
	}
	if (debounce==0x03) {
		detectedFLag=1;
	}
	return detectedFLag;
}

unsigned char button_down(){
  int buttonState = digitalRead(buttonPin3);
  static unsigned char debounce=0xFF;
	unsigned char detectedFLag=0;
	if(buttonState == LOW){
		debounce=(debounce<<1);
	} else {
		debounce= (debounce<<1)|1;
	}
	if (debounce==0x03) {
		detectedFLag=1;
	}
	return detectedFLag;
}


void ringtone()
{
  switch(melodiku)
  {
    case c4:
    {
      tone(buzzer, 262); // C4
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = d4;
      }
      break;
    }
    case d4:
    {
      tone(buzzer, 294); // D4
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = e4;
      }
      break;
    }
    case e4:
    {
      tone(buzzer, 330); // E4
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = f4;
      }
      break;
    }
    case f4:
    {
      tone(buzzer, 349); // F4
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = g4;
      }
      break;
    }
    case g4:
    {
      tone(buzzer, 392); // G4
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = a4;
      }
      break;
    }
    case a4:
    {
      tone(buzzer, 440); // A4
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = b4;
      }
      break;
    }
    case b4:
    {
      tone(buzzer, 440); // A4
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = c5;
      }
      break;
    }
    case c5:
    {
      tone(buzzer, 523); // C5
      if(d_tone++ <= 4){}else{
        d_tone =0;
        melodiku = c4;
      }
      break;
    }
  }
}

void ringtone_pagi(){
  switch(pagi)
  {
    case f0:
    {
      tone(buzzer, 500);
      if(d_tone++<=2){}else{
        d_tone=0;
        pagi=f1;
      }
      break;
    }
    case f1:
    {
      tone(buzzer, 600);
      if(d_tone++<=2){}else{
        d_tone=0;
        pagi=f2;
      }
      break;
    }
    case f2:
    {
      tone(buzzer, 700);
      if(d_tone++<=2){}else{
        d_tone=0;
        pagi=f3;
      }
      break;
    }
    case f3:
    {
      tone(buzzer, 800);
      if(d_tone++<=2){}else{
        d_tone=0;
        pagi=f41;
      }
      break;
    }
    case f41:
    {
      tone(buzzer, 900);
      if(d_tone++<=7){}else{
        d_tone=0;
        pagi=f0;
      }
      break;
    }
  }
}