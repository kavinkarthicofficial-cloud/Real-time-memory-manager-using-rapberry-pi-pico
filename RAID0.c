#include <SPI.h>
#include <SD.h>

#define DISK_SIZE 200
#define DISK1_FILE "disk1.bin"
#define DISK2_FILE "disk2.bin"
#define CS_PIN 4

File disk1, disk2;

void clearSerialBuffer() {
  while (Serial.available()) Serial.read();
}

String readSerialString() {
  String input = "";
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') return input;
      input += c;
    }
  }
}

void raid0_striping() {
  String data;
  char read_buf[DISK_SIZE];

  Serial.print("Enter data to write (max ");
  Serial.print(DISK_SIZE);
  Serial.println(" characters):");
  data = readSerialString();
  Serial.println(data);

  // Write data with striping
  disk1 = SD.open(DISK1_FILE, FILE_WRITE);
  disk2 = SD.open(DISK2_FILE, FILE_WRITE);
  if (!disk1 || !disk2) {
    Serial.println("Error opening disks!");
    return;
  }

  for (int i = 0; i < data.length(); i++) {
    if (i % 2 == 0) disk1.print(data[i]);
    else disk2.print(data[i]);
  }

  disk1.close();
  disk2.close();

  Serial.println("Data written using RAID 0 striping.");

  // Read data back
  disk1 = SD.open(DISK1_FILE, FILE_READ);
  disk2 = SD.open(DISK2_FILE, FILE_READ);
  if (!disk1 || !disk2) {
    Serial.println("Error reading disks!");
    return;
  }

  String reconstructed = "";
  int ch1, ch2;
  while (disk1.available() || disk2.available()) {
    if (disk1.available()) {
      ch1 = disk1.read();
      reconstructed += (char)ch1;
    }
    if (disk2.available()) {
      ch2 = disk2.read();
      reconstructed += (char)ch2;
    }
  }

  disk1.close();
  disk2.close();

  Serial.print("Reconstructed data: ");
  Serial.println(reconstructed);

  // Simulate failure
  Serial.println("\nSimulating disk2 failure...");
  Serial.println("Trying to reconstruct with only disk1...");

  disk1 = SD.open(DISK1_FILE, FILE_READ);
  if (!disk1) {
    Serial.println("Disk1 failed too. All data lost.");
    return;
  }

  Serial.print("Partial data from disk1: ");
  while (disk1.available()) {
    Serial.print((char)disk1.read());
    Serial.print("_"); // Missing disk2 pieces
  }
  disk1.close();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n--- RAID 0: Striping ---");

  if (!SD.begin(CS_PIN)) {
    Serial.println("SD initialization failed!");
    return;
  }

  if (!SD.exists(DISK1_FILE)) {
    disk1 = SD.open(DISK1_FILE, FILE_WRITE);
    disk1.close();
  }
  if (!SD.exists(DISK2_FILE)) {
    disk2 = SD.open(DISK2_FILE, FILE_WRITE);
    disk2.close();
  }

  raid0_striping();
}

void loop() {
  // Nothing to do
}
