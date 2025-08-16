#pragma once
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>

void initUI(tgui::Gui& gui, sf::RenderWindow& window,
    tgui::Button::Ptr& runPauseBtn,
    tgui::Slider::Ptr& gridSlider,
    tgui::Button::Ptr& box0Btn);
