# OpenGL Brick Breaker Game

## Overview

This is a simple OpenGL-based brick breaker game developed in C++. The game features destructible and reflective bricks, bouncing balls, and a dynamic environment where additional balls are generated upon collision with destructible bricks.

## Features

- Real-time physics-based brick breaking
- Different types of bricks (Reflective and Destructible)
- Dynamic ball generation upon brick destruction
- OpenGL rendering using GLFW and GLEW
- Texture loading for bricks using stb\_image
- Collision detection and realistic ball movement

## Installation and Dependencies

### Required Libraries:

- [GLFW](https://www.glfw.org/) - Window management and input handling
- [GLEW](http://glew.sourceforge.net/) - OpenGL Extension Wrangler
- [stb\_image](https://github.com/nothings/stb) - Image loading for textures

### Setup:

1. Clone the repository:
   ```sh
   git clone https://github.com/keithpotz/OpenBallGame.git
   ```
2. Install the necessary dependencies:
   - Ensure you have OpenGL development libraries installed.
   - Download and link GLFW and GLEW.
   - Include `stb_image.h` in your project.
3. Compile the project:
   ```sh
   g++ -o BrickBreaker MainCode.cpp -lglfw -lGLEW -lGL -lGLU -lm
   ```
4. Run the executable:
   ```sh
   ./BrickBreaker
   ```

## Controls

- `SPACE` - Releases a new ball into the scene
- `ESC` - Exits the game

## Code Structure

- ``: Contains the game logic, rendering, collision detection, and physics implementation.
- ``: Used for texture loading.

## How It Works

- The game initializes a window using GLFW.
- Bricks are generated at random positions with either a **Reflective** or **Destructible** property.
- When a ball collides with a **Destructible** brick, it reduces its hit count. Once the hit count reaches zero, the brick disappears, and additional balls spawn.
- Balls bounce off **Reflective** bricks based on collision physics.
- The game renders everything in real-time and tracks the number of balls on the screen.

## Software Version

- Version: `20250106a`
- This version includes a fix for the `MoveOneStep` method, improving ball movement realism.

## Future Improvements

- Implement a scoring system.
- Add power-ups and special effects.
- Improve collision detection accuracy.
- Implement sound effects and background music.

## License

This project is licensed under the MIT License. Feel free to modify and distribute.

## Contact

For any issues or feature requests, please open an issue on GitHub or contact `keithpotz@gmail.com`.

---

Enjoy the game!

