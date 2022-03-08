
#define watchdogPin 26
#define digitalInputPin 33
#define frequencyMeasurePin 27
#define analogueInputPin 32
#define LedPin 25

int digitalInput = 0;
float measuredFrequency = 0.0;
int analogueReading = 0;
float analogueVoltage = 0.0;
float filterArray[] = {0, 0, 0, 0};
float filteredVoltage = 0.0;
int errorCode = 0;

void setup() 
{
  Serial.begin(115200);
  pinMode(watchdogPin, OUTPUT);
  pinMode(digitalInputPin, INPUT);
  pinMode(LedPin, OUTPUT);
}

// Task 1: Digital watchdog pulse (50us) every 20650us (approx 20ms: 50Hz)
void Task1()
{
  digitalWrite(watchdogPin, HIGH);
  delayMicroseconds(50);
  digitalWrite(watchdogPin, LOW);
}

// Task 2: Monitor digital input every 200ms (5Hz)
void Task2()
{
  if (digitalRead(digitalInputPin))
  {
    if (digitalInput == 0) digitalInput = 1;
    else digitalInput = 0; 
  }
}

// Task 3: Measure frequency of square wave every 1000ms (1Hz)
void Task3()
{
  // Use the pulseIn() Arduino function to get the high period of the square wave in microseconds. This is half the period of the full square wave
  float period = pulseIn(frequencyMeasurePin, HIGH);
  // Double the high period to get the period of the full square wave
  period *= 2;
  // Get the frequency in Hz: f = 1/T(s) = 1000000/T(us)
  measuredFrequency = 1000000 / period;
}

// Task 4: Read analogue input every 41.6667ms (approx 42ms: 23.81Hz)
void Task4()
{
  // Get a value between 0 and 4095
  analogueReading = analogRead(analogueInputPin);
  analogueVoltage = map(analogueReading, 0, 4095, 0, 3300);
  analogueVoltage = (float)analogueVoltage / 1000;
}

// Task 5: Moving average filter with previous 4 values of task 4. Called every 41.6667ms (approx 42ms: 23.81Hz)
void Task5()
{
  filteredVoltage = 0.0;
  // Shift data into filter
  for (int i = 3; i >= 0; i--)
  {
    // Elements 1, 2a nd 3 are right-shifted
    if (i > 0)
    {
      filterArray[i] = filterArray[i - 1];
    }
    // New reading is shifted into element 0
    else
    {
      filterArray[i] = analogueVoltage;
    }
    // Sum all of the elements in the updated array
    filteredVoltage += filterArray[i];
  }
  
  // Divide to get the average value of the updated array
  filteredVoltage /= 4.0;
}

// Task 6: Execute __asm__ __volatile__ ("nop"); 1000 times. Called every 100ms
void Task6()
{
  for (int i = 1000; i > 0; i--)
  {
    __asm__ __volatile__ ("nop");  
  }
}

// Task 7: Get error code based on analgoue reading every 333.333ms (approx 334ms: 2.99Hz)
void Task7()
{
  // If filtered analogue value is greater than half of the maximum range (3.3V / 2)
  if (filteredVoltage > 1.65)
  {
    errorCode = 1;
  }
  else
  {
    errorCode = 0;
  }
}

// Task 8: Visualise the error code from task 7. Called every 333.333ms (approx 334ms: 2.99Hz)
void Task8()
{
  if (errorCode == 1)
    digitalWrite(LedPin, HIGH);
  else
    digitalWrite(LedPin, LOW);
}

// Task 9: Print to serial monitor every 5000ms (0.2Hz)
void Task9()
{
  // A temporary buffer to hold the concatonated string
  char dataString[16];
  // Concatonate the appropriate variables into a string in comma separated format. Casting frequency to an integer and limiting the voltage to 2dp
  sprintf(dataString, "%d, %d, %.2f\n", digitalInput, (int)measuredFrequency, filteredVoltage);
  Serial.print(dataString);
}

void loop()
{  
  unsigned long startingTime = millis();
  for (int i = 0; i < 1; i++){
      Task9();
  }
  unsigned long timeTaken = millis() - startingTime;
  Serial.print(timeTaken); Serial.print(" ");
//  Task2();
//  Task3();
//  Task4();
//  Task5();
//  Task7();
//  Task8();
//  Task9();
//  Serial.print(filteredVoltage); Serial.print(", ");
//  Serial.print(errorCode); Serial.print("\n");
//  delay(42);
}
