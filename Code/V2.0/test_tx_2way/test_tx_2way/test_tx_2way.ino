#include <SPI.h>
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

Enrf24 radio(P2_0, P2_1, P2_2);

const uint8_t Node1tx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
const uint8_t Node1rx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x4E };

const uint8_t Node2tx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x6E };
const uint8_t Node2rx[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xEE };

uint16_t staterec[2] = {0, 0};
bool stateset[2] = {0, 0};

void ISR1() {
  stateset[0] = true;
  stateset[1] = true;
}
void ISR2() {
  stateset[0] = false;
  stateset[1] = false;
}
void ISR3() {
  stateset[0] = !stateset[0];
}
void ISR4() {
  stateset[1] = !stateset[1];
}

void transmitstate(char* address, bool state) {
  radio.begin();
  delay(5);
  radio.setTXaddress((void*) address);
  radio.print(state);
  radio.flush();
  radio.end();
}

int receivestate(char* address) {
  char inbuf[33];
  radio.begin();
  delay(5);

  radio.setRXaddress((void*)address);
  radio.enableRX();

  long int start = millis();

  while (!radio.available(true)) {
    if (millis() - start == 200) {
      return (0);
    }
  }
  if ( radio.read(inbuf)) {
    radio.end();
    return (atoi((char*)inbuf));
  }
}

void setup() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
 
  pinMode(P2_3, OUTPUT);
  pinMode(P2_4, OUTPUT);
  digitalWrite(P2_3, LOW);
  digitalWrite(P2_4, LOW);

  pinMode(P1_0, INPUT_PULLUP);
  pinMode(P1_1, INPUT_PULLUP);
  pinMode(P1_2, INPUT_PULLUP);
  pinMode(P1_3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(P1_0), ISR1, RISING);
  attachInterrupt(digitalPinToInterrupt(P1_1), ISR2, RISING);
  attachInterrupt(digitalPinToInterrupt(P1_2), ISR3, RISING);
  attachInterrupt(digitalPinToInterrupt(P1_3), ISR4, RISING);
}

void loop() {
  staterec[0] = receivestate((char*) Node1rx);
  if (stateset[0] != staterec[0]) {
    transmitstate((char*) Node1tx, stateset[0]);
  }

  staterec[1] = receivestate((char*) Node2rx);
  if (stateset[1] != staterec[1]) {
    transmitstate((char*) Node2tx, stateset[1]);
  }
  digitalWrite(P2_4, staterec[1]);
  digitalWrite(P2_3, staterec[0]);

}
