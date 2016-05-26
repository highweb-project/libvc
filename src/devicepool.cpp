#include "devicepool.h"
#include "vulkandebug.h"

#define ENABLE_VALIDATION false

namespace vc {

DevicePool::DevicePool()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "libvc";
	appInfo.pEngineName = "libvc";
	//appInfo.apiVersion = VK_API_VERSION;
	appInfo.apiVersion = VK_MAKE_VERSION(1,0,5);

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = NULL;
	instanceCreateInfo.pApplicationInfo = &appInfo;

    if (ENABLE_VALIDATION) {
	    std::vector<const char*> enabledExtensions = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME };
	    instanceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
	    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
	    instanceCreateInfo.enabledLayerCount = vkDebug::validationLayerCount;
	    instanceCreateInfo.ppEnabledLayerNames = vkDebug::validationLayerNames;
    }

    //VkInstanceCreateInfo instanceCreateInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    if (VK_SUCCESS != vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
        throw ERROR_INSTANCE;
    }

    if (ENABLE_VALIDATION) {
        vkDebug::setupDebugging(instance, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, NULL);
    }

    uint32_t numDevices;
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(instance, &numDevices, nullptr) || !numDevices) {
        throw ERROR_DEVICES;
    }

    VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[numDevices];
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(instance, &numDevices, physicalDevices)) {
        throw ERROR_DEVICES;
    }

    for (uint32_t i = 0; i < numDevices; i++) {
        devices.push_back(Device(physicalDevices[i]));
    }

    delete [] physicalDevices;
}

std::vector<Device> &DevicePool::getDevices()
{
    return devices;
}

VkInstance &DevicePool::getInstance()
{
    return instance;
}

}
