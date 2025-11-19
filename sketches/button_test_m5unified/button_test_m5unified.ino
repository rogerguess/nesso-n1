/*
 * Nesso N1 Button and Touch Test - M5Unified (CORRECT METHOD)
 *
 * This sketch properly reads buttons via the I2C GPIO expander
 * using the M5Unified library, plus touch screen detection.
 *
 * Hardware:
 * - KEY1 (Button A) - Left button
 * - KEY2 (Button B) - Right button
 * - Touch screen - Capacitive touch
 * - All buttons are on I2C expander (NOT direct GPIO!)
 *
 * Visual Feedback:
 * - Display shows button/touch press counts
 * - Buzzer beeps on button/touch press
 * - Serial output shows detailed events
 *
 * Library Required:
 * - M5Unified (install via Library Manager)
 */

#include <M5Unified.h>

// Buzzer configuration
#define BEEP_PIN 11

// Counters
int button1Count = 0;
int button2Count = 0;
int touchCount = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n\n========================================");
  Serial.println("  Nesso N1 BUTTON TEST");
  Serial.println("  Using M5Unified Library");
  Serial.println("========================================\n");

  // Initialize M5Unified (this sets up I2C GPIO expanders!)
  auto cfg = M5.config();
  M5.begin(cfg);

  Serial.println("✅ M5Unified initialized");
  Serial.println("✅ I2C GPIO expanders ready");
  Serial.println("✅ Buttons are now accessible\n");

  // Initialize display
  M5.Display.init();
  M5.Display.setRotation(1);  // Landscape mode
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextDatum(MC_DATUM);
  M5.Display.setTextSize(2);

  // Draw initial UI
  drawUI();

  // Initialize buzzer
  ledcAttach(BEEP_PIN, 2000, 8);
  ledcWrite(BEEP_PIN, 0);

  // Startup beep
  playTone(800, 100);

  Serial.println("========================================");
  Serial.println("Ready! Press buttons or touch screen!");
  Serial.println("========================================\n");
  Serial.println("Button A (KEY1) = Left button");
  Serial.println("Button B (KEY2) = Right button");
  Serial.println("Touch screen = Touch anywhere\n");
}

void loop() {
  // Update button states (reads I2C expander)
  M5.update();

  // Check Button A (KEY1)
  if (M5.BtnA.wasPressed()) {
    button1Count++;
    Serial.println("╔════════════════════════════════════╗");
    Serial.println("║  *** BUTTON A PRESSED ***         ║");
    Serial.println("╚════════════════════════════════════╝");
    Serial.print("Press count: ");
    Serial.println(button1Count);
    Serial.println();

    playTone(1000, 50);
    updateDisplay();
  }

  if (M5.BtnA.wasReleased()) {
    Serial.println(">>> Button A released");
    Serial.println();
  }

  if (M5.BtnA.isPressed()) {
    // Held down - optional feedback
  }

  // Check Button B (KEY2)
  if (M5.BtnB.wasPressed()) {
    button2Count++;
    Serial.println("╔════════════════════════════════════╗");
    Serial.println("║  *** BUTTON B PRESSED ***         ║");
    Serial.println("╚════════════════════════════════════╝");
    Serial.print("Press count: ");
    Serial.println(button2Count);
    Serial.println();

    playTone(1200, 50);
    updateDisplay();
  }

  if (M5.BtnB.wasReleased()) {
    Serial.println(">>> Button B released");
    Serial.println();
  }

  // Both buttons pressed together
  static bool bothShown = false;  // Declare once, outside the if/else blocks
  if (M5.BtnA.isPressed() && M5.BtnB.isPressed()) {
    if (!bothShown) {
      Serial.println("╔════════════════════════════════════╗");
      Serial.println("║  *** BOTH BUTTONS PRESSED! ***    ║");
      Serial.println("╚════════════════════════════════════╝");
      Serial.println();

      playTone(1500, 100);
      bothShown = true;

      // Reset all counters
      button1Count = 0;
      button2Count = 0;
      touchCount = 0;
      updateDisplay();
    }
  } else {
    bothShown = false;  // Now this correctly resets the single variable
  }

  // Check for touch screen
  static bool wasTouched = false;
  lgfx::touch_point_t tp;

  if (M5.Display.getTouch(&tp)) {
    if (!wasTouched) {
      touchCount++;
      Serial.println("╔════════════════════════════════════╗");
      Serial.println("║  *** SCREEN TOUCHED! ***          ║");
      Serial.println("╚════════════════════════════════════╝");
      Serial.print("Touch position: X=");
      Serial.print(tp.x);
      Serial.print(", Y=");
      Serial.println(tp.y);
      Serial.print("Touch count: ");
      Serial.println(touchCount);
      Serial.println();

      playTone(1400, 50);
      updateDisplay();
      wasTouched = true;
    }
  } else {
    wasTouched = false;
  }

  // Heartbeat every 5 seconds
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 5000) {
    Serial.println(">>> Waiting for button press...");
    lastHeartbeat = millis();
  }

  delay(10);
}

void drawUI() {
  M5.Display.fillScreen(TFT_BLACK);

  // Title
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(TFT_CYAN);
  M5.Display.drawString("BUTTON & TOUCH", 120, 8);

  // Labels (increased from size 1 to 2)
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(TFT_YELLOW);
  M5.Display.drawString("BTN A", 40, 32);
  M5.Display.drawString("BTN B", 120, 32);
  M5.Display.drawString("TOUCH", 200, 32);

  // Divider line
  M5.Display.drawFastHLine(0, 55, 240, TFT_WHITE);

  updateDisplay();
}

void updateDisplay() {
  // Clear count area
  M5.Display.fillRect(0, 60, 240, 75, TFT_BLACK);

  // Button A count
  M5.Display.setTextSize(3);
  M5.Display.setTextColor(TFT_GREEN);
  M5.Display.drawString(String(button1Count), 40, 75);

  // Button B count
  M5.Display.setTextColor(TFT_MAGENTA);
  M5.Display.drawString(String(button2Count), 120, 75);

  // Touch count
  M5.Display.setTextColor(TFT_CYAN);
  M5.Display.drawString(String(touchCount), 200, 75);

  // Instructions (increased from size 1 to 2)
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.drawString("Press both to reset", 120, 108);
}

void playTone(int frequency, int duration) {
  ledcChangeFrequency(BEEP_PIN, frequency, 8);
  ledcWrite(BEEP_PIN, 128);  // 50% duty cycle
  delay(duration);
  ledcWrite(BEEP_PIN, 0);
}
