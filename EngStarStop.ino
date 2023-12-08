void eng_start(int raz,boolean pult ){      //запуск дрыгателя
  int tm=1;
  while (raz>=tm) {
   vin =volt();
  digitalWrite(key, HIGH);//вставляем ключ
  digitalWrite(stop_1, HIGH);//концевик тормоза
  delay(100);
  digitalWrite(start_stop, HIGH);
  delay(100);
  digitalWrite(start_stop, LOW);


  delay(3000);
  digitalWrite(stop_1, LOW);//концевик тормоза
  delay(2000);                                                        
  digitalWrite(key, LOW);//вытаскиваем ключ
  if (control_eng(vin)) suc=true; else suc=false;
 //if (volt()>vin+0.6) suc=true; else suc=false;                      
  if (suc) {
    if(!avtopusk or mun) 
    {
      sendSMS(phone, "Eng start!",true,false);
      sendSMS(phone, dht_sensor()+"\nt motora: "+String(DS18B20()),false,true);
    }
    if (!pult){
      eng_on=true;
      digitalWrite(key, LOW);
      atimer=millis();
    }
      Serial.print("завел с ");
      Serial.print(tm);
      Serial.println(" раза");
      break;
    }
  
  tm+=1;

  }
 if(!suc) {
  sendSMS(phone, "Ne zavel",true,false); 
  sendSMS(phone, "\nAKB: "+String(volt())+"v",false,false);
  sendSMS(phone, dht_sensor()+"\nt motora: "+String(DS18B20()),false,true);
  eng_stop(); 
  suc=false; 
  vc=false; }
 } 

//=======================процедура остановки мотора=================
  void eng_stop(){  
    //Serial.println("стоп ");
  digitalWrite(key, HIGH);
  delay(100); 
  digitalWrite(start_stop, HIGH);
  delay(100);
  digitalWrite(start_stop, LOW);
  delay(100);
  digitalWrite(key, LOW);
  delay(2000);
  digitalWrite(door_limit, HIGH);
  delay(1000);
  digitalWrite(door_limit, LOW);
  delay(500);
  digitalWrite(door_open, HIGH);
  delay(100);
  digitalWrite(door_open, LOW);
  eng_on=false;
  //btimer=NAN;
  atimer=NAN;
  avtopusk=false;
  mun=false;
  sendSMS(phone, "Eng stop! "+AVZ +"otkl",true,true);
  EEPROM.write(1,avtopusk);
  
}
  float volt(){
    int i = 0;
    float v=0;
    while (i<3){
      
    v = v+(((analogRead(v_in_pin) * 5.0) / 1024.0) / (R2/(R1+R2)))*kv;
    //if (v<5.00)  v=0.0;// обнуляем нежелательное значение
    i=i+1;
    }
    v=v/3;
    return v;
 }
 bool control_eng(float v){
  bool stat = false;
  if (volt()>=v+0.9){
    stat = true;
    //Serial.println("перв. провер.");
  }
  else {
    int i=0;
    int j=0;
    float v_in=0;
    while (i<2)
    {
      while (j<3)
      {
        v_in = v_in + volt();
        delay(1000);
        j=j+1;
      }
      j=0;
      v_in=v_in/3;
      //Serial.print("контроль ");
      //Serial.println(i);
      if (v_in>v+0.5) {
        stat = true;
        break;       
        }
      delay(1000);
      v_in=0;
      i=i+1;
      
    }
    
  }
  return stat;
 }
