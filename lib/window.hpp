#pragma once

#include "common.hpp"
#include "push.hpp"

class GameWindow : public sf::RenderWindow {
 public:
  GameWindow(array<Player, 2>& players, const Vector2u& size);
  ~GameWindow();

  const std::shared_ptr<Command>& GetCommand();
  Push& GetButtons();
  Event& GetEvent();
  Music& GetMusic(const string& elem);
  map<string, string>& GetBoxes();
  void SetButtons(const string& str);
  void SetObject(const string& scene, const string& elem, size_t index, const string& str);
  void SetShow(const string& scene, const string& elem, bool show, int index = -1);
  void DrawObjects();
  void SetVolume(CMDVolume type);
  void Ficha();

 private:
  View m_view;
  Event m_event;

  Movie m_movie;
  map<string, Music> m_music;
  map<string, string> m_boxes;
  Push m_push;
};
