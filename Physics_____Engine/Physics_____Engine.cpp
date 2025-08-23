#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <cmath>
#include <algorithm>
#include "UIUX.hpp"
#include "Objects.hpp"

static float lerp(float a, float b, float t) { return a + (b - a) * t; }

int main()
{
    sf::RenderWindow window(sf::VideoMode(1100, 700), "Physics Engine ", sf::Style::Close);
    window.setFramerateLimit(60);
    tgui::Gui gui{ window };

    sf::RectangleShape background(sf::Vector2f(static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)));
    background.setFillColor(sf::Color(37, 42, 59));


    tgui::Button::Ptr runPauseBtn;
    tgui::Button::Ptr resetBtn;
    tgui::Slider::Ptr gridSlider;
    tgui::Button::Ptr box0Btn;
    tgui::Button::Ptr box1Btn;
    tgui::Button::Ptr box2Btn;
    tgui::Button::Ptr stopTimeBtn;
    tgui::Label::Ptr timerLabel;
    tgui::Label::Ptr stoppedTimesLabel;

    initUI(gui, window, runPauseBtn, gridSlider, box0Btn, box1Btn, box2Btn, resetBtn);

    // Timer label
    timerLabel = tgui::Label::create("Time: 0.00s");
    timerLabel->setTextSize(18);
    timerLabel->getRenderer()->setTextColor(sf::Color::White);
    timerLabel->setPosition({ 22.f, 222.f });
    gui.add(timerLabel);

    // Stopped times label
    stoppedTimesLabel = tgui::Label::create("");
    stoppedTimesLabel->setTextSize(16);
    stoppedTimesLabel->getRenderer()->setTextColor(sf::Color::Yellow);
    stoppedTimesLabel->setPosition({ 22.f, 250.f });
    gui.add(stoppedTimesLabel);

    bool isRunning = false;
    float simulationTime = 0.f;

    Objects objects(gui, window);

    // Store initial positions of objects
    std::vector<sf::Vector2f> initialPositions;

    if (runPauseBtn)
        runPauseBtn->onPress([&]() {
        if (!isRunning) {
            initialPositions.clear();
            for (const auto& obj : objects.getObjects()) {
                initialPositions.push_back(obj.shape->getPosition());
            }
        }
        isRunning = !isRunning;
        runPauseBtn->setText(isRunning ? "Pause" : "Run");
            });

    // Max 5 stopped times
    const int maxStops = 5;
    std::vector<float> stoppedTimes;

    if (resetBtn)
        resetBtn->onPress([&]() {
        simulationTime = 0.f;
        timerLabel->setText("Time: 0.00s");
        stoppedTimes.clear();
        stoppedTimesLabel->setText("");
        if (!initialPositions.empty()) {
            auto& objs = const_cast<std::vector<PhysicsObject>&>(objects.getObjects());
            for (size_t i = 0; i < objs.size() && i < initialPositions.size(); ++i) {
                objs[i].shape->setPosition(initialPositions[i]);
                objs[i].velocity = { 0.f, 0.f };
            }
        }
            });

    if (box0Btn)
        box0Btn->onPress([&]() { objects.handleBoxClick(); });

    if (box1Btn)
        box1Btn->onPress([&]() { objects.enablePathTracing(); });

    if (box2Btn)
        box2Btn->onPress([&]() { objects.toggleRangeLine(); });

    // Stop Clock button
    stopTimeBtn = tgui::Button::create("Stop Clock");
    stopTimeBtn->setSize({ 140.f, 44.f });
    stopTimeBtn->setPosition({ 800.f, 18.f });
    stopTimeBtn->getRenderer()->setBackgroundColor(tgui::Color(100, 102, 184));
    stopTimeBtn->getRenderer()->setTextColor(sf::Color::White);
    stopTimeBtn->getRenderer()->setRoundedBorderRadius(10);
    gui.add(stopTimeBtn);

    if (stopTimeBtn)
        stopTimeBtn->onPress([&]() {
        if (stoppedTimes.size() < maxStops) {
            stoppedTimes.push_back(simulationTime);
            std::string text = "Stopped Times:\n";
            for (float t : stoppedTimes)
                text += std::to_string(t).substr(0, 5) + "s\n";
            stoppedTimesLabel->setText(text);
        }
            });

    const float canvasX = 240.f, canvasY = 80.f;
    const float canvasW = 820.f, canvasH = 560.f;
    const sf::FloatRect canvasFramePx(canvasX + 6.f, canvasY + 6.f, canvasW - 12.f, canvasH - 12.f);

    sf::View worldView;
    worldView.setCenter(canvasFramePx.left + canvasFramePx.width * 0.5f,
        canvasFramePx.top + canvasFramePx.height * 0.5f);
    worldView.setSize(canvasFramePx.width, canvasFramePx.height);

    auto applyViewport = [&]() {
        sf::Vector2u ws = window.getSize();
        worldView.setViewport({
            canvasFramePx.left / static_cast<float>(ws.x),
            canvasFramePx.top / static_cast<float>(ws.y),
            canvasFramePx.width / static_cast<float>(ws.x),
            canvasFramePx.height / static_cast<float>(ws.y)
            });
        };
    applyViewport();

    const sf::Vector2f baseViewSize = worldView.getSize();
    float currentZoom = 1.0f;
    float targetZoom = 1.0f;
    sf::Vector2f targetCenter = worldView.getCenter();

    bool panning = false;
    sf::Vector2i lastMousePx{ 0,0 };

    const float groundHeight = 60.f;
    const float groundTopY = canvasFramePx.top + canvasFramePx.height - groundHeight;
    const sf::FloatRect groundCarrierRect(0.f, groundTopY, 0.f, groundHeight);

    auto inCanvasFrame = [&](int px, int py) -> bool {
        return canvasFramePx.contains(static_cast<float>(px), static_cast<float>(py));
        };

    auto toWorld = [&](int px, int py) -> sf::Vector2f {
        return window.mapPixelToCoords({ px, py }, worldView);
        };

    auto computeGridWorld = [&](float desiredPx) {
        const float pixelsPerWorld = canvasFramePx.width / worldView.getSize().x;
        float world = desiredPx / std::max(1e-6f, pixelsPerWorld);

        float base = 1.f;
        float mant = world;
        while (mant >= 10.f) { mant /= 10.f; base *= 10.f; }
        while (mant < 1.f) { mant *= 10.f; base /= 10.f; }
        float snapped;
        if (mant < 2.f) snapped = 1.f * base;
        else if (mant < 5.f) snapped = 2.f * base;
        else snapped = 5.f * base;
        return snapped;
        };

    sf::Clock clock;

    while (window.isOpen())
    {
        const float dt = clock.restart().asSeconds();

        if (isRunning) simulationTime += dt;
        timerLabel->setText("Time: " + std::to_string(simulationTime).substr(0, 5) + "s");

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (inCanvasFrame(event.mouseWheelScroll.x, event.mouseWheelScroll.y))
                {
                    float factor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
                    targetZoom = std::clamp(targetZoom * factor, 0.2f, 10.0f);

                    sf::Vector2f before = toWorld(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                    sf::Vector2f sz = baseViewSize * targetZoom;
                    worldView.setSize(sz);
                    sf::Vector2f after = toWorld(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                    worldView.setSize(baseViewSize * currentZoom);
                    targetCenter += (before - after);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2f wpos = toWorld(event.mouseButton.x, event.mouseButton.y);

                if (event.mouseButton.button == sf::Mouse::Right && inCanvasFrame(event.mouseButton.x, event.mouseButton.y))
                {
                    objects.handleMousePress(wpos, canvasFramePx, true);
                    panning = true;
                    lastMousePx = { event.mouseButton.x, event.mouseButton.y };
                }

                if (event.mouseButton.button == sf::Mouse::Left && inCanvasFrame(event.mouseButton.x, event.mouseButton.y))
                {
                    const float BIG = 1e6f;
                    sf::FloatRect huge(-BIG, -BIG, 2 * BIG, 2 * BIG);
                    objects.handleMousePress(wpos, huge);
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Right)
                    panning = false;

                if (event.mouseButton.button == sf::Mouse::Left)
                    objects.handleMouseRelease();
            }

            if (event.type == sf::Event::MouseMoved)
            {
                sf::Vector2f wpos = toWorld(event.mouseMove.x, event.mouseMove.y);
                objects.handleMouseDrag(wpos);

                if (panning)
                {
                    sf::Vector2f prevW = toWorld(lastMousePx.x, lastMousePx.y);
                    sf::Vector2f currW = toWorld(event.mouseMove.x, event.mouseMove.y);
                    targetCenter += (prevW - currW);
                    lastMousePx = { event.mouseMove.x, event.mouseMove.y };
                }
            }

            gui.handleEvent(event);
        }

        float s = std::clamp(dt * 7.5f, 0.f, 1.f);
        currentZoom = lerp(currentZoom, targetZoom, s);
        sf::Vector2f curC = worldView.getCenter();
        curC.x = lerp(curC.x, targetCenter.x, s);
        curC.y = lerp(curC.y, targetCenter.y, s);
        worldView.setCenter(curC);
        worldView.setSize(baseViewSize * currentZoom);

        window.clear();
        window.draw(background);

        sf::RectangleShape frameBack({ canvasFramePx.width, canvasFramePx.height });
        frameBack.setPosition({ canvasFramePx.left, canvasFramePx.top });
        frameBack.setFillColor(sf::Color(24, 118, 181));
        frameBack.setOutlineThickness(2.f);
        frameBack.setOutlineColor(sf::Color::Black);
        window.draw(frameBack);

        applyViewport();
        window.setView(worldView);

        sf::Vector2f vC = worldView.getCenter();
        sf::Vector2f vS = worldView.getSize();
        sf::RectangleShape worldBg(vS);
        worldBg.setPosition(vC - vS * 0.5f);
        worldBg.setFillColor(sf::Color(148, 134, 227));
        window.draw(worldBg);

        sf::RectangleShape ground({ vS.x, groundHeight });
        ground.setPosition({ vC.x - vS.x * 0.5f, groundTopY });
        ground.setFillColor(sf::Color(51, 45, 87));
        ground.setOutlineThickness(2.f);
        ground.setOutlineColor(sf::Color::Black);
        window.draw(ground);

        // Grid drawing
        if (gridSlider && gridSlider->getValue() > 0.5f)
        {
            const float step = computeGridWorld(60.f);
            const float L = vC.x - vS.x * 0.5f;
            const float R = vC.x + vS.x * 0.5f;
            const float T = vC.y - vS.y * 0.5f;
            const float B = vC.y + vS.y * 0.5f;

            auto firstMultiple = [&](float start) { return std::floor(start / step) * step; };
            sf::Color lineColor(18, 30, 44);

            for (float x = firstMultiple(L); x <= R; x += step) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(x, T), lineColor),
                    sf::Vertex(sf::Vector2f(x, B), lineColor)
                };
                window.draw(line, 2, sf::Lines);
            }

            for (float y = firstMultiple(T); y <= B; y += step) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(L, y), lineColor),
                    sf::Vertex(sf::Vector2f(R, y), lineColor)
                };
                window.draw(line, 2, sf::Lines);
            }
        }

        objects.update(dt, isRunning, groundCarrierRect, groundHeight);
        objects.draw(window);

        window.setView(window.getDefaultView());
        gui.draw();
        window.display();
    }

    return 0;
}