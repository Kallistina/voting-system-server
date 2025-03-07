# MultiThreaded Voting System

## Overview
This project implements a multi-threaded voting system consisting of:
- A **multi-threaded network server** (`poller`) that manages incoming votes.
- A **batch client** (`pollSwayer`) to simulate voters submitting votes.
- Various **Bash scripts** to automate vote generation, tallying, and log processing.

The system ensures proper handling of client-server communication, thread synchronization, and data consistency.

---

## Installation & Compilation
### Compile the Project
```bash
make
```

### Run the Multi-threaded Server
```bash
./poller 5634 8 16 pollLog.txt pollStats.txt
```
- `5634`: Server port
- `8`: Number of worker threads
- `16`: Buffer size
- `pollLog.txt`: Log file
- `pollStats.txt`: Output file for election results

### Run the Batch Client
```bash
./pollSwayer linux01.di.uoa.gr 5634 inputFile.txt
```
- Connects to `poller` and submits votes read from `inputFile.txt`

---

## Project Structure
### **Source Files**
#### **Multi-threaded Network Server (poller)**
- **`poller.cpp`**  
  Implements the server logic, including handling incoming connections and processing votes.
- **`headFile.h`**  
  Contains all necessary headers, definitions, function prototypes, and structures.

#### **Batch Client (pollSwayer)**
- **`pollSwayer.cpp`**  
  Simulates multiple clients sending votes to the server.

#### **Bash Scripts for Automation**
- **`create_input.sh`**  
  Generates random voter data for testing.
- **`tallyVotes.sh`**  
  Counts votes and outputs sorted results.
- **`processLogFile.sh`**  
  Processes server logs to extract election results.

---

## Functionality
### **1. Multi-threaded Server (`poller`)**
- Listens for client connections.
- Accepts voter names and votes.
- Checks if a voter has already voted.
- Updates vote counts and logs votes.
- Supports safe termination (`CTRL+C`) while saving results.

### **2. Batch Client (`pollSwayer`)**
- Reads voter data from `inputFile.txt`.
- Creates threads for each voter to send votes to `poller`.
- Handles server responses and detects duplicate votes.

### **3. Bash Scripts**
#### **`create_input.sh`**
- Generates an input file with simulated voter data.
```bash
./create_input.sh politicalParties.txt numLines
```
#### **`tallyVotes.sh`**
- Removes duplicate votes and tallies results.
```bash
./tallyVotes.sh tallyResultsFile
```
#### **`processLogFile.sh`**
- Processes server logs to extract results.
```bash
./processLogFile.sh poll-log
```
> **Note:** The script expects `poll-log` but test cases may use `log.txt`. Modify accordingly.

---

## Notes & Considerations
- Ensure correct permissions for Bash scripts:
  ```bash
  chmod 777 scriptName.sh
  ```
- The server logs election results in `pollStats.txt`.
- The project correctly handles multiple concurrent voters.
- Duplicate votes are rejected.

---

## License
This project is open-source. Feel free to use, modify, and distribute it as needed.

---

Thank you for exploring this project! 🚀
