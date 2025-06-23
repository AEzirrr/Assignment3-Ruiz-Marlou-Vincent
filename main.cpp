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
#include "Physics/Chain.h"
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

// Values for camera position and rotation
float x_cam = 0;
float y_cam = 0;
float z_cam = 0;

float yaw = 270;
float pitch = 0;

float scaleVal = 1.0f;

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

/*void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        keyStates[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}*/

/*void ProcessInput() {
    double currTime = glfwGetTime();

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (keyStates[GLFW_KEY_W]) {
        x_cam += front.x * camSpeed;
        y_cam += front.y * camSpeed;
        z_cam += front.z * camSpeed;
    };

    if (keyStates[GLFW_KEY_S]) {
        x_cam -= front.x * camSpeed;
        y_cam -= front.y * camSpeed;
        z_cam -= front.z * camSpeed;
    };

    if (keyStates[GLFW_KEY_A]) {
        x_cam -= right.x * camSpeed;
        y_cam -= front.y * camSpeed;
        z_cam -= right.z * camSpeed;
    };

    if (keyStates[GLFW_KEY_D]) {
        x_cam += right.x * camSpeed;
        y_cam += front.y * camSpeed;
        z_cam += right.z * camSpeed;
    };

    // Moves camera pitch and yaw based on arrow inputs (Look up and down)
    if (keyStates[GLFW_KEY_UP]) pitch += camLookSpeed;
    if (keyStates[GLFW_KEY_DOWN]) pitch -= camLookSpeed;

    if (keyStates[GLFW_KEY_LEFT]) yaw -= camLookSpeed;
    if (keyStates[GLFW_KEY_RIGHT]) yaw += camLookSpeed;
} */

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
    //glfwSetKeyCallback(window, Key_Callback);

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

    OrthographicCamera orthoCamera(
        glm::vec3(0.0f, 0.0f, 0.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f),      // Front direction
        glm::vec3(0.0f, 1.0f, 0.0f),      // Up direction
        0.0f,                             // Yaw
        0.0f,                             // Pitch
        50.0f,                            // Distance from target
        10.0f                             // Orthographic size
    );

    glm::mat4 projectionMatrix = orthoCamera.GetProjectionMatrix(windowWidth / windowHeight);

    glEnable(GL_DEPTH_TEST);


    // Load sphere
    Model3D sphereObject(
        "3D/sphere.obj", // 3D model
        "", // texture
        glm::vec3(0, 0, 0), // Spawn pos
        glm::vec3(0, 0, 0), //Rotation
        glm::vec3(0.5f, 0.5f, 0.5f), // Scale
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

    /*// Load sphere
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
    );*/


	std::list<RenderParticle*> renderParticles;

    //Add the particles
	Physics::PhysicsWorld physicsWorld = Physics::PhysicsWorld();

	/*Physics::P6Particle particle = Physics::P6Particle();
	particle.position = Physics::MyVector(-10, 0, 0);
	//particle.velocity = Physics::MyVector(10, 0, 0);
    //particle.acceleration = Physics::MyVector(4, 0, 0);

	particle.mass = 1.0f;
    particle.AddForce(Physics::MyVector(800.0f, 200.0f, 0.0f));
	Physics::DragForceGenerator drag = Physics::DragForceGenerator(0.14f, 0.1);

	physicsWorld.forceRegistry.Add(&particle, &drag); // Add the drag force generator to the particle

	particle.damping = 1.0f; 
	physicsWorld.AddParticle(&particle);
	RenderParticle rp1 = RenderParticle(&particle, &sphereObject, Physics::MyVector(0.0f, 1.0f, 0.0f));
	renderParticles.push_back(&rp1);*/

    /*Physics::P6Particle particle2 = Physics::P6Particle();
    particle2.position = Physics::MyVector(8, -8, 0);
    particle2.velocity = Physics::MyVector(-1, 1, 0);
    particle2.acceleration = Physics::MyVector(-4, 4, 0);
	physicsWorld.AddParticle(&particle2);
    sphereObject2.color(glm::vec3(0.0f, 1.0f, 0.0f));
	RenderParticle rp2 = RenderParticle(&particle2, &sphereObject2, Physics::MyVector(0.0f, 1.0f, 0.0f));
	renderParticles.push_back(&rp2);

    Physics::P6Particle particle3 = Physics::P6Particle();
    particle3.position = Physics::MyVector(-8, 8, 0);
    particle3.velocity = Physics::MyVector(1, -1, 0);
    particle3.acceleration = Physics::MyVector(4, -4, 0);
	physicsWorld.AddParticle(&particle3);
    sphereObject3.color(glm::vec3(0.0f, 1.0f, 0.0f));
	RenderParticle rp3 = RenderParticle(&particle3, &sphereObject3, Physics::MyVector(0.0f, 1.0f, 0.0f));
	renderParticles.push_back(&rp3);

    Physics::P6Particle particle4 = Physics::P6Particle();
    particle4.position = Physics::MyVector(-8, -8, 0);
    particle4.velocity = Physics::MyVector(1, 1, 0);
    particle4.acceleration = Physics::MyVector(4, 4, 0);
	physicsWorld.AddParticle(&particle4);
    sphereObject4.color(glm::vec3(0.0f, 1.0f, 0.0f));
	RenderParticle rp4 = RenderParticle(&particle4, &sphereObject4, Physics::MyVector(0.0f, 1.0f, 0.0f));
	renderParticles.push_back(&rp4);*/

	Physics::P6Particle p1 = Physics::P6Particle();
	p1.position = Physics::MyVector(-2, 0, 0);
	p1.mass = 1.0f;
	physicsWorld.AddParticle(&p1);
    RenderParticle rp1 = RenderParticle(&p1, &sphereObject, Physics::MyVector(0.0f, 0.0f, 1.0f));
    renderParticles.push_back(&rp1);
    
    Physics::P6Particle p2 = Physics::P6Particle();
	p2.position = Physics::MyVector(2.0f, 0, 0);
	p2.mass = 1.0f;
	physicsWorld.AddParticle(&p2);
	RenderParticle rp2 = RenderParticle(&p2, &sphereObject2, Physics::MyVector(1.0f, 0.0f, 0.0f));
	renderParticles.push_back(&rp2);

    //I set the bungee rest length to 3.0f just so they are on the same level when stopping
	//because due to mass, the particle is suspended and stretches the bungee a lot lower
    //////////Bungee//////////
    Physics::Bungee aBungee = Physics::Bungee(Physics::MyVector(-2, 0, 0), 5.0f, 3.0f);
	aBungee.damping = 0.1f; // Set damping for the bungee
    physicsWorld.forceRegistry.Add(&p1, &aBungee);
    //////////Bungee TEST//////////

    //////////CHAIN//////////
    Physics::Chain aChain = Physics::Chain(Physics::MyVector(2, 0, 0), 5.0f, 5.0f);
    physicsWorld.forceRegistry.Add(&p2, &aChain);
    //////////CHAIN//////////

   

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

	std::cout << "Wait 5 secs to see chain movement using force" << std::endl;
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

        // Apply force to the chain after 5 seconds (to simulate chain movement)
        if (!forceApplied && elapsedTime >= 5.0f) {
            p2.AddForce(Physics::MyVector(Physics::MyVector(0.25f, 0.25f, 0.0f) * 1000));
            forceApplied = true;
            std::cout << "Force applied to chain!" << std::endl;
        }

        //std::cout << "Normal Update" << "\n";

        //ProcessInput(); // Key inputs

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

        // Calculates the camera's position and front vector
        glm::vec3 cameraPos = glm::vec3(0 + x_cam, 0 + y_cam, 0 + z_cam);
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);

        // Calculates the view matrix
        glm::vec3 cameraCenter = cameraPos + front;
        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraCenter, glm::vec3(0, 1, 0));

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

        // draw the bungee and chain lines
        {
            glm::vec3 lineColor(1.0f, 1.0f, 1.0f);
            DrawLine(sphereObject.position * 0.1f, ToGlmVec3(aBungee.anchorPoint) * 0.1f, lineColor);
            DrawLine(sphereObject2.position * 0.1f, ToGlmVec3(aChain.anchorPoint) * 0.1f, lineColor);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
