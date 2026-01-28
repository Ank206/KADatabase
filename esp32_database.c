  #include "FS.h"
  #include "SD.h"
  #include "SPI.h"
  #include <unordered_map>
  #include <vector>
  #include <ArduinoJson.h>

  // --- Config ---
  #define CS_PIN 5
  const char* DB_FILENAME = "/chatdatabase.dat";

  // --- Data Structures ---
  struct ChatRecord {
    char message[256];
  };

  // --- The Database Class ---
  class SimpleDB {
    private:
      const char* filename;
      std::unordered_map<int, unsigned long> index; 
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

        ChatRecord temp;
        unsigned long current_position = 0;
        
        // Read specifically the size of the struct
        while (file.read((uint8_t*)&temp, sizeof(ChatRecord))) {
          if (temp.active) {
            index[temp.id] = current_position;
          }
          current_position = file.position(); // Update pos for next loop
        }

        file.close();
        Serial.print("[System] Index complete. Records found: ");
        Serial.println(index.size());
      }
      void insertUser(String chat) {
        ChatRecord temp;
        strncpy(temp.message, chat.c_str(), 255);
        temp.message[255] = '\0'; // Ensure null termination

        // Open, Write, Close (Safest for SD cards)
        File file = SD.open(filename, FILE_APPEND);
        if (!file) {
          Serial.println("❌ Failed to open file for writing");
          return;
        }

        // Get the position BEFORE writing (this is our index)
        unsigned long writePosition = file.position();
        
        // Write the binary struct
        file.write((uint8_t*)&temp, sizeof(ChatRecord));
        file.close();

        // Update Index in RAM
        index[id] = writePosition;
        
        Serial.print("✅ Saved Message: ");
        Serial.print(chat);
      }
      void getLastUsers() {
        File file = SD.open(filename, FILE_READ);
        if (!file) return;
        unsigned long fileSize = file.size();
        size_t recordSize = sizeof(ChatRecord);
        int n = 20; // to read the last 20 records.

        int totalRecords = fileSize/recordSize;

        if(totalRecords < 20)
        {
          n = totalRecords;
        }   

        unsigned long startPos = fileSize - (n * recordSize);
        file.seek(startPos);

        DynamicJsonDocument doc(8192); 
        JsonArray array = doc.to<JsonArray>();

        ChatRecord temp;
        while(file.read((uint8_t*)&temp, recordSize)) {
          array.add(temp.message); // ArduinoJson copies the string automatically
        }
        file.close();

        serializeJson(doc, Serial);
        Serial.println();
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
    // Serial.println("Commands:");
    // Serial.println("  add [id] [name] [age]  -> e.g. 'add 101 Justin 25'");
    // Serial.println("  get [id]               -> e.g. 'get 101'");
    // Serial.println("  list                   -> Show all users");
  }

  void loop() {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();

      // --- Command Parser ---
      if (input.startsWith("add")) {
        // Basic parsing logic
        int firstSpace = input.indexOf(' ');

        if (firstSpace > 0) {
          String chatmess = input.substring(firstSpace + 1);
          db.insertUser(chatmess);
        } else {
          Serial.println("Usage: add [your message]");
        }
      } 
      else if (input == "list") {
        db.getLastUsers();
      }
      else {
        Serial.println("Unknown command.");
      }
    }
  }