
```markdown
# Design Document — Lamport Authentication Project

This document explains the system design, architecture, and main components of the Lamport Authentication demo.

---

## 1. Protocol Overview
We implement **Lamport’s one-way hash chain authentication protocol**:

- **Setup**:  
  - Bob generates a chain of length *n* = 100 from random seed h0.  
  - Alice stores hn, Bob stores h0.  
  - (In this demo, h0 and hn are pre-generated and stored in `bob_h0.txt` and `alice_hn.txt`.)

- **Challenge–Response**:  
  - Alice issues challenge *c*.  
  - Bob responds with r = H^(n−c)(h0).  
  - Alice verifies by checking `Hash(r) == storedHash`, then updates storedHash = r.  
  - Repeated up to *n-1* times.

---

## 2. System Architecture
The project has four main modules:

### a) **Crypto Layer**
- File: `src/crypto/LamportChain.*`
- Implements:
  - `HashRepeat(value, times)` = apply SHA-256 multiple times.
  - `VerifyResponse(r, storedHash)` = check if `Hash(r) == storedHash`.

### b) **Configuration Layer**
- File: `src/util/ConfigManager.*`
- Loads `config_alice.json` or `config_bob.json`.
- Provides local/peer IPs, ports, role (Alice/Bob), sleep duration, and hash file paths.

### c) **Networking Layer**
- File: `src/network/NetworkManager.*`
- Wrapper around Qt `QTcpServer` and `QTcpSocket`.
- Provides:
  - `startServer(ip, port)`
  - `connectToPeer(ip, port)`
  - `sendLine(message)`
- Emits signals for `messageReceived`, `connected`, `disconnected`, `errorOccurred`.

### d) **Controller**
- File: `src/app/Controller.*`
- Orchestrates Lamport protocol:
  - Alice: send challenge, await response, verify, update hash.
  - Bob: await challenge, compute response, send.
- Emits signals: `logMessage`, `statusChanged`, `roundChanged`.

### e) **GUI**
- File: `src/gui/MainWindow.*`, `mainwindow.ui`
- Provides buttons and log display.
- Connects to Controller signals for status/log updates.
- Shows colored logs and round counter.

---

## 3. Message Format
Simple line-based protocol (terminated by `\n`):

- `CHALLENGE:<c>` — from Alice to Bob  
- `RESPONSE:<r>` — from Bob to Alice  
- Control messages: `STOP`, `ERROR:<msg>`

---

## 4. Workflow
1. **Connect**:
   - Bob starts a server and listens.
   - Alice connects to Bob.

2. **Start** (Alice):
   - Alice sends `CHALLENGE:1`.  
   - Bob replies `RESPONSE:<r1>`.  
   - Alice verifies and updates hash.

3. **Repeat**:
   - Alice increments counter and repeats until *n−1* rounds.

4. **Stop / Finish**:
   - Either party can stop early.
   - Process ends after *n−1* successful rounds.

---

## 5. GUI Design
- **Buttons**:
  - **Connect**: loads config and either starts server (Bob) or connects (Alice).
  - **Start**: starts challenge-response (only Alice, only enabled when connected).
  - **Stop**: halts protocol early.
  - **Disconnect**: closes connection and resets UI.
- **Log Window**:
  - Shows all protocol messages with color coding.
- **Status Label**:
  - Displays current state (Idle, Connected, Running, Finished).
- **Round Counter**:
  - Shows current round vs total.

---

## 6. Design Choices
- **Qt6**: For GUI + networking in a single framework.  
- **Crypto++**: For secure SHA-256 hashing.  
- **Config JSON**: Simplifies parameter changes without recompilation.  
- **Line-based messages**: Human-readable, easy to debug.  
- **Separation of concerns**: Crypto, config, network, controller, and GUI are cleanly separated.


