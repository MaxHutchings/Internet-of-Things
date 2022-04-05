#define BLYNK_PRINT Serial

// Libararies that will be used, which includes the ESP wifi library, Blynk library for the ESP8266, and the MLX sensor library
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_MLX90614.h>
 
// Authentication token to connect this NodeMCU to the Blynk app
char auth[] = "3RMkGwU42CjqLGSrZlQCE1c2qvlTnRBm";
 
// Wifi name and password, both the NodeMCU and the mobile app must be connected to this Wifi
char ssid[] = "tp-link";
char pass[] = "MPBRRPVBMQ";

// Blynk terminal widget is connected to virtual pin 12
WidgetTerminal terminal(V12);

// Create MLX instance
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Varibles that will store the temperature values gathered from the MLX
double temp_amb;
double temp_obj;
double calibration = 2.36;

// Variables to store the pins for each LED
int ledBlue = 12;     // D6
int ledGreen = 13;    // D7
int ledRed = 15;      // D8

// Initiate timer
BlynkTimer timer;

// Function to handle thresholds for temperatures
void lights()
{
  // Get temperature correct temperature from the input + the calibration value
  double temp = temp_obj + calibration;

  // Turn off all LEDs 
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, LOW);

  if (temp <= 20)   // If the temperature is below the minimum temperature threshold
  {
    // Send an ouput to the terminal widget which display which threshold and the temeperature value
    Blynk.virtualWrite(V12, "Tyre temp too cold: " + String(temp) + "\n");
    Blynk.virtualWrite(V12, "-------------------------\n");

    // Turn on the blue LED
    digitalWrite(ledBlue, HIGH);
  }
  else if (temp > 20 && temp <= 28)   // If the temperature is within the optimal threshold
  {
    // Send an ouput to the terminal widget which display which threshold and the temeperature value
    Blynk.virtualWrite(V12, "Tyre temp optimal: " + String(temp) + "\n");
    Blynk.virtualWrite(V12, "-------------------------\n");

    // Turn on the green LED
    digitalWrite(ledGreen, HIGH);
  }
  else
  {
    // Send an ouput to the terminal widget which display which threshold and the temeperature value
    Blynk.virtualWrite(V12, "Tyre temp too hot: " + String(temp) + "\n");
    Blynk.virtualWrite(V12, "-------------------------\n");

    // Turn on the red LED
    digitalWrite(ledRed, HIGH);
  }
}

// Function that will run when the NodeMCU starts
void setup()
{
  
  Serial.begin(9600);                 // Start connection with 9600 baud rate
  mlx.begin();                        // Initiate MLX sensor
  Blynk.begin(auth, ssid, pass);      // Connect NodeMCU to wifi and to the authentication code

  terminal.clear();                   // Clear the Blynk terminal widget when the NodeMCU starts

  // Set all LED pins to output only
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);

  // Run the lights function every 4 seconds to avoid spamming the terminal with updates
  timer.setInterval(4000L, lights);
}

// Function that will run indefinitely when the NodeMCU is on
void loop()
{
  // Starts a Blynk connection
  Blynk.run();

  // Grabs the values from the sensor and stores them in the according global variables
  temp_amb = mlx.readAmbientTempC();
  temp_obj = mlx.readObjectTempC();

  // Initiate the timer
  timer.run();

  // Send the values to the Blynk app on seperate virtual pins
  Blynk.virtualWrite(V1, temp_amb);
  Blynk.virtualWrite(V2, (temp_obj + calibration));

  // Go through the loop every 1 seconds, which stops 'flood errors' where more than 100 values are sent per second
  delay(1000);
}
