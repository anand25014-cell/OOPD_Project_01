# 📡 Cellular Network Simulator (OOPD Project)
- Authors: arjun25064 , anand25014-cell
- Course: Object Oriented Programming & Design
- Technologies Simulated: 2G, 3G, 4G, 5G
- Features: Templates, Exception Handling, Custom Vector, CSV Reader, Multithreading, MIMO, Protocol Polymorphism

### 📘 1. Introduction
Modern cellular systems consist of:
- User Devices (UE)
- Cell Towers
- Cellular Cores
- Communication Protocols (2G–5G)

This project simulates a miniature real-world cellular network using:
- OOP concepts → inheritance, abstraction, polymorphism
- Templates → GenericVector
- Exceptions → custom VectorException
- Threading → core workers
- Custom I/O → using syscall.S and basicIO (no std I/O)
- Custom CSV parser
- No standard C/C++ libraries except threading and atomics


The simulator loads a CSV of user requests, assigns users to channels, computes bandwidth occupancy, runs message-processing threads, and produces a full usage summary.

### 📚 2. Folder Structure
```
OOPD-Network-Simulator-Project/
│
├── include/
│   ├── CellTower.h
│   ├── CellularCore.h
│   ├── Protocol.h
│   ├── UserDevice.h
│   ├── GenericVector.h
│   ├── CsvReader.h
│   └── basicIO.h
│
├── src/
│   ├── CellTower.cpp
│   ├── CellularCore.cpp
│   ├── Protocol.cpp
│   ├── UserDevice.cpp
│   ├── CsvReader.cpp
│   ├── basicIO.cpp
│   └── main.cpp
│
├── syscall.S
├── Makefile
├── bin/
├── build/
├── 2G.csv
├── 3G.csv
├── 4G.csv
├── 5G.csv
└── README.md   (this file)

```

## 🧩 3. High-Level Architecture
This simulator contains four major classes, matching the assignment specification.

#### 🟦 3.1 Protocol (Base + Derived Classes)
Each technology (2G, 3G, 4G, 5G) is modelled as a derived class of Protocol.
Every protocol defines:
```cpp
messages_per_user
users_per_channel
bandwidth_per_channel
number of channels
MIMO antennas supported
protocol name string
```

This enables polymorphism.

#### 🟩 3.2 UserDevice
Each device has:

```cpp
id
protocol
requested_channel
messages_per_connection
assigned_channel_index
assigned_antenna_index
```

Each line in CSV creates a ```UserDevice```.

#### 🟥 3.3 CellTower
Each tower is built from a protocol and:
- allocates users to channels
- performs MIMO allocation
- drops extra users beyond channel capacity
- reports assignment summary
- calculates total messages to be processed

Finally, each ```CellTower``` sends a ```MessageBatch``` to the ```Cellular Core```.

#### 🟪 3.4 CellularCore
The heart of the simulation.

- Has a maximum capacity (e.g., 10,000 msgs)
- Has overhead percentage (e.g., 20%)
- Uses worker threads to process messages
- Applies overhead:
```cpp 
effective = msgs × (1 + overhead/100)
```
- Tracks accepted vs rejected messages
- Calculates theoretical number of cores:
```cpp
cores = ceil( total_messages / (capacity - overhead_effect) )
```

Uses:
```cpp 
std::thread
std::atomic<int>
std::mutex
std::condition_variable
```


#### 📄 4. CSV Input Format
Every CSV file has:

```cpp
user_id,requested_channel
1,0
2,0
3,2
...
```

Channels start from 0.
A separate CSV exists for:

```cpp 
2G.csv
3G.csv
4G.csv
5G.csv
```

These represent users attempting to access respective technologies.

### 📡 5. How Each Technology Is Simulated

#### 5.1 2G Simulation
Specs from Q2:
- 5 channels (1 MHz / 200 kHz)
- 16 users per channel
- 5 messages/data + 15 messages/voice = 20 messages per user
- No MIMO


How code simulates it:
- Protocol2G defines above parameters.
- CellTower allows 16 users per channel.
- Others get dropped.
- Total messages = assigned_users × 20.



#### 5.2 3G Simulation
Specs from Q3:

- CDMA → 32 users per 200 kHz channel
- Fixed 10 messages/user

Simulation:

- 32 users allowed per channel
- No MIMO
- Total messages = users × 10



#### 5.3 4G Simulation
Specs from Q4:

- 10 kHz per channel
- 30 users per 10 kHz
- 10 messages per user
- Up to 4 antennas parallel → MIMO = 4


Simulation:
- Channel reused 4× due to MIMO
- Maximum users allowed = users_per_channel × antennas
- Total messages = assigned_users × 10



#### 5.4 5G Simulation
Specs from Q5:

- 10 MHz extra spectrum
- 30 users per 1 MHz
- Massive MIMO: 16 antennas
- Extremely high channel reuse


Simulation:

- 16× channel reuse → huge capacity
- Total messages = users × 10 messages



### ⚙️ 6. Building the Project
You must have:

```cpp
g++
make

```


#### ⭐ Build optimized simulator
```cpp 
make
```

Creates:

```cpp 
bin/simulator
```


#### ⭐ Build debug version
```cpp 
make debug
```

Creates:
```cpp
bin/simulator_debug
```

Debug version includes huge logging output ([DEBUG] ...).

#### ⭐ Clean everything
```cpp
make clean
```

### ▶️ 7. Running the Simulator
Generic Command:
```cpp 
cat <csvfile> | ./bin/simulator <2G|3G|4G|5G>
```

Example:
```cpp 
cat 2G.csv | ./bin/simulator 2G
```

This prints:
- Channel-wise assigned and dropped users
- First channel users
- Total messages
- Theoretical cores required
- Core accepted/rejected messages



### 🐞 8. Running the Debug Version
```cpp 
cat 2G.csv | ./bin/simulator_debug 2G
```

Outputs everything above plus debugging logs, e.g.
```cpp
[DEBUG] CsvReader: parsed -> ID=1 CH=0
[DEBUG] assign_user: assigned ID=1 ch=0 ant=0
[DEBUG] send_batch_to_core: tower=1 msgs=80
[DEBUG] worker_thread: processing tower=1 msgs=80
```

This helps diagnose:
- thread behavior
- channel assignment
- dropped users
- message batching
- MIMO antenna usage



### 📐 9. Important Formulas
#### 📌 Raw Capacity
```cpp
raw_capacity = channels * users_per_channel * antennas
```

#### 📌 Total messages generated
```cpp 
total_msgs = assigned_users * messages_per_user
```

#### 📌 Effective messages due to overhead
```cpp
effective_msgs = total_msgs * (100 + overhead) / 100
```

#### 📌 Theoretical number of cores
```cpp
cores = ceil( total_msgs / (core_capacity - overhead_effect) )
```


## 🧪 10. Example Output
```cpp
=== CellTower Summary ===
Protocol: 2G
Channels: 5
Raw Capacity: 40
Assigned users total: 16
Channel 0: Assigned=6 Dropped=0
Channel 1: Assigned=3 Dropped=0
Channel 2: Assigned=3 Dropped=0
Channel 3: Assigned=2 Dropped=0
Channel 4: Assigned=2 Dropped=0
First channel users: 1 2 9 14 15 16
Dropped users total: 0
Total Msgs: 80
Cores Needed (theoretical): 1
```
