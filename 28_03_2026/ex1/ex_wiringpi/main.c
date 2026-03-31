#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#define ledPin 12 // BCM Pin 12 (Physical Pin 32)

int main() {
   printf("Initializing GPIO...\n");
   
   if (wiringPiSetupGpio() == -1) {
      fprintf(stderr, "Unable to initialize wiringPi!\n");
      return 1;
   }

   pinMode(ledPin, OUTPUT);
   printf("Blinking LED on BCM pin %d (100ms interval)...\n", ledPin);

   while (1) {
      digitalWrite(ledPin, HIGH); // Use constants like HIGH/LOW for clarity
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
   }

   return 0;
}
