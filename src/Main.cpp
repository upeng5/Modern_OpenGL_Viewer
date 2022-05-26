#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <memory>

#include "api/opengl/Shader.h"
#include "api/opengl/VertexArray.h"
#include "api/opengl/VertexBuffer.h"
#include "api/opengl/IndexBuffer.h"
#include "api/opengl/Texture2D.h"
#include "core/Camera.h"
#include "gui/AppGUI.h"
#include "core/Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef _DEBUG
#include "Debug.h"
#endif

/* CONSTANTS */
// Usually 640*480
const unsigned int kWidth = 1600;
const unsigned int kHeight = 900;

/* CALLBACKS */
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void ProcessInput(GLFWwindow* window);

// GLOBALS
Camera camera(glm::vec3(0.0f, 1.2f, 4.0f));
bool is_changed = false;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = kWidth / 2.0;
float lastY = kHeight / 2.0;
float fov = 45.0f;

// timing
float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

// IMGUI Control Variables
LightProperties light_props;
float shininess = 32.f;
bool cool_to_warm = false;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) {
		std::cout << "ERROR::GLFW::GLFW_INITIALIZATION_FAILED" << std::endl;
		return -1;
	}

	/* GLFW hints */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
	// The debug context must be enabled right after initializing the windowing system
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(kWidth, kHeight, "Model Viewer", nullptr, nullptr);
	if (!window)
	{
		std::cout << "ERROR::GLFW::WINDOW_CREATION_FAILED" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Initialize GLAD */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR::GLAD::FAILED_TO_INITIALIZE_OPENGL_CONTEXT" << std::endl;
		return -1;
	}

#ifdef _DEBUG
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugOutputCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else {
		std::cout << "ERROR::OPENGL::FAILED_TO_ENABLE_DEBUG_OUTPUT" << "\n" << std::endl;
	}
#endif // _DEBUG

	/* Configure global OpenGL state */
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	/* GLFW Callbacks */
	glfwSetErrorCallback(ErrorCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	/*  */
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

#ifdef _DEBUG
	std::cout << "Version:  " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Vendor:   " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << std::endl;
#endif

	/* Load shaders */
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("shaders/default.vert", "shaders/lighting/phong.frag");
	std::shared_ptr<Shader> lamp_shader = std::make_shared<Shader>("shaders/lighting/lamp.vert", "shaders/lighting/lamp.frag");

	/* Load models */
	Model external_model("res/models/AntiqueCamera/glTF/AntiqueCamera.gltf");

	/* Define vertices */
	std::vector<float> vertices = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	VertexArray vao;
	vao.Bind();
	VertexBuffer vbo(vertices);
	vbo.SetLayout({
		{ 3, GL_FLOAT, 6 }, // Position attribute
		{ 3, GL_FLOAT, 6 }, // Normal attribute
		});

	vao.Unbind();

	/* Add textures here */

	/* Define transformation matrices */
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	/* Initialize GUI */
	AppGUI::Init(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ProcessInput(window);

		// Start the ImGui frame
		AppGUI::InitFrame();

		// Check if the shader option has changed
		if (is_changed) {
			// Check the shading type
			if (cool_to_warm) {
				shader = std::make_shared<Shader>("shaders/default.vert", "shaders/lighting/cool_to_warm.frag");
			}
			else {
				shader = std::make_shared<Shader>("shaders/default.vert", "shaders/lighting/phong.frag");
			}
			is_changed = false;
		}

		/* Draw here */
		//texture.Bind(0);
		// Bind shader
		shader->Bind();
		// Lighting
		shader->SetVec3f("light.position", light_props.position);
		shader->SetVec3f("light.ambient", light_props.ambient);
		shader->SetVec3f("light.diffuse", light_props.diffuse);
		shader->SetVec3f("light.specular", light_props.specular);
		shader->SetVec3f("u_CameraPos", camera.Position);
		shader->SetFloat("u_Shininess", shininess);
		// Init transformation matrices
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.5f));

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(45.0f), (float)kWidth / (float)kHeight, 0.1f, 100.0f);

		shader->SetMat4f("u_Model", model);
		shader->SetMat4f("u_View", view);
		shader->SetMat4f("u_Projection", projection);

		// Draw model here
		external_model.Draw(*shader);

		// Draw light source
		lamp_shader->Bind();
		lamp_shader->SetVec3f("light.position", light_props.position);
		lamp_shader->SetVec3f("light.ambient", light_props.ambient);
		lamp_shader->SetVec3f("light.diffuse", light_props.diffuse);
		lamp_shader->SetVec3f("light.specular", light_props.specular);
		model = glm::mat4(1.0f);
		model = glm::translate(model, light_props.position);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		lamp_shader->SetMat4f("u_Model", model);
		lamp_shader->SetMat4f("u_View", view);
		lamp_shader->SetMat4f("u_Projection", projection);
		vao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		vao.Unbind();

		// Render GUI here
		//AppGUI::RenderGui();
		AppGUI::RenderGui(light_props, shininess, is_changed, cool_to_warm);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	/* Delete objects */
	vao.Unbind();
	vbo.Unbind();

	// Destroy window & GUI
	AppGUI::Destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void ErrorCallback(int error, const char* description)
{
	std::cerr << "ERROR::GLFW\n" << description << std::endl;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		return;
	}

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera.Front = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(camera.MovementSpeed * delta_time);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Position += cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Position -= cameraSpeed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
}