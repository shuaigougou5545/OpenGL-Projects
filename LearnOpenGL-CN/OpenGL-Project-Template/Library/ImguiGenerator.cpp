#include "ImguiGenerator.h"

void ImguiGenerator::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 支持键盘控制
    ImGui::StyleColorsDark(); // style
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImguiGenerator::InitWindow()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImguiGenerator::DrawWindow()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
