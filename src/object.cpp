#include "../lib/object.hpp"

DrawObject::DrawObject(shared_ptr<sf::Drawable> spriten) : sprite(std::move(spriten)) {}

DrawObject::DrawObject(shared_ptr<sf::Drawable> spriten, bool shown)
    : sprite(std::move(spriten)), show(shown) {}

TextObject::TextObject(const string& str, size_t size, const Color& color, const Vector2u& pos,
                       const sf::Font& font, int style, bool show)
    : DrawObject(std::make_shared<Text>(str, font, size), show) {
  static_cast<Text*>(sprite.get())->setFillColor(color);
  static_cast<Text*>(sprite.get())->setPosition(Vector2f(pos));
  static_cast<Text*>(sprite.get())->setStyle(style);
}

RectObject::RectObject(const Vector2u& size, const Color& color, const Vector2u& pos, bool show)
    : DrawObject(std::make_shared<RectangleShape>(Vector2f(size)), show) {
  static_cast<RectangleShape*>(sprite.get())->setFillColor(color);
  static_cast<RectangleShape*>(sprite.get())->setPosition(Vector2f(pos));
}
