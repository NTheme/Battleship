#pragma once

#include "common.hpp"

class Button {
 public:
  template <typename... Args>
  Button(std::shared_ptr<Command> cmd, Args... obj);
  virtual ~Button() = default;

  virtual bool IsPressed(const Event& event) const;
  const std::shared_ptr<Command>& GetCommand() const;
  deque<DrawObject>& GetShapes();

 protected:
  shared_ptr<Command> m_cmd;
  deque<DrawObject> m_draw;
};

class MouseButton final : public Button {
 public:
  template <typename... Args>
  MouseButton(const Mouse::Button& btn, std::shared_ptr<Command> cmd, Args... obj);

  bool IsPressed(const Event& event) const final;

 protected:
  Mouse::Button m_btn;

  bool Inside(const Vector2i& mouse) const;
};

class KeyboardButton final : public Button {
 public:
  template <typename... Args>
  KeyboardButton(std::shared_ptr<Command> cmd, Args... obj);

  bool IsPressed(const Event& event) const final;
};

template <typename... Args>
Button::Button(std::shared_ptr<Command> cmd, Args... obj) : m_cmd(std::move(cmd)) {
  (..., m_draw.push_back(std::move(obj)));
}

template <typename... Args>
MouseButton::MouseButton(const Mouse::Button& btn, std::shared_ptr<Command> cmd, Args... obj)
    : Button(std::move(cmd)), m_btn(btn) {
  (..., m_draw.push_back(std::move(obj)));
}

template <typename... Args>
KeyboardButton::KeyboardButton(std::shared_ptr<Command> cmd, Args... obj) : Button(std::move(cmd)) {
  (..., m_draw.push_back(std::move(obj)));
}
