# 🎮 2D Sprite Sheet Character Animation

A **C++ OpenGL/GLUT animation project** that demonstrates sprite-sheet animation, character movement, texture rendering, and a scrolling background. The character stays centered while the background moves to create the illusion of traveling through the world. 🏃‍♂️🌄

## ✨ Features

* 🏃 **8-Frame Sprite Animation** — Uses an 8-column sprite sheet to create a smooth running animation.
* ⬅️➡️ **Left & Right Movement** — Control the character using the keyboard arrow keys.
* 🔄 **Direction Flipping** — The character automatically faces the direction it is moving.
* 🌄 **Parallax Background** — The background scrolls opposite to the character's movement.
* 🖱️ **Mouse Auto-Run** — Click either side of the screen to make the character run automatically for 20 seconds.
* ⏱️ **Time-Based Animation** — Animation and movement are updated using real elapsed time.
* 🖼️ **Texture Rendering** — Supports BMP backgrounds and PNG character sprites with transparency.
* 🔲 **Wireframe Mode** — Toggle between normal and wireframe rendering.

## 🎮 Controls

| Control                     | Action                        |
| --------------------------- | ----------------------------- |
| **← Left Arrow**            | Move left                     |
| **→ Right Arrow**           | Move right                    |
| **Release Arrow**           | Stop moving                   |
| **Left Click — Left Side**  | Auto-run left for 20 seconds  |
| **Left Click — Right Side** | Auto-run right for 20 seconds |
| **W**                       | Toggle wireframe mode         |
| **Q / ESC**                 | Exit the program              |

## 🖼️ Sprite Sheet

The character animation uses **8 frames in one row**. OpenGL selects one section of the sprite sheet at a time using texture coordinates and changes the frame while the character moves. 🎞️

```text
Frame 1 → Frame 2 → Frame 3 → ... → Frame 8 → Repeat 🔁
```

## 📁 Project Structure

```text
Project/
├── main.cpp
├── stb_image.h
└── images/
    ├── Background.bmp
    └── character.png
```

The program loads the background as a **BMP texture** and the character sprite sheet as a **PNG texture**.

## 🛠️ Technologies Used

* 💻 C++
* 🎨 OpenGL
* 🪟 GLUT / FreeGLUT
* 🖼️ stb_image
* 🎞️ Sprite Sheet Animation
* 🌄 Texture Mapping
* 🏃 Time-Based Movement

## 🚀 How It Works

When the character moves, the program changes through the sprite-sheet frames to produce the running animation. Instead of moving the character away from the center of the screen, the background scrolls in the opposite direction. This creates the visual effect that the character is moving through the environment. 🌎🏃‍♂️💨

---

⭐ A simple demonstration of **2D character animation, sprite sheets, movement controls, and parallax scrolling using C++ and OpenGL.**
