# OOP-Project

## 🗳️ E-Voting System (GUI-Based)

This project is a **group-developed GUI-based e-voting system** built using **C++ and Raylib**. It offers a complete and secure environment for voters and administrators to perform their respective tasks in a simulated online election process.
### 👥 Group Contribution

> This project was developed collaboratively as part of an academic assignment.
> **Both group members contributed equally** to its design, implementation, debugging, and testing.

### 🎯 Project Highlights

#### 🔐 Voter Features:

* Login with ID, password, and name.
* Cast vote for a candidate.
* View voting instructions.
* Change password securely.
* Restriction: One vote per voter.

#### 🧑‍💼 Admin Features:

* Admin login using credentials.
* Add and remove voters or candidates.
* View voting results (including ties and runner-ups).
* Save and load candidate data to/from file.
* Detect and prevent duplicate candidate entries.

#### 📊 Additional Functionalities:

* Real-time feedback using result messages.
* Data persistence with file handling.
* Interactive GUI using Raylib.
* Input validation and structured class design (OOP).

### 🧱 Technologies Used

* **Language**: C++
* **Library**: [Raylib](https://www.raylib.com/) (for GUI)
* **Concepts**: OOP (Classes, Inheritance, Encapsulation), File I/O, Dynamic Memory

### 🚀 Getting Started

#### 🔧 Requirements:

* Raylib installed and linked with your C++ environment.
* C++17 compatible compiler (e.g., g++, clang++).

#### 🛠️ Build & Run:
g++ -o evoting main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./evoting

> Make sure to include the font files or update the font path in the code.

### 📁 Files Included

* `main.cpp`: Main source file with complete GUI and logic.
* `candidate_info.txt`: Stores candidate data persistently.
* Additional data files will be created at runtime if required.

### 🧑‍🤝‍🧑 Authors

This system was built as a **collaborative academic project**.
**Both contributors shared responsibilities equally**, including:

* GUI layout and design
* Backend logic and class structure
* File handling and user input
* Testing, debugging, and polish
