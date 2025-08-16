#include "Objects.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

static const float gravity = 9.81f * 50.f; // 50 px ~ 1 m

Objects::Objects(tgui::Gui& guiRef, sf::RenderWindow& winRef)
    : gui(guiRef), window(winRef)
{
}

// Show shape selection popup
void Objects::handleBoxClick()
{
    if (popup && popup->isVisible())
        return;

    popup = tgui::ChildWindow::create("Select Object");
    popup->setSize(200.f, 180.f);
    popup->setPosition(300.f, 200.f);
    popup->getRenderer()->setBackgroundColor(tgui::Color(18, 26, 38));
    popup->getRenderer()->setBorderColor(tgui::Color(74, 106, 148));
    popup->getRenderer()->setBorders(2.f);
    popup->onClose([this]() { popup = nullptr; });
    gui.add(popup);

    auto btnCircle = tgui::Button::create("Circle");
    btnCircle->setSize(160.f, 40.f);
    btnCircle->setPosition(20.f, 20.f);
    popup->add(btnCircle);
    btnCircle->onPress([this]() { startCreatingObject(ObjectType::Circle); popup->close(); });

    auto btnRect = tgui::Button::create("Rectangle");
    btnRect->setSize(160.f, 40.f);
    btnRect->setPosition(20.f, 70.f);
    popup->add(btnRect);
    btnRect->onPress([this]() { startCreatingObject(ObjectType::Rectangle); popup->close(); });

    auto btnTri = tgui::Button::create("Triangle");
    btnTri->setSize(160.f, 40.f);
    btnTri->setPosition(20.f, 120.f);
    popup->add(btnTri);
    btnTri->onPress([this]() { startCreatingObject(ObjectType::Triangle); popup->close(); });
}

// Called after selecting shape
void Objects::startCreatingObject(ObjectType type)
{
    pendingType = type;
    creatingObject = false; // creation begins on canvas click
}

// Mouse press on canvas
void Objects::handleMousePress(const sf::Vector2f& pos, const sf::FloatRect& canvasRect)
{
    if (pendingType == ObjectType::None || !canvasRect.contains(pos))
        return;

    creatingObject = true;
    currentCanvasRect = canvasRect;

    // Clamp start position inside canvas
    startPos = {
        clampf(pos.x, currentCanvasRect.left, currentCanvasRect.left + currentCanvasRect.width),
        clampf(pos.y, currentCanvasRect.top,  currentCanvasRect.top + currentCanvasRect.height)
    };

    sf::Color fillColor(10, 26, 47);
    sf::Color outlineColor = sf::Color::Black;

    switch (pendingType)
    {
    case ObjectType::Circle:
        tempObject.shape = std::make_unique<sf::CircleShape>(1.f);
        break;
    case ObjectType::Rectangle:
        tempObject.shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(1.f, 1.f));
        break;
    case ObjectType::Triangle:
    {
        auto tri = std::make_unique<sf::ConvexShape>(3);
        tri->setPoint(0, { 0.f, 1.f });
        tri->setPoint(1, { 0.5f, 0.f });
        tri->setPoint(2, { 1.f, 1.f });
        tempObject.shape = std::move(tri);
        break;
    }
    default: break;
    }

    tempObject.shape->setFillColor(fillColor);
    tempObject.shape->setOutlineColor(outlineColor);
    tempObject.shape->setOutlineThickness(3.f);
    tempObject.shape->setPosition(startPos);
    tempObject.velocity = { 0.f, 0.f };
}

// Drag to resize object
void Objects::handleMouseDrag(const sf::Vector2f& pos)
{
    if (!creatingObject || !tempObject.shape)
        return;

    const float left = currentCanvasRect.left;
    const float top = currentCanvasRect.top;
    const float right = currentCanvasRect.left + currentCanvasRect.width;
    const float bottom = currentCanvasRect.top + currentCanvasRect.height;

    sf::Vector2f clampedPos{
        clampf(pos.x, left, right),
        clampf(pos.y, top,  bottom)
    };

    sf::Vector2f delta = clampedPos - startPos;

    if (pendingType == ObjectType::Circle)
    {
        auto* circle = dynamic_cast<sf::CircleShape*>(tempObject.shape.get());
        if (!circle) return;

        float requestedR = 0.5f * std::sqrt(delta.x * delta.x + delta.y * delta.y);

        float maxR = std::min(
            std::min(startPos.x - left, right - startPos.x),
            std::min(startPos.y - top, bottom - startPos.y)
        );
        maxR = std::max(0.f, maxR);

        float r = std::min(std::abs(requestedR), maxR);
        circle->setRadius(r);
        circle->setOrigin(r, r);
        circle->setPosition(startPos);
    }
    else if (pendingType == ObjectType::Rectangle)
    {
        auto* rect = dynamic_cast<sf::RectangleShape*>(tempObject.shape.get());
        if (!rect) return;

        float desiredW = std::abs(delta.x);
        float desiredH = std::abs(delta.y);

        if (delta.x >= 0) desiredW = std::min(desiredW, right - startPos.x);
        else              desiredW = std::min(desiredW, startPos.x - left);

        if (delta.y >= 0) desiredH = std::min(desiredH, bottom - startPos.y);
        else              desiredH = std::min(desiredH, startPos.y - top);

        sf::Vector2f finalPos = startPos;
        if (delta.x < 0) finalPos.x = startPos.x - desiredW;
        if (delta.y < 0) finalPos.y = startPos.y - desiredH;

        rect->setSize({ desiredW, desiredH });
        rect->setPosition(finalPos);
    }
    else if (pendingType == ObjectType::Triangle)
    {
        auto* tri = dynamic_cast<sf::ConvexShape*>(tempObject.shape.get());
        if (!tri) return;

        float desiredW = std::abs(delta.x);
        float desiredH = std::abs(delta.y);

        if (delta.x >= 0) desiredW = std::min(desiredW, right - startPos.x);
        else              desiredW = std::min(desiredW, startPos.x - left);

        if (delta.y >= 0) desiredH = std::min(desiredH, bottom - startPos.y);
        else              desiredH = std::min(desiredH, startPos.y - top);

        tri->setPoint(0, { 0.f,           desiredH });
        tri->setPoint(1, { desiredW / 2.f, 0.f });
        tri->setPoint(2, { desiredW,       desiredH });

        sf::Vector2f finalPos = startPos;
        if (delta.x < 0) finalPos.x = startPos.x - desiredW;
        if (delta.y < 0) finalPos.y = startPos.y - desiredH;

        tri->setPosition(finalPos);
    }
}

// Release mouse: finalize placement
void Objects::handleMouseRelease()
{
    if (creatingObject && tempObject.shape)
    {
        objects.push_back({ std::move(tempObject.shape), tempObject.velocity });
        openVelocityPopup(objects.size() - 1); // pass index, not reference
    }

    creatingObject = false;
    pendingType = ObjectType::None;
}

// Draw all objects
void Objects::draw(sf::RenderWindow& window)
{
    for (auto& obj : objects)
        window.draw(*obj.shape);

    if (creatingObject && tempObject.shape)
        window.draw(*tempObject.shape);
}

// Physics update
void Objects::update(float dt, bool isRunning, const sf::FloatRect& canvasRect, float groundHeight)
{
    if (!isRunning) return;

    const float groundY = canvasRect.top + canvasRect.height - groundHeight;

    for (auto& obj : objects)
    {
        obj.velocity.y += gravity * dt;
        obj.shape->move(obj.velocity * dt);

        sf::FloatRect b = obj.shape->getGlobalBounds();
        float bottom = b.top + b.height;
        if (bottom > groundY)
        {
            float dy = groundY - bottom;
            obj.shape->move(0.f, dy);
            obj.velocity.y = 0.f;
        }
    }
}

// Velocity popup
void Objects::openVelocityPopup(size_t index)
{
    if (index >= objects.size())
        return;

    if (velPopup)
    {
        gui.remove(velPopup);
        velPopup = nullptr;
    }

    PhysicsObject& obj = objects[index];

    velPopup = tgui::ChildWindow::create("Launch Object");
    velPopup->setSize(260.f, 190.f);
    velPopup->setPosition(360.f, 220.f);
    velPopup->getRenderer()->setBackgroundColor(tgui::Color(18, 26, 38));
    velPopup->getRenderer()->setBorderColor(tgui::Color(74, 106, 148));
    velPopup->getRenderer()->setBorders(2.f);
    velPopup->onClose([this]() { velPopup = nullptr; });
    gui.add(velPopup);

    auto lbl1 = tgui::Label::create("Speed (px/s):");
    lbl1->setPosition(16.f, 20.f);
    velPopup->add(lbl1);

    speedBox = tgui::EditBox::create();
    speedBox->setSize(120.f, 28.f);
    speedBox->setPosition(120.f, 16.f);
    speedBox->setInputValidator(tgui::EditBox::Validator::Float);
    speedBox->setText("400");
    velPopup->add(speedBox);

    auto lbl2 = tgui::Label::create("Angle (deg):");
    lbl2->setPosition(16.f, 62.f);
    velPopup->add(lbl2);

    angleBox = tgui::EditBox::create();
    angleBox->setSize(120.f, 28.f);
    angleBox->setPosition(120.f, 58.f);
    angleBox->setInputValidator(tgui::EditBox::Validator::Float);
    angleBox->setText("45");
    velPopup->add(angleBox);

    auto applyBtn = tgui::Button::create("Apply");
    applyBtn->setSize(100.f, 34.f);
    applyBtn->setPosition(30.f, 110.f);
    velPopup->add(applyBtn);

    auto cancelBtn = tgui::Button::create("Cancel");
    cancelBtn->setSize(100.f, 34.f);
    cancelBtn->setPosition(130.f, 110.f);
    velPopup->add(cancelBtn);

    cancelBtn->onPress([this]() {
        if (velPopup) { gui.remove(velPopup); velPopup = nullptr; }
        });

    applyBtn->onPress([this, index]() {
        try {
            float speed = std::stof(speedBox->getText().toStdString());
            float angle = std::stof(angleBox->getText().toStdString());
            float rad = angle * 3.14159f / 180.f;

            objects[index].velocity.x = speed * std::cos(rad);
            objects[index].velocity.y = -speed * std::sin(rad); // negative for upward
        }
        catch (...) { std::cout << "Invalid velocity input.\n"; }

        if (velPopup) { gui.remove(velPopup); velPopup = nullptr; }
        });
}
