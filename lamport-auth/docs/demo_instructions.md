
````markdown
# Demo Instructions — Lamport Authentication Project

This document explains how to run the Lamport Authentication demo for both **Alice** and **Bob** roles.  
Two instances of the desktop application must be started — either on the **same machine** or on **two machines** connected via LAN.

---

## 1. Build the Application
From the project root:

```bash
rm -rf build
mkdir build
cd build

cmake -DCMAKE_CXX_COMPILER=clang++ -DQt6_DIR=$(brew --prefix)/opt/qt/lib/cmake/Qt6 ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
````

The executable will be created at lamport-auth/build:

```
(run below command from build directory)
    
./lamport_app
```

---

## 2. Local (same machine) Demo

### Step 1: Start Bob (server)

* Open a terminal:

  ```bash
  cd lamport-auth
  ./build/lamport_app
  ```
* In the GUI:

  * Click **Connect**.
  * Select `config_bob.json`.
* Log should show:

  ```
  Loaded Bob h0: ...
  Server started and listening (role=Bob)
  Status: Listening
  ```

### Step 2: Start Alice (client)

* Open another terminal:

  ```bash
  cd lamport-auth
  ./build/lamport_app
  ```
* In the GUI:

  * Click **Connect**.
  * Select `config_alice.json`.
* Log should show:

  ```
  Loaded Alice hn: ...
  Attempting to connect as client...
  Network connected
  Status: Connected
  ```

### Step 3: Start the protocol

* In Alice’s window, click **Start**.
* Alice begins sending challenges, Bob responds.

### Step 4: Observe

* **Alice log**:
  `Sent CHALLENGE: 1`
  `Received RESPONSE: ...`
  `Verify OK...`

* **Bob log**:
  `Received CHALLENGE: 1`
  `Sent RESPONSE for challenge 1`

---

## 3. Cross-Machine (LAN) Demo

1. Update configs:

   * `config_alice.json`:

     * `local_ip` = Alice’s LAN IP
     * `peer_ip` = Bob’s LAN IP
   * `config_bob.json`:

     * `local_ip` = Bob’s LAN IP
     * `peer_ip` = Alice’s LAN IP
   * Ports must be complementary (e.g., Alice local=5000, peer=5001; Bob local=5001, peer=5000).

2. Run Bob first on his machine, then Alice on hers.

---

## 4. Stopping & Disconnecting

* **Stop**: halts the challenge–response process early.
* **Disconnect**: closes the TCP connection and resets UI buttons.

---

## 5. Logs and Rounds

* The log window shows colored messages:

  * ✅ Green = successful challenge/response (PASS)
  * ❌ Red = errors or verification failures
  * ⚫ Black = informational messages
* The **status label** updates in real time (Idle, Connected, Running, Finished).
* The **round counter** shows progress (e.g., `Round: 3 / 99`).

---

## 6. Saving Evidence

For submission, capture:

* One screenshot of Alice’s window during protocol.
* One screenshot of Bob’s window during protocol.
* Save log contents (copy-paste from GUI or use Save button if available).


