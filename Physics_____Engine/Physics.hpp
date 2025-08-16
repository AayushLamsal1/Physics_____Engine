#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>

enum class ShapeType { Circle, Rectangle, Triangle };
enum class AppMode { Edit, Run };
enum class DialogType { None, Position, Velocity, Direction };

class PhysicsObject {
public:
    PhysicsObject(ShapeType type = ShapeType::Circle, float size = 20.f);

    void update(float dt);
    void draw(sf::RenderWindow& window);
    void applyForce(const sf::Vector2f& force);
    void setPosition(const sf::Vector2f& pos);
    void setVelocity(const sf::Vector2f& vel);
    void setDirection(float angle, float speed);

    const sf::Vector2f& getPosition() const;
    const sf::Vector2f& getVelocity() const;

    void reset();
    bool isOffScreen(const sf::RenderWindow& window);
    bool checkGroundCollision(float groundLevel);
    bool contains(const sf::Vector2f& point) const;

private:
    ShapeType shapeType;
    sf::CircleShape circleShape;
    sf::RectangleShape rectangleShape;
    sf::ConvexShape triangleShape;

    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;

    float size;
    bool onGround;

    void initTriangle(float size);
    void updateShapePositions();
};

class UI {
public:
    UI(sf::RenderWindow& window);

    void handleClick(const sf::Vector2f& mousePos, AppMode& mode, ShapeType& selectedShape,
        std::vector<PhysicsObject>& objects, DialogType& currentDialog, int& selectedObjectIndex);
    void handleHover(const sf::Vector2f& mousePos);
    void processInput(const std::string& input, DialogType dialogType,
        std::vector<PhysicsObject>& objects, int selectedObjectIndex);
    void draw(sf::RenderWindow& window, AppMode mode, ShapeType selectedShape);
    void drawContextMenu(sf::RenderWindow& window, const sf::Vector2f& position);
    void drawInputDialog(sf::RenderWindow& window, DialogType dialogType, const std::string& currentInput);

    bool isInToolPanel(const sf::Vector2f& mousePos) const;
    bool isShapeMenuOpen() const;
    void setContextMenuVisible(bool visible, const sf::Vector2f& position = { 0, 0 });

private:
    sf::RenderWindow& window;

    // UI elements
    sf::RectangleShape toolPanel;
    sf::RectangleShape runButton;
    std::vector<sf::RectangleShape> toolButtons;
    sf::RectangleShape shapeMenu;
    std::vector<sf::RectangleShape> shapeOptions;

    // Context menu
    sf::RectangleShape contextMenu;
    std::vector<sf::RectangleShape> contextOptions;
    bool contextMenuVisible;
    sf::Vector2f contextMenuPos;

    // Input dialog
    sf::RectangleShape inputDialog;

    // State
    bool shapeMenuOpen;
    int hoveredButton;
    int hoveredShapeOption;
    int hoveredContextOption;

    void initializeUI();
    void drawLetter(sf::RenderWindow& window, char letter, const sf::Vector2f& position, float size);
    void drawText(sf::RenderWindow& window, const std::string& text, const sf::Vector2f& position, float size);
};