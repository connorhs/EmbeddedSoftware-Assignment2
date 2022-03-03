
#define watchdogPin 26
#define digitalInputPin 33
#define frequencyMeasurePin 27
#define analogueInputPin 32
#define LedPin 25

unsigned char digitalInput = 0;
unsigned long lastInputTime = 0;
unsigned int debounceDelay = 1;
float measuredFrequency = 0;
int analogueReading = 0;

void setup() 
{
  Serial.begin(9600);
  pinMode(watchdogPin, OUTPUT);
  pinMode(digitalInputPin, INPUT);
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
  // Use the pulseIn() Arduino function to get the periods of the high and low pulses of the square wave
  float highPeriod = pulseIn(frequencyMeasurePin, HIGH);
  float lowPeriod = pulseIn(frequencyMeasurePin, LOW);
  // Get the square wave period in microseconds
  float measuredPeriod = highPeriod + lowPeriod;
  // Get the frequency in Hz
  measuredFrequency = 1000000 / measuredPeriod;
}

// Task 4: Read analogue input every 41.6667ms (approx 42ms: 23.81Hz)
void Task4()
{
  // Get a value between 0 and 4095
  analogueReading = analogRead(analogueInputPin);
}

// Task 5: Moving average filter with previous 4 values of task 4. Called every 41.6667ms (approx 42ms: 23.81Hz)
void Task5()
{
  
}

// Task 6: 
void Task6()
{
  
}

void loop()
{  
//  unsigned long startingTime = millis();
//  for (int i = 0; i < 1; i++){
//      Task3(); 
//  }
//  unsigned long timeTaken = millis() - startingTime;
//  Serial.print(timeTaken); Serial.print(" ");
  Task4();
  Serial.print(analogueReading); Serial.print(" ");
  delay(42);
}
