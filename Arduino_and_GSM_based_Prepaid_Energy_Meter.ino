//Libraries///////////////////////////////////////////////////////////////////// 
#include <EEPROM.h> 
#include <SoftwareSerial.h> 
#include <LiquidCrystal.h> 

SoftwareSerial SIM900(8, 9);

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Suplementary variables and constants/////////////////////////////////////// 

float Voltage1 = A0;           //Defining and initializing the voltage
float Current1 = A1;          // Defining and initializing the current


float I1 = 0;
float V1 = 0;

double energyCount; 
double powerCount;
double sum_inst_power;
double inst_power=0;
 
const float frequencyPerKiloWatt = 0.8889 ; 

int contactor=  10; 
int buzzer = 13;

float recharged_amount =0.00f ; 
float remaining_units = 0.00f ; 
float total_consumption = 00.00f ; 
float monthly_consumption=0.00f; 
float last_recharge = 0.00f; 
float averagePower =0.00f; 
float temp_recharged=0.00f; //Unused variables//////////////////////////////////////////////////////////////////////////////// 
float supply_pf=1; //variables related to string decode//////////////////////////////////////////////////////////// 
int mon_reset=0; 

int n1 = 1; 
int n2 = 1; 
int d = 1; 
int r = 1; 
int t=1; 
long time_int=0; //Notifications//////////////////////////////////////////////////////////////////// 

char inchar; // Will hold the incoming character from the GSM shield

#define addr_recharged_amount 0 
#define addr_remaining_units 4 
#define addr_total_consumption 8 
#define addr_last_recharge 12 
#define addr_monthly_consumption 16 
#define addr_energy_count 20 //////////////////////////////////////////////////////////////////////////////////////////// 

long milisec = millis(); // calculate time in milliseconds
long time=milisec/1000; // convert milliseconds to seconds

void setup()
{ 

  pinMode(Voltage1,INPUT);        //Set voltage as input pin
  pinMode(Current1,INPUT);        //Set current as input pin
  
  SIM900.begin(9600); 
  Serial.begin(9600); 
  delay(100); 
  pinMode(contactor,OUTPUT); 
  pinMode(buzzer,OUTPUT); 
  
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(6,1);
  lcd.print("WELCOME");
  lcd.setCursor(5,2);
  lcd.print("Energy Meter");
 
  SIM900.print("AT\r\n");  // set SMS mode to text
  delay(1000);
  SIM900.print("AT+CMGF=1\r\n");  // set SMS mode to text
  delay(1000);
  SIM900.print("AT+CNMI=2,2,0,0,0\r\n"); 
  delay(1000);
  sim1(); 
  delay(1000);
  SIM900.print("ON GSM"); // Message contents
  delay(500);
  SIM900.write(byte(26)); // (signals end of message)
  delay(1000);
  SIM900.println("AT+CMGD=1,4\r\n"); // delete all SM
  delay(500);
  readfromEEPROM(); 
  } ////////////////////////////////////////////////////////////////////////////////////// 
  void loop() 
  {
  //If a character comes in from the cellular module...
  if(SIM900.available() >0)
  {
    inchar=SIM900.read(); 
    if (inchar=='R')
    {
 
      delay(10);
    inchar=SIM900.read(); 
      if (inchar=='U')
      {
        delay(10);
        inchar=SIM900.read();
        if (inchar=='1')
        {
       sim1();
       delay(1000);
       SIM900.println("Recharge Card 100"); // Message contents
       delay(500);
       SIM900.write(byte(26)); // (signals end of message)
       delay(500);  
        digitalWrite(contactor,HIGH); 
       prs = prs+100;
       unt = prs/5;
       con1=0;
       con2=0;
        }
        else if (inchar=='2')
        {
       sim1();
       delay(1000);
       SIM900.println("Recharge Card 200"); // Message contents
       delay(500);
       SIM900.write(byte(26)); // (signals end of message)
       delay(500);     
        digitalWrite(contactor,HIGH); 
       prs = prs+200;
       unt = prs/5;
       con1=0;
       con2=0;
        }
        else if (inchar=='3')
        {
       sim1();
       delay(1000);
       SIM900.println("Recharge Card 300"); // Message contents
       delay(500);
       SIM900.write(byte(26)); // (signals end of message)
       delay(500);   
        digitalWrite(contactor,HIGH); 
       prs = prs+300;
       unt = prs/5;
       con1=0;
       con2=0;
        }
         else if (inchar=='4')
        {
        sim1();
       delay(1000);
       SIM900.println("Recharge Card 400"); // Message contents
       delay(500);
       SIM900.write(byte(26)); // (signals end of message)
       delay(500);  
        digitalWrite(contactor,HIGH); 
       prs = prs+400;
       unt = prs/5;
       con1=0;
       con2=0;
        }
           else if (inchar=='4')
        {
       sim1();
       delay(1000);
       SIM900.println("Recharge Card 500"); // Message contents
       delay(500);
       SIM900.write(byte(26)); // (signals end of message)
       delay(500);    
        digitalWrite(contactor,HIGH); 
       prs = prs+500;
       unt = prs/5;
       con1=0;
       con2=0;
        }
   else if (inchar=='D')
        {
       sim1();
       delay(1000);
       SIM900.print("Unit="); // Message contents
       SIM900.println(unt); // Message contents
       SIM900.print("Price="); // Message contents
       SIM900.println(prs); // Message contents
       SIM900.print("Energy="); // Message contents
       SIM900.println(energy); // Message contents
       SIM900.print("Power="); // Message contents
       SIM900.println(power); // Message contents
       delay(500);
       SIM900.write(byte(26)); // (signals end of message)
       delay(500);    

       con1=0;
       con2=0;
        }
        delay(10);
       SIM900.println("AT+CMGD=1"); // delete all SMS
      
      }
    }
  }

     
 for (int n=0;n<1000;n++)
  {
  V1= ((analogRead(Voltage1)*(4.5/1023)*60));
  I1= ((analogRead(Current1)*(4.5/1023)*0.4545));
  inst_power=V1*I1; 
  sum_inst_power=sum_inst_power+abs(inst_power);
  delay(0.05);
  }
  
  powerCount =sum_inst_power/1000.0;
  time=millis();
  energyCount =(powerCount*time/1000)/3600;
    
  displayEnergyData(); 
  reconnection(); 
  energyCalculation(); 
  writeEngData(); 
} //pulse counterand calcounter////////////////////////////////////////////////////////////

  
  void reconnection()
  { 
    if(recharged_amount >= 6 && r==1 )
    { 
      r=0; 
      d = 1; 
      generateReconSignal(); 
    
      } 
      } //energyCalculation****************************************************************** ************** 
      
      void energyCalculation()
      { 
        if(recharged_amount>= 6.00)
        { 
          remaining_units += recharged_amount ; 
          last_recharge = recharged_amount ; 
          temp_recharged= recharged_amount ; 
          recharged_amount = 0.00; 
          } 
          if (energyCount >= 1)
          
          { 
          energyCount = 0 ; 
          monthly_consumption += 1; //0.1kWh = 1Wh 
          total_consumption += 1 ; //0.1kWh = 1Wh 
          remaining_units -= 1; //0.1kWh = 1Wh 
          }       
          }
 //Power calculation//////////////////////////////////////////////////////////////////////////Powercalculation with a maximum error of 1.25W 
 
 void powerCalculation()
 { 
  float averageFrequency = powerCount/(900); 
  averagePower = averageFrequency/frequencyPerKiloWatt ; 
  powerCount =0; 
  } //Display ////////////////////////////////////////////////////////////////////////////////// 
  
  
  
  void displayEnergyData()
  { 

    lcd.setCursor(0,1); 
    lcd.print(remaining_units); 
    delay(20); 
    lcd.setCursor(11,1); 
    lcd.print(last_recharge); 
    delay(20); 
    lcd.setCursor(0,2); 
    lcd.print(total_consumption); 
    delay(20); 
    lcd.setCursor(11,2); 
    lcd.print(monthly_consumption); 
    delay(20); 
    lcd.setCursor(0,3); 
    
    delay(20); 
    lcd.setCursor(11,3); 

    delay(20); 
    } //discon&recon////////////////////////////////////////////////////////////////////////////////////////// 
    void generateDiconSignal()
    { 
      digitalWrite(contactor,LOW); 
      } 
      
      void generateReconSignal()
      { 
        digitalWrite(contactor,HIGH); 
      } //Generating notification//////////////////////////////////////////////////////////////////// 

    
void month_reset() 
{ 
  if(mon_reset==1) 
  { 
    monthly_consumption=0; 
    } 
    } //////////////////////////////////////////////////////////////////////////////////// 
 //copying values from EEProm///////////////////////////////////////////////////// 
void readfromEEPROM()
{ 
  EEPROM.get(addr_recharged_amount,recharged_amount); 
  delay(200); 
  EEPROM.get(addr_remaining_units,remaining_units); 
  delay(200); 
  EEPROM.get(addr_total_consumption,total_consumption); 
  delay(200); 
  EEPROM.get(addr_last_recharge,monthly_consumption); 
  delay(200); 
  EEPROM.get(addr_monthly_consumption,last_recharge); 
  delay(200); 
  EEPROM.get(addr_energy_count,energyCount); 
  delay(200); 
  if(remaining_units>1) 
  { 
    generateReconSignal(); 
    } 
    
    } ///////////////////////put the serverevery 15 minits//////////////////////////////// 
    void time_check() 
    { 
      if(millis()<60000 && t==1) 
      { 
        time_int=0; 
        t=0; 
        } 
        if(millis()-time_int>60000) 
        { 
          time_int=millis(); 
          t=1; 
          powerCalculation(); 
          } 
          } //////////////////////////////////////write energy data/////////////////////////////////// 
          void writeEngData() 
          { 
            EEPROM.put(addr_recharged_amount,recharged_amount); 
            delay(200); 
            EEPROM.put(addr_remaining_units,remaining_units); 
            delay(200); 
            EEPROM.put(addr_total_consumption,total_consumption); 
            delay(200); 
            EEPROM.put(addr_last_recharge,last_recharge);
            delay(200); 
            EEPROM.put(addr_monthly_consumption,monthly_consumption); 
            delay(200); 
            EEPROM.put(addr_energy_count,energyCount); 
            delay(200); 
          }
           
void writeEngDataGSM() 
{ 
  EEPROM.put(addr_recharged_amount,recharged_amount); 
  delay(200); 
  EEPROM.put(addr_monthly_consumption,monthly_consumption); 
  delay(200); 
  } /////////shieldon/////////// 
  

void sim1()  
{
SIM900.println("AT+CMGS=\"+923378655465\"\r\n"); // Sends message  1  
}
