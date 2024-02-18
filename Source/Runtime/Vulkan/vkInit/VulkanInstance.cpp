﻿#include "VulkanInstance.h"
#include <version.h>
#include <GLFW/glfw3.h>
#include <Core/HAL/Platform/Window.h>
#include "../vkUtils/VulkanValidation.h"

namespace vkInit
{
    const std::vector<const char*> getRequiredExtensions()
    {
        uint32 glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		
#ifdef ENABLE_VALIDATION_LAYERS
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        if (!extensions.empty())
        {
            std::stringstream ss;
            for (const char* extensionName : extensions)
            {
                ss << '\t' << extensionName << '\n';
            }
            RUNTIME_TRACE("Device extensions to be requested:\n" + ss.str());
        }
#endif
        return extensions;
    }
    
    const VkInstance createInstance(const uint32& vulkanApi)
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = PROJECT_NAME;
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
        appInfo.pEngineName = PROJECT_NAME;
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
        appInfo.apiVersion = vulkanApi;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        const auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef ENABLE_VALIDATION_LAYERS
        const bool layersSupported = vkUtils::checkValidationLayerSupport();
        RUNTIME_ASSERT(layersSupported, "validation layers requested, but not available!");

        createInfo.enabledLayerCount = static_cast<uint32>(vkUtils::validationLayers.size());
        createInfo.ppEnabledLayerNames = vkUtils::validationLayers.data();
        const auto debugCreateInfo = vkUtils::populateDebugReportCreateInfo();
        createInfo.pNext = (VkDebugReportCallbackCreateInfoEXT*)&debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif
		
        VkInstance instance;
        VkResult err = vkCreateInstance(&createInfo, nullptr, &instance);
        VK_CHECK(err, "failed to create instance!");

        RUNTIME_TRACE("Vulkan instance created.");

        return instance;
    }
    
    const VkSurfaceKHR createSurface(const VkInstance& instance, const VoxelEngine::Window* window)
    {
        VkSurfaceKHR surface;

        GLFWwindow* nativePtr = (GLFWwindow*)(window->getNativeWindow());

        VkResult err = glfwCreateWindowSurface(instance, nativePtr, nullptr, &surface);
        VK_CHECK(err, "failed to create window surface!");

        RUNTIME_TRACE("Vulkan surface created.");

        return surface;
    }
}