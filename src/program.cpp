#include "program.h"

namespace vc {

Program::Program(Device &device, const char *fileName, std::vector<ResourceType> resourceTypes) : Device(device)
{
    //std::ifstream fin(fileName, std::ifstream::ate);
    //size_t byteLength = fin.tellg();
    //fin.seekg(0, std::ifstream::beg);
    //char *data = new char[byteLength];
    //fin.read(data, byteLength);
    //fin.close();

	size_t byteLength;
	void* data;
	FILE* fp = fopen(fileName, "rb");
	if (!fp) {
		throw ERROR_SHADER;
	}

	fseek(fp, 0L, SEEK_END);
	byteLength = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	data = malloc(byteLength);
	int ret = fread(data, byteLength, 1, fp);
	if (!ret) {
		throw ERROR_SHADER;
	}

	fclose(fp);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    shaderModuleCreateInfo.codeSize = byteLength;
    shaderModuleCreateInfo.pCode = (uint32_t *) data;
    if (VK_SUCCESS != vkCreateShaderModule(this->device, &shaderModuleCreateInfo, nullptr, &shaderModule)) {
        throw ERROR_SHADER;
    }

    VkDescriptorSetLayoutBinding *bindings = new VkDescriptorSetLayoutBinding[resourceTypes.size()];
    for (uint32_t i = 0; i < resourceTypes.size(); i++) {
        bindings[i].binding = i;
        bindings[i].descriptorType = (VkDescriptorType) resourceTypes[i];
        bindings[i].descriptorCount = 1;
        bindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        bindings[i].pImmutableSamplers = nullptr;
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    descriptorSetLayoutCreateInfo.pBindings = bindings;
    descriptorSetLayoutCreateInfo.bindingCount = resourceTypes.size();
    if (VK_SUCCESS != vkCreateDescriptorSetLayout(this->device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout)) {
        throw ERROR_SHADER;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    if (VK_SUCCESS != vkCreatePipelineLayout(this->device,&pipelineLayoutCreateInfo, nullptr, &pipelineLayout)) {
        throw ERROR_SHADER;
    }

    delete [] bindings;
    delete [] data;

    VkPipelineShaderStageCreateInfo pipelineShaderInfo = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    pipelineShaderInfo.module = shaderModule;
    pipelineShaderInfo.pName = "main";
    pipelineShaderInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;

    VkComputePipelineCreateInfo pipelineInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    pipelineInfo.stage = pipelineShaderInfo;
    pipelineInfo.layout = pipelineLayout;
    if (VK_SUCCESS != vkCreateComputePipelines(this->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline)) {
        throw ERROR_DEVICES;
    }
}

void Program::bindTo(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void Program::destroy()
{
	vkDestroyShaderModule(this->device, shaderModule, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(this->device, pipelineLayout, VK_NULL_HANDLE);
	vkDestroyPipeline(this->device, pipeline, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(this->device, descriptorSetLayout, VK_NULL_HANDLE);
}

}
