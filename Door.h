#ifndef DOOR_H
#define DOOR_H

#define PinDoorLocker 8
#define UnlockTimeInSeconds 4

class {
public:
  inline void lock() {
    // Set propper locical state
    digitalWrite(PinDoorLocker, HIGH);
  }

  inline void unlock() {
    // Set propper logical state
    digitalWrite(PinDoorLocker, LOW);
  }
  void init() {
    pinMode(PinDoorLocker, OUTPUT);
    lock();
  }

  void letIn() {
    unlock();
    delay(UnlockTimeInSeconds * 1000);
    lock();
  }
  
} FrontDoor;

#endif  // DOOR_H
