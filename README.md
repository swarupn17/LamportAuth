# LamportAuth
This is the Cybersecurity Course Project based on Lamport's Authentication.


# Lamport Authentication Demo (Qt + Crypto++)

This project demonstrates **Lamport’s one-way hash chain based authentication protocol** using a desktop application built in **C++17**, **Qt6 (Widgets + Networking)**, and **Crypto++**.

Two instances of the app run as **Alice** and **Bob**, communicating over TCP/IP using challenge–response.  
The GUI provides **Connect, Start, Stop, Disconnect** buttons, and a log window to display the protocol in action.


---

## ⚙️ Prerequisites
- macOS or Linux  
- [Homebrew](https://brew.sh/) (for dependencies)  
- Install dependencies:
  ```bash
  brew install cmake qt cryptopp
````

---

## 🔨 Build Instructions

From the project root:

```bash
rm -rf build
mkdir build
cd build

cmake -DCMAKE_CXX_COMPILER=clang++ -DQt6_DIR=$(brew --prefix)/opt/qt/lib/cmake/Qt6 ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
```

The app will be built as:

```
build/lamport_app
```

---

## ▶️ Running the Demo (same machine)

Run **two instances** of the app — one for Alice, one for Bob.

### 1. Start Bob

```bash
cd lamport-auth
./build/lamport_app
```

* In the GUI, click **Connect**.
* Select `config_bob.json`.
* Bob starts a server and listens on `127.0.0.1:5001`.

### 2. Start Alice

```bash
cd lamport-auth
./build/lamport_app
```

* In the GUI, click **Connect**.
* Select `config_alice.json`.
* Alice connects to Bob on `127.0.0.1:5001`.

### 3. Start the Protocol

* In Alice’s window, click **Start**.
* Alice begins sending challenges, Bob responds, and logs show each round.

---

## 🌐 Running Across Two Machines

1. Edit `config_alice.json` and `config_bob.json`:

   * Set `local_ip` to each machine’s LAN IP.
   * Set `peer_ip` to the other machine’s IP.
   * Use complementary ports (e.g., Alice local\_port=5000, peer\_port=5001; Bob local\_port=5001, peer\_port=5000).

2. Start Bob first (server).

3. Start Alice (client).

---

## 📝 Example Logs

**Alice (client):**

```
[2025-09-07T12:45:12] Loaded Alice hn: dc35aadb...
[2025-09-07T12:45:12] Attempting to connect as client...
[2025-09-07T12:45:12] Network connected
[2025-09-07T12:45:13] Sent CHALLENGE: 1
[2025-09-07T12:45:13] Received RESPONSE: dc35aadb...
[2025-09-07T12:45:13] Verify OK for challenge 1; updated storedHash
```

**Bob (server):**

```
[2025-09-07T12:45:10] Loaded Bob h0: random_seed_123...
[2025-09-07T12:45:10] Server started and listening (role=Bob)
[2025-09-07T12:45:12] Network connected
[2025-09-07T12:45:13] Received CHALLENGE: 1
[2025-09-07T12:45:13] Sent RESPONSE for challenge 1
```

---

## 📸 Documentation

For final submission, include:

* Screenshots of both GUIs (Alice + Bob during run).
* This `README.md`.
* Short explanation of protocol flow (Controller ↔ NetworkManager ↔ LamportChain).

---

## 👩‍💻 Authors

* *Add group members’ names here*

```

```
