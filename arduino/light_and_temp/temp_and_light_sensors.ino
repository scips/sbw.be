#define SAMPLE_SIZE 11
#define SAMPLE_BY   10
#define DELAY       500
//#define DEBUG

#include <LiquidCrystal.h>

int LightSensor = 2; //define the 1th analog pin for light sensor brick 
int TemperatureSensor = 1;
int MoistureSensor = 3;
int Button = 9;
int lights[SAMPLE_SIZE];
int temps[SAMPLE_SIZE];
int moistures[SAMPLE_SIZE];
int indice = 0;
float meanLight = 0.0;
float meanTemp = 0.0;
float meanMoisture = 0.0;
int minTemp = 1024;
int maxTemp = 0;
int minMoisture = 1024;
int maxMoisture = 0;
int cpt=0;
int buttonPressed=false;
int buttonPreviousStatus=false;
int displayMode=0;

LiquidCrystal lcd(2,3,4,5,6,7,8);

void setup() { 
  lcd.begin(16, 2);
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  pinMode(Button,INPUT);
}

void loop() { 
  printRemainingTimeBeforeRefreshToLCD();
  int valLight=0;
  int valTemperature=0;
  int valMoisture=0;
  valLight=analogRead(LightSensor); //Read analog level which match the luminous intensity 
  valTemperature=analogRead(TemperatureSensor);
  valMoisture=analogRead(MoistureSensor);
  buttonPressed=digitalRead(Button);
  
  if(valTemperature>maxTemp){
    maxTemp = valTemperature;
  }else{
    if(valTemperature<minTemp){
      minTemp = valTemperature;
    }
  }
  
  if(valMoisture>maxMoisture){
    maxMoisture = valMoisture;
  }else{
    if(valMoisture<minMoisture){
      minMoisture = valMoisture;
    }
  }
  
  if(buttonPressed && buttonPreviousStatus==false){
    if(displayMode<3){
      displayMode++;
    }else{
      displayMode=0;
    }
    switch(displayMode){
      case 0:
        printArrays();
        break;
      case 1:
        printTemperatureValues(valTemperature,minTemp,maxTemp);
        break;
      case 2:
        // humidity min/max
        printMoistureValues(valMoisture,minMoisture,maxMoisture);
        break;
    }
  }
  

  if(buttonPressed){
    buttonPressed = false;
    buttonPreviousStatus = true;
  }else{
    buttonPreviousStatus = false;
  }
  storeInArray(valLight,valTemperature,valMoisture);
  if(cpt>=SAMPLE_BY-1){
    cpt=0;
    printArrays();
  }else{
    cpt++;
  }
  #ifdef DEBUG
  Serial.print(".");
  #endif
  delay(DELAY);
}

void storeInArray(int light,int temp, int moisture) {
  int nextItem;
  // loop continuously in the array
  if(indice>=SAMPLE_SIZE){
    indice=0;
  }
  lights[indice]=light;
  temps[indice]=temp;
  moistures[indice]=moisture;
  indice++;
  nextItem=indice;
  if(nextItem>=SAMPLE_SIZE){
    nextItem=0;
  }
  lights[nextItem]=0;
  temps[nextItem]=0;
  moistures[nextItem]=0;
}

void printArrays() {
  meanLight=0.0;
  meanTemp=0.0;
  meanMoisture=0;0;
  int tempCount=0;
  int lightCount=0;
  int moistureCount=0;
  int i=0;
  #ifdef DEBUG
  Serial.println();
  #endif
  for(i=0;i<SAMPLE_SIZE;i++){
    if(lights[i]>0) {
      meanLight=meanLight+lights[i];
      lightCount++;
    }
  }
  meanLight=meanLight/lightCount;
  #ifdef DEBUG
  Serial.print("Mean Light (");
  Serial.print(lightCount);
  Serial.print("):");
  Serial.println(meanLight);
  #endif
  for(i=0;i<SAMPLE_SIZE;i++){
    if(temps[i]>0){
      meanTemp+=temps[i];
      tempCount++;
    }
  }
  meanTemp = meanTemp/tempCount;
  String temperature=convert2HumanReadableTemp(meanTemp);
  #ifdef DEBUG
  Serial.print("Mean Temp (");
  Serial.print(tempCount);
  Serial.print("):");
  Serial.print(meanTemp);
  Serial.print("/");
  Serial.println(temperature);
  #endif
  for(i=0;i<SAMPLE_SIZE;i++){
    if(moistures[i]>0){
      meanMoisture+=moistures[i];
      moistureCount++;
    }
  }
  meanMoisture = meanMoisture/moistureCount;
  printInfoToLCD(temperature,meanLight,meanMoisture);
}

void printTemperatureValues(int nowTemperature, int minTemp, int maxTemp){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("T: ");
  lcd.print(convert2HumanReadableTemp(minTemp));
  lcd.print("<");
  lcd.print(convert2HumanReadableTemp(nowTemperature));
  lcd.setCursor(0,1);
  lcd.print("<");
  lcd.print(convert2HumanReadableTemp(maxTemp));
}

void printMoistureValues(int valMoisture,int minMoisture,int maxMoisture){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("H: ");
  lcd.print(minMoisture);
  lcd.print("<");
  lcd.print(valMoisture);
  lcd.setCursor(0,1);
  lcd.print("<");
  lcd.print(maxMoisture);
}

void printRemainingTimeBeforeRefreshToLCD(){
  lcd.setCursor(14,0);
  String countdown="00";
  int count=SAMPLE_BY-cpt-1;
  int tenth=count/10;
  int unit =count-(10*tenth);
  countdown[0]='0'+tenth;
  countdown[1]='0'+unit;
  lcd.print(countdown);
}

void printInfoToLCD(String temperature,float meanLigth,float meanMoisture){
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print(char(223));
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("L:");
  lcd.print(meanLight);
  lcd.print(" H:");
  lcd.print(meanMoisture);
}

String convert2HumanReadableTemp(float temp){
  // 480 = 20°C
  // 540 = 25°C ==> +60 = +5°C <=> +12 = +1°C & 240 = 0
  int tempReadable = int((temp-240)/12*10);
  String temperature = "+00.0";
  if(tempReadable<0){
    temperature[0]='-';
  }
  Serial.print("Debug:");
  Serial.print(tempReadable);
  int tenth = int(tempReadable/100.0);
  int unit = int((tempReadable-(tenth*100))/10.0);
  int dec = int( tempReadable-(tenth*100)-(unit*10) );
  temperature[1]='0'+tenth;
  temperature[2]='0'+unit;
  temperature[4]='0'+dec;
  return temperature;
}
