/***********************************************************************
 Purpose: Read temprature and humidity from the Temperature Sensor AHT10 
 and display to a Seven Segment Display as well as logging over a network.
***********************************************************************/

#include <Wire.h>
#include <AHTxx.h>
#include <ESP8266WiFi.h>
#include <LedControl.h>
#include <math.h>

AHTxx aht10(AHTXX_ADDRESS_X38, AHT1x_SENSOR); // Allows communication with ATH10 temperature sensor
LedControl lc=LedControl(12,14,16,1); //pin 12 is connected to the DataIn, pin 14 is connected to the CLK, pin 16 is connected to LOAD
//unsigned long delaytime=500; 

void printStatus(); 

void setup()
{
  WiFi.persistent(false);  //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin();  //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep

  Serial.begin(115200);
  Serial.println();

  while (aht10.begin(0, 2) != true) //calling begin setting SDA to GPIO0, and SCL to GPIO2
  {
    Serial.println("Failed to connect to AHT10, RETRYING");
    delay(5000);
  }
  
  //The MAX72XX is in power-saving mode on startup
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,1);
  /* and clear the display */
  lc.clearDisplay(0);
}

void loop()
{
  Serial.println();
  float temperature = aht10.readTemperature();

  if (temperature == AHTXX_ERROR) //checking if ath10 returns an error
  {
    Serial.println("ERROR");
    printStatus();
  }
  else
  {  
    Serial.print(temperature);
    Serial.println(" C");
  }

  Serial.println();
  float humidity = aht10.readHumidity(AHTXX_USE_READ_DATA);

  if (humidity == AHTXX_ERROR) //checking if ath10 returns an error
  {
    Serial.println("ERROR");
    printStatus();
  }
  else
  {  
    Serial.print(humidity);
    Serial.println(" H");
  }
  
  temperature = roundf(temperature * 10) / 10; // e.g 21.444 -> 21.4

  char temperature_array[5]; // array size +1 what is actually needed for null termination
  sprintf(temperature_array,"%4.1f",temperature);
  Serial.println(temperature_array);

  humidity = roundf(humidity * 10) / 10; // e.g 21.444 -> 21.4

  char humidity_array[5]; // array size +1 what is actually needed for null termination
  sprintf(humidity_array,"%4.1f",humidity);
  Serial.println(humidity_array);

  // temperature
  lc.setChar(0, 7, temperature_array[0], false);
  lc.setChar(0, 6, temperature_array[1], true);
  lc.setChar(0, 5, temperature_array[3], false);
  //lc.setChar(0, 4, 'C', false); // char 'C' is ugly, make your own.
  lc.setRow(0, 4, B01001110); // custom made capital 'C'

  // humidity
  lc.setChar(0, 3, humidity_array[0], false);
  lc.setChar(0, 2, humidity_array[1], true);
  lc.setChar(0, 1, humidity_array[3], false);
  lc.setChar(0, 0, 'H', false);

  delay(30000); // delaying to prevent overheating
}

void printStatus()
{
  switch (aht10.getStatus())
  {
    case AHTXX_NO_ERROR:
      Serial.println(F("unknown error"));
      break;

    case AHTXX_BUSY_ERROR:
      Serial.println(F("sensor busy, increase polling time"));
      break;

    case AHTXX_ACK_ERROR:
      Serial.println(F("sensor didn't return ACK, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;

    case AHTXX_DATA_ERROR:
      Serial.println(F("received data smaller than expected, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;

    case AHTXX_CRC8_ERROR:
      Serial.println(F("computed CRC8 not match received CRC8, this feature supported only by AHT2x sensors"));
      break;

    default:
      Serial.println(F("unknown status"));    
      break;
  }
}