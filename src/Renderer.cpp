//
// Created by Hayden Chalin on 5/1/25.
//

#include "Renderer.h"


Renderer::Renderer(Window &windowSrc, Model *model):
    // Get device from the metalLayer in the window
    device(windowSrc.getMTLLayer()->device()),
    window(&windowSrc),
    camera(Vector3f(1.0, 0.0, 5.0), Vector3f(0.0, 0.0, 0.0)), // * camera(camPos, target)
    model(model) {
    const float aRatio = windowSrc.getAspectRatio();
    if (!device) {
        throw std::runtime_error("Failed to create MTL::Device");
    }

    // ^ Timing
    lastTime = glfwGetTime();           // Set the initial time

    // ^ Command Queue
    constexpr int maxBufferAmt{64};
    commandQueue = device->newCommandQueue(maxBufferAmt);
    if (!commandQueue) {
        throw std::runtime_error("Failed to create command queue");
    }

    // ^ Allocate memory for the uniform buffer
    uniformBuffer = device->newBuffer(sizeof(Matrix4f) * 3, MTL::ResourceStorageModeManaged); // Room for33 matrices
    if (!uniformBuffer) {
        throw std::runtime_error("Failed to create uniform buffer");
    }
    updateProjectionMatrix(aRatio);


    // ^ Camera view matrix
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents()); // @ Get pointer to buffers contents
    *bufferPtr = viewMatrix; // @ This will do something similar to memcopy into the uniform buffer
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));

    // ^ Set GLFW Callbacks
    glfwSetWindowUserPointer(window->getGLFWWindow(), this);
    glfwSetWindowRefreshCallback(window->getGLFWWindow(), framebuffer_refresh_callback);
    glfwSetFramebufferSizeCallback(window->getGLFWWindow(), framebuffer_size_callback);
    glfwSetKeyCallback(window->getGLFWWindow(), keyCallback);
    glfwSetScrollCallback(window->getGLFWWindow(), scrollCallback);

    // ^ model matrix, sent into the GPU's uniform buffer
    BroMath::Transform &matrix = model->getTransform();     // ^ This returns a type: BroMath::Transform
    matrix.setTranslation(0,-1.0, 0);
    *(bufferPtr + 2) = matrix.getMatrix();                  // ^ This returns a type: Eigen::Matrix4f
    uniformBuffer->didModifyRange(NS::Range(2 * sizeof(Matrix4f), sizeof(Matrix4f)));

    // ^ Create render pipeline state
    createPipelineState();
}

void Renderer::updateProjectionMatrix(float aRatio) {
    // ^ Define the vertical fov to radias, horizontal will be based off of this.
    const float fovY = 45.0f * (M_PI / 180.0f);

    // ^ Frustrum clipping planes
    const float near = 0.1f;
    const float far = 100.0f;

    // ^ Reset projectionMatrix
    projectionMatrix.setIdentity();

    float tanHalfFovy = std::tan(fovY / 2.0f);

    // Set up perspective matrix (column-major order in Eigen)
    projectionMatrix(0, 0) = 1.0f / (aRatio * tanHalfFovy); // Scale X
    projectionMatrix(1, 1) = 1.0f / tanHalfFovy; // Scale Y
    projectionMatrix(2, 2) = -(far + near) / (far - near); // Scale and translate Z
    projectionMatrix(2, 3) = -(2.0f * far * near) / (far - near); // Perspective divide term
    projectionMatrix(3, 2) = -1.0f; // Enables perspective division
    projectionMatrix(3, 3) = 0.0f; // Required for perspective

    // Update uniform buffer with combined view-projection matrix
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *(bufferPtr + 1) = projectionMatrix;           // @ Put the projection matrix in the 2nd position of the uniform buffer
    // NOTE, for managed memory(CPU and GPU each have thier own copy), didModifyRange() tells metal the CPU updated this region so the GPU's copy stays in sync
    uniformBuffer->didModifyRange(NS::Range(offsetof(Uniforms, projectionMatrix), sizeof(Matrix4f)));
}

void Renderer::createPipelineState() {
    /*
     * Shaders
     */
    const std::string shaderFileName = "shaders.metal";
    NS::Error *err{nullptr};
    MTL::Library *library{nullptr};
    NS::String *shaderSource{nullptr};

    try {
        shaderSource = loadShaderFromFile(device, shaderFileName);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    NS::Error *error = nullptr;
    MTL::CompileOptions *compileOptions = MTL::CompileOptions::alloc()->init();
    library = device->newLibrary(shaderSource, compileOptions, &error);
    if (!library) {
        compileOptions->release();
        throw std::runtime_error("Failed to create MTL::Library");
    } else {
        // @ If the library creation is successful, release the shader source
        if (shaderSource)
            shaderSource->release();
    }
    // ^ Create shader functions
    MTL::Function *vertexFunction{nullptr};
    MTL::Function *fragmentFunction{nullptr};

    // ^ Vertex Function
    vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
    if (!vertexFunction) {
        std::cerr << "Failed to create vertex function" << std::endl;
        library->release();
        throw std::runtime_error("Failed to create vertex function");
    }

    // ^ Fragment Function
    fragmentFunction = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
    if (!fragmentFunction) {
        std::cerr << "Failed to create fragment function" << std::endl;
        vertexFunction->release();                                          // Release the vertex function
        library->release();
        throw std::runtime_error("Failed to create fragment function");
    }


    // ^ Create pipeline descriptor and set functions
    MTL::RenderPipelineDescriptor *renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setVertexFunction(vertexFunction);
    renderPipelineDescriptor->setFragmentFunction(fragmentFunction);

    // ^ Configure color attachment - NOTE: The color attachment represents the output target for the fragment shader.
    MTL::RenderPipelineColorAttachmentDescriptor *colorAttachment = renderPipelineDescriptor->colorAttachments()->
            object(0);
    colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

    // ^ This overwrites the entire color buffer, keep this in mind when rendering fog etc...
    colorAttachment->setBlendingEnabled(false);


    /*
     *      Floor
     */
    Vertex floorVertices[] = {
        {{-1.5, -0.501, 1.5, 1.0}, {0.5, 0.0, 0.0, 1.0}, Eigen::Vector3f::Zero(), Eigen::Vector2f::Zero()},
        {{-1.5, -0.501, -1.5, 1.0}, {.5, 0.0, 0.0, 1.0}, Eigen::Vector3f::Zero(), Eigen::Vector2f::Zero()},
        {{1.5, -0.501, -1.5, 1.0}, {.5, 0.0, 0.0, 1.0}, Eigen::Vector3f::Zero(), Eigen::Vector2f::Zero()},
        {{1.5, -0.501, 1.5, 1.0}, {.5, 0.0, 0.0, 1.0}, Eigen::Vector3f::Zero(), Eigen::Vector2f::Zero()},
    };

    int floorIndices[] = {0, 1, 3, 1, 2, 3};

    floorIndexBuffer = device->newBuffer(floorIndices, sizeof(floorIndices), MTL::ResourceStorageModeManaged);
    if (!floorIndexBuffer) {
        throw std::runtime_error("Failed to create index buffer");
    }

    floorVertexBuffer = device->newBuffer(floorVertices, sizeof(floorVertices), MTL::ResourceStorageModeManaged);
    if (!floorVertexBuffer) {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    // ^ Describe the Vertex in memory
    MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

    // Position
    vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(0)->setOffset(0);
    vertexDescriptor->attributes()->object(0)->setBufferIndex(0);
    // Color
    vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormatFloat4);
    vertexDescriptor->attributes()->object(1)->setOffset(sizeof(float4));
    vertexDescriptor->attributes()->object(1)->setBufferIndex(0);
    //Layout
    vertexDescriptor->layouts()->object(0)->setStride(sizeof(Vertex));

    renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

    renderPipelineState = device->newRenderPipelineState(renderPipelineDescriptor, &err);
    if (!renderPipelineState) {
        throw std::runtime_error("Failed to create MTL::RenderPipelineState");
    }

    // Resource clean up
    library->release();
    compileOptions->release();
    vertexFunction->release();
    fragmentFunction->release();
    renderPipelineDescriptor->release();
    vertexDescriptor->release();
}

Renderer::~Renderer() {
    if (commandQueue) {
        commandQueue->release();
        commandQueue = nullptr;
    }
    if (uniformBuffer) {
        uniformBuffer->release();
        uniformBuffer = nullptr;
    }
    if (triangleVertexBuffer) {
        triangleVertexBuffer->release();
        triangleVertexBuffer = nullptr;
    }
    if (floorVertexBuffer) {
        floorVertexBuffer->release();
        floorVertexBuffer = nullptr;
    }
    if (floorIndexBuffer) {
        floorIndexBuffer->release();
        floorIndexBuffer = nullptr;
    }
    if (renderPipelineState) {
        renderPipelineState->release();
        renderPipelineState = nullptr;
    }

    if (uniformBuffer) {
        uniformBuffer->release();
        uniformBuffer = nullptr;
    }
}

void Renderer::render() {
    // ^ https://stackoverflow.com/questions/23858454/glfw-poll-waits-until-window-resize-finished-how-to-fix

    while (!glfwWindowShouldClose(window->getGLFWWindow())) {
        // ^ Timeing
        auto currentFrame = static_cast<double>(glfwGetTime());
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        glfwPollEvents();
        drawFrame();
    }
}

/*
 *     This function draws a single frame
 *
 */

void Renderer::drawFrame() {
    // * Auto release pool for memory management
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
    // ^  Get the next drawable
    CA::MetalDrawable *drawable = window->getMTLLayer()->nextDrawable();
    if (!drawable) {
        // * Clean up
        pool->release();
        glfwDestroyWindow(window->getGLFWWindow());
        return;
    }


    // * Create command buffer
    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();

    if (!commandBuffer) {
        throw std::runtime_error("Failed to render command buffer");
    }

    // * Create render pass descriptor
    MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor *colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
    colorAttachment->setTexture(drawable->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setClearColor(MTL::ClearColor(0.1, 0.1, 0.1, 1.0));
    colorAttachment->setStoreAction(MTL::StoreActionStore);

    // *  Encoding
    MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
    encoder->setRenderPipelineState(renderPipelineState);
    encoder->setVertexBuffer(uniformBuffer, 0, 1);          // Set the uniform buffer

    // Draw floor
    encoder->setVertexBuffer(floorVertexBuffer, 0, 0);
    Eigen::Matrix4f identity = Eigen::Matrix4f::Identity();
    encoder->setVertexBytes(identity.data(), sizeof(identity), 11);

    // @ Uncomment to draw a red floor
  /* encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
                                 6, // 6 indices
                                 MTL::IndexTypeUInt32,
                                 floorIndexBuffer,
                                 0, // offset
                                 1); // i
                                 */


  // @ Draw model
  if (model) {
      encoder->setVertexBuffer(model->getVertexBuffer(), 0, 0);

      // Send the transformation matrix
      // rotate around Y by deltaTime * spinSpeed
      float spinSpeed = 0.3f;
      model->getTransform().setRotation(deltaTime * spinSpeed, 0.0f, 1.0f, 0.0f);
      const Eigen::Matrix4f &transformMatrix = model->getTransform().getMatrix();
      auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
      *(bufferPtr + 2) = transformMatrix;
      // NOTE, for managed memory(CPU and GPU each have their own copy), didModifyRange() tells metal the CPU updated this region so the GPU's copy stays in sync
      uniformBuffer->didModifyRange(NS::Range(2 * sizeof(Matrix4f), sizeof(Matrix4f)));         // ^ Update the 3rd slot for the uniform buffer
      // Note, this another way to calculate the required offset for the buffer update
      //uniformBuffer->didModifyRange(NS::Range(offsetof(Uniforms, projectionMatrix), sizeof(Matrix4f)));         // ^ Update the 3rd slot for the uniform buffer
      // Note, below is a FULL buffer flush
      //uniformBuffer->didModifyRange(NS::Range(0, uniformBuffer->length()));


      // ^ This is the draw call
      encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle,
                                     model->getIndexCount(),
                                     MTL::IndexTypeUInt32,
                                     model->getIndexBuffer(),
                                     0,
                                     1);
    }


        encoder->endEncoding();

        // Present
        commandBuffer->presentDrawable(drawable);
        commandBuffer->commit();

        // ! CLEAN UP
        renderPassDescriptor->release();
        pool->release();
}


Window &Renderer::getWindow() {
      return *window;
}

void Renderer::cameraUp() {
    camera.moveUp(0.05);
    // Before you draw a frame, update viewMatrix sent to the gpu
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;  // Copy updated view matrix to uniform buffer
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));

}

void Renderer::cameraDown() {
    camera.moveDown(0.05);
    // Before you draw a frame, update viewMatrix sent to the gpu
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;  // Copy updated view matrix to uniform buffer
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));


}

void Renderer::cameraRight() {
    camera.moveRight(0.05);
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
}
void Renderer::cameraLeft() {
    camera.moveLeft(0.05);
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
}

void Renderer::cameraZoom(float aZoom) {
    camera.zoom(aZoom);
    // ! TODO: abstract this repeated code for movement
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto *bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
}
void Renderer::cameraMove(float scalar) {
    camera.move(scalar);
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto* bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
}

void Renderer::cameraRotate(float aTurn) {
    camera.rotate(aTurn);
    Matrix4f viewMatrix = camera.getViewMatrix();
    auto* bufferPtr = static_cast<Matrix4f *>(uniformBuffer->contents());
    *bufferPtr = viewMatrix;
    uniformBuffer->didModifyRange(NS::Range(0, sizeof(Matrix4f)));
}

void Renderer::processInput(int key, int scancode, int action, int mods){

    if (keyMap[GLFW_KEY_W]) {
        cameraUp();
    }
    if (keyMap[GLFW_KEY_S]) {
        cameraDown();
    }
    if (keyMap[GLFW_KEY_A]) {
        cameraLeft();
    }
    if (keyMap[GLFW_KEY_D]) {
        cameraRight();
    }
}


/** @note Below are the free functions for glfw
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    auto *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    if (!renderer || height == 0) return;


    renderer->getWindow().getMTLLayer()->setDrawableSize(CGSize{
        static_cast<double>(width),
        static_cast<double>(height)
    });

    float aspect = static_cast<float>(width) / height;
    renderer->updateProjectionMatrix(aspect);
    renderer->drawFrame();
}


void framebuffer_refresh_callback(GLFWwindow* window) {
    // ! Not currently in use
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS)
    {
        renderer->keyMap[key] = true;
    } else if (action == GLFW_RELEASE)
    {
        renderer->keyMap[key] = false;
    }
    renderer->processInput(key, scancode, action, mods);
}
/*
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // ! TODO, make this handle key holds, not just presses.
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    // ^ W Press
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        std::cout << "KEY PRESSED" << std::endl;
        while (renderer->keyDown) {
            renderer->cameraUp();
            if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
                renderer->keyDown = false;
            }
        }
    }
    // ^ W Release
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        renderer->keyDown = false;
        std::cout << "KEY Released" << std::endl;
    }
    // ^ A
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        renderer->cameraLeft();
    }
    // ^ S
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        renderer->cameraDown();
    }
    // ^ D
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        renderer->cameraRight();
    }
}
*/

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    constexpr float dampen {0.09};      // Slow down trackpad movement
    constexpr float degreesPerScroll {5.0f};
    float angleDeg = xoffset * degreesPerScroll;
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        //std::cout << "Scroll: x = " << xoffset << ", y = " << yoffset << std::endl;
        //std::cout << "ScrollCallback" << std::endl;
        renderer->cameraRotate(-xoffset);
    }else {
    xoffset *= dampen;
    yoffset *= dampen;
    // TODO: Make this one function call?
    renderer->cameraMove(xoffset);
    renderer->cameraZoom(yoffset);
    }
};
