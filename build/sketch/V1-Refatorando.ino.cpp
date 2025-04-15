#include <Arduino.h>
#line 1 "C:\\Users\\VCP2909\\Desktop\\Carrinho_OBR\\Programação OBR_Arduino\\2025\\V1-Refatorando\\V1-Refatorando\\V1-Refatorando.ino"

int verificacaoCurva = 10;
int verificacaoCurvaA = 25;
int verificacaoCurvaVerde = 125;
int subtracaoSensoresCor = 175;
int tempoPulinhoEspecial = 15;
int erroGiro = 5;
int erroGiro2 = 5;
int espera = 25; //x 10
int tempoDepoisDoVerde = 150;
int valorCnoBranco = 500;

int minLuxVerde = 87;
int maxLuxverde = 200;

int minLuxCinza = 200;
int maxLuxCinxa = 400;
int maxCNoCinza = 1350;
int minCNoCinza = 750;

int distancia1, distancia2, anguloEntrada, esquerda90, direita90, tamanhoTotal, ladoMaior;

#include "Servo.h"
#include "Adafruit_TCS34725softi2c.h"
#include "Wire.h"
#include <MPU6050_light.h>
#include <Ultrasonic.h>

#define SDApin1 2 //Sensor Esquerda
#define SCLpin1 3 //Sensor Esquerda
#define SDApin2 4 //Sensor Direita
#define SCLpin2 5 //Sensor Direita 

#define motorEpin 6 //Servo Esquerdo
#define motorDpin 7 //Servo Direito

int trigPin1=10; //Ultrasonico esquerda
int echoPin1=11; //Ultrasonico esquerda
int trigPin2=8; //Ultrasonico frente
int echoPin2=9; //Ultrasonico frente

#define SE 22 //Sensor Esquerdo
#define SME 23 //Sensor Meio Esquerdo
#define SM 24 //Sensor Meio
#define SMD 25 //Sensor Meio Direito
#define SD 26 //Sensor Direito

// Inicializar duas instâncias do sensor TC34725
Adafruit_TCS34725softi2c tcs1 = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X, SDApin2, SCLpin2);
Adafruit_TCS34725softi2c tcs2 = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X, SDApin1, SCLpin1);

//Cria dois obejetos servos
Servo motorE;
Servo motorD;

//Cria o objeto do giroscópio
MPU6050 giro(Wire);
unsigned long timer = 0;

int sensores[] = {SE,SME,SM,SMD,SD};
static int valores[5];

uint16_t r1, g1, b1, c1, lux1, r2, g2, b2, c2, lux2;
long duration1, distance1, duration2, distance2;


int diferencaDasCores = 4;
int diferencaDasCoresVermelho = 50;
int erro = 2;
int anguloRampaSubida, anguloRampaDescida, anguloDoReto, anguloReto;
int* sl;
boolean trava = false;

//****************************
//*         Funções          *
//****************************

int* lerSensoresLinha(){
  for(byte i = 0; i < 5; i++){
    valores[i] = digitalRead(sensores[i]);
  }
  return valores;
}

int lerVerde(){ //Função de retorno para analizar apenas
  tcs1.getRawData(&r1, &g1, &b1, &c1);
  lux1 = tcs1.calculateLux(r1, g1, b1);
  tcs2.getRawData(&r2, &g2, &b2, &c2);
  lux2 = tcs2.calculateLux(r2, g2, b2);
  Serial.println("*Sensor 01 Dirieta*");
  Serial.print("Lux: "); Serial.print(lux1, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r1, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g1, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b1, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c1, DEC); Serial.print(" ");
  Serial.println(" ");

  //Serial.print("Sensor 2: Color Temp: "); Serial.print(colorTemp2, DEC); Serial.print(" K - "); da bug nem tenta kkk
  Serial.println("*Sensor 02 Esquerda*");
  Serial.print("Lux: "); Serial.print(lux2, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r2, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g2, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b2, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c2, DEC); Serial.print(" ");
  Serial.println(" ");
}

void verificaVermelho(){//Se vermelho para, senão segue reto
  tcs1.getRawData(&r1, &g1, &b1, &c1);
  lux1 = tcs1.calculateLux(r1, g1, b1);
  tcs2.getRawData(&r2, &g2, &b2, &c2);
  lux2 = tcs2.calculateLux(r2, g2, b2);
  Serial.println("Verificando vermelho");
  if((r1 > g1 && r1 > b1 && r1 - g1 > diferencaDasCoresVermelho && r1 - b1 > diferencaDasCoresVermelho) || (r2 > g2 && r2 > b2 && c2 <= valorCnoBranco) && (r2 > g2 && r2 > b2 && r2 - g2 > diferencaDasCoresVermelho && r2 - b2 > diferencaDasCoresVermelho)){
    while(true){
      Serial.println("VERMELHO");
      motorE.write(90);
      motorD.write(90);
    }
  }else{
    lerCinza();
    return;
  }
}

void lerVerde2(){
  tcs1.getRawData(&r1, &g1, &b1, &c1);
  lux1 = tcs1.calculateLux(r1, g1, b1);
  tcs2.getRawData(&r2, &g2, &b2, &c2);
  lux2 = tcs2.calculateLux(r2, g2, b2);
  
  if(c1 <= valorCnoBranco || c2 <= valorCnoBranco){
    if((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g1 - r1 > diferencaDasCores && g1 - b1 > diferencaDasCores) && (g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (g2 - r2 > diferencaDasCores && g2 - b2 > diferencaDasCores)){
      Serial.println("180°");
    }else if((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g1 - r1 > diferencaDasCores && g1 - b1 > diferencaDasCores)){
      Serial.println("Verde na Direita!");
      Serial.println("*Sensor 01 Direita*");
      Serial.print("Lux: "); Serial.print(lux1, DEC); Serial.print(" - ");
      Serial.print("R: "); Serial.print(r1, DEC); Serial.print(" ");
      Serial.print("G: "); Serial.print(g1, DEC); Serial.print(" ");
      Serial.print("B: "); Serial.print(b1, DEC); Serial.print(" ");
      Serial.print("C: "); Serial.print(c1, DEC); Serial.print(" ");
      Serial.println(" ");
      Serial.println("Direita");
    }else if((g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (g2 - r2 > diferencaDasCores && g2 - b2 > diferencaDasCores)){
      Serial.println("Verde na Esquerda!");
      Serial.println("*Sensor 02 Esquerda*");
      Serial.print("Lux: "); Serial.print(lux2, DEC); Serial.print(" - ");
      Serial.print("R: "); Serial.print(r2, DEC); Serial.print(" ");
      Serial.print("G: "); Serial.print(g2, DEC); Serial.print(" ");
      Serial.print("B: "); Serial.print(b2, DEC); Serial.print(" ");
      Serial.print("C: "); Serial.print(c2, DEC); Serial.print(" ");
      Serial.println(" ");
      Serial.println("Esquerda");
    }
  }else{
    Serial.println("Branco");
    return false;
  }
}

bool lerVerde1(){ //Função que verifica se tem verde, retorna True ou False
  tcs1.getRawData(&r1, &g1, &b1, &c1);
  lux1 = tcs1.calculateLux(r1, g1, b1);
  tcs2.getRawData(&r2, &g2, &b2, &c2);
  lux2 = tcs2.calculateLux(r2, g2, b2);
  
  if(c1 <= valorCnoBranco || c2 <= valorCnoBranco){
    if((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g1 - r1 > diferencaDasCores && g1 - b1 > diferencaDasCores) && (g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (g2 - r2 > diferencaDasCores && g2 - b2 > diferencaDasCores)){
      return true;
    }else if((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g1 - r1 > diferencaDasCores && g1 - b1 > diferencaDasCores)){
      Serial.println("Verde na Direita!");
      Serial.println("*Sensor 01 Direita*");
      Serial.print("Lux: "); Serial.print(lux1, DEC); Serial.print(" - ");
      Serial.print("R: "); Serial.print(r1, DEC); Serial.print(" ");
      Serial.print("G: "); Serial.print(g1, DEC); Serial.print(" ");
      Serial.print("B: "); Serial.print(b1, DEC); Serial.print(" ");
      Serial.print("C: "); Serial.print(c1, DEC); Serial.print(" ");
      Serial.println(" ");
      return true;
    }else if((g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (g2 - r2 > diferencaDasCores && g2 - b2 > diferencaDasCores)){
      Serial.println("Verde na Esquerda!");
      Serial.println("*Sensor 02 Esquerda*");
      Serial.print("Lux: "); Serial.print(lux2, DEC); Serial.print(" - ");
      Serial.print("R: "); Serial.print(r2, DEC); Serial.print(" ");
      Serial.print("G: "); Serial.print(g2, DEC); Serial.print(" ");
      Serial.print("B: "); Serial.print(b2, DEC); Serial.print(" ");
      Serial.print("C: "); Serial.print(c2, DEC); Serial.print(" ");
      Serial.println(" ");
      return true;
    }
  }else{
    return false;
  }
}

void lerCinza(){
  int cc1 = (int)c1;
  int cc2 = (int)c2;
  int luu1 = (int)lux1;
  int luu2 = (int)lux2;
  
  Serial.println("*Sensor 01 Direita*");
  Serial.print("Lux: "); Serial.print(lux1, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r1, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g1, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b1, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c1, DEC); Serial.print(" ");
  Serial.println(" ");
  Serial.println("*Sensor 02 Esquerda*");
  Serial.print("Lux: "); Serial.print(lux2, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r2, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g2, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b2, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c2, DEC); Serial.print(" ");
  Serial.println(" ");

  Serial.print("lux1 > 200 && lux1 < 325  = "); Serial.println(luu1 > minLuxCinza && luu1 < maxLuxCinxa);
  Serial.print("lux2 > 200 && lux2 < 325 = "); Serial.println(luu2 > minLuxCinza && luu2 < maxLuxCinxa);
  Serial.print("c1 < 1350 && c2 < 1350 = "); Serial.println(c1 < maxCNoCinza && c2 < maxCNoCinza);
  Serial.print("c1 > 750 && c2 > 750= "); Serial.println(c1 > minCNoCinza && c2 > minCNoCinza);

  if((luu1 > minLuxCinza && luu1 < maxLuxCinxa) && (luu2 > minLuxCinza && luu2 < maxLuxCinxa) && (c1 < maxCNoCinza && c2 < maxCNoCinza) && (c1 > minCNoCinza && c2 > minCNoCinza)){
    Serial.println("");
    Serial.println("CINZAAAAA");
    Serial.println("");
    motorE.write(90);
    motorD.write(90);

    anguloEntrada = retornoAnguloZ();

    distancia1 = retornoSensorFrente();
    distancia2 = distancia1 - 20;
    Serial.println(" **PASSO 1** ");
    Serial.print("Distancia 1 ="); Serial.print(distancia1); Serial.print(" Distancia 2 = "); Serial.println(distancia2);
    while(retornoSensorFrente() >= distancia2){
      motorE.write(180);
      motorD.write(0);
      giro.update();
    }
    motorE.write(90);
    motorD.write(90);

    Serial.println(" **PASSO 2** ");//Girar para Esquerda
    motorE.write(30);
    motorD.write(30);
    while((anguloEntrada + 85) >= retornoAnguloZ()){
      giro.update();
      Serial.print("90° esquerda = "); Serial.println(retornoAnguloZ());
    }
    motorE.write(90);
    motorD.write(90);
    esquerda90 = retornoSensorFrente();
    Serial.print("Valor esquerda = "); Serial.println(esquerda90);

    Serial.println(" **PASSO 3** ");//Girar para Direita
    motorE.write(150);
    motorD.write(150);
    while((anguloEntrada - 85) <= retornoAnguloZ()){
      giro.update();
      Serial.print("90° direita = "); Serial.println(retornoAnguloZ());
    }
    motorE.write(90);
    motorD.write(90);
    direita90 = retornoSensorFrente();
    Serial.print("Valor direita = "); Serial.println(direita90);

    ladoMaior = (esquerda90 > direita90) ? 1 : 2; //1 = esquerda; 2 = direita;
    tamanhoTotal = direita90 + esquerda90 + 20;
    Serial.print("Tamanho total = "); Serial.println(tamanhoTotal);
    Serial.print("Lado maior = "); Serial.print(ladoMaior); Serial.print(" Meio = "); Serial.println((tamanhoTotal/2) - 10);

    Serial.println(" **PASSO 4** ");

    if(ladoMaior == 2){
      while(retornoSensorFrente() >= ((tamanhoTotal/2) - 10)){
        Serial.println("Indo para frente");
        motorE.write(135);
        motorD.write(50);
      }

      motorE.write(45);
      motorD.write(50);
      while((anguloEntrada) >= retornoAnguloZ()){
        giro.update();
        Serial.print("90° Esquerda = "); Serial.println(retornoAnguloZ());
      }

      motorE.write(90);
      motorD.write(90);


    }else if(ladoMaior == 1){
      motorE.write(30);
      motorD.write(30);
      while((anguloEntrada + 85) >= retornoAnguloZ()){
        giro.update();
        Serial.print("90° esquerda = "); Serial.println(retornoAnguloZ());
      }

      motorE.write(90);
      motorD.write(90);

      Serial.println(" **PASSO 5** ");

      while(retornoSensorFrente() >= ((tamanhoTotal/2) - 10)){
        Serial.println("Indo para frente");
        Serial.println(retornoSensorFrente());
        motorE.write(135);
        motorD.write(50);
      }

      motorE.write(135);
      motorD.write(125);
      Serial.println(" "); Serial.print("AnguloEntrada = "); Serial.print(anguloEntrada); Serial.println(" ");
      while((anguloEntrada) <= retornoAnguloZ()){
        giro.update();
        Serial.print("90° Direita = "); Serial.println(retornoAnguloZ());
      }
      motorE.write(90);
      motorD.write(90);
    }

    Serial.println(" **PASSO 6** ");

    while(retornoSensorFrente() >= ((distancia1 / 2)-10)){
      Serial.println("Indo pro meio");
      Serial.print("Distancia = "); Serial.println(retornoSensorFrente());
      motorE.write(135);
      motorD.write(50);
    }

    Serial.println(" **PASSO 7** ");

    motorE.write(115);
    motorD.write(110);
    //Gira para a direita, busacando a saida
    Serial.println(" "); Serial.print("AnguloEntrada = "); Serial.print(anguloEntrada); Serial.println(" ");
    while((anguloEntrada - 85 <= retornoAnguloZ()) && (retornoSensorFrente() < 100)){
      giro.update();
      Serial.print("90° Direita = "); Serial.print(retornoAnguloZ()); Serial.print(" Distância frente = "); Serial.println(retornoSensorFrente());
    }
    Serial.print("Frente = ");Serial.println(retornoSensorFrente());
    motorE.write(90);
    motorD.write(90);
    //Se achou vai atrás
    if(retornoSensorFrente() >= 100){
      Serial.println("Saida encontrada");
      sl = lerSensoresLinha();

      while(sl[2]==1){
        Serial.println(retornoSensorFrente());
        sl = lerSensoresLinha();
        
        while(anguloEntrada >= retornoAnguloZ()){
          Serial.println("Ficando reto da direita");
          motorE.write(50);
          motorD.write(55);
        }
        motorE.write(90);
        motorD.write(90);

        while(retornoSensorFrente() > 20){
          Serial.println(retornoSensorFrente());
          Serial.print(retornoAnguloZ());
          Serial.println("Jutando na parede 2");
          motorE.write(170);
          motorD.write(15);
        }

        while(anguloEntrada - 85 >= retornoAnguloZ()){
          Serial.println(retornoSensorFrente());
          Serial.println(retornoAnguloZ());
          Serial.println("Virando pra sair 2");
          motorE.write(120);
          motorD.write(115);
        }
        sl = lerSensoresLinha();

        while(sl[2]==1){
          sl = lerSensoresLinha();
          motorE.write(170);
          motorD.write(15);
          sl = lerSensoresLinha();
        }
      }
    }else{
      motorE.write(70);
      motorD.write(75);
      //Vira para a esquerda busacando a saida
      Serial.println(" "); Serial.print("AnguloEntrada = "); Serial.print(anguloEntrada); Serial.println(" ");
      while((anguloEntrada + 85 >= retornoAnguloZ()) && (retornoSensorFrente() < 100)){
        giro.update();
        Serial.print("90° Esquerda = "); Serial.print(retornoAnguloZ()); Serial.print(" Distância frente = "); Serial.println(retornoSensorFrente());
      }

      motorE.write(90);
      motorD.write(90);
      int frente = retornoSensorFrente();
      Serial.print("Frente = ");Serial.println(retornoSensorFrente());
      //Se achou sai
      if(frente >= 100){
        Serial.println("Saida encontrada");
        sl = lerSensoresLinha();

        while(sl[2]==1){
          Serial.println(retornoSensorFrente());
          sl = lerSensoresLinha();
          while(anguloEntrada <= retornoAnguloZ()){
            Serial.println("Ficando reto da esquerda");
            motorE.write(130);
            motorD.write(125);
          }
          motorE.write(90);
          motorD.write(90);
          
          while(retornoSensorFrente() > 20){
            Serial.println(retornoSensorFrente());
            Serial.print(retornoAnguloZ());
            Serial.println("Jutando na parede 1");
            motorE.write(170);
            motorD.write(15);
          }

          while(anguloEntrada + 85 >= retornoAnguloZ()){
            Serial.println(retornoSensorFrente());
            Serial.println(retornoAnguloZ());
            Serial.println("Virando pra sair 1");
            motorE.write(70);
            motorD.write(75);
          }
          sl = lerSensoresLinha();

          while(sl[2]==1){
            sl = lerSensoresLinha();
            motorE.write(170);
            motorD.write(15);
            sl = lerSensoresLinha();
          }
        }
      }
    }
    motorE.write(90);
    motorD.write(90);
  }
}

int* lerGiroscopio(){ //Não usa kkk
  static int valores[3];
  giro.update();
  valores[0] = giro.getAngleX();
  valores[1] = giro.getAngleY();
  valores[2] = giro.getAngleZ();

  return valores;
}

int retornoAnguloZ(){
  giro.update();
  return giro.getAngleZ();
}

int retornoAnguloY(){
  giro.update();
  return giro.getAngleY();
}

void giroVerde(){
  motorE.write(180);
  motorD.write(0);
  giro.update();
  delay(verificacaoCurvaVerde);
  giro.update();
  motorE.write(90);
  motorD.write(90);
  giro.update();
  
  tcs1.getRawData(&r1, &g1, &b1, &c1);
  lux1 = tcs1.calculateLux(r1, g1, b1);
  tcs2.getRawData(&r2, &g2, &b2, &c2);
  lux2 = tcs2.calculateLux(r2, g2, b2);

  Serial.println("*Sensor 01 Direita*");
  Serial.print("Lux: "); Serial.print(lux1, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r1, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g1, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b1, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c1, DEC); Serial.print(" ");
  Serial.println(" ");
  Serial.println("*Sensor 02 Esquerda*");
  Serial.print("Lux: "); Serial.print(lux2, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r2, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g2, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b2, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c2, DEC); Serial.print(" ");
  Serial.println(" ");

    int cc1 = (int)c1;
    int cc2 = (int)c2;
    int luu1 = (int)lux1;
    int luu2 = (int)lux2;
    Serial.print("lux1 > 125 && lux1 < 200  = "); Serial.println(luu1 > minLuxVerde && luu1 < maxLuxverde);
    Serial.print("lux2 > 125 && lux2 < 200 = "); Serial.println(luu2 > minLuxVerde && luu2 < maxLuxverde);
    Serial.print("c1 - c2 = "); Serial.println(abs(cc1-cc2) < subtracaoSensoresCor);
    Serial.print("Direita "); Serial.println((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && c1 < c2);
    Serial.print("Esquerda "); Serial.println((g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && c2 < c1);
    Serial.print("180° "); Serial.println((abs(cc1 - cc2) < subtracaoSensoresCor) && (g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (luu1 > minLuxVerde && luu1 < maxLuxverde) && (luu2 > minLuxVerde && luu2 < maxLuxverde));

  if(c1 <= valorCnoBranco || c2 <= valorCnoBranco){
    Serial.println("Possivel verde GV1");
    giro.update();
    if((abs(cc1 - cc2) < subtracaoSensoresCor) && (g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (luu1 > minLuxVerde && luu1 < maxLuxverde) && (luu2 > minLuxVerde && luu2 < maxLuxverde)){
      giro.update();
      Serial.println("VERDE!! Curva 180°");
      motorE.write(180);
      motorD.write(0);
      giro.update();
      delay(75);
      giro.update();
      anguloReto = retornoAnguloZ();
      motorE.write(180);
      motorD.write(180);
      while(anguloReto - (180 - erroGiro) <= retornoAnguloZ()){
        giro.update();
        Serial.print("VERDE!! Fazendo curva 180° | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto - 180);
      }
      motorE.write(90);
      motorD.write(90);
    }else if((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (c1 < c2) && (luu1 > minLuxVerde && luu1 < maxLuxverde)){
      giro.update();
      Serial.println("Verde na Direita!");
      motorE.write(180);
      motorD.write(0);
      giro.update();

      for(int i=0;i<espera;i++){
        delay(10);
        giro.update();
      }
      motorE.write(90);
      motorD.write(90);
      anguloReto = retornoAnguloZ();
      motorE.write(180);
      motorD.write(180);
      while(anguloReto - (90 - erroGiro) < retornoAnguloZ()){//Curva para a direita
        giro.update();
        Serial.print("VERDE!! Fazendo curva para a direita | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto - 90);
      }
      motorE.write(90);
      motorD.write(90);
      anguloReto = retornoAnguloZ();
    }else if((g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (c2 < c1) && (luu2 > minLuxVerde && luu2 < maxLuxverde)){
      giro.update();
      Serial.println("Verde na Esquerda!");
      motorE.write(180);
      motorD.write(0);
      for(int i=0;i<espera;i++){
        delay(10);
        giro.update();
      }
      motorE.write(90);
      motorD.write(90);
      sl = lerSensoresLinha();
      anguloReto = retornoAnguloZ();
      motorE.write(0);
      motorD.write(0);
      while(anguloReto + (90 + erroGiro) > retornoAnguloZ()){//Curva para a esquerda
        giro.update();
        Serial.print("VERDE!! Fazendo curva para a esquerda | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto + 90);
      }
      motorE.write(90);
      motorD.write(90);
      anguloReto = retornoAnguloZ();
    }
  }
  motorE.write(180);
  motorD.write(0);
  giro.update();
  delay(tempoDepoisDoVerde);
  giro.update();
}

void giroVerde2(){
  giro.update();
  motorE.write(90);
  motorD.write(90);

  tcs1.getRawData(&r1, &g1, &b1, &c1);
  lux1 = tcs1.calculateLux(r1, g1, b1);
  tcs2.getRawData(&r2, &g2, &b2, &c2);
  lux2 = tcs2.calculateLux(r2, g2, b2);

  Serial.println("*Sensor 01 Direita*");
  Serial.print("Lux: "); Serial.print(lux1, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r1, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g1, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b1, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c1, DEC); Serial.print(" ");
  Serial.println(" ");
  Serial.println("*Sensor 02 Esquerda*");
  Serial.print("Lux: "); Serial.print(lux2, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r2, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g2, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b2, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c2, DEC); Serial.print(" ");
  Serial.println(" ");

  int cc1 = (int)c1;
  int cc2 = (int)c2;
  int luu1 = (int)lux1;
  int luu2 = (int)lux2;
  Serial.print("lux1 > 125 && lux1 < 200  = "); Serial.println(luu1 > minLuxVerde && luu1 < maxLuxverde);
  Serial.print("lux2 > 125 && lux2 < 200 = "); Serial.println(luu2 > minLuxVerde && luu2 < maxLuxverde);
  Serial.print("c1 - c2 = "); Serial.println(abs(cc1-cc2) < subtracaoSensoresCor);
  Serial.print("Direita "); Serial.println((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && c1 < c2);
  Serial.print("Esquerda "); Serial.println((g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && c2 < c1);
  Serial.print("180° "); Serial.println((abs(cc1 - cc2) < subtracaoSensoresCor) && (g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (luu1 > minLuxVerde && luu1 < maxLuxverde) && (luu2 > minLuxVerde && luu2 < maxLuxverde));

  if(c1 <= valorCnoBranco || c2 <= valorCnoBranco){
    Serial.println("Possivel verde GV2");
    giro.update();
    if((abs(cc1 - cc2) < subtracaoSensoresCor) && (g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (luu1 > minLuxVerde && luu1 < maxLuxverde) && (luu2 > minLuxVerde && luu2 < maxLuxverde)){
      giro.update();
      Serial.println("VERDE!! Curva 180°");
      motorE.write(180);
      motorD.write(0);
      giro.update();
      delay(75);
      giro.update();
      anguloReto = retornoAnguloZ();
      motorE.write(180);
      motorD.write(180);
      while(anguloReto - (180 - erroGiro2) <= retornoAnguloZ()){
        giro.update();
        Serial.print("VERDE!! Fazendo curva 180° | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto - 180);
      }
      motorE.write(90);
      motorD.write(90);
    }else if((g1 > r1 && g1 > b1 && c1 < valorCnoBranco) && (c1 < c2) && (luu1 > minLuxVerde && luu1 < maxLuxverde)){
      giro.update();
      Serial.println("Verde na Direita!");
      motorE.write(180);
      motorD.write(0);
      giro.update();

      for(int i=0;i<espera;i++){
        delay(10);
        giro.update();
      }
      motorE.write(90);
      motorD.write(90);
      anguloReto = retornoAnguloZ();
      motorE.write(180);
      motorD.write(180);
      while(anguloReto - (90 - erroGiro2) < retornoAnguloZ()){//Curva para a direita
        giro.update();
        Serial.print("VERDE!! Fazendo curva para a direita | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto - 90);
      }
      motorE.write(90);
      motorD.write(90);
      anguloReto = retornoAnguloZ();
    }else if((g2 > r2 && g2 > b2 && c2 < valorCnoBranco) && (c2 < c1) && (luu2 > minLuxVerde && luu2 < maxLuxverde)){
      giro.update();
      Serial.println("Verde na Esquerda!");
      motorE.write(180);
      motorD.write(0);
      for(int i=0;i<espera;i++){
        delay(10);
        giro.update();
      }
      motorE.write(90);
      motorD.write(90);
      sl = lerSensoresLinha();
      anguloReto = retornoAnguloZ();
      motorE.write(0);
      motorD.write(0);
      while(anguloReto + (90 + erroGiro2) > retornoAnguloZ()){//Curva para a esquerda
        giro.update();
        Serial.print("VERDE!! Fazendo curva para a esquerda | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto + 90);
      }
      motorE.write(90);
      motorD.write(90);
      anguloReto = retornoAnguloZ();
    }
  }
  motorE.write(180);
  motorD.write(0);
  giro.update();
  delay(tempoDepoisDoVerde);
  giro.update();
}

void curva90(){
  motorE.write(180);
  motorD.write(0);
  giro.update();
  delay(verificacaoCurva);
  giro.update();
  motorE.write(90);
  motorD.write(90);

  sl = lerSensoresLinha();
  if(sl[0]==0 || sl[4]==0){
    sl = lerSensoresLinha();
    if(sl[0]==0 && sl[1]==0){
      sl = lerSensoresLinha();
      anguloReto = retornoAnguloZ();
      while(anguloReto + 90 > retornoAnguloZ() && sl[2]==1){
        giro.update();
        motorE.write(0);
        motorD.write(0);
        sl = lerSensoresLinha();
        Serial.print("Fazendo curva para a esquerda | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto + 90);
      }
      return;
    }else if(sl[3]==0 && sl[4]==0){
      sl = lerSensoresLinha();
      anguloReto = retornoAnguloZ();
      while(anguloReto - 90 < retornoAnguloZ() && sl[2]==1){
        giro.update();
        motorE.write(180);
        motorD.write(180);
        sl = lerSensoresLinha();
        Serial.print("Fazendo curva para a direita | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto - 90);
      }
    }
  }
}

void correcao(){//OLHAR DEPOIS
  //Giroscópio para a direita diminue o Z,para a esquerda aumenta o Z
  verificaVermelho();
  Serial.print("Caiu na correção | "); Serial.print("AnguloReto: "); Serial.print(anguloReto); Serial.print(" | "); Serial.print("AnguloAtual: "); Serial.println(retornoAnguloZ());
  sl = lerSensoresLinha();
  if(sl[0]==1 && sl[1]==1 && sl[3]==1 && sl[4]==1){
    if(anguloReto - erro > retornoAnguloZ()){//Vira a esquerda pois foi para a direita
      while(anguloReto - erro > retornoAnguloZ()){
        Serial.print("Virando a direita, correção | "); Serial.print("AnguloReto: "); Serial.print(anguloReto); Serial.print(" | "); Serial.print("AnguloAtual: "); Serial.print(retornoAnguloZ()); Serial.print(" | Objetivo: "); Serial.println(anguloReto + erro);
        motorE.write(90);
        motorD.write(20);
        sl = lerSensoresLinha();
        if(sl[0]==0 || sl[1]==0 || sl[3]==0 || sl[4]==0){
          curva90();
          break;
        }
      }
      motorE.write(90);
      motorD.write(90);
    }else if(anguloReto + erro < retornoAnguloZ()){//Vira a direita pois foi para a esquerda
      while(anguloReto + erro < retornoAnguloZ()){
        Serial.print("Virando a esquerda, correção | "); Serial.print("AnguloReto: "); Serial.print(anguloReto); Serial.print(" | "); Serial.print("AnguloAtual: "); Serial.print(retornoAnguloZ()); Serial.print(" | Objetivo: "); Serial.println(anguloReto - erro);
        motorE.write(160);
        motorD.write(90);
        sl = lerSensoresLinha();
        if(sl[0]==0 || sl[1]==0 || sl[3]==0 || sl[4]==0){
          curva90();
          return;
        }
      }
      motorE.write(90);
      motorD.write(90);
    }else if(anguloReto == retornoAnguloZ() || anguloReto - erro <= retornoAnguloZ() && retornoAnguloZ() <= anguloReto + erro){
      sl = lerSensoresLinha();
      Serial.print("Andando retro, correção | "); Serial.print("AnguloReto: "); Serial.print(anguloReto); Serial.print(" | "); Serial.print("AnguloAtual: "); Serial.println(retornoAnguloZ());
      motorE.write(160);
      motorD.write(20);
    }else if(sl[0]==0 || sl[4]==0){
      if(sl[0]==0 || sl[1]==0 || sl[3]==0 || sl[4]==0){
        curva90();
        return;
      }
    }
  }
  verificaVermelho();
}

int retornoSensorEsquerda(){
  digitalWrite(trigPin1, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = (duration1/2) / 29.1;
  delay(100);
  Serial.println(distance1);
  return distance1;
}

int retornoSensorFrente(){
  digitalWrite(trigPin2, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2= (duration2/2) / 29.1;
  delay(100);
  return distance2; //MUDAR para distance2
}

//Giroscópio para a direita diminue o Z,para a esquerda aumenta o Z

void ultrasonico(){
  Serial.println("Ultrasonico");
  if(retornoSensorFrente() == 7){//MUDAR O 1 para 7 e arrumar os angulos
    Serial.println("Sim");
    motorE.write(90);
    motorD.write(90);
    giro.update();
    Serial.print("Distância frente: "); Serial.println(retornoSensorFrente());

    anguloReto = retornoAnguloZ();

    while(anguloReto - 84 < retornoAnguloZ()){//Primeira curva, direita
      giro.update();
      motorE.write(180);
      motorD.write(180);
    }
    motorE.write(90);
    motorD.write(90);
    Serial.print("AnguloReto = "); Serial.print(anguloReto); Serial.print(" AnguloZ = "); Serial.println(retornoAnguloZ());
    giro.update();
    Serial.println("Andando reto");
    giro.update();

    motorE.write(180);
    motorD.write(0);
    //delay com for pra ficar melhor
    for(int i =0;i<20;i++){//anda reto por 2 segundos
      Serial.print("Andando reto: "); Serial.println(i);
      giro.update();
      delay(100);
      giro.update();
    }
    motorE.write(90);
    motorD.write(90);
    giro.update();
    
    anguloReto = retornoAnguloZ(); 
    Serial.print("Segunda curva, esquerda"); Serial.print(" Objetivo: "); Serial.println(anguloReto + 85);
    Serial.print("Angulo reto = "); Serial.print(anguloReto); Serial.print(" Angulo Atual: "); Serial.println(retornoAnguloZ());
    while(anguloReto + 85 > retornoAnguloZ()){//Segundo curva, esquerda
      giro.update();
      motorE.write(45);
      motorD.write(45);
    }
    motorE.write(90);
    motorD.write(90);
    giro.update();

    motorE.write(180);
    motorD.write(0);

    for(int i =0;i<35;i++){//anda reto por 4, ou para de achar linha
      sl = lerSensoresLinha();
      giro.update();
      if(sl[0]==0 || sl[1]==0 || sl[2]==0 || sl[3]==0 || sl[4]==0 || sl[5]==0){
        giro.update();
        motorE.write(180);
        motorD.write(0);

        for(int l=0;l<7;l++){//Pulo pra frente de 0.7segundos
          giro.update();
          delay(100);
          giro.update();
        }

        anguloReto = retornoAnguloZ();
        while(anguloReto - 85 < retornoAnguloZ()){//Curva direita, se tiver linha
          giro.update();
          motorE.write(125);
          motorD.write(125);
        }
        giro.update();
        Serial.println("Voltando 1");
        return;
      }
      giro.update();
      delay(100);
      giro.update();
    }

    anguloReto = retornoAnguloZ(); 
    Serial.print("Terceira curva, esquerda"); Serial.print(" Objetivo: "); Serial.println(anguloReto + 85);
    Serial.print("Angulo reto = "); Serial.print(anguloReto); Serial.print(" Angulo Atual: "); Serial.println(retornoAnguloZ());
    while(anguloReto + 85 > retornoAnguloZ()){//Terceira curva, esquerda
      giro.update();
      motorE.write(45);
      motorD.write(45);
    }
    motorE.write(90);
    motorD.write(90);

    motorE.write(180);
    motorD.write(0);
    for(int i =0;i<40;i++){//anda reto por 4, ou para de achar linha
      sl = lerSensoresLinha();
      giro.update();
      if(sl[0]==0 || sl[1]==0 || sl[2]==0 || sl[3]==0 || sl[4]==0 || sl[5]==0){
        giro.update();
        motorE.write(180);
        motorD.write(0);

        for(int l=0;l<7;l++){//Pulo pra frente de 0.7segundos
          giro.update();
          delay(100);
          giro.update();
        }

        anguloReto = retornoAnguloZ();
        while(anguloReto - 85 < retornoAnguloZ()){//Curva direita, se tiver linha
          giro.update();
          motorE.write(125);
          motorD.write(125);
        }
        giro.update();
        Serial.println("Voltando 2");
        return;
      }
      giro.update();
      delay(100);
      giro.update();
    }

    anguloReto = retornoAnguloZ(); 

    Serial.print("Quarta curva, esquerda"); Serial.print(" Objetivo: "); Serial.println(anguloReto + 85);
    Serial.print("Angulo reto = "); Serial.print(anguloReto); Serial.print(" Angulo Atual: "); Serial.println(retornoAnguloZ());
    while(anguloReto + 85 > retornoAnguloZ()){//Quarta curva, esquerda
      giro.update();
      motorE.write(45);
      motorD.write(45);
    }
    motorE.write(180);
    motorD.write(0);
    for(int i =0;i<40;i++){//anda reto por 4, ou para de achar linha
      sl = lerSensoresLinha();
      giro.update();
      if(sl[0]==0 || sl[1]==0 || sl[2]==0 || sl[3]==0 || sl[4]==0 || sl[5]==0){
        giro.update();
        motorE.write(180);
        motorD.write(0);

        for(int l=0;l<7;l++){//Pulo pra frente de 0.7segundos
          giro.update();
          delay(100);
          giro.update();
        }

        anguloReto = retornoAnguloZ();
        while(anguloReto - 85 < retornoAnguloZ()){//Curva direita, se tiver linha
          giro.update();
          motorE.write(125);
          motorD.write(125);
        }
        giro.update();
        Serial.println("Voltando 2");
        return;
      }
      giro.update();
      delay(100);
      giro.update();
    }
  }
}

void andarReto(){
  sl = lerSensoresLinha();
  
  //Detecta início da pista
  if(sl[0]==0 && sl[1]==0 && sl[2]==0 && sl[3]==0 && sl[4]==0){//00000
    Serial.println("Inicio da pista ou curva verde");
    sl = lerSensoresLinha();
    anguloReto = retornoAnguloZ();
    if(lerVerde1()){
      Serial.println("180° VERDE!!");
      giroVerde();
    }else{
      Serial.println("Andando reto por 1/4 segundo");
      motorE.write(180);
      motorD.write(0);
      delay(250);
    }
  }

  giro.update();
  sl = lerSensoresLinha();
  if((sl[0]==1 && sl[1]==1 && sl[2]==0 && sl[3]==1 && sl[4]==1) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//11011 - Andando reto (1)
    motorE.write(180);
    motorD.write(0);
    Serial.println("Andando reto");
    ultrasonico();
    sl = lerSensoresLinha();
    if(trava){
      anguloReto= retornoAnguloZ();
      Serial.println("Novo angulo RETO");
      trava = false;
    }

    sl = lerSensoresLinha();
    if(((sl[0]==0 && sl[1]==0 || sl[3]==0 && sl[4]==0) && sl[2]==0) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){ // 00011 ou 11000 Falsa curva ou verde (3 e 5) CERTO
      motorE.write(90);
      motorD.write(90);
      Serial.println("Falsa curva");
      retornoAnguloZ();
      sl = lerSensoresLinha();
      giro.update();
      giroVerde();
    }

    //Função de desvio de obstáculo

  }else if((sl[0]==1 && sl[1]==0 && sl[2]==0 && sl[3]==1 && sl[4]==1) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//10011
    Serial.println("Fazendo curva a esquerda 1");
    ultrasonico();
    giro.update();
    motorE.write(100);
    motorD.write(45);
    sl = lerSensoresLinha();
    trava = true;
  }else if((sl[0]==1 && sl[1]==0 && sl[2]==1 && sl[3]==1 && sl[4]==1) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//10111
    Serial.println("Fazendo curva a esquerda 2");
    ultrasonico();
    giro.update();
    motorE.write(90);
    motorD.write(25);
    sl = lerSensoresLinha();
    trava = true;
  }else if((sl[0]==1 && sl[1]==1 && sl[2]==0 && sl[3]==0 && sl[4]==1) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//11001
    Serial.println("Fazendo curva a direita 1");
    ultrasonico();
    giro.update();
    motorE.write(135);
    motorD.write(80);
    sl = lerSensoresLinha();
    trava = true;
  }else if((sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==0 && sl[4]==1) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//11101
    Serial.println("Fazendo curva a direita 2");
    ultrasonico();
    giro.update();
    motorE.write(150);
    motorD.write(90);
    sl = lerSensoresLinha();
    trava = true;
  }else if((sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==1 && sl[4]==1) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//11111
    giro.update();
    motorE.write(180);
    motorD.write(0);
    delay(verificacaoCurvaA);
    motorE.write(90);
    motorD.write(90);
    Serial.println("Alguma coisa ai 1");
    retornoSensoresLinha();
    Serial.println(" ");
    giro.update();
    if(sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==0 && sl[4]==0){//00100
      Serial.println("Alguma coisa ai, verde");
      giroVerde2();
    }else if(sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==1 && sl[4]==1){//11111
      Serial.println("Alguma coisa ai, correção");
      correcao();
    }else if((sl[0]==0 && sl[1]==1 && sl[2]==1 && sl[3]==0 && sl[4]==1) || (sl[0]==1 && sl[1]==0 && sl[2]==1 && sl[3]==1 && sl[4]==0)){//01101 ou 10110
      Serial.println("Cansei disso já");
      motorE.write(180);
      motorD.write(0);
      giro.update();
      delay(tempoPulinhoEspecial);
      giro.update();
    }
  }else if((sl[0]==0 || sl[4]==0) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){
    sl = lerSensoresLinha();
    giro.update();
    if((sl[0]==0 && sl[1]==0 && sl[2]==0 && sl[3]==1 && sl[4]==1) || (sl[0]==1 && sl[1]==1 && sl[2]==0 && sl[3]==0 && sl[4]==0)){//Curva falsa ou quadrado verde 00011 11000
      Serial.println("Curva falsa ou quadrado verde");
      giro.update();
      giroVerde();
    }else{
      if(sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==0 && sl[4]==0){ //00100
        anguloReto = retornoAnguloZ();
        Serial.println("T detectado");
        giroVerde();
        giro.update();
        sl = lerSensoresLinha();
      }else if(sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==1 && sl[4]==1){//Curva 90° para a esquerda 00111 MODIFICADA "Pulinho"
        anguloReto = retornoAnguloZ();
        motorE.write(180);
        motorD.write(0);
        giro.update();
        delay(verificacaoCurva);
        giro.update();
        sl = lerSensoresLinha();

        if(sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==1 && sl[4]==1){//00111
          while((anguloReto + 90 > retornoAnguloZ()) && sl[2]==1){
            giro.update();
            motorE.write(0);
            motorD.write(0);
            sl = lerSensoresLinha();
            Serial.print("Fazendo curva para a esquerda | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto + 90);
          }
        }else{//MUDAR , talvez colocar uma condição
          motorE.write(90);
          motorD.write(90);
          retornoSensoresLinha();
          Serial.println("***Esquerda 90° mudou***");
          return;
        }
      }else if(sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==0 && sl[4]==0){//Curva 90° para a direita 11100 MODIFICADA "Pulinho"
        anguloReto = retornoAnguloZ();
        motorE.write(180);
        motorD.write(0);
        giro.update();
        delay(verificacaoCurva);
        giro.update();
        sl = lerSensoresLinha();

        if(sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==0 && sl[4]==0){//11100
          while((anguloReto - 90 < retornoAnguloZ()) && sl[2]==1){
            giro.update();
            motorE.write(180);
            motorD.write(180);
            sl = lerSensoresLinha();
            Serial.print("Fazendo curva para a direita | ");Serial.print("Angulo Atual: "); Serial.print(retornoAnguloZ()); Serial.print("Objetivo: "); Serial.println(anguloReto - 90);
          }
        }else{//MUDAR , talvez colocar uma condição
          motorE.write(90);
          motorD.write(90);
          retornoSensoresLinha();
          Serial.println("***Direita 90° mudou***");
          return;
        }
      }else if(sl[0]==0 && sl[1]==1 && sl[2]==1 && sl[3]==1 && sl[4]==1){//Curva para a esquerda 01111 MODIFICADA "Ré"
        Serial.println("Fazendo curva a esquerda 3");
        motorE.write(180);
        motorD.write(0);
        delay(verificacaoCurva);
        sl = lerSensoresLinha();

        while(sl[0]==0 && sl[1]==1){
          giro.update();
          sl = lerSensoresLinha();
          Serial.println(retornoAnguloY());
          motorE.write(70);
          motorD.write(90);
          Serial.println("01111a");
          if(retornoAnguloY() <= anguloRampaSubida){
            break;
          }
        }
      }else if(sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==1 && sl[4]==0){//Curva para a direita  11110
        Serial.println("Fazendo curva a direita 3");
        motorE.write(180);
        motorD.write(0);
        delay(verificacaoCurva);
        sl = lerSensoresLinha();

        while(sl[3]==1 && sl[4]==0){
          giro.update();
          sl = lerSensoresLinha();
          motorE.write(90);
          motorD.write(110);
          Serial.println("11110a");
          Serial.println(retornoAnguloY());
          if(retornoAnguloY() <= anguloRampaSubida){
            break;
          }
        }
      }
    }
  }
  while(retornoAnguloY() <= anguloRampaSubida || retornoAnguloY() >= anguloRampaDescida){
    //Padrão de velocidade na subida
    //motorE.write(160);
    //motorD.write(20);
    sl = lerSensoresLinha();
    if(sl[1]==1 && sl[2]==0 && sl[3]==1 && retornoAnguloY <= anguloRampaSubida){
      motorE.write(160);
      motorD.write(20);
      sl = lerSensoresLinha();
      Serial.print("Subida reta detectada, angulo: "); Serial.println(retornoAnguloY());
    }else if(sl[1]==0 && sl[3]==1 && retornoAnguloY <= anguloRampaSubida){//Foga para a direita com as duas rodas andando
      motorE.write(120);
      motorD.write(0);
      sl = lerSensoresLinha();
      Serial.print("Subida esquerda detectada, angulo: "); Serial.println(retornoAnguloY());
    }else if(sl[1]==1 && sl[3]==0 && retornoAnguloY <= anguloRampaSubida){
      motorE.write(180);
      motorD.write(70);
      sl = lerSensoresLinha();
      Serial.print("Subida direita detectada, angulo: "); Serial.println(retornoAnguloY());
    }
  }

  Serial.println("Se perdeu");

  if((sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==0 && sl[4]==0) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//00100
    Serial.println("T fora de lugar");
    motorE.write(90);
    motorD.write(90);
    giroVerde();
  }else if((sl[0]==1 && sl[1]==0 && sl[2]==0 && sl[3]==0 && sl[4]==0) || (sl[0]==0 && sl[1]==0 && sl[2]==0 && sl[3]==0 && sl[4]==1) || (sl[0]==1 && sl[1]==0 && sl[2]==0 && sl[3]==0 && sl[4]==1)){//10000 ou 00001 ou 10001
    motorE.write(90);
    motorD.write(90);
    giroVerde2();
  }else if((sl[0]==1 && sl[1]==1 && sl[2]==0 && sl[3]==1 && sl[4]==0) || (sl[0]==0 && sl[1]==1 && sl[2]==0 && sl[3]==1 && sl[4]==1)){// 11010 ou 01011
    motorE.write(180);
    motorD.write(0);
    delay(tempoPulinhoEspecial);
  }else if((sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==1 && sl[4]==1) || (sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==0 && sl[4]==0)){// 00111 ou 11100 fora de lugar
    Serial.println("00111 ou 11100 fora de lugar");
    motorE.write(90);
    motorD.write(90);
    curva90();
    motorE.write(90);
    motorD.write(90);
  }else if((sl[0]==0 && sl[1]==1 && sl[2]==1 && sl[3]==1 && sl[4]==1) || (sl[0]==1 && sl[1]==1 && sl[2]==1 && sl[3]==1 && sl[4]==0)){
    Serial.println("01111 ou 11110 fora de lugar");
    motorE.write(90);
    motorD.write(90);

    while(sl[3]==1 && sl[4]==0){
      giro.update();
      sl = lerSensoresLinha();
      motorE.write(90);
      motorD.write(110);
      Serial.println("11110b");
      Serial.println(retornoAnguloY());
      if(retornoAnguloY() <= anguloRampaSubida){
        break;
      }
    }

    while(sl[0]==0 && sl[0]==1){
      giro.update();
      sl = lerSensoresLinha();
      motorE.write(70);
      motorD.write(90);
      Serial.println("01111b");
      Serial.println(retornoAnguloY());
      if(retornoAnguloY() <= anguloRampaSubida){
        break;
      }
    }
  }
  giro.update();

  //*******************
  //* Casos especiais *
  //*******************
  sl = lerSensoresLinha();

  if((sl[0]==1 && sl[1]==0 && sl[2]==1 && sl[3]==0 && sl[4]==1) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){ //10101
    Serial.println("Caso estranho 10101");
    motorE.write(180);
    motorD.write(0);
    delay(tempoPulinhoEspecial);
    sl = lerSensoresLinha();
  }else if(((sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==0 && sl[4]==1) || (sl[0]==1 && sl[1]==0 && sl[2]==1 && sl[3]==0 && sl[4]==0)) && (retornoAnguloY() >= anguloRampaSubida && retornoAnguloY() <= anguloRampaDescida)){//00101 ou 10100 com Pulinho
    retornoSensoresLinha();
    motorE.write(90);
    motorD.write(90);
    curva90();
  }
  Serial.println("Saiu do principal, curva falsa ou 90° ou gangorra");
  sl = lerSensoresLinha();
  giro.update();
  if(sl[0]==0 && sl[1]==0 && sl[2]==1 && sl[3]==0 && sl[4]==0){//00100
      Serial.println("T do final");
      giroVerde2();
  }

  for(byte i = 0; i < 5; i++){
    Serial.print(sl[i]);
  }
  Serial.println(" ");

}

//Funções para demonstração de valores

void retornoGiroscopio(){
  static int valores[3];
  giro.update();
  valores[0] = giro.getAngleX();
  valores[1] = giro.getAngleY();
  valores[2] = giro.getAngleZ();

  for(int i=0;i<3;i++){
    if(i==0){
      Serial.print("angleX : ");
      Serial.print(valores[i]);
    }else if(i==1){
      Serial.print("\tangleY : ");
      Serial.print(valores[i]);
    }else if(i==2){
      Serial.print("\tangleZ : ");
      Serial.println(valores[i]);
    }
  }
}

void retornoSensoresLinha(){
  sl = lerSensoresLinha();
  for(byte i = 0; i < 5; i++){
    Serial.print(sl[i]);
  }
  Serial.println(" ");
}

//***********
//*          Código Setup         *
//***********

void setup() {
  Serial.begin(9600);
  Wire.begin();

  //Inicialização do giroscópio
  byte status = giro.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  giro.calcOffsets();
  Serial.println("Done!\n");
  ////////////////////

  for (int i = 0; i < 5; i++) {
    pinMode(sensores[i], INPUT);
  }

  lerVerde();

  //Inicia os servos
  motorE.attach(motorEpin);//Seta o pino do motor
  motorD.attach(motorDpin);
  motorE.write(90);
  motorD.write(90);

  anguloDoReto = retornoAnguloY();
  anguloRampaSubida = anguloDoReto - 10;
  anguloRampaDescida = anguloDoReto + 10;

  lerVerde();
}

//*********************************
//*          Código  Loop         *
//*********************************

void loop() {
  //1° versão apenas corrige a linha, pra eu aprender, considero que acabou no dia 26/05
  //2° versão, corrige linha e faz cuva 90° sem o verde
  //3° versão, corrige linha e faz cruva 90° e 180° com verde e sem verde
  //4° versão, corrige tudo + vermelho
  //5° versão, corrige tudo de cima e talvez ultrasonicos e saida da arena lá

  //lerVerde();
  //retornoGiroscopio(); //para testar o giroscopio
  //retornoSensoresLinha(); //par4a ter retorno dos sensores da linha
  //retornoSensoresLinha(); Serial.print(" | "); Serial.println(retornoAnguloZ());
  //Serial.println(retornoAnguloZ());

  //giro.update();
  //ultrasonico();
  //Serial.println(retornoSensorFrente());
  //motorE.write(0);
  //motorD.write(180);
  andarReto();
  //correcao();
  //lerVerde2();
  //lerVerde();
  //verificaVermelho();
}
