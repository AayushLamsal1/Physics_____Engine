#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "UIUX.hpp"
#include "Objects.hpp"

int main()
{
    // Window setup
    sf::RenderWindow window(sf::VideoMode(1100, 700), "Physics Engine - UI Mockup", sf::Style::Close);
    window.setFramerateLimit(60);

    tgui::Gui gui{ window };

    sf::RectangleShape background(sf::Vector2f(static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)));
    background.setFillColor(sf::Color(20, 32, 46));

    // UI references
    tgui::Button::Ptr runPauseBtn;
    tgui::Slider::Ptr gridSlider;
    tgui::Button::Ptr box0Btn;

    initUI(gui, window, runPauseBtn, gridSlider, box0Btn);

    // Run/Pause toggle
    bool isRunning = false;
    if (runPauseBtn)
        runPauseBtn->onPress([&]() {
        isRunning = !isRunning;
        runPauseBtn->setText(isRunning ? "Pause" : "Run");
            });

    // Objects manager
    Objects objects(gui, window);
    if (box0Btn)
        box0Btn->onPress([&]() {
        objects.handleBoxClick();
            });

    // Canvas dimensions
    const float canvasX = 240.f, canvasY = 80.f;
    const float canvasW = 820.f, canvasH = 560.f;
    const float gridSpacing = 60.f;
    const sf::FloatRect canvasRect(canvasX + 6.f, canvasY + 6.f, canvasW - 12.f, canvasH - 12.f);

    const float groundHeight = 60.f; // permanent ground

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                objects.handleMousePress(sf::Vector2f(event.mouseButton.x, event.mouseButton.y), canvasRect);

            if (event.type == sf::Event::MouseMoved)
                objects.handleMouseDrag(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                objects.handleMouseRelease();

            gui.handleEvent(event);
        }

        window.clear();
        window.draw(background);

        // Canvas backplate (simulation box)
        sf::RectangleShape canvasBack(sf::Vector2f(canvasRect.width, canvasRect.height));
        canvasBack.setPosition(canvasRect.left, canvasRect.top);
        canvasBack.setFillColor(sf::Color(0, 53, 107)); // Yale Blue
        canvasBack.setOutlineThickness(2.f);
        canvasBack.setOutlineColor(sf::Color::Black);
        window.draw(canvasBack);

        // Permanent ground
        sf::RectangleShape ground(sf::Vector2f(canvasRect.width, groundHeight));
        ground.setPosition(canvasRect.left, canvasRect.top + canvasRect.height - groundHeight);
        ground.setFillColor(sf::Color(10, 26, 47)); // Space Cadet
        ground.setOutlineThickness(2.f);
        ground.setOutlineColor(sf::Color::Black);
        window.draw(ground);

        // Black joint line at top of ground (4 px)
        sf::RectangleShape jointLine(sf::Vector2f(canvasRect.width, 4.f));
        jointLine.setPosition(canvasRect.left, canvasRect.top + canvasRect.height - groundHeight);
        jointLine.setFillColor(sf::Color::Black);
        window.draw(jointLine);

        // Draw grid if slider is ON
        if (gridSlider && gridSlider->getValue() > 0.5f)
        {
            sf::Color lineColor(18, 30, 44);

            // Vertical lines
            for (float x = canvasRect.left; x <= canvasRect.left + canvasRect.width; x += gridSpacing)
            {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(x, canvasRect.top), lineColor),
                    sf::Vertex(sf::Vector2f(x, canvasRect.top + canvasRect.height), lineColor)
                };
                window.draw(line, 2, sf::Lines);
            }

            // Horizontal lines
            for (float y = canvasRect.top; y <= canvasRect.top + canvasRect.height; y += gridSpacing)
            {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(canvasRect.left, y), lineColor),
                    sf::Vertex(sf::Vector2f(canvasRect.left + canvasRect.width, y), lineColor)
                };
                window.draw(line, 2, sf::Lines);
            }
        }

        // Update physics
        objects.update(1.f / 60.f, isRunning, canvasRect, groundHeight);

        // Draw objects
        objects.draw(window);

        // Draw GUI
        gui.draw();

        window.display();
    }

    return 0;
}