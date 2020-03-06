#include <SPI.h>
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

Enrf24 radio(P2_0, P2_1, P2_2);

const uint8_t Node1rx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
const uint8_t Node1tx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x4E };

uint16_t stateset, staterec;

void setup() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  radio.begin();
  radio.setRXaddress((char*)Node1rx);
  radio.enableRX();

  pinMode(P1_0, OUTPUT);
  digitalWrite(P1_0, LOW);
}

void loop() {
  char inbuf[33];

  while (!radio.available(true)) {
    radio.setTXaddress((char*)Node1tx);
    radio.print(stateset);
    radio.flush();
    delay(50);
  }
  if (radio.read(inbuf)) {
    staterec = atoi((char*) inbuf);
  }
  digitalWrite(P1_0, staterec);
  stateset = staterec;
}
