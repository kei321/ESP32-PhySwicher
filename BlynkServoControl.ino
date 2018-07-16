#define BLYNK_PRINT Serial
 
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
 
char auth[] = ""; //Blynkから送られてきたメール内にある Auth コード
char ssid[] = ""; //ルーターの SSID
char pass[] = ""; //ルーターのパスワード

/* SERVOをつないだGPIOピンの定義 */
const uint8_t SERVO_PIN_0 = 13; //LED GPIO #13
const uint8_t SERVO_PIN_1 = 12; //LED GPIO #12
/* チャンネルの定義 */
const int CHANNEL_0 = 0;
const int CHANNEL_1 = 1;

const int LEDC_TIMER_BIT = 10;   // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
const int LEDC_BASE_FREQ = 50; // 周波数(Hz)
int ON_PWM = 69;
int OFF_PWM = 100;

bool servo_switch_state = false;

const int counter_max = 15000;
int timer_counter = 0;
bool counter_switch = true;
 
void setup()
{
  Serial.begin(115200);
 
  Blynk.begin(auth, ssid, pass);
  
  pinMode(SERVO_PIN_0, OUTPUT); // SERVO_PIN_0
  ledcSetup(CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(SERVO_PIN_0, CHANNEL_0);
  
  pinMode(SERVO_PIN_1, OUTPUT); // SERVO_PIN_1
  ledcSetup(CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(SERVO_PIN_1, CHANNEL_1);
}
 
void loop()
{
  Blynk.run();

  if(servo_switch_state == true)
  {
    servo_switch(CHANNEL_0);
  }
  else if(servo_switch_state == false)
  {
    servo_switch(CHANNEL_1);
  }
}

void servo_switch(int channel)
{
    if(counter_switch == true && timer_counter < counter_max/2)
    {
     ledcWrite(channel, ON_PWM); 
     timer_counter = timer_counter + 1;
    }
    else if(counter_switch == true && timer_counter >= counter_max/2 && timer_counter <= counter_max)
    {
     ledcWrite(channel, OFF_PWM); 
     timer_counter = timer_counter + 1;
    }
    else
    {
      counter_switch = false;
      timer_counter = 0;
    }
}
 
BLYNK_WRITE(V0){ 
  Serial.printf("Blynk Pushed OFF.\r\n");
  Blynk.virtualWrite(V0, 1);
  Blynk.virtualWrite(V1, 0);
  counter_switch = true;
  servo_switch_state = false;
  timer_counter = 0;
  delay(1000);
}
BLYNK_WRITE(V1){ 
  Serial.printf("Blynk Pushed ON.\r\n");
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 1);
  counter_switch = true;
  servo_switch_state = true;
  timer_counter = 0;
  delay(1000);
}
BLYNK_WRITE(V2){
  //スマホ側 Blynk アプリで設定したスライダー値の受信
  OFF_PWM = param.asInt();
  Serial.printf("%d\r\n",  OFF_PWM);
}
BLYNK_WRITE(V3){
  //スマホ側 Blynk アプリで設定したスライダー値の受信
  ON_PWM = param.asInt();
  Serial.printf("%d\r\n", ON_PWM);
}
