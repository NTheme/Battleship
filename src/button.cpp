#include "../lib/button.hpp"

#include "../lib/command.hpp"
#include "../lib/object.hpp"

bool Button::IsPressed(const Event& event) const {
  return m_cmd != nullptr && event.type == m_cmd->GetType();
}

const std::shared_ptr<Command>& Button::GetCommand() const { return m_cmd; }

deque<DrawObject>& Button::GetShapes() { return m_draw; }

bool MouseButton::IsPressed(const Event& event) const {
  return m_cmd != nullptr && event.type == Event::MouseButtonPressed &&
         Inside(Mouse::getPosition());
}

bool MouseButton::Inside(const Vector2i& mouse) const {
  RectangleShape* shape = nullptr;
  if (m_draw.empty() ||
      (shape = dynamic_cast<RectangleShape*>(m_draw[0].sprite.get())) == nullptr) {
    return false;
  }

  auto left = shape->getPosition();
  auto size = shape->getSize();
  auto right = Vector2f(left.x + size.x, left.y + size.y);
  return mouse.x >= left.x && mouse.x <= right.x && mouse.y >= left.y && mouse.y <= right.y;
}

bool KeyboardButton::IsPressed(const Event& event) const {
  return m_cmd != nullptr && event.type == Event::TextEntered;
}
