#pragma once

#ifdef __APPLE__
#include <AppKit/NSApplication.hpp>
#include <MetalKit/MetalKit.hpp>

namespace UI { class Label; }

class MyMTKViewDelegate;

class MyAppDelegate : public NS::ApplicationDelegate {
public:
    MyAppDelegate();
    virtual ~MyAppDelegate();

    void applicationDidFinishLaunching(NS::Notification* pNotification) override;

    UI::Label* fpsLabel() const { return _pLabel; }

private:
    NS::Window* _pWindow{};
    NS::ViewController* _pViewController{};
    MTK::View* _pView{};
    MyMTKViewDelegate* _pViewDelegate{};
    UI::Label* _pLabel{};
};
#endif // __APPLE__
