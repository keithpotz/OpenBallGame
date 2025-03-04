#include <Windows.h>
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>

int ballCount = 0;
using namespace std;
const float DEG2RAD = 3.14159 / 180;

// software version number (note: this version contains the fix for the 
// MoveOneStep method)
const char* const SW_VERSION = "20250106a"; // do not delete this line

void processInput(GLFWwindow* window);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };

class Circle;
vector<class Brick> bricks;
vector<Circle> world;

//Implement stb_image to add textures to the bricks
GLuint loadTexture(const char* filename) {
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cerr << "Failed to load texture" << std::endl;
		return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB; // Support RGBA if available

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return textureID;
}

class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	BRICKTYPE brick_type;
	ONOFF onoff;
	int hitCount; //Tracking amount of hits before a block is destroyed.
	GLuint textureID;

	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb,int hits, const char* texturePath)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		onoff = ON;
		hitCount = 20; //Bricks need 20 hits to dissapear
		textureID = loadTexture(texturePath);
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glColor3f(1.0f, 1.0f, 1.0f);

			glBegin(GL_QUADS);



			glColor3d(red, green, blue);
			glBegin(GL_QUADS);
			glTexCoord2f(1, 1); glVertex2d(x + halfside, y + halfside);
			glTexCoord2f(1, 0); glVertex2d(x + halfside, y - halfside);
			glTexCoord2f(0, 0); glVertex2d(x - halfside, y - halfside);
			glTexCoord2f(0, 1); glVertex2d(x - halfside, y + halfside);
			glEnd();

			glDisable(GL_TEXTURE_2D);
		}
	}
	void takeHit() {
		hitCount--;
		if (hitCount <= 0) {
			onoff = OFF;
		}
	};
};

	class Circle
	{
	public:
		float red, green, blue;
		float radius;
		float x;
		float y;
		float speed = 0.03;
		int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left

		Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
		{
			x = xx;
			y = yy;
			radius = rr;
			red = r;
			green = g;
			blue = b;
			radius = rad;
			direction = dir;
		}
	
		void CheckCollision(Brick* brk)
		{
			if (brk->onoff == OFF) return; //Skip if brik is destroyed

			bool collisionX = (x + radius > brk->x - brk->width / 2) && (x - radius < brk->x + brk->width) && (y + radius > brk->y - brk->width && y - radius < brk->y + brk->width);
			bool collisionY = (y + radius > brk->y - brk->width / 2) && (y - radius < brk->y + brk->width) && (x + radius > brk->x - brk->width && x - radius < brk->x + brk->width);

			if (collisionX && collisionY) {
				if (brk->brick_type == REFLECTIVE) {
					float overlapX = min(abs((x + radius) - (brk->x - brk->width / 2)), abs((x - radius) - (brk->x + brk->width / 2)));
					float overlapY = min(abs((y + radius) - (brk->y - brk->width / 2)), abs((y - radius) - (brk->y + brk->width / 2)));

					//Determin whether to bounce on X or Y based on impact direction
					if (overlapX < overlapY) {
						//Hit on sides -> reverse X direction
						if (direction == 2) direction = 4;
						else if (direction == 4) direction = 2; //Right -> left
						else if (direction == 5) direction = 6; //Up-right -> Up-Left
						else if (direction == 6) direction = 5; // Up-Left-> Up-right
						else if (direction == 7) direction = 8; //Down-right -> Down-left
						else if (direction == 8) direction = 7; //Down-left -> Down-right
					}
					else {
						//Hit on top or bottom -> reverse Y direction
						if (direction == 1) direction = 3; //Up -> Down
						else if (direction == 3) direction = 1; //Down -> Up
						else if (direction == 5) direction = 7; //Up-right -> Down-right
						else if (direction == 6) direction = 8;	//Up-left -> down-left
						else if (direction == 7) direction = 5; //Up-left -> Down-left
						else if (direction == 8) direction = 6; //Down-left -> Up-left
					}
					//push the ball slightly away to prevent sticking
					if (direction == 1 || direction == 5 || direction == 6) y += 0.01;
					else if (direction == 3 || direction == 7 || direction == 8) y -= 0.01;
					else if (direction == 2 || direction == 5 || direction == 7) x -= 0.01;
					else if (direction == 4 || direction == 6 || direction == 8) x += 0.01;

				}
				else if (brk->brick_type == DESTRUCTABLE) {
					brk->takeHit(); // Reduce hit count instead of destroing immediately
					// Spawn smaller balls when a brick is hit
					int numNewCircles = (rand() % 2) + 2;

					for (int i = 0; i < numNewCircles; i++) {
						float newX = x + ((rand() % 20) / 100.0f) - 0.1f; //Slightly offset
						float newY = y + ((rand() % 20) / 100.0f) - 0.1f; //Slightly offset
						float newRadius = radius * 0.5f; 
						int newDirection = (rand() % 8) + 1;

						float r = static_cast<float>(rand()) / RAND_MAX; //random color
						float g = static_cast<float>(rand()) / RAND_MAX; //random color
						float b = static_cast<float>(rand()) / RAND_MAX; //random color
						world.push_back(Circle(newX, newY, newRadius, newDirection, newRadius, r, g, b));
						//ball counter
						ballCount++;
					}
				}

			}
		}

		int GetRandomDirection()
		{
			return (rand() % 8) + 1;
		}

		void MoveOneStep() //I dediced to modify the MoveOneStep... I didn't like the way it bounced or moved in the scene
			//so this is a modified one to make it more realistic
		{
			// Hit the top or bottom?  Reverse Y direction
			if (y + radius >= 1.0 || y - radius <= -1.0)
			{
				if (direction == 1) direction = 3;
				else if (direction == 3) direction = 1;
				else if (direction == 5) direction = 7;
				else if (direction == 7) direction = 5;
				else if (direction == 6) direction = 8;
				else if (direction == 8) direction = 6;
			}
			//Hit left or right? reverse X direction
			if (x + radius >= 1.0 || x - radius <= -1.0)
			{
				if (direction == 2) direction = 4;
				else if (direction == 4) direction = 2;
				else if (direction == 5) direction = 6;
				else if (direction == 6) direction = 5;
				else if (direction == 7) direction = 8;
				else if (direction == 8) direction = 7;
			}
			//Now move in the chose direction
			switch (direction) {
			case 1: y += speed; break; // Up 
			case 2: x += speed; break; // right
			case 3: y -= speed; break; // down
			case 4: x -= speed; break; // left
			case 5: x += speed; y += speed; break; // up right
			case 6: x -= speed; y += speed; break; // up left
			case 7: x += speed; y -= speed; break; // down right
			case 8: x -= speed; y -= speed; break; // down left

			}
		}

		void DrawCircle()
		{
			glColor3f(red, green, blue);
			glBegin(GL_POLYGON);
			for (int i = 0; i < 360; i++) {
				float degInRad = i * DEG2RAD;
				glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
			}
			glEnd();
		}
	};
	

	void renderText(const std::string& text, float x, float y) {
		glColor3f(1.0f, 1.0f, 1.0f);  // White color
		glRasterPos2f(x, y);  // Set text position

		for (char c : text) {
			// Use Windows OpenGL function to render text
			wglUseFontBitmaps(GetDC(NULL), c, 1, 1000);
			glCallList(1000);
		}
	}
    // Replace the displayBallcount function with the following code
    void displayBallcount() {
		std::string text = "Ball Count: " + std::to_string(ballCount);
		renderText(text, -0.9f, 0.9f); // Adjust the position as needed
        
    }
	

	void generateBricks() {

		int numBricks = (rand() % 16) + 5; // Generate between 5 and 20 bricks

		float brickWidth = 0.2; // ensure width is defined

		for (int i = 0; i < numBricks; i++) {
			float randomX = 0.0f, randomY = 0.0f; // intilize variables
			bool overlap;
			//ensure bricks don't overlap
			do {
				overlap = false;
				 randomX = ((rand() % 80) / 50.0) - 0.8;  //Ranges from -0.8 to 0.8
				 randomY = ((rand() % 80) / 50.0) - 0.8;
				for (auto& brick : bricks) {
					if (abs(randomX - brick.x) < 0.2 && abs(randomY - brick.y) < 0.2) {
						overlap = true;
						break;
					}
				}
			} while (overlap);

			BRICKTYPE type = (rand() % 2 == 0) ? REFLECTIVE : DESTRUCTABLE;
			int randomHits = (rand() % 21) + 40; // Random hits between 40 and 60
			bricks.push_back(Brick(type, randomX, randomY, brickWidth, 1, 1, 0, randomHits, "textures/brick.jpg"));
		}

	}

	// toggle for spacebar
	bool spacePressed = false;

	int main(void) {
		srand(time(NULL));

		if (!glfwInit()) {
			exit(EXIT_FAILURE);
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		GLFWwindow* window = glfwCreateWindow(480, 480, "8-2 Assignment", NULL, NULL);
		if (!window) {
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		glewInit();
		displayBallcount();
		generateBricks();

		// print the version to the console
		std::cout << std::endl << "Version: " << SW_VERSION << std::endl; // do not delete this line

		

		while (!glfwWindowShouldClose(window)) {
			//Setup View
			float ratio;
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			ratio = width / (float)height;
			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT);

			processInput(window);
			displayBallcount(); //ball count

			for (auto& ball : world) {
				for (auto& brk : bricks) {
					ball.CheckCollision(&brk);
				}
				ball.MoveOneStep();
				ball.DrawCircle();
			}
			world.erase(
				remove_if(world.begin(), world.end(),
					[](const Circle& c)-> bool {return c.radius == 0; }
				),
				world.end()
			);

			for (auto& brk : bricks) {
				brk.drawBrick();
			}

			
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_SUCCESS);
	}

	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		static bool spacePressed = false; // prevent multiple triggers

		// code has been updated to release one ball per spacebar press and release
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			spacePressed = true;
		}
		// code has been updated to release one ball per spacebar press and release
		if (spacePressed && (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE))
		{
			spacePressed = false;
			//Generate random position for ball (-.9 to 0.7)
			float randomX = ((rand() % 140) / 100.0) - 0.7;
			float randomY = ((rand() % 140) / 100.0) - 0.7;

			//Random Direction 
			int possibleDirection[] = { 5, 6, 7, 8 };
			int randomDirection = possibleDirection[rand() % 4];

			//Generate Random Color
			float r = static_cast<float>(rand()) / RAND_MAX;
			float g = static_cast<float>(rand()) / RAND_MAX;
			float b = static_cast<float>(rand()) / RAND_MAX;

			//Create new ball with random properties
			world.push_back(Circle(randomX, randomY, 0.02, randomDirection, 0.05, r, g, b));

			
		}
	}

