#pragma once

#include "common.hpp"

class Push {
 public:
  Push();

  template <typename Type, typename... Args>
  void Set(string scene, string obj, Args... param);

  const map<string, shared_ptr<Button>>& Data(const string& scene) const;
  Button* GetPressed(const string& scene, const Event& event);
  Button* Get(const string& scene, const string& obj);

  void Config(array<Player, 2>& players, const Vector2u& size, const map<string, Music>& music,
              const map<string, string>& boxes);

 private:
  Font m_font;
  Texture m_bg;
  map<string, map<string, shared_ptr<Button>>> m_buttons;

  void ConfigMainMenu();
  void ConfigSettings(const map<string, Music>& music, const map<string, string>& boxes);
  void ConfigVolume(const map<string, Music>& music);
  void ConfigPort(const map<string, string>& boxes);
  void ConfigPlayMenu();
  void ConfigClient();
  void ConfigServer();
  void ConfigField(array<Player, 2>& players, const Vector2u& size);
  void ConfigPlay(array<Player, 2>& players);
};

template <typename Type, typename... Args>
void Push::Set(string scene, string obj, Args... param) {
  if (scene == "any") {
    for (auto& scr : m_buttons) {
      scr.second[obj] = make_shared<Type>(param...);
    }
  } else {
    m_buttons[scene][obj] = make_shared<Type>(param...);
  }
}
