#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <memory>
#include <vector>

enum class ObjectType { None, Circle, Rectangle, Triangle };

struct PhysicsObject {
    std::unique_ptr<sf::Shape> shape;
    sf::Vector2f velocity{};
};

class Objects {
public:
    Objects(tgui::Gui& guiRef, sf::RenderWindow& winRef);

    void handleBoxClick();
    void startCreatingObject(ObjectType type);

    // Creation / interaction
    void handleMousePress(const sf::Vector2f& pos, const sf::FloatRect& canvasRect);
    void handleMouseDrag(const sf::Vector2f& pos); // keep same signature
    void handleMouseRelease();

    // Render & simulate
    void draw(sf::RenderWindow& window);
    void update(float dt, bool isRunning, const sf::FloatRect& canvasRect, float groundHeight);

private:
    static float clampf(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }
    static bool hitTestShape(const sf::Shape& s, const sf::Vector2f& p) {
        return s.getGlobalBounds().contains(p);
    }
    void openVelocityPopup(std::size_t index);

private:
    tgui::Gui& gui;
    sf::RenderWindow& window;

    std::vector<PhysicsObject> objects;
    PhysicsObject tempObject;

    bool creatingObject = false;
    ObjectType pendingType = ObjectType::None;

    sf::Vector2f startPos{};
    sf::FloatRect currentCanvasRect{};

    // Existing object-type chooser popup
    tgui::ChildWindow::Ptr popup = nullptr;

    // New: velocity popup
    tgui::ChildWindow::Ptr velPopup = nullptr;
    tgui::EditBox::Ptr speedBox;
    tgui::EditBox::Ptr angleBox;
};
