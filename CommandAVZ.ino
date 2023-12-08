
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
    T_temp_pusk=tmp.toInt();
    sendSMS(phone, "temp -"+tmp+" ustanovlena",true,true);
    EEPROM.write(3,T_temp_pusk);
   }
   //================остальные команды=================     
 switch(result.toInt()){
      case 9:
                            if (!eng_on and volt()<13.6) {
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

                              if (!eng_on and volt()<13.6) {
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
                                mun=true;
                                if (!eng_on and volt()<13.6){
                               
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
                                if (T_pusk==true) 
                                    {
                                      T_pusk = false;                                    
                                      EEPROM.write(2,T_pusk);
                                      sendSMS(phone, AVZ +"po temp otkl",true,true);
                                    }
                                else{
                                  T_pusk = true;                                    
                                  EEPROM.write(2,T_pusk);
                                  sendSMS(phone, AVZ+"po temp -"+String(T_temp_pusk)+" vkl",true,true);
                                }
                                break;
                case 6:
                                sendSMS(phone,gps_monitor(),true,true);
                                break;
                case 3:
                                  //======вкл/выкл автозапуска======
                                  if (avtopusk==true) 
                                    {sendSMS(phone,  AVZ+"yge rabotaet",true,true);}
                                  else{
                                  avtopusk=true;
                                  sendSMS(phone,  AVZ+String(every)+"m po "+String(progrev_min)+"m aktiv",true,true);
                                  atimer=millis();
                                  EEPROM.write(1,avtopusk);
                                  }
                                
                                  break;
                  case 4:       
                                //========информация об авто=========
                                //volt();
                                //Serial.println(memoryFree());

                                String txtok=AVZ;                                
                                if (!eng_on) tme=round(every-(millis()-atimer)/60000); //else tme=progrev_min-(millis()-atimer)/60000;
                                 else tme=round(progrev_min-(millis()-atimer)/60000);
                                 //sendSMS(phone,  txtok,true,false);
                                 //txtok="";
                                 //if(suc) {txtok=txtok+"Dvigatel zapushen. ";} else {txtok=txtok+"Dvigatel zaglushen. ";}
                                if(avtopusk) {
                                  txtok=txtok+"rabotaet t: "+tme+"m";
                                  } 
                                  else {
                                  txtok=txtok+" otklychen";
                                  }
                                  sendSMS(phone,  txtok,true,false);
                                  txtok="";
                                  if(eng_on and!avtopusk) txtok=txtok+" T: "+tme+"m";
                                  if (T_pusk){
                                    txtok=txtok+"\n"+AVZ+" po temp vkl pri -"+String(T_temp_pusk);                              
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
