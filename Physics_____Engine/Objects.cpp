#include "Objects.hpp"
#include <cmath>
#include <algorithm>

static const float gravity = 9.81f * 50.f;

Objects::Objects(tgui::Gui& guiRef, sf::RenderWindow& winRef)
    : gui(guiRef), window(winRef) {
}

void Objects::handleBoxClick() {
    if (popup && popup->isVisible()) return;

    popup = tgui::ChildWindow::create("Select Object");
    popup->setSize(200, 180);
    popup->setPosition({ 300.f, 200.f });
    popup->getRenderer()->setBackgroundColor(tgui::Color(18, 26, 38));
    popup->getRenderer()->setBorderColor(tgui::Color(74, 106, 148));
    popup->getRenderer()->setBorders(2);
    popup->onClose([this]() { popup = nullptr; });
    gui.add(popup);

    auto btnCircle = tgui::Button::create("Circle");
    btnCircle->setSize({ 160.f, 40.f });
    btnCircle->setPosition({ 20.f, 20.f });
    popup->add(btnCircle);
    btnCircle->onPress([this]() { startCreatingObject(ObjectType::Circle); popup->close(); });

    auto btnRect = tgui::Button::create("Rectangle");
    btnRect->setSize({ 160.f, 40.f });
    btnRect->setPosition({ 20.f, 70.f });
    popup->add(btnRect);
    btnRect->onPress([this]() { startCreatingObject(ObjectType::Rectangle); popup->close(); });

    auto btnTri = tgui::Button::create("Triangle");
    btnTri->setSize({ 160.f, 40.f });
    btnTri->setPosition({ 20.f, 120.f });
    popup->add(btnTri);
    btnTri->onPress([this]() { startCreatingObject(ObjectType::Triangle); popup->close(); });
}

void Objects::startCreatingObject(ObjectType type) {
    pendingType = type;
    creatingObject = false;
}

void Objects::handleMousePress(const sf::Vector2f& pos, const sf::FloatRect& canvasRect, bool editMode) {
    const float groundHeight = 60.f;
    const float groundY = canvasRect.top + canvasRect.height - groundHeight;

    if (editMode) {
        if (pos.y >= groundY && canvasRect.contains(pos)) {
            openFrictionPopup();
            return;
        }

        for (size_t i = 0; i < objects.size(); ++i) {
            if (objects[i].shape && objects[i].shape->getGlobalBounds().contains(pos)) {
                openVelocityPopup(i);
                return;
            }
        }
        return;
    }

    if (pendingType == ObjectType::None || !canvasRect.contains(pos)) return;

    creatingObject = true;
    currentCanvasRect = canvasRect;

    startPos = {
        std::clamp(pos.x, canvasRect.left, canvasRect.left + canvasRect.width),
        std::clamp(pos.y, canvasRect.top, canvasRect.top + canvasRect.height)
    };

    const sf::Color fillColor(10, 26, 47);
    const sf::Color outlineColor = sf::Color::Black;

    switch (pendingType) {
    case ObjectType::Circle: {
        auto circle = std::make_unique<sf::CircleShape>(1.f);
        circle->setFillColor(fillColor);
        circle->setOutlineColor(outlineColor);
        circle->setOutlineThickness(3.f);
        circle->setPosition(startPos);
        tempObject.shape = std::move(circle);
        break;
    }
    case ObjectType::Rectangle: {
        auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(1.f, 1.f));
        rect->setFillColor(fillColor);
        rect->setOutlineColor(outlineColor);
        rect->setOutlineThickness(3.f);
        rect->setPosition(startPos);
        tempObject.shape = std::move(rect);
        break;
    }
    case ObjectType::Triangle: {
        auto tri = std::make_unique<sf::ConvexShape>(3);
        tri->setPoint(0, { 0.f, 1.f });
        tri->setPoint(1, { 0.5f, 0.f });
        tri->setPoint(2, { 1.f, 1.f });
        tri->setFillColor(fillColor);
        tri->setOutlineColor(outlineColor);
        tri->setOutlineThickness(3.f);
        tri->setPosition(startPos);
        tempObject.shape = std::move(tri);
        break;
    }
    case ObjectType::None:
    default:
        break;
    }

    tempObject.velocity = { 0.f, 0.f };
    tempObject.elasticity = 0.5f;
    tempObject.mass = 1.f;
}

// --- Drag & Release ---
void Objects::handleMouseDrag(const sf::Vector2f& pos) {
    if (!creatingObject || !tempObject.shape) return;

    const float left = currentCanvasRect.left;
    const float top = currentCanvasRect.top;
    const float right = left + currentCanvasRect.width;
    const float bottom = top + currentCanvasRect.height;

    sf::Vector2f clampedPos{ std::clamp(pos.x, left, right), std::clamp(pos.y, top, bottom) };
    sf::Vector2f delta = clampedPos - startPos;

    if (pendingType == ObjectType::Circle) {
        auto* circle = dynamic_cast<sf::CircleShape*>(tempObject.shape.get());
        if (!circle) return;

        float requestedR = 0.5f * std::sqrt(delta.x * delta.x + delta.y * delta.y);
        float maxR = std::min(std::min(startPos.x - left, right - startPos.x), std::min(startPos.y - top, bottom - startPos.y));
        float r = std::max(0.f, std::min(requestedR, maxR));
        circle->setRadius(r);
        circle->setOrigin(r, r);
        circle->setPosition(startPos);
    }
    else if (pendingType == ObjectType::Rectangle) {
        auto* rect = dynamic_cast<sf::RectangleShape*>(tempObject.shape.get());
        if (!rect) return;

        float desiredW = std::min(std::abs(delta.x), delta.x >= 0 ? right - startPos.x : startPos.x - left);
        float desiredH = std::min(std::abs(delta.y), delta.y >= 0 ? bottom - startPos.y : startPos.y - top);

        sf::Vector2f finalPos = startPos;
        if (delta.x < 0) finalPos.x -= desiredW;
        if (delta.y < 0) finalPos.y -= desiredH;

        rect->setSize({ desiredW, desiredH });
        rect->setPosition(finalPos);
    }
    else if (pendingType == ObjectType::Triangle) {
        auto* tri = dynamic_cast<sf::ConvexShape*>(tempObject.shape.get());
        if (!tri) return;

        float desiredW = std::min(std::abs(delta.x), delta.x >= 0 ? right - startPos.x : startPos.x - left);
        float desiredH = std::min(std::abs(delta.y), delta.y >= 0 ? bottom - startPos.y : startPos.y - top);

        tri->setPoint(0, { 0.f, desiredH });
        tri->setPoint(1, { desiredW / 2.f, 0.f });
        tri->setPoint(2, { desiredW, desiredH });

        sf::Vector2f finalPos = startPos;
        if (delta.x < 0) finalPos.x -= desiredW;
        if (delta.y < 0) finalPos.y -= desiredH;

        tri->setPosition(finalPos);
    }
}

void Objects::handleMouseRelease() {
    if (creatingObject && tempObject.shape) {
        objects.push_back(std::move(tempObject));
        openVelocityPopup(objects.size() - 1);
    }
    creatingObject = false;
    pendingType = ObjectType::None;
}

// --- Draw ---
void Objects::draw(sf::RenderWindow& window) {
    for (auto& obj : objects) {
        if (obj.shape) window.draw(*obj.shape);
    }

    if (creatingObject && tempObject.shape) window.draw(*tempObject.shape);

    // Path tracing for the most recently selected object
    if (pathTracingEnabled && tracedObjectIndex >= 0 && tracedObjectIndex < static_cast<int>(objects.size())) {
        auto& obj = objects[tracedObjectIndex];
        if (obj.shape) trajectoryCurve.append(sf::Vertex(obj.shape->getPosition(), sf::Color::Red));
        window.draw(trajectoryCurve);
    }

    if (rangeLineEnabled && rangeObjectIndex >= 0 && rangeObjectIndex < static_cast<int>(objects.size())) {
        auto& obj = objects[rangeObjectIndex];
        sf::Vector2f pos = obj.shape->getPosition();

        if (auto* circle = dynamic_cast<sf::CircleShape*>(obj.shape.get()))
            pos.x += circle->getRadius();
        else if (auto* rect = dynamic_cast<sf::RectangleShape*>(obj.shape.get()))
            pos.x += rect->getSize().x / 2.f;
        else if (auto* tri = dynamic_cast<sf::ConvexShape*>(obj.shape.get()))
            pos.x += (tri->getPoint(2).x - tri->getPoint(0).x) / 2.f;

        rangeLine.clear();
        rangeLine.append(sf::Vertex(rangeStartPos, sf::Color::Red));
        rangeLine.append(sf::Vertex({ pos.x, rangeLineY }, sf::Color::Red));
        window.draw(rangeLine);
    }
}
// --- Update ---
void Objects::update(float dt, bool isRunning, const sf::FloatRect& canvasRect, float groundHeight) {
    if (!isRunning) return;
    const float groundY = canvasRect.top + canvasRect.height - groundHeight;

    for (size_t i = 0; i < objects.size(); ++i) {
        auto& obj = objects[i];
        if (!obj.shape) continue;

        obj.velocity.y += gravity * dt;
        obj.shape->move(obj.velocity * dt);

        sf::FloatRect b = obj.shape->getGlobalBounds();
        if (b.top + b.height >= groundY) {
            float dy = groundY - (b.top + b.height);
            obj.shape->move(0.f, dy);

            obj.velocity.y = -obj.velocity.y * obj.elasticity;
            obj.velocity.x *= (1.f - groundFriction);

            if (std::abs(obj.velocity.y) < 1.f) obj.velocity.y = 0.f;
            if (std::abs(obj.velocity.x) < 1.f) obj.velocity.x = 0.f;
        }

        for (size_t j = i + 1; j < objects.size(); ++j) {
            auto& A = objects[i];
            auto& B = objects[j];
            if (!A.shape || !B.shape) continue;

            if (A.shape->getGlobalBounds().intersects(B.shape->getGlobalBounds())) {
                sf::Vector2f posA = A.shape->getPosition();
                sf::Vector2f posB = B.shape->getPosition();

                sf::Vector2f n = posB - posA;
                float dist2 = n.x * n.x + n.y * n.y;
                if (dist2 == 0.f) continue;

                float dist = std::sqrt(dist2);
                n /= dist;

                sf::Vector2f rel = B.velocity - A.velocity;
                float relVel = rel.x * n.x + rel.y * n.y;

                if (relVel > 0.f) continue;

                float e = std::min(A.elasticity, B.elasticity);
                float invMassSum = (1.f / A.mass) + (1.f / B.mass);
                if (invMassSum <= 0.f) continue;

                float jVal = -(1.f + e) * relVel / invMassSum;
                sf::Vector2f impulse = jVal * n;

                A.velocity -= (1.f / A.mass) * impulse;
                B.velocity += (1.f / B.mass) * impulse;

                const float penetration = 1.f;
                A.shape->move(-n * penetration * 0.5f);
                B.shape->move(n * penetration * 0.5f);
            }
        }
    }
}

// --- Velocity Popup ---
void Objects::openVelocityPopup(size_t index) {
    if (velPopup && velPopup->isVisible()) return;

    velPopup = tgui::ChildWindow::create("Velocity / Angle / Elasticity / Mass");
    velPopup->setSize({ 240.f, 300.f });
    velPopup->setPosition({ 400.f, 250.f });
    velPopup->getRenderer()->setBackgroundColor(tgui::Color(18, 26, 38));
    velPopup->getRenderer()->setBorderColor(tgui::Color(74, 106, 148));
    velPopup->getRenderer()->setBorders(2);
    velPopup->onClose([this]() { velPopup = nullptr; });
    gui.add(velPopup);

    speedBox = tgui::EditBox::create();
    speedBox->setSize({ 200.f, 30.f });
    speedBox->setPosition({ 20.f, 20.f });
    speedBox->setText("500");
    velPopup->add(speedBox);

    angleBox = tgui::EditBox::create();
    angleBox->setSize({ 200.f, 30.f });
    angleBox->setPosition({ 20.f, 60.f });
    angleBox->setText("45");
    velPopup->add(angleBox);

    elasticityBox = tgui::EditBox::create();
    elasticityBox->setSize({ 200.f, 30.f });
    elasticityBox->setPosition({ 20.f, 100.f });
    elasticityBox->setText("0.5");
    velPopup->add(elasticityBox);

    massBox = tgui::EditBox::create();
    massBox->setSize({ 200.f, 30.f });
    massBox->setPosition({ 20.f, 140.f });
    massBox->setText("1.0");
    velPopup->add(massBox);

    auto applyBtn = tgui::Button::create("Apply");
    applyBtn->setSize({ 200.f, 30.f });
    applyBtn->setPosition({ 20.f, 180.f });
    velPopup->add(applyBtn);

    auto deleteBtn = tgui::Button::create("Delete");
    deleteBtn->setSize({ 200.f, 30.f });
    deleteBtn->setPosition({ 20.f, 220.f });
    deleteBtn->getRenderer()->setTextColor(tgui::Color::Red);
    velPopup->add(deleteBtn);

    applyBtn->onPress([this, index]() {
        if (index >= objects.size()) return;

        float speed = speedBox->getText().toFloat();
        float angle = angleBox->getText().toFloat() * 3.14159265f / 180.f;
        float elasticity = std::clamp(elasticityBox->getText().toFloat(), 0.f, 1.f);
        float mass = std::max(0.01f, massBox->getText().toFloat());

        objects[index].velocity.x = speed * std::cos(angle);
        objects[index].velocity.y = -speed * std::sin(angle);
        objects[index].elasticity = elasticity;
        objects[index].mass = mass;

        if (velPopup) velPopup->close();
        });

    deleteBtn->onPress([this, index]() {
        if (index < objects.size()) objects.erase(objects.begin() + static_cast<std::ptrdiff_t>(index));
        if (velPopup) velPopup->close();
        });
}

// --- Friction Popup ---
void Objects::openFrictionPopup() {
    if (frictionPopup && frictionPopup->isVisible()) return;

    frictionPopup = tgui::ChildWindow::create("Set Ground Friction");
    frictionPopup->setSize({ 220.f, 120.f });
    frictionPopup->setPosition({ 400.f, 300.f });
    frictionPopup->getRenderer()->setBackgroundColor(tgui::Color(18, 26, 38));
    frictionPopup->getRenderer()->setBorderColor(tgui::Color(74, 106, 148));
    frictionPopup->getRenderer()->setBorders(2);
    frictionPopup->onClose([this]() { frictionPopup = nullptr; });
    gui.add(frictionPopup);

    frictionBox = tgui::EditBox::create();
    frictionBox->setSize({ 180.f, 30.f });
    frictionBox->setPosition({ 20.f, 20.f });
    frictionBox->setText(std::to_string(groundFriction));
    frictionPopup->add(frictionBox);

    auto applyBtn = tgui::Button::create("Apply");
    applyBtn->setSize({ 180.f, 30.f });
    applyBtn->setPosition({ 20.f, 60.f });
    frictionPopup->add(applyBtn);

    applyBtn->onPress([this]() {
        groundFriction = std::clamp(frictionBox->getText().toFloat(), 0.f, 1.f);
        if (frictionPopup) frictionPopup->close();
        });
}

// --- Path Tracing ---
void Objects::enablePathTracing() {
    if (objects.empty()) return;
    pathTracingEnabled = true;
    tracedObjectIndex = static_cast<int>(objects.size()) - 1;
    trajectoryCurve.clear();
}

// --- Range Line ---
void Objects::toggleRangeLine() {
    if (objects.empty()) return;

    rangeLineEnabled = !rangeLineEnabled;
    if (rangeLineEnabled) {
        rangeObjectIndex = static_cast<int>(objects.size()) - 1;
        rangeStartPos = objects[rangeObjectIndex].shape->getPosition();
        rangeLineY = rangeStartPos.y;
        currentRangeX = rangeStartPos.x;
        rangeActive = true;
    }
    else {
        rangeObjectIndex = -1;
        rangeActive = false;
    }
    rangeLine.clear();
}

