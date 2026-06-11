# Project Generation Prompts & History

This document lists the original assignment requirements and the prompts/steps used to design and build this 2D Vector-CLI Graphics Editor.

---

## 📋 1. Original Assignment Prompt

```text
Create a repository in GitHub.
Submit the link to the GitHub repository in Moodle. (In the online text field provided in the assignment)
Daily push to GitHub is mandatory.

Problem Statement: Design and implement a menu-driven 2D Graphics Editor in C using a 2D character array as the drawing canvas. The canvas should initially be filled with the character _ (underscore), and graphical objects should be drawn using the character * (asterisk).

Write functions to draw a circle, rectangle, line, and triangle.
You should be able to:
- add objects to the picture,
- delete objects from the picture, and
- modify objects in the picture.
Use a 2d array of characters to store the picture.
Write a function to display the picture.

Additionally, you can use the ncurses package to create menus and draw pictures.
```

---

## 🛠️ 2. Architectural Design Choices

To meet the requirement of **adding**, **deleting**, and **modifying** shapes without leaving residue pixels or breaking overlaps, the following prompt-engineering structure was defined:

1. **Model (Shape List)**:
   - Dynamic vector storage for shapes (`Shape` structs) with active/inactive states.
   - Allows clean deletion and modification of parameters by updating the database.
2. **View (2D Raster Canvas)**:
   - A $22 \times 70$ grid initialized with `_`.
   - On change, it clears the grid and re-rasterizes the active shape list using asterisks `*`.

---

## 📐 3. Mathematical Drawing Algorithms

The math algorithms selected for standard integer-based raster grid drawing:

- **Bresenham's Line Algorithm**: Computes coordinates between any $(x_1,y_1)$ and $(x_2,y_2)$ without floating-point math.
- **Midpoint Circle Algorithm**: Renders circle octants symmetrically.
- **Rectangle**: Draws 4 lines connecting bounding corners.
- **Triangle**: Draws 3 lines connecting vertices.

---

## ⚙️ 4. Local Environment & Compiler Commands

For compilation on Windows Command Line using MinGW GCC:

```powershell
# Prepend MinGW bin directory to the execution path (locally)
$env:PATH = "C:\MinGW\bin;" + $env:PATH

# Compile with optimization
gcc -std=c99 -O2 graphics_editor.c -o graphics_editor.exe

# Run the compiled executable
.\graphics_editor.exe
```

---

## 🚀 5. Git & GitHub Push Commands

To version control the project locally and link it to the GitHub repository:

```powershell
# Initialize git repository
git init

# Add all project source files
git add .

# Initial commit
git commit -m "Initial commit: 2D graphics editor in C"

# Link to GitHub remote repository (named "Harin.v_R25EH163")
git remote add origin https://github.com/Harin-1407/Harin.v_R25EH163.git

# Set branch name to main
git branch -M main

# Push code to GitHub
git push -u origin main
```
