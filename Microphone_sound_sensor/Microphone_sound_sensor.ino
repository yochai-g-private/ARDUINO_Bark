/*
 * Rui Santos
 * Complete Project Details http://randomnerdtutorials.com
*/

int ledPin = 12;
int sensorPin = 7;

unsigned long tick;
uint32_t cnt;
bool	 on;

void setup() {
	pinMode(ledPin, OUTPUT);
	pinMode(sensorPin, INPUT);
	Serial.begin(115200);
	Serial.println("Ready!!!");
	tick = millis();
}


void loop() {
	boolean val = digitalRead(sensorPin);
	static boolean prev_val;
	static unsigned long prev_tick;

	if (val != prev_val)
	{
		unsigned long now = millis();

		if ((now - prev_tick) < 10)
		{
			prev_tick = now;

		}

			if (val)
				cnt++;

			on = val;
			Serial.print(cnt); Serial.print(". ");
			if(on)
				Serial.println("ON");
			else
			{
				Serial.print("OFF : ");
				Serial.println(now - tick);
			}

			digitalWrite(ledPin, on ? HIGH : LOW);

			tick = now;
		}
	}
}