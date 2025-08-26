#ifdef __APPLE__
#include "MyMTKViewDelegate.h"
#include <UIKit/UILabel.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <QuartzCore/CAMediaTiming.h>
#include <Foundation/Foundation.hpp>
#include <cstdio>

using namespace NS;
using namespace UI;

MyMTKViewDelegate::MyMTKViewDelegate() : _pLabel(nullptr), _lastTime(0.0), _runningFPS(0.0f) {}

void MyMTKViewDelegate::setLabel(UI::Label* pLabel) {
    _pLabel = pLabel;
}

void MyMTKViewDelegate::drawInMTKView(MTK::View* pView) {
    double current = CACurrentMediaTime();
    if(_lastTime > 0.0) {
        double delta = current - _lastTime;
        float fps = delta > 0.0 ? 1.0f / static_cast<float>(delta) : 0.0f;
        // running average smoothing
        _runningFPS = 0.9f * _runningFPS + 0.1f * fps;
        if(_pLabel) {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "FPS: %.2f", _runningFPS);
            _pLabel->setText(NS::String::string(buf, NS::ASCIIStringEncoding));
        }
    }
    _lastTime = current;
}
#endif // __APPLE__
