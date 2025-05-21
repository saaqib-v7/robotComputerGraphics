# Robot Computer Graphics Project
# Overview
This project implements a 3D robot scene with interactive features, hierarchical transformations, and custom shaders. The scene includes a wooden table with a checkerboard pattern and an articulated robot that can move, rotate, and shoot lasers.

# Features
1. Scene Setup
Camera: Positioned at (0,60,150) looking at (0,0,0) with world up vector (0,1,0)

Table:

Tabletop (200x10x200 units) centered at (0,0,0)

Four legs (20x120x20 units) positioned symmetrically

Uses "Wood_Table_Texture.png"

2. Robot Model
Hierarchically constructed from separate OBJ files:

Body, head, arms (upper and lower), and legs

Each part has defined parent-child relationships with relative offsets

Textured with "Robot_Texture.png"

3. Interactive Controls
GUI Sliders for controlling:

Arm rotations (around X-axis)

Head rotations (independent X and Y-axis)

Movement and rotation speeds

Animation:

Legs animate only during movement using CalculateLegAnim()

One leg offset by π for alternating motion

4. Movement System
Ctrl+Click on table to set target position

Robot:

Rotates to face target

Moves linearly to target

Debug cube appears at selected point (toggleable)

5. Shader Effects
Phong shading with directional light from (1/3, 2/3, 2/3)

Tabletop checkerboard pattern (8x8 grid)

Robot shine from "shine.png" texture (red channel × 16)

Debug views:

Normal vectors

Shading only

Diffuse texture only

Shininess texture only

6. Special Effects
Laser:

Cast from robot's head (0,5,3.5) direction (0,0,1)

Creates red point light 1 unit above intersection

Only affects robot and tabletop

3rd Person Camera:

Follows robot from behind head

Matches robot's orientation

Toggleable in GUI

# Implementation Notes
Hierarchical Transformations
Robot parts are transformed relative to their parents using the provided offset vectors. This creates realistic joint movements.

# Animation System
Leg animation is synchronized with movement and pauses when the robot stops. The animOffset parameter creates alternating leg motion.

# Shader Customization
Different shader effects are controlled by passing integer flags to indicate object types (table, robot parts, etc.).

# How to Use
Run the program

Use GUI sliders to control robot joints

Ctrl+Click on table to move robot

Toggle debug views and camera mode in GUI

Click to fire laser from robot's head

# Assets
Textures: Wood_Table_Texture.png, Robot_Texture.png, shine.png

# Models: torso.obj, head.obj, arm/leg OBJ files

This project demonstrates computer graphics concepts including hierarchical modeling, Phong shading, texture mapping, and interactive 3D controls.

