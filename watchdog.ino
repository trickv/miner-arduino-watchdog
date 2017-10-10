//USB hardware watchdog project
//Jason Greathouse 07/11/2011


int incomingByte = 0;	// for incoming serial data
int notAlive = 0;   // counter for seconds between alive
int handshake = 0;  // have we connected to watchdog daemon
int relayPin = 3; //pin for relay
int led = 13; // onboard LED pin
long tick = 1000; //miliseconds in a second
long previousMillis = 0;
int printed60 = 0; //Just "flags" to keep it from repeating messages
int printed120 = 0;

void setup()
{
	Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
	pinMode(relayPin, OUTPUT); //Set relay pin
	pinMode(led, OUTPUT);
	digitalWrite(relayPin, HIGH);
	establishContact(); //Wait for inital Keep Alive before starting timers
}



void loop()
{
	digitalWrite(led, LOW);
	delay(100);
	unsigned long currentMillis = millis(); //Currnet number of miliseconds since boot.

	if (Serial.available() > 0)
	{
		// read the incoming byte:
		incomingByte = Serial.read();
	}

	if (incomingByte == 'A')
	{
		//we got an alive message
		Serial.println("Received Alive");
		notAlive = 0;
		printed60 = 0;
		printed120 = 0;
		digitalWrite(led, HIGH);
	}
	if (incomingByte == 'R')
	{
		//we got a going down for reboot message, going back to establish contact.
		Serial.println("Received Reboot Message");
		establishContact();
		notAlive = 0;
		printed60 = 0;
		printed120 = 0;
	}


	// count off seconds
	if (currentMillis - previousMillis > tick)
	{
		previousMillis = currentMillis;
		notAlive = notAlive + 1;
		//Serial.print("No Alive for ");
		//Serial.print(notAlive);
		//Serial.println(" seconds");
	}
	
	if (printed60 == 1) {
		digitalWrite(led, HIGH);
		delay(100);
	}

	//Not alive warnings for 60/120/180 seconds
	if (notAlive == 60)
	{
		if (printed60 == 0)
		{
			Serial.println("No Alive for 60 Seconds");
			printed60 = 1;
		}
	}
	if (notAlive == 120)
	{
		if (printed120 == 0)
		{
			Serial.println("No Alive for 120 Seconds");
			printed120 = 1;
		}
	}
	if (notAlive >= 180)
	{
		//we have not seen an "A" in 180 seconds
		Serial.println("No Alive for 180 Seconds - Rebooting Now!");

		digitalWrite(relayPin, LOW); //Hit reset button
		delay(1000);
		digitalWrite(relayPin, HIGH);

		delay(5000); // Wait 5 seconds for the reboot
		notAlive = 0;
		printed60 = 0;
		printed120 = 0;
		establishContact();
	}

	incomingByte = 0;

}

//wait for an inital keepalive before starting timer
void establishContact()
{
	digitalWrite(led, LOW);
	while (incomingByte != 'A')
	{
		if (Serial.available() > 0)
		{
			// read the incoming byte:
			incomingByte = Serial.read();
		}
		Serial.println("Waiting for Initial KeepAlive...");
		delay(1000);
	}
	Serial.println("Initialized, Timer Started.");
	incomingByte = 0;
	digitalWrite(led, HIGH);
	
}
