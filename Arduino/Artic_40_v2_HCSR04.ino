/***************************************************************
* Control de posición de una pelota en levitación V0.2
* Desarrollado por Garikoitz Martínez [garikoitz.info] [03/2024]
* https://garikoitz.info/blog/?p=2215
***************************************************************/
/***************************************************************
* Librerías
***************************************************************/
#include <PID_v1.h>
/***************************************************************
* Variables
***************************************************************/
const int trigPin = 5;
const int echoPin = 6;
float distancia = 0;
float distancia_Ant = 0;
float distancia_Clamp = 0;
float distanciaFilter;
long duracion = 0;
int Ts = 100; //Sample time in ms
unsigned long previousMillis = 0;
double alpha = 0.4; //Filtro EMA
//Ventilador
float ZC1_PV = 0.0;
float ZC1_SP = 0.0;
int ZC1_OP = 0;
int ZC1SPMIN = 10;
int ZC1SPMAX = 22;
int ZC1OPMIN = 0;
int ZC1OPMAX = 100;
int valorPWM = 0;
// global variables
const char sp = ' ';           
const char nl = '\n';
boolean newData = false; 
char Buffer[64]; 
int buffer_index = 0; 
String cmd;  
float val;
/***************************************************************
* Sintonías calculadas -> Integrador
* Test -> APV = 3% | AOP = 3% | T0 = 1.2s
* v = 2.5 | Kv = 2.08 | T0 = 1.2
* KcZC1=0.69, KiZC1=0.29, KdZC1=0.41; (Sintonía inicial)
***************************************************************/
double SetpointZC1, InputZC1, OutputZC1;
double KcZC1=0.69, KiZC1=0.29, KdZC1=0.41; 
PID PIDZC1(&InputZC1, &OutputZC1, &SetpointZC1, KcZC1, KiZC1, KdZC1, P_ON_E, REVERSE);    //PI-D //DIRECT //REVERSE
//PID PIDZC1(&InputZC1, &OutputZC1, &SetpointZC1, KcZC1, KiZC1, KdZC1, P_ON_M, REVERSE);  //I-PD
/***************************************************************
* SETUP
***************************************************************/ 
void setup() {
  while (!Serial) {
    // wait for serial port to connect.
  }
  Serial.begin(9600);
  pinMode(9, OUTPUT); // Configura el pin 9 como salida
  valorPWM = 0;
  // Configuración del Timer1 para PWM a 25 kHz
  TCCR1A = _BV(COM1A1) | _BV(WGM11); // Modo de comparación no invertido y modo Fast PWM
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // Sin preescalado y modo Fast PWM
  ICR1 = 639; // Establece el TOP para obtener 25 kHz = 639
  OCR1A = 0; // Inicializa el ciclo de trabajo en 0
  /*
   * ICR1 = ((Frecuencia_CPU) / (Frecuencia PWM deseada * preescalado)) - 1
   * Ejemplo para 20 kHz
   * ICR1 = (16000000 Hz / 20000 Hz * 1) - 1 = 799
   */
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //
  previousMillis=0;
  PIDZC1.SetOutputLimits(0, 255);
  PIDZC1.SetMode(MANUAL);
  if (Ts < 100){
    PIDZC1.SetSampleTime(Ts);
  }
  distancia = medirDistancia();
}
/***************************************************************
* BUCLE PRINCIPAL
***************************************************************/
void loop() {
  if (millis() - previousMillis > Ts)
  {
    distancia = medirDistancia();
    distancia = constrain(distancia, 0, 26);
    distanciaFilter = alpha*distancia+(1.0-alpha)*distanciaFilter; //Filtro EMA
    ZC1_PV = distanciaFilter;
    //ZC1_PV = map(distanciaFilter, 0, 26, 26, 0);
    //ZC1_PV = map(distanciaFilter, 4, 26, 0, 100); //cm -> %
    distancia_Ant = distancia;
    
    previousMillis = millis();
    //Funciones Lectura/Escritura Serial
    LeoCMD();
    ProcesoCMD();
    EjecutoCMD();
    //PID (Parámetros vía serial cmds)
    if (PIDZC1.GetMode() == 1){//AUTO
        InputZC1 = ZC1_PV;
        SetpointZC1 = ZC1_SP;
        PIDZC1.Compute();
        OCR1A = map(OutputZC1, 0, 255, 0, ICR1);
        ZC1_OP = map(OutputZC1, 0, 255, 0, 100); //PWM -> %
     }else if (PIDZC1.GetMode() == 0) {//MANUAL
    /*>>INICIALIZACIÓN<< -> para evitar problemas al pasar de MANUAL a AUTO
    (1) --> PV Tracking -> igualamos SP a PV
    (2) --> Calculamos la salida de control para que 'inicialice' correctamente el PID sin provocar saltos bruscos al pasar de Manual a Auto.
    */
        ZC1_SP = ZC1_PV; //(1)
        OCR1A = map(OutputZC1, 0, 255, 0, ICR1);
        OutputZC1 = map(ZC1_OP, 0, 100, 0, 255); //(2)
     }
     //Para Arduino COM Plotter
     Serial.print("#");        //Char inicio
     Serial.print(ZC1_SP,0);    //Serial.print(ZC1_SP,0);
     Serial.write(" ");        //separador
     Serial.print(ZC1_PV,0);    //
     Serial.write(" ");        //separador
     Serial.print(ZC1_OP);      //
     Serial.println();
    //
  }//millis
}//loop
/***************************************************************
* FUNCIONES
***************************************************************/
//No hace falta tocar
void LeoCMD() {
  while (Serial && (Serial.available() > 0) && (newData == false)) {
    int byte = Serial.read();
    if ((byte != '\r') && (byte != nl) && (buffer_index < 64)) {
      Buffer[buffer_index] = byte;
      buffer_index++;
    }
    else {
      newData = true;
    }
  }   
}
//No hace falta tocar
void ProcesoCMD(void) {
  if (newData) {
    String read_ = String(Buffer);
    // separate command from associated data
    int idx = read_.indexOf(sp);
    cmd = read_.substring(0, idx);
    cmd.trim();
    cmd.toUpperCase();
    // extract data. toFloat() returns 0 on error
    String data = read_.substring(idx + 1);
    data.trim();
    val = data.toFloat();
    // reset parameter for next command
    memset(Buffer, 0, sizeof(Buffer));
    buffer_index = 0;
    newData = false;
  }
}
//A partir de aquí comandos personalizados
float medirDistancia() {
  digitalWrite(trigPin, LOW);// Limpiar el pin trig
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);// Configurar el pin trig en HIGH por 10 microsegundos
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duracion = pulseIn(echoPin, HIGH);// Leer el tiempo de duración del eco
  distancia = duracion * 0.017;// Calcular la distancia * 0.034 * 0.5
  if(duracion>1600 || duracion < 200){
    //Fuera de rango. Depende de si invertimos la PV será 4 ó 26, en este caso.
    distancia = distancia_Clamp;
  }else{
    return distancia;
  }
}
void EjecutoCMD(void) {
  if (cmd == "ZC1A") {
    PIDZC1.SetMode(AUTOMATIC);
  }
  else if (cmd == "ZC1M") {
    PIDZC1.SetMode(MANUAL);
  }
  else if (cmd == "ZC1SP") {
    if (PIDZC1.GetMode() == 1){
      if (val > ZC1SPMAX){
        val = ZC1SPMAX;
      }
    if (val < ZC1SPMIN){
        val = ZC1SPMIN;
      }
    ZC1_SP = val;
    }
  }
  else if (cmd == "ZC1OP") {
    if (PIDZC1.GetMode() == 0){
      if (val > ZC1OPMAX){
        val = ZC1OPMAX;
      }
      if (val < ZC1OPMIN){
        val = ZC1OPMIN;
      }
      ZC1_OP = val;
      //map(value, fromLow, fromHigh, toLow, toHigh)
      val = map(val, 0, 100, 0, 255);
      OCR1A = map(OutputZC1, 0, 255, 0, ICR1);
    }
  }
  else if (cmd == "ZC1KC") {
    PIDZC1.SetTunings(val,KiZC1,KdZC1);
  }
  else if (cmd == "ZC1KI") {
    PIDZC1.SetTunings(KcZC1,val,KdZC1);
  }
  else if (cmd == "ZC1KD") {
    PIDZC1.SetTunings(KcZC1,KiZC1,val);
  }
  else if (cmd == "FILTRO") {
    if (val > 1){
        val = 1;
    }
    if (val <= 0){
        val = 0.01;
    }
    alpha = val;
  }
  else if (cmd == "ZC1FR") {
    ICR1 = val;
  }
  Serial.flush();
  cmd = "";
}
