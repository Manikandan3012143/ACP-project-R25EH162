# Vector-CLI 2D Graphics Editor

A robust, interactive 2D graphics editor written in C, designed to run directly in the command line interface. The editor features a canvas initialized with underscores (`_`) where geometric shapes are rasterized with asterisks (`*`). 

This project incorporates vector shape management combined with pixel rasterization, allowing users to **add, modify, delete, and clear** objects without leaving artifacts or destroying overlaps.

---

## 🎨 Preview & Aesthetics

The editor leverages:
- **ANSI Escape Colors**: Highlights active controls, distinguishes pixel layers (canvas grid vs shape pixels), and presents menus in high-contrast cyan, magenta, and yellow.
- **Double-Line Borders**: A solid terminal border surrounding the graphics screen for a premium, integrated application feel.
- **Robust Input Handling**: Automatic input buffer clearing to prevent command loop crashes when entering invalid inputs.

---

## ⚙️ Core Architecture & Design

Standard terminal graphics programs draw directly onto a 2D array, which destroys the background and merges overlapping shapes. This editor solves that problem by implementing a **Vector-Model / Raster-View** design:

1. **Shape Database (Model)**: Shapes are stored as a vector list in a database array. Each shape has a unique ID, type, coordinate data, and active state.
2. **Dynamic Redraw Engine (View)**: When any action (add, modify, delete) takes place:
   - The canvas is completely cleared and filled with `_`.
   - The program loops through active shapes in the database and calls their drawing algorithms to write `*` symbols onto the canvas.
   - The canvas is displayed.
   - This prevents "ghost pixels" and retains correct overlaps!

---

## 📐 Mathematical Drawing Algorithms

The editor implements classical computer graphics rasterization algorithms:

### 1. Bresenham's Line Algorithm
Draws lines between any arbitrary coordinates $(x_1, y_1)$ and $(x_2, y_2)$ using only fast integer arithmetic:
- Pre-calculates offsets and step directions.
- Loops and increments coordinates based on a slope decision error term, updating pixels on the grid.

### 2. Midpoint Circle Algorithm (Bresenham's Circle)
Draws symmetric circles around a center $(cx, cy)$ with radius $r$:
- Draws the circle boundary by calculating coordinates for a single octant.
- Automatically mirrors pixels across all 8 octants to render a perfect circle.

### 3. Rectangle Outline Drawing
- Renders 4 line segments connecting the corners $(x, y)$, $(x+w-1, y)$, $(x, y+h-1)$, and $(x+w-1, y+h-1)$.

### 4. Triangle Outline Drawing
- Renders 3 line segments connecting 3 arbitrary vertex points.

---

## 📁 Repository Structure

```
graphics-editor/
├── graphics_editor.c   # Main C source code (logic, algorithms, UI loop)
├── compile.bat         # Compilation automation batch file for Windows
└── README.md           # Documentation
```

---

## 🚀 How to Compile and Run

This project can be built and run on Windows.

### Compilation using `compile.bat` (Recommended)
Pre-configured to find the compiler and launch the application instantly.
1. Double-click the `compile.bat` file in the project folder, or
2. Run it via PowerShell or Command Prompt:
   ```cmd
   .\compile.bat
   ```

### Manual Compilation
If compiling manually using GCC, you can run:
```bash
gcc -std=c99 -O2 graphics_editor.c -o graphics_editor.exe
.\graphics_editor.exe
```

---

## 📝 Usage Instructions

1. **Add Shape**: Enter the coordinate parameters when prompted (checks are performed to clip shapes gracefully to the canvas boundaries).
2. **Modify Shape**: Displays active shapes with their coordinates. Select the Shape ID to input new coordinate values.
3. **Delete Shape**: Displays active shapes. Enter the Shape ID to disable it. The drawing engine will immediately update.
4. **Clear Canvas**: Clears all shapes, resetting the canvas.
5. **Exit**: Terminates the application.
