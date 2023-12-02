#include <Adafruit_Arcada.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_SHT31.h>
#include <Adafruit_APDS9960.h>
#include <Adafruit_BMP280.h>
#include <PDM.h>
#define WHITE_LED 43

Adafruit_Arcada arcada;
Adafruit_LSM6DS33 lsm6ds33;
Adafruit_LIS3MDL lis3mdl;
Adafruit_SHT31 sht30;
Adafruit_APDS9960 apds9960;
Adafruit_BMP280 bmp280;
extern PDMClass PDM;
extern Adafruit_FlashTransport_QSPI flashTransport;
extern Adafruit_SPIFlash Arcada_QSPI_Flash;

uint32_t buttons, last_buttons;
uint8_t j = 0;  // neopixel counter for rainbow

double refPress = 101325;

void setup() {
  Serial.begin(115200);

  // enable NFC pins  
  if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) == (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)){
    Serial.println("Fix NFC pins");
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
    NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
    Serial.println("Done");
    NVIC_SystemReset();
  }
  
  pinMode(WHITE_LED, OUTPUT);
  digitalWrite(WHITE_LED, LOW);

  Serial.println("Hello! Arcada Clue test");
  if (!arcada.arcadaBegin()) {
    Serial.print("Failed to begin");
    while (1);
  }
  arcada.displayBegin();
  Serial.println("Arcada display begin");

  for (int i=0; i<250; i+=10) {
    arcada.setBacklight(i);
    delay(1);
  }

  arcada.display->setCursor(0, 0);
  arcada.display->setTextWrap(true);
  arcada.display->setTextSize(2);
  
  /********** Check QSPI manually */
  if (!Arcada_QSPI_Flash.begin()){
    Serial.println("Could not find flash on QSPI bus!");
    arcada.display->setTextColor(ARCADA_RED);
    arcada.display->println("QSPI Flash FAIL");
  } else {
    uint32_t jedec;
    jedec = Arcada_QSPI_Flash.getJEDECID();
  }
  
  arcada.display->setTextColor(ARCADA_WHITE);


  /********** Check APDS */
  if (!apds9960.begin()) {
    Serial.println("No APDS9960 found");
    arcada.display->setTextColor(ARCADA_RED);
  } else {
    Serial.println("**APDS9960 OK!");
    arcada.display->setTextColor(ARCADA_GREEN);
    apds9960.enableColor(true);
  }


  /********** Check LSM6DS33 */
  if (!lsm6ds33.begin_I2C()) {
    Serial.println("No LSM6DS33 found");
    arcada.display->setTextColor(ARCADA_RED);
  } else {
    Serial.println("**LSM6DS33 OK!");
    arcada.display->setTextColor(ARCADA_GREEN);
  }

  
  /********** Check LIS3MDL */
  if (!lis3mdl.begin_I2C()) {
    Serial.println("No LIS3MDL found");
    arcada.display->setTextColor(ARCADA_RED);
  } else {
    Serial.println("**LIS3MDL OK!");
    arcada.display->setTextColor(ARCADA_GREEN);
  }


  /********** Check SHT3x */
  if (!sht30.begin(0x44)) {
    Serial.println("No SHT30 found");
    arcada.display->setTextColor(ARCADA_RED);
  } else {
    Serial.println("**SHT30 OK!");
    arcada.display->setTextColor(ARCADA_GREEN);
  }


  /********** Check BMP280 */
  if (!bmp280.begin()) {
    Serial.println("No BMP280 found");
    arcada.display->setTextColor(ARCADA_RED);
  } else {
    Serial.println("**BMP280 OK!");
    arcada.display->setTextColor(ARCADA_GREEN);
  }

  arcada.display->setTextWrap(false);
}


void loop() {
    for (int i=0; i<250; i+=10) {
    arcada.setBacklight(i);
    delay(1);
  }
  arcada.display->setTextColor(ARCADA_WHITE, ARCADA_BLACK);
  arcada.display->setCursor(0, 0);
  arcada.display->setTextSize(3);

  arcada.display->print("Temperature: ");
  arcada.display->println("         "); 
  if (bmp280.readTemperature() > 20) {
    arcada.display->setTextColor(ARCADA_RED, ARCADA_BLACK);
  } 
  else if (0 < bmp280.readTemperature() && bmp280.readTemperature() < 20) {
    arcada.display->setTextColor(ARCADA_ORANGE, ARCADA_BLACK);
  }
  else if (-20 < bmp280.readTemperature() && bmp280.readTemperature() < 0) {
    arcada.display->setTextColor(ARCADA_YELLOW, ARCADA_BLACK);
  }
  else if (-40 < bmp280.readTemperature() && bmp280.readTemperature() <-20) {
    arcada.display->setTextColor(ARCADA_GREENYELLOW, ARCADA_BLACK);
  }
  else {
    arcada.display->setTextColor(ARCADA_GREEN, ARCADA_BLACK);
  }
  arcada.display->print(bmp280.readTemperature());
  arcada.display->setTextColor(ARCADA_WHITE, ARCADA_BLACK);
  arcada.display->print(" C");
  arcada.display->println("         ");
  

  arcada.display->print("Pressure: ");
  arcada.display->println("         ");
  if (bmp280.readPressure() > 80000) {
    arcada.display->setTextColor(ARCADA_BLUE, ARCADA_BLACK);
  } 
  else if (60000 < bmp280.readPressure() && bmp280.readPressure() < 80000) {
    arcada.display->setTextColor(ARCADA_PURPLE, ARCADA_BLACK);
  }
  else if (40000 < bmp280.readPressure() && bmp280.readPressure() < 60000) {
    arcada.display->setTextColor(ARCADA_MAGENTA, ARCADA_BLACK);
  }
  else if (20000 < bmp280.readPressure() && bmp280.readPressure() <40000) {
    arcada.display->setTextColor(ARCADA_PINK, ARCADA_BLACK);
  }
  else {
    arcada.display->setTextColor(ARCADA_RED, ARCADA_BLACK);
  }
  arcada.display->print(bmp280.readPressure());
  arcada.display->setTextColor(ARCADA_WHITE, ARCADA_BLACK);
  arcada.display->print(" Pa");
  arcada.display->println("         ");

  uint16_t r, g, b, c;
  //wait for color data to be ready
  while(! apds9960.colorDataReady()) {
    delay(5);
  }

  arcada.display->setTextColor(ARCADA_WHITE, ARCADA_BLACK);
  sensors_event_t accel, gyro, mag, temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);
  lis3mdl.getEvent(&mag);
  arcada.display->print("Acceleration:");
  arcada.display->println("         ");
  if (accel.acceleration.x <0) {
    arcada.display->setTextColor(ARCADA_RED, ARCADA_BLACK);
  } else {
    arcada.display->setTextColor(ARCADA_GREEN, ARCADA_BLACK);
  }
  arcada.display->print(accel.acceleration.x, 1);
  arcada.display->setTextColor(ARCADA_WHITE, ARCADA_BLACK);
  arcada.display->print(",");
  if (accel.acceleration.y <0) {
    arcada.display->setTextColor(ARCADA_RED, ARCADA_BLACK);
  } else {
    arcada.display->setTextColor(ARCADA_GREEN, ARCADA_BLACK);
  }
  arcada.display->print(accel.acceleration.y, 1);
  arcada.display->setTextColor(ARCADA_WHITE, ARCADA_BLACK);
  arcada.display->print(",");
  if (accel.acceleration.z <0) {
    arcada.display->setTextColor(ARCADA_RED, ARCADA_BLACK);
  } else {
    arcada.display->setTextColor(ARCADA_GREEN, ARCADA_BLACK);
  }
  arcada.display->print(accel.acceleration.z, 1);
  arcada.display->setTextColor(ARCADA_WHITE, ARCADA_BLACK);
  arcada.display->println("         ");

  double CurTemp = bmp280.readTemperature();
  double CurPress = bmp280.readPressure();
  double Altitude = ((pow(refPress/CurPress,1/5.257)-1)*(CurTemp+273.15))/.0065;

  arcada.display->println("         ");
  arcada.display->println("         ");
  arcada.display->print("Altitude: ");
  arcada.display->println("         ");
  arcada.display->print(Altitude);
  arcada.display->print(" m");
  arcada.display->println("         ");

}