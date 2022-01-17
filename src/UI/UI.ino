#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

void setup(void)
{
  u8g2.begin();
  u8g2.enableUTF8Print();
}

void loop(void)
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tf);
    u8g2.drawStr(0, 16, "37.6");
    u8g2.drawUTF8(40, 16, DEGREE_SYMBOL);
    u8g2.drawStr(48, 16, "F");

    
    u8g2.drawStr(75, 16, "99.9%");
    
    u8g2.setFont(u8g2_font_ncenB18_tf);
    u8g2.drawStr(30, 60, "2.5");
    u8g2.setFont(u8g2_font_ncenB12_tf);
    u8g2.drawStr(70, 60, "Bar");
    
    
  } while (u8g2.nextPage());
  delay(1000);
}
