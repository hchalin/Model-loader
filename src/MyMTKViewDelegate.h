#pragma once

#ifdef __APPLE__
#include <MetalKit/MetalKit.hpp>
namespace UI { class Label; }

class MyMTKViewDelegate : public MTK::ViewDelegate {
public:
    MyMTKViewDelegate();
    void setLabel(UI::Label* pLabel);
    void drawInMTKView(MTK::View* pView) override;

private:
    UI::Label* _pLabel{};
    double _lastTime{0.0};
    float _runningFPS{0.0f};
};
#endif // __APPLE__
