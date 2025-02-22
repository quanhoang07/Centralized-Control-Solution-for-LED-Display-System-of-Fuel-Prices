#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13

#define A_PIN 23
#define B_PIN 19
#define C_PIN 5
#define D_PIN 17
#define E_PIN -1

#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16

#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 1

MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  delay(1000);

  HUB75_I2S_CFG::i2s_pins _pins = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
  delay(10);

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, _pins);
  delay(10);

  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
  delay(10);

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(35);
  
  dma_display->clearScreen();
  
  dma_display->fillScreen(myWHITE);
  delay(1000);
  dma_display->fillScreen(myRED);
  delay(1000);
  dma_display->fillScreen(myGREEN);
  delay(1000);
  dma_display->fillScreen(myBLUE);
  delay(1000);
  
  dma_display->clearScreen();
  delay(1000);
}

void Scrolling_text(int scroll_speed, String scroll_text, uint16_t color) {
  static uint32_t lastUpdate = 0;
  static int16_t x = PANEL_RES_X;
  int16_t x1, y1;
  uint16_t w, h;

  dma_display->getTextBounds(scroll_text, x, 12, &x1, &y1, &w, &h);

  if (millis() - lastUpdate > scroll_speed) {
    dma_display->fillScreen(myBLACK);
    dma_display->setCursor(x, 12);
    dma_display->setTextColor(color);
    dma_display->print(scroll_text);

    x--;
    if (x < -w) {
      x = PANEL_RES_X;
    }
    lastUpdate = millis();
  }
}

void loop() {
  dma_display->setTextSize(1);    
  dma_display->setTextWrap(false);

  Scrolling_text(50, "I LOVE YOU mãi mãi ", myRED);
}