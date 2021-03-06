/* 
 * EEPROM memory structure 
 * [0] - MEMORY_INITIALIZED_ADDRESS
 * [1] - Solved level index
 * [2] - Audio state, 0 - off, 1 - on
 * [level_index + LEVEL_ADDRESS_OFFSET] - made steps count for each level
 * 
 * if memory address for level_index is -1 or 0, then level is not solved
 */
#define MEMORY_INITIALIZED_ADDRESS 0
#define SOLVED_LEVEL_INDEX_ADDRESS 1
#define LEVEL_ADDRESS_OFFSET 2
#define AUDIO_STATE_ADDRESS 2

#define MEMORY_INITIALIZED_FLAG_VALUE 132

void initMemory() {
  if (EEPROM.read(MEMORY_INITIALIZED_ADDRESS) != MEMORY_INITIALIZED_FLAG_VALUE) {
    clearMemory();
    EEPROM.write(MEMORY_INITIALIZED_ADDRESS, MEMORY_INITIALIZED_FLAG_VALUE);
  }
}

void clearMemory() {
  byte memorySize = 1 /* SOLVED_LEVEL_INDEX_ADDRESS */ + 1 /* AUDIO_STATE_ADDRESS */ + LEVELS_COUNT;
  for (byte i = 1; i <= memorySize; i++) {
    EEPROM.write(i, 0);
  }
}

void resetGame() {
  EEPROM.write(SOLVED_LEVEL_INDEX_ADDRESS, 0);
  for (int levelIndex = LEVEL_ADDRESS_OFFSET; levelIndex <= LEVELS_COUNT; levelIndex++) {
    EEPROM.write(levelIndex + 1, 0);
  }
}

void setSolvedLevel(byte solvedLevelIndex) {
  saveValue(SOLVED_LEVEL_INDEX_ADDRESS, solvedLevelIndex);
}

byte getLastSolvedLevelIndex() {
  return readValue(SOLVED_LEVEL_INDEX_ADDRESS);
}

bool isAudioOn() {
  return readValue(AUDIO_STATE_ADDRESS) == 1;
}

void saveAudioState(bool isOn) {
  saveValue(AUDIO_STATE_ADDRESS, isOn ? 1 : 0);
}

void saveLevelMoves(int levelIndex, byte movesMade) {
  // levelIndex starts with 1
  saveValue(levelIndex + LEVEL_ADDRESS_OFFSET, movesMade);
}

byte readLevelMoves(int levelIndex) {
  // levelIndex starts with 1
  return readValue(levelIndex + LEVEL_ADDRESS_OFFSET);
}

void saveValue(int address, byte number) {
  EEPROM.write(address, number & 0xFF);
}

byte readValue(int address) {
  return EEPROM.read(address);
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
