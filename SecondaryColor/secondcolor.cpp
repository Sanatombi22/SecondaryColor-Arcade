#include <GL/glut.h>
#include <iostream>
#include <cstdlib>  // Include for the exit function
#include <cmath>


// Constants
const int screenWidth = 800;
const int screenHeight = 600;
const float playerSpeed = 8.0;
const float projectileSpeed = 10.0;
const int maxProjectiles = 10;
const float enemySpeed = 1.5;
const int maxEnemies = 10;

struct Projectile {
    float x, y;
    int active;
};

struct Enemy {
    float x, y;
    int active;
    int secondaryColor;
};

Projectile projectiles[maxProjectiles];
Enemy enemies[maxEnemies];

float playerX = screenWidth / 2;
float playerY = 50.0;


int leftKeyPressed = 0;    // Variables for player movement
int rightKeyPressed = 0;   // Variables for player movement
const int fireRate = 10;    // Constants for shooting ( Limit the rate of fire )
int frameCount = 0;         // Constants for shooting ( Counter for frame updates )
int spacebarPressed = 0;    // Declare and initialize spacebarPressed variable

const float projectileRadius = 5.0; // Constants for projectile
const float enemyRadius = 15.0;     // Constants for enemy
const float enemySpawnInterval = 120; // Adjust enemy spawn rate
int enemySpawnTimer = 0;

// Function to draw a circle
void drawCircle(float x, float y, float radius, float color[]) {
    int numSegments = 100;
    glBegin(GL_TRIANGLE_FAN);
    glColor3fv(color);
    glVertex2f(x, y);
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0 * M_PI * (float)i / (float)numSegments;
        float px = x + radius * cosf(angle);   // cosfunction
        float py = y + radius * sinf(angle);
        glVertex2f(px, py);
    }
    glEnd();
}

// Initialize the game  // Initialize projectiles and enemies here
void initGame() {
    // Initialize projectiles
    for (int i = 0; i < maxProjectiles; i++) {
        projectiles[i] = { -1.0, -1.0, 0 };
    }

     // Initialize enemies
     for (int i = 0; i < maxEnemies; i++) {
        enemies[i].x = rand() % screenWidth; // Random initial X position
        enemies[i].y = screenHeight; // Start enemies at the top of the screen
        enemies[i].active = 1; // Activate the enemy
        enemies[i].secondaryColor = rand() % 5; // Random secondary and tertiary color
    }
}

// Handle player movement and shooting
// Handle player movement using keyboard input
void handlePlayer() {
    if (leftKeyPressed) {
        playerX -= playerSpeed;
        if (playerX < 0) {
            playerX = 0;
        }
    }
    if (rightKeyPressed) {
        playerX += playerSpeed;
        if (playerX > screenWidth) {
            playerX = screenWidth;
        }
    }

// Handle shooting ( when the spacebar is pressed)
    frameCount++; // Increment frame count
    if (frameCount >= fireRate) {
        if (spacebarPressed) {
            // Find an inactive projectile
            for (int i = 0; i < maxProjectiles; i++) {
                if (!projectiles[i].active) {
                    projectiles[i] = { playerX, playerY, 1 };
                    break;  // Stop searching for an inactive projectile
                }
            }
        }
        frameCount = 0; // Reset the frame count
    }
}

// Keyboard callback function to handle key presses
void keyPressed(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':   // Handle spacebar press for shooting
            spacebarPressed = 1; // Set spacebar flag to indicate shooting
            break;
        case 'a':
            leftKeyPressed = 1;
            break;
        case 'd':
            rightKeyPressed = 1;
            break;
    }
}

// Keyboard callback function to handle key releases
void keyReleased(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':   // Handle spacebar release (to stop shooting)
            spacebarPressed = 0; // Reset spacebar flag
            break;
        case 'a':
            leftKeyPressed = 0;
            break;
        case 'd':
            rightKeyPressed = 0;
            break;
    }
}


// Handle projectile movement and collisions    // Update projectile positions and check for collisions
void handleProjectiles() {
    for (int i = 0; i < maxProjectiles; i++) {
        if (projectiles[i].active) {
            projectiles[i].y += projectileSpeed;  // Update projectile position

            // Check for collision with enemies
            for (int j = 0; j < maxEnemies; j++) {
                if (enemies[j].active) {
                    float dx = projectiles[i].x - enemies[j].x;
                    float dy = projectiles[i].y - enemies[j].y;
                    float distance = sqrt(dx * dx + dy * dy);

                    // Check if the projectile has hit an enemy and Handle collision
                    if (distance < projectileRadius) {
                        projectiles[i].active = 0;  // Deactivate the projectile
                        enemies[j].active = 0;      // Deactivate the enemy
                    }
                }
            }

            // Deactivate projectiles when they go off-screen
            if (projectiles[i].y > screenHeight) {
                projectiles[i].active = 0;
            }
        }
    }
}


// Handle enemy movement and collisions
void handleEnemies() {

    // Update enemy positions and check for collisions
    for (int i = 0; i < maxEnemies; i++) {
        if (enemies[i].active) {
            enemies[i].y -= enemySpeed; // Update enemy position ( move downward)

            // Check for collision with the player
            float dx = playerX - enemies[i].x;
            float dy = playerY - enemies[i].y;
            float distance = sqrt(dx * dx + dy * dy);

            // Check if the enemy has hit the player and Handle collision
            if (distance < enemyRadius) {
               enemies[i].active = 0; // Deactivate the enemy
                std::cout << "Game Over! Enemy collided with the player." << std::endl;
                exit(0);
            }

            // Deactivate enemies when they go off-screen
            if (enemies[i].y < 0) {
                enemies[i].active = 0;
            }
        }
    }

// Spawn new enemies periodically
enemySpawnTimer++;
    if (enemySpawnTimer >= enemySpawnInterval) {
        for (int i = 0; i < maxEnemies; i++) {
            if (!enemies[i].active) {      // Spawn a new enemy
                   enemies[i].x = rand() % screenWidth; // Random X position
                  enemies[i].y = screenHeight; // Start from the top
                enemies[i].active = 1; // Activate the enemy
               break; // Spawn one enemy at a time
            }
        }
        enemySpawnTimer = 0; // Reset the spawn timer
    }
}

// Handle collisions between projectiles and enemies
void handleCollisions() {
    // Check for collisions between projectiles and enemies
    //If a collision occurs, remove the projectile/enemy and trigger Game Over
    for (int i = 0; i < maxProjectiles; i++) {
        if (projectiles[i].active) {
            for (int j = 0; j < maxEnemies; j++) {
                if (enemies[j].active) {
                    // Calculate distance between projectile and enemy
                    float dx = projectiles[i].x - enemies[j].x;
                    float dy = projectiles[i].y - enemies[j].y;
                    float distance = sqrt(dx * dx + dy * dy);

                    // Check if the projectile has hit an enemy
                    if (distance < projectileRadius + enemyRadius) {
                        // Check if the projectile color matches the tertiary color of the enemy
                        // if (enemies[j].tertiaryColor != 0 && enemies[j].tertiaryColor != 1 && enemies[j].tertiaryColor != 2 &&
                        //    enemies[j].tertiaryColor != 3 && enemies[j].tertiaryColor != 4 && enemies[j].tertiaryColor != 5) {

                        // Check if the projectile color matches the tertiary color of the enemy
                       if (enemies[j].secondaryColor != 0 && enemies[j].secondaryColor != 1 && enemies[j].secondaryColor != 2) {
                            std::cout << "Game Over! Projectile hit a non-secondary color enemy." << std::endl;  // Trigger Game Over logic
                            exit(0);    // Exit the program ,that is end the game and close the window
                        }
                        else {
                            // Projectile color matches the tertiary color of the enemy
                            // Deactivate the projectile and enemy
                            projectiles[i].active = 0;
                            enemies[j].active = 0;
                        }
                    }
                }
            }
        }
    }
}

// Handle drawing objects on the screen
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the player
    float playerColor[] = { 1.0, 1.0, 1.0 }; // White
    drawCircle(playerX, playerY, 20.0, playerColor);

    // Draw projectiles
    for (int i = 0; i < maxProjectiles; i++) {
        if (projectiles[i].active) {    // Draw projectiles
            float projectileColor[] = { 1.0, 0.0, 0.0 }; // Red color
            drawCircle(projectiles[i].x, projectiles[i].y, projectileRadius, projectileColor);
        }
    }

// Draw enemies
    for (int i = 0; i < maxEnemies; i++) {
        if (enemies[i].active) {
            // Determine color based on secondaryColor (0: Yellow, 1: Cyan, 2: Magenta, 3: Orange, 4: Green-yellow, 5: dark green )
            float enemyColor[3];
            if (enemies[i].secondaryColor == 0) {  //0 YELLOW
                enemyColor[0] = 1.0;
                enemyColor[1] = 1.0;
                enemyColor[2] = 0.0;
            } else if (enemies[i].secondaryColor == 1) { //1 CYAN
                enemyColor[0] = 0.0;
                enemyColor[1] = 1.0;
                enemyColor[2] = 1.0;
            }
             else if (enemies[i].secondaryColor == 2) {  //2 Magenta
                enemyColor[0] = 1.0;
                enemyColor[1] = 0.0;
                enemyColor[2] = 1.0;
            }
             else if (enemies[i].secondaryColor == 3) { //3 Orange
                enemyColor[0] = 1.0;
                enemyColor[1] = 0.5;
                enemyColor[2] = 0.0;
            }
            // else if (enemies[i].secondaryColor == 4) { //4 Green-Yellow
             //   enemyColor[0] = 0.5;
             //   enemyColor[1] = 1.0;
             //   enemyColor[2] = 0.0;
     //   }
            else {
                enemyColor[0] = 0.0;    //5 dark green
                enemyColor[1] = 0.5;
                enemyColor[2] = 0.0;
            }

            // Draw enemies
            drawCircle(enemies[i].x, enemies[i].y, enemyRadius, enemyColor);
        }
    }

    // Swap buffers
    glutSwapBuffers();
}


// Main game loop
void gameLoop(int value) {

    handlePlayer();     // Handle player input
    handleProjectiles(); // Update projectile positions and check for collisions
    handleEnemies(); // Update enemy positions and check for collisions
    handleCollisions(); // Check for collisions between projectiles and enemies
    glutPostRedisplay();  // Redraw the scene
    glutTimerFunc(1000 / 60, gameLoop, 0); // Call the game loop again
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Secondary Color Arcade - Cyan, Magenta, Yellow");
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
    glutDisplayFunc(display);

    // Register keyboard callback functions
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);

    initGame();
    glutTimerFunc(0, gameLoop, 0);
    glutMainLoop();
    return 0;
}
