game_level level;

byte actualLevelIndex;
byte hashedShape[25];
byte cellSize;
byte xSelect;
byte ySelect;
byte maxSelect;
byte movesMade;
boolean isSolved = false;
boolean isGameCompleted = false;
boolean isConfirmDialogShown = false;

#define rotationTone 200.00
#define shapeX 4
#define shapeY 1
#define cellPadding 2
#define previewCellPadding 0
#define previewCellSize 9

void gameScreen_setup() {
  xSelect = 0;
  ySelect = 0;
  movesMade = 0;
  isSolved = false;
  isGameCompleted = false;
}

void gameScreen_loop() {
  //arduboy.digitalWriteRGB(GREEN_LED, RGB_OFF);

  drawLevel();
  drawLevelInfo();
    
  if (isSolved) {
    handleSolvedButtons();
    drawSolvedDialog();
  } else if (isGameCompleted) {
    handleGameCompletedButtons();
    drawGameCompletedDialog();
  } else if (isConfirmDialogShown) {
    handleExitDialogButtons();
    drawExitDialog();
  } else {
    handleGameButtons();
    drawMoves();
  }
  
  beep.timer();
}

void prepareLevel(byte levelIndex) {
  actualLevelIndex = levelIndex;
  memcpy_P(&level, &levels[levelIndex], sizeof(game_level));
  memcpy(hashedShape, level.hashedShape, sizeof(level.hashedShape[0])*25);
}

void drawMoves() {
  arduboy.setCursor(74, 57);
  arduboy.print("Moves: " + String(movesMade));
}

void drawLevelInfo() {
  arduboy.setCursor(74, 48);
  arduboy.print("Level: " + String(actualLevelIndex));
}

void drawLevel() {
  byte previewLocation = 74;

  switch (level.levelSize) {
    case 4:
      cellSize = 14;
      maxSelect = 2;
      break;
    case 5:
      cellSize = 11;
      maxSelect = 3;
      break;
  }
  
  drawShape(hashedShape, level.levelSize, shapeX, shapeY, cellSize, cellPadding);

  drawShape(level.correctShape, level.levelSize, previewLocation, shapeY, previewCellSize, previewCellPadding);

  arduboy.drawLine(70, 0, 70, 68);

  arduboy.drawRect(
    (shapeX + (xSelect * (cellSize + cellPadding)) - 1), 
    (shapeY + (ySelect * (cellSize + cellPadding)) - 1), 
    cellSize * 2 + cellPadding * 1 + 2,
    cellSize * 2 + cellPadding * 1 + 2
  );
}

void drawShape(byte shape[], byte levelSize, byte left, byte top, byte cellSize, byte padding) {
  byte cornerRadius = 1;
  for (byte y = 0; y < levelSize; y++) {
    for (byte x = 0; x < levelSize; x++) {
      byte value = shape[y * levelSize + x];
      byte startX = left + x * (cellSize + padding);
      byte startY = top + y * (cellSize + padding);
      if (value == 1 || value == 2 || value == 3) {
        // Stroke
        arduboy.drawRoundRect(startX, startY, cellSize, cellSize, cornerRadius);
      }
      if (value == 0) {
        // Solid
        arduboy.fillRoundRect(startX, startY, cellSize, cellSize, cornerRadius);
      } else if (value == 2) {
        // Lines
        arduboy.drawLine(startX + 1, startY + 1, startX + cellSize - 2, startY + cellSize - 2);
        arduboy.drawLine(startX + 1, startY + cellSize - 2, startX + cellSize - 2, startY + 1);
      } else if (value == 3) {
        // Dots
        for (byte iy = 2; iy < cellSize - 1; iy += 2) {
          for (byte ix = 2; ix < cellSize - 1; ix += 2) {
            arduboy.drawPixel(startX + ix, startY + iy);
          }
        }
      }
    }
  }
}

void drawGameCompletedDialog() {
  drawDialogBase();
  arduboy.setCursor(9, 11);
  arduboy.print("Game is completed!");
  resetTextColor();
}

void drawSolvedDialog() {
  drawDialogBase();
  
  drawCenterText("Solved in " + String(movesMade) + " moves!", 12, false);
  
  short starX = 37;
  byte starY = 30;

  byte starsNumber;

  if (movesMade <= level.stars[0]) {
    starsNumber = 3;
  } else if (movesMade <= level.stars[1]) {
    starsNumber = 2;
  } else {
    starsNumber = 1;
  }
  
  char *secondStar = starsNumber > 1 ? starFilled : starEmpty;
  char *thirdStar = starsNumber > 2 ? starFilled : starEmpty;
    
  arduboy.drawBitmap(starX, starY, starFilled, ICON_SIZE, ICON_SIZE, BLACK);
  arduboy.drawBitmap(starX + 19, starY, secondStar, ICON_SIZE, ICON_SIZE, BLACK);
  arduboy.drawBitmap(starX + 38, starY, thirdStar, ICON_SIZE, ICON_SIZE, BLACK);
  
  resetTextColor();
}

void rotateSelection() {
  byte startOffset = ySelect * level.levelSize + xSelect;
  
  byte levelPart[4] = {
    hashedShape[startOffset], hashedShape[startOffset + 1],
    hashedShape[startOffset + level.levelSize], hashedShape[startOffset + level.levelSize + 1],
  };

  hashedShape[startOffset] = levelPart[2];
  hashedShape[startOffset + 1] = levelPart[0];
  hashedShape[startOffset + level.levelSize] = levelPart[3];
  hashedShape[startOffset + level.levelSize + 1] = levelPart[1];
}

boolean isShapeCorrect() {
  for (byte n = 0; n < 25; n++) { 
    if (hashedShape[n] != level.correctShape[n]) {
      return false;
    }
  }
  return true;
}

void drawExitDialog() {
  drawDialogBase();
  
  arduboy.setCursor(9, 11);
  arduboy.print("Exit level?");
  arduboy.setCursor(9, 46);
  arduboy.print("Yes(A)   No(B)");
  
  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
}

void handleSolvedButtons() {
  // A button is exiting to level select
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    if (isGameCompleted) {
      isSolved = false;
    } else {
      switchToSelectLevelScreen();
    }
  }
}

void handleGameCompletedButtons() {
  // A button is exiting to level select
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    switchToSelectLevelScreen();
  }
}

void handleExitDialogButtons() {
  // A button is exiting to level select
  if (arduboy.justPressed(A_BUTTON)) {
    isConfirmDialogShown = false;
      switchToSelectLevelScreen();
  } else if (arduboy.justPressed(B_BUTTON)) {
    isConfirmDialogShown = false;
  }
}

void handleGameButtons() {

  // A button is exiting to level select
  if (arduboy.justPressed(A_BUTTON)) {
    if (isSolved) {
      switchToSelectLevelScreen();
    } else {
      isConfirmDialogShown = true;
    }
  }

  // B button is rotating
  if (arduboy.justPressed(B_BUTTON)) {
    if (isSolved) {
      switchToSelectLevelScreen();
    } else {
      playRotationSound();
      rotateSelection();
  
      movesMade = movesMade + 1;
        
      if (ONE_ROTATION_TO_SOLVE || isShapeCorrect()) {
        isSolved = true;
        isGameCompleted = actualLevelIndex + 1 == LEVELS_COUNT;
        //arduboy.digitalWriteRGB(GREEN_LED, RGB_ON);
        saveGameResult();
      }
    }
  }

  // Selection movement
  if (arduboy.justPressed(LEFT_BUTTON) && xSelect > 0) {
    xSelect = xSelect - 1;
  }
  if (arduboy.justPressed(RIGHT_BUTTON) && xSelect < maxSelect) {
    xSelect = xSelect + 1;
  }
  if (arduboy.justPressed(UP_BUTTON) && ySelect > 0) {
    ySelect = ySelect - 1;
  }
  if (arduboy.justPressed(DOWN_BUTTON) && ySelect < maxSelect) {
    ySelect = ySelect + 1;
  }
}

void saveGameResult() {
  byte levelIndex = actualLevelIndex + 1;
  byte savedLevelMoves = readLevelMoves(levelIndex);
  byte savedSolvedLevel = getLastSolvedLevelIndex();
  
  if (savedSolvedLevel <= 0 || levelIndex > savedSolvedLevel){
    setSolvedLevel(levelIndex);
  }

  if (savedLevelMoves <= 0 || movesMade < savedLevelMoves) {
    saveLevelMoves(levelIndex, movesMade);
  }
}

void playRotationSound() {
  if (isAudioOn()) {
    beep.tone(beep.freq(rotationTone), 2);
  }
}
