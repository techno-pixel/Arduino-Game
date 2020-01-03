// Jad Seaidoun - 200340278
// must be in webeditor

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int trigPin = 5; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 4; // Echo Pin of Ultrasonic Sensor
unsigned long time1 = 0; // time for entry within range
unsigned long time2 = 0; // time for reaching randomNumber
int button = 6; // button pin
int score = 0; // default score value
bool isInZone = false; // condition to see if in range
bool buttonReset = false; // condition to see if button was pressed to start new level
long randNumber = 0; // random number generator later on

void setup()
{
  Serial.begin(9600); // Starting Serial Terminal
  randomSeed(analogRead(0)); // initializes the pseudo-random number generator
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Distance:");
  delay(50);
}


void loop()
{
  // code for pin taken from: 
  // https://www.instructables.com/id/Measuring-Distance-Over-Time-With-Arduino-HC-SR04-/
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  long duration, cm;
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  
  
  // if button is pushed down, print random number between 5 and 30 on lcd screen
  // and set value for button reset to true
  if (digitalRead(button) == HIGH) {
    lcd.setCursor(10, 0);
    lcd.print("    ");
    randNumber = random(5, 30);
    buttonReset = true;
    lcd.setCursor(10, 0);
    lcd.print(randNumber);
    lcd.setCursor(12, 0);
    lcd.print("cm");
    delay(20);
  }

  // maxTime is the number of milliseconds to beat for win, or if you pass, to lose
  long maxTime = 3000;
  
  // microsecondsToCentimers is defined at the bottom, uses speed of sound to determine distance in cm
  cm = microsecondsToCentimeters(duration);
  printCM(cm);
  delay(100);

  // set time2 to millis() when button is pressed
  if (buttonReset) {
    time2 = millis();
  }

  // sets time1 when object moved inZone of <=70cm
  if (cm <= 70 && time1 == 0 && isInZone == false && buttonReset == true)
  {
    time1 = millis();
    time2 = time1;
    isInZone = true;
    printTime(time1);
    delay(100);
  }
  
  
  // gives a point if object reaches desired random number [cm == randNumber]
  else if ((cm == randNumber) && (isInZone == true) && buttonReset && abs(time2 - time1) <= maxTime && (time2 - time1 > 0))
  {
    printSuccess();
    score++;
    isInZone = false;
    time1 = 0;
    time2 = 0;
    buttonReset = false;
  }

  // reset score if you do not reach desired randNumber and you have gone over allowed maxTime
  else if ((cm != randNumber) && (abs(time2 - time1) > maxTime) && isInZone)
  {
    printFailure();
    score = 0;
    isInZone = false;
    time1 = time2 = 0;
    buttonReset = false;
  }

  // display score
  lcd.setCursor(1, 1);
  lcd.print("Score:");
  lcd.print(score);
  lcd.print("   ");
  delay(100);
}

// function to print when you have failed in serial monitor
void printFailure() {
  Serial.println("*************");
  Serial.println("Failure");
  Serial.println("Your time was: ");
  Serial.println(time2 - time1);
  Serial.println("*************");
  Serial.println();
}

// function to print when you have scored in serial monitor
void printSuccess() {
  printTime(time2);
  delay(100);
  Serial.println("*************");
  Serial.println("Sucess");
  Serial.println("Your time was: ");
  Serial.println(time2 - time1);
  Serial.println("*************");
  Serial.println();
}

// function to print current cm from sensor AND desired randNumber on next line in serial monitor
void printCM(long cm) {
  Serial.print(cm);
  Serial.print(" cm");
  Serial.println();
  Serial.println(randNumber);
  Serial.println();
}

// print time in serial monitor to show for ac
void printTime(long t) {
  Serial.println("time  : ");
  Serial.println(t);
  Serial.println();
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  // this code is also taken from https://www.instructables.com/id/Measuring-Distance-Over-Time-With-Arduino-HC-SR04-/
  return microseconds / 29 / 2;
}