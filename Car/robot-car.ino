#include <Servo.h>
#include <SoftwareSerial.h>
#define NUM_ANGLES 7

#define pinBluetooth_rx 9 // Broche 11 en tant que RX, à raccorder sur TX du HC-05
#define pinBluetooth_tx 1 // Broche 10 en tant que TX, à raccorder sur RX du HC-05
SoftwareSerial SerialBluetooth(pinBluetooth_rx, pinBluetooth_tx);
Servo servo;
Servo servo2;

const int trigPin = 13;
const int echoPin=12;

const int bipPin=8;
const int servoPin=11;
const int servo2Pin = 10;
const int enAPin=6;
const int in1Pin=7;
const int in2Pin=5;
const int in3Pin = 4;
const int in4Pin = 2;
const int enBPin = 3;
const int SW_pin = 9;
const int X_pin = 1;
const int Y_pin = 2;
unsigned int weapon = 0;
bool manual = true;


enum Motor { LEFT, RIGHT};

//unsigned char sensorAngle[NUM_ANGLES] = {60,70,80,90,100,110,120};
unsigned char sensorAngle[NUM_ANGLES] = {60,72,84,96,108,120,130};
unsigned int distance[NUM_ANGLES];


void go( enum Motor m, int speed) {
    digitalWrite( m == LEFT ? in1Pin : in3Pin, speed > 0 ? HIGH: LOW);
    digitalWrite( m == LEFT ? in2Pin : in4Pin, speed <= 0 ? HIGH: LOW);
    analogWrite( m == LEFT ? enAPin : enBPin, speed < 0 ? -speed : speed);
}

void testMotors (){
    static int speed[8] = {128, 255, 128, 0 , -128,-255,-128,0};
    go(RIGHT, 0);

    for (unsigned char i = 0; i< 8; i++){
        go(LEFT, speed[i]);
        delay(200);
    }

    for (unsigned char i = 0; i< 8; i++){
        go(RIGHT, speed[i]);
        delay(200);
    }
}

unsigned int readDistance(){
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    unsigned long period = pulseIn(echoPin, HIGH);
    Serial.println("period");
    Serial.println(period);
    return period * 343 / 2000;
}

void readNextDistance(){
    static unsigned char angleindex = 0;
    static signed char step = 1;

    distance[angleindex] = readDistance();
    angleindex += step;

    if(angleindex == NUM_ANGLES - 1){
        step = -1;
    }
    else
    {
        if(angleindex == 0){
            step = 1;
        }
    }

    servo.write(sensorAngle[angleindex]);
}

void setup() {
    pinMode(pinBluetooth_rx, INPUT);
    pinMode(pinBluetooth_tx, OUTPUT);
    SerialBluetooth.begin(9600);

    pinMode(SW_pin, INPUT);
    digitalWrite(SW_pin, HIGH);
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
    pinMode(enAPin,OUTPUT);
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(in3Pin,OUTPUT);
    pinMode(in4Pin, OUTPUT);
    pinMode(enBPin, OUTPUT);
    pinMode(bipPin,OUTPUT);
    servo.attach(servoPin);
    servo.write(130);
    
    servo2.attach(servo2Pin);
    servo2.write(90);
    

    go(LEFT,0);
    go(RIGHT,0);

    testMotors();

    servo.write(sensorAngle[0]);
    delay(200);

    for (unsigned char i = 0; i < NUM_ANGLES; i++) {
        readDistance();
        delay(200);
    }
 }

int command;
String _btBuffer = "";
char motor_direction;

void loop() {
    _btBuffer = "";
    while (SerialBluetooth.available() > 0)
    {
        if(_btBuffer=="OK+CONN"){
        _btBuffer="";
        }
        char letter = (char)SerialBluetooth.read();
        if (letter != '\r' && letter != '\n')
        {
            _btBuffer += String(letter);
        }
    }
    _btBuffer = _btBuffer[0];

    if (manual == true){

        if (_btBuffer == "1" || _btBuffer == "2" || _btBuffer == "3" || _btBuffer == "7" || _btBuffer == "8" || _btBuffer == "9" || _btBuffer == "5"){
            Serial.println("Changement de direction");
            motor_direction = _btBuffer[0];
        }
        else if(_btBuffer == "0"){
            tone(bipPin,350,50);
        }
        else if(_btBuffer =="6" || _btBuffer =="4"){
            
            if (weapon < 90)
            {
                weapon = weapon + 10;
                servo2.write(weapon);
            }
            else{
            weapon = 0;
            }
        }
        else if(_btBuffer == "#"){
            manual = false;
        }
        

        Serial.println(motor_direction);

        if (motor_direction == '3'){           
            go(LEFT,255);
            go(RIGHT,120);
        }
        else if(motor_direction == '1'){            
            go(LEFT,120);
            go(RIGHT,255);
        }

        else if (motor_direction == '9'){ 
            go(LEFT,-255);
            go(RIGHT,120);
        }
        else if(motor_direction == '7'){
            go(LEFT,120);
            go(RIGHT,-255);
        }

        else if(motor_direction == '2'){
            go(LEFT,255);
            go(RIGHT,255);
        }

        else if(motor_direction =='8'){
            go(LEFT,-255);
            go(RIGHT,-255);
        }

        else if(motor_direction =='5'){
            go(LEFT,0);
            go(RIGHT,0);
        }
    }
 
    else{
        if(_btBuffer == "*"){
            manual = true;
            motor_direction = '5';
        }
        weapon = weapon + 10;
        readNextDistance();
        unsigned char tooClose = 0;
        for(unsigned char i = 0; i < NUM_ANGLES; i ++ ){
            Serial.println("distance");
            Serial.println(distance[i]);
            if(distance[i] < 300){
                tooClose = 1;
            }
        }

        if(tooClose){
            go(LEFT, -180);
            go(RIGHT, 100);
            tone(bipPin,350,50);
        }
        else {
            go(LEFT,255);
            go(RIGHT,255);
        }
        servo2.write(weapon);
        
        
        if (weapon > 90){
        weapon = 0;
        }
    }

    delay(50);
}
