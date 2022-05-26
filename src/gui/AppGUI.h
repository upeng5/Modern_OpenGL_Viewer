#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdio.h>

struct LightProperties {
	glm::vec3 position = glm::vec3(0.3f, 3.0f, 1.5f);
	glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
};

namespace AppGUI {
	void Init(GLFWwindow* window);
	// Note: Frame must be initialized in the render loop after the colors have been cleared.
	void InitFrame();
	void RenderGui(LightProperties& light_props, float& shininess, bool& is_changed, bool& cool_to_warm);
	void Destroy();
}