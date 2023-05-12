#pragma once

#include "common.hpp"

class Command {
  friend class GameLoop;

 public:
  Command();
  Command(const Event::EventType& type);
  virtual ~Command() = default;

  virtual void Execute(bool is_remote = false) = 0;
  const Event::EventType& GetType();

 protected:
  static GameLoop* m_loop;
  Event::EventType m_type;
};

class IPBoxCommand final : public Command {
 public:
  IPBoxCommand() = default;
  ~IPBoxCommand() final = default;

  void Execute(bool is_remote = false);
};

class PortBoxCommand final : public Command {
 public:
  PortBoxCommand() = default;
  ~PortBoxCommand() final = default;

  void Execute(bool is_remote = false);
};

class IPClientCommand final : public Command {
 public:
  IPClientCommand() = default;
  ~IPClientCommand() final = default;

  void Execute(bool is_remote = false);

  static std::string m_ip_port;

 private:
  static std::string m_ip_addr;
  static std::string m_ip_full;
  static std::regex m_ip_regex;

  static pair<string, size_t> ParseIp();
};

class IPServerCommand final : public Command {
 public:
  IPServerCommand() = default;
  ~IPServerCommand() final = default;

  void Execute(bool is_remote = false);
};

class PortCommand final : public Command {
 public:
  PortCommand() = default;
  ~PortCommand() final = default;

  void Execute(bool is_remote = false);

 protected:
  static std::regex m_port_regex;
};

class DisconnectCommand final : public Command {
 public:
  DisconnectCommand() = default;
  ~DisconnectCommand() final = default;

  void Execute(bool is_remote = false);
};

class VolumeCommand final : public Command {
 public:
  VolumeCommand(CMDVolume type);
  ~VolumeCommand() final = default;

  void Execute(bool is_remote = false);

 private:
  CMDVolume m_type;
};

class SetSceneCommand final : public Command {
 public:
  SetSceneCommand(const string& str);
  ~SetSceneCommand() final = default;

  void Execute(bool is_remote = false) final;

 private:
  static deque<string> m_stack;
  string m_str;
};

class CellCommand : public Command {
 public:
  CellCommand(Player* player, Cell* cell);
  ~CellCommand() override = default;
  virtual void Execute(bool is_remote = false) override = 0;

 protected:
  Player* m_player;
  Cell* m_cell;

  virtual bool IsValid() const = 0;
  virtual void Send() = 0;
};

class AddCellCommand final : public CellCommand {
  friend class Player;

 public:
  AddCellCommand(Player* player, Cell* cell);
  ~AddCellCommand() final = default;
  void Execute(bool is_remote = false) final;

 protected:
  bool IsValid() const final;
  void Send() final;
};

class ShootCommand final : public CellCommand {
 public:
  ShootCommand(Player* player, Cell* cell);
  ~ShootCommand() final = default;
  void Execute(bool is_remote = false) final;

 protected:
  bool IsValid() const final;
  void Send() final;
};

class AddShipCommand : public Command {
 public:
  AddShipCommand(Player* player);
  ~AddShipCommand() = default;
  void Execute(bool is_remote = false) final;

 protected:
  Player* m_player;

  bool IsValid() const;
  static void Send();
};
