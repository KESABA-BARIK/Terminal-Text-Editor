# Terminal Text Editor (C++)

A simple terminal-based text editor built using **C++** and **(PD)Curses** library.  
Lightweight, fast, and runs in the terminal!

---

## ✨ Features
- Create and edit text files
- Save your work
- Undo/Redo functionality
- Minimalist and efficient terminal UI
- Cross-platform support (Linux & Windows)

---

## 🛠 Requirements

### For Linux:
- C++ compiler (`g++`, `clang++`)
- `ncurses` library

Install `ncurses`:

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```
## For Windows:
C++ compiler (g++ from MinGW or similar)

PDCurses library installed form https://pdcurses.sourceforge.io/

## ⚡ How to Compile and Run
### On Linux:
```bash
g++ main.cpp -lncurses -o text_editor
./text_editor
```
### On Windows:
Download and set up PDCurses.

Compile using:

```bash
g++ main.cpp -I<YOUR_PATH>/PDCurses -L<YOUR_PATH>/PDCurses/wincon -l:pdcurses.a -o text_editor.exe
```
(Adjust the C:/Users/Lenovo/PDCurses path if needed.)

#### Run:

```bash
text_editor.exe
```

### 📜 License
Open-source project — free to use, modify, and share!
