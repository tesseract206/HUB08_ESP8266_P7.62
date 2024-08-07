#include <Arduino.h>
#include "font.h"
//library sendiri nih boss

//pin

#define P_R1 5   // Red data
#define P_G1 4   // Green data
#define P_R2 0   //R2
#define P_G2 2   //G2
#define P_CLK 14 // Clock
#define P_LAT 12 // Latch
#define P_OE 13  // Output Enable
#define P_A 16   // Address pins
#define P_B 15
#define P_C 3
#define P_D 1


uint8_t frameBufferR1[32][64 / 8] = {0}; // Buffer for R1
uint8_t frameBufferR2[32][64 / 8] = {0}; // Buffer for R2

void setup() {
  pinMode(P_R1, OUTPUT);
  pinMode(P_G1, OUTPUT);
  pinMode(P_R2, OUTPUT);
  pinMode(P_G2, OUTPUT);
  pinMode(P_CLK, OUTPUT);
  pinMode(P_LAT, OUTPUT);
  pinMode(P_OE, OUTPUT);
  pinMode(P_A, OUTPUT);
  pinMode(P_B, OUTPUT);
  pinMode(P_C, OUTPUT);
  pinMode(P_D, OUTPUT);
  memset(frameBufferR1, 0, sizeof(frameBufferR1));
  memset(frameBufferR2, 0, sizeof(frameBufferR2));
}
void setPixel1(int x, int y, bool on) {
  if (x < 0 || x >= 64 || y < 0 || y >= 32) return; // Check bounds

  int col = x / 8;
  int bitPos = x % 8;
  
  if (on) {
    frameBufferR1[y][col] |= (1 << bitPos);
  } else {
    frameBufferR1[y][col] &= ~(1 << bitPos);
  }
}
void setPixel2(int x, int y, bool on) {
  if (x < 0 || x >= 64 || y < 0 || y >= 32) return; // Check bounds

  int col = x / 8;
  int bitPos = x % 8;
  
  if (on) {
    frameBufferR2[y][col] |= (1 << bitPos);
  } else {
    frameBufferR2[y][col] &= ~(1 << bitPos);
  }
}
void drawChar(int x, int y, char c) {
  int charIndex;
  if (c >= '0' && c <= '9') {
    charIndex = c - '0'+1; // Numbers 0-9
} else if (c >= 'A' && c <= 'Z') {
    charIndex = c - 'A' + 11; // Letters A-Z, with 'A' mapped to 10
} else if (c >= 'a' && c <= 'z'){
    charIndex = c - 'a' + 37;
} else {
    return; // Handle non-alphanumeric characters if necessary
} // Calculate index
  //i<besar font
  for (int i = 0; i < 8; i++) {
    uint8_t colData = number0_up[charIndex][i];

    for (int j = 0; j < 8; j++) {
      setPixel1(x + i, y + j, colData & (1 << j));
    }
  }
}
void drawChar1(int x, int y, char c) {
  int charIndex;
  if (c >= '0' && c <= '9') {
    charIndex = c - '0'+1; // Numbers 0-9
} else if (c >= 'A' && c <= 'Z') {
    charIndex = c - 'A' + 11; // Letters A-Z, with 'A' mapped to 10
} else if (c >= 'a' && c <= 'z'){
    charIndex = c - 'a' + 37;
} else {
    return; // Handle non-alphanumeric characters if necessary
} // Calculate index
  //i<besar font
  for (int i = 0; i < 8; i++) {
    uint8_t colData = number0_dw[charIndex][i];

    for (int j = 0; j < 8; j++) {
      setPixel2(x + i, y + j, colData & (1 << j));
    }
  }
}
void displayText(const char* text, int x, int y) {
  while (*text) {
    drawChar(x, y, *text);
    x += 8; // Move to the next character position disesuaikan besar font
    text++;
  }
}
void displayText1(const char* text, int x, int y) {
  while (*text) {
    drawChar1(x, y, *text);
    x += 8; // Move to the next character position disesuaikan besar font
    text++;
  }
}

void updateDisplay() {
  for (int row = 0; row < 32; row++) {
    // Set row address
    digitalWrite(P_A, row & 0x01);
    digitalWrite(P_B, (row >> 1) & 0x01);
    digitalWrite(P_C, (row >> 2) & 0x01);
    digitalWrite(P_D, (row >> 3) & 0x01);
    
    for (int col = 0; col < 64 / 8; col++) {
      uint8_t redData1 = frameBufferR1[row][col];
      uint8_t greenData1 = frameBufferR1[row][col];
      uint8_t redData2 = frameBufferR2[row][col];
      uint8_t greenData2 = frameBufferR2[row][col];
      
      for (int bit = 0; bit < 8; bit++) {
        digitalWrite(P_R1, (redData1 >> bit) & 0x01); // Set red
        digitalWrite(P_G1, (greenData1 >> bit) & 0x00); // Set green
        digitalWrite(P_R2, (redData2 >> bit) & 0x01);
        digitalWrite(P_G2, (greenData2 >> bit) & 0x00); // Set green
        digitalWrite(P_CLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(P_CLK, LOW);
      }
    }
    
    // Latch data
    digitalWrite(P_LAT, HIGH);
    delayMicroseconds(1);
    digitalWrite(P_LAT, LOW);
    
    // Enable output
    digitalWrite(P_OE, LOW);
    delayMicroseconds(200); // Time to display this row
    digitalWrite(P_OE, HIGH);
  }
}
void loop() {
  // Clear the display
  memset(frameBufferR1, 0, sizeof(frameBufferR1));
  memset(frameBufferR2, 0, sizeof(frameBufferR2));

  static int scrollPosition = 64;
  // Display text starting at position (0, 0)
  displayText(texT, scrollPosition, 8);
  displayText1(texT, scrollPosition, 0);
  

  // Delay for scrolling speed
  scrollPosition--;
  // Update the display
  updateDisplay();
  if (scrollPosition < -((int)strlen(texT) * 8)) {
    scrollPosition = 64;
  }
  delayMicroseconds(200);
}


