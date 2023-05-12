#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <deque>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <regex>
#include <sfeMovie/Movie.hpp>
#include <unordered_map>

using sf::Color;
using sf::Event;
using sf::Font;
using sf::IpAddress;
using sf::Keyboard;
using sf::Mouse;
using sf::Music;
using sf::Packet;
using sf::RectangleShape;
using sf::RenderWindow;
using sf::Socket;
using sf::SoundBuffer;
using sf::SoundSource;
using sf::Sprite;
using sf::TcpListener;
using sf::TcpSocket;
using sf::Text;
using sf::Texture;
using sf::Thread;
using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::VideoMode;
using sf::View;
using sfe::Movie;
using std::array;
using std::deque;
using std::make_shared;
using std::map;
using std::pair;
using std::shared_ptr;
using std::string;
using std::unordered_map;

class Button;
class MouseButton;
class KeyboardButton;

class Cell;
class Ship;
class Player;

class Command;
class IPBoxCommand;
class PortBoxCommand;
class IPServerCommand;
class IPClientCommand;
class PortCommand;
class VolumeCommand;
class DisconnectCommand;
class SetSceneCommand;
class CellCommand;
class AddCellCommand;
class ShootCommand;
class AddShipCommand;

class Field;
class MyField;
class RivalField;

class Push;
class Network;
class GameWindow;
class GameLoop;

struct DrawObject;
struct TextObject;
struct RectObject;

enum class CMDVolume { Silence, Less, More, Max };
enum class CMDType { Close, Ficha };
enum class ShotState { Kill, Harm, Miss, Unknown };
enum class CellState { Alive, Harmed, Missed, Clear, Unknown, Prohibited, Killed, Chosen };

static const string kName = "BATTLESH!P!!";
static const string kRes = "/share/battlesh1p/";
static const size_t kMoveSleep = 700;

namespace bs {
string atos(long double num);
string Path();
}  // namespace bs
