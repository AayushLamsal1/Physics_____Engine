#pragma once
#include <SFML/Graphics.hpp>

class Ground
{
private:
    sf::RectangleShape rect;

public:
    Ground(float x, float y, float width, float height)
    {
        rect.setSize({ width,height });
        rect.setPosition(x, y);
        rect.setFillColor(sf::Color(180, 180, 180));
    }

    void draw(sf::RenderWindow& window) { window.draw(rect); }
    float getY() const { return rect.getPosition().y; }
    sf::FloatRect getBounds() const { return rect.getGlobalBounds(); }
};
