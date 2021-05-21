// input pin definitions
#define DIGITS A4
#define OPERATIONS A3
#define POINT A5
#define SIGN A2
#define BUFFERSIZE 16
// header inclusion
#include <LiquidCrystal.h>

// liquid crystal configured to 4 bit mode
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// analogRead operation storage
int curDigit = 0, curOp = 0;
// variables for detecting a rising edge on analog pins
uint8_t keyDigit = 1, keyOp = 1, keyPoint = 1, keySign = 1;
// input buffer for storing a number text
char inputBuffer[BUFFERSIZE] = {};
// operand and result storage
double operand1 = 0;
double operand2 = 0;
double result = 0;
// input buffer index
uint8_t bufferIndex = 1;
// operation storage
char operation = 0;
// digit storage
char digit = 0;
// flags
bool signPlaced = 0;
bool pointPlaced = 0;
bool computationFinished = 0;
bool operand1Entered = 0;
int mainFunction();

void setup() {
  // configure input pins
  pinMode(DIGITS, INPUT);
  pinMode(OPERATIONS, INPUT);
  pinMode(POINT, INPUT);
  pinMode(SIGN, INPUT);
  // initialize the lcd display
  lcd.begin(16,2);
  // initialize inputBuffer
  for (uint8_t i = 0; i < BUFFERSIZE; i++) {
    inputBuffer[i] = ' ';
  }
  // set lcd display cursor position at first line and second column
  // the first column is used for displaying a number sign
  lcd.setCursor(1,0);
}

void loop() {
  // main calculator function is used to interrupt and start over at any time
  mainFunction();
}

int mainFunction() {
  // voltage level acquisition and low level noise suppresion
  curDigit = analogRead(DIGITS);
  if(curDigit < 5) curDigit = 0;
  curOp = analogRead(OPERATIONS);
  if(curOp < 5) curOp = 0;
  // ================= digits =====================
  // the numbers in parentheses are selected according to analogRead values
  // to detect any button press, we need to compare voltage level on the analog pin to
  // the range of a certain button
  // the if statement executes only if digit key is 1 and any digit button is pressed
  if(curDigit > 70 && keyDigit == 1) {
    // digit variable will be assigned a digit symbol
    digit = 0;
    if (curDigit <= 103 && curDigit >= 95){
      digit = '0';
    }
    if (curDigit <= 201 && curDigit >= 193){
      digit = '1';
    }
    if (curDigit <= 296 && curDigit >= 288){
      digit = '2';
    }
    if (curDigit <= 392 && curDigit >= 384){
      digit = '3';
    }
    if (curDigit <= 488 && curDigit >= 480){
      digit = '4';
    }
    if (curDigit <= 586 && curDigit >= 578){
      digit = '5';
    }
    if (curDigit <= 688 && curDigit >= 680){
      digit = '6';
    }
    if (curDigit <= 794 && curDigit >= 786){
      digit = '7';
    }
    if (curDigit <= 906 && curDigit >= 898){
      digit = '8';
    }
    if (curDigit <= 1023 && curDigit >= 1019){
      digit = '9';
    }
    // clear the screen after finishing a computation
    if(computationFinished == 1) {
      lcd.clear();
      lcd.setCursor(1,0);
      computationFinished = 0;
    }
    // if there was an assignment of the digit variable
    // and if bufferIndex doesn't exceed the buffer size limit
    // write the digit symbol to a input buffer element
    if (bufferIndex < BUFFERSIZE && digit != 0) {
      inputBuffer[bufferIndex] = digit;
      bufferIndex++;
      lcd.print(digit);
    }
    // write digit key to zero to execute only one time
    keyDigit = 0;
  }
  // reset digit key if no digit button is pressed
  if(curDigit < 70) keyDigit = 1;

  // the if statement executes only if operation key is 1 and any operation button is pressed
  if(curOp > 70 && keyOp == 1) {
    
    //convert first operand string into number while the operation is not chosen but an operation button is pressed
    if (operation == 0) {
      // count digits using std library function
      // since the atof function returns zero either if the number is zero or the input string is invalid
      // we have to manually check if the input string is valid to convert
      uint8_t digCount = 0;
      for(uint8_t i = 0; i < BUFFERSIZE; i++)
        digCount += isdigit(inputBuffer[i]);
      // if there's at least one digit, convert into operand1 via atof
      if(digCount > 0) {
        operand1 = atof(inputBuffer);
        // to keep the entered first operand number on the screen and enter the second operand,
        // we need to switch to the second screen line.
        // we can use a flag to indicate that the first operand is entered
        operand1Entered = 1;
      } else {
        // operand 1 is not created
        lcd.clear();
        lcd.print("NO OPERAND1");
        delay(1000);
        lcd.clear();
        // reset everything and start over
        reset();
        return 0;
      }
      
      // clear the buffer with spaces and reset index and flags related to number input
      for (uint8_t i = 0; i < BUFFERSIZE; i++) 
        inputBuffer[i] = ' ';
      pointPlaced = 0;
      bufferIndex = 1;
      signPlaced = 0;
      // =============== operations =================
      // choose and operation
      if (curOp <= 317 && curOp >= 314){ // op1^op2
        operation = '^';
      }
      if (curOp <= 471 && curOp >= 467){ // op1/op2
        operation = '/';
      }
      if (curOp <= 632 && curOp >= 628){ // op1*op2
        operation = '*';
      }
      if (curOp <= 813 && curOp >= 809){ // op1-op2
        operation = '-';
      }
      if (curOp <= 1023 && curOp >= 1021){ // op1+op2
        operation = '+';
      }
      lcd.setCursor(15,0);
      lcd.print(operation);
      lcd.setCursor(1,1);
    }
    
    // compute result if equals are chosen
    if (curOp <= 162 && curOp >= 158){
      // count spaces in the inputBuffer
      uint8_t digCount = 0;
      for(uint8_t i = 0; i < BUFFERSIZE; i++)
        digCount += isdigit(inputBuffer[i]);
      // if there is something in the buffer, convert it into second operand
      if(digCount > 0) {
        operand2 = atof(inputBuffer);
        // clear the input buffer and reset flags, and index
        for (uint8_t i = 0; i < BUFFERSIZE; i++)
          inputBuffer[i] = ' ';
        bufferIndex = 1;
        pointPlaced = 0;
        signPlaced = 0;
        // check if operation is chosen
        if (operation != 0) {
          // perform operation on operands
          switch(operation) {
            case '+':
              result = operand1+operand2;
            break;
            case '-':
              result = operand1-operand2;
            break;
            case '*':
              result = operand1*operand2;
            break;
            case '/':
              result = operand1/operand2;
            break;
            case '^':
              result = pow(operand1, operand2);
            break;
          }
          // clear the screen
          lcd.clear();
          // print all result digits
          lcd.print(result, 16);
          // reset everything
          reset();
          // reset operation key to make the if statement run a single time
          keyOp = 0;
          computationFinished = 1;
          return 1;
        } else {
          // print no operation error and reset
          lcd.clear();
          lcd.print("NO OP ERROR");
          delay(1000);
          lcd.clear();
          reset();
          return 0;
        }
      } else {
        // if operand2 is not created, then print error and reset
        lcd.clear();
        lcd.print("NO OPERAND2");
        delay(1000);
        lcd.clear();
        reset();
        return 0;
      }
      
    }
    // reset the operation key to make the if statement run a single time
    keyOp = 0;
  }
  // reset the operation key while no operation buttons are pressed
  if(curOp < 70) keyOp = 1;

  // add the decimal point to the number
  // the if statement runs only if point button is pressed and point key is 1
  if(digitalRead(POINT) == 1 && keyPoint == 1) {
    if (bufferIndex < BUFFERSIZE && pointPlaced != 1) {
      // fractions can also be written without a zero before the decimal point
      // clear the screen after the last computation
      if(computationFinished == 1) {
        lcd.clear();
        lcd.setCursor(1,0);
        computationFinished = 0;
      }
      // put the decimal point at the necessary input buffer position
      inputBuffer[bufferIndex] = '.';
      bufferIndex++;
      // don't allow placing a second decimal point
      pointPlaced = 1;
      lcd.print('.');
    }
    // reset the point key to make the if statement run a single time
    keyPoint = 0;
  }
  // reset the point key while the point button is not pressed
  if (digitalRead(POINT) == 0)
    keyPoint = 1;

  // add the minus sign to the input number and buffer
  // the if statement runs only if point button is pressed and point key is 1
  if(digitalRead(SIGN) == 1 && keySign == 1) {
    if (bufferIndex < BUFFERSIZE && signPlaced != 1) {
      // clear the screen after the last computation
      if(computationFinished == 1) {
        lcd.clear();
        lcd.setCursor(1,0);
        computationFinished = 0;
      }
      // put the minus sign at the zeroth input buffer location
      inputBuffer[0] = '-';
      // don't allow placing a second minus sign
      signPlaced = 1;
      // decide where to put the sign
      if(operand1Entered == 0) {
        lcd.setCursor(0,0);
        lcd.print('-');
        lcd.setCursor(bufferIndex,0);
      } else {
        lcd.setCursor(0,1);
        lcd.print('-');
        lcd.setCursor(bufferIndex,1);
      }
    }
    // reset the sign key to make the if statement run a single time
    keySign = 0;
  }
  // reset the sign key while the point button is not pressed
  if (digitalRead(SIGN) == 0)
    keySign = 1;
}

// reset function
void reset() {
  // reset operands
  operand1 = 0;
  operand2 = 0;
  // reset operation symbol
  operation = 0;
  // reset all flags
  pointPlaced = 0;
  signPlaced = 0;
  operand1Entered = 0;
  // clear the input buffer and index
  for (uint8_t i = 0; i < BUFFERSIZE; i++)
    inputBuffer[i] = ' ';
  bufferIndex = 1;
  // set cursor to the first lina and second column
  lcd.setCursor(1,0);
}
