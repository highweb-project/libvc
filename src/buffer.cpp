#include "buffer.h"

namespace vc {

Buffer::Buffer(Device &device, size_t byteSize, bool mappable) : Device(device)
{
    // create buffer
    VkBufferCreateInfo bufferCreateInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferCreateInfo.size = byteSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (VK_SUCCESS != vkCreateBuffer(this->device, &bufferCreateInfo, nullptr, &buffer)) {
        throw ERROR_MALLOC;
    }

    // get memory requirements
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(this->device, buffer, &memoryRequirements);

    // allocate memory for the buffer
    VkMemoryAllocateInfo memoryAllocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    //memoryAllocateInfo.memoryTypeIndex = mappable ? memoryTypeMappable : memoryTypeLocal;
	memoryAllocateInfo.memoryTypeIndex = findMemoryTypeFromRequirements(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    if (VK_SUCCESS != vkAllocateMemory(this->device, &memoryAllocateInfo, nullptr, &memory)) {
        throw ERROR_MALLOC;
    }

    // bind memory to the buffer
    if (VK_SUCCESS != vkBindBufferMemory(this->device, buffer, memory, 0)) {
        throw ERROR_MALLOC;
    }
}

void Buffer::fill(uint32_t value)
{
    //implicitCommandBuffer->begin();
    //vkCmdFillBuffer(*implicitCommandBuffer, buffer, 0, VK_WHOLE_SIZE, value);
    //implicitCommandBuffer->end();
    //submit(*implicitCommandBuffer);
    //wait();

	VkResult err;

	void* deviceMemoryPtr = nullptr;
	err = vkMapMemory(device, memory, 0, 10240, 0, &deviceMemoryPtr);

	double* testData = new double[10240];
	for (size_t i = 0; i < 10240; i++) {
		testData[i] = value;
	}

	memcpy(deviceMemoryPtr, testData, sizeof(double) * 10240);
	vkUnmapMemory(device, memory);
}

uint32_t Buffer::findMemoryTypeFromRequirements(uint32_t hostRequirements)
{
	VkPhysicalDeviceMemoryProperties props;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &props);
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
		if ((props.memoryTypes[i].propertyFlags & hostRequirements) == hostRequirements) {
			return i;
		}
	}
}

void Buffer::enqueueCopy(Buffer src, Buffer dst, size_t byteSize, VkCommandBuffer commandBuffer)
{
    VkBufferCopy bufferCopy = {0, 0, byteSize};
    vkCmdCopyBuffer(commandBuffer, src.buffer, dst.buffer, 1, &bufferCopy);
}

void Buffer::download(void *hostPtr)
{
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(this->device, buffer, &memoryRequirements);

    Buffer mappable(*this, memoryRequirements.size, memoryTypeMappable);

    implicitCommandBuffer->begin();
    enqueueCopy(*this, mappable, memoryRequirements.size, *implicitCommandBuffer);
    implicitCommandBuffer->end();
    submit(*implicitCommandBuffer);
    wait();

    memcpy(hostPtr, mappable.map(), memoryRequirements.size);
    mappable.unmap();
    mappable.destroy();
}

Buffer::operator VkBuffer()
{
    return buffer;
}

void Buffer::destroy()
{
    vkFreeMemory(device, memory, nullptr);
    vkDestroyBuffer(device, buffer, nullptr);
}

void Buffer::unmap()
{
    vkUnmapMemory(device, memory);
}

void *Buffer::map()
{
    double *pointer;
    if (VK_SUCCESS != vkMapMemory(device, memory, 0, VK_WHOLE_SIZE, 0, (void **) &pointer)) {
        throw ERROR_MAP;
    }

    return pointer;
}

}
