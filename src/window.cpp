#include "../lib/window.hpp"

#include "../lib/button.hpp"
#include "../lib/object.hpp"

GameWindow::GameWindow(array<Player, 2>& players, const Vector2u& size) {
  create(VideoMode(1920, 1080), kName, sf::Style::Fullscreen);
  m_view.setSize(Vector2f(getSize()));
  m_view.setCenter(Vector2f(m_view.getSize().x / 2, m_view.getSize().y / 2));
  setView(m_view);

  if (!m_music["game"].openFromFile(bs::Path() + kRes + "ficha2.what")) {
    throw std::runtime_error("Cannot load ficha");
  }
  if (!m_music["main"].openFromFile(bs::Path() + kRes + "ficha1.what")) {
    throw std::runtime_error("Cannot load fichaaaa");
  }
  if (!m_movie.openFromFile(bs::Path() + kRes + "ficha3.what")) {
    throw std::runtime_error("Cannot load ficha");
  }

  m_music["game"].setLoop(true);
  m_music["main"].setLoop(true);
  m_music["main"].play();
  m_movie.fit(0, 0, getSize().x, getSize().y);

  m_boxes["scene"] = "menu";
  m_boxes["port"] = "2000";
  m_boxes["ip"] = "";

  m_push.Config(players, size, m_music, m_boxes);
  DrawObjects();
}

GameWindow::~GameWindow() {}

const std::shared_ptr<Command>& GameWindow::GetCommand() {
  while (true) {
    waitEvent(m_event);
    auto* btn = m_push.GetPressed(m_boxes["scene"], m_event);
    if (btn != nullptr) {
      return btn->GetCommand();
    }
  }
}

Push& GameWindow::GetButtons() { return m_push; }

Event& GameWindow::GetEvent() { return m_event; }

Music& GameWindow::GetMusic(const string& elem) { return m_music[elem]; }

map<string, string>& GameWindow::GetBoxes() { return m_boxes; }

void GameWindow::SetButtons(const string& str) {
  m_boxes["scene"] = str;
  DrawObjects();
}

void GameWindow::SetObject(const string& scene, const string& elem, size_t index,
                           const string& str) {
  dynamic_cast<Text*>(m_push.Get(scene, elem)->GetShapes()[index].sprite.get())->setString(str);
  DrawObjects();
}

void GameWindow::SetShow(const string& scene, const string& elem, bool show, int index) {
  if (index == -1) {
    for (auto& item : m_push.Get(scene, elem)->GetShapes()) {
      item.show = show;
    }
  } else {
    m_push.Get(scene, elem)->GetShapes()[index].show = show;
  }
  DrawObjects();
}

void GameWindow::DrawObjects() {
  clear();
  for (const auto& item : m_push.Data(m_boxes["scene"])) {
    for (const auto& object : item.second->GetShapes()) {
      if (object.show) {
        draw(*object.sprite);
      }
    }
  }
  display();
}

void GameWindow::SetVolume(CMDVolume type) {
  for (auto& elem : m_music) {
    switch (type) {
      case CMDVolume::Silence:
        elem.second.setVolume(0);
        elem.second.pause();
        break;

      case CMDVolume::Less:
        elem.second.setVolume(std::max(0.F, elem.second.getVolume() - 10));
        if (elem.second.getVolume() == 0) {
          elem.second.pause();
        }
        break;

      case CMDVolume::More:
        elem.second.setVolume(std::min(100.F, elem.second.getVolume() + 10));
        if (elem.second.getStatus() == sf::SoundSource::Paused) {
          elem.second.play();
        }
        break;

      case CMDVolume::Max:
        elem.second.setVolume(100);
        if (elem.second.getStatus() == sf::SoundSource::Paused) {
          elem.second.play();
        }
        break;
    }
  }
  SetObject("settings", "volume", 0, "Volume: " + bs::atos(m_music["main"].getVolume()));
}

void GameWindow::Ficha() {
  m_music["main"].stop();
  m_music["game"].stop();
  m_movie.play();
  while (true) {
    m_movie.update();
    clear();
    draw(m_movie);
    display();
  }
}
