#pragma once
#include <pch.h>
#include "../utils/VulkanQueueFamilies.h"
#include "../utils/VulkanAlloc.h"
#include "../utils/VulkanValidation.h"

namespace vulkan
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct SwapChainFrame
	{
		VkImage image;
		VkImageView imageView;
		VkFramebuffer framebuffer;
		VkCommandBuffer commandBuffer;
		VkSemaphore imageAvailableSemaphore, renderFinishedSemaphore;
		VkFence inFlightFence;
	};

	struct SwapChainBundle 
	{
		VkSwapchainKHR swapchain;
		std::vector<SwapChainFrame> frames;
		VkFormat format;
		VkExtent2D extent;
	};

	constexpr const VkSurfaceFormatKHR& chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	constexpr const VkPresentModeKHR& chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	constexpr const VkExtent2D chooseSwapChainExtent(
		const uint32& width, 
		const uint32& height, 
		const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent =
			{
				static_cast<uint32>(width),
				static_cast<uint32>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}

	const SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}
		return details;
	}

	const std::vector<VkImage> getSwapChainImagesKHR(
		const VkDevice& logicalDevice,
		const VkSwapchainKHR& swapchain,
		uint32* imageCount,
		const VkSurfaceKHR& surface)
	{
		std::vector<VkImage> vector;
		vkGetSwapchainImagesKHR(logicalDevice, swapchain, imageCount, nullptr);
		vector.resize(*imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, swapchain, imageCount, vector.data());
		return vector;
	}

	const SwapChainBundle createSwapChain(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkSurfaceKHR& surface, const uint32& width, const uint32& height)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
		VkSurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapChainPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapChainExtent(width, height, swapChainSupport.capabilities);

		uint32 imageCount = std::clamp(swapChainSupport.capabilities.minImageCount + 1, (uint32)0, swapChainSupport.capabilities.maxImageCount);

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
		uint32 queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;						// Optional
			createInfo.pQueueFamilyIndices = nullptr;					// Optional
		}

		SwapChainBundle bundle;
		VkSwapchainKHR swapchain;
		VkResult err = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapchain);
		check_vk_result(err, "failed to create swap chain!");

		bundle.swapchain = swapchain;
		bundle.format = surfaceFormat.format;
		bundle.extent = extent;

		std::vector<VkImage> images = getSwapChainImagesKHR(logicalDevice, bundle.swapchain, &imageCount, surface);
		bundle.frames.resize(images.size());
		for (size_t i = 0; i < images.size(); ++i) 
		{
			VkComponentMapping components =
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			};
			bundle.frames[i].image = images[i];
			bundle.frames[i].imageView = memory::createImageView(logicalDevice, images[i], bundle.format, components);
		}

		VOXEL_CORE_TRACE("Vulkan swap chain created.")

		return bundle;
	}

	void destroySwapChain(const VkDevice& logicalDevice, const VkSwapchainKHR& swapchain)
	{
		vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
	}
}