# Virtual Memory Manager Simulator (C++)

## Overview
This project simulates a simple but powerful **Virtual Memory Manager** in C++. It demonstrates core operating system memory management concepts, including **paging**, **segmentation**, and **page replacement algorithms** (FIFO and LRU). The project is designed for clarity, efficiency, and educational value—perfect for IT associate portfolios or OS coursework.

## Features
- **Paging**: Simulates logical-to-physical address translation using page tables.
- **Segmentation**: Supports multiple, user-named memory segments (e.g., code, data, stack).
- **Page Replacement**: Choose between FIFO and LRU algorithms at runtime.
- **Interactive CLI**: Menu-driven interface for exploring memory management concepts.
- **Statistics**: Tracks page faults, accesses, and fault rates.
- **Robust Input Validation**: Handles invalid input gracefully.
- **Configurable**: Set memory size, page size, segment count, and segment names at startup.

## Requirements
- C++11 or newer
- Windows: [MinGW-w64](https://www.mingw-w64.org/downloads/) recommended
- Linux/Mac: Any modern g++/clang++

## Build Instructions (Windows/MinGW)
1. Open **Command Prompt** or **PowerShell**.
2. Navigate to the project directory:
   ```sh
   cd "C:\Users\abcd\OneDrive\Desktop\virtual_memory_manager"
   ```
3. Compile the project:
   ```sh
   g++ -std=c++11 -o vmm.exe virtual_memory_manager.cpp
   ```

## Running the Program
In PowerShell or Command Prompt, run:
```sh
.\vmm.exe
```

## Usage Example
```
Enter total memory size (bytes): 1024
Enter page size (bytes): 64
Enter number of segments: 3
Enter name for segment 0: code
Enter name for segment 1: data
Enter name for segment 2: stack
Select page replacement policy (1 = FIFO, 2 = LRU): 2

Virtual Memory Manager Simulator
1. Show Segments
2. Show Page Table
3. Show Frames
4. Access Address
5. Show Statistics
0. Exit
Enter choice: 1

Segments:
0: code: Base = 0, Limit = 341
1: data: Base = 341, Limit = 341
2: stack: Base = 682, Limit = 341
```

### Accessing Addresses
- Choose option 4, then enter a segment index and offset.
- Try accessing enough unique pages to trigger page replacement.
- Use option 5 to view statistics.

## Notes
- **Page size** must divide memory size evenly.
- **Segment sizes** are calculated automatically.
- Handles invalid input and out-of-bounds accesses gracefully.

## Credits
Developed by [Your Name] as a professional/portfolio project for IT associate roles.

---
**Showcase your understanding of OS memory management with this project!** 