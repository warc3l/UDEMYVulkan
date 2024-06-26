//
// Created by Marcel Vilalta i Soler on 20/4/24.
//

#include "VulkanRenderitzar.h"

VulkanRenderitzar::VulkanRenderitzar() {

}

int VulkanRenderitzar::init(GLFWwindow* newWindow) {
    window = newWindow;

    try {
        crearInstancia();
        createSurface(); // We have access to the "surface" to draw to
        getPhysicalDevice(); // Both checks depending on the surface.
        createLogicalDevice(); // We have the device to draw to
        createSwapChain(); // We can have access to the images which we can keep switching out to the screen
        createRenderPass(); // We have something commands to draw to
        createDescriptorSetLayout();
        createPushConstantRange();
        createGraphicsPipeline(); // Our graphics pipeline to ready to draw
        createColorBufferImage();
        createDepthBufferImage();
        createFramebuffers();
        createCommandPool();
        // int firstTexture = createTextureImage("giraffe.jpg");
        createTextureSampler();
        createCommandBuffers();
        createTextureSampler();
//        allocateDynamicBufferTransferSpace();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
//        recordCommands();
        createInputDescriptorSets();
        createSynchronization();

        uboViewProjection.projection = glm::perspective(glm::radians(45.0f), (float) swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 100.0f );
        uboViewProjection.view = glm::lookAt(glm::vec3(35.0f, 50.0f, 0.0f), glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        uboViewProjection.projection[1][1] *= -1;
        /*
        // Vertex Data
        std::vector<Vertex> meshVertices = {
                { { -0.4, 0.4, 0.0 },{ 1.0f, 0.0f, 0.0f }, {1.0f, 1.0f} },	// 0
                { { -0.4, -0.4, 0.0 },{ 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f} },	    // 1
                { { 0.4, -0.4, 0.0 },{ 1.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },    // 2
                { { 0.4, 0.4, 0.0 },{ 1.0f, 0.0f, 0.0f },  {0.0f, 1.0f}},   // 3
        };

        // Index Data
        std::vector<uint32_t> meshIndices = {
                0, 1, 2,
                2, 3, 0
        };

        std::vector<Vertex> secondVertices = {
                { { -0.25, 0.6, 0.0 },{ 0.0f, 0.0f, 1.0f }, {1.0f, 1.0f}  },	// 0
                { { -0.25, -0.6, 0.0 },{ 0.0f, 0.0f, 1.0f }, {1.0f, 0.0f} },	    // 1
                { { 0.25, -0.6, 0.0 },{ 0.0f, 0.0f, 1.0f }, {0.0f, 0.0f} },    // 2
                { { 0.25, 0.6, 0.0 },{ 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}},   // 3
        };

        // Index Data
        std::vector<uint32_t> secondMeshIndices = {
                0, 1, 2,
                2, 3, 0
        };

        // Graphics queue are also transfer queues
        Mesh firstMesh = Mesh(mainDevice.physicalDevice, mainDevice.logicalDevice,  graphicsQueue, graphicsCommandPool, &meshVertices, &meshIndices,
                              createTexture("giraffe.jpg"));
        Mesh secondMesh =  Mesh(mainDevice.physicalDevice, mainDevice.logicalDevice,  graphicsQueue, graphicsCommandPool, &secondVertices, &secondMeshIndices,
                                createTexture("giraffe.jpg"));
        meshList.push_back(firstMesh);
        meshList.push_back(secondMesh);
        */

//        glm::mat4 meshModelMatrix = meshList[0].getModel().model;
//        meshModelMatrix = glm::rotate(meshModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//        meshList[0].setModel(meshModelMatrix);


        createTexture("giraffe.jpg");

    } catch(const std::runtime_error & e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void VulkanRenderitzar::createFramebuffers() {
    swapChainFrameBuffers.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainFrameBuffers.size(); i++) {

        std::array<VkImageView, 3> attachments  = {
            swapChainImages[i].imageView,
            colorBufferImageView[i],
            depthBufferImageView[i]
        };

        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass; // Render pass layout the framebuffer will be used with
        framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCreateInfo.pAttachments = attachments.data(); // The actual attachment
        framebufferCreateInfo.width = swapChainExtent.width;
        framebufferCreateInfo.height = swapChainExtent.height;
        framebufferCreateInfo.layers = 1;

        // Everything is 1-1 connection
        VkResult result = vkCreateFramebuffer(mainDevice.logicalDevice, &framebufferCreateInfo, nullptr, &swapChainFrameBuffers[i]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the framebuffer due to " +  std::to_string(result));
        }

    }
}


int VulkanRenderitzar::createTexture(std::string filename) {
    int textureImageLoc = createTextureImage(filename);

    VkImageView imageView = createImageView(textureImages[textureImageLoc], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
    textureImageViews.push_back(imageView);

    // Descriptor set, we need to create it here
    // Create Texture Descriptor
    int descriptorLoc = createTextureDescriptor(imageView);

    // Return location of set with texture
    return descriptorLoc;
}

int VulkanRenderitzar::createTextureImage(const std::string& filename) {
    int width;
    int height;

    VkDeviceSize imageSize;
    stbi_uc * imgData = loadTexture(filename, &width, &height, &imageSize);

    VkBuffer imageStagingBuffer;
    VkDeviceMemory imageStageBufferMemory;
    createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &imageStagingBuffer, &imageStageBufferMemory);


    void *data;
    vkMapMemory(mainDevice.logicalDevice, imageStageBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, imgData, static_cast<size_t>(imageSize));
    vkUnmapMemory(mainDevice.logicalDevice, imageStageBufferMemory);

    stbi_image_free(imgData);

    VkImage texImage;
    VkDeviceMemory texImageMemory;
    texImage = createImage(width, height,
                           VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texImageMemory);

    transitionImageLayout(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool,
                          texImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


    copyImageBuffer(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, imageStagingBuffer, texImage, width, height);

    transitionImageLayout(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, texImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    textureImages.push_back(texImage);
    textureImageMemory.push_back(texImageMemory);

    vkDestroyBuffer(mainDevice.logicalDevice, imageStagingBuffer, nullptr);
    vkFreeMemory(mainDevice.logicalDevice, imageStageBufferMemory, nullptr);

    // Return index of new texture image
    return textureImages.size() - 1;
}

stbi_uc * VulkanRenderitzar::loadTexture(const std::string& filename, int* width, int *height, VkDeviceSize * imageSize) {
    int channels;
    std::string fileLoc = "../Textures/" + filename;
    stbi_uc* image = stbi_load(fileLoc.c_str(), width, height, &channels, STBI_rgb_alpha);

    if (!image) {
        throw std::runtime_error("Failed to load the image");
    }

    *imageSize = *width * *height * 4;

    return image;
}


void VulkanRenderitzar::createRenderPass() {

    std::array<VkSubpassDescription, 2> subpasses = {};

    // Color attachment of the redner pass
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = chooseSupportedFormat({ VK_FORMAT_R8G8B8A8_UNORM },
                                                   VK_IMAGE_TILING_OPTIMAL,
                                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear operation, like GLSL, between before rendering
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp =  VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // Framebuffer will be stored as an image, but image can have different layouts to give optimal operartions
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // We will not have any particular layout.
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // The source of the PRESENTATION. Image data layout after the render pass (what we need to change to)

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = chooseSupportedFormat( {VK_FORMAT_D32_SFLOAT_S8_UINT,  VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT},
                                                    VK_IMAGE_TILING_OPTIMAL,
                                                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    // Attachment reference uses an attachment index that refers to the index in the renderPassCreateInfo

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 1;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // It is the middle layout between initial and final

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 2; // Reference of the colorAttachhemtn
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

//    VkSubpassDescription subpass = {};
//    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Pipeline type subpass is to
//    subpass.colorAttachmentCount = 1;
//    subpass.pColorAttachments = &colorAttachmentReference;
//    subpass.pDepthStencilAttachment = &depthAttachmentReference;
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].colorAttachmentCount = 1;
    subpasses[0].pColorAttachments = &colorAttachmentReference;
    subpasses[0].pDepthStencilAttachment = &depthAttachmentReference;


    // Threading... implicit layout ransiations
    // Need to determine when layout transitions occur using subpass dependencies.
    // What would happen between the two points when doing the transition between passes/layout?
    // We have the dependencies.

    // We need to have it as we have an external subpass, basically, the external subpass. Before the first and the second
    // we neeed to make sure that it is converted properly to the proper layout type

    VkAttachmentDescription swapChainColorAttachment = {};
    swapChainColorAttachment.format = swapChainImageFormat;
    swapChainColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    swapChainColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    swapChainColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    swapChainColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapChainColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    swapChainColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    swapChainColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference swapChainColorAttachmentReference = {};
    swapChainColorAttachmentReference.attachment = 0;
    swapChainColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::array<VkAttachmentReference, 2> inputReferences = {};
    inputReferences[0].attachment = 1;
    inputReferences[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    inputReferences[1].attachment = 2;
    inputReferences[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Sub-pass 2
    subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[1].colorAttachmentCount = 1;
    subpasses[1].pColorAttachments = &swapChainColorAttachmentReference;
    subpasses[1].inputAttachmentCount = static_cast<uint32_t>(inputReferences.size());
    subpasses[1].pInputAttachments = inputReferences.data();

    std::array<VkSubpassDependency, 3> subPassDependencies = {};

    // Conversion from VK_IMAGE_LAYOUT_UNDEFIND to VK_IMAGE_LAYOUT_COLORATTACHMENT_OPTIONAL

    // First transition:
    // -------
    // Transition must happen after this:
    subPassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subPassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subPassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    // Transistion must happen before this:
    subPassDependencies[0].dstSubpass = 0;
    subPassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subPassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subPassDependencies[0].dependencyFlags = 0;

    subPassDependencies[1].srcSubpass = 0;
    subPassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subPassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subPassDependencies[1].dstSubpass = 1;
    subPassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    subPassDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    subPassDependencies[1].dependencyFlags = 0;


    // Second transition:
    // -------
    // Transition must happen after this:
    subPassDependencies[2].srcSubpass = 0;
    subPassDependencies[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subPassDependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    // Transistion must happen before this:
    subPassDependencies[2].dstSubpass = VK_SUBPASS_EXTERNAL;
    subPassDependencies[2].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subPassDependencies[2].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subPassDependencies[2].dependencyFlags = 0;

    std::array<VkAttachmentDescription, 3> renderPassAttachments = { swapChainColorAttachment,  colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(renderPassAttachments.size());
    renderPassCreateInfo.pAttachments = renderPassAttachments.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassCreateInfo.pSubpasses = subpasses.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subPassDependencies.size());
    renderPassCreateInfo.pDependencies = subPassDependencies.data();

    VkResult result = vkCreateRenderPass(mainDevice.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot create render pass due to " + std::to_string(result));
    }


}

void VulkanRenderitzar::updateModel(int modelId, glm::mat4 newModel) {
    if (modelId >= meshList.size()) return;
    meshList[modelId].setModel(newModel);
}

void VulkanRenderitzar::updateUniformBuffers(uint32_t imageIndex) {
//    for (size_t i = 0; i < vpUniformBufferMemory.size(); i++) {
    void *data;
    vkMapMemory(mainDevice.logicalDevice, vpUniformBufferMemory[imageIndex], 0, sizeof(UboViewProjection), 0, &data);
    memcpy(data, &uboViewProjection, sizeof(UboViewProjection));
    vkUnmapMemory(mainDevice.logicalDevice, vpUniformBufferMemory[imageIndex]);
//     }

    // Copy the model data
//    for (size_t i = 0; i < meshList.size(); i++) {
//        UboModel* thisModel = (UboModel*)((uint64_t) modelTransferSpace + (i* modelUniformAlignment));
//        *thisModel = meshList[i].getModel(); // Copy accross the data through a pointer.
//    }

//    vkMapMemory(mainDevice.logicalDevice, mDUniformBufferMemory[imageIndex], 0, modelUniformAlignment * meshList.size(), 0, &data);
//    memcpy(data, modelTransferSpace, modelUniformAlignment * meshList.size());
//    vkUnmapMemory(mainDevice.logicalDevice, mDUniformBufferMemory[imageIndex]);
}

void VulkanRenderitzar::createPushConstantRange() {
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(UboModel);

}


void VulkanRenderitzar::createDescriptorSetLayout() {

    // UboViewProjection Binding info
    VkDescriptorSetLayoutBinding vpLayoutBinding = {};
    vpLayoutBinding.binding = 0;
    vpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vpLayoutBinding.descriptorCount = 1;
    vpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vpLayoutBinding.pImmutableSamplers = nullptr;

//    VkDescriptorSetLayoutBinding modelLayoutBinding = {};
//    modelLayoutBinding.binding = 1;
//    modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//    modelLayoutBinding.descriptorCount = 1;
//    modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//    modelLayoutBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { vpLayoutBinding /* , modelLayoutBinding */ };

    // Create Descriptor Set layout with given binding
    VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = static_cast<int32_t>(layoutBindings.size());
    layoutCreateInfo.pBindings = layoutBindings.data();

    VkResult result = vkCreateDescriptorSetLayout(mainDevice.logicalDevice, &layoutCreateInfo, nullptr, &descriptorSetLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot create descriptror due to " + std::to_string(result));
    }

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo textureLayoutCreateInfo = {};
    textureLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    textureLayoutCreateInfo.bindingCount = 1;
    textureLayoutCreateInfo.pBindings = &samplerLayoutBinding;

    result = vkCreateDescriptorSetLayout(mainDevice.logicalDevice, &textureLayoutCreateInfo, nullptr, &samplerSetLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a descriptorset layout due to " + std::to_string(result));
    }

    VkDescriptorSetLayoutBinding colourInputLayoutBinding = {};
    colourInputLayoutBinding.binding = 0;
    colourInputLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    colourInputLayoutBinding.descriptorCount = 1;
    colourInputLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding depthInputLayoutBinding = {};
    depthInputLayoutBinding.binding = 1;
    depthInputLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    depthInputLayoutBinding.descriptorCount = 1;
    depthInputLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<VkDescriptorSetLayoutBinding> inputBindings = { colourInputLayoutBinding, depthInputLayoutBinding };

    VkDescriptorSetLayoutCreateInfo inputLayoutCreateInfo = {};
    inputLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    inputLayoutCreateInfo.bindingCount = static_cast<uint32_t>(inputBindings.size());
    inputLayoutCreateInfo.pBindings = inputBindings.data();

    result = vkCreateDescriptorSetLayout(mainDevice.logicalDevice, &inputLayoutCreateInfo, nullptr, &inputSetLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a descriptor set Layout due to " + std::to_string(result));
    }



}

void VulkanRenderitzar::createDescriptorSets() {
    descriptorSets.resize(swapChainImages.size());

    std::vector<VkDescriptorSetLayout> setLayouts(swapChainImages.size(), descriptorSetLayout);

    VkDescriptorSetAllocateInfo setAllocateInfo = {};
    setAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocateInfo.descriptorPool = descriptorPool;
    setAllocateInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
    setAllocateInfo.pSetLayouts = setLayouts.data();

    // Allocate descriptor sets (multiple)
    VkResult result = vkAllocateDescriptorSets(mainDevice.logicalDevice, &setAllocateInfo, descriptorSets.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set " + std::to_string(result));
    }

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkDescriptorBufferInfo vpBufferInfo = {};
        vpBufferInfo.buffer = vpUniformBuffer[i];
        vpBufferInfo.offset = 0;
        vpBufferInfo.range = sizeof(UboViewProjection);

        VkWriteDescriptorSet vpSetWrite = {};
        vpSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        vpSetWrite.dstSet = descriptorSets[i];
        vpSetWrite.dstBinding = 0;
        vpSetWrite.dstArrayElement = 0;
        vpSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vpSetWrite.descriptorCount = 1;
        vpSetWrite.pBufferInfo = &vpBufferInfo;

        // MODEL DESCRIPTOR
        // Model Buffer Binding Info
//        VkDescriptorBufferInfo modelBufferInfo = {};
//        modelBufferInfo.buffer = mDUniformBuffer[i];
//        modelBufferInfo.offset = 0;
//        modelBufferInfo.range = modelUniformAlignment;

//        VkWriteDescriptorSet modelSetWrite = {};
//        modelSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        modelSetWrite.dstSet = descriptorSets[i];
//        modelSetWrite.dstBinding = 1;
//        modelSetWrite.dstArrayElement = 0;
//        modelSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//        modelSetWrite.descriptorCount = 1;
//        modelSetWrite.pBufferInfo = &modelBufferInfo;

        // List od descriptors
        std::vector<VkWriteDescriptorSet> setWrites = { vpSetWrite };

        vkUpdateDescriptorSets(mainDevice.logicalDevice, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 0, nullptr);
    }


}

void VulkanRenderitzar::createUniformBuffers () {
    VkDeviceSize vpBufferSize = sizeof(UboViewProjection);

//    VkDeviceSize modelBufferSize = modelUniformAlignment * MAX_OBJECTS;

    vpUniformBuffer.resize(swapChainImages.size());
    mDUniformBuffer.resize(swapChainImages.size());

    vpUniformBufferMemory.resize(swapChainImages.size());
    mDUniformBufferMemory.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, vpBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vpUniformBuffer[i], &vpUniformBufferMemory[i]);

//        createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, modelBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &mDUniformBuffer[i], &mDUniformBufferMemory[i]);

    }

}

VkFormat VulkanRenderitzar::chooseSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags) {
    // Loop through the options
    for (VkFormat format: formats) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(mainDevice.physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags) == featureFlags) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & featureFlags) == featureFlags) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find a matching format!");
}

void VulkanRenderitzar::createDepthBufferImage() {
    depthBufferImage.resize(swapChainImages.size());
    depthBufferImageView.resize(swapChainImages.size());
    depthBufferImageMemory.resize(swapChainImages.size());


    VkFormat depthFormat = chooseSupportedFormat( {VK_FORMAT_D32_SFLOAT_S8_UINT,  VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT},
                                                  VK_IMAGE_TILING_OPTIMAL,
                                                  VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    // Create Depth Buffer image
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        depthBufferImage[i] = createImage(swapChainExtent.width,
                                       swapChainExtent.height,
                                       depthFormat,
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                       &depthBufferImageMemory[i]);
    }

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        depthBufferImageView[i] = createImageView(depthBufferImage[i], depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

}

VkImage VulkanRenderitzar::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags, VkDeviceMemory *imageMemory)
{
    // CREATE the IMAGE
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = useFlags;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


    // CREATE MEMORY FOR IMAGE
    VkImage image;
    VkResult  result = vkCreateImage(mainDevice.logicalDevice, &imageCreateInfo, nullptr, &image);

    if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create an image");
    }

    // CREATE MEMORY FOR IMAGE
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(mainDevice.logicalDevice, image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(mainDevice.physicalDevice, memoryRequirements.memoryTypeBits, propFlags);

    result = vkAllocateMemory(mainDevice.logicalDevice, &memoryAllocateInfo, nullptr, imageMemory);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate memory for image!");
    }

    // Connect
    vkBindImageMemory(mainDevice.logicalDevice, image, *imageMemory, 0);


    return image;
}


void VulkanRenderitzar::recordCommands(uint32_t currentImage) {
    // Information about how to begin each command buffer

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // If one command is already in the queue, we can re-submit the same to the queue

    // Information about how to begin a render pass, only for graphical applications
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset = {0,0};
    renderPassBeginInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 3> clearValues = {};

    clearValues[0].color= {0.0f, 0.0f, 0.0f, 0.0f};
    clearValues[1].color= {0.6f, 0.65f, 0.4, 1.0f};
    clearValues[2].depthStencil.depth = 1.0f;

    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size()); // List of clear values, TODO depth attachment, after
    renderPassBeginInfo.pClearValues = clearValues.data();

//    for (size_t i = 0 ; i <commandBuffers.size(); i++) {
//        // Every framebuffer must have its own command buffer.
//        renderPassBeginInfo.framebuffer = swapChainFrameBuffers[i];
//
//        // Start to recording commands to the command buffer, that's cool!
//        VkResult result = vkBeginCommandBuffer(commandBuffers[i], &bufferBeginInfo);
//        if (result != VK_SUCCESS) {
//            throw std::runtime_error("Failed to start the recording a command buffer " + std::to_string(result));
//        }
//
//            // We need the vkCmdBeginRenderPass();
//            // This is INSIDE the Begin Commad Buffer, that's the reason of the tabulation
//            vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            renderPassBeginInfo.framebuffer = swapChainFrameBuffers[currentImage];

            VkResult result = vkBeginCommandBuffer(commandBuffers[currentImage], &bufferBeginInfo);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to start a recording command " + std::to_string(result));
            }

                vkCmdBeginRenderPass(commandBuffers[currentImage], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);



                // We can have multiple commands. Let's bin in the pass render
                    vkCmdBindPipeline(commandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

                    // We can specify ANOTHER draw here...
                    // Interesting for Deferred shading
                    // vkCmdBindPipeline(kdsfsd, ... );

                    for (size_t j = 0; j < meshList.size(); j++) {
                        MeshModel thisModel = meshList[j];

                        vkCmdPushConstants(
                                commandBuffers[currentImage],
                                pipelineLayout,
                                VK_SHADER_STAGE_VERTEX_BIT,
                                0,
                                sizeof (UboModel),
                                &thisModel.getModel() );

                        for (size_t k = 0; k < thisModel.getMeshCount(); k++) {
                            VkBuffer vertexBuffer[] = { thisModel.getMesh(k)->getVertexBuffer() };
                            VkDeviceSize offsets[] = { 0 };
                            vkCmdBindVertexBuffers(commandBuffers[currentImage], 0, 1, vertexBuffer, offsets);


                            vkCmdBindIndexBuffer(commandBuffers[currentImage], thisModel.getMesh(k)->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32 );

                            // Dynamic Offset Amount
                            uint32_t dynamicOffset = static_cast<uint32_t>(modelUniformAlignment) * j;

                            vkCmdBindDescriptorSets(commandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                                    0, 1, &descriptorSets[currentImage], 0, nullptr);

                            vkCmdPushConstants( commandBuffers[currentImage], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(UboModel),	&meshList[j].getModel());


                            std::array<VkDescriptorSet, 2> descriptorSetGroup = { descriptorSets[currentImage], samplerDescriptorSets[thisModel.getMesh(k)->getTexId()] };

                            vkCmdBindDescriptorSets(commandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
                                                    0, static_cast<uint32_t>(descriptorSetGroup.size()), descriptorSetGroup.data(), 0, nullptr);

                            // Now we need to execute something, how many vertices, the instances.
                            // we can draw the objects instances, by a single draw call if the instance is loaded


                            // If we would liek to draw WITHOUT index:
                            // vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(firstMesh.getVertexCount()), 1, 0, 0);
                            vkCmdDrawIndexed(commandBuffers[currentImage], thisModel.getMesh(k)->getIndicesCount(), 1, 0, 0, 0);

                        }

                    }

                    // FOR ANOTHER OBJECT, is vkCmdDraw, another
                    // What happens for dynamic?  you would need to "re-record" the whole function, and make sure that are not
                    // re-used.

                    // Second subpass
                    vkCmdNextSubpass(commandBuffers[currentImage], VK_SUBPASS_CONTENTS_INLINE);

                    vkCmdBindPipeline(commandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, secondPipeline);
                    vkCmdBindDescriptorSets(commandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, secondPipelineLayout,
                                            0, 1, &inputDescriptorSets[currentImage], 0, nullptr);
                    vkCmdDraw(commandBuffers[currentImage], 3, 1, 0, 0);


            vkCmdEndRenderPass(commandBuffers[currentImage]);


        // Stop
        result = vkEndCommandBuffer(commandBuffers[currentImage]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to start the recording a command buffer " + std::to_string(result));
        }

//    }

}


void VulkanRenderitzar::createColorBufferImage() {
    colorBufferImage.resize(swapChainImages.size());
    colorBufferImageMemory.resize(swapChainImages.size());
    colorBufferImageView.resize(swapChainImages.size());

    VkFormat colorFormat = chooseSupportedFormat(
            { VK_FORMAT_R8G8B8A8_UNORM },
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        colorBufferImage[i] = createImage(swapChainExtent.width, swapChainExtent.height, colorFormat, VK_IMAGE_TILING_OPTIMAL,
                                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &colorBufferImageMemory[i]);

        colorBufferImageView[i] = createImageView(colorBufferImage[i], colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}


void VulkanRenderitzar::createInputDescriptorSets() {
    inputDescriptorSets.resize(swapChainImages.size());

    std::vector<VkDescriptorSetLayout> setLayouts(swapChainImages.size(), inputSetLayout);

    VkDescriptorSetAllocateInfo setAllocInfo = {};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.descriptorPool = inputDescriptorPool;
    setAllocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
    setAllocInfo.pSetLayouts = setLayouts.data();

    VkResult result = vkAllocateDescriptorSets(mainDevice.logicalDevice, &setAllocInfo, inputDescriptorSets.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed for " + std::to_string(result));
    }

    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkDescriptorImageInfo colourAttachmentDescriptor = {};
        colourAttachmentDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        colourAttachmentDescriptor.imageView = colorBufferImageView[i];
        colourAttachmentDescriptor.sampler = VK_NULL_HANDLE;
        VkWriteDescriptorSet colourWrite = {};
        colourWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        colourWrite.dstSet = inputDescriptorSets[i];
        colourWrite.dstBinding = 0;
        colourWrite.dstArrayElement = 0;
        colourWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        colourWrite.descriptorCount = 1;
        colourWrite.pImageInfo = &colourAttachmentDescriptor;

        VkDescriptorImageInfo depthAttachmentDescriptor = {};
        depthAttachmentDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        depthAttachmentDescriptor.imageView = depthBufferImageView[i];
        depthAttachmentDescriptor.sampler = VK_NULL_HANDLE;

        VkWriteDescriptorSet depthWrite = {};
        depthWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        depthWrite.dstSet = inputDescriptorSets[i];
        depthWrite.dstBinding = 1;
        depthWrite.dstArrayElement = 0;
        depthWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        depthWrite.descriptorCount = 1;
        depthWrite.pImageInfo = &depthAttachmentDescriptor;

        std::vector<VkWriteDescriptorSet> setWrites = { colourWrite, depthWrite };

        vkUpdateDescriptorSets(mainDevice.logicalDevice, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 0, nullptr);
    }
}


int VulkanRenderitzar::createMeshModel(std::string modelFile)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(modelFile, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
    if (!scene) {
        throw std::runtime_error("Failed to load model! (" + modelFile + ")");
    }

    std::vector<std::string> textureNames = MeshModel::LoadMaterials(scene);
    std::vector<int> matToTex(textureNames.size());

    for (size_t i = 0; i < textureNames.size(); i++) {
        if (textureNames[i].empty()) {
            matToTex[i] = 0;
        }
        else {
            matToTex[i] = createTexture(textureNames[i]);
        }
    }

    std::vector<Mesh> modelMeshes = MeshModel::LoadNode(mainDevice.physicalDevice, mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool,
                                                        scene->mRootNode, scene, matToTex);

    MeshModel meshModel = MeshModel(modelMeshes);
    meshList.push_back(meshModel);

    return meshList.size() - 1;
}

void VulkanRenderitzar::createSynchronization() {

    imageAvailable.resize(MAX_FRAME_DRAW);
    renderFinished.resize(MAX_FRAME_DRAW);
    drawFences.resize(MAX_FRAME_DRAW);

    // Semaphosre creation information
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Fence creation information
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAME_DRAW; i++) {
        if ( vkCreateSemaphore(mainDevice.logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailable[i]) != VK_SUCCESS ||
                vkCreateSemaphore(mainDevice.logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinished[i]) != VK_SUCCESS ||
                vkCreateFence(mainDevice.logicalDevice, &fenceCreateInfo, nullptr, &drawFences[i]) != VK_SUCCESS
        ) {
            throw std::runtime_error("Failed to create the semaphore and/or fence!!" );
        }
    }


}

void VulkanRenderitzar::draw() {

    // We need to start the fence "signaling"
    // Wait for given fence to signal open from last draw before contiuing
    vkWaitForFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    vkResetFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame]); // You are closing the next fence so the previous one can go through.

    uint32_t imageIndex;
    vkAcquireNextImageKHR(mainDevice.logicalDevice, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailable[currentFrame],  VK_NULL_HANDLE, &imageIndex);

    recordCommands(imageIndex);
    // 1. Get next available image to draw and set something to signal when we are finished with this image
    updateUniformBuffers(imageIndex);


    // 2. Submit the command buffer - we need to make sure the semaphores are per each, not by one all.
    // we need to hold and slow down.

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1; // We just need to wait one, for a certain state of the pipeline
    submitInfo.pWaitSemaphores = &imageAvailable[currentFrame];
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex]; // Command buffer to submit!
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinished[currentFrame];

    VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, drawFences[currentFrame]);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot submit the command buffer to the queue due to " + std::to_string(result));
    }

    // 3. Present image to thee screen when it has signaled finished rendering

    // FINALLYYY
    VkPresentInfoKHR presentInfoKhr = {};
    presentInfoKhr.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfoKhr.waitSemaphoreCount = 1;
    presentInfoKhr.pWaitSemaphores = &renderFinished[currentFrame];
    presentInfoKhr.swapchainCount = 1;
    presentInfoKhr.pSwapchains = &swapChain;
    presentInfoKhr.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(graphicsQueue, &presentInfoKhr);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot present the queue " + std::to_string(result));
    }

    currentFrame = (currentFrame+1) % MAX_FRAME_DRAW; // just for looping the maximum frames
}

void VulkanRenderitzar::createCommandBuffers() {
    commandBuffers.resize(swapChainFrameBuffers.size());

    VkCommandBufferAllocateInfo cbAllocInfo = {};
    cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocInfo.commandPool = graphicsCommandPool;
    cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // vkCmdExecuteCommands(buffer), - Buffer you submit directly to queue, cant be called by other buffers
                        // VK_COMMAND_BUFFER_LEVEL_SECONDARY: Buffer cant be called direclty. Can be called from ohter buffers via vkCmdBufferExecuteCommands
    cbAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    VkResult result = vkAllocateCommandBuffers(mainDevice.logicalDevice, &cbAllocInfo, commandBuffers.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate Command Buffers");
    }


}

void VulkanRenderitzar::createCommandPool() {

    QueueFamilyIndices queueFamilyIndices = getQueueFamilies(mainDevice.physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily; // Queue family type that buffers from this command pool will use
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    // Create GRAPHICS queue family command pool

    VkResult result = vkCreateCommandPool(mainDevice.logicalDevice, &poolInfo, nullptr, &graphicsCommandPool);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error of creating the command pool due to " + std::to_string(result));
    }

}

int VulkanRenderitzar::createTextureDescriptor(VkImageView textureImage) {
    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo setAllocInfo = {};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.descriptorPool = samplerDescriptorPool;
    setAllocInfo.descriptorSetCount = 1;
    setAllocInfo.pSetLayouts = &samplerSetLayout;

    VkResult result = vkAllocateDescriptorSets(mainDevice.logicalDevice, &setAllocInfo, &descriptorSet);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate Texture Descriptor Sets due to " + std::to_string(result));
    }

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;	// Image layout when in use
    imageInfo.imageView = textureImage;									// Image to bind to set
    imageInfo.sampler = textureSampler;									// Sampler to use for set

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(mainDevice.logicalDevice, 1, &descriptorWrite, 0, nullptr);

    samplerDescriptorSets.push_back(descriptorSet);

    return samplerDescriptorSets.size() - 1;
}

void VulkanRenderitzar::createTextureSampler() {
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 0.0f;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy = 16;

    VkResult result = vkCreateSampler(mainDevice.logicalDevice, &samplerCreateInfo, nullptr, &textureSampler);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create the Texture Sampler due to " + std::to_string(result));
    }

}

void VulkanRenderitzar::cleanup() {
    // WE NEED TO WAIT UNTIL IT IS POSSIBLE TO CLEAN! JUST TO NOT HAVE PENDING
    vkDeviceWaitIdle(mainDevice.logicalDevice); // It is idle, there inothing pending in the logical device

    // aligned_free... we do not have a free for alloc_aligned...
    // free(modelTransferSpace);

    for (size_t i = 0; i < meshList.size(); i++) {
        meshList[i].destroyMeshModel();
    }

    vkDestroyDescriptorPool(mainDevice.logicalDevice, inputDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mainDevice.logicalDevice, inputSetLayout, nullptr);

    vkDestroyDescriptorPool(mainDevice.logicalDevice, samplerDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mainDevice.logicalDevice, samplerSetLayout, nullptr);
    vkDestroySampler(mainDevice.logicalDevice, textureSampler, nullptr);

    for (size_t i = 0; i < textureImages.size(); i++) {
        vkDestroyImageView(mainDevice.logicalDevice, textureImageViews[i], nullptr);
        vkDestroyImage(mainDevice.logicalDevice, textureImages[i], nullptr);
        vkFreeMemory(mainDevice.logicalDevice, textureImageMemory[i], nullptr);
    }

//    vkDestroyImageView(mainDevice.logicalDevice, depthBufferImageView, nullptr);
//    vkDestroyImage(mainDevice.logicalDevice, depthBufferImage, nullptr);
//    vkFreeMemory(mainDevice.logicalDevice, depthBufferImageMemory, nullptr);
    for (size_t i = 0; i < depthBufferImage.size(); i++) {
        vkDestroyImageView(mainDevice.logicalDevice, depthBufferImageView[i], nullptr);
        vkDestroyImage(mainDevice.logicalDevice, depthBufferImage[i], nullptr);
        vkFreeMemory(mainDevice.logicalDevice, depthBufferImageMemory[i], nullptr);
    }

    for (size_t i = 0; i < colorBufferImage.size(); i++) {
        vkDestroyImageView(mainDevice.logicalDevice, colorBufferImageView[i], nullptr);
        vkDestroyImage(mainDevice.logicalDevice, colorBufferImage[i], nullptr);
        vkFreeMemory(mainDevice.logicalDevice, colorBufferImageMemory[i], nullptr);
    }


    vkDestroyDescriptorPool(mainDevice.logicalDevice, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mainDevice.logicalDevice, descriptorSetLayout, nullptr);

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vkDestroyBuffer(mainDevice.logicalDevice, vpUniformBuffer[i], nullptr);
        vkFreeMemory(mainDevice.logicalDevice, vpUniformBufferMemory[i], nullptr);

//        vkDestroyBuffer(mainDevice.logicalDevice, mDUniformBuffer[i], nullptr);
//        vkFreeMemory(mainDevice.logicalDevice, mDUniformBufferMemory[i], nullptr);

    }

//    for(size_t i = 0; i < meshList.size(); i++) {
//        meshList[i].destroyBuffers();
//    }

    for (size_t i = 0; i < MAX_FRAME_DRAW; i++) {
        vkDestroySemaphore(mainDevice.logicalDevice, renderFinished[i], nullptr);
        vkDestroySemaphore(mainDevice.logicalDevice, imageAvailable[i], nullptr);
        vkDestroyFence(mainDevice.logicalDevice, drawFences[i], nullptr);
    }
    vkDestroyCommandPool(mainDevice.logicalDevice, graphicsCommandPool, nullptr);


    for (auto framebuffer: swapChainFrameBuffers) {
        vkDestroyFramebuffer(mainDevice.logicalDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(mainDevice.logicalDevice, secondPipeline, nullptr);
    vkDestroyPipelineLayout(mainDevice.logicalDevice, secondPipelineLayout, nullptr);

    vkDestroyPipeline(mainDevice.logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(mainDevice.logicalDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(mainDevice.logicalDevice, renderPass, nullptr);

    for (auto image: swapChainImages) {
        vkDestroyImageView(mainDevice.logicalDevice, image.imageView, nullptr);
    }

    vkDestroySwapchainKHR(mainDevice.logicalDevice, swapChain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}

VulkanRenderitzar::~VulkanRenderitzar()
{

};

void VulkanRenderitzar::createDescriptorPool() {

    // Type of descriptors + how many Descriptors, not descriptor sets
    VkDescriptorPoolSize vpPoolSize = {};
    vpPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vpPoolSize.descriptorCount = static_cast<uint32_t>(vpUniformBuffer.size());


    // Model pool dynamic
//    VkDescriptorPoolSize modelPoolSize = {};
//    modelPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//    modelPoolSize.descriptorCount = static_cast<uint32_t>(mDUniformBuffer.size());

    std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {vpPoolSize };

    VkDescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());
    poolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    poolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

    VkResult result = vkCreateDescriptorPool(mainDevice.logicalDevice, &poolCreateInfo, nullptr, &descriptorPool);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a descriptor pool due to " + std::to_string(result));
    }


    VkDescriptorPoolSize samplerPoolSize = {};
    samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerPoolSize.descriptorCount = MAX_OBJECTS;

    VkDescriptorPoolCreateInfo samplerPoolCreateInfo = {};
    samplerPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    samplerPoolCreateInfo.maxSets = MAX_OBJECTS;
    samplerPoolCreateInfo.poolSizeCount = 1;
    samplerPoolCreateInfo.pPoolSizes = &samplerPoolSize;

    result = vkCreateDescriptorPool(mainDevice.logicalDevice, &samplerPoolCreateInfo, nullptr, &samplerDescriptorPool);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a Descriptor Pool!");
    }

    VkDescriptorPoolSize colourInputPoolSize = {};
    colourInputPoolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    colourInputPoolSize.descriptorCount = static_cast<uint32_t>(colorBufferImageView.size());

    VkDescriptorPoolSize depthInputPoolSize = {};
    depthInputPoolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    depthInputPoolSize.descriptorCount = static_cast<uint32_t>(depthBufferImageView.size());

    std::vector<VkDescriptorPoolSize> inputPoolSizes = { colourInputPoolSize, depthInputPoolSize };

    VkDescriptorPoolCreateInfo inputPoolCreateInfo = {};
    inputPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    inputPoolCreateInfo.maxSets = swapChainImages.size();
    inputPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(inputPoolSizes.size());
    inputPoolCreateInfo.pPoolSizes = inputPoolSizes.data();

    result = vkCreateDescriptorPool(mainDevice.logicalDevice, &inputPoolCreateInfo, nullptr, &inputDescriptorPool);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("erro create descriptor pool " + std::to_string(result));
    }

}

void VulkanRenderitzar::crearInstancia() {

    // Information of the application itself.
    // Most data here does not affect the program and it is for developer convenience,
    // just a debug-like tooling
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App"; // Directly the vulkan name of the application
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // Custom version of the application
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // Just for developer, is not functional
    appInfo.apiVersion = VK_API_VERSION_1_0; // I think that I have 1.3.280.1

    // Everything we create in Vulkan, will be through 'create info' functions
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // structure Type.
    // We can chec "which type" as we are doing "reflection" with other languages using this sType

    // The idea is to get "extension" here. Extended information, extended data.
    // createInfo.sNext = nullptr; // We are not going to use this

    // createInfo.flags, there are a lot of flags, as a bitmask to enable things
    createInfo.pApplicationInfo = &appInfo; // pXXX pointer of XXX
    // let's start with the extension because is simply to explain


    // Create list to hold instance extensions
    std::vector<const char*> instanceExtensions = std::vector<const char*>();

    uint32_t glfwExtensionCount = 0; // we will increment it, GLFW may require multiple
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Add GLFW extension to list of extensions
    for (int i = 0; i < glfwExtensionCount; i++) {
        instanceExtensions.push_back(glfwExtensions[i]);
    }
    // instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // In case of MacOS...
    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    if (!checkInstanceExtensionSupport(&instanceExtensions)) {
        // At least one of the extensions, is not supported.
        throw std::runtime_error("VkInstance does not support required extensions");
    }

//    if (!checkValidationLayerSupport()) {
//        throw std::runtime_error("Validation Layers not available");
//    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()); // Depending on the implementation, better to explicitly set this
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    // Validation Layer - per default, it will handle to the standard output.
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); // We will set this on videos later, as it is more complicated for now.
    createInfo.ppEnabledLayerNames = validationLayers.data();

    // FINALLY - Create an instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if ( result != VK_SUCCESS ) {
        throw std::runtime_error("Failed to create a Vulkan Instance with an error " + std::to_string(result));
    }


    // Before to create the Device, wee need to DESTROY and free the memory.
    // We need two devices, Physical and Logical. Logical is the interface TO the physical.



}

void VulkanRenderitzar::createGraphicsPipeline()
{
    auto vertexShaderCode = readFile("../Shaders/vert.spv");
    auto fragmentShaderCode = readFile("../Shaders/frag.spv");

    VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
    VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

    // CREATE PIPELINE
    VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
    vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderCreateInfo.module = vertexShaderModule;
    vertexShaderCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
    fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderCreateInfo.module = fragmentShaderModule;
    fragmentShaderCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderCreateInfo, fragmentShaderCreateInfo };

    // It is not an structure, it is data to make it easier to pass all as a whole (color, texture..etc)
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0; // Can bind mulitple streams of data
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // How the dta for an attribute is defined withing a vertex
    std::array<VkVertexInputAttributeDescription, 3> attributeDescription = {};


    // The binding, is the default = 0, from the GLSLin the vertex shader
    attributeDescription[0].binding = 0;
    attributeDescription[0].location = 0; // Location in shader where the data will be read from the GLSL
    attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[0].offset = offsetof(Vertex, pos);     // Where is pos defined in the Vertex structure

    attributeDescription[1].binding = 0;
    attributeDescription[1].location = 1; // Location in shader where the data will be read from the GLSL
    attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[1].offset = offsetof(Vertex, col);     // Where is color defined in the Vertex structure

    attributeDescription[2].binding = 0;
    attributeDescription[2].location = 2; // Location in shader where the data will be read from the GLSL
    attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription[2].offset = offsetof(Vertex, tex);     // Where is color defined in the Vertex structure

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescription.data();


    // INPUT ASSEMBLY
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;


    // -- VIEWPORT & SCISSOR
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0,0};
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    // Dynamic states
    std::vector<VkDynamicState> dynamicStateEnables;
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();

    // Rasterizar
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; // Between vertices, how to handle
    rasterizationStateCreateInfo.lineWidth = 1.0f;  // How thick the lines would be
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasClamp = VK_FALSE;


    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorState = {};
    // We would apply on all colors in the blending
    colorState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorState.blendEnable = VK_TRUE;
    colorState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // This is like the inversion of the alpha
    colorState.colorBlendOp = VK_BLEND_OP_ADD;

    // Summarised (VK_BLEND_FACTOR_SRC_ALPHA * newColor) + (VK_BLEND_ONE_MINUS_SRC_ALPHA * oldColour)
    colorState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorState.alphaBlendOp = VK_BLEND_OP_ADD;



    // Blending
    VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo = {};
    colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY ;
    colorBlendingCreateInfo.attachmentCount = 1;
    colorBlendingCreateInfo.pAttachments = &colorState;

    // Descriptor Sets, we need to create a pipeline descriptor.

    // PIPELINE LAYOUT

    std::array<VkDescriptorSetLayout, 2> descriptorSetLayouts = {descriptorSetLayout, samplerSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    // Create pipeline layout
    VkResult result = vkCreatePipelineLayout(mainDevice.logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout for " + std::to_string(result));
    }

    // We need to do the depth stencil steing.
    // It is kinda of complicated and we need to get images, and this.
    // we need buffers, and copy between buffers, and device memory how it works.
    // out of scope for now.

    // We need to create the Render pass, like an intermediate between the pipeline and the framebuffer
    // We then "configure" the different subpasses for that render pass.

    // For now, we are going to have just one subpass

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Existing pipline to derive from
    pipelineCreateInfo.basePipelineIndex = -1;              // or idnex of pipeline to derive from

    result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error to create the graphics pipeline due to " + std::to_string(result));
    }

    vkDestroyShaderModule(mainDevice.logicalDevice, fragmentShaderModule, nullptr);
    vkDestroyShaderModule(mainDevice.logicalDevice, vertexShaderModule, nullptr);

    // SECOND SUBPASS
    // Second pass shaders
    auto secondVertexShaderCode = readFile("../Shaders/vert_second.spv");
    auto secondFragmentShaderCode = readFile("../Shaders/frag_second.spv");

    // Build shaders
    VkShaderModule secondVertexShaderModule = createShaderModule(secondVertexShaderCode);
    VkShaderModule secondFragmentShaderModule = createShaderModule(secondFragmentShaderCode);

    // Set new shaders
    vertexShaderCreateInfo.module = secondVertexShaderModule;
    fragmentShaderCreateInfo.module = secondFragmentShaderModule;

    VkPipelineShaderStageCreateInfo secondShaderStages[] = { vertexShaderCreateInfo, fragmentShaderCreateInfo };

    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

    depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo secondPipelineLayoutCreateInfo = {};
    secondPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    secondPipelineLayoutCreateInfo.setLayoutCount = 1;
    secondPipelineLayoutCreateInfo.pSetLayouts = &inputSetLayout;
    secondPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    secondPipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    result = vkCreatePipelineLayout(mainDevice.logicalDevice, &secondPipelineLayoutCreateInfo, nullptr, &secondPipelineLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("pipeline layout error due to " + std::to_string(result));
    }

    pipelineCreateInfo.pStages = secondShaderStages;
    pipelineCreateInfo.layout = secondPipelineLayout;
    pipelineCreateInfo.subpass = 1;

    result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &secondPipeline);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot create graphcis pipeline due to " + std::to_string(result));
    }

    // Destroy second shader modules
    vkDestroyShaderModule(mainDevice.logicalDevice, secondFragmentShaderModule, nullptr);
    vkDestroyShaderModule(mainDevice.logicalDevice, secondVertexShaderModule, nullptr);

}

VkShaderModule VulkanRenderitzar::createShaderModule(const std::vector<char> & code) {
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(mainDevice.logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a shader module " + std::to_string(result));
    }

    return shaderModule;
}

void VulkanRenderitzar::createLogicalDevice() {
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    std::set<int> queueFamilyIndices = {indices.graphicsFamily, indices.presentationFamily };

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float priority = 1.0f;

//queueCreateInfos

    for (auto queueFamilyIndex : queueFamilyIndices) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;

        queueCreateInfos.push_back(queueCreateInfo);
    }


    // Informamtion to create the logical device.s
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());  // Number of Queues Create Infos
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();


    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // We do not have for now extensions for the DEVICE (but yes for the instance)
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data(); // The Device Extensions are predefined before, then, we validate that we actually have them. If not, throw errors

    VkPhysicalDeviceFeatures deviceFeatures = {};

    // Need to add now
    deviceFeatures.depthClamp = VK_TRUE;
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;



    // Creates Devices and Queues
    VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Could not create a Logical Device due to " + std::to_string(result));
    }

    // Get a reference of the Queue Family graphics to able to clean-up later
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);

    // We just a handle of the Presentation. This does not create the queue, just gets the references.
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);
}


void VulkanRenderitzar::createSurface() {
    // We can take benefit of GLFW built-in function to know which operating systme
    // we are working on

    // Crete the Surface
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot create a surface for the window due to " + std::to_string(result));
    }
}

void VulkanRenderitzar::createSwapChain() {
    SwapChainDetails swapChainDetails = getSwapChainDetails(mainDevice.physicalDevice);

    // 1. We need to choose the BEST surface format
    VkSurfaceFormatKHR surfaceFormatKhr = chooseSurfaceFormat(swapChainDetails.formats);

    // 2. Choose the Presentation mode
    VkPresentModeKHR  presentModeKhr = chooseBestPresentationMode(swapChainDetails.presentationsModes);

    // 3. Chose the SWAP IMAGE RESOLUTION
    VkExtent2D extent = chooseSwapExtent(swapChainDetails.surfaceCapabilities);

    // How many images are in the swap chain?
    uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;
    if (swapChainDetails.surfaceCapabilities.maxImageCount < imageCount > 0 && swapChainDetails.surfaceCapabilities.maxImageCount < imageCount) {
        imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfoKhr = {};
    swapchainCreateInfoKhr.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfoKhr.imageFormat = surfaceFormatKhr.format;
    swapchainCreateInfoKhr.surface = surface;
    swapchainCreateInfoKhr.imageColorSpace = surfaceFormatKhr.colorSpace;
    swapchainCreateInfoKhr.presentMode = presentModeKhr;
    swapchainCreateInfoKhr.imageExtent = extent;
    swapchainCreateInfoKhr.minImageCount = imageCount;
    swapchainCreateInfoKhr.imageArrayLayers = 1;
    swapchainCreateInfoKhr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfoKhr.preTransform = swapChainDetails.surfaceCapabilities.currentTransform;
    swapchainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfoKhr.clipped = VK_TRUE;


    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    if (indices.graphicsFamily != indices.presentationFamily) {
        uint32_t queueFamilyIndices[] = {
                (uint32_t) indices.graphicsFamily,
                (uint32_t) indices.presentationFamily
        };

        swapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfoKhr.queueFamilyIndexCount = 2;
        swapchainCreateInfoKhr.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        // Only one queue. Graphcis and Presentation family are the same here
        // This is our case here
        swapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfoKhr.queueFamilyIndexCount = 0;
        swapchainCreateInfoKhr.pQueueFamilyIndices = nullptr;
    }

    // Useful when resizing the window
    swapchainCreateInfoKhr.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapchainCreateInfoKhr, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Cannot create SwapChain due to " + std::to_string(result));
    }

    // Save these values.
    swapChainImageFormat = surfaceFormatKhr.format;
    swapChainExtent = extent;


    // Get Swap Images Count
    uint32_t swapImagesCount;
    vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapChain, &swapImagesCount, nullptr);
    std::vector<VkImage> images(swapImagesCount);
    vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapChain, &swapImagesCount, images.data());

    for (VkImage image: images) {
        // Store image handle
        SwapChainImage swapChainImage = {};
        swapChainImage.image = image;

        // CREATE IMAGE VIEW HERE
        swapChainImage.imageView = createImageView(image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        swapChainImages.push_back(swapChainImage);
    }

}

VkImageView VulkanRenderitzar::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image = image;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.format = format;
    // Similar about OpenGL swizzle
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;        // Allows remapping rgba components to other rgba values
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Subsources allow the view to view only part of image.
    viewCreateInfo.subresourceRange.aspectMask = aspectFlags; // We for now the color space. Which aspect to use
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;

    VkResult result = vkCreateImageView(mainDevice.logicalDevice, &viewCreateInfo, nullptr, &imageView);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Problem to create the image VIEW " + std::to_string(result));
    }
    return imageView;
}



// Best format is subjetive but based on the Udemy recommendation is VK_FORMAT_R8GB8A8 (omg why that many 8 and B)
VkSurfaceFormatKHR VulkanRenderitzar::chooseSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& formats)
{
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& format: formats) {
        if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR VulkanRenderitzar::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes) {
    for (const auto&presentationMode: presentationModes) {
        if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentationMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderitzar::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilitiesKhr)
{
    if ( surfaceCapabilitiesKhr.currentExtent.width != std::numeric_limits<uint32_t>::max() ) {
        return surfaceCapabilitiesKhr.currentExtent;
    }

    // Otherwise, we need to find it ourselves. It means that the value can vary
    int width, height;
    glfwGetFramebufferSize(window, &width, &height); // The actual width/height of the window

    // We need to create a new extend with the window size
    VkExtent2D newExtent = {};
    newExtent.width = static_cast<uint32_t>(width);
    newExtent.height = static_cast<uint32_t>(height);

    // We need to make sure that within the boundaries by clamping the value.
    newExtent.width = std::max(surfaceCapabilitiesKhr.minImageExtent.width, std::min(surfaceCapabilitiesKhr.maxImageExtent.width, newExtent.width));
    newExtent.height = std::max(surfaceCapabilitiesKhr.minImageExtent.height, std::min(surfaceCapabilitiesKhr.maxImageExtent.height, newExtent.height));

    return newExtent;
}




void VulkanRenderitzar::getPhysicalDevice() {
    // Enumerate Physical Devices which we can have access
    uint32_t deviceCount = 0;
    // How many instance we would like to enumerate of?
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // If no devices available...
    if (deviceCount == 0) {
        throw std::runtime_error("No GPU physical devices encountered");
    }

    // Get me the list
    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

    for (const auto& device: deviceList) {
        if (checkDeviceSuitable(device)) {
            mainDevice.physicalDevice = device;
            break;
        }
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(mainDevice.physicalDevice, &deviceProperties);

//    minUniformBufferOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;


}

void VulkanRenderitzar::allocateDynamicBufferTransferSpace() {

    // Calculate the alignment of the model data
//    modelUniformAlignment = (sizeof(UboModel) + minUniformBufferOffset - 1) & ~(minUniformBufferOffset - 1);

    // Use callloc instead of _align_malloc
//    modelTransferSpace = (UboModel*) calloc(modelUniformAlignment * MAX_OBJECTS, modelUniformAlignment);


}

bool VulkanRenderitzar::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions) {
    // We need to check twice, once for the size, and other for the allocate.

    // First, to get the amount.
    uint32_t howManyVulkansExtensionsSupport = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &howManyVulkansExtensionsSupport, nullptr);

    // Create a list of VkExtensions using the previous count.
    std::vector<VkExtensionProperties> extensions(howManyVulkansExtensionsSupport);
    vkEnumerateInstanceExtensionProperties(nullptr, &howManyVulkansExtensionsSupport, extensions.data());

    bool hasExtension;
    for (const auto& checkExtension: *checkExtensions) {
        hasExtension = false;
        for (const auto& extension: extensions) {
            // We just need to compare the names
            if (strcmp(extension.extensionName, checkExtension)==0) {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension) {
            return false;
        }
    }

    // All the extensions exist
    return true;
}

bool VulkanRenderitzar::checkDeviceExtensionSupport(VkPhysicalDevice device) {

    // How many
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if (extensionCount == 0) {
        return false; // no extensions.
    }

    // Let's get
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());


    for (auto const& deviceExtension: deviceExtensions) {
        bool hasExtension = false;
        for (const auto& extension: extensions) {
            if (strcmp(deviceExtension, extension.extensionName) == 0) {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension) {
            return false;
        }
    }

    return true;
}


bool VulkanRenderitzar::checkValidationLayerSupport() {

    //How many
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    // Let's get
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName: validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

bool VulkanRenderitzar::checkDeviceSuitable(VkPhysicalDevice device) {

    // Get the information of the device itself
//    VkPhysicalDeviceProperties deviceProperties;
//    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//
//    // Get the physical device features.
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


    // Let's check which queues are supported, and let's add a new Utility class

    QueueFamilyIndices indices = getQueueFamilies(device);

    // Support the extension?
    bool extensionSupported = checkDeviceExtensionSupport(device);

    bool swapChainValid = false;
    if (extensionSupported) {
        SwapChainDetails swapChainDetails = getSwapChainDetails(device);
        swapChainValid = !swapChainDetails.presentationsModes.empty() && !swapChainDetails.formats.empty();
    }

    checkPortabilitySubsetExtension(device);
    return indices.isValid() && extensionSupported && swapChainValid && deviceFeatures.samplerAnisotropy; // we will return on this later, for when doing queue families.
}






QueueFamilyIndices VulkanRenderitzar::getQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    // How many
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // Give me the list
    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

    // Go through each equeu family and check it has at least 1 of the required type of the queu.

    uint32_t i = 0;// it would be better to use for loop instead of a foreach one
    for (const auto& queueFamily: queueFamilyList) {
        // We are going to check if the VK GRAPHICS QUEUE is there
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // what is the index of the queue family?
            indices.graphicsFamily = i;
        }

        // We need to check the presentation family as well
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
        // Can we check both if graphics and presenations queue
        if (queueFamily.queueCount > 0 && presentationSupport) {
            indices.presentationFamily = i;
        }

        if (indices.isValid()) {
            break;
        }

        i++;
    }

    return indices;
}

SwapChainDetails VulkanRenderitzar::getSwapChainDetails(VkPhysicalDevice device) {
    SwapChainDetails result;

    // Get the surface capabilities for the given surface on the given physical device
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &result.surfaceCapabilities);

    // Let's get the Formats
    uint32_t  formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount > 0) {
        result.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, result.formats.data());
    }

    // Lastly, we need to get the presentation mode, similar way as we did
    uint32_t presentationCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);

    if (presentationCount > 0) {
        result.presentationsModes.resize(presentationCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, result.presentationsModes.data());
    }


    return result;
}


//extra
void VulkanRenderitzar::checkPortabilitySubsetExtension(VkPhysicalDevice device) {
    portabilityFeatures = {};
    portabilityFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};
    physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2.pNext = &portabilityFeatures;
    vkGetPhysicalDeviceFeatures2(device, &physicalDeviceFeatures2);
}










