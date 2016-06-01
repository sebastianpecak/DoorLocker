#ifndef CARDSMANAGER_H
#define CARDSMANAGER_H

#include <EEPROM.h>

#define MIFIRE_UID_LENGTH 4
// 1 master 10 standart
#define MAX_CARDS 11

class CardsManager {
public:
  enum Status {
    OK, ERROR, ADDED, NOTADDED, NOTEXIST, EXIST
  };

private:
  int _cardsArrayAdress;
  byte* _cardsArray;

  int compareUID(byte*, byte*);
public:

  // we can select where cards array is located in eeprom
  // by default it is 0, beginning
  Status init(int = -1);
  int getCardsNumber();
  Status doesCardExist(byte *);
  Status addCard(byte *);
  bool isMasterCard(byte *);
  // Updates data in eeprom memory so it can be used further
  Status commitChanges();

  void printBuffer();
};

/////////////////////////////////////////////////////
CardsManager::Status CardsManager::init(int adress) {
  // Look up for cards and update _cardsNumber var
  if(adress == -1) _cardsArrayAdress = 0;
  // It's not very professional so we do not check data correctness
  else _cardsArrayAdress = adress;

  // read numbers of cards
  int cardsNumber = EEPROM.read(_cardsArrayAdress);

  // Create in-memory buffer
  int bufferSize = 1 + (MAX_CARDS * MIFIRE_UID_LENGTH);
  _cardsArray = new byte[bufferSize];
  if(_cardsArray) {
    for(int i = 0; i < bufferSize; i++) _cardsArray[i] = EEPROM.read(_cardsArrayAdress + i);
    return OK;
  }
  else return ERROR;
}

/////////////////////////////////////////////////////
int CardsManager::getCardsNumber() {
  if(_cardsArray) return *_cardsArray;
  else return 0;
}

/////////////////////////////////////////////////////
CardsManager::Status CardsManager::doesCardExist(byte *uid) {
  for(int i = 0; i < *_cardsArray; i++) {
    if(compareUID(uid, (_cardsArray + 1 + (i*MIFIRE_UID_LENGTH))) == 1) return EXIST; 
  }
  return NOTEXIST;
}


/////////////////////////////////////////////////////
int CardsManager::compareUID(byte *uid1, byte *uid2) {
  if(uid1 && uid2) {
    for(int i=0; i<4;i++) {
      if(*(uid1 + i) != *(uid2 + i)) return 0;
    }
    // The same
    return 1;
  }
  // Error
  else return -1;
}

/////////////////////////////////////////////////////
CardsManager::Status CardsManager::addCard(byte *uid) {
  if(uid && (doesCardExist(uid) == NOTEXIST) && (*_cardsArray <= 11)) {
    int newCardAdress = 1 + ((*_cardsArray) * MIFIRE_UID_LENGTH);
    for(int i = 0; i < MIFIRE_UID_LENGTH; i++) *(_cardsArray + newCardAdress + i) = *(uid+i);
    // Set card couner
    (*_cardsArray)++;
    return ADDED;
  }
  else return NOTADDED;
}

/////////////////////////////////////////////////////
void CardsManager::printBuffer() {
  for(int i=0; i<(1+(11*MIFIRE_UID_LENGTH)); i++) {
    if(i==0) Serial.print("Cards: ");
    else if(i%4 == 1) Serial.print("Next card: ");
    Serial.println(_cardsArray[i], HEX);
  }
}

/////////////////////////////////////////////////////
CardsManager::Status CardsManager::commitChanges() {
  if(_cardsArray) {
    // Check out if there was anything change
    int inEeprom = EEPROM.read(_cardsArrayAdress), inMemory = *_cardsArray;
    if(inMemory > inEeprom) {
      int toSave = inMemory - inEeprom;
      for(int i=0; i<(toSave*MIFIRE_UID_LENGTH); i++)
        EEPROM.write(_cardsArrayAdress+1+(inEeprom*MIFIRE_UID_LENGTH)+i, *(_cardsArray+1+(inEeprom*MIFIRE_UID_LENGTH)+i));

      // Change cards number
      EEPROM.write(_cardsArrayAdress, *_cardsArray); 
    }
    else Serial.println("There is nothing to commit.");

    return OK;
  }
  else return ERROR;
}

/////////////////////////////////////////////////////
// This ought to be more like professional...
// but for now kiss <3
bool CardsManager::isMasterCard(byte *uid) {
  if(uid && compareUID(_cardsArray+1, uid) == 1) return true;
  else return false;
}

#endif  // CARDSMANAGER_H
