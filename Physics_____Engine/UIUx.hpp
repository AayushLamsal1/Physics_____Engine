#pragma once
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

// UI initialization function
void initUI(tgui::Gui& gui, sf::RenderWindow& window,
    tgui::Button::Ptr& runPauseBtn,
    tgui::Slider::Ptr& gridSlider,
    tgui::Button::Ptr& box0Btn,
    tgui::Button::Ptr& box1Btn,
    tgui::Button::Ptr& box2Btn,
    tgui::Button::Ptr& resetBtn);  
