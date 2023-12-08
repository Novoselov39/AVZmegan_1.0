
String dht_sensor(){
  float h = 0;
  float t1 = 0;
  int i=0;
  while (i<1){
    dht.read();
   t1 = h+dht.tem;
  h =t1+dht.hum;
  //dht.debug();
 // h = h+dht.getTemperature();
  //t1 =t1+dht.getHumidity();
  //Serial.println("проверочка");
  i=i+1;
  }
  String dht_val="\nt1: "+String(t1/1)+" h: "+String(h/1)+" %";
  return dht_val;
  dht_val="";
}
float DS18B20(){
  int i=0;
  float tm=0;
  while (i<1){
    byte data[2];
    ds.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
    ds.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
    ds.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
    delay(1000); // Микросхема измеряет температуру, а мы ждем.  
  
    ds.reset(); // Теперь готовимся получить значение измеренной температуры
    ds.write(0xCC); 
    ds.write(0xBE); // Просим передать нам значение регистров со значением температуры
  
    data[0] = ds.read(); // Читаем младший байт значения температуры
    data[1] = ds.read(); // А теперь старший
  
    tm = tm+((data[1] << 8) | data[0]) * 0.0625;
    //Serial.println("проверочка 2");
    i=i+1;
  
  }
  //String DS18B20_val="\ntm: "+String(tm/1);
  return tm;
  //DS18B20_val="";
}
