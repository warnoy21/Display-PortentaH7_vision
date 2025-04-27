#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

#define RX_pin 16
#define TX_pin 17

TFT_eSPI tft = TFT_eSPI(); // Uses settings in User_Setup.h
#define MAX_JPEG_SIZE 30000  // 30 KB max image size buffer

uint8_t jpg_buffer[MAX_JPEG_SIZE];
int jpg_index = 0;
bool receiving = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_pin, TX_pin);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Init JPEG decoder
  TJpgDec.setJpgScale(1);  // No scaling
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
}

// Callback function for JPEG decoder to push pixels to TFT
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  tft.pushImage(x, y, w, h, bitmap);
  return true;
}

void loop() {
  while (Serial2.available()) {
    uint8_t byte = Serial2.read();

    // Detect JPEG start
    if (!receiving && byte == 0xFF) {
      uint8_t next_byte = Serial2.read();
      if (next_byte == 0xD8) {
        receiving = true;
        jpg_index = 0;
        jpg_buffer[jpg_index++] = 0xFF;
        jpg_buffer[jpg_index++] = 0xD8;
        continue;
      }
    }

    if (receiving) {
      if (jpg_index < MAX_JPEG_SIZE) {
        jpg_buffer[jpg_index++] = byte;

        // Check for JPEG end marker
        if (jpg_index >= 2 &&
            jpg_buffer[jpg_index - 2] == 0xFF &&
            jpg_buffer[jpg_index - 1] == 0xD9) {

          // Decode JPEG and display
          Serial.println("JPEG received. Decoding...");
          tft.fillScreen(TFT_BLACK);
          TJpgDec.drawJpg(0, 0, jpg_buffer, jpg_index);

          receiving = false;
        }
      } else {
        // Overflow - reset
        Serial.println("JPEG buffer overflow.");
        receiving = false;
        jpg_index = 0;
      }
    }
  }
}
