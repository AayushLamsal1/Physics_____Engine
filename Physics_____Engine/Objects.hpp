#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>

// ---------------------
// ---------------------
struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime = 0.f;
};


enum class ObjectType { None, Circle, Rectangle, Triangle };


// ---------------------
struct PhysicsObject {
    std::unique_ptr<sf::Shape> shape;
    sf::Vector2f velocity{};
    float elasticity = 0.5f;
    float mass = 1.f;

    // Animation / Effects
    float flashTimer = 0.f;
    float squashScale = 1.f;
    std::vector<Particle> particles;
};

// ---------------------
// ---------------------
struct TriggerLine {
    sf::RectangleShape lineShape;
    sf::Vector2f start;
    sf::Vector2f end;
    bool selected = false;

    TriggerLine() = default;
    TriggerLine(const sf::Vector2f& s, const sf::Vector2f& e)
        : start(s), end(e)
    {
        lineShape.setSize({ std::hypot(e.x - s.x, e.y - s.y), 2.f });
        lineShape.setPosition(s);
        float angle = std::atan2(e.y - s.y, e.x - s.x) * 180.f / 3.14159265f;
        lineShape.setRotation(angle);
        lineShape.setFillColor(sf::Color::Red);
    }

    bool containsPoint(const sf::Vector2f& point) const {
        return lineShape.getGlobalBounds().contains(point);
    }
};

// ---------------------
// Objects Manager
// ---------------------
class Objects {
public:
    Objects(tgui::Gui& guiRef, sf::RenderWindow& winRef);

    // Object creation & interaction
    void handleBoxClick();
    void startCreatingObject(ObjectType type);

    // Mouse interactions
    void handleMousePress(const sf::Vector2f& pos, const sf::FloatRect& canvasRect, bool editMode = false);
    void handleMouseDrag(const sf::Vector2f& pos);
    void handleMouseRelease();

    // Update & Draw
    void draw(sf::RenderWindow& window);
    void update(float dt, bool isRunning, const sf::FloatRect& canvasRect, float groundHeight = 60.f);

    const std::vector<PhysicsObject>& getObjects() const { return objects; }

    // Path tracing
    void enablePathTracing();

    // Range line toggle
    void toggleRangeLine();

    // Trigger lines
    void addTriggerLine(const sf::Vector2f& start, const sf::Vector2f& end);
    bool editTriggerMode = false;

private:
    static float clampf(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }

    // Popups
    void openVelocityPopup(size_t index);
    void openFrictionPopup();

    // Collision handling
    void resolveCollision(PhysicsObject& A, PhysicsObject& B);

    // Trigger effects
    void triggerCollisionEffects(PhysicsObject& obj, const sf::Vector2f& impactDir, float impactStrength);

private:
    tgui::Gui& gui;
    sf::RenderWindow& window;

    std::vector<PhysicsObject> objects;
    PhysicsObject tempObject;

    bool creatingObject = false;
    ObjectType pendingType = ObjectType::None;

    sf::Vector2f startPos{};
    sf::FloatRect currentCanvasRect{};
    tgui::ChildWindow::Ptr popup = nullptr;

    // Velocity popup
    tgui::ChildWindow::Ptr velPopup = nullptr;
    tgui::EditBox::Ptr speedBox;
    tgui::EditBox::Ptr angleBox;
    tgui::EditBox::Ptr elasticityBox;
    tgui::EditBox::Ptr massBox;

    // Friction popup
    tgui::ChildWindow::Ptr frictionPopup = nullptr;
    tgui::EditBox::Ptr frictionBox;
    float groundFriction = 0.f;

    // Path tracing
    bool pathTracingEnabled = false;
    int tracedObjectIndex = -1;
    sf::VertexArray trajectoryCurve{ sf::LinesStrip };

    // Range line
    bool rangeLineEnabled = false;
    int rangeObjectIndex = -1;
    sf::VertexArray rangeLine{ sf::Lines };
    bool rangeActive = false;
    sf::Vector2f rangeStartPos{};
    float currentRangeX = 0.f;
    float maxRangeX = 0.f;
    float rangeLineY = 0.f;
    std::vector<sf::VertexArray> completedRangeLines;

    // Trigger lines
    std::vector<TriggerLine> triggerLines;
    TriggerLine* selectedLine = nullptr;
};
