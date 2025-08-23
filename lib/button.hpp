#pragma once

#include "common.hpp"

class Button {
public:
  template <typename... Args>
  explicit Button(std::shared_ptr<Command> cmd, Args... obj);
  virtual ~Button() = default;

  [[nodiscard]] virtual bool IsPressed(const Event& event) const;
  [[nodiscard]] const std::shared_ptr<Command>& GetCommand() const;
  deque<DrawObject>& GetShapes();

protected:
  shared_ptr<Command> m_cmd;
  deque<DrawObject> m_draw;
};

class MouseButton final : public Button {
public:
  template <typename... Args>
  MouseButton(const sf::Mouse::Button& btn, std::shared_ptr<Command> cmd, Args... obj);

  [[nodiscard]] bool IsPressed(const Event& event) const override;

protected:
  sf::Mouse::Button m_btn;

  [[nodiscard]] bool Inside(const Vector2i& mouse) const;
};

class KeyboardButton final : public Button {
public:
  template <typename... Args>
  explicit KeyboardButton(std::shared_ptr<Command> cmd, Args... obj);

  [[nodiscard]] bool IsPressed(const Event& event) const override;
};

template <typename... Args>
Button::Button(std::shared_ptr<Command> cmd, Args... obj) : m_cmd(std::move(cmd)) {
  (..., m_draw.push_back(std::move(obj)));
}

template <typename... Args>
MouseButton::MouseButton(const sf::Mouse::Button& btn, std::shared_ptr<Command> cmd, Args... obj)
  : Button(std::move(cmd)), m_btn(btn) {
  (..., m_draw.push_back(std::move(obj)));
}

template <typename... Args>
KeyboardButton::KeyboardButton(std::shared_ptr<Command> cmd, Args... obj) : Button(std::move(cmd)) {
  (..., m_draw.push_back(std::move(obj)));
}