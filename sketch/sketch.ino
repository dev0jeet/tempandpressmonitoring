// SPDX-FileCopyrightText: Copyright (C) Arduino s.r.l. and/or its affiliated companies
//
// SPDX-License-Identifier: MPL-2.0

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP3XX.h>
#include <Arduino_LED_Matrix.h>
#include <Arduino_RouterBridge.h>

Adafruit_BMP3XX bmp;
ArduinoLEDMatrix matrix;

int32_t latest_temp_scaled = 0;
int32_t latest_press_scaled = 0;
uint32_t sample_interval_ms = 2000;
unsigned long last_read_time = 0;

// Simple 5x7 Font Definition for digits,
const uint8_t font[15][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, 
    {0x00, 0x42, 0x7F, 0x40, 0x00}, 
    {0x42, 0x61, 0x51, 0x49, 0x46},
    {0x22, 0x41, 0x49, 0x49, 0x36}, 
    {0x18, 0x14, 0x12, 0x7F, 0x10}, 
    {0x27, 0x45, 0x45, 0x45, 0x39}, 
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, 
    {0x01, 0x71, 0x09, 0x05, 0x03}, 
    {0x36, 0x49, 0x49, 0x49, 0x36}, 
    {0x06, 0x49, 0x49, 0x29, 0x1E}, 
    {0x00, 0x60, 0x60, 0x00, 0x00},
    {0x3E, 0x41, 0x41, 0x41, 0x22}, 
    {0x7F, 0x09, 0x09, 0x09, 0x06}, 
    {0x20, 0x54, 0x54, 0x54, 0x78}  
};

int get_char_index(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c == '.') return 10;
    if (c == 'C') return 11;
    if (c == 'h') return 12;
    if (c == 'P') return 13;
    if (c == 'a') return 14;
    return -1;
}

// Global buffer holding the canvas frame matrix
uint8_t text_buffer[8][13];
char scroll_text[32] = "0.0C 0hPa";
int text_len = 9;

int32_t get_temperature() { return latest_temp_scaled; }
int32_t get_pressure() { return latest_press_scaled; }
void set_interval(uint32_t seconds) { sample_interval_ms = seconds * 1000; }

void setup() {
    Wire.begin();
    if (!bmp.begin_I2C(0x77)) { bmp.begin_I2C(0x76); }
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);

    matrix.begin();
    Bridge.begin();
    Bridge.provide("get_temperature", get_temperature);
    Bridge.provide("get_pressure", get_pressure);
    Bridge.provide("set_interval", set_interval);
}

void update_scroll_string() {
    float t = (float)latest_temp_scaled / 100.0f;
    float p = (float)latest_press_scaled / 100.0f;
    
    // Format string containing measurements combined strictly with their units
    snprintf(scroll_text, sizeof(scroll_text), " %.1fC %.0fhPa ", t, p);
    text_len = strlen(scroll_text);
}

void render_scroll_frame(int scroll_pos) {
    memset(text_buffer, 0, sizeof(text_buffer));
    
    for (int col = 0; col < 13; col++) {
        int target_pixel_idx = scroll_pos + col;
        int char_idx = target_pixel_idx / 6; 
        int char_col = target_pixel_idx % 6; 
        
        if (char_idx >= 0 && char_idx < text_len) {
            char current_char = scroll_text[char_idx];
            int font_idx = get_char_index(current_char);
            
            if (font_idx != -1 && char_col < 5) {
                uint8_t font_byte = font[font_idx][char_col];
                for (int row = 0; row < 7; row++) {
                    if (font_byte & (1 << row)) {
                        text_buffer[row + 1][col] = 1; 
                    }
                }
            }
        }
    }
    matrix.renderBitmap(text_buffer, 8, 13);
}

void loop() {
    unsigned long current_time = millis();
    
    if (current_time - last_read_time >= sample_interval_ms) {
        if (bmp.performReading()) {
            latest_temp_scaled = (int32_t)(bmp.temperature * 100.0f);
            latest_press_scaled = (int32_t)(bmp.pressure); 
            update_scroll_string();
        }
        last_read_time = current_time;
    }

    // Scroll animation execution loop
    int total_scroll_width = text_len * 5;
    for (int pos = 0; pos < total_scroll_width; pos++) {
        render_scroll_frame(pos);
        delay(40); // Control text sliding speed
    }
}