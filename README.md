# KADatabase

**KADatabase** is a custom-built, lightweight, **Table-Based Database Engine** written in C++. It features a custom binary file format, persistent storage, and an in-memory indexing system for O(1) read operations.

The project demonstrates low-level system programming concepts including binary serialization, manual memory management (padding/alignment), and custom schema parsing.

## 🚀 Features

-   **Custom Binary Storage:** Data is stored in `.dat` files with a custom 1024-byte header structure containing schema metadata.
-   **SQL-Like Syntax:** Supports `CREATE`, `ADD`, and `GET` commands.
-   **Hybrid Architecture:**
    -   **Persistence:** All data is written to disk immediately using `fstream`.
    -   **Performance:** Data is indexed into RAM (`std::map`) at startup for instant retrieval.
-   **Dynamic Schema:** Supports creating tables with variable column definitions (Integers and fixed-size Strings).
  
## 🔮 Further Goals (Roadmap)

The current implementation is a foundational prototype. Future updates aim to introduce:

1.  **Soft Deletes:** Implementing an `is_deleted` flag in the record schema to mark records as inactive without immediately removing them from disk, allowing for data recovery, and O(1) deletion.
2.  **Vacuum/Cleanup:** A maintenance utility to permanently remove "soft deleted" records and defragment the `.dat` files to reclaim storage space.
3.  **Concurrency (Producer-Consumer):** Decoupling the command parser from the disk I/O using a thread pool and a job queue. This will allow the engine to handle multiple operations asynchronously using 2+ threads.

## 🛠️ Tech Stack

-   **Language:** C++ (Standard Library, FileSystem API)
-   **Storage:** Binary File I/O (`fstream`)
-   **Indexing:** Red-Black Tree (`std::map`)

## ⚙️ Architecture

### File Structure (`.dat`)
Every table is stored as a separate binary file structured as follows:

| Section | Size | Description |
| :--- | :--- | :--- |
| **Header** | 1024 Bytes | Contains the File Signature (`KAD`), Column Definitions, and Record Size. |
| **Body** | Variable | Contiguous blocks of binary data representing rows. |

### Commands

**1. Create Table**
Creates a new binary file with the specified schema.
```bash
create users name:string:32 age:int id:int:PK
```
* `string:32` defines a char array of size 32.
* `PK` marks the Primary Key (Must be an Integer).

**2. Add Record**
Writes a serialized binary record to the end of the file.
```bash
users add Alice 25 1
```

**3. Get Record**
Retrieves a record by its Primary Key from the in-memory index.
```bash
users get 1
```

## 🔧 Installation & Usage

1.  **Clone the Repository**
    ```bash
    git clone https://github.com/YOUR_USERNAME/KADatabase.git
    cd KADatabase
    ```

2.  **Compile**
    Ensure you have a C++ compiler (GCC/Clang/MSVC) that supports C++17.
    ```bash
    g++ -o kadb core.cpp -std=c++17
    ```

3.  **Run**
    ```bash
    ./kadb
    ```

## 📂 Project Structure

-   `core.cpp`: Main entry point and command loop.
-   `parser.cpp`: Tokenizes raw input strings into command structures.
-   `structures.cpp`: Defines the binary file headers and data structures.
-   `tableCreator.cpp`: Handles the creation of binary files and header padding.
-   `operations.cpp`: Handles `WRITE` (Disk) and `READ` (RAM) operations.

## 📝 Constraints

-   The Primary Key (PK) must be an `integer`.
-   The maximum string length must be defined during table creation.

## 🤝 Contributing

Please refer to the Contibuting Guidelines.
