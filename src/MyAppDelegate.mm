#ifdef __APPLE__
#include "MyAppDelegate.h"
#include "MyMTKViewDelegate.h"
#include <Foundation/Foundation.hpp>
#include <UIKit/UILabel.hpp>
#include <UIKit/UIFont.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

using namespace NS;
using namespace UI;

MyAppDelegate::MyAppDelegate() {}
MyAppDelegate::~MyAppDelegate() {}

void MyAppDelegate::applicationDidFinishLaunching(NS::Notification* pNotification) {
    // Create the FPS label
    Label* pLabel = Label::alloc()->init();
    pLabel->setFrame(NS::Rect{{10.0, 10.0}, {100.0, 20.0}});
    pLabel->setFont(UI::Font::systemFontOfSize(14.0f));
    pLabel->setText(NS::String::string("FPS: 0", NS::ASCIIStringEncoding));

    // Add as subview of view controller
    if(_pViewController && _pViewController->view()) {
        _pViewController->view()->addSubview(pLabel);
    }

    // Store pointer for later updates
    _pLabel = pLabel;
    if(_pViewDelegate) {
        _pViewDelegate->setLabel(pLabel);
    }
}
#endif // __APPLE__
