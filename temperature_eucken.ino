// EN.605.715.81.SP21 Software Development for Real-Time Embedded Systems
// Project 2 - Serial Transmit of Temperature 
// Douglas Eucken
// Feb, 2nd 2021

// Temperature and Humidity reading via Arduino DUO and DHT-22 sensor
// Transmits updated data every 10 seconds after a 10 second stablilization time

// Libraries required to interface with the DHT-22 and use a timer interrupt
#include <DHT.h>
#include <TimerOne.h>

// Temp Sensor Setup/Initialization for DHT Library
#define DHTPIN 3     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE); 
 
// Flag used to stop the program
bool done = false;
 
// Used for capturing the time
unsigned long time;

// Used for storing sensor data
float hum;  //Stores humidity value
float tempc; //Stores temperature value C
float tempf; //Stores temperature value F

// Onboard LED 
int ledPin = 13;
 
/**
 *  Function runs only once, after each powerup or reset of the Arduino Uno
 */
void setup() {
  // Start serial output and initialize DHT sensor
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Turn off onboard LED
  Serial.begin(9600);
  dht.begin(); 
   
  // Show a welcome message as human-readable ASCII text
  Serial.println("EN.605.715.81.SP21 Software Development for Real-Time Embedded Systems");
  Serial.println("Project 2 - Serial Transmit of Temperature");
  Serial.println("Douglas Eucken");
  Serial.println("");
  Serial.println("This program transmits the time, humidity, and temperature at a periodic");
  Serial.println("rate of ~10 seconds across a Serial bus (USB) to the Host.");
  Serial.println("Press # to end the program");
  Serial.println("");
  Serial.println("TIME(S), HUMIDITY(%), TEMPERATURE(F)");
 
  // Interrupt: Set a timer of length 10000000 microseconds (10 seconds)
  Timer1.initialize(10000000);
 
  //Runs "isr_read_temperature" on each timer interrupt
  Timer1.attachInterrupt(isr_read_temperature); 
  
}
 
/**
 *  Main function
 */
void loop() {
 
  // Wait 10 seconds for temperature to stabilize
  // Temperature is being read in the background
  // via the interrupts
  delay(10000);
 
  while(!done) {    
    
    // Display the time and temperature
    display_time_and_temperature();
   
    // End program if sentinel is entered 
    end_program();    
 
    // Display time and temperature every 10 seconds
    delay(10000);
  }
   
  // Do nothing
  while (true) {}
}
 
/**
  * This function is the interrupt service routine.
  * It reads the voltage and converts to degrees Fahrenheit
  * It also uses the calibration offset for the DHT-22 that
  * I mesasured with an at home thermometer (+0.5 degrees)
  */
void isr_read_temperature() {
   
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  tempc = dht.readTemperature();
  tempf = (tempc*1.8)+32+0.5;
}
 
/**
  * Function displays the time and temperature
  */
void display_time_and_temperature() {
   
  // Capture the time and covert to seconds
  time = millis() / 1000;
 
  // Transmit the time, humidity, and temperature
  Serial.print(time); 
  Serial.print(" , ");
  Serial.print(hum);
  Serial.print(" , ");
  Serial.println(tempf);
     
}
 
/**
  * This function ends the program
  */
void end_program() {
   
  // Used for reading data from the serial monitor
  char ch;
 
  // Check to see if ! is available to be read
  if (Serial.available()) {     
   
    // Read the character
    // Serial.read() returns the first (oldest) character in the buffer 
    // and removes that byte of data from the buffer
    ch = Serial.read();    
 
    // End the program if a pound sign is entered in the
    // serial monitor
    if (ch == '#') {
      done = true;  
      Serial.println("Finished recording temperature. Goodbye.");
    }
  }    
}
