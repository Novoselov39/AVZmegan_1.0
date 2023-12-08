
void commendAVZ (String result) {
  //================help=================
if (result=="Help" or result=="help") {
         sendSMS(phone,  "1-pusk\n2-stop\n3-"+AVZ,true,false);  // HEEELP
         sendSMS(phone,  "4-info\n5-"+AVZ+"_temp\n6-gps\n8-open\n9-close",false,true);
         
    }
  //================время автозапуска=================
    if (result[3]=='h') {
    String tmp=String(result[0])+String(result[1])+String(result[2]);
        every=tmp.toInt();
        if (every>255) every=255;
        tmp=String(result[4])+String(result[5])+String(result[6]);
        progrev_min=tmp.toInt();
        result[3]='-';
        sendSMS(phone, "vremya progreva izmeneno",true,true); 
         EEPROM.write(0,every);
         EEPROM.write(4,progrev_min);
        }
   //================запуск по температуре=================
   if (result[0]=='t' or result[0]=='T') {
    String tmp=String(result[1])+String(result[2]);
    TTempPusk=tmp.toInt();
    sendSMS(phone, "temp -"+tmp+" ustanovlena",true,true);
    EEPROM.write(3,TTempPusk);
   }
   //================остальные команды=================     
 switch(result.toInt()){
      case 9:
                            if (!eng_on and volt()<vP) {
                                digitalWrite(start_stop, HIGH);
                                delay(500);
                                digitalWrite(start_stop, LOW);
                                digitalWrite(door_limit, HIGH);
                                delay(500);
                                digitalWrite(door_limit, LOW);
                                delay(500);
                                digitalWrite(door_open, HIGH);
                                delay(500);
                                digitalWrite(door_open, LOW);
                                sendSMS(phone,"close",true,true);
                                 }
                                else {
                                digitalWrite(door_limit, HIGH);
                                delay(500);
                                digitalWrite(door_limit, LOW);
                                delay(500);
                                digitalWrite(door_open, HIGH);
                                delay(500);
                                digitalWrite(door_open, LOW);
                                sendSMS(phone,"close",true,true);
                                }
                                 break;
                                 
       case 8:
                                //======открываем дверь дверь======

                              if (!eng_on and volt()<vP) {
                                  digitalWrite(key, HIGH);
                                  delay(500);
                                  digitalWrite(start_stop, HIGH);
                                  delay(500);
                                  digitalWrite(start_stop, LOW);
                                  delay(1500);
                                  digitalWrite(key, LOW);
                                  delay(500);
                                  digitalWrite(door_limit, HIGH);
                                  delay(500);
                                  digitalWrite(door_limit, LOW);                       
                                  sendSMS(phone,"open",true,true);
                                }
                                else {
                                  digitalWrite(door_open, HIGH);
                                  delay(500);
                                  digitalWrite(door_open, LOW);
                                  sendSMS(phone,"open",true,true);                               
                                }
                                break;
                case 1:
                                //======запуск мотора======
                                //manual=true;
                                if (!eng_on and volt()<vP){
                               
                                 eng_start(3,false);
 
                                }
                                else{
                                  sendSMS(phone,  "Mot rab",true,true);
                                  
                                }
                                
                                break;
                case 2:
                                //======остановка мотора======
                                eng_stop();
                                break;
                case 5:
                                //======остановка мотора======
                                if (TPusk==true) 
                                    {
                                      TPusk = false;                                    
                                      EEPROM.write(2,TPusk);
                                      sendSMS(phone, AVZ +"po temp otkl",true,true);
                                      TTiimer=NAN;
                                    }
                                else{
                                  TPusk = true;                                    
                                  EEPROM.write(2,TPusk);
                                  sendSMS(phone, AVZ+"po temp -"+String(TTempPusk)+" vkl",true,true);
                                  TTiimer=millis();
                                }
                                break;
                case 6:
                                sendSMS(phone,gps_monitor(),true,true);
                                break;
                case 3:
                                  //======вкл/выкл автозапуска======
                                  if (avtopusk) 
                                    {sendSMS(phone,  AVZ+"yge rabotaet",true,true);}
                                  else{
                                  avtopusk=true;
                                  sendSMS(phone,  AVZ+String(every)+"m po "+String(progrev_min)+"m aktiv",true,true);
                                  atoPuskTimer=millis();
                                  EEPROM.write(1,avtopusk);
                                  }
                                
                                  break;
                  case 4:       
                                //========информация об авто=========
                                
                                //Serial.println(memoryFree());

                                String txtok=AVZ;                                
                                if (!eng_on) tme=round(every-(millis()-atoPuskTimer)/60000);
                                 else tme=round(progrev_min-(millis()-atoPuskTimer)/60000);
                                
                                if(avtopusk) {
                                  txtok=txtok+"rabotaet t: "+tme+"m";
                                  } 
                                  else {
                                  txtok=txtok+" otklychen";
                                  }
                                  sendSMS(phone,  txtok,true,false);
                                  txtok="";
                                  if(eng_on and!avtopusk) txtok=txtok+" T: "+tme+"m";
                                  if (TPusk){
                                    txtok=txtok+"\n"+AVZ+" po temp vkl pri -"+String(TTempPusk);                              
                                  }                                  
                                  txtok=txtok+"\nAKB: "+String(vin)+"v";
                                  sendSMS(phone,  txtok,false,false);
                                  txtok="";
                                  txtok=txtok+dht_sensor()+"\nt motora: "+String(DS18B20());
                                  sendSMS(phone,  txtok,false,true);
                                  txtok="";
                                  //Serial.println(memoryFree());
                                  //Serial.println(txtok);            
                                  //sendSMS(phone,  txtok,true,true);
                                              
                                  
                                 


                                break;                            
  
 }
  //Serial.println("проверочка");
  //Serial.println(result);
}
