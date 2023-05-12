#include "../lib/push.hpp"

#include "../lib/button.hpp"
#include "../lib/cell.hpp"
#include "../lib/command.hpp"
#include "../lib/object.hpp"
#include "../lib/player.hpp"

Push::Push() {
  if (!m_font.loadFromFile(bs::Path() + kRes + "symbola.ttf")) {
    throw std::runtime_error("Cannot load m_font");
  }
  if (!m_bg.loadFromFile(bs::Path() + kRes + "background.jpg")) {
    throw std::runtime_error("Cannot load background");
  }
}

const map<string, shared_ptr<Button>>& Push::Data(const string& scene) const {
  return m_buttons.at(scene);
}

Button* Push::Get(const string& scene, const string& obj) { return m_buttons[scene][obj].get(); }

Button* Push::GetPressed(const string& scene, const Event& event) {
  for (auto& button : m_buttons[scene]) {
    if (button.second->IsPressed(event)) {
      return button.second.get();
    }
  }
  return nullptr;
}

void Push::Config(array<Player, 2>& players, const Vector2u& size, const map<string, Music>& music,
                  const map<string, string>& boxes) {
  ConfigMainMenu();
  ConfigSettings(music, boxes);
  ConfigPlayMenu();
  ConfigClient();
  ConfigServer();
  ConfigField(players, size);
  ConfigPlay(players);

  Set<Button>("any", "close", make_shared<SetSceneCommand>("adios"));

  Set<MouseButton>("any", "return", Mouse::Left, make_shared<SetSceneCommand>("back"),
                   RectObject({100, 100}, {0, 255, 95}, {70, 65}),
                   TextObject("<-", 60, Color::Red, {85, 70}, m_font));

  Set<Button>("any", "background", nullptr, DrawObject(make_shared<Sprite>(m_bg)),
              TextObject(kName, 140, Color::Red, {475, 0}, m_font, Text::Bold | Text::Underlined));
}

void Push::ConfigMainMenu() {
  Set<MouseButton>("menu", "play", Mouse::Left, make_shared<SetSceneCommand>("play"),
                   RectObject({340, 150}, {0, 255, 95}, {790, 300}),
                   TextObject("Play", 140, Color::Red, {820, 270}, m_font));

  Set<MouseButton>("menu", "settings", Mouse::Left, make_shared<SetSceneCommand>("settings"),
                   RectObject({530, 150}, {0, 255, 95}, {700, 820}),
                   TextObject("Settings", 140, Color::Red, {720, 790}, m_font));

  Set<MouseButton>(
      "menu", "ficha", Mouse::Left, make_shared<SetSceneCommand>("ficha"),
      RectObject({370, 50}, {0, 255, 95}, {1550, 1030}, false),
      TextObject("By NThemeDEV & ortbor", 30, Color::Green, {1550, 990}, m_font, Text::Italic),
      TextObject("Click to learn more...", 30, Color::Green, {1550, 1030}, m_font, Text::Bold));
}

void Push::ConfigSettings(const map<string, Music>& music, const map<string, string>& boxes) {
  ConfigVolume(music);
  ConfigPort(boxes);
}

void Push::ConfigVolume(const map<string, Music>& music) {
  Vector2u coord(370, 590);

  Set<Button>("settings", "volume", nullptr,
              TextObject("Volume: " + bs::atos(music.at("main").getVolume()), 100, Color::Red,
                         {coord.x, coord.y}, m_font, Text::Bold));

  Set<MouseButton>("settings", "vol_silence", Mouse::Left,
                   make_shared<VolumeCommand>(CMDVolume::Silence),
                   RectObject({100, 100}, {0, 255, 95}, {coord.x + 590, coord.y + 22}),
                   TextObject("<X", 60, Color::Red, {coord.x + 592, coord.y + 32}, m_font));

  Set<MouseButton>("settings", "vol_less", Mouse::Left, make_shared<VolumeCommand>(CMDVolume::Less),
                   RectObject({100, 100}, {0, 255, 95}, {coord.x + 740, coord.y + 22}),
                   TextObject("-", 60, Color::Red, {coord.x + 777, coord.y + 29}, m_font));

  Set<MouseButton>("settings", "vol_more", Mouse::Left, make_shared<VolumeCommand>(CMDVolume::More),
                   RectObject({100, 100}, {0, 255, 95}, {coord.x + 890, coord.y + 22}),
                   TextObject("+", 60, Color::Red, {coord.x + 918, coord.y + 27}, m_font));

  Set<MouseButton>("settings", "vol_max", Mouse::Left, make_shared<VolumeCommand>(CMDVolume::Max),
                   RectObject({100, 100}, {0, 255, 95}, {coord.x + 1040, coord.y + 22}),
                   TextObject("<))", 60, Color::Red, {coord.x + 1045, coord.y + 27}, m_font));
}

void Push::ConfigPort(const map<string, string>& boxes) {
  Vector2u coord(595, 300);

  Set<Button>("settings", "port", nullptr,
              TextObject("Change port: ", 100, Color::Red, {coord.x, coord.y}, m_font, Text::Bold));

  Set<KeyboardButton>(
      "settings", "port_box", make_shared<PortBoxCommand>(),
      RectObject({120, 70}, {200, 200, 200}, {coord.x + 608, coord.y + 35}),
      TextObject(boxes.at("port"), 40, Color::Black, {coord.x + 618, coord.y + 43}, m_font));

  Set<MouseButton>("settings", "port_save", Mouse::Left, make_shared<PortCommand>(),
                   RectObject({150, 70}, {0, 255, 95}, {coord.x + 297, coord.y - 65}),
                   TextObject("Save", 60, Color::Red, {coord.x + 310, coord.y - 73}, m_font));

  Set<Button>(
      "settings", "port_status", nullptr,
      TextObject("Success", 80, Color::Green, {coord.x + 243, 445}, m_font, Text::Bold, false),
      TextObject("Invalid Port", 80, Color::Red, {coord.x + 145, 445}, m_font, Text::Bold, false),
      TextObject("Failed to set", 80, Color::Red, {coord.x + 143, 445}, m_font, Text::Bold, false));
}

void Push::ConfigPlayMenu() {
  Set<MouseButton>("play", "client", Mouse::Left, make_shared<SetSceneCommand>("client"),
                   RectObject({665, 150}, {0, 255, 95}, {630, 300}),
                   TextObject("Throw a glove", 100, Color::Red, {640, 300}, m_font));

  Set<MouseButton>("play", "server", Mouse::Left, make_shared<IPServerCommand>(),
                   RectObject({690, 150}, {0, 255, 95}, {615, 820}),
                   TextObject("Wait for a shot", 100, Color::Red, {625, 823}, m_font));
}

void Push::ConfigClient() {
  Set<KeyboardButton>("client", "box", make_shared<IPBoxCommand>(),
                      RectObject({500, 70}, {200, 200, 200}, {708, 445}),
                      TextObject("", 40, Color::Black, {715, 450}, m_font));

  Set<MouseButton>("client", "save", Mouse::Left, make_shared<IPClientCommand>(),
                   RectObject({260, 150}, {0, 255, 95}, {827, 600}),
                   TextObject("Save", 100, Color::Red, {857, 600}, m_font));

  Set<Button>(
      "client", "status", nullptr,
      TextObject("Success", 80, Color::Green, {823, 800}, m_font, Text::Bold, false),
      TextObject("Invalid IP", 80, Color::Red, {775, 800}, m_font, Text::Bold, false),
      TextObject("Server is offline", 80, Color::Red, {660, 800}, m_font, Text::Bold, false),
      TextObject("Connection timeout!", 80, Color::Red, {590, 800}, m_font, Text::Bold, false),
      TextObject("Port is busy", 80, Color::Red, {735, 800}, m_font, Text::Bold, false),
      TextObject("Connecting...", 80, Color::Blue, {725, 800}, m_font, Text::Bold, false));
}

void Push::ConfigServer() {
  Set<Button>(
      "server", "status", nullptr,
      TextObject("Waiting for the connection...", 100, Color::Red, {330, 490}, m_font, Text::Bold),
      TextObject("Connected!", 100, Color::Green, {700, 550}, m_font, Text::Bold, false));
}

void Push::ConfigField(array<Player, 2>& players, const Vector2u& size) {
  for (size_t pl = 0; pl < 2; ++pl) {
    for (size_t i = 0; i < size.x; ++i) {
      for (size_t j = 0; j < size.y; ++j) {
        auto* cell_my = players[pl].GetMField()->GetCell(Vector2u(i, j));
        auto* cell_rv = players[pl].GetRField()->GetCell(Vector2u(i, j));
        auto play = "play_" + std::to_string(pl);
        auto select = "select_" + std::to_string(pl);
        auto cell = std::to_string(i * size.y + j);

        auto pos_my = Vector2u(140 + i * 70, 250 + j * 70);
        auto pos_rv = Vector2u(1080 + i * 70, 250 + j * 70);
        Set<MouseButton>(select, "cell_m_" + cell, Mouse::Left,
                         make_shared<AddCellCommand>(players.data() + pl, cell_my),
                         RectObject({65, 65}, {255, 120, 255}, pos_my));
        Set<MouseButton>(play, "cell_r_" + cell, Mouse::Left,
                         make_shared<ShootCommand>(players.data() + pl, cell_rv),
                         RectObject({65, 65}, {255, 255, 255}, pos_rv));

        Set<Button>(play, "cell_m_" + cell, nullptr, Get(select, "cell_m_" + cell)->GetShapes()[0]);

        auto rect_my = Get(select, "cell_m_" + cell)->GetShapes()[0];
        auto rect_rv = Get(play, "cell_r_" + cell)->GetShapes()[0];
        cell_my->SetShape(dynamic_cast<RectangleShape*>(rect_my.sprite.get()));
        cell_rv->SetShape(dynamic_cast<RectangleShape*>(rect_rv.sprite.get()));
      }
    }
  }
}

void Push::ConfigPlay(array<Player, 2>& players) {
  for (size_t pl = 0; pl < 2; ++pl) {
    unsigned int start = 255;
    unsigned int shift = 120;
    auto play = "play_" + std::to_string(pl);
    auto select = "select_" + std::to_string(pl);

    Set<MouseButton>(select, "settings", Mouse::Left, make_shared<SetSceneCommand>("settings"),
                     RectObject({150, 60}, {0, 255, 95}, {1730, 75}),
                     TextObject("Settings", 40, Color::Red, {1736, 75}, m_font));

    Set<MouseButton>(play, "settings", Mouse::Left, make_shared<SetSceneCommand>("settings"),
                     RectObject({150, 60}, {0, 255, 95}, {1730, 75}),
                     TextObject("Settings", 40, Color::Red, {1736, 75}, m_font));

    Set<Button>(play, "cell_clear", nullptr, RectObject({40, 40}, {255, 120, 255}, {940, start}),
                TextObject("Clear", 40, Color::Red, {910, start + 40}, m_font));

    Set<Button>(play, "cell_alive", nullptr,
                RectObject({40, 40}, {0, 255, 255}, {940, start + shift}),
                TextObject("Alive", 40, Color::Red, {910, start + shift + 40}, m_font));

    Set<Button>(play, "cell_harmed", nullptr,
                RectObject({40, 40}, {100, 100, 0}, {940, start + shift * 2}),
                TextObject("Harmed", 40, Color::Red, {887, start + shift * 2 + 40}, m_font));

    Set<Button>(play, "cell_killed", nullptr,
                RectObject({40, 40}, {0, 0, 0}, {940, start + shift * 3}),
                TextObject("Killed", 40, Color::Red, {908, start + shift * 3 + 40}, m_font));

    Set<Button>(play, "cell_missed", nullptr,
                RectObject({40, 40}, {0, 0, 255}, {940, start + shift * 4}),
                TextObject("Missed", 40, Color::Red, {900, start + shift * 4 + 40}, m_font));

    Set<Button>(play, "cell_unknown", nullptr,
                RectObject({40, 40}, {255, 255, 255}, {940, start + shift * 5}),
                TextObject("Unknown", 40, Color::Red, {873, start + shift * 5 + 40}, m_font));

    Set<MouseButton>(select, "ship", Mouse::Left, make_shared<AddShipCommand>(players.data() + pl),
                     RectObject({335, 110}, {0, 255, 95}, {1210, 530}),
                     TextObject("Add ship", 80, Color::Red, {1220, 530}, m_font),
                     TextObject("Select your ships", 80, Color::Blue, {1100, 300}, m_font));

    Set<Button>(
        select, "status", nullptr,
        TextObject("Success!", 80, Color::Green, {1240, 750}, m_font, Text::Bold, false),
        TextObject("Cannot select\n    this cell!", 80, Color::Red, {1120, 750}, m_font, Text::Bold,
                   false),
        TextObject("Wrong shaped ship!", 80, Color::Red, {1030, 750}, m_font, Text::Bold, false));

    Set<Button>(play, "turn", nullptr,
                TextObject("Your turn", 100, Color::Red, {730, 930}, m_font, sf::Text::Bold, false),
                TextObject("Wait", 100, Color::Red, {845, 930}, m_font, sf::Text::Bold, false),
                TextObject("Your field", 80, Color::Red, {135, 950}, m_font),
                TextObject("Rival field", 80, Color::Red, {1410, 950}, m_font));
  }

  Set<Button>("waiting", "text", nullptr,
              TextObject("Waiting for other player...", 100, Color::Red, {380, 550}, m_font));

  Set<Button>("won_0", "text", nullptr, TextObject("You win!", 120, Color::Red, {720, 350}, m_font),
              TextObject("        Do you feel proud of yourself after\n"
                         "you killed all innocent other player's ships?..",
                         60, Color::Red, {360, 750}, m_font, Text::Bold));

  Set<Button>("won_1", "text", nullptr,
              TextObject("Rival win(", 120, Color::Red, {690, 350}, m_font, Text::Bold),
              TextObject("Don't worry, be happy!", 60, Color::Red, {650, 750}, m_font, Text::Bold));

  Set<Button>("disconnected", "text", nullptr,
              TextObject("The connection is lost", 120, Color::Red, {400, 550}, m_font));
}
