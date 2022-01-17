#include <Arduino.h>
#include <U8g2lib.h>

// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11

#define SCREEN_WIDTH 64;

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

double pres, temp;
uint32_t humd;

byte buzzer_pin = 3;
bool BuzzerOn = false;

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Environment Sensor"); Serial.println("");
  pinMode(buzzer_pin, OUTPUT);
  bmp_setup();
  dht_setup();
  u8g2_setup();
}

void bmp_setup()
{
  /* Initialise the sensor */
  if (!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
}

void dht_setup() {
  dht.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void u8g2_setup()
{
  u8g2.begin();
  u8g2.enableUTF8Print();
}

void convertReadings()
{
  //  Serial.println(pres);
  pres = (double)pres / (double) 1000.0;
  //  Serial.println(pres);
  temp = (double)temp * 1.8 + 32;
  //humd = humd;
}

//void drawText(int y, char *s)
//{
//  int textPos = (SCREEN_WIDTH - u8g2.getStrWidth(s) / 2);
//  u8g2.drawStr(textPos, y, s);
//}

void loop(void)
{

  /* Get a new sensor event */
  sensors_event_t event;

  /*
     BMP180 Reading
  */
  bmp.getEvent(&event);
  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    pres = event.pressure;
    /* First we get the current temperature from the BMP085 */
    /*    float temperature;
        bmp.getTemperature(&temperature);
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
    */
  }
  else
  {
    Serial.println("BMP Sensor error");
  }

  /*
     DHT11 Reading
  */
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("DHT Sensor error: temperature!"));
  }
  else {
    temp = event.temperature;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    humd = event.relative_humidity;
  }

  check_threshold() ? buzzer_on() : buzzer_off();
  convertReadings();

  char pres_buf[6], temp_buf[6], humd_buf[6];

  dtostrf(pres, 5, 2, pres_buf);
  //  sprintf(pres_buf, "%s Bar", pres_buf);
  //  sprintf(sP, "Pres: %s hPa", pres_buf);
  dtostrf(temp, 5, 1, temp_buf);
  //  sprintf(sT, "Temp: %s C", temp_buf);
  dtostrf(humd, 5, 0, humd_buf);
  //  sprintf(humd_buf, "%s%%", humd_buf);
  //  sprintf(humd_buf, "%d%%", humd);

  //  Serial.println(sP);
  //  Serial.println(sT);
  //  Serial.println(sH);
  //
  //
  //    /* Display atmospheric pressue in hPa */
  //    Serial.print("Pressure:    ");
  //    Serial.print(pres);
  //    Serial.println(" hPa");
  //    /* Display Tempertatur in °C */
  //    Serial.print(F("Temperature: "));
  //    Serial.print(temp);
  //    Serial.println(F("°C"));
  //    /* Display Humidity in % */
  //    Serial.print(F("Humidity: "));
  //    Serial.print(humd);
  //    Serial.println(F("%"));
  //

  draw(pres_buf, temp_buf, humd_buf);

  delay(1000);
}

void draw(char pres_buf[6], char temp_buf[6], char humd_buf[6])
{
  u8g2.clearBuffer();          // clear the internal memory

  //  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font

  //  Serial.println(sP);
  //  Serial.println(sT);
  //  Serial.println(sH);
  //
  //  u8g2.firstPage();
  //  do {
  //    drawText(12, sP);
  //
  //    drawText(32, sT);
  //
  //    drawText(56, sH);
  //  } while ( u8g2.nextPage() );


  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tf);
    u8g2.drawStr(0, 16, temp_buf);
    u8g2.drawUTF8(50, 16, DEGREE_SYMBOL);
    u8g2.drawStr(58, 16, "F");

    u8g2.drawStr(80, 16, humd_buf);

    u8g2.setFont(u8g2_font_ncenB18_tf);
    u8g2.drawStr(10, 60, pres_buf);
    //    u8g2.setFont(u8g2_font_ncenB12_tf);
    u8g2.drawStr(70, 60, "Bar");


  } while (u8g2.nextPage());
}

bool check_threshold()
{
  /*
     104.14 F = 40.07 C
     2.5 Bar = 2500 hPa
  */
  if (temp > 40.07 || humd >= 70 || pres >= 2500)
    return true;
  return false;
}

void buzzer_on()
{
  if (!BuzzerOn)
  {
    tone(buzzer_pin, 2000, 5000);
    BuzzerOn = true;
  }
}

void buzzer_off()
{
  noTone(buzzer_pin);
  BuzzerOn = false;
}
