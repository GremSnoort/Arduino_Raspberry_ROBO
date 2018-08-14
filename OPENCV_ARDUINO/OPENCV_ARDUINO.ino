#define SPEED_1     6  // 5
#define DIR_1      7  // 3
#define DIR_2     8  // 4

#define SPEED_2     5  //6
#define DIR_3      4  //8
#define DIR_4      3  //7

#include <Servo.h>

Servo myServo;

#include "QList.h"
// Need to be for unknown reason
#include "QList.cpp"

QList<String> myList;


double start = millis();
int timer = 0;
bool m = false;
bool stringComplete = false;


String inputString = "";


void Stop()
{
  analogWrite(SPEED_1, 0);
  analogWrite(SPEED_2, 0);
  digitalWrite(DIR_1, LOW);
  digitalWrite(DIR_2, LOW);
  digitalWrite(DIR_3, LOW);
  digitalWrite(DIR_4, LOW);
}


void Move(int speed_motor1, int speed_motor2)
{
  //Функция движения. если скорость мотора отрицательная, то крутим в назад
  // Для коллекторного мотора можно выбрать значение скорости от 0 до 255.
  if (speed_motor1 < 0)
  {
    digitalWrite(DIR_1, LOW);
    digitalWrite(DIR_2, HIGH);
    analogWrite(SPEED_1, -speed_motor1);


  }
  else
  {
    digitalWrite(DIR_2, LOW);
    digitalWrite(DIR_1, HIGH);
    analogWrite(SPEED_1, speed_motor1);


  }

  if (speed_motor2 < 0)
  {
    digitalWrite(DIR_3, HIGH);
    digitalWrite(DIR_4, LOW);
    analogWrite(SPEED_2, -speed_motor2);

  } else
  {
    digitalWrite(DIR_4, HIGH);
    digitalWrite(DIR_3, LOW);
    analogWrite(SPEED_2, speed_motor2);

  }


}
void MoveT(int speed_motor1, int speed_motor2, double t)
{
  // Движение по линии заданное количество миллисекунд
  start = millis();
  do
  {
    Move(speed_motor1, speed_motor2);
  } while (start + t > millis());
  Stop();
  Serial.println("TIME LINE  " + String(t) );
}



void serialEvent() {
  bool read_flag = false;
  while (Serial.available()) {

 

  
    // get the new byte:
    char inChar = (char)Serial.read();
    
    // add it to the inputString:
    inputString += inChar;
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '|') {
      // помещаем пакет в очередь
      String s = inputString;
      myList.push_back(s);
      inputString = "";

      
      //
      
            // чистим остаток
            while (Serial.available()) {
              // get the new byte:
              char inChar = (char)Serial.read();
            }
      
    }
    
    stringComplete = true;
  }
  if (read_flag)
  {

    read_flag = false;
  }
}


void setup() {
  // initialize serial:

  Serial.begin(115200);

  // reserve 200 bytes for the inputString:
  inputString.reserve(1000);

  pinMode(SPEED_1, OUTPUT);
  pinMode(DIR_1, OUTPUT);
  pinMode(SPEED_2, OUTPUT);
  pinMode(DIR_2, OUTPUT);
  pinMode(DIR_3, OUTPUT);
  pinMode(DIR_3, OUTPUT);

  myServo.attach(9);
  myServo.write(90); // открыли захват


}

void loop() {

  //delay(2000);
  //MoveT(5000, 5000, 100);
  // print the string when a newline arrives:
  //Serial.println("Hello World!");


  if (myList.size() > 0)
  {
    if (stringComplete) 
        Serial.println(inputString);
    // берем пакет из очереди
    String strin = myList.at(0);
    myList.pop_front();


    int pos = strin.indexOf("MOVE");
    if ( pos >= 0)
    {
      // парсим пакет на 3 переменных
      String S = strin.substring(pos + 5);
      int pos2 = S.indexOf(",");
      int spd1 = S.substring(0, pos2).toInt();
      String S2  = S.substring(pos2 + 1);
      int pos3 = S2.indexOf(",");
      int spd2 = S2.substring(0, pos3).toInt();
      String S3  = S2.substring(pos3 + 1);
      timer = S3.toInt();
      Serial.println("MOVE KOMANDA " + String(spd1) + "  " + String(spd2) + "  " + String(timer));
      //Send("OK", ch_id);
      //if (timer > 0)
        //MoveT(spd1, spd2, timer);
      start = millis();
      m = true;
      Move(spd1, spd2);

      // Образец команды
      //|MOVE,50,50,500\n

    }

    pos = strin.indexOf("SERV");
    if ( pos >= 0)
    {
      // парсим пакет на 3 переменных
      String S = strin.substring(pos + 5);
      int pos2 = S.indexOf("|");
      int spd1 = S.substring(0, pos2).toInt();

      Serial.println("SRERV " + String(spd1));

      int srv = 0;
      if (spd1 < 0)
        myServo.write(90 + spd1 * -1);
      if (spd1 >= 0)
        myServo.write(90 - spd1);





    }

    // clear the string:

    stringComplete = false;
  }

  //Serial.println("STOP " + String(start) + "  " + String(timer) + "  " + String(millis()));
  if (m == true)
    if ((start + timer) < millis())
    {
      m = false;
      //Serial.println("STOP " + String(start) + "  " + String(timer) + "  " + String(millis()));
      Stop();
    }
}
