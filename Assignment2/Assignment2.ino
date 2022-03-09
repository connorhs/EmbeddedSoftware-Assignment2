
#include <Ticker.h>

#define watchdogPin 26
#define digitalInputPin 33
#define frequencyMeasurePin 27
#define analogueInputPin 32
#define LedPin 25
#define timingPin 14

int timingState = 0;

int digitalInput = 0;
float measuredFrequency = 0.0;
int analogueReading = 0;
float analogueVoltage = 0.0;
float filterArray[] = {0, 0, 0, 0};
float filteredVoltage = 0.0;
int errorCode = 0;

Ticker cyclicExecutiveClock;
int tickCounter = 0;

// A function used to measure the timings of the cyclic executive. Every time a task is executed the timing pin is toggled between high and low. By looking at this pin on an oscilloscope, the timings of the cyclic executive can be determined
void toggleTimingPin()
{
  // Toggle the state of the timing pin
  if (timingState == 0)
  {
    timingState = 1;
    digitalWrite(timingPin, HIGH); 
  }
  else
  {
    timingState = 0;
    digitalWrite(timingPin, LOW); 
  }
}

// Each task has two components:
// The TimingX struct contains the information used to time the task appropriately according to the cyclic executive shedule. All timings are measured in clock ticks (1 tick = 5ms)
// Task 1 is called every 20ms (4 ticks of the 5ms clock)
struct Timing1
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing1 = {4, 0};

// The ExecuteTaskX() function is used to execute the functionality of each class
// Task 1 pulses a digital watchdog signal for 50us
void ExecuteTask1()
{
  toggleTimingPin();
  
  // Pulse the output high for 50 microseconds
  digitalWrite(watchdogPin, HIGH);
  delayMicroseconds(50);
  // Set the output low until the task is re-executed 20ms later
  digitalWrite(watchdogPin, LOW);
}


// Task 2 is called every 200ms (40 ticks), but is offset from task 1 by 15ms (3 ticks)
struct Timing2
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing2 = {40, 3};

// Task 2 monitors a digital input (push button) and toogles a state variable when the button is pressed
void ExecuteTask2()
{
  toggleTimingPin();
  
  // If the button has been pressed
  if (digitalRead(digitalInputPin))
  {
    // Toggle the input state variable
    if (digitalInput == 0) digitalInput = 1;
    else digitalInput = 0; 
  }
}


// Task 3 is called every 1000ms (200 ticks), offset from task 1 by 65ms (13 ticks)
struct Timing3
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing3 = {200, 13};

// Task 3 measures the frequency of a square wave on one of the GPIO pins. The square wave will be between 0.5 & 1 kHz, with a duty cycle of 50%
void ExecuteTask3()
{
  toggleTimingPin();
  
  // Use the pulseIn() Arduino function to get the high period of the square wave in microseconds. This is half the period of the full square wave
  float period = pulseIn(frequencyMeasurePin, HIGH);
  // Double the high period to get the period of the full square wave
  period *= 2;
  // Get the frequency in Hz: f = 1/T(s) = 1000000/T(us)
  measuredFrequency = 1000000 / period;
}


// Task 4 is called every 40ms (8 ticks), offset from task 1 by 5ms (1 tick)
struct Timing4
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing4 = {8, 1};

// Task 4 takes an analogue reading from a potentiometer
void ExecuteTask4()
{
  toggleTimingPin();
  
  // Use the Arduino analogRead() command to get a value between 0 and 4095 based on the input voltage of the potentiometer (0V - 3.3V)
  analogueReading = analogRead(analogueInputPin);
  // Map the reading back to a voltage (0V - 3.3V). The map() function cannot take a float argument so the reading will be mapped to 3300 then devided into a float
  analogueVoltage = map(analogueReading, 0, 4095, 0, 3300);
  // Cast int to float and divide into correct range
  analogueVoltage = (float)analogueVoltage / 1000;
}


// Task 5 is called every 40ms (8 ticks), offset from task 1 by 10ms (2 ticks)
struct Timing5
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing5 = {8, 2};

// Task 5 is a moving average filter which calculates the average value of the last 4 task 4 readings
void ExecuteTask5()
{
  toggleTimingPin();
  
  filteredVoltage = 0.0;
  // Shift new reading into filter
  for (int i = 3; i >= 0; i--)
  {
    // Elements 1, 2 nd 3 are right-shifted
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


// Task 6 is called every 100ms (20 ticks), offset by 35ms (7 ticks)
struct Timing6
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing6 = {20, 7};

// Task 6 executes the __asm__ __volatile__ ("nop") command 1000 times
void ExecuteTask6()
{
  toggleTimingPin();
  
  for (int i = 1000; i > 0; i--)
  {
    __asm__ __volatile__ ("nop");  
  }
}


// Task 7 is called every 400ms (80 ticks), offset by 25ms (5 ticks)
struct Timing7
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing7 = {80, 5};

// Task 7 sets an error code depending on whether the filtered task 5 result is above or below half its maximum range
void ExecuteTask7()
{
  toggleTimingPin();
  
  // If filtered analogue value is greater than half of the maximum range (3.3V / 2 = 1.65V)
  if (filteredVoltage > 1.65)
  {
    errorCode = 1;
  }
  else
  {
    errorCode = 0;
  }
}

// Task 8 is called every 400ms (80 ticks), offset by 30ms (6 ticks)
struct Timing8
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
} timing8 = {80, 6};

// Task 8 visualises the error code provided by task 7 on an LED
void ExecuteTask8()
{
  toggleTimingPin();
  
  // Turn LED on if the error code is 1
  if (errorCode == 1)
    digitalWrite(LedPin, HIGH);
  // Turn LED off if the error code is 0
  else
    digitalWrite(LedPin, LOW);
}


// Task 9 is called every 2000ms (400 ticks), offset by 70ms (14 ticks)
struct Timing9
{
  // The period after which the task repeats
  int period;
  // The offset from which the task is called
  int offset;
}timing9 = {400, 14};

// Task 9 prints data to the serial monitor is comma separated value format (digitalInput, measuredFrequency, filteredVoltage). The frequency will be rounded to an integer value
void ExecuteTask9()
{
  toggleTimingPin();
  
  // A temporary buffer to hold the concatonated string
  char dataString[16];
  // Concatonate the appropriate variables into a string in comma separated format. Casting frequency to an integer and limiting the voltage to 2dp
  sprintf(dataString, "%d, %d, %.2f\n", digitalInput, (int)measuredFrequency, filteredVoltage);
  // Print the string to the serial monitor
  Serial.print(dataString);
}


// The main cyclic executive function. This function calls tasks 1-9 at the appropriate times using a counter to measure each 5ms tick in the 2000ms hyperperiod.
// eg Task 2 is called every 200ms (200 / 5 = 40 ticks), and is offset by 15 ms (3 ticks). Hence, the cyclic executive must call task 2 on ticks 3, 43, 83, ...
void CyclicExecutive()
{
  // For each task, determine whether it should be executed this tick using the following check:
  if (((tickCounter - timing1.offset) % timing1.period) == 0) { ExecuteTask1(); }
  
  if (((tickCounter - timing2.offset) % timing2.period) == 0) { ExecuteTask2(); }
    
  if (((tickCounter - timing3.offset) % timing3.period) == 0) { ExecuteTask3(); }

  if (((tickCounter - timing4.offset) % timing4.period) == 0) { ExecuteTask4(); }

  if (((tickCounter - timing5.offset) % timing5.period) == 0) { ExecuteTask5(); }

  if (((tickCounter - timing6.offset) % timing6.period) == 0) { ExecuteTask6(); }

  if (((tickCounter - timing7.offset) % timing7.period) == 0) { ExecuteTask7(); }

  if (((tickCounter - timing8.offset) % timing8.period) == 0) { ExecuteTask8(); }

  if (((tickCounter - timing9.offset) % timing9.period) == 0) { ExecuteTask9(); }
  
  // Increment the tick counter
  tickCounter++;
  // If the tickCounter exceeds the hyperperiod, wrap around to zero
  if (tickCounter > 2000) tickCounter = 0;
}

void setup() 
{
  // Set up serial communication, baud rate was increased from 9600 to 115200 decrease the time taken on each Serial.print() call
  Serial.begin(115200);
  // Set up IO pins
  pinMode(watchdogPin, OUTPUT);
  pinMode(LedPin, OUTPUT);
  pinMode(timingPin, OUTPUT);
  pinMode(digitalInputPin, INPUT);

  // Set up the ticker obejct to call the CyclicExecutive() function on a clock cycle every 5ms
  cyclicExecutiveClock.attach_ms(5, CyclicExecutive);
}

// The loop function is unused by the cyclic executive, but was used during testing to time the individual tasks
void loop()
{ 
//  unsigned long startingTime = millis();
//  for (int i = 0; i < 1000; i++){
//      ExecuteTask6();
//  }
//  unsigned long timeTaken = millis() - startingTime;
//  Serial.print(timeTaken); Serial.print(" ");
}
