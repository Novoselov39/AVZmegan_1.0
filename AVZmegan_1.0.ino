#include <SoftwareSerial.h>                                   // Библиотека програмной реализации обмена по UART-протоколу
#include <EEPROM.h>
#include <iarduino_DHT.h>
#include <OneWire.h>
#include <GyverButton.h>
GButton myButt1;
#include <TinyGPS.h>
TinyGPS gps;
SoftwareSerial SIM900(10, 9);                                  // RX, TX
SoftwareSerial gpsSerial(12, 11);
#define key 6 // пин ключа
#define start_stop 4// пин кнопки старт стоп
#define door_limit 7// пин концевика двери
#define door_open 8// пин открытия дверей
#define stop_1 5// педаль тормоза первый контакт
#define v_in_pin 0 //делитель ннапряжения
#define remote_pin 2//пин пулта
boolean eng_on=false;// статус мотора работает/не работает
boolean suc=false; //флаг проверка статус машины завел/не завел sucsess or no
boolean avtopusk=false; //флаг работы автозапуска
//boolean manual=false; //флаг ручного запуска
boolean TPusk;// пуск по температуре
byte TTempPusk;//температура пуска TTempPusk
long atoPuskTimer=NAN;  //таймер автозапуска
long TTiimer=NAN; // таймер запуска по температуре
long tme=0; // переменная для вывода информации по времени работы\простоя
byte progrev_min; //время прогрева мотора
byte every; //время простоя мотора на автозапуске
float vin = 0.0; //ппеременая для хранения текущего напряжения
float const R1 = 90000.0; // сопротивление R1 (100K)
float const R2 = 10000.0;
float const kv=1.025;//поправ коэф на измерение напряжения
float const vP=13.6;//Пороговое напряжение, по которому определяется статус мотора раб./не раб.



String _response    = "";    // Переменная для хранения ответа модуля
String  AVZ= "AVZ ";
long lastUpdate = millis();                                   // Время последнего обновления
long const updatePeriod   = 5000;                                  // Проверять каждую минуту

//char* nom1="+79087060821"; //основной номер
String phone;
String phones = "+79087060821, +79507241178, +79954706081";   // Белый список телефонов
// Датчик DHT
#define DHTPIN 3 

iarduino_DHT dht(DHTPIN);
// Датчик температуры
OneWire ds(2);

void setup() {

  Serial.begin(115200);                                         // Скорость обмена данными с компьютером
  SIM900.begin(9600);                                         // Скорость обмена данными с модемом
  Serial.println("Start!");

  sendATCommand("AT", true);                                  // Отправили AT для настройки скорости обмена данными
  //sendATCommand("AT+CMGDA=\"DEL ALL\"", true);               // Удаляем все SMS, чтобы не забивать память

  // Команды настройки модема при каждом запуске
  //_response = sendATCommand("AT+CLIP=1", true);             // Включаем АОН
  //_response = sendATCommand("AT+DDET=1", true);             // Включаем DTMF
  sendATCommand("AT+CMGF=1;&W", true);                        // Включаем текстовый режима SMS (Text mode) и сразу сохраняем значение (AT&W)!
  lastUpdate = millis();                                      // Обнуляем таймер
    pinMode(v_in_pin, INPUT); //Делитель напряжения
    pinMode(remote_pin, INPUT); //радио пульт
    pinMode(key, OUTPUT); //зажигание
    pinMode(start_stop, OUTPUT); //стартер
    pinMode(door_limit, OUTPUT); //стартер
    pinMode(door_open, OUTPUT); //стартер
    pinMode(stop_1, OUTPUT); //стартер
    
   every=EEPROM.read(0);                   //время "неработы" при автозапуске
   progrev_min=EEPROM.read(4);             //время прогрева
   avtopusk=EEPROM.read(1);                // статус автозапуска
   TPusk=EEPROM.read(2);
   TTempPusk=EEPROM.read(3);
}



bool hasmsg = false;                                              // Флаг наличия сообщений к удалению
void loop() {

  if (lastUpdate + updatePeriod < millis() ) {                    // Пора проверить наличие новых сообщений
    do {
      _response = sendATCommand("AT+CMGL=\"REC UNREAD\",1", true);// Отправляем запрос чтения непрочитанных сообщений
      if (_response==""){
        sendATCommand("AT+CMGDA=\"DEL ALL\"", true);
        //Serial.println("deeeel");  
      }
      if (_response.indexOf("+CMGL: ") > -1) {                    // Если есть хоть одно, получаем его индекс
        int msgIndex = _response.substring(_response.indexOf("+CMGL: ") + 7, _response.indexOf("\"REC UNREAD\"", _response.indexOf("+CMGL: ")) - 1).toInt();
        char i = 0;                                               // Объявляем счетчик попыток
        do {
          i++;                                                    // Увеличиваем счетчик
          _response = sendATCommand("AT+CMGR=" + (String)msgIndex + ",1", true);  // Пробуем получить текст SMS по индексу
          _response.trim();                                       // Убираем пробелы в начале/конце
          //Serial.println("rer:"+_response);
          //if (_response=(">")){sendATCommand("\r" + (String)((char)26), true);}
          if (_response.endsWith("OK")) {                         // Если ответ заканчивается на "ОК"
            if (!hasmsg) hasmsg = true;                           // Ставим флаг наличия сообщений для удаления
            sendATCommand("AT+CMGR=" + (String)msgIndex, true);   // Делаем сообщение прочитанным
            sendATCommand("\n", true);                            // Перестраховка - вывод новой строки
            parseSMS(_response);                                  // Отправляем текст сообщения на обработку
            break;                                                // Выход из do{}
          }
          else {                                                  // Если сообщение не заканчивается на OK
            //Serial.println ("Error answer");                      // Какая-то ошибка
            sendATCommand("\n", true);                            // Отправляем новую строку и повторяем попытку
          }
        } while (i < 10);
        break;
      }
      else {
        lastUpdate = millis();                                    // Обнуляем таймер
        if (hasmsg) {
          sendATCommand("AT+CMGDA=\"DEL READ\"", true);           // Удаляем все прочитанные сообщения
          hasmsg = false;
        }
        break;
      }
    } while (1);
    _response.trim(); 
    if (_response.endsWith(">")){sendATCommand("\r" + (String)((char)26), true);}
  }

  if (SIM900.available())   {                         // Если модем, что-то отправил...
    _response = waitResponse();                       // Получаем ответ от модема для анализа
    _response.trim();                                 // Убираем лишние пробелы в начале и конце
    //Serial.println(_response);                        // Если нужно выводим в монитор порта
    if (_response.indexOf("+CMTI:")>-1) {             // Пришло сообщение об отправке SMS
      lastUpdate = millis() -  updatePeriod;          // Теперь нет необходимости обрабатываеть SMS здесь, достаточно просто
                                                      // сбросить счетчик автопроверки и в следующем цикле все будет обработано
    }
  }
  //if (Serial.available())  {                          // Ожидаем команды по Serial...
    //SIM900.write(Serial.read());                      // ...и отправляем полученную команду модему
  //};

  //////////////////////////////автозапуск\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//==================Мониторим нажатие кнопки пульта======================

      vin=volt();
      int analog(analogRead(remote_pin));
       myButt1.tick(analog > 1000);
      if (myButt1.isHolded() and !eng_on and vin<vP){
                    digitalWrite(key, HIGH);
                    delay(100);
                    digitalWrite(start_stop, HIGH);
                    delay(100);
                    digitalWrite(start_stop, LOW);
                    delay(100);
                    digitalWrite(key, LOW);
                    delay(100);
                    digitalWrite(door_limit, HIGH);
                    delay(100);
                    digitalWrite(door_limit, LOW); 
                    eng_start(3,true); 
                    digitalWrite(key, LOW);
                    //Serial.print("start");
                    sendATCommand("AT+CMGDA=\"DEL ALL\"", true);                   
        
      }
      if (myButt1.isPress() && (eng_on || vin>vP)){
          digitalWrite(door_open, HIGH);
          delay(500);
          digitalWrite(door_open, LOW);
          sendSMS(phone,  "door open",true,true);
          eng_on=false;
          avtopusk=false;
          digitalWrite(key, LOW);
          //Serial.print("open");
      }
      //==================Контролируем время простоя при автозапуске======================
    
      
      if(millis()-atoPuskTimer>60000*every and !eng_on){
        //Serial.println("zap");       
          if (avtopusk) eng_start(3,false); //модуль автозапуска каждые 2 часа           стандартный автозапуск по времени
          }
            

      //==================Контролируем время работы мотора======================
      
      if (eng_on and (millis()-atoPuskTimer>60000*progrev_min)){
      digitalWrite(key, HIGH);
      delay(100); 
      digitalWrite(start_stop, HIGH);
      delay(500);
      digitalWrite(start_stop, LOW);
      digitalWrite(key, LOW);
      digitalWrite(door_limit, HIGH);
      delay(1000);
      digitalWrite(door_limit, LOW);
      digitalWrite(door_open, HIGH);
      delay(500);
      digitalWrite(door_open, LOW);
      //Serial.println("stop");  
      eng_on=false;
      atoPuskTimer=millis();
      }
       //==================Автозаапуск по температуре======================
       if (TPusk and !eng_on){
        
        if (millis()-TTiimer>120000){
          if (DS18B20()<=-TTempPusk and vin<vP){
            sendSMS(phone,  String(DS18B20()),true,true);
            eng_start(3,false);
          }
          
          TTiimer=millis();
        }
       }

}

void parseSMS(String msg) {                                   // Парсим SMS
  _response    = "";
  String msgheader  = "";
  String msgbody    = "";
  String msgphone   = "";

  msg = msg.substring(msg.indexOf("+CMGR: "));
  msgheader = msg.substring(0, msg.indexOf("\r"));            // Выдергиваем телефон

  msgbody = msg.substring(msgheader.length() + 2);
  msgbody = msgbody.substring(0, msgbody.lastIndexOf("OK"));  // Выдергиваем текст SMS
  msgbody.trim();

  int firstIndex = msgheader.indexOf("\",\"") + 3;
  int secondIndex = msgheader.indexOf("\",\"", firstIndex);
  
  msgphone = msgheader.substring(firstIndex, secondIndex);
  

  //Serial.println("Phone: " + msgphone);                       // Выводим номер телефона
  Serial.println(": " + msgbody);                      // Выводим текст SMS
  
  

  if (msgphone.length() > 6 && phones.indexOf(msgphone) > -1) { // Если телефон в белом списке, то...
    phone = msgphone;
    commendAVZ(msgbody);// ...выполняем команду
    
  }


}
