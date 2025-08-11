#include <Wire.h>

// FDC1004 I2C Address
#define FDC1004_ADDRESS 0x50

// Register Addresses
#define MEAS1_MSB       0x00
#define MEAS1_LSB       0x01
#define MEAS2_MSB       0x02
#define MEAS2_LSB       0x03
#define CONF_MEAS1      0x08
#define CONF_MEAS2      0x09
#define FDC_CONF        0x0C
#define MANUFACTURER_ID 0xFE
#define DEVICE_ID       0xFF

// Configuration values
#define RATE_100SPS     0x0400  // 100 samples per second
#define RATE_200SPS     0x0800  // 200 samples per second
#define RATE_400SPS     0x0C00  // 400 samples per second

// Channel assignments for OoP technique
#define CHA_CIN1        0x0000  // CIN1 as positive channel
#define CHA_CIN2        0x2000  // CIN2 as positive channel
#define CHB_CIN4        0x1800  // CIN4 as negative channel (floating)

// Global variables for calibration
float levelOffset = 0.0;
float refOffset = 0.0;
bool isCalibrated = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Serial.println("FDC1004 Out-of-Phase Liquid Level Demo");
  Serial.println("=====================================");
  
  // Initialize FDC1004
  if (initializeFDC1004()) {
    Serial.println("FDC1004 initialized successfully!");
    
    // Configure for OoP measurements
    configureOoPMeasurements();
    
    // Perform initial calibration
    Serial.println("Performing initial calibration...");
    calibrateSystem();
    
    Serial.println("System ready for measurements");
    Serial.println("Place container and liquid for testing");
  } else {
    Serial.println("Failed to initialize FDC1004!");
    while(1); // Stop execution
  }
}

void loop() {
  // Perform OoP measurements
  float levelCap = performOoPMeasurement(1);  // LEVEL electrode
  float refCap = performOoPMeasurement(2);    // REFERENCE electrode
  
  // Calculate liquid level using OoP technique
  float liquidLevel = calculateLiquidLevel(levelCap, refCap);
  
  // Display results
  Serial.print("LEVEL Cap: ");
  Serial.print(levelCap, 3);
  Serial.print(" pF, REF Cap: ");
  Serial.print(refCap, 3);
  Serial.print(" pF, Liquid Level: ");
  Serial.print(liquidLevel, 2);
  Serial.println(" cm");
  
  delay(500); // Measurement interval
}

bool initializeFDC1004() {
  // Check device ID
  uint16_t deviceID = readRegister16(DEVICE_ID);
  uint16_t manufacturerID = readRegister16(MANUFACTURER_ID);
  
  Serial.print("Device ID: 0x");
  Serial.print(deviceID, HEX);
  Serial.print(", Manufacturer ID: 0x");
  Serial.println(manufacturerID, HEX);
  
  // Verify it's the correct device
  if (deviceID == 0x1004 && manufacturerID == 0x5449) {
    return true;
  }
  return false;
}

void configureOoPMeasurements() {
  /*
   * Out-of-Phase Configuration:
   * - MEAS1: CIN1 (LEVEL) - CIN4 (floating)
   * - MEAS2: CIN2 (REF) - CIN4 (floating)
   * - CIN4 is left floating for differential mode
   * - SHLD1 paired with CIN1/CIN2, SHLD2 paired with CIN4
   */
  
  // Configure MEAS1: CIN1 - CIN4 (LEVEL electrode)
  uint16_t meas1Config = CHA_CIN1 | CHB_CIN4; // CIN1 as CHA, CIN4 as CHB
  writeRegister16(CONF_MEAS1, meas1Config);
  
  // Configure MEAS2: CIN2 - CIN4 (REFERENCE electrode)  
  uint16_t meas2Config = CHA_CIN2 | CHB_CIN4; // CIN2 as CHA, CIN4 as CHB
  writeRegister16(CONF_MEAS2, meas2Config);
  
  Serial.println("OoP measurements configured:");
  Serial.println("- MEAS1: CIN1 - CIN4 (LEVEL)");
  Serial.println("- MEAS2: CIN2 - CIN4 (REFERENCE)");
}

float performOoPMeasurement(uint8_t measurement) {
  // Configure measurement rate and trigger
  uint16_t fdcConfig = RATE_100SPS; // 100 SPS for better resolution
  
  if (measurement == 1) {
    fdcConfig |= 0x0080; // Enable MEAS1
  } else if (measurement == 2) {
    fdcConfig |= 0x0040; // Enable MEAS2
  }
  
  // Trigger measurement
  writeRegister16(FDC_CONF, fdcConfig);
  
  // Wait for measurement completion
  delay(15); // At 100 SPS, each measurement takes ~10ms
  
  // Check if measurement is done
  uint16_t status = readRegister16(FDC_CONF);
  uint8_t doneBit = (measurement == 1) ? 0x08 : 0x04;
  
  int timeout = 100;
  while (!(status & doneBit) && timeout > 0) {
    delay(1);
    status = readRegister16(FDC_CONF);
    timeout--;
  }
  
  if (timeout == 0) {
    Serial.println("Measurement timeout!");
    return 0.0;
  }
  
  // Read measurement result
  uint8_t msbReg = (measurement == 1) ? MEAS1_MSB : MEAS2_MSB;
  uint8_t lsbReg = (measurement == 1) ? MEAS1_LSB : MEAS2_LSB;
  
  uint16_t msb = readRegister16(msbReg);
  uint16_t lsb = readRegister16(lsbReg);
  
  // Combine MSB and LSB to form 24-bit result
  int32_t rawData = ((int32_t)msb << 8) | (lsb >> 8);
  
  // Convert to signed 24-bit
  if (rawData & 0x800000) {
    rawData |= 0xFF000000; // Sign extend
  }
  
  // Convert to capacitance in pF
  // Formula: Capacitance (pF) = rawData / 2^19
  float capacitance = (float)rawData / 524288.0; // 2^19 = 524288
  
  return capacitance;
}

void calibrateSystem() {
  // Perform calibration with empty container
  Serial.println("Calibrating with empty container...");
  
  float levelSum = 0.0;
  float refSum = 0.0;
  int samples = 10;
  
  for (int i = 0; i < samples; i++) {
    levelSum += performOoPMeasurement(1);
    refSum += performOoPMeasurement(2);
    delay(100);
  }
  
  levelOffset = levelSum / samples;
  refOffset = refSum / samples;
  isCalibrated = true;
  
  Serial.print("Calibration complete - Level offset: ");
  Serial.print(levelOffset, 3);
  Serial.print(" pF, Ref offset: ");
  Serial.print(refOffset, 3);
  Serial.println(" pF");
}

float calculateLiquidLevel(float levelCap, float refCap) {
  if (!isCalibrated) {
    return 0.0;
  }
  
  // Apply calibration offsets
  float correctedLevel = levelCap - levelOffset;
  float correctedRef = refCap - refOffset;
  
  // OoP liquid level calculation
  // This is a simplified calculation - adjust based on your sensor geometry
  float heightPerPF = 2.0; // cm per pF - calibrate this for your setup
  float liquidLevel = correctedLevel * heightPerPF;
  
  // Ensure non-negative result
  if (liquidLevel < 0) {
    liquidLevel = 0;
  }
  
  return liquidLevel;
}

void writeRegister16(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(FDC1004_ADDRESS);
  Wire.write(reg);
  Wire.write((value >> 8) & 0xFF); // MSB
  Wire.write(value & 0xFF);        // LSB
  Wire.endTransmission();
}

uint16_t readRegister16(uint8_t reg) {
  Wire.beginTransmission(FDC1004_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(FDC1004_ADDRESS, 2);
  
  if (Wire.available() >= 2) {
    uint16_t msb = Wire.read();
    uint16_t lsb = Wire.read();
    return (msb << 8) | lsb;
  }
  
  return 0;
}