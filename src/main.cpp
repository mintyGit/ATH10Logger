/***********************************************************************
 Purpose: Read temprature and humidity from the Temperature Sensor AHT10 
 and display to a Seven Segment Display as well as logging over a network.
***********************************************************************/

#include <Wire.h>
#include <AHTxx.h>
#include <ESP8266WiFi.h>

AHTxx aht10(AHTXX_ADDRESS_X38, AHT1x_SENSOR); // Allows communication with ATH10 temperature sensor

void printStatus(); // declare function before usage (foward declaration, more elegant to declare in header)


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
    Serial.println(" Celcius");
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
    Serial.println(" %");
  }

  delay(10000); // delaying to prevent overheating

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