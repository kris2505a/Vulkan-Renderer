#pragma once
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

const std::vector<char const*> g_validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> g_deviceExtensions {
	vk::KHRSwapchainExtensionName
};

#ifdef _DEBUG 
constexpr bool g_enableValidationLayers = true;
#else
constexpr bool g_enableValidationLayers = false;
#endif // _DEBUG 

struct WindowProperties {
	int width;
	int height;
	std::string title;
};

class App {
public:
	void run();
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallBack(
			vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
			vk::DebugUtilsMessageTypeFlagsEXT type,
			const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
			void* userData
	); 

private:
	void init();
	void terminate();
	void mainLoop();

	void createWindow();
	void createInstance();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	void createLogicalDevice();

	uint32_t findQueueFamilies(vk::raii::PhysicalDevice physicalDevice);
	std::vector<const char*> getRequiredInstanceExtensions();

private:
	GLFWwindow* m_window = nullptr;
	WindowProperties m_props;
	vk::raii::Instance m_instance = nullptr;
	vk::raii::Context m_context;
	vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
	vk::raii::PhysicalDevice m_physicalDevice = nullptr;
	vk::raii::Device m_device = nullptr;
};

