#ifndef PIPELINE_H
#define PIPELINE_H

// needs clean-ups!

#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>

namespace vc {

class Pipeline {
private:
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkDescriptorPool descriptorPool;
    VkDevice device;

public:
    Pipeline(VkDevice device, const char *fileName, std::vector<ResourceType> resourceTypes) : device(device)
    {
        std::ifstream fin(fileName, std::ifstream::ate);
        size_t byteLength = fin.tellg();
        fin.seekg(0, std::ifstream::beg);
        char *data = new char[byteLength];
        fin.read(data, byteLength);
        fin.close();

//        Valid Usage
//        • sType must be VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO
//        • pNext must be NULL
//        • flags must be 0
//        • pCode must be a pointer to an array of codeSize
//        4
//        uint32_t values
//        • codeSize must be greater than 0
//        • codeSize must be a multiple of 4
//        • pCode must point to valid SPIR-V code, formatted and packed as described by the SPIR-V Specification v1.0
//        • pCode must adhere to the validation rules described by the Validation Rules within a Module section of the
//        SPIR-V Environment appendix
//        • pCode must declare the Shader capability
//        • pCode must not declare any capability that is not supported by the API, as described by the Capabilities section of
//        the SPIR-V Environment appendix
//        • If pCode declares any of the capabilities that are listed as not required by the implementation, the relevant feature
//        must be enabled, as listed in the SPIR-V Environment appendix
        VkShaderModule shaderModule;
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        shaderModuleCreateInfo.codeSize = byteLength;
        shaderModuleCreateInfo.pCode = (uint32_t *) data;
        if (VK_SUCCESS != vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule)) {
            throw ERROR_SHADER;
        }

        delete [] data;


//        Valid Usage
//        • sType must be VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO
//        • pNext must be NULL
//        • flags must be 0
//        • stage must be a valid VkShaderStageFlagBits value
//        • module must be a valid VkShaderModule handle
//        • pName must be a null-terminated string
//        • If pSpecializationInfo is not NULL, pSpecializationInfo must be a pointer to a valid
//        VkSpecializationInfo structure
//        • If the geometry shaders feature is not enabled, stage must not be VK_SHADER_STAGE_GEOMETRY_BIT
//        • If the tessellation shaders feature is not enabled, stage must not be VK_SHADER_STAGE_TESSELLATION_
//        CONTROL_BIT or VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
//        • stage must not be VK_SHADER_STAGE_ALL_GRAPHICS, or VK_SHADER_STAGE_ALL
//        • pName must be the name of an OpEntryPoint in module with an execution model that matches stage
//        • If the identified entry point includes any variable in its interface that is declared with the ClipDistance
//        BuiltIn decoration, that variable must not have an array size greater than
//        VkPhysicalDeviceLimits::maxClipDistances
//        • If the identified entry point includes any variable in its interface that is declared with the CullDistance
//        BuiltIn decoration, that variable must not have an array size greater than
//        VkPhysicalDeviceLimits::maxCullDistances
//        • If the identified entry point includes any variables in its interface that are declared with the ClipDistance or
//        CullDistance BuiltIn decoration, those variables must not have array sizes which sum to more than
//        VkPhysicalDeviceLimits::maxCombinedClipAndCullDistances
//        • If the identified entry point includes any variable in its interface that is declared with the SampleMask
//        BuiltIn decoration, that variable must not have an array size greater than
//        VkPhysicalDeviceLimits::maxSampleMaskWords
//        • If stage is VK_SHADER_STAGE_VERTEX_BIT, the identified entry point must not include any input variable
//        in its interface that is decorated with CullDistance
//        • If stage is VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT or VK_SHADER_STAGE_
//        TESSELLATION_EVALUATION_BIT, and the identified entry point has an OpExecutionMode instruction
//        that specifies a patch size with OutputVertices, the patch size must be greater than 0 and less than or equal
//        to VkPhysicalDeviceLimits::maxTessellationPatchSize
//        • If stage is VK_SHADER_STAGE_GEOMETRY_BIT, the identified entry point must have an
//        OpExecutionMode instruction that specifies a maximum output vertex count that is greater than 0 and less
//        than or equal to VkPhysicalDeviceLimits::maxGeometryOutputVertices
//        • If stage is VK_SHADER_STAGE_GEOMETRY_BIT, the identified entry point must have an
//        OpExecutionMode instruction that specifies an invocation count that is greater than 0 and less than or equal to
//        VkPhysicalDeviceLimits::maxGeometryShaderInvocations
//        • If stage is VK_SHADER_STAGE_GEOMETRY_BIT, and the identified entry point writes to Layer for any
//        primitive, it must write the same value to Layer for all vertices of a given primitive
//        • If stage is VK_SHADER_STAGE_GEOMETRY_BIT, and the identified entry point writes to ViewportIndex
//        for any primitive, it must write the same value to ViewportIndex for all vertices of a given primitive
//        • If stage is VK_SHADER_STAGE_FRAGMENT_BIT, the identified entry point must not include any output
//        variables in its interface decorated with CullDistance
//        • If stage is VK_SHADER_STAGE_FRAGMENT_BIT, and the identified entry point writes to FragDepth in any
//        execution path, it must write to FragDepth in all execution paths
        VkPipelineShaderStageCreateInfo pipelineShaderInfo = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        pipelineShaderInfo.module = shaderModule;
        pipelineShaderInfo.pName = "main";
        pipelineShaderInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;




//        Valid Usage
//        • descriptorType must be a valid VkDescriptorType value
//        • If descriptorType is VK_DESCRIPTOR_TYPE_SAMPLER or VK_DESCRIPTOR_TYPE_COMBINED_
//        IMAGE_SAMPLER, and descriptorCount is not 0 and pImmutableSamplers is not NULL,
//        pImmutableSamplers must be a pointer to an array of descriptorCount valid VkSampler handles
//        • If descriptorCount is not 0, stageFlags must be a valid combination of VkShaderStageFlagBits
//        values
        VkDescriptorSetLayoutBinding *bindings = new VkDescriptorSetLayoutBinding[resourceTypes.size()];
        for (int i = 0; i < resourceTypes.size(); i++) {
            bindings[i].binding = i;
            bindings[i].descriptorType = (VkDescriptorType) resourceTypes[i];
            bindings[i].descriptorCount = 1;
            bindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            bindings[i].pImmutableSamplers = nullptr;
        }

//        Valid Usage
//        • sType must be VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO
//        • pNext must be NULL
//        • flags must be 0
//        • If bindingCount is not 0, pBindings must be a pointer to an array of bindingCount valid
//        VkDescriptorSetLayoutBinding structures
        //VkDescriptorSetLayout setLayouts;
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        descriptorSetLayoutCreateInfo.pBindings = bindings;
        descriptorSetLayoutCreateInfo.bindingCount = resourceTypes.size();
        if (VK_SUCCESS != vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout)) {
            throw ERROR_SHADER;
        }



//        Valid Usage
//        • device must be a valid VkDevice handle
//        • pCreateInfo must be a pointer to a valid VkPipelineLayoutCreateInfo structure
//        • If pAllocator is not NULL, pAllocator must be a pointer to a valid VkAllocationCallbacks structure
//        • pPipelineLayout must be a pointer to a VkPipelineLayout handle
        //VkPipelineLayout pipelineLayout;
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        if (VK_SUCCESS != vkCreatePipelineLayout(device,&pipelineLayoutCreateInfo, nullptr, &pipelineLayout)) {
            throw ERROR_SHADER;
        }

//        Valid Usage
//        • device must be a valid VkDevice handle
//        • If pipelineCache is not VK_NULL_HANDLE, pipelineCache must be a valid VkPipelineCache handle
//        • pCreateInfos must be a pointer to an array of createInfoCount valid
//        VkComputePipelineCreateInfo structures
//        • If pAllocator is not NULL, pAllocator must be a pointer to a valid VkAllocationCallbacks structure
//        • pPipelines must be a pointer to an array of createInfoCount VkPipeline handles
//        • The value of createInfoCount must be greater than 0
//        • If pipelineCache is a valid handle, it must have been created, allocated or retrieved from device
//        • Each of device and pipelineCache that are valid handles must have been created, allocated or retrieved from
//        the same VkPhysicalDevice
//        • If the value of the flags member of any given element of pCreateInfos contains the VK_PIPELINE_
//        CREATE_DERIVATIVE_BIT flag, and the basePipelineIndex member of that same element is not -1, the
//        value of basePipelineIndex must be less than the index into pCreateInfos that corresponds to that element
        //VkPipeline pipeline;
        VkComputePipelineCreateInfo pipelineInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        pipelineInfo.stage = pipelineShaderInfo;
        pipelineInfo.layout = pipelineLayout;
        if (VK_SUCCESS != vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline)) {
            throw ERROR_DEVICES;
        }

        delete [] bindings;

        // how many of each type
        VkDescriptorPoolSize descriptorPoolSizes[] = {
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (uint32_t) resourceTypes.size()}
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.maxSets = 1;
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
        if (VK_SUCCESS != vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool)) {
            throw ERROR_SHADER;
        }

        // allocate the descriptor set
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        descriptorSetAllocateInfo.descriptorSetCount = 1; // 1?
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;

        if (VK_SUCCESS != vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet)) {
            throw ERROR_SHADER;
        }
    }

    VkDescriptorSet getDescriptorSet()
    {
        return descriptorSet;
    }

    VkPipelineLayout getPipelineLayout()
    {
        return pipelineLayout;
    }

    void destroy()
    {
        vkFreeDescriptorSets(device, descriptorPool, 1, &descriptorSet);
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }

    operator VkPipeline()
    {
        return pipeline;
    }
};

}

#endif // PIPELINE_H

