#include <Adafruit_MAX31865.h>


#include "SSD1306Wire.h" 

#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

//NTC
byte NTCPin = A0;
#define SERIESRESISTOR 100000
#define NOMINAL_RESISTANCE 10000
#define NOMINAL_TEMPERATURE 25
#define BCOEFFICIENT 3977

//DS18B20
OneWire oneWire(D3); 
DallasTemperature sensors(&oneWire);

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D2, D1);

// SPI cs defined for HW SPI
Adafruit_MAX31865 max = Adafruit_MAX31865(D8);
// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

void setup()
{
  delay(1000);
  Serial.begin(115200);
  Serial.println("Start up....");
  Serial.println();

  Serial.println("DS1820B init...");
  sensors.begin();
  sensors.setResolution(10);


  // Initialising the UI will init the display too.
  Serial.println("DS1820B oled...");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);

  Serial.println("PT100 init...");
  max.begin(MAX31865_2WIRE);
  Serial.println("Setup completed.");
}

void loop()
{

Serial.println("Start");
sensors.requestTemperatures();
float ds18Temp = sensors.getTempCByIndex(0);
float pt100 = readPT100();
Serial.print("DS18 Temp:");Serial.println(ds18Temp);
  // clear the display
  display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "DS: " + String(ds18Temp) + "C");
    display.drawString(0, 24, "PT: " + String(pt100) + "C");
  // write the buffer to the display
  display.display();
delay(3000);
}

void adcNTC()
{
float ADCvalue;
float Resistance;
ADCvalue = sample(NTCPin);
Serial.print("Analoge ");
Serial.print(ADCvalue);
Serial.print(" = ");
//convert value to resistance
Resistance = (1023 / ADCvalue) - 1;
Resistance = SERIESRESISTOR / Resistance;
Serial.print(Resistance);
Serial.println(" Ohm");

float steinhart;
steinhart = Resistance / NOMINAL_RESISTANCE; // (R/Ro)
steinhart = log(steinhart); // ln(R/Ro)
steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
steinhart = 1.0 / steinhart; // Invert
steinhart -= 273.15; // convert to C

Serial.print("Temperature");Serial.print(steinhart);
Serial.println(" oC");
  
}

float readPT100()
{
  uint16_t rtd = max.readRTD();
  float temperature = max.temperature(RNOMINAL, RREF);
  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  Serial.print("Ratio = "); Serial.println(ratio,8);
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  Serial.print("Temperature = "); Serial.println(max.temperature(RNOMINAL, RREF));

  // Check and print any faults
  uint8_t fault = max.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    max.clearFault();
  }
  return temperature;
}


float sample(byte z)
/* This function will read the Pin 'z' 5 times and take an average.
 */
{
  byte i;
  float sval = 0;
  for (i = 0; i < 5; i++)
  {
  sval = sval + analogRead(z);// sensor on analog pin 'z'
  delay(500);
  }
  sval = sval / 5.0;    // average
  return sval;
}
//-------------
