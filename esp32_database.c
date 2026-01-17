#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <unordered_map>
#include <vector>

// --- Config ---
#define CS_PIN 5
const char* DB_FILENAME = "/my_database.dat";

// --- Data Structures (Same as your core.cpp) ---
struct UserRecord {
  int id;
  char username[32]; // Fixed size is crucial for binary storage
  int age;
  bool active;
};

// --- The Database Class ---
class SimpleDB {
  private:
    const char* filename;
    std::unordered_map<int, unsigned long> index; // Maps ID -> Byte Position

  public:
    SimpleDB(const char* fname) {
      filename = fname;
    }

    void init() {
      // If file doesn't exist, create it
      if (!SD.exists(filename)) {
        Serial.println("[System] Creating new database file...");
        File f = SD.open(filename, FILE_WRITE);
        if (f) f.close();
      }
      rebuildIndex();
    }

    void rebuildIndex() {
      Serial.println("[System] Rebuilding Index from SD Card...");
      index.clear();

      File file = SD.open(filename, FILE_READ);
      if (!file) {
        Serial.println("❌ Error opening file for indexing");
        return;
      }

      UserRecord temp;
      unsigned long current_position = 0;
      
      // Read specifically the size of the struct
      while (file.read((uint8_t*)&temp, sizeof(UserRecord))) {
        if (temp.active) {
          index[temp.id] = current_position;
        }
        current_position = file.position(); // Update pos for next loop
      }

      file.close();
      Serial.print("[System] Index complete. Records found: ");
      Serial.println(index.size());
    }

    void insertUser(int id, String name, int age) {
      // Prepare the Record
      UserRecord temp;
      temp.id = id;
      strncpy(temp.username, name.c_str(), 31);
      temp.username[31] = '\0'; // Ensure null termination
      temp.age = age;
      temp.active = true;

      // Open, Write, Close (Safest for SD cards)
      File file = SD.open(filename, FILE_APPEND);
      if (!file) {
        Serial.println("❌ Failed to open file for writing");
        return;
      }

      // Get the position BEFORE writing (this is our index)
      unsigned long writePosition = file.position();
      
      // Write the binary struct
      file.write((uint8_t*)&temp, sizeof(UserRecord));
      file.close();

      // Update Index in RAM
      index[id] = writePosition;
      
      Serial.print("✅ Saved User: ");
      Serial.print(name);
      Serial.print(" at offset ");
      Serial.println(writePosition);
    }

    void getUser(int id) {
      if (index.find(id) == index.end()) {
        Serial.print("❌ User ID "); Serial.print(id); Serial.println(" not found.");
        return;
      }

      unsigned long position = index[id];
      
      File file = SD.open(filename, FILE_READ);
      if (!file) return;

      // Jump straight to the data (O(1) Access)
      file.seek(position);
      
      UserRecord temp;
      file.read((uint8_t*)&temp, sizeof(UserRecord));
      file.close();

      Serial.println("\n--- Record Found ---");
      Serial.print("ID:   "); Serial.println(temp.id);
      Serial.print("Name: "); Serial.println(temp.username);
      Serial.print("Age:  "); Serial.println(temp.age);
      Serial.println("--------------------\n");
    }

    void listAllUsers() {
      File file = SD.open(filename, FILE_READ);
      if (!file) return;

      UserRecord temp;
      Serial.println("\nID | Name | Age");
      Serial.println("----------------");
      
      while (file.read((uint8_t*)&temp, sizeof(UserRecord))) {
        if (temp.active) {
          Serial.print(temp.id);
          Serial.print(" | ");
          Serial.print(temp.username);
          Serial.print(" | ");
          Serial.println(temp.age);
        }
      }
      file.close();
      Serial.println("----------------\n");
    }
};

// --- Global Object ---
SimpleDB db(DB_FILENAME);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\nInitializing SD Card...");
  if (!SD.begin(CS_PIN)) {
    Serial.println("❌ SD Card Mount Failed");
    return;
  }
  
  // Start the Database
  db.init();

  Serial.println("\n--- KADatabase ESP32 Shell ---");
  Serial.println("Commands:");
  Serial.println("  add [id] [name] [age]  -> e.g. 'add 101 Justin 25'");
  Serial.println("  get [id]               -> e.g. 'get 101'");
  Serial.println("  list                   -> Show all users");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // --- Command Parser ---
    if (input.startsWith("add")) {
      // Basic parsing logic
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      int thirdSpace = input.indexOf(' ', secondSpace + 1);

      if (firstSpace > 0 && secondSpace > 0 && thirdSpace > 0) {
        String idStr = input.substring(firstSpace + 1, secondSpace);
        String name = input.substring(secondSpace + 1, thirdSpace);
        String ageStr = input.substring(thirdSpace + 1);

        db.insertUser(idStr.toInt(), name, ageStr.toInt());
      } else {
        Serial.println("Usage: add [id] [name] [age]");
      }
    } 
    else if (input.startsWith("get")) {
      int space = input.indexOf(' ');
      if (space > 0) {
        String idStr = input.substring(space + 1);
        db.getUser(idStr.toInt());
      }
    }
    else if (input == "list") {
      db.listAllUsers();
    }
    else {
      Serial.println("Unknown command.");
    }
  }
}