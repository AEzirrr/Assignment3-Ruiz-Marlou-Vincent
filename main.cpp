#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <fstream>


#include "model.h"
#include "camera.h"
#include "Physics/MyVector.h"
#include "Physics/PhysicsWorld.h" 
#include "RenderParticle.h"
#include "chrono"

#include "Physics/DragForceGenerator.h"
#include "Physics/ParticleContact.h"
#include "Physics/AnchoredSpring.h"
#include "Physics/Rod.h"    
#include "Physics/Bungee.h"
#include "Physics/Cable.h"
#include "Physics/ParticleSpring.h"


using namespace std::chrono_literals;

constexpr std::chrono::nanoseconds timeStep(16ms);

#define pi 3.141592653589793238462643383279502884197

// camera speed values
float camSpeed = 0.0035f;
float camLookSpeed = 0.035f;

// Stores the state of the key (pressed or released)
std::unordered_map<int, bool> keyStates;

// Value for tracking object spawn timer
double lastSpawn = 0.0;

float yaw = 90;
float pitch = 0;

float scaleVal = 1.0f;

bool spacePressed = false;

// Switch between camera modes
bool usePerspective = false;

// Prevent camera toggling on key hold
bool changeCamPressed = false;

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        keyStates[key] = true;
    }

    if (key == GLFW_KEY_1 && usePerspective)
    {
        usePerspective = false;
        changeCamPressed = true;
    }
    if (key == GLFW_KEY_2 && !usePerspective)
    {
        usePerspective = true;
        changeCamPressed = true;
    }

    else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}

// Set a fixed radius for the orbit
float camRadius = 20.0f;

void ProcessInput() {

    // Only update pitch / yaw for orbiting
    if (keyStates[GLFW_KEY_W]) pitch += camLookSpeed;
    if (keyStates[GLFW_KEY_S]) pitch -= camLookSpeed;
    if (keyStates[GLFW_KEY_A]) yaw += camLookSpeed;
    if (keyStates[GLFW_KEY_D]) yaw -= camLookSpeed;

    // Clamp pitch to avoid flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    if (keyStates[GLFW_KEY_SPACE]) {
        if (!spacePressed) {
            spacePressed = true;
            std::cout << "Space key pressed!" << std::endl;
        }
    }
    else {
        spacePressed = false;
    }

    // Moves camera pitch and yaw based on arrow inputs (Look up and down)
    if (keyStates[GLFW_KEY_UP]) pitch += camLookSpeed;
    if (keyStates[GLFW_KEY_DOWN]) pitch -= camLookSpeed;

    if (keyStates[GLFW_KEY_LEFT]) yaw -= camLookSpeed;
    if (keyStates[GLFW_KEY_RIGHT]) yaw += camLookSpeed;
}

void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    glUseProgram(0);
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_LINES);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);
    glEnd();
}

glm::vec3 ToGlmVec3(const Physics::MyVector& v) {
    return glm::vec3(v.x, v.y, v.z);
}


bool AtCenter(const Physics::P6Particle& particle, float epsilon = 0.1f) {
    const auto& pos = particle.position;
    return (std::abs(pos.x) < epsilon) &&
        (std::abs(pos.y) < epsilon) &&
        (std::abs(pos.z) < epsilon);
}

int main(void) {
    float cableLen = 0.0f;
    float particleGap = 0.0f;
    float particleRadius = 0.0f;
    float gravityStrength = 0.0f;
    float applyForceX = 0.0f;
    float applyForceY = 0.0f;
    float applyForceZ = 0.0f;

    std::cout << "Enter Cable Length: ";
    std::cin >> cableLen;
    std::cout << "Enter Particle Gap: ";
    std::cin >> particleGap;
    std::cout << "Enter Particle Radius: ";
    std::cin >> particleRadius;
    std::cout << "Enter Gravity Strength (Y-axis, negative for downward): ";
    std::cin >> gravityStrength;
    std::cout << "Apply Force";
    std::cout << std::endl << "X: "; std::cin >> applyForceX;
    std::cout << "Y: "; std::cin >> applyForceY;
    std::cout << "Z: "; std::cin >> applyForceZ;

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    // Window values
    float windowWidth = 640;
    float windowHeight = 640;

    // Window creation
    window = glfwCreateWindow(windowWidth, windowHeight, "Assignment3 Marlou Vincent Ruiz", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    // Input handling for program
    glfwSetKeyCallback(window, Key_Callback);

    // Load vertex shader
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    // Load fragment shader
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    // Compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    // Compile the fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    // Link shaders
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);
    glLinkProgram(shaderProg);

    OrthographicCamera orthoCam(
        glm::vec3(0.0f, 0.0f, 0.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f),      // Front direction
        glm::vec3(0.0f, 1.0f, 0.0f),      // Up direction
        0.0f,                             // Yaw
        0.0f,                             // Pitch
        50.0f,                            // Distance from target
        10.0f                             // Orthographic size
    );

    glm::mat4 projectionMatrix = orthoCam.GetProjectionMatrix(windowWidth / windowHeight);

    PerspectiveCamera perspectiveCam(
        glm::vec3(0.0f, 0.0f, 0.0f), // Position
        glm::vec3(0.0f, 0.0f, 0.0f), // Front dir
        glm::vec3(0.0f, 1.0f, 0.0f), // Up dir
        270.0f, // Yaw
        0.0f, // Pitch
        15.0f, // Distance
        70.0f // FOV
    );


    glEnable(GL_DEPTH_TEST);


    // Load sphere
    Model3D sphereObject(
        "3D/sphere.obj", // 3D model
        "", // texture
        glm::vec3(0, 0, 0), // Spawn pos
        glm::vec3(0, 0, 0), //Rotation
        glm::vec3(scaleVal, scaleVal, scaleVal), // Scale
        shaderProg
    );

    // Load sphere
    Model3D sphereObject2(
        "3D/sphere.obj", // 3D model
        "", // texture
        glm::vec3(0, 0, 0), // Spawn pos
        glm::vec3(0, 0, 0), //Rotation
        glm::vec3(scaleVal, scaleVal, scaleVal), // Scale
        shaderProg
    );

    // Load sphere
    Model3D sphereObject3(
        "3D/sphere.obj", // 3D model
        "", // texture
        glm::vec3(0, 0, 0), // Spawn pos
        glm::vec3(0, 0, 0), //Rotation
        glm::vec3(scaleVal, scaleVal, scaleVal), // Scale
        shaderProg
    );

    // Load sphere
    Model3D sphereObject4(
        "3D/sphere.obj", // 3D model
        "", // texture
        glm::vec3(0, 0, 0), // Spawn pos
        glm::vec3(0, 0, 0), //Rotation
        glm::vec3(scaleVal, scaleVal, scaleVal), // Scale
        shaderProg
    );

    // Load sphere
    Model3D sphereObject5(
        "3D/sphere.obj", // 3D model
        "", // texture
        glm::vec3(0, 0, 0), // Spawn pos
        glm::vec3(0, 0, 0), //Rotation
        glm::vec3(scaleVal, scaleVal, scaleVal), // Scale
        shaderProg
    );


	std::list<RenderParticle*> renderParticles;
    // --- PARTICLE SETUP USING USER INPUT ---
    //Add the particles
	Physics::PhysicsWorld physicsWorld = Physics::PhysicsWorld();

    float startX = -2 * particleGap;
    Physics::P6Particle p1, p2, p3, p4, p5;

    p1.position = Physics::MyVector(startX + 0 * particleGap, 0, 0);
    p2.position = Physics::MyVector(startX + 1 * particleGap, 0, 0);
    p3.position = Physics::MyVector(startX + 2 * particleGap, 0, 0);
    p4.position = Physics::MyVector(startX + 3 * particleGap, 0, 0);
    p5.position = Physics::MyVector(startX + 4 * particleGap, 0, 0);

    p1.mass = p2.mass = p3.mass = p4.mass = p5.mass = 1.0f;
    p1.radius = p2.radius = p3.radius = p4.radius = p5.radius = particleRadius;

    physicsWorld.AddParticle(&p1);
    physicsWorld.AddParticle(&p2);
    physicsWorld.AddParticle(&p3);
    physicsWorld.AddParticle(&p4);
    physicsWorld.AddParticle(&p5);

    RenderParticle rp1(&p1, &sphereObject, Physics::MyVector(1.0f, 0.0f, 0.0f));
    rp1.Scale = Physics::MyVector(p1.radius, p1.radius, p1.radius);
    renderParticles.push_back(&rp1);

    RenderParticle rp2(&p2, &sphereObject2, Physics::MyVector(0.0f, 0.0f, 1.0f));
    rp2.Scale = Physics::MyVector(p2.radius, p2.radius, p2.radius);
    renderParticles.push_back(&rp2);

    RenderParticle rp3(&p3, &sphereObject3, Physics::MyVector(0.0f, 0.0f, 1.0f));
    rp3.Scale = Physics::MyVector(p3.radius, p3.radius, p3.radius);
    renderParticles.push_back(&rp3);

    RenderParticle rp4(&p4, &sphereObject4, Physics::MyVector(0.0f, 0.0f, 1.0f));
    rp4.Scale = Physics::MyVector(p4.radius, p4.radius, p4.radius);
    renderParticles.push_back(&rp4);

    RenderParticle rp5(&p5, &sphereObject5, Physics::MyVector(0.0f, 0.0f, 1.0f));
    rp5.Scale = Physics::MyVector(p5.radius, p5.radius, p5.radius);
    renderParticles.push_back(&rp5);

    // --- CABLE SETUP USING USER INPUT ---
    Physics::Cable aCable(p1.position, cableLen, cableLen + 1.0f);
    physicsWorld.forceRegistry.Add(&p1, &aCable);

    Physics::Cable aCable2(p2.position, cableLen, cableLen + 1.0f);
    physicsWorld.forceRegistry.Add(&p2, &aCable2);

    Physics::Cable aCable3(p3.position, cableLen, cableLen + 1.0f);
    physicsWorld.forceRegistry.Add(&p3, &aCable3);

    Physics::Cable aCable4(p4.position, cableLen, cableLen + 1.0f);
    physicsWorld.forceRegistry.Add(&p4, &aCable4);

    Physics::Cable aCable5(p5.position, cableLen, cableLen + 1.0f);
    physicsWorld.forceRegistry.Add(&p5, &aCable5);
    // --- GRAVITY SETUP USING USER INPUT ---
    Physics::MyVector gravity(0.0f, gravityStrength, 0.0f);
    // If you have a gravity force generator, register it here.
    // Otherwise, add gravity in the update loop or as a force to each particle:
    p1.AddForce(gravity * p1.mass);
    p2.AddForce(gravity * p2.mass);
    p3.AddForce(gravity * p3.mass);
    p4.AddForce(gravity * p4.mass);
    p5.AddForce(gravity * p5.mass);

   

    //////////ANCHORED SPRING TEST//////////
    /*Physics::AnchoredSpring aSpring = Physics::AnchoredSpring(Physics::MyVector(0, 0, 0), 5.0f, 0.5f);
    physicsWorld.forceRegistry.Add(&p1, &aSpring);

    p1.AddForce(Physics::MyVector(0.6f, 0.3f, 0.0f) * 1000); // Add an initial force to the particle*/

    /*//PARTICLE SPRING TEST
	Physics::ParticleSpring pSpring = Physics::ParticleSpring(&p1, 5.0f, 1.0f);
	physicsWorld.forceRegistry.Add(&p2, &pSpring);

    Physics::ParticleSpring pSpring2 = Physics::ParticleSpring(&p2, 5.0f, 1.0f);
    physicsWorld.forceRegistry.Add(&p1, &pSpring2);

	p1.AddForce(Physics::MyVector(0.6f, 0.3f, 0.0f) * 1000); // Add an initial force to the particle*/
    //////////ANCHORED SPRING TEST//////////
	

	/*Physics::ParticleContact contact = Physics::ParticleContact();
	contact.particles[0] = &p1;
	contact.particles[1] = &p2;

	contact.contactNormal = p1.position - p2.position;
	contact.contactNormal = contact.contactNormal.Normalize(); // Normalize the contact normal vector
	contact.restitution = 0.0f; // Set restitution coefficient*/


	//p1.velocity = Physics::MyVector(0.2, 0, 0);
    //p2.velocity = Physics::MyVector(-0.1, 0, 0);

    //Physics::MyVector dir = p1.position - p2.position;
	//dir.Normalize();

	//physicsWorld.addContact(&p1, &p2, 1.0f, dir); // Add the contact to the physics world

	//Initializes the clock and variables
	using clock = std::chrono::high_resolution_clock;
	auto curr_time = clock::now();
	auto prev_time = curr_time;
	std::chrono::nanoseconds curr_ns(0);

    constexpr float fixedDeltaTime = 0.005f;

    float elapsedTime = 0.0f;
    bool forceApplied = false;

    while (!glfwWindowShouldClose(window)) {

		curr_time = clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
		prev_time = curr_time;

		curr_ns += dur;

        float frameTime = std::chrono::duration<float>(dur).count();
        elapsedTime += frameTime;

        if (curr_ns >= timeStep) {
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
			//std::cout << "MS: " << (float)ms.count() << "\n";

            curr_ns -= timeStep;
			//std::cout << "P6Update"<< "\n";

			physicsWorld.Update((float)ms.count() / 1000.0f);  

        }

        if (!forceApplied && spacePressed) {
            p1.AddForce(Physics::MyVector(Physics::MyVector(applyForceX, applyForceY, applyForceZ) * 1000));
            forceApplied = true;
            std::cout << "Force applied to cable!" << std::endl;
        }

        //std::cout << "Normal Update" << "\n";

        ProcessInput(); // Key inputs

        float aspect = windowWidth / windowHeight;
        projectionMatrix = usePerspective ? perspectiveCam.GetProjectionMatrix(aspect) : orthoCam.GetProjectionMatrix(aspect);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*if (!particle.IsDestroyed()) {
            sphereObject.position = (glm::vec3)particle.position;
            sphereObject.draw();
		}

        if (!particle2.IsDestroyed()) {
            sphereObject2.position = (glm::vec3)particle2.position;
            sphereObject2.draw();
        }

        if (!particle3.IsDestroyed()) {
            sphereObject3.position = (glm::vec3)particle3.position;
            sphereObject3.draw();
        }

        if (!particle4.IsDestroyed()) {
            sphereObject4.position = (glm::vec3)particle4.position;
            sphereObject4.draw();
        }*/

        // Calculate camera position based on spherical coordinates (orbit logic)
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);

        float x = camRadius * cos(pitchRad) * cos(yawRad);
        float y = camRadius * sin(pitchRad);
        float z = camRadius * cos(pitchRad) * sin(yawRad);

        glm::vec3 camPos(x, y, z);
        glm::vec3 camTarget(0.0f, 0.0f, 0.0f); // Always look at the center
        glm::vec3 camUp(0.0f, 1.0f, 0.0f);

        // Use orbit logic for both camera modes
        glm::mat4 viewMatrix = glm::lookAt(camPos, camTarget, camUp);


        // Pass the view matrix to the shader
        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUseProgram(shaderProg);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // Pass the projection matrix to the shader
        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        for (std::list<RenderParticle*>::iterator i = renderParticles.begin(); i != renderParticles.end(); i++) {
            (*i)->Draw();
        }

        // draw the bungee and cable lines
        {
            glm::vec3 lineColor(1.0f, 1.0f, 1.0f);
            DrawLine(sphereObject.position * 0.1f, ToGlmVec3(aCable.anchorPoint) * 0.1f, lineColor);
            DrawLine(sphereObject2.position * 0.1f, ToGlmVec3(aCable2.anchorPoint) * 0.1f, lineColor);
            DrawLine(sphereObject3.position * 0.1f, ToGlmVec3(aCable3.anchorPoint) * 0.1f, lineColor);
            DrawLine(sphereObject4.position * 0.1f, ToGlmVec3(aCable4.anchorPoint) * 0.1f, lineColor);
            DrawLine(sphereObject5.position * 0.1f, ToGlmVec3(aCable5.anchorPoint) * 0.1f, lineColor);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
