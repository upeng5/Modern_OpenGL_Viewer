#include "AppGUI.h"

#include <glm/gtc/type_ptr.hpp>

void AppGUI::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void AppGUI::InitFrame() {
	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void AppGUI::RenderGui(LightProperties& light_props, float& shininess, bool& is_changed, bool& cool_to_warm)
{
	// Create a settings window
	ImGui::Begin("Settings");
	ImGui::Spacing();
	// Lighting controls section
	ImGui::Text("Lighting Controls");
	ImGui::Spacing();
	ImGui::Separator();

	ImGui::Spacing();
	ImGui::SliderFloat3("Light Position", glm::value_ptr(light_props.position), -10.0f, 10.0f);
	ImGui::Spacing();
	ImGui::ColorEdit3("ambient", glm::value_ptr(light_props.ambient));
	ImGui::Spacing();
	ImGui::ColorEdit3("diffuse", glm::value_ptr(light_props.diffuse));
	ImGui::Spacing();
	ImGui::ColorEdit3("specular", glm::value_ptr(light_props.specular));

	// Material Control Section
	ImGui::Text("Material Controls");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::SliderFloat("Material shininess", &shininess, 0.0f, 256.f);

	// Framerate section
	ImGui::Spacing();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Separator();

	// Shading controls section
	ImGui::Spacing();
	ImGui::Text("Shader Controls (Default: Phong)");
	ImGui::Spacing();
	if (ImGui::Button("Toggle Cool To Warm Shading")) {
		is_changed = true;
		cool_to_warm = !cool_to_warm;
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AppGUI::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}