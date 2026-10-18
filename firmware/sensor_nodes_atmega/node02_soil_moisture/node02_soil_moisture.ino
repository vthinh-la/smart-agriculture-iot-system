// Node02 
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
RF24 radio(7, 8); // nRF24L01 (CE,CSN)
RF24Network network(radio); // Include the radio in the network
const uint16_t this_node = 04; // Address of this node in Octal format
const uint16_t node00 = 00; 
float data[6]; // number of sensors

void setup() {

  
   if (!radio.begin()) 
  {
   
    while (1) {}
  } 

SPI.begin();
network.begin(90, this_node); //(channel, node address)
radio.setDataRate(RF24_2MBPS);
}
void loop() {
network.update();
float potValue = analogRead(A0); // Read the potentiometer value
float doam = map(potValue,0.0,1023.0,100.0,0.0);
data[0] = doam; 

RF24NetworkHeader header8(node00);
bool ok = network.write(header8, &data, sizeof(data)); // Send the data
delay(1000);
}
