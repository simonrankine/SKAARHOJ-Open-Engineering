/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_C15\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false);
  buttons.setDefaultColor(0); // Off by default
  buttons.setColorBalanceRGB(4, 100, 100, 100);
  buttons.setColorBalanceRGB(5, 10, 10, 10);
  if (getConfigMode()) {
    Serial << F("Test: BI8 Board\n");
    buttons.testSequence(10);
  } else
    delay(500);
  buttons.setButtonColorsToDefault();
  statusLED(QUICKBLANK);

  // Look for a button press / sustained button press to bring the device into config/config default modes:
  uint8_t retVal = 0;
  unsigned long timer = millis();
  if ((buttons.buttonIsPressedAll() & 0xFF) > 0) {
    retVal = 1;
    statusLED(LED_BLUE);
    while ((buttons.buttonIsPressedAll() & 0xFF) > 0) {
      if (sTools.hasTimedOut(timer, 2000)) {
        retVal = 2;
        statusLED(LED_WHITE);
      }
    }
  } else {
    // Preset
    if (getNumberOfPresets() > 1) {
      uint8_t presetNum = EEPROM.read(EEPROM_PRESET_START + 1);
      while (!sTools.hasTimedOut(timer, 2000) || buttons.buttonIsPressedAll() > 0) {
        uint8_t b16Map[] = {3, 4, 1, 2};
        for (uint8_t a = 0; a < 4; a++) {
          uint8_t color = b16Map[a] <= getNumberOfPresets() ? (b16Map[a] == presetNum ? 4 : 5) : 0;
          buttons.setButtonColor(a + 1, color);
          if (color > 0 && buttons.buttonIsHeldFor(a + 1, 2000)) {
            setCurrentPreset(b16Map[a]);
            buttons.setButtonColor(a + 1, 2);
            while (buttons.buttonIsPressedAll() > 0)
              ;
          }
        }
      }
      buttons.setButtonColorsToDefault();
    }
  }

  return retVal;
}

/**
 * Hardware test
 */
void HWtestL() {

#if (SK_MODELVAR == 1) // C15 with 2+8 buttons
  buttons.testProgramme(0x3FF);
#else
  buttons.testProgramme(B11001111);
#endif
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;

  // BI8 buttons:
  uint8_t b16Map[] = {8, 9, 6, 7, 0, 0, 1, 2}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons, b16Map, sizeof(b16Map));

  // SmartSwitch buttons:
  uint8_t b16Map2[] = {4, 3}; // These numbers refer to the drawing in the web interface
  HWrunLoop_SSWbuttons(b16Map2);
}
