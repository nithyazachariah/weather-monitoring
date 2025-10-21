#define BLYNK_TEMPLATE_ID "TMPL3A0vPg9Up"

#define BLYNK_TEMPLATE_NAME "Microcontroller Weather Monitoring"

#define BLYNK_AUTH_TOKEN "iqNoBxHB6dGCp8Ac8qHloQr_9Np0AnQ5"


#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#include <DHT.h>

#include <SFE_BMP180.h>

#include <ESP8266WiFi.h>

#include <BlynkSimpleEsp8266.h>


// --- Pin definitions ---

#define DHTPIN D3

#define DHTTYPE DHT11

#define LDRPIN D5

#define SDA_PIN D2

#define SCL_PIN D1


// --- WiFi credentials ---

char ssid[] = "OPPO F27 5G";

char pass[] = "akshayaj";


// --- Objects ---

DHT dht(DHTPIN, DHTTYPE);

SFE_BMP180 bmp;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Try 0x3F if 0x27 doesn't work


bool bmpReady = false;


// --- Function to send sensor data ---

void sendSensorData() {

  float temperature = dht.readTemperature();

  float humidity = dht.readHumidity();


  double pressure = 0;

  double bmpTemperature = 0;

  double pressure_hPa = 0;


  if (bmpReady) {

    char status = bmp.getPressure(pressure, bmpTemperature);

    if (status) {

      pressure_hPa = pressure / 100.0; // Convert Pa → hPa

    } else {

      Serial.println("Pressure read failed");

    }

  }


  int lightState = digitalRead(LDRPIN);

  const char* lightText = lightState ? "Dark" : "Bright";


  // --- Send to Blynk ---

  Blynk.virtualWrite(V0, temperature);

  Blynk.virtualWrite(V1, humidity);

  Blynk.virtualWrite(V2, lightText);

  Blynk.virtualWrite(V3, pressure_hPa);


  // --- Serial Monitor ---

  Serial.print("Temp: "); Serial.print(temperature); Serial.print("°C  ");

  Serial.print("Humidity: "); Serial.print(humidity); Serial.print("%  ");

  Serial.print("Light: "); Serial.print(lightText); Serial.print("  ");

  Serial.print("Pressure: "); Serial.print(pressure_hPa); Serial.println(" hPa");


  // --- LCD Display ---

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("T:");

  lcd.print(temperature, 1);

  lcd.print("C H:");

  lcd.print(humidity, 0);

  lcd.print("%");


  lcd.setCursor(0, 1);

  lcd.print("L:");

  lcd.print(lightText);

  lcd.print(" P:");

  lcd.print(pressure_hPa, 0);

}


void setup() {

  Serial.begin(9600);

  Serial.println("\nSystem starting...");


  // Initialize I2C manually

  Wire.begin(SDA_PIN, SCL_PIN);


  // Initialize LCD

  lcd.init();

  lcd.backlight();

  lcd.setCursor(0, 0);

  lcd.print("Starting...");


  // Start DHT

  dht.begin();


  // Start BMP180

  if (bmp.begin()) {

    bmpReady = true;

    Serial.println("BMP180 ready.");

  } else {

    Serial.println("BMP180 not found!");

    lcd.setCursor(0, 1);

    lcd.print("BMP180 FAIL");

  }


  pinMode(LDRPIN, INPUT);


  // WiFi + Blynk

  Serial.println("Connecting WiFi...");

  lcd.clear();

  lcd.print("Connecting WiFi...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);


  Serial.println("WiFi connected.");

  lcd.clear();

  lcd.print("WiFi Connected");


  // Start timer for updates

  timer.setInterval(2000L, sendSensorData);

  Serial.println("Setup complete.");

}


void loop() {

  Blynk.run();

  timer.run();

}