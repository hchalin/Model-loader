//
// Created by Hayden Chalin on 5/1/25.
//

#include "Window.h"
#include "Renderer.h"

// Forward-declare static callbacks at file scope
static void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height);
static void framebuffer_refresh_callback(GLFWwindow* glfwWindow);

Window::Window(MTL::Device *device) {
    // disable GLFW's default behavior of creating an OpenGL context
    // Look at declaration for better explination
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create window
    int windowWidth = 800, windowHeight = 600;
    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Model Viewer", nullptr, nullptr);
    if (!glfwWindow) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwSwapInterval(1);
    aspectRatio = (float)windowWidth / windowHeight;

    // Set the metal layer on the window
    metalLayer = CA::MetalLayer::layer()->retain();
    metalLayer->setDevice(device);    // Create metal device and set it as the metal layer for glfw
    metalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);     // Standard 8-bit format.
    metalLayer->setFramebufferOnly(true);                       // Textures only as render targets


    nsWindow = get_ns_window(glfwWindow, metalLayer)->retain(); // Next Step window


    // Set up the menu bar
    setupMenu();




    // ^ Set GLFW Callbacks
    glfwSetWindowUserPointer(this->glfwWindow, this);
    glfwSetWindowRefreshCallback(this->glfwWindow, framebuffer_refresh_callback);       // ! Not in use
    glfwSetFramebufferSizeCallback(this->glfwWindow, framebuffer_size_callback);        // @ On Resize

}

Window::~Window() {
    if (metalLayer) {
        metalLayer->release();
    }
}

float Window::getDeltaTime(){return deltaTime;}

void Window::setDeltaTime(float dt) {
    deltaTime = dt;
}


void Window::setCamera(Camera *camera) {
    this->camera = camera;
}

CA::MetalLayer *Window::getMTLLayer() const {
    if (metalLayer) {
        return metalLayer;
    }
    return nullptr;
}
GLFWwindow *Window::getGLFWWindow() const {
    if (metalLayer) {
        return glfwWindow;
    }
    return nullptr;
}

float Window::getAspectRatio() const {
    return aspectRatio;
}

void framebuffer_refresh_callback(GLFWwindow *glfwWindow) {
    std::cout << "Refresh" << std::endl;
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window) return;
    // Draw a frame while the OS runs the modal refresh cycle
    window->requestRedraw();

}

void Window::setRenderer(Renderer *renderer) {
    if (renderer) {
        this->renderer = renderer;
    }
    if (renderer)
        std::cout << "Renderer set" << std::endl;
}

void Window::requestRedraw() {
    if (renderer) {
        renderer->drawFrame();
    }
}

void Window::setController(Controller *controller) {
    if (controller) {
        this->controller = controller;
    }
}

void Window::setupMenu(void) {
    // Get the shared app
    NS::Application *app = NS::Application::sharedApplication();

    // Create menu bar
    NS::Menu *mainMenu = NS::Menu::alloc()->init();

    // Create file menu
    NS::MenuItem *fileMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu *fileMenu = NS::Menu::alloc()->init();

    // Add open menu item
    NS::MenuItem *openItems = fileMenu->addItem(
        NS::String::string("Open Model", NS::UTF8StringEncoding),
        nullptr,
        NS::String::string("o", NS::UTF8StringEncoding)
    );

    // Add separator
    //fileMenu->addItem(NS::MenuItem::separatorItem());
    // Add "Quit" menu item
    NS::MenuItem* quitItem = fileMenu->addItem(
        NS::String::string("Quit", NS::UTF8StringEncoding),
        nullptr,
        NS::String::string("q", NS::UTF8StringEncoding)
    );

    fileMenuItem->setSubmenu(fileMenu);
    mainMenu->addItem(fileMenuItem);

    // Create "View" menu
    NS::MenuItem* viewMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* viewMenu = NS::Menu::alloc()->init(NS::String::string("View", NS::UTF8StringEncoding));

    // Add "Reset Camera" menu item
    NS::MenuItem* resetCameraItem = viewMenu->addItem(
        NS::String::string("Reset Camera", NS::UTF8StringEncoding),
        nullptr,
        NS::String::string("r", NS::UTF8StringEncoding)
    );

    // Add "Wireframe Mode" menu item
    NS::MenuItem* wireframeItem = viewMenu->addItem(
        NS::String::string("Toggle Wireframe", NS::UTF8StringEncoding),
        nullptr,
        NS::String::string("w", NS::UTF8StringEncoding)
    );
    {
        // Testing
        NS::Menu* testMenu = NS::Menu::alloc()->init(NS::String::string("Test", NS::UTF8StringEncoding));
        NS::MenuItem *button = testMenu->addItem(NS::String::string("Test Button", NS::UTF8StringEncoding), nullptr, NS::String::string("t", NS::UTF8StringEncoding));
        testMenu->release();
    }

    viewMenuItem->setSubmenu(viewMenu);
    mainMenu->addItem(viewMenuItem);

    // Set the main menu
    app->setMainMenu(mainMenu);

    // Clean up (don't release if you need to keep references)
    fileMenu->release();
    fileMenuItem->release();
    viewMenu->release();
    viewMenuItem->release();


}




static void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!window || height == 0) return;

    // Reset the window's size
    window->getMTLLayer()->setDrawableSize(CGSize{
        static_cast<double>(width),
        static_cast<double>(height)
    });

    float aspect = static_cast<float>(width) / height;
    window->aspectRatio = aspect; // keep cached aspect in sync
    if (window->camera) {
        window->camera->updateAspectRatio(aspect);
    }
    window->requestRedraw();
}