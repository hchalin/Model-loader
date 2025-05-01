#import "glfw_adaptor.h"


NS::Window* get_ns_window(GLFWwindow * window, CA::MetalLayer* metalLayer){
  CALayer * obj_layer = (__bridge CALayer*) metalLayer;
  NSWindow* obj_window = glfwGetCocoaWindow(window);

  obj_window.contentView.layer = obj_layer;
  obj_window.contentView.wantsLayer = YES;
  return (__bridge NS::Window*) obj_window;
}
