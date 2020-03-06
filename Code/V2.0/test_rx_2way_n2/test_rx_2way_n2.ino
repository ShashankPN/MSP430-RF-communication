#include <SPI.h>
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

Enrf24 radio(P2_0, P2_1, P2_2);

const uint8_t Node2rx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x6E };
const uint8_t Node2tx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xEE };

uint16_t stateset,staterec;

void setup() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  radio.begin();
  Serial.begin(9600);
  Serial.println("Started");

  pinMode(P1_0, OUTPUT);
  digitalWrite(P1_0, LOW);

  radio.setRXaddress((char*)Node2rx);
  radio.enableRX();
}
void loop() {
  char inbuf[33];
  
  while (!radio.available(true)) {
    radio.setTXaddress((char*)Node2tx);
    radio.print(stateset);
    radio.flush();
    Serial.print("Transmitting:");
    Serial.println(stateset);
    Serial.flush();
    delay(50);
  }
  if(radio.read(inbuf)){
    staterec = atoi((char*) inbuf);
    Serial.print("Received:");
    Serial.println(staterec);
    Serial.flush();
  }
  digitalWrite(P1_0, staterec);
  stateset = staterec;
}
