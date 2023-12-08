String sendATCommand(String cmd, bool waiting) {
  String _resp = "";                                              // Переменная для хранения результата
  //Serial.println(cmd);                                            // Дублируем команду в монитор порта
  SIM900.println(cmd);                                            // Отправляем команду модулю
  if (waiting) {                                                  // Если необходимо дождаться ответа...
    _resp = waitResponse();                                       // ... ждем, когда будет передан ответ
    // Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
    if (_resp.startsWith(cmd)) {                                  // Убираем из ответа дублирующуюся команду
      _resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
    }
    //Serial.println(_resp);                                        // Дублируем ответ в монитор порта
  }
  return _resp;                                                   // Возвращаем результат. Пусто, если проблема
}

String waitResponse() {                                           // Функция ожидания ответа и возврата полученного результата
  String _resp = "";                                              // Переменная для хранения результата
  long _timeout = millis() + 10000;                               // Переменная для отслеживания таймаута (10 секунд)
  while (!SIM900.available() && millis() < _timeout)  {};         // Ждем ответа 10 секунд, если пришел ответ или наступил таймаут, то...
  if (SIM900.available()) {                                       // Если есть, что считывать...
    _resp = SIM900.readString();                                  // ... считываем и запоминаем
  }
  else {                                                          // Если пришел таймаут, то...
    //Serial.println("Timeout...");                                 // ... оповещаем об этом и...
  }
  return _resp;                                                   // ... возвращаем результат. Пусто, если проблема
}

void sendSMS(String phone_, String message_, boolean send1_, boolean send2_)
{
  Serial.println(memoryFree());
  if (send1_){
  sendATCommand("AT+CMGS=\"" + phone_ + "\"", true);             // Переходим в режим ввода текстового сообщения
  }
  //sendATCommand(mess  age_ + "\r" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z
  sendATCommand(message_, true);
    if (send2_){
    sendATCommand( (String)((char)26), true);
    }
  Serial.println(memoryFree());
}
