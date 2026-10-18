// Node00
#include <RF24.h>
#include <nRF24L01.h>
#include <RF24Network.h>
#include <SPI.h>
#include <SimpleTimer.h>


SimpleTimer timer;

#define CE_PIN 17//26
#define CSN_PIN 5//27 
#define SCK_PIN 18//14
#define MISO_PIN 19//12
#define MOSI_PIN 23//13

RF24 radio(CE_PIN, CSN_PIN, SCK_PIN, MISO_PIN, MOSI_PIN);

RF24Network network(radio); // Include the radio in the network

const uint16_t this_node = 00; // Address of our node in Octal format
unsigned long data[6]; // number of sensors

unsigned long data1; 
unsigned long data2;

void setup() {
Serial.begin(115200);

   delay(100);
   if (!radio.begin()) 
  {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  } 
network.begin(90, this_node); //(channel, node address)
radio.setDataRate(RF24_2MBPS);

 timer.setInterval(200L, SenSerial);
}
void loop() {
timer.run(); 
network.update();
while ( network.available() ) { // Is there any incoming data?
RF24NetworkHeader header;


network.read(header, &data, sizeof(data)); // Read the incoming data

if (header.from_node == 1) { // If data comes from Node 01
data1 = data[0]; 
}
 
if (header.from_node == 2) { // If data comes from Node 02
data2 = data[0]; 
  
}


}

}

void SenSerial()
{
Serial.print("SENSOR 1: ");   Serial.print("   ");   Serial.print(data1);

Serial.print("SENSOR 2: ");   Serial.print("   ");   Serial.print(data2);
Serial.println("     ");  Serial.print("     ");  
}
