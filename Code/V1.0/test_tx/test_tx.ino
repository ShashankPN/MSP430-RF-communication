#include <SPI.h>
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

Enrf24 radio(P2_0, P2_1, P2_2);  // P2.0=CE, P2.1=CSN, P2.2=IRQ
const uint8_t txaddr0[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
const uint8_t txaddr1[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x04 };

const char *str_on = "ON";
const char *str_off = "OFF";

void dump_radio_status_to_serialport(uint8_t);
bool state[2] = {0,0};

void ISR1(){
  state[0] = !state[0];
}
void ISR2(){
  state[1] = !state[1];
}
void ISR3(){
  state[0] = false;
  state[1] = false;
}
void ISR4(){
  state[0] = true;
  state[1] = true;
}

void setup() {
  Serial.begin(9600);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  pinMode(P2_3, OUTPUT);
  pinMode(P2_4,OUTPUT);
  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  dump_radio_status_to_serialport(radio.radioState());
  radio.setTXaddress((void*)txaddr0);
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
  
  radio.setTXaddress((void*)txaddr0);
  radio.print(state[0]);
  radio.flush();
  Serial.print("NODE 0");
  Serial.println(state[0]);  
  digitalWrite(P2_3, state[0]);
 
  radio.setTXaddress((void*)txaddr1 );
  radio.print(state[1]);
  radio.flush();
  Serial.print("NODE 1");
  Serial.println(state[1]);  
  digitalWrite(P2_4, state[1]);

  dump_radio_status_to_serialport(radio.radioState());
  delay(200);
}

void dump_radio_status_to_serialport(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
    case ENRF24_STATE_NOTPRESENT:
      Serial.println("NO TRANSCEIVER PRESENT");
      break;

    case ENRF24_STATE_DEEPSLEEP:
      Serial.println("DEEP SLEEP <1uA power consumption");
      break;

    case ENRF24_STATE_IDLE:
      Serial.println("IDLE module powered up w/ oscillators running");
      break;

    case ENRF24_STATE_PTX:
      Serial.println("Actively Transmitting");
      break;

    case ENRF24_STATE_PRX:
      Serial.println("Receive Mode");
      break;

    default:
      Serial.println("UNKNOWN STATUS CODE");
  }
}
