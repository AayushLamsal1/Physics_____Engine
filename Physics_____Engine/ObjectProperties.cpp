#include "ObjectProperties.hpp"
#include <cmath>
#include <string>

void ObjectProperties::showPopup(Object* obj, tgui::GuiSFML& gui)
{
    auto popup = tgui::ChildWindow::create("Object Properties");
    popup->setSize(250, 220);
    popup->setPosition(300, 200);
    popup->getRenderer()->setBackgroundColor(tgui::Color(18, 26, 38));
    popup->getRenderer()->setBorderColor(tgui::Color(74, 106, 148));
    popup->getRenderer()->setBorders(2);
    gui.add(popup);

    auto lblVel = tgui::Label::create("Velocity:");
    lblVel->setPosition(20, 20);
    lblVel->setTextSize(14);
    lblVel->getRenderer()->setTextColor(tgui::Color::White);
    popup->add(lblVel);

    auto inputVel = tgui::EditBox::create();
    inputVel->setPosition(100, 18);
    inputVel->setSize(120, 24);
    popup->add(inputVel);

    auto lblAngle = tgui::Label::create("Angle:");
    lblAngle->setPosition(20, 60);
    lblAngle->setTextSize(14);
    lblAngle->getRenderer()->setTextColor(tgui::Color::White);
    popup->add(lblAngle);

    auto inputAngle = tgui::EditBox::create();
    inputAngle->setPosition(100, 58);
    inputAngle->setSize(120, 24);
    popup->add(inputAngle);

    auto btnApply = tgui::Button::create("Apply");
    btnApply->setSize(200, 40);
    btnApply->setPosition(25, 140);
    popup->add(btnApply);

    btnApply->onPress([obj, inputVel, inputAngle, popup]() {
        float v = std::stof(inputVel->getText().toStdString());
        float a = std::stof(inputAngle->getText().toStdString()) * 3.14159265f / 180.f;
        obj->velocity.x = v * std::cos(a);
        obj->velocity.y = -v * std::sin(a);
        popup->close();
        });
}
