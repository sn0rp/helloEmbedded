/*
| Device  | Name    | Arduino PIN | Device PIN |
|---------|---------|-------------|------------|
| ILI9341 | VCC     | 5V          | 1          |
| ILI9341 | GND     | GND         | 2          |
| ILI9341 | CS      | D10         | 3          |
| ILI9341 | LCD_RST | D8          | 4          |
| ILI9341 | LCD_RS  | D9          | 5          |
| ILI9341 | MOSI    | D11         | 6          |
| ILI9341 | SCK     | D13         | 7          |
| ILI9341 | LED     | D6          | 8          |
| ILI9341 | MISO    | D12         | 9          |
| ILI9341 | CTP_SCL | SCL (D19)   | 10         |
| ILI9341 | CTP_RST | D7          | 11         |
| ILI9341 | CTP_SDA | SDA (D18)   | 12         |
| ILI9341 | CTP_INT | D5          | 13         |
| ILI9341 | SD_CD   | N/A         | 14         |
|         |         |             |            |
| MPU-6050| VCC     | 3.3V        | VCC        |
| MPU-6050| GND     | GND         | GND        |
| MPU-6050| SCL     | SCL (D19)   | SCL        |
| MPU-6050| SDA     | SDA (D18)   | SDA        |
| MPU-6050| XDA     | N/A         | XDA        |
| MPU-6050| XCL     | N/A         | XCL        |
| MPU-6050| AD0     | GND         | AD0        |
| MPU-6050| INT     | D4          | INT        |
|         |         |             |            |
| Potent  | Left    | GND         | -          |
| Potent  | Middle  | A0          | -          |
| Potent  | Right   | 5V          | -          |
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <MPU6050_tockn.h>
#include "Lck.h"

#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8
#define TFT_MOSI 11
#define TFT_LED 6
#define TFT_SCK 13

#define POT_PIN A0

// create objects for each component
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
MPU6050 mpu6050(Wire);

// Global variables for sensor data
typedef struct {
  float x;
  float y;
  float z;
} Vector3D;

typedef struct {
  Vector3D accel;  // Holds X, Y, Z for accelerometer
  Vector3D gyro;   // Holds X, Y, Z for gyroscope
  float tempK;     // Temperature in Kelvin
} SensorData;

float potValue;
bool isLocked = true;
SensorData sensorData;

void setup() {
  Serial.begin(115200);
  pinMode(POT_PIN, INPUT);
  
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  tft.begin();
  
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  drawHomeScreen();
}

void loop() {
  mpu6050.update();
  readSensors();
  displayData();
  delay(500);
}

void readSensors() {
  potValue = analogRead(POT_PIN);

  sensorData.accel.x = mpu6050.getAccX();
  sensorData.accel.y = mpu6050.getAccY();
  sensorData.accel.z = mpu6050.getAccZ();

  sensorData.gyro.x = mpu6050.getGyroX();
  sensorData.gyro.y = mpu6050.getGyroY();
  sensorData.gyro.z = mpu6050.getGyroZ();

  sensorData.tempK  = mpu6050.getTemp() + 273.15;
}

void displayData() {
  // Clear previous data values only
  tft.fillRect(120, 60, 200, 180, ILI9341_BLACK); // Clear the area for values

  // Display the data
  tft.setCursor(120, 60); // Position for potentiometer value
  tft.print(potValue, 2); // Print with 2 decimal places

  tft.setCursor(120, 80); // Position for accelerometer X
  tft.print(sensorData.accel.x);

  tft.setCursor(120, 100); // Position for accelerometer Y
  tft.print(sensorData.accel.y);

  tft.setCursor(120, 120); // Position for accelerometer Z
  tft.print(sensorData.accel.z);

  tft.setCursor(120, 140); // Position for gyroscope X
  tft.print(sensorData.gyro.x);

  tft.setCursor(120, 160); // Position for gyroscope Y
  tft.print(sensorData.gyro.y);

  tft.setCursor(120, 180); // Position for gyroscope Z
  tft.print(sensorData.gyro.z);

  tft.setCursor(120, 200); // Position for temperature in Kelvin
  tft.print(sensorData.tempK);
  tft.println(" K");
}

void drawLockIcon() {
  if (isLocked) {
    tft.drawRGBBitmap(0, 0, epd_bitmap_allArray[1], 32, 32); // Lock icon
  } else {
    tft.drawRGBBitmap(0, 0, epd_bitmap_allArray[0], 32, 32); // Unlock icon
  }
}

void drawHomeScreen() {
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  
  // Draw the lock/unlock icon
  drawLockIcon();

  // Draw the title
  tft.setTextColor(ILI9341_GREENYELLOW);
  tft.setTextSize(2);
  tft.setCursor(40, 10); // Adjust cursor position for title
  tft.println("Snorp Stats");

  // Set text size for labels
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(0, 60); // Start below the header
  tft.print("Potentiometer: ");
  
  tft.setCursor(0, 80); // Position for accelerometer X
  tft.print("Accel X: ");
  
  tft.setCursor(0, 100); // Position for accelerometer Y
  tft.print("Accel Y: ");
  
  tft.setCursor(0, 120); // Position for accelerometer Z
  tft.print("Accel Z: ");
  
  tft.setCursor(0, 140); // Position for gyroscope X
  tft.print("Gyro X: ");
  
  tft.setCursor(0, 160); // Position for gyroscope Y
  tft.print("Gyro Y: ");
  
  tft.setCursor(0, 180); // Position for gyroscope Z
  tft.print("Gyro Z: ");
  
  tft.setCursor(0, 200); // Position for temperature in Kelvin
  tft.print("Temp: ");

  tft.setTextColor(ILI9341_PINK);
}
