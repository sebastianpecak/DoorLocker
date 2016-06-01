#include "CardsManager.h"
#include "Others.h"
#include "Door.h"
#include "Monitor.h"
#include <SPI.h>
#include <MFRC522.h>

// Here u can change pin configuration
#define PinSDA 10
#define PinRESET 9

// Modes
#define StdMode 1
#define ProgramMode 2

// Current program mode
static int currentMode = StdMode;

// This is the max number of attempts to unlock the door
// After this number door can only be opened by phicical key
// And module has to be restarted 
#define MaxUnlockAttempt 3

CardsManager cardsManager;
MFRC522 nfcModule(PinSDA, PinRESET);
int unlockAttemptCounter = 0;

void setup() {
  // Just for debugging now
  Serial.begin(9600);
  while(!Serial) {}
  SPI.begin();

  // Be sure that doors are locked
  FrontDoor.init();

  // Init nfc module
  nfcModule.PCD_Init();

  // Init cards
  cardsManager.init();

  Serial.println("Stared...");

  Monitor.init();
  Monitor.lightUp();

  // Enable watchdog just to be sure that it will keep running all the time
  wdt_enable(WDTO_4S);
}

void loop() {
  // Reset watchdog to not get interrupted
  wdt_reset();
  // Lookup for new cards
  if(nfcModule.PICC_IsNewCardPresent() && nfcModule.PICC_ReadCardSerial()) {
    if(cardsManager.isMasterCard(nfcModule.uid.uidByte)) {
      if(currentMode == ProgramMode) {
        // Apply changes
        if(cardsManager.commitChanges() == CardsManager::OK) Serial.println("Changes commited.");
        else Serial.println("Changes could not be commited.");

       cardsManager.printBuffer();

        // Return to stdMode
        currentMode = StdMode;

        Serial.println("Leaving program mode.");
      }
      else {
        Serial.println("Entering program mode.");
        currentMode = ProgramMode;
      }
    }

    else {
      if(currentMode == ProgramMode) {
        if(cardsManager.addCard(nfcModule.uid.uidByte) == CardsManager::ADDED) {
          Serial.print("\nAdded new card uid: ");
          printUid(nfcModule.uid.uidByte);
        }
        else {
          Serial.print("\nNot added new card uid: ");
          printUid(nfcModule.uid.uidByte);
        }
      }
      
      else {
        // Distinguish card
        if(cardsManager.doesCardExist(nfcModule.uid.uidByte) == CardsManager::EXIST) {
          // Reset counter
          unlockAttemptCounter = 0;
          Serial.println("Opening the door.");
          FrontDoor.letIn();
        }
        else {
          unlockAttemptCounter++;
          if(unlockAttemptCounter == MaxUnlockAttempt) {
            Serial.println("Entering safe mode. Need reset.");
            wdt_disable();
            // Turn off antene
            nfcModule.PCD_AntennaOff();
            while(true) {
              if(analogRead(0) == 0) {
                Serial.println("Leaving safe mode.");
                wdt_enable(WDTO_4S);
                unlockAttemptCounter = 0;
                nfcModule.PCD_AntennaOn();
                break;
              }
              delay(200);
            }
          }
        }
      }
    }

    nfcModule.PICC_HaltA();
  }
}

