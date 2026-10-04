#pragma once

class PFModel;
class PFShader;

// time between current frame and last frame
float DeltaTime = 0.0f;

bool LoadTexture(const char* fileLocation, unsigned int& TextureID, const bool bUseAlpha = false, const bool bFlipVerticallyOnLoad = false);
void MouseCallback(GLFWwindow* Window, double xpos, double ypos);
void UpdateUI();
void RenderModels(std::vector<PFModel>& Models, PFShader& ModelShader);
void FramebufferSizeCallback(GLFWwindow* Window, int width, int height);
void ProcessInput(GLFWwindow* Window);
void ScrollCallback(GLFWwindow* Window, double InXOffSet, double InYOffSet);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float FOV = 45.0f;

bool bMouseInit = false;
