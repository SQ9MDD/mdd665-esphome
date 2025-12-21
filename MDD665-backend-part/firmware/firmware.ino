/* 
 MDD665 EXPANDER FOR ESP8266
 Copyright Rysiek Labus 2025 r.labus@sinux.pl
 Released under GPL v.2
 
 CHANGELOG
 2025.12.07 - calkowita zmiana firmware pod ESP8266
 */
//**************************************************************************************************************//
#define sv_version  0.1
#include <stdio.h> 


String rxLine = "";

//binary inputs
const int bi_1 = 3;   // BI1 (binary input)
const int bi_2 = 4;   // BI2
const int bi_3 = 5;   // BI3
const int bi_4 = 6;   // BI4
const int bi_5 = 7;   // BI5
const int bi_6 = 8;   // BI6

//binary outputs
const int do_1 = 9;    // BO1  (in future analog output PWM out)
const int do_2 = 10;   // BO2  (in future analog output PWM out)
const int do_3 = 11;   // BO3  (in future analog output PWM out)
const int do_4 = 12;   // BO4
const int do_5 = 13;   // BO5
const int do_6 = A0;   // BO6

//analog inputs
const int ai_1 = A1;  // raw from ADC
const int ai_2 = A2;  // raw from ADC
const int ai_3 = A3;  // raw from ADC
const int ai_4 = A4;  // raw from ADC
const int ai_5 = A5;  // raw from ADC

//**************************************************************************************************************//

//reading BI
String read_bi() {
  String bits = "";
  bits += (digitalRead(bi_1) == HIGH) ? '1' : '0';
  bits += (digitalRead(bi_2) == HIGH) ? '1' : '0';
  bits += (digitalRead(bi_3) == HIGH) ? '1' : '0';
  bits += (digitalRead(bi_4) == HIGH) ? '1' : '0';
  bits += (digitalRead(bi_5) == HIGH) ? '1' : '0';
  bits += (digitalRead(bi_6) == HIGH) ? '1' : '0';
  return bits;
}

//reading BO
String read_do() {
  String bits = "";
  bits += (digitalRead(do_1) == LOW) ? '1' : '0';
  bits += (digitalRead(do_2) == LOW) ? '1' : '0';
  bits += (digitalRead(do_3) == LOW) ? '1' : '0';
  bits += (digitalRead(do_4) == LOW) ? '1' : '0';
  bits += (digitalRead(do_5) == LOW) ? '1' : '0';
  bits += (digitalRead(do_6) == LOW) ? '1' : '0';
  return bits;
}

void write_do(uint8_t index, bool state) {
  switch(index) {
    case 1: digitalWrite(do_1, state ? LOW : HIGH); break;
    case 2: digitalWrite(do_2, state ? LOW : HIGH); break;
    case 3: digitalWrite(do_3, state ? LOW : HIGH); break;
    case 4: digitalWrite(do_4, state ? LOW : HIGH); break;
    case 5: digitalWrite(do_5, state ? LOW : HIGH); break;
    case 6: digitalWrite(do_6, state ? LOW : HIGH); break;
    default: return;   // niepoprawny numer
  }
}

void sendState() {
  String di = read_bi();
  String dout = read_do();

  int v1 = analogRead(ai_1);
  int v2 = analogRead(ai_2);
  int v3 = analogRead(ai_3);
  int v4 = analogRead(ai_4);
  int v5 = analogRead(ai_5);

  Serial.print("R;DI=");
  Serial.print(di);
  Serial.print(";DO=");
  Serial.print(dout);
  Serial.print(";AI=");
  Serial.print(v1);
  Serial.print(",");
  Serial.print(v2);
  Serial.print(",");
  Serial.print(v3);
  Serial.print(",");
  Serial.print(v4);
  Serial.print(",");
  Serial.println(v5);
}

//parsowanie komendy W;O3=1
void applySingleOutput(const String &line) {
  // Format: W;O3=1
  if (line.length() < 6) {
    Serial.println("ERR");
    return;
  }

  char numChar = line.charAt(3);   // indeks wyjscia jako znak
  char valChar = line.charAt(5);   // wartosc 0 lub 1

  if (numChar < '1' || numChar > '6') {
    Serial.println("ERR");
    return;
  }

  if (valChar != '0' && valChar != '1') {
    Serial.println("ERR");
    return;
  }

  uint8_t index = numChar - '1' + 1;  // zamiana '3' na 3
  bool state = (valChar == '1');

  write_do(index, state);

  Serial.println("OK");
}

void processLine(String line) {
  if (line == "R") {
    sendState();
  }
  else if (line.startsWith("W;O")) {
    applySingleOutput(line);
  }
  else {
    Serial.println("ERR");
  }
}

void handleSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processLine(rxLine);   // tu wykonuje się twoja komenda
      rxLine = "";
    } else if (c != '\r') {
      rxLine += c;
    }
  }
}

void setup(){
  Serial.begin(115200);
 
  //init BI
  pinMode(bi_1,INPUT_PULLUP);
  pinMode(bi_2,INPUT_PULLUP);
  pinMode(bi_3,INPUT_PULLUP);
  pinMode(bi_4,INPUT_PULLUP);
  pinMode(bi_5,INPUT_PULLUP);
  pinMode(bi_6,INPUT_PULLUP);  
  
  //init BO
  pinMode(do_1,OUTPUT);
  pinMode(do_2,OUTPUT);
  pinMode(do_3,OUTPUT);
  pinMode(do_4,OUTPUT);
  pinMode(do_5,OUTPUT);
  pinMode(do_6,OUTPUT);  

  // stan startowy: LOGICZNE OFF = MOSFET WYŁ
  digitalWrite(do_1, HIGH);
  digitalWrite(do_2, HIGH);
  digitalWrite(do_3, HIGH);
  digitalWrite(do_4, HIGH);
  digitalWrite(do_5, HIGH);
  digitalWrite(do_6, HIGH);

  //init AI
  pinMode(ai_1,INPUT);
  pinMode(ai_2,INPUT);
  pinMode(ai_3,INPUT);
  pinMode(ai_4,INPUT);
  pinMode(ai_5,INPUT); 
}

void loop(){    
  handleSerial(); 
}
//**************************************************************************************************************//
