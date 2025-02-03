# ΒραΧαΨα III - A 3D Candy Crush-like Game

## Overview
This project is a 3D puzzle game, inspired by the mechanics of Candy Crush, developed using OpenGL and GLUT. The game involves interacting with 3D cubes representing "rock", "paper", "scissors" and colored blocks in a 15x15 grid. Players swap cubes to form matching combinations and score points.

## Features
- **3D Rendering**: Realistic 3D cubes with lighting and perspective projection.
- **Game Mechanics**:
  - Matching three or more cubes causes an "explosion" and awards points.
  - Special mechanics for destroying adjacent cubes based on type interactions.
- **Camera Control**: Rotate the camera to view the grid from different angles using arrow keys.
- **Score System**: Tracks and displays player scores based on gameplay actions.
- **User Interface**:
  - Start and exit options in the menu.
  - Black background with interactive cube movement.

## Controls
- **Mouse**:
  - Left-click to select and swap adjacent cubes.
- **Keyboard**:
  - Arrow keys to rotate the camera.

## Tools
- OpenGL
- GLUT

## Files
- `main.cpp`: The main program source code.
- `Makefile`: Build configuration file.
- `paper.png`, `rock.png`, `scissors.png`: Image resources for cube textures.
- `stb_image.h`: Header file for image loading.
- `readme.pdf`: Contains detailed descriptions and team member details.

## Credits
Eleni Mouzaki
Panagiotis Papaioannou
