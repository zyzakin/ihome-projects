
#define POWER 2
#define DIRECTION 9
#define LEFT 11
#define RIGHT 10
#define CUR_LEFT 15
#define CUR_RIGHT 14
#define GATE_SIGNAL 3
#define WICKET_SIGNAL 4

typedef struct {
  int pin;
  unsigned long time = 0;
  float measurment[10] = {0.0};
  int next = 0;
  float avg = 0.0;
} t_current; //definicja zmiennej z pomiarami prądu

enum t_state {OPENING, ALARM, CLOSING, PENDING};
enum t_action {OPEN, STOP, CLOSE, PAUSE};
/*
  OPEN - otwieranie ustawiane z pilota
  STOP - ustawiane przez sterownik po zakończeniu otwierania lub zamykania
  CLOSE - zamykanie ustawiane z pilota
  PAUSE - ustawiane z pilota tylko w czasie ruchu
*/
enum names {NORMAL, WICKED };

//String state[3] ={"OPEN", "STOP", "CLOSE", "STOP"};

typedef struct  {
        String name;
        int pin;
        unsigned long start_time;
        t_current current;
        t_state state;
        t_action action;
        t_action last_move = OPEN;
} t_gate_part; //str definujaca stan skrzydła bramy


t_gate_part gate[2];

t_action action = STOP;
word zupka=0;
word kubek = 0;
unsigned long timer1 = 0;
boolean sec_flag = false;
unsigned long timer2 = 0;
unsigned long timer3 = 0;
unsigned long timer4 = 0;
String action_dict[] = {"OPEN", "STOP", "CLOSE", "PAUSE"};
String wicket = "RIGHT"; //furtka
int deferment = 7; //zwłoka, w sekundach, zamykania skrzydła furtki
String server_ip = "192.168.1.80";
String server_port = "8888";

int odczytanaWartosc = 0;
float bocznik = 0.1;
float threshold = 1.9; //wartość graniczna prądu powyżej której odłączamy silniki

void moveGate(t_gate_part gate[]);
void checkCurrent(t_gate_part * container);
void checkAction(t_gate_part gate[]);
void pause(t_gate_part * gate );
void alarm(t_gate_part * gate );
//void pending(t_gate_part * gate );
void pending(t_gate_part * part );
void open(t_gate_part * gate );
void close(t_gate_part * gate );


void setup() {
  Serial.begin(9600);//Uruchomienie komunikacji przez USART
  Serial.println("Uruchomienie sterownika bramy wjazdowej");
  pinMode(POWER, OUTPUT); //przekaźnik załaczenia 230V na transformator
  pinMode(DIRECTION, OUTPUT); //Przekaźnik mostka H - zmiana kierunku ruchu bramy
  pinMode(LEFT, OUTPUT); //Przekaźnik lewego skrzydła
  pinMode(RIGHT, OUTPUT); //Przekaźnik prawego skrzydła
  pinMode(GATE_SIGNAL, INPUT_PULLUP); //sygnał otwarcia/zamknięcia bramy
  pinMode(WICKET_SIGNAL, INPUT_PULLUP); //sygnał otwarcia/zamknięcia bramy
  
  digitalWrite(POWER, LOW);//Włączenie przekaźnika  
  digitalWrite(LEFT, LOW);//Włączenie przekaźnika
  digitalWrite(RIGHT, LOW);//Włączenie przekaźnika
  digitalWrite(DIRECTION, LOW);//Włączenie przekaźnika
 
  timer1 = millis();
  //inicjalizacja obiektu bramy
  gate[NORMAL].name = "NORMAL";
  gate[NORMAL].pin = LEFT;
  gate[NORMAL].current.pin = CUR_LEFT;
  gate[NORMAL].current.avg = 0.0;
  gate[NORMAL].state = PENDING;
  gate[NORMAL].action = STOP;
  gate[NORMAL].last_move = CLOSE;
  gate[WICKED].name = "WICKED";
  gate[WICKED].pin = RIGHT;
  gate[WICKED].current.pin = CUR_RIGHT;
  gate[WICKED].current.avg = 0.0;
  gate[WICKED].current.measurment[0] = (0.0);
  gate[WICKED].state = PENDING;
  gate[WICKED].action = STOP;
  gate[WICKED].last_move = CLOSE;

//"NORMAL"LEFT, 0, {15, 0, { }, 1, 0.0}, PENDING, STOP} ;  
//  gate[WICKED] ={"WICKED", RIGHT, 0, {14, 0, { }, 1, 0.0}, PENDING, STOP} ;

    Serial.print("STATE|");
    Serial.println(action_dict[gate[NORMAL].last_move]);
}
 
void loop() {
  
  if (timer1 + 1000 < millis()){
    timer1 = millis();
    sec_flag = true;
  }else
    sec_flag = false;

  checkAction(gate); // sprawdzamy stan przycisku i maszyny stanów
  Serial.print(gate[NORMAL].action);
  Serial.println(gate[WICKED].action);
  //moveGate(gate);
  //checkCurrent(&gate[NORMAL]);
  //checkCurrent(&gate[WICKED]);

  if (sec_flag){
    // NAME|ACTION|AVG CURRENT
  }


}

/*Sprawdzanie niciśnietych przycisków 
Ustawianie odpowiednie statusu pracy bramy

*/
void checkAction(t_gate_part gate[]){
  String name = "";
  if (digitalRead(GATE_SIGNAL) == LOW && zupka == 0){
    zupka=30000;
    // STATE|prev_move|state
    Serial.print("STATE|");
    switch (gate[NORMAL].action){
      case STOP :
        if ( gate[NORMAL].last_move == OPEN ){
          // obsługa ustawień dla CLOSE
          Serial.print(action_dict[gate[NORMAL].last_move]);
          gate[NORMAL].action = CLOSE;
          gate[NORMAL].last_move = CLOSE;
          gate[NORMAL].state = CLOSING;
          gate[NORMAL].start_time = millis();
          gate[WICKED].action = CLOSE;
          gate[WICKED].state = CLOSING;
          gate[WICKED].last_move = CLOSE;
          gate[WICKED].start_time = millis();
          Serial.print("|");
          Serial.println(action_dict[gate[NORMAL].action]);
        } else if ( gate[NORMAL].last_move == CLOSE ){
          // obsługa ustawień dla OPEN
          Serial.print(action_dict[gate[NORMAL].last_move]);
          gate[NORMAL].action = OPEN;
          gate[NORMAL].state = OPENING;
          gate[NORMAL].last_move = OPEN;
          gate[WICKED].action = OPEN;
          gate[WICKED].state = OPENING;
          gate[WICKED].last_move = OPEN;
          Serial.print("|");
          Serial.println(action_dict[gate[NORMAL].action]);
        }else{
          Serial.println("");
          Serial.print("DEBUG|blad warunku if w akcji STOP - nieobslugiwana wartosc gate[NORMAL].last_move :");
          Serial.println(gate[NORMAL].last_move);
        }
        break;
      case OPEN :
        break;
      case CLOSE :
        break;
      case PAUSE :
        break;
    }
  }else
    zupka = zupka == 0 ? 0 : zupka-1; 
}

/* Ruszamy bramą - zamykamy/otwieramy


*/
void moveGate(t_gate_part gate[]) {

  if (gate[NORMAL].action == gate[WICKED].action ) {
    // otwieranie bramy
    switch (gate[NORMAL].action){
      case OPEN :
        open(gate);
        if (gate[NORMAL].current.avg > threshold) {
          Serial.print("Otwieranie LEWA - wylaczam - przekroczono prog pradu : ");
          Serial.println(gate[NORMAL].current.avg);
          digitalWrite(gate[NORMAL].pin, LOW);
	        gate[NORMAL].current.avg = 0.0;
          gate[NORMAL].state=PENDING;
        }
        if (gate[WICKED].current.avg > threshold) {
          Serial.print("Otwieranie PRAWA - wylaczam - przekroczono prog pradu : ");
          Serial.println(gate[WICKED].current.avg);
          digitalWrite(gate[WICKED].pin, LOW);
	        gate[WICKED].current.avg = 0.0;
          gate[WICKED].state=PENDING;
        }
        if ( gate[WICKED].state == gate[NORMAL].state && gate[WICKED].state == PENDING ){
          gate[NORMAL].action=STOP;
          gate[WICKED].action=STOP;
  	      digitalWrite(DIRECTION, LOW);
          digitalWrite(POWER, LOW);
          Serial.println("STOP oba silniki");
        }
          
        break;
      case CLOSE :
        close(gate);
        if (gate[NORMAL].current.avg > threshold) {
          Serial.print("Zamykanie LEWA - wylaczam- przekroczono prog pradu : ");
          Serial.println(gate[NORMAL].current.avg);
          digitalWrite(gate[NORMAL].pin, LOW);
	        gate[NORMAL].current.avg = 0.0;
          gate[NORMAL].state=PENDING;
        }
        if (gate[WICKED].current.avg > threshold) {
          Serial.print("Zamykanie PRAWA -wylaczam - przekroczono prog pradu : ");
          Serial.println(gate[WICKED].current.avg);
          digitalWrite(gate[WICKED].pin, LOW);
	        gate[WICKED].current.avg = 0.0;
          gate[WICKED].state=PENDING;
        }
        if ( gate[WICKED].state == gate[NORMAL].state && gate[WICKED].state == PENDING ){
          gate[NORMAL].action=STOP;
          gate[WICKED].action=STOP;
  	      digitalWrite(DIRECTION, LOW);
          digitalWrite(POWER, LOW);
          Serial.println("STOP oba silniki");
        }
        break;
      case STOP :
        digitalWrite(gate[WICKED].pin, LOW);
        digitalWrite(gate[NORMAL].pin, LOW);
  	    digitalWrite(DIRECTION, LOW);
        digitalWrite(POWER, LOW);
        gate[NORMAL].state = PENDING;
        gate[WICKED].state = PENDING;
        break;
    
        default:
         Serial.println("DEBUG|moveGate - brak wartosci na liscie switch");
    }
  }else{
    Serial.print("DEBUG|moveGate - niespełniony warunek gate[NORMAL].action == gate[WICKED].action : ");
    Serial.print(gate[NORMAL].action);
    Serial.print(" == ");
    Serial.println(gate[WICKED].action);
    // otwieranie furtki
  ;
  }

}

/* pomiar prady silnika
 * Wyliczanie sredniego pradu z 10 pomiarów robionych co 100ms
 */
void checkCurrent(t_gate_part * container){
  float sum=0.0;
  long val=0;
  if (container->action == CLOSE or container->action == OPEN){
    if (container->current.time == 0 )
      container->current.time = millis();
      
    if (container->current.time + 100 < millis()) { //pomiar pradu co 100 ms
      container->current.time = millis();
      val = analogRead(container->current.pin);
      container->current.measurment[container->current.next] = (val * (5.0/1023.0)) / bocznik ; //Przeliczenie wartości na prad
      container->current.next = container->current.next < 9 ? container->current.next+1 : 0;
      if (val > 0)
        for (int i = 0; i < 10; i++)
          sum += container->current.measurment[i]; 
      container->current.avg = sum/10.0;

    }
  }
}  

void open(t_gate_part * gate ){
  digitalWrite(POWER, HIGH);
  digitalWrite(DIRECTION, LOW);
  if (gate[NORMAL].state == OPENING || gate[NORMAL].state == CLOSING){
    digitalWrite(gate[NORMAL].pin, HIGH);
    Serial.print("INFO|OPEN wlaczam silnik NORMAL w :");
    Serial.println(millis());
  }
  if (gate[WICKED].state == OPENING || gate[WICKED].state == CLOSING){
    digitalWrite(gate[WICKED].pin, HIGH);
    Serial.print("INFO|OPEN wlaczam silnik WICKED w :");
    Serial.println(millis());
  }
  
};
void close(t_gate_part * gate ){
  digitalWrite(POWER, HIGH);
  digitalWrite(DIRECTION, HIGH);
  if (gate[NORMAL].state == OPENING || gate[NORMAL].state == CLOSING){
    digitalWrite(gate[NORMAL].pin, HIGH);
    Serial.print("INFO|CLOSE wlaczam silnik NORMAL w :");
    Serial.println(gate[NORMAL].start_time);
  }
  if (gate[WICKED].state == OPENING || gate[WICKED].state == CLOSING)
    if (gate[WICKED].start_time + (deferment * 1000)  < millis() ){ // zwłoka w zamykaniu bramy na zakładkę
      digitalWrite(gate[WICKED].pin, HIGH);
      Serial.print(gate[WICKED].start_time + (deferment * 1000));
      Serial.print("INFO|CLOSE wlaczam silnik WICKED z opoznieniem :");
      Serial.println((millis() - gate[WICKED].start_time)/1000);
    }else{
      Serial.print(gate[WICKED].start_time + (deferment * 1000));
      Serial.print(" - ");
      Serial.println(millis());
    }
};

void pause(t_gate_part * gate ){
  digitalWrite(gate[NORMAL].pin, LOW);
  digitalWrite(gate[WICKED].pin, LOW);
  
};

void alarm(t_gate_part * gate ){
  digitalWrite(gate[NORMAL].pin, LOW);
  digitalWrite(gate[WICKED].pin, LOW);

};

void pending(t_gate_part * part ){
//void pending(t_gate_part * gate ){
  digitalWrite(part->pin, LOW);
  //digitalWrite(gate[WICKED].pin, LOW);
};
