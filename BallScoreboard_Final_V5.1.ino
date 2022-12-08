//球類積分版 發送端 運算端

#include <Servo.h>

Servo myservo;

//直流馬達腳位
#define AB_A 18    // 控制 A B 馬達的前進（ HIGH ）、 後退（ LOW
#define AB_B 5    // 控制 A B 馬達的速度 0~255 LOW 表示停止
#define Speed 150  // 設定初始速度

//伺服馬達腳位
#define servomotorPin 3  //伺服馬達

const int pinState[10][7] = {
  { 1, 1, 1, 1, 1, 1, 0 },  //0
  { 0, 1, 1, 0, 0, 0, 0 },  //1
  { 1, 1, 0, 1, 1, 0, 1 },  //2
  { 1, 1, 1, 1, 0, 0, 1 },  //3
  { 0, 1, 1, 0, 0, 1, 1 },  //4
  { 1, 0, 1, 1, 0, 1, 1 },  //5
  { 1, 0, 1, 1, 1, 1, 1 },  //6
  { 1, 1, 1, 0, 0, 0, 0 },  //7
  { 1, 1, 1, 1, 1, 1, 1 },  //8
  { 1, 1, 1, 1, 0, 1, 1 },  //9
};

//4位7段顯示器
int set7SegPin[7] = { 2, 4, 19, 6, 7, 8, 9 };  //abcdefg
int set7SegBytePin[4] = { 10, 11, 12, 13 };   //0123

int value[4];  //4位7段顯示器 4位存儲
int n0, n1, n2, n3;

int attackScpre;
int defenseScpre;

int BeforeAttackScpre;
int BeforeDefenseScpre;

int pinstatue[18];

//偵測到+離開 算一次
bool U1Lock = false;
bool U1UnLock = false;
bool U2Lock = false;
bool U2UnLock = false;
int AttackScore = 0;
int DefenceScore = 0;

int AllScore;

int val;

unsigned long time_previous;  
int angle;

//超音波設定
int time;           //超音波來回時間
int Distance;       //超音波偵測距離
int count;          //超音波計數物件出現次數
int trigPin1 = 14;  //超音波trig1
int echoPin1 = 15;  //超音波echo1
int trigPin2 = 16;  //超音波trig2
int echoPin2 = 17;  //超音波echo2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(set7SegBytePin[i], OUTPUT);
  }
  for (int i = 0; i < 7; i++) {
    pinMode(set7SegPin[i], OUTPUT);
  }
  attackScpre = 0;
  defenseScpre = 0;

  n0 = n1 = n2 = n3 = 0;

  Serial.begin(9600);  //Serial

  //超音波設定
  //超音波1設定
  pinMode(trigPin1, OUTPUT);    //超音波1trig腳位輸出
  pinMode(echoPin1, INPUT);     //超音波1echo腳位寫入
  digitalWrite(trigPin1, LOW);  //超音波1
  //超音波2設定
  pinMode(trigPin2, OUTPUT);    //超音波2trig腳位輸出
  pinMode(echoPin2, INPUT);     //超音波2echo腳位寫入
  digitalWrite(trigPin2, LOW);  //超音波2

  //馬達設定
  pinMode(AB_A, OUTPUT);
  pinMode(AB_B, OUTPUT);
  digitalWrite(AB_A, LOW);
  digitalWrite(AB_B, LOW);

  //伺服馬達
  myservo.attach(servomotorPin);
}

void loop() {
  // put your main code here, to run repeatedly:
  //attackScpre = 15;
  //defenseScpre = 18;
  //set7Seg(1518);
  fireall();
  U1Lock = Ultrasound(trigPin1, echoPin1, 300, 0);
  //Serial.print("U1Lock = ");
  // Serial.println(U1Lock);
  //delay(1);
  if (U1Lock != U1UnLock) {
    if (U1Lock == 1) {
      attackScpre++;
      //Serial.print("attackScpre = ");
      //Serial.println(attackScpre);
    }
  }
  U1UnLock = U1Lock;
  //delay(50);

  U2Lock = Ultrasound(trigPin2, echoPin2, 220, 0);
  //Serial.print("U2Lock = ");
  // Serial.println(U2Lock);
  if (U2Lock != U2UnLock) {
    if (U2Lock == 1) {
      defenseScpre++;
      attackScpre--;
      //Serial.print("defenseScpre = ");
      //Serial.println(defenseScpre);
    }
  }
  U2UnLock = U2Lock;
  //AllScore = U1Lock * 100 + U2Lock;
  AllScore = attackScpre * 100 + defenseScpre;
  if (attackScpre < 0)
    attackScpre = 0;
  if (defenseScpre < 0)
    defenseScpre = 0;
  set7Seg(AllScore);

unsigned long time_now = millis();  
  if(time_now - time_previous > 3000){  
    angle = random(45,135);  //亂數範圍0-9999  
    time_previous += 1000;  
  }
  //angle = random(181);
  servostart(angle);

  //delay(50);
  //delay(1000);
  //servoforward();

  //Serial.print("BeforeAttackScpre = ");
  //Serial.println(BeforeAttackScpre);

  //Serial.print("BeforeDefenseScpre = ");
  //Serial.println(BeforeDefenseScpre);

  /*if ((BeforeAttackScpre != attackScpre) || (BeforeDefenseScpre == defenseScpre)) {
    for (int i = 0; i < 4; i++) {
      set7Seg(value[i], i + 1);
    }
    //set7Seg(1, 4);
    //set7Seg(2, 3);
    //set7Seg(3, 2);
    //set7Seg(4, 1);
    //serial_blink();
    Serial.println("case1");
    BeforeAttackScpre = attackScpre;
    BeforeDefenseScpre = defenseScpre;

  } else {
    value[3] = BeforeAttackScpre / 10;
    value[2] = BeforeAttackScpre % 10;
    value[1] = BeforeDefenseScpre / 10;
    value[0] = BeforeDefenseScpre % 10;
    for (int i = 0; i < 4; i++) {
      set7Seg(value[i], i + 1);
    }
    Serial.println("case2");
  }*/
  //delay(1000);
  //servoback();
  //Serial.print("BeforeAttackScpre = ");
  //Serial.println(BeforeAttackScpre);

  //Serial.print("BeforeDefenseScpre = ");
  //Serial.println(BeforeDefenseScpre);
}

void setDigit(int number, int pos) {
  for (int i = 0; i < 4; i++) {
    if (i + 1 == pos)
      digitalWrite(set7SegBytePin[i], 0);
    else
      digitalWrite(set7SegBytePin[i], 1);
  }

  for (int i = 0; i < 7; i++) {
    digitalWrite(set7SegPin[i], pinState[number][i]);
  }
}

void set7Seg(int number) {
  n3 = number / 1000;
  number %= 1000;
  n2 = number / 100;
  number %= 100;
  n1 = number / 10;
  n0 = number % 10;

  setDigit(n3, 4);
  delay(5);
  setDigit(n2, 3);
  delay(5);
  setDigit(n1, 2);
  delay(5);
  setDigit(n0, 1);
  delay(5);
}

//超音波偵測函式(trig腳位,echo腳位,時間閥值,數量閥值)
int Ultrasound(int trigPins, int echoPins, int Timehreshold, int countthreshold) {
  count = 0;
  digitalWrite(trigPins, HIGH);  //發射超音波
  delay(1);
  digitalWrite(trigPins, LOW);
  delay(1);
  time = pulseIn(echoPins, HIGH);  //計算發射到接收時間
  //Distance = time * 0.034 / 2;     //計算距離
  Serial.print("time = ");
  Serial.println(time);
  if (time < Timehreshold)  //時間是否小於閥值
  {
    count++;
    if (count > countthreshold)  //時間是否小於閥值
    {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
  //return Distance;
}

//馬達設定
//所有馬達停止
void stopall() {
  digitalWrite(AB_B, LOW);  //A馬達速度0
}

//馬達發射 A正轉 B反轉
void fireall() {
  digitalWrite(AB_A, HIGH);  //A馬達正轉
  //digitalWrite(AB_B, HIGH);  //A馬達正轉
  analogWrite(AB_B, Speed);  //A馬達速度
}

//伺服馬達
void servostart(int angle) {
  myservo.write(angle);
}