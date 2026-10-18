
// Node01
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
//#include <avr/sleep.h>

RF24 radio(7, 8); // nRF24L01 (CE,CSN)
RF24Network network(radio); // Include the radio in the network
const uint16_t this_node = 01; // Address of this node in Octal format
const uint16_t node00 = 00; 
unsigned long data[6]; // number of sensors

int led = 9;

unsigned long lastInterruptTime = 0;

void setup() {
     if (!radio.begin()) 
    {
      while (1) {
       digitalWrite(led,HIGH);
       delay(200);
       digitalWrite(led,LOW);
        }
    }  
  SPI.begin();
  
  pinMode(led, OUTPUT);
  
  network.begin(90, this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
}
void loop() {
  network.update();
  unsigned long potValue = analogRead(A0); // Read the potentiometer value
  float doam = map(potValue,0,1023,100,37);
  data[0] = doam; 

  RF24NetworkHeader header8(node00);
  bool ok = network.write(header8, &data, sizeof(data)); // Send the data
  digitalWrite(led, LOW);
  delay(3000);
  digitalWrite(led, HIGH);
  

 /* unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 1000) { // Chống rung phím
    RF24NetworkHeader header7(node00);
    bool ok = network.write(header7, &data, sizeof(data)); // Send the data
    digitalWrite(led, HIGH);
    delay(1000);   
  }
  lastInterruptTime = interruptTime;
  digitalWrite(led, LOW);*/
}
  
 
