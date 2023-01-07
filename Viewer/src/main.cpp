#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <iostream>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"
static int slected_item_frame = 0;
/**
 * Fields
 */
bool show_demo_window = false;
bool show_another_window = false;
glm::vec4 clear_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);
static int meshcount = 2;

/**
 * Function declarations
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char** argv)
{


	int windowWidth = 1280, windowHeight = 720;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();
	std::string path = "..\\Data\\camera.obj";
	std::shared_ptr<MeshModel> cam_model = Utils::LoadMeshModel(path);
	scene.camera_model = cam_model;
	int id = scene.GetCameraCount();
	scene.AddCameranew(std::make_shared<Camera>());
	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		RenderFrame(window, scene, renderer, io);
	}

	Cleanup(window);
	return 0;


}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	// very importent!! initialization of glad
	// https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{

	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
		glfwSetWindowAspectRatio(window, renderer.GetViewportWidth(), renderer.GetViewportHeight());
	}

	renderer.ClearColorBuffer(clear_color);
	renderer.Render(scene);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	/**
	 * MeshViewer menu
	 */
	ImGui::Begin("MeshViewer Menu");

	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					scene.AddModel(Utils::LoadMeshModel(outPath));

					free(outPath);

				}
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
				}

			}
			ImGui::EndMenu();
		}

		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);

	// TODO: Add more controls as needed

	ImGui::End();

	/**
	 * Imgui demo - you can remove it once you are familiar with imgui
	 */

	 // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		// init all variables
		static float f = 0.0f;
		static int counter = 0;
		static int scale_value = 1;	
		static bool mesh = TRUE;

		// init to scale(50,50,35) to get good scale in the scene
		static float scale_x = 50;
		static float scale_y = 50;
		static float scale_z = 35;
		static float translate_x = 0;
		static float translate_y = 0;
		static float translate_z = 0;
		static float rotate_x = 0;
		static float rotate_y = 0;
		static float rotate_z = 0;
		static bool colored = false;

		if (scene.GetModelCount() > 0) {
			MeshModel curr_mesh = scene.GetActiveModel();
			if (slected_item_frame == 0)//local frame
			{
				translate_x = curr_mesh.local_translation[0];
				translate_y = curr_mesh.local_translation[1];
				translate_z = curr_mesh.local_translation[2];
				rotate_x = curr_mesh.local_rotation[0];
				rotate_y = curr_mesh.local_rotation[1];
				rotate_z = curr_mesh.local_rotation[2];
				scale_x = curr_mesh.local_scale[0];
				scale_y = curr_mesh.local_scale[1];
				scale_z = curr_mesh.local_scale[2];
			}
			else
			{
				scale_x = curr_mesh.world_scale[0];
				scale_y = curr_mesh.world_scale[1];
				scale_z = curr_mesh.world_scale[2];
				translate_x = curr_mesh.world_translation[0];
				translate_y = curr_mesh.world_translation[1];
				translate_z = curr_mesh.world_translation[2];
			}

		}


		/////////////////////////////////// init main window///////////////////////////////////////

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (scene.GetModelCount())
		{
			static bool axis = scene.GetActiveModel().GetShowAxis();
			static bool bounding_box = scene.GetActiveModel().GetBoundedBox();
			static bool show_face_normals = scene.GetActiveModel().GetShowFaceNormals();
			static bool show_vertex_normals = scene.GetActiveModel().GetShowVertexNormals();
			{// world local tranformation selction


				const char* items[] = { "local", "world" };
				/*static const char* current_item = items[0];*/
				if (ImGui::Combo("World/Local", &slected_item_frame, items, 2))
				{
					if (scene.GetModelCount() > 0 && slected_item_frame == 0)
					{
						scene.GetActiveModel().SetWorldLocal(false);
					}
					else if (scene.GetModelCount() > 0)
					{
						scene.GetActiveModel().SetWorldLocal(true);
					}
				}
			}


			{//model selction
				if (scene.GetModelCount() > 1)
				{
					static int sliderint = 1;
					ImGui::SliderInt("Model Selction", &sliderint, 1, scene.GetModelCount());
					scene.SetActiveModelIndex(sliderint - 1);
				}

			}

			{//transformation selection
				static int selectedItem = 0;
				const char* items[] = { "Translation", "Scale", "Rotation" };
				if (ImGui::Combo("Traformation TYPE", &selectedItem, items, 3))
				{


				}
				//scaling sliders
				if (selectedItem == 1) {
					if (ImGui::SliderFloat("scale x", &scale_x, 0, 50))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalScaleMatrix(scale_x, scale_y, scale_z);

							}
							else//TODO:world
							{
								scene.GetActiveModel().SetWorldScaleMatrix(scale_x, scale_y, scale_z);
							}
						}

					}
					if (ImGui::SliderFloat("scale y", &scale_y, 0, 50))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalScaleMatrix(scale_x, scale_y, scale_z);

							}
							else//TODO:world
							{
								scene.GetActiveModel().SetWorldScaleMatrix(scale_x, scale_y, scale_z);
							}
						}

					}
					if (ImGui::SliderFloat("scale z", &scale_z, 1, 50))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalScaleMatrix(scale_x, scale_y, scale_z);

							}
							else//TODO: world
							{
								scene.GetActiveModel().SetWorldScaleMatrix(scale_x, scale_y, scale_z);
							}
						}
					}
				}

				//translation sliders
				if (selectedItem == 0) {
					if (ImGui::SliderFloat("translate x", &translate_x, -40, 40))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalTranslateMatrix(translate_x, translate_y, translate_z);
							}
							else//TODO:world
							{
								scene.GetActiveModel().SetWorldTranslateMatrix(translate_x, translate_y, translate_z);
							}
						}

					}
					if (ImGui::SliderFloat("translate y", &translate_y, -20, 20))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalTranslateMatrix(translate_x, translate_y, translate_z);
							}
							else//TODO:world
							{
								scene.GetActiveModel().SetWorldTranslateMatrix(translate_x, translate_y, translate_z);
							}
						}

					}
					if (ImGui::SliderFloat("translate z", &translate_z, -10, 10))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalTranslateMatrix(translate_x, translate_y, translate_z);
							}
							else//TODO: world
							{
								scene.GetActiveModel().SetWorldTranslateMatrix(translate_x, translate_y, translate_z);
							}
						}
					}
				}
				//rotation sliders
				if (selectedItem == 2) {
					if (ImGui::SliderFloat("rotate x", &rotate_x, -200, 200))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalRotateMatrix(rotate_x, rotate_y, rotate_z);
							}
							else//TODO:world
							{
								scene.GetActiveModel().SetWorldRotateMatrix(rotate_x, rotate_y, rotate_z);
							}
						}

					}
					if (ImGui::SliderFloat("rotate y", &rotate_y, -200, 200))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalRotateMatrix(rotate_x, rotate_y, rotate_z);
							}
							else//TODO:world
							{
								scene.GetActiveModel().SetWorldRotateMatrix(rotate_x, rotate_y, rotate_z);
							}
						}

					}
					if (ImGui::SliderFloat("rotate z", &rotate_z, -200, 200))//
					{
						if (scene.GetModelCount() != 0)
						{
							if (slected_item_frame == 0)
							{
								scene.GetActiveModel().SetLocalRotateMatrix(rotate_x, rotate_y, rotate_z);
							}
							else//TODO: world
							{
								scene.GetActiveModel().SetWorldRotateMatrix(rotate_x, rotate_y, rotate_z);
							}
						}
					}
				}

				ImGui::Checkbox("faces BoundingBox", &scene.GetActiveModel().faces_bound);
				if (ImGui::Checkbox("is_colored", &colored))
				{
					if (colored)
					{
						scene.setiscolored(1);
					}
					else
					{
						scene.setiscolored(0);
					}
				}
			//	ImGui::Checkbox("Show axis ", &scene.GetActiveModel().show_axis);
			/*	ImGui::Checkbox("Show box ", &scene.GetActiveModel().bounding_box);

				if (ImGui::Checkbox("Faces Normals", &show_face_normals))
				{
					scene.GetActiveModel().SetShowFaceNormals(!scene.GetActiveModel().GetShowFaceNormals());
				}
				if (ImGui::Checkbox("vertices Normals", &show_vertex_normals))
				{
					scene.GetActiveModel().SetShowVertexNormals(!scene.GetActiveModel().GetShowVertexNormals());
				}
			*/
			}
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

	}

	// 3. Show another simple window.
	{//camera controls window
	static int ortho_persp = 1;//true ortho false pers
	static float Near = scene.GetActiveCamera().GetzNear();
	static float Far = scene.GetActiveCamera().GetzFar();
	static float left = scene.GetActiveCamera().GetLeft();
	static float right = scene.GetActiveCamera().GetRight();
	static float top = scene.GetActiveCamera().GetTop();
	static float bottom = scene.GetActiveCamera().GetBottom();
	static float translate_x = 0;
	static float translate_y = 0;
	static float translate_z = 0;
	static float rotate_x = 0;
	static float rotate_y = 0;
	static float rotate_z = 0;
	static int slected_item_frame_camera = 0;
	static int selectedItem = 0;
	static float fovy = 0.25;
	static float aspect = 1;
	static bool draw_camera = false;
	if (slected_item_frame_camera == 0)
	{
		translate_x = scene.GetActiveCamera().GetTranslateLocx();
		translate_y = scene.GetActiveCamera().GetTranslateLocy();
		translate_z = scene.GetActiveCamera().GetTranslateLocz();
	}
	else
	{
		translate_x = scene.GetActiveCamera().GetTranslateWorx();
		translate_y = scene.GetActiveCamera().GetTranslateWory();
		translate_z = scene.GetActiveCamera().GetTranslateWorz();
	}
	ImGui::Begin("Camera Control Window");
	if (ImGui::CollapsingHeader("Cameras"))
	{
		static int camera_index = 1;
		if (scene.GetCameraCount() > 1)
		{
			static int sliderint = 1;
			ImGui::SliderInt("Camera Selction", &camera_index, 1, scene.GetCameraCount());
			scene.SetActiveCameraIndex(camera_index - 1);
		}
		if (ImGui::Button("Add Camera"))
		{
			scene.AddCameranew(std::make_shared<Camera>());
		}
		{
			ImGui::RadioButton("Perspictive", &ortho_persp, 0);
			ImGui::SameLine();
			ImGui::RadioButton("orthotographic", &ortho_persp, 1);
			ImGui::SameLine();
			if (ImGui::Checkbox("Draw Cameers", &draw_camera))
			{
				scene.SetDrawCamera(!scene.GetDrawCamera());
			}
		}
		scene.GetActiveCamera().Setorth(ortho_persp);
		if (scene.GetCameraCount())
		{
			if (!ortho_persp)
			{
				scene.GetActiveCamera().set_perspective(glm::radians(fovy));
				if (ImGui::SliderFloat("fovy", &fovy, -200, 200))
				{
					scene.GetActiveCamera().set_perspective(glm::radians(fovy));
				}
				if (ImGui::SliderFloat("Far", &Far, -10, 10))
				{
					scene.GetActiveCamera().SetzFar(Far);
					//scene.GetActiveCamera().set_perspective(glm::radians(fovy));
				}
				if (ImGui::SliderFloat("Near", &Near, -10, 10))
				{
					scene.GetActiveCamera().SetzNear(Near);
					//scene.GetActiveCamera().set_perspective(glm::radians(fovy));
				}
				if (ImGui::SliderFloat("Aspect", &aspect, 0, 1))
				{
					scene.GetActiveCamera().SetAspect(aspect);
					//scene.GetActiveCamera().set_perspective(glm::radians(fovy));
				}
			}
			else
			{
				if (ImGui::SliderFloat("Near", &Near, -100, 100))
				{
					scene.GetActiveCamera().SetzNear(Near);
				}
				if (ImGui::SliderFloat("Far", &Far, -100, 100))
				{
					scene.GetActiveCamera().SetzFar(Far);

				}
				if (ImGui::SliderFloat("Left", &left, -100, 100))
				{
					scene.GetActiveCamera().SetLeft(left);

				}
				if (ImGui::SliderFloat("Right", &right, -100, 100))
				{
					scene.GetActiveCamera().SetRight(right);

				}
				if (ImGui::SliderFloat("Top", &top, -100, 100))
				{
					scene.GetActiveCamera().SetTop(top);

				}
				if (ImGui::SliderFloat("Bottom", &bottom, -100, 100))
				{
					scene.GetActiveCamera().SetBottom(bottom);
				}

			}

		}
		static int selectedItem = 0;
		const char* items[] = { "Translation", "Rotation" };
		const char* frames[] = { "Local", "World" };
		if (ImGui::Combo("World/Local", &slected_item_frame_camera, frames, 2))
		{

		}
		if (ImGui::Combo("Traformation TYPE", &selectedItem, items, 2))
		{
		}
		if (selectedItem == 0) {
			if (ImGui::SliderFloat("translate x", &translate_x, -640, 640))//
			{
				if (scene.GetCameraCount() != 0)
				{
					if (slected_item_frame_camera == 0)
					{
						scene.GetActiveCamera().SetLocalTranslateMatrix(translate_x, translate_y, translate_z);
					}
					else//TODO:world
					{
						scene.GetActiveCamera().SetWorldTranslateMatrix(translate_x, translate_y, translate_z);
					}
				}

			}
			if (ImGui::SliderFloat("translate y", &translate_y, -320, 320))//
			{
				if (scene.GetCameraCount() != 0)
				{
					if (slected_item_frame_camera == 0)
					{
						scene.GetActiveCamera().SetLocalTranslateMatrix(translate_x, translate_y, translate_z);
					}
					else//TODO:world
					{
						scene.GetActiveCamera().SetWorldTranslateMatrix(translate_x, translate_y, translate_z);
					}
				}

			}
			if (ImGui::SliderFloat("translate z", &translate_z, -10, 10))//
			{
				if (scene.GetCameraCount() != 0)
				{
					if (slected_item_frame_camera == 0)
					{
						scene.GetActiveCamera().SetLocalTranslateMatrix(translate_x, translate_y, translate_z);
					}
					else//TODO: world
					{
						scene.GetActiveCamera().SetWorldTranslateMatrix(translate_x, translate_y, translate_z);
					}
				}
			}
		}
		if (selectedItem == 1) {
			if (ImGui::SliderFloat("rotate x", &rotate_x, -360, 360))//
			{
				if (scene.GetCameraCount() != 0)
				{
					if (slected_item_frame_camera == 0)
					{
						scene.GetActiveCamera().SetLocalRotateMatrix(rotate_x, rotate_y, rotate_z);
					}
					else//TODO:world
					{
						scene.GetActiveCamera().SetWorldRotateMatrix(rotate_x, rotate_y, rotate_z);
					}
				}

			}
			if (ImGui::SliderFloat("rotate y", &rotate_y, -100, 100))//
			{
				if (scene.GetCameraCount() != 0)
				{
					if (slected_item_frame_camera == 0)
					{
						scene.GetActiveCamera().SetLocalRotateMatrix(rotate_x, rotate_y, rotate_z);
					}
					else//TODO:world
					{
						scene.GetActiveCamera().SetWorldRotateMatrix(rotate_x, rotate_y, rotate_z);
					}
				}

			}
			if (ImGui::SliderFloat("rotate z", &rotate_z, -360, 360))//
			{
				if (scene.GetCameraCount() != 0)
				{
					if (slected_item_frame_camera == 0)
					{
						scene.GetActiveCamera().SetLocalRotateMatrix(rotate_x, rotate_y, rotate_z);
					}
					else//TODO: world
					{
						scene.GetActiveCamera().SetWorldRotateMatrix(rotate_x, rotate_y, rotate_z);
					}
				}
			}
		}
	}
	ImGui::End();
	}
}
