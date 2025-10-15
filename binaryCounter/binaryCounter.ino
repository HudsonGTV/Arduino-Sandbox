#define DIGIT_COUNT 8
#define PIN_START_BIT 4

#define PIN_CT_ON 12

volatile uint8_t displayedNum = 0;
volatile unsigned long lastInterrupt = 0;
volatile unsigned long lastLEDUpdate = 0;
volatile bool enableAutoCount = false;

void setup() {
	// INIT BINARY LED LAMPS
	pinMode(4, OUTPUT);				// 1
	pinMode(5, OUTPUT);				// 2
	pinMode(6, OUTPUT);				// 4
	pinMode(7, OUTPUT);				// 8
	pinMode(8, OUTPUT);				// 16
	pinMode(9, OUTPUT);				// 32
	pinMode(10, OUTPUT);			// 64
	pinMode(11, OUTPUT);			// 128

	pinMode(PIN_CT_ON, OUTPUT);		// Counting LED
	pinMode(2, INPUT_PULLUP);		// BUTTON UP
	pinMode(3, INPUT_PULLUP);		// BUTTON DOWN
	pinMode(13, INPUT);				// BUTTON RESUME COUNT

	// Create interrupts
	attachInterrupt(digitalPinToInterrupt(2), editValue, FALLING);
	attachInterrupt(digitalPinToInterrupt(3), editValue, FALLING);
}

void displayNumber(int num, int digits = DIGIT_COUNT) {
	for(int i = 0; i < digits; ++i) {
		digitalWrite(i + PIN_START_BIT, num % 2);
		num /= 2;
	}
}

void loop() {

	// put your main code here, to run repeatedly:
	/*for(int i = 0; i < 16; ++i) {
		displayNumber(i, 4);
		delay(1000);
	}*/

	// SEE IF USER TOGGLES ON AUTO COUNT
	if(digitalRead(13) == LOW) {
		enableAutoCount = !enableAutoCount;
		delay(250);
	}

	// UPDATE VALUE EVERY SECOND IF AUTO COUNT ENABLED
	unsigned long now = millis();
	if(now - lastLEDUpdate >= 1000 && enableAutoCount) {
		++displayedNum;
		displayedNum %= 1 << DIGIT_COUNT;
		lastLEDUpdate = now;
	}

	// Display updated value on board
	displayNumber(displayedNum);

}

/**
 * For when a user pushes a button to increase/decrease counter
**/
void editValue() {

	// Disable auto counter if user manually changes value
	enableAutoCount = false;

	unsigned long now = millis();
	if(now - lastInterrupt > 250) {
		if(digitalRead(2) == LOW)
			++displayedNum;
		else if(digitalRead(3) == LOW)
			--displayedNum;
		// pow inefficient and unecessary
		//displayedNum %= (uint8_t)pow(2, DIGIT_COUNT) - 1;
		// We can bit shift left by the digits we have to constrain the max val displayedNum can have
		displayedNum %= 1 << DIGIT_COUNT;
		lastInterrupt = now;
	}

}
