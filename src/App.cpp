#include "App.hpp"
#include <cstdint>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <sys/types.h>


VKAPI_ATTR vk::Bool32 VKAPI_CALL App::debugCallBack (
	vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	vk::DebugUtilsMessageTypeFlagsEXT type,
	const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData) 
{	
	
	std::cerr << "Validation Layer: Type " << vk::to_string(type) << " Msg: " << callbackData->pMessage << std::endl;
	return vk::False;

}

void App::run() {
	init();
	mainLoop();
	terminate();
}

void App::init() {
	createWindow();
	createInstance();
	setupDebugMessenger();
	pickPhysicalDevice();
	createLogicalDevice();
}

void App::mainLoop() {
	while(!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
	}
}

void App::terminate() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void App::createWindow() {
	m_props.width = 1280;
	m_props.height = 720;
	m_props.title = "Vulkan Renderer";

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window = glfwCreateWindow(m_props.width, m_props.height, m_props.title.c_str(), nullptr, nullptr);
	glfwSetKeyCallback(
		m_window,
		[](GLFWwindow* window, int key, int scanCode, int action, int mods) {
			if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
		}
	);
}

void App::createInstance() {

	std::vector<char const*> requiredLayers;

	if(g_enableValidationLayers) {
		requiredLayers.assign(g_validationLayers.begin(), g_validationLayers.end());
	}

	for(auto& layer: requiredLayers) {
		std::cout << "Enabling Layer: " << layer << "\n";
	}

	auto layerProperties = m_context.enumerateInstanceLayerProperties();

	for(const char* layer : requiredLayers) {
		bool found = false;

		for(const auto& property : layerProperties) {
			if(strcmp(property.layerName, layer) == 0) {
				found = true;
				break;
			} 
		}

		if(!found) {
			throw std::runtime_error("Required layer not supported: " + std::string(layer));
		}
	}


	vk::ApplicationInfo appInfo {};
	appInfo.pApplicationName = "Vulkan App";
	appInfo.applicationVersion = vk::makeVersion(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = vk::makeVersion(1, 0, 0);
	appInfo.apiVersion = vk::ApiVersion13;

	auto extensions = getRequiredInstanceExtensions();
		
	vk::InstanceCreateInfo createInfo {};
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
	createInfo.ppEnabledLayerNames = requiredLayers.data();
	
	m_instance = vk::raii::Instance(m_context, createInfo);
}

std::vector<const char*> App::getRequiredInstanceExtensions() {
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if(g_enableValidationLayers) {
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	for(auto& ext : extensions) {
		std::cout << "Extensions: " << ext << "\n";
	}
	
	return extensions;
}

void App::setupDebugMessenger() {
	if(!g_enableValidationLayers) {
		return;
	}

	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
	);

	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags (
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
	);

	vk::DebugUtilsMessengerCreateInfoEXT createInfo {};

	createInfo.messageSeverity = severityFlags;
	createInfo.messageType = messageTypeFlags;
	createInfo.pfnUserCallback = &debugCallBack;

	m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo);
}

void App::pickPhysicalDevice() {
	auto devices = m_instance.enumeratePhysicalDevices();

	for(auto const& device : devices) {
		bool isSuitable = true;

		if(device.getProperties().apiVersion < vk::ApiVersion13) {
			isSuitable = false;
		}
		
		bool hasGraphicsQueue = false;
		auto queueFamilies = device.getQueueFamilyProperties();

		for(auto const& q : queueFamilies) {
			if(q.queueFlags & vk::QueueFlagBits::eGraphics) {
				hasGraphicsQueue = true;
				break;
			}
		}

		isSuitable &= hasGraphicsQueue;

		auto extensions = device.enumerateDeviceExtensionProperties();

		for(auto required : g_deviceExtensions) {
			bool found = false;

			for(auto const& ext : extensions) {
				if(strcmp(ext.extensionName, required) == 0) {
					found = true;
					break;
				}
			}
			isSuitable &= found;
		}

		if(isSuitable) {
			m_physicalDevice = device;
			return;
		}
		throw std::runtime_error("failed to find a suitable GPU!");

	}

}

uint32_t App::findQueueFamilies(vk::raii::PhysicalDevice physicalDevice) {
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	for(uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
		if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			return i;
		}
	}

	throw std::runtime_error("No graphics queue family found!");
}

void App::createLogicalDevice() {
	std::vector <vk::QueueFamilyProperties> queueFamilyProps = m_physicalDevice.getQueueFamilyProperties();
}
