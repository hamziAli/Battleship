# Battleship Game ⚓ 🚢

A modular, console-based **C++** text-based strategy game replicating the classic tabletop naval combat experience[cite: 120, 134]. Developed as a semester project for **CS112 (Object-Oriented Programming)** at the **Ghulam Ishaq Khan Institute of Engineering Sciences and Technology (GIKI)**[cite: 109, 113, 120].

Developed by[cite: 114, 115]:
* **Mohammad Hassaan** (Reg No: 2024302)
* **Hamza Ali** (Reg No: 2024208)
* **Raja Hamza Sikandar** (Reg No: 2024532)

---

## 🚀 Features

- **Multiple Game Modes:** - **Player vs Player (PvP):** Face off against a friend locally with grid strategy combat[cite: 121, 126].
  - **Player vs Computer (PvC):** Play against the system using two distinct AI difficulty modes[cite: 121, 130].
- **Adaptive AI Intelligence:** [cite: 130, 134]
  - **Normal Mode:** Computer targets cells completely at random[cite: 130].
  - **Smart Mode:** Adjacency-targeting logic that systematically hunts surrounding cells once a hit is registered, handling tricky edge and corner boundaries[cite: 130, 136].
- **Quickplay Demo Mode:** An automated walkthrough that instantly plays out an entire simulation game end-to-end to showcase logic execution[cite: 130, 154].
- **Patriotic Theme Configuration:** Visual design styled to resemble a command center display, featuring ship classifications inspired directly by **Pakistan Navy Vessels**[cite: 125]:
  - `T` - PNS Tughril (Length: 5) [cite: 125, 209]
  - `Z` - PNS Zulfiqar (Length: 4) [cite: 125, 209]
  - `H` - PNS Hangor (Length: 3) [cite: 125, 209]
  - `Y` - PNS Yarmuk (Length: 3) [cite: 125, 209]
  - `M` - PNS Mujahid (Length: 2) [cite: 125, 209]
- **Robust Rule Enforcement:** Built-in verification checks to prevent out-of-bounds positioning, overlapping vessel placement, and redundant shots[cite: 134, 137].

---

## 📁 System Architecture & Modules

The software is constructed with a strict **separation of concerns**, decoupling the underlying business logic from raw console I/O[cite: 134, 212]:

- **Player Module:** Encapsulates stats, scorekeeping, and active game states for both human and CPU configurations[cite: 145, 147]. Each player retains two separate `Board` objects (one for tracking self positioning, one for firing records)[cite: 146, 147].
- **Board Module:** Houses the underlying data representing the $8\times8$ operational matrix grid[cite: 149, 150]. Controls rule validation triggers inside `placeShip()` and processes damage updates inside `processShot()`.
- **Game Module:** Powers the overarching main run loop (`Game::run()`), evaluates player turn sequencing, hooks the smart algorithm, and manages the primary game configurations[cite: 153, 154].
- **UI Module:** Streamlines console output formatting, clears frames gracefully between active turns to prevent text garbling, and wraps `cin`/`cout` inside reusable utility functions like `drawGameBoard()`[cite: 135, 156, 157, 159].
- **Main Entry Point (`main.cpp`):** Seeds the random number generator, bootstraps the application shell, and hands initialization off to the `Game` instance[cite: 161, 162].

---

## 🛠️ OOP Concepts Utilized

1. **Encapsulation:** All structural properties and grid properties are explicitly isolated as `private` context fields (e.g., `Player::score`, `Board::grid`, `Ship::positions`), exposed strictly through verified accessor and mutator methods[cite: 165, 166, 167].
2. **Abstraction:** Internal layout cycles and dynamic memory handling routines are completely masked behind a high-level orchestration interface API (`run()`, `play()`)[cite: 168, 170].
3. **Composition:** Models authentic real-world entity associations using cohesive "has-a" relationship designs: A `Player` contains two `Board` instances, a `Board` holds an array of `Ship` structures, and each `Ship` maps out an array of `Position` coordinates[cite: 171, 172, 173, 174].
4. **Polymorphism:** Standardized implementation interface templates ensure both human entities and automated CPU players execute turns seamlessly through identical polymorphic references (`Player&`)[cite: 175, 176, 177].
5. **Extensibility via Inheritance:** All major operational boundaries implement scalable public virtual contracts, paving the way for replacement implementations such as a graphical UI client (`GuiUI`) or a network connection buffer layer (`NetworkedPlayer`) without breaking game loop invariants[cite: 179, 180, 181, 183].

---

## 💻 Getting Started

### Prerequisites
- Any modern C++ compiler supporting standard versions (**C++11** or newer) such as `GCC`, `Clang`, or `MSVC`.

### Installation & Compilation
1. Clone the repository down to your computer workspace[cite: 216]:
   ```bash
   git clone [https://github.com/hamziAli/Battleship.git](https://github.com/hamziAli/Battleship.git)
   cd Battleship
