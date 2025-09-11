# Lamport Authentication Demo (Qt + Crypto++)

This project demonstrates **Lamport’s one-way hash chain based authentication protocol** using a desktop application built in **C++17**, **Qt6 (Widgets + Networking)**, and **Crypto++**.

Two instances of the app run as **Alice** and **Bob**, communicating over TCP/IP using challenge–response.  
The GUI provides **Connect, Start, Stop, Disconnect** buttons, and a log window to display the protocol in action.


## ⚙️ Prerequisites

### macOS

* Ensure [Homebrew](https://brew.sh/) is installed.
* Install required dependencies:

```bash
brew install cmake qt cryptopp
```

### Linux (Debian/Ubuntu-based)

* Update package list and install required packages:

```bash
sudo apt update
sudo apt install clang cmake qtbase5-dev libcrypto++-dev libqt6core6 libqt6gui6 libqt6widgets6 qt6-base-dev build-essential
```



## 🔨 Build Instructions

From the project root:

### macOS

```bash
rm -rf build
mkdir build
cd build

cmake -DCMAKE_CXX_COMPILER=clang++ -DQt6_DIR=$(brew --prefix)/opt/qt/lib/cmake/Qt6 ..
cmake --build . -- -j$(sysctl -n hw.ncpu)
```

### Linux

```bash
rm -rf build
mkdir build
cd build

cmake -DCMAKE_CXX_COMPILER=clang++ -DQt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6 ..
cmake --build . -- -j$(nproc)
```

The app will be built at:

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

### Swarup Narkhede (IIT2023168)
### Atharva Narkhede (IIT2023153)
### Arpit Anand (IIT2023170)
### Snehal Gupta (IIT2023169)
### Darshan Nagre (IIT2023206)







