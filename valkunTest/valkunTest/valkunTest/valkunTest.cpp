#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <assert.h>
#include <vector>
#include <string>
#include <fstream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

void InitWindow();
void InitInstance();
void InitDevice();
void InitSwapchain();
void InitImageView();
void InitShaderModule();
void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
void InitShaderStage();
void InitPipelineLayout();
void InitRenderPass();
void InitGraphicsPipeline();
void InitFrameBuffer();
void UninitWindow();
void UninitGraphicsPipeline();
void UninitRenderPass();
void UninitPipelineLayout();
void UninitShaderStage();
void UninitImageView();
void UninitSwapchain();
void UninitDevice();
void UninitInstance();
void UninitFrameBuffer();

VkInstance  instance = nullptr;
VkPhysicalDevice gpu = nullptr;
VkDevice device = nullptr;
VkQueue queueHundle = nullptr;
VkSurfaceKHR surface = VK_NULL_HANDLE;
VkSwapchainKHR swapchain = nullptr;
std::vector<VkImage> swapchainImage;
std::vector<VkImageView> swapchainImageView;
VkViewport viewport;
VkRect2D scissor;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
VkPipelineShaderStageCreateInfo shader_stage_create_info_vert;
VkPipelineShaderStageCreateInfo shader_stage_create_info_frag;
VkPipelineShaderStageCreateInfo shaderStages[2];
VkPipelineVertexInputStateCreateInfo vertex_input_create_info;
VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info;
VkPipelineViewportStateCreateInfo viewport_state_create_info;
VkPipelineRasterizationStateCreateInfo rasterrization_create_info;
VkPipelineMultisampleStateCreateInfo multisample_create_info;
VkPipelineColorBlendAttachmentState colorBlendAttachment;
VkPipelineColorBlendStateCreateInfo colorblend_create_info;
VkPipelineLayoutCreateInfo pipeline_layout_create_info;
VkPipelineLayout pipelineLayout;
VkRenderPass renderPass;
VkPipeline pipeLine;
VkFramebuffer frameBuffers;

GLFWwindow* window;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main()
{
    InitWindow();
    InitInstance();
    InitDevice();
    InitSwapchain();
    InitImageView();
    InitShaderModule();
    InitShaderStage();
    InitPipelineLayout();
    InitRenderPass();
    InitGraphicsPipeline();
    InitFrameBuffer();


    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    UninitFrameBuffer();
    UninitGraphicsPipeline();
    UninitRenderPass();
    UninitPipelineLayout();
    UninitShaderStage();
    UninitImageView();
    UninitSwapchain();
    UninitDevice();
    UninitInstance();
    UninitWindow();
}

void InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(WIDTH, HEIGHT, "vulkanTest", nullptr, nullptr);
}

void UninitWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void InitInstance()
{
    

    VkApplicationInfo application_info{};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.apiVersion = VK_API_VERSION_1_0;
    application_info.pApplicationName = "VulkanTest";

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &application_info;
    const std::vector<const char*>instanceLayers = { "VK_LAYER_KHRONOS_validation" };
    instance_create_info.enabledLayerCount = instanceLayers.size();
    instance_create_info.ppEnabledLayerNames = instanceLayers.data();
    const std::vector<const char*>instanceExtensions = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
    instance_create_info.enabledExtensionCount = instanceExtensions.size();
    instance_create_info.ppEnabledExtensionNames = instanceExtensions.data();

    auto err = vkCreateInstance(&instance_create_info, nullptr, &instance);
    if (VK_SUCCESS != err) {
        assert(0 && "Vulkan ERROR: Create instance failed!!");
        std::exit(-1);
    }

    glfwCreateWindowSurface(instance, window, nullptr, &surface);
}

void InitDevice()
{
    uint32_t gpuCount = 0;
    vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    std::vector<VkPhysicalDevice> gpuList(gpuCount);
    std::cout << "gpuCount = " << gpuCount << std::endl;
    std::cout << std::endl;
    vkEnumeratePhysicalDevices(instance, &gpuCount, gpuList.data());
    gpu = gpuList[0];

    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> familyProperty(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, familyProperty.data());
    bool found = false;
    uint32_t familyIndex;
    for (uint32_t i = 0; i < familyCount; ++i)
    {
        if (familyProperty[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            found = true;
            familyIndex = i;
        }
    }

    if (!found) {
        assert(0 && "Vulkan ERROR: Queue Family Supporting graphics not founded!");
    }

    float queuePriorities[]{ 1.0f };
    VkDeviceQueueCreateInfo device_queue_create_info{};
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.queueFamilyIndex = familyIndex;
    device_queue_create_info.queueCount = 1;
    device_queue_create_info.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    const std::vector<const char*> deviceLayers = { "VK_LAYER_KHRONOS_validation" };
    device_create_info.enabledLayerCount = deviceLayers.size();
    device_create_info.ppEnabledLayerNames = deviceLayers.data();
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    device_create_info.enabledExtensionCount = deviceExtensions.size();
    device_create_info.ppEnabledExtensionNames = deviceExtensions.data();

    vkCreateDevice(gpu, &device_create_info, nullptr, &device);

    vkGetDeviceQueue(device, 0, 0, &queueHundle);
}

void UninitInstance()
{

    vkDestroySurfaceKHR(instance, surface, nullptr);
    surface = nullptr;
    vkDestroyInstance(instance, nullptr);
    instance = nullptr;
}

void UninitDevice()
{
    vkDestroyDevice(device, nullptr);
    device = nullptr;
}

void InitSwapchain()
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCapabilities);

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = nullptr;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = surfaceCapabilities.minImageCount;
    swapchain_create_info.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchain_create_info.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_create_info.imageExtent.width = WIDTH;
    swapchain_create_info.imageExtent.height = HEIGHT;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain);

}

void InitImageView()
{
    uint32_t swapchainImageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImage.data());

    swapchainImage.resize(swapchainImageCount);
    swapchainImageView.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImage.data());

    for (int i = 0; i < swapchainImageCount; i++)
    {
        VkImageViewCreateInfo image_view_create_info{};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = nullptr;
        image_view_create_info.flags = 0;
        image_view_create_info.image = swapchainImage[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = VK_FORMAT_B8G8R8A8_UNORM;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        vkCreateImageView(device, &image_view_create_info, nullptr, &swapchainImageView[i]);

    }
}

void UninitSwapchain()
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    swapchain = nullptr;
}

void UninitImageView()
{
    for (auto view : swapchainImageView)
    {
        vkDestroyImageView(device, view, nullptr);
    }
}

std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Can't read " + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> fileBuffer(fileSize);
    file.seekg(0);
    file.read(fileBuffer.data(), fileSize);
    file.close();
    return fileBuffer;
}

void InitShaderModule()
{
    std::vector<char> vertShader;
    std::vector<char> fragShader;
    vertShader = ReadFile("vert.spv");
    fragShader = ReadFile("frag.spv");

    CreateShaderModule(vertShader, &shaderModuleVert);
    CreateShaderModule(fragShader, &shaderModuleFrag);

}

void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
{
    VkShaderModuleCreateInfo shader_create_info;
    shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_create_info.pNext = nullptr;
    shader_create_info.flags = 0;
    shader_create_info.codeSize = code.size();
    shader_create_info.pCode = (uint32_t*)code.data();

    auto err = vkCreateShaderModule(device, &shader_create_info, nullptr, shaderModule);
    if (VK_SUCCESS != err) {
        assert(0 && "Vulkan ERROR: Create ShaderModule failed.");
        std::exit(-1);
    }
}

void InitShaderStage()
{
    shader_stage_create_info_vert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_info_vert.pNext = nullptr;
    shader_stage_create_info_vert.flags = 0;
    shader_stage_create_info_vert.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stage_create_info_vert.module = shaderModuleVert;
    shader_stage_create_info_vert.pName = "main";
    shader_stage_create_info_vert.pSpecializationInfo = nullptr;

    shader_stage_create_info_frag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_info_frag.pNext = nullptr;
    shader_stage_create_info_frag.flags = 0;
    shader_stage_create_info_frag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stage_create_info_frag.module = shaderModuleFrag;
    shader_stage_create_info_frag.pName = "main";
    shader_stage_create_info_frag.pSpecializationInfo = nullptr;

    shaderStages[0] = shader_stage_create_info_vert;
    shaderStages[1] = shader_stage_create_info_frag;
}

void UninitShaderStage()
{
    vkDestroyShaderModule(device, shaderModuleVert, nullptr);
    vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
    shaderModuleVert = nullptr;
    shaderModuleFrag = nullptr;
}

void InitPipelineLayout()
{
    vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_create_info.pNext = nullptr;
    vertex_input_create_info.flags = 0;
    vertex_input_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_create_info.pVertexBindingDescriptions = nullptr;
    vertex_input_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_create_info.pVertexAttributeDescriptions = nullptr;

    input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_create_info.pNext = nullptr;
    input_assembly_create_info.flags = 0;
    input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

    viewport.x = 0.0f;
    viewport.y = 1.0f;
    viewport.width = 800;
    viewport.height = 600;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset = { 0,0 };
    scissor.extent = { 800, 600 };

    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.pNext = nullptr;
    viewport_state_create_info.flags = 0;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.pViewports = &viewport;
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.pScissors = &scissor;

    rasterrization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterrization_create_info.pNext = nullptr;
    rasterrization_create_info.flags = 0;
    rasterrization_create_info.depthBiasClamp = VK_FALSE;
    rasterrization_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterrization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterrization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterrization_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterrization_create_info.depthBiasEnable = VK_FALSE;
    rasterrization_create_info.depthBiasConstantFactor = 0.0f;
    rasterrization_create_info.depthBiasClamp = 0.0f;
    rasterrization_create_info.depthBiasSlopeFactor = 0.0f;
    rasterrization_create_info.lineWidth = 1.0f;

    multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_create_info.pNext = nullptr;
    multisample_create_info.flags = 0;
    multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_create_info.sampleShadingEnable = VK_FALSE;
    multisample_create_info.minSampleShading = 1.0f;
    multisample_create_info.pSampleMask = nullptr;
    multisample_create_info.alphaToCoverageEnable = VK_FALSE;
    multisample_create_info.alphaToOneEnable = VK_FALSE;

    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    colorblend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorblend_create_info.pNext = nullptr;
    colorblend_create_info.flags = 0;
    colorblend_create_info.logicOpEnable = VK_FALSE;
    colorblend_create_info.logicOp = VK_LOGIC_OP_NO_OP;
    colorblend_create_info.attachmentCount = 1;
    colorblend_create_info.pAttachments = &colorBlendAttachment;
    colorblend_create_info.blendConstants[0] = 0.0f;
    colorblend_create_info.blendConstants[1] = 0.0f;
    colorblend_create_info.blendConstants[2] = 0.0f;
    colorblend_create_info.blendConstants[3] = 0.0f;

    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = nullptr;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    auto err = vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &pipelineLayout);
    if (VK_SUCCESS != err) {
        assert(0 && "Vulkan ERROR: Create Pipeline Layout Failed!");
        std::exit(-1);
    }

}

void InitRenderPass()
{
    VkAttachmentDescription attachmentDesc;
    attachmentDesc.flags = 0;
    attachmentDesc.format = VK_FORMAT_B8G8R8A8_UNORM;
    attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentRef;
    attachmentRef.attachment = 0;
    attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDesc;
    subpassDesc.flags = 0;
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.inputAttachmentCount = 0;
    subpassDesc.pInputAttachments = nullptr;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &attachmentRef;
    subpassDesc.pResolveAttachments = nullptr;
    subpassDesc.pDepthStencilAttachment = nullptr;
    subpassDesc.preserveAttachmentCount = 0;
    subpassDesc.pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo render_pass_create_info;
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.pNext = nullptr;
    render_pass_create_info.flags = 0;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &attachmentDesc;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpassDesc;
    render_pass_create_info.dependencyCount = 0;
    render_pass_create_info.pDependencies = nullptr;

    auto err = vkCreateRenderPass(device, &render_pass_create_info, nullptr, &renderPass);
    if (VK_SUCCESS != err) {
        assert(0 && "Vulkan ERROR: Create Render Pass failed!");
        std::exit(-1);
    }
}

void InitGraphicsPipeline()
{
    VkGraphicsPipelineCreateInfo pipeline_create_info;
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.pNext = nullptr;
    pipeline_create_info.flags = 0;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shaderStages;
    pipeline_create_info.pVertexInputState = &vertex_input_create_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
    pipeline_create_info.pTessellationState = nullptr;
    pipeline_create_info.pViewportState = &viewport_state_create_info;
    pipeline_create_info.pRasterizationState = &rasterrization_create_info;
    pipeline_create_info.pMultisampleState = &multisample_create_info;
    pipeline_create_info.pDepthStencilState = nullptr;
    pipeline_create_info.pColorBlendState = &colorblend_create_info;
    pipeline_create_info.pDynamicState = nullptr;
    pipeline_create_info.layout = pipelineLayout;
    pipeline_create_info.renderPass = renderPass;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    auto err = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeLine);
    if (VK_SUCCESS != err) {
        assert(0 && "Vulkan ERROR: Create Pipeline failed!");
        std::exit(-1);
    }
}

void UninitPipelineLayout()
{
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    pipelineLayout = nullptr;
}

void UninitRenderPass()
{
    vkDestroyRenderPass(device, renderPass, nullptr);
    renderPass = nullptr;
}

void UninitGraphicsPipeline()
{
    vkDestroyPipeline(device, pipeLine, nullptr);
    pipeLine = nullptr;
}

void InitFrameBuffer()
{
    VkFramebufferCreateInfo frame_buffer_create_info;
    
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.pNext = nullptr;
    frame_buffer_create_info.flags = 0;
    frame_buffer_create_info.renderPass = renderPass;
    frame_buffer_create_info.attachmentCount = 1;
    frame_buffer_create_info.pAttachments = &(swapchainImageView[0]);
    frame_buffer_create_info.width = WIDTH;
    frame_buffer_create_info.height = HEIGHT;
    frame_buffer_create_info.layers = 1;

    vkCreateFramebuffer(device, &frame_buffer_create_info, nullptr, &frameBuffers);
    
}

void UninitFrameBuffer()
{
    vkDestroyFramebuffer(device, frameBuffers, nullptr);
    frameBuffers = nullptr;
    
}