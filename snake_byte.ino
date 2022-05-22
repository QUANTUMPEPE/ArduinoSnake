#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

const byte KEY_ROWS = 4; // 4 строки
const byte KEY_COLS = 4; // 4 столбца

const byte SCREEN_ROWS = 4; // 4 строки
const byte SCREEN_COLS = 20; // 20 столбцов
const byte SCREEN_DOTS = SCREEN_ROWS*SCREEN_COLS;

LiquidCrystal_I2C lcd(0x27, SCREEN_COLS, SCREEN_ROWS);

struct Primitive{
    char symbol;
    byte x;
    byte y;
    Primitive(){}
    Primitive(char symbol, byte x, byte y){
      this->symbol = symbol;
      this->x = x;
      this->y = y;
      }
  };

enum Directions{
    Up,
    Down,
    Left,
    Right
  };

class Snake{
public:
  byte length = 0;
  byte xBuffer; //Сделать проще? 
  byte yBuffer;
  Directions moveDirection;
  Primitive snake [SCREEN_DOTS];

  Snake(byte length = 2){
    this->length = length;
    moveDirection = Right;
    snake[0].symbol = '@';
    snake[0].y = SCREEN_ROWS/2;
    snake[0].x = SCREEN_COLS/2;
    for(byte i = 1; i<length; i++){
      snake[i].symbol ='#';
      snake[i].y = snake[0].y;
      snake[i].x = snake[i-1].x-1; 
      }
    for(byte i = length; i<SCREEN_DOTS; i++){
      snake[i].symbol ='x';
      snake[i].y = -1;
      snake[i].x = -1; 
      }
    }

    void changeDirection(Directions direction){
        if(direction == moveDirection){
            return;
          }
        else if(direction == Up && moveDirection == Down){
            return;
          }
        else if(direction == Down && moveDirection == Up){
            return;
          }
        else if(direction == Left && moveDirection == Right){
            return;
          }
        else if(direction == Right && moveDirection == Left){
            return;
          }
        else{
            moveDirection = direction;
          }
      }

    void move(){              
        for(byte i = length; i>=1; i--){          
          snake[i].x = snake[i-1].x;
          snake[i].y = snake[i-1].y;

          xBuffer = snake[length-1].x; //Сделать проще? 
          yBuffer = snake[length-1].y;
        }                
        
        switch(moveDirection){
          case Up: 
            snake[0].y-=1;
            break;
          case Down:
            snake[0].y+=1;
            break;
          case Left: 
            snake[0].x-=1;
            break;
          case Right:
            snake[0].x+=1;
            break;
        };        
      }

    void grow(){
      snake[length].x = xBuffer;
      snake[length].y = yBuffer;
      snake[length].symbol = '#';
      length++;       
      }
  };

class Game{
private:
  Snake* character;
  Primitive* map;
  byte level = 0;
  byte mapSize = 0; 
  Primitive fruit;
  double score = 0;
  byte diff;

  void mapInit(byte level = 0){
     switch (level){
      case 1:
        map = new Primitive[4] {
          Primitive('=', 6, 1),
          Primitive('=', 6, 2),
          Primitive('=', 13, 1),
          Primitive('=', 13, 2),
          };
          mapSize = 4;
        break;
        case 2:
        map = new Primitive[8] {
          Primitive('=', 4, 0),
          Primitive('=', 4, 1),
          Primitive('=', 8, 2),
          Primitive('=', 8, 3),
          Primitive('=', 12, 0),
          Primitive('=', 12, 1),
          Primitive('=', 16, 2),
          Primitive('=', 16, 3)
          };
          mapSize = 8;
        break;
        case 0:       
          mapSize = 0;
        break;   
      }
    }
  
  void drawSnake(){
    lcd.clear();
    lcd.setCursor(fruit.x, fruit.y);
    lcd.print(fruit.symbol);
    
    for(byte i = 0; i<character->length; i++){
      lcd.setCursor(character->snake[i].x, character->snake[i].y);
      lcd.print(character->snake[i].symbol);
      }    

    for(byte i = 0; i<mapSize; i++){
      lcd.setCursor(map[i].x, map[i].y);
      lcd.print(map[i].symbol);
      }
    }
    
  void gameOver(){
    bIsGameOver = true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("U'r score: ");
    lcd.print(score);    
    lcd.setCursor(0,1);
    lcd.print("Press A to start!");
    lcd.setCursor(0,2);
    lcd.print("Choose map: ");
    lcd.print("C< ");
    lcd.print(level);
    lcd.print(" >D");
    lcd.setCursor(0,3);
    lcd.print("Set diff: ");
    lcd.print("*< ");
    lcd.print(diff);
    lcd.print(" >#");
    }

  void checkConditions(){
    if(character->snake[0].x>=20 || character->snake[0].x<0 || character->snake[0].y>=4|| character->snake[0].y<0){
        gameOver();
      }
      
    for(byte i = 1; i<character->length; i++){
     if(character->snake[0].x == character->snake[i].x && character->snake[0].y == character->snake[i].y){
        gameOver();
        }
      }
     for(byte i = 0; i<mapSize; i++){
      if(character->snake[0].x == map[i].x && character->snake[0].y == map[i].y){
        gameOver();
        }
      }
      
    if(fruit.x == character->snake[0].x && fruit.y == character->snake[0].y){
      character->grow();
      spawnFruit();
      score+=1*((double)10/(double)(11-diff));
      }
    }

   void spawnFruit(){
    randomSeed(analogRead(0));
    byte x = random(0,20);
    byte y = random(0,4);

    for(byte i = 1; i<character->length; i++){
     if(x == character->snake[i].x && y == character->snake[i].y){
      spawnFruit();
      return;  
      }
    }
    for(byte i = 0; i<mapSize; i++){
      if(x == map[i].x && y == map[i].y){
      spawnFruit();
      return; 
      }       
    }  
    fruit.x = x;
    fruit.y = y;   
   }
    
public:
  bool bIsGameOver = true;
  
  Game(byte diff = 5, byte level = 0){
    fruit.symbol = '*';
    bIsGameOver = false;
    character = new Snake();
    this->diff = diff;
    this->level = level;
    if(level>3 || level <0){
      mapInit(0);
      }
    else{
      mapInit(level);
      }
    drawSnake();
    spawnFruit();
    }

  void tick(){
    character->move();
    drawSnake();
    checkConditions();
    }

  void changeDirection(Directions direction){
    character->changeDirection(direction);
    }

  ~Game(){
    delete character;
    delete map;
    }
   
  };  
  
char keys[KEY_ROWS][KEY_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
}; 
byte rowPins[KEY_ROWS] = {11,10, 9, 8}; 
byte colPins[KEY_COLS] = {7, 6, 5, 4}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, KEY_ROWS, KEY_COLS );
byte gameDiff = 5;
byte level = 0;
Game* game;

extern int __bss_end;
extern void *__brkval;

int memoryFree()
{
   int freeValue;
   if((int)__brkval == 0){
      freeValue = ((int)&freeValue) - ((int)&__bss_end);
   }
   else{
      freeValue = ((int)&freeValue) - ((int)__brkval);
   }
   return freeValue;
}

void diffOutput(){
  lcd.setCursor(13,3);
  lcd.print("  ");
  lcd.setCursor(13,3);
  lcd.print(gameDiff);
  }
  
void levelOutput(){
  lcd.setCursor(15,2);
  lcd.print(level);
  }
  
void setup() {
  Serial.begin(9600);
  lcd.init();                            // Инициализация lcd             
  lcd.backlight();                       // Включаем подсветку
  lcd.setCursor(0, 0);                   
  lcd.print("Press A to start!");  
  lcd.setCursor(0, 1);                   
  lcd.print("Use 2,4,5,6 to move!"); 
  lcd.setCursor(0,2);
  lcd.print("Choose map: ");
  lcd.print("C< ");
  lcd.print(level);
  lcd.print(" >D"); 
  lcd.setCursor(0,3);
  lcd.print("Set diff: ");
  lcd.print("*< ");
  lcd.print(gameDiff);
  lcd.print(" >#"); 
}

unsigned long timeA;
unsigned long timeB;
bool bIsKeyPressed = false;

void loop() {
 char key = keypad.getKey();
 Serial.println("I'm in loop!");
 if(!bIsKeyPressed){
  switch (key){
    case 'A': 
      delete game;  
      game = new Game(gameDiff, level);
      Serial.println(memoryFree());
      break;
    case '*':  
      if(gameDiff<=1 || !game->bIsGameOver){
        break;
        }
      gameDiff--;
      diffOutput();
      break;
    case '#': 
      if(gameDiff>=10 || !game->bIsGameOver){
        break;
        }
      gameDiff++;
      diffOutput();
      break;
    case 'D':
      if(level>=2 || !game->bIsGameOver){
         break;
         }
      level++;
      levelOutput(); 
      break;
    case 'C':
      if(level<=0 || !game->bIsGameOver){
         break;
         }
      level--;
      levelOutput(); 
      break;  
    case '1':
      game->tick();
      break;
    case '2':
      game->changeDirection(Up);
      bIsKeyPressed = true;
      break;
    case '4':
      game->changeDirection(Left);
      bIsKeyPressed = true;
      break;
    case '6':
      game->changeDirection(Right);
      bIsKeyPressed = true;
      break;
    case '5':
      game->changeDirection(Down);
      bIsKeyPressed = true;
      break;  
  }     
 }
  if((timeB - timeA)>1100-(gameDiff*100)){    
    if(!game->bIsGameOver){
    game->tick();
    timeA = millis();
    }
    bIsKeyPressed = false;
    }
  timeB = millis();
}
