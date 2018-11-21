#define heartratePin A1
#include "DFRobot_Heartrate.h"
#include <CurieBLE.h>
#include "CurieIMU.h"
#include "math.h"

DFRobot_Heartrate heartrate(DIGITAL_MODE); //< ANALOG_MODE lub DIGITAL_MODE

int ledPin = 13;

BLEPeripheral blePeripheral;       // BLE urządzenie peryferyjne
BLEService heartRateService("180D"); // standardowy serwis BLE Heart Rate

// standardowa charakterystyka Heart Rate Measurement
BLECharacteristic heartRateChar("2A37",  // standardowe 16-bitowe UUID charakterystyki
    BLERead | BLENotify, 2);  
// charakterystyka krokomierza
BLEUnsignedIntCharacteristic stepChar("2A53", BLERead | BLENotify);
BLEUnsignedIntCharacteristic alkoChar("2A8E", BLERead | BLENotify);

int steps=0; //zmienna przechowująca liczbę kroków
long previousMillis = 0;
int minimumTime=400; //czas jaki musi upłynąć przed następnym krokiem
float minimumValue=2.5;  //próg detekcji kroków


// zmienne do alkomatu
int sensorPin = A3;
int buttonPin = 7;
int sensorValue = 0;
int sensorDelay = 100;
int kalibracja = 0; 
int pomiar = 0;
float przelicznik = 0.0038671875;
float przelicznikPromile = 0.21;
float promile = 0.0;
float temp = 0.0;


// funkcja krokomierza, pobieranie danych z akcelerometru
// oraz detekcja kroków
void stepCounter(void) {
  long currentStepMillis = millis();
  static long previousStepMillis = 0;
  float ax, ay, az;   //wartości z akcelerometru
  float vector;       //wektor wypadkowy przyspieszenia
  
  // zapisanie danych z akcelerometru do zmiennych
  CurieIMU.readAccelerometerScaled(ax, ay, az);
//  Serial.print(ax);
//  Serial.print("\t");
//  Serial.print(ay);
//  Serial.print("\t");
//  Serial.print(az);
//  Serial.print("\t");
//  Serial.println();

  // obliczanie wektora przyspieszenia
  vector=sqrt(ax*ax+ay*ay+az*az);
 // Serial.print(vector);
 // Serial.println();

  // detekcja kroków
  if(vector >= minimumValue && currentStepMillis - previousStepMillis >= minimumTime){
    previousStepMillis = currentStepMillis;
    steps=steps+1;
    Serial.print("Liczba kroków: ");
    Serial.print(steps);
    Serial.println();
    stepChar.setValue(steps);
  }
}



// Funkcja alkomatu
void alkomat(void){

    // read the value from the sensor:
    Serial.println("Nagrzewanie.");
    delay(10000);
    Serial.println("Kalibracja czujnika. Prosze czekac.");
    sensorValue  = 0;
    kalibracja = 0;
    for(int i=0; i<20; i++){
      sensorValue = analogRead(sensorPin);
      Serial.println(sensorValue);
      kalibracja += sensorValue;
      delay(sensorDelay);
    }
    kalibracja /= 20;
    Serial.print("Srednia, po kalibracji: ");
    Serial.println(kalibracja);
    delay(10000);
    Serial.println("Po nastepnym komunikacie, mozna dmuchac ");
    delay(1000);
    Serial.println("PROSZE DMUCHAC PRZEZ PARE SEKUND!: ");
  
    sensorValue  = 0;
    pomiar = 0;
    delay(500);
    for(int j=0; j<20; j++){
      sensorValue = analogRead(sensorPin);
      Serial.println(sensorValue);
  
      pomiar += sensorValue;
      
      // turn the ledPin on
      digitalWrite(ledPin, HIGH);
      // stop the program for <sensorValue> milliseconds:
      delay(sensorDelay);
      // turn the ledPin off:
      digitalWrite(ledPin, LOW);
      // stop the program for for <sensorValue> milliseconds:
      delay(sensorDelay);
    }
  
    pomiar /= 20;
    Serial.print("Wynik pomiaru: ");
    Serial.println(pomiar);
  
    Serial.print("Wynik pomiar - kalibracja: ");
    Serial.println(pomiar-kalibracja);
  
    Serial.print("Wynik przelicznik: ");
  
    temp = (pomiar-kalibracja)*przelicznik;
    Serial.println(temp);
    promile = (temp*10)*przelicznikPromile;
    
    Serial.print("WYNIK W PROMILACH: ");
    Serial.println(promile);

    alkoChar.setValue(pomiar-kalibracja);
    delay(10000);
  }
  






void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);   // zapalenie diody, podczas połączenia z urządzeniem centralnym
  pinMode(buttonPin, INPUT_PULLUP); // ustawienie przycisku
  
  // nadanie urządzeniu peryferyjnemu nazwy, pod którą będzie wydoczne dla innych urządzeń
  blePeripheral.setLocalName("FitApi");
  blePeripheral.setAdvertisedServiceUuid(heartRateService.uuid());  // dodanie UUID serwisu
  blePeripheral.addAttribute(heartRateService);   // dodanie serwisu Heart Rate
  blePeripheral.addAttribute(heartRateChar); // dodanie charakterystyk dla pulsometru i krokomierza
  blePeripheral.addAttribute(stepChar);
  blePeripheral.addAttribute(alkoChar);
  stepChar.setValue(steps); 
  stepChar.setValue(0);

  // Aktywowanie urządzenia. Od tej chwili urządzenie jest widoczne dla innych do chwili połączenia
  blePeripheral.begin();
  CurieIMU.begin();
  Serial.println("Bluetooth device active, waiting for connections...");
  // ustawienie zakresu pracy akcelerometru
  CurieIMU.setAccelerometerRange(2);
}

void loop() {
   // nasłuchiwanie urządzeń
  BLECentral central = blePeripheral.central();

  if (central) {
     Serial.print("Connected to central: ");
    // wypisanie adresu MAC sparowanego urządzenia
   Serial.println(central.address());
    // zapalenie diody
    digitalWrite(13, HIGH);
    
    // petla zawierająca funkcje do obsługi krokomierza i pulsometru
    // wykonywana dopóki nie zostanie zerwane połączenie
    while (central.connected()) {
      // wywołanie funkcji krokomierza
      if(digitalRead(buttonPin) == LOW){
        alkomat();
      }
      stepCounter();
      long currentMillis = millis();
      // pobranie danych z pulsometru co 18ms
      if(currentMillis - previousMillis >= 18){
        previousMillis = currentMillis;
        uint8_t rateValue;
        heartrate.getValue(heartratePin); // pobranie wartości z portu
        rateValue = heartrate.getRate(); // obliczanie pulsu
        if(rateValue)  {
          int heartRateMeasurement = rateValue;
          int heartRate = map(heartRateMeasurement, 0, 1023, 0, 100);
  
          Serial.print("Heart Rate is now: ");
          Serial.println(rateValue);
          
          const unsigned char heartRateCharArray[2] = { 0, (char)rateValue };
          heartRateChar.setValue(heartRateCharArray, 2);
          
          stepChar.setValue(steps);
        }
          //delay(20);
      }
    }
  }

  // wyłączenie diody po rozłączeniu z urządzeniem centralnym
    digitalWrite(13, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
 }
