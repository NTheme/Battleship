#pragma once

#include "common.hpp"

class Command {
  friend class GameLoop;

public:
  Command();
  explicit Command(MyEventType type);
  virtual ~Command() = default;

  virtual void Execute(bool is_remote = false) = 0;
  [[nodiscard]] MyEventType GetType() const noexcept;

protected:
  static GameLoop* m_loop;
  MyEventType m_type;
};

class IPBoxCommand final : public Command {
public:
  IPBoxCommand() = default;
  ~IPBoxCommand() override = default;

  void Execute(bool is_remote = false) override;
};

class PortBoxCommand final : public Command {
public:
  PortBoxCommand() = default;
  ~PortBoxCommand() override = default;

  void Execute(bool is_remote = false) override;
};

class IPClientCommand final : public Command {
public:
  IPClientCommand() = default;
  ~IPClientCommand() override = default;

  void Execute(bool is_remote = false) override;

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
  ~IPServerCommand() override = default;

  void Execute(bool is_remote = false) override;
};

class PortCommand final : public Command {
public:
  PortCommand() = default;
  ~PortCommand() override = default;

  void Execute(bool is_remote = false) override;

protected:
  static std::regex m_port_regex;
};

class DisconnectCommand final : public Command {
public:
  DisconnectCommand() = default;
  ~DisconnectCommand() override = default;

  void Execute(bool is_remote = false) override;
};

class VolumeCommand final : public Command {
public:
  explicit VolumeCommand(CMDVolume type);
  ~VolumeCommand() override = default;

  void Execute(bool is_remote = false) override;

private:
  CMDVolume m_type;
};

class SetSceneCommand final : public Command {
public:
  explicit SetSceneCommand(const string& str);
  ~SetSceneCommand() override = default;

  void Execute(bool is_remote = false) override;

private:
  static deque<string> m_stack;
  string m_str;
};

class CellCommand : public Command {
public:
  CellCommand(Player* player, Cell* cell);
  ~CellCommand() override = default;
  void Execute(bool is_remote = false) override = 0;

protected:
  Player* m_player;
  Cell* m_cell;

  [[nodiscard]] virtual bool IsValid() const = 0;
  virtual void Send() = 0;
};

class AddCellCommand final : public CellCommand {
  friend class Player;

public:
  AddCellCommand(Player* player, Cell* cell);
  ~AddCellCommand() override = default;
  void Execute(bool is_remote = false) override;

protected:
  [[nodiscard]] bool IsValid() const override;
  void Send() override;
};

class ShootCommand final : public CellCommand {
public:
  ShootCommand(Player* player, Cell* cell);
  ~ShootCommand() override = default;
  void Execute(bool is_remote = false) override;

protected:
  [[nodiscard]] bool IsValid() const override;
  void Send() override;
};

class AddShipCommand final : public Command {
public:
  explicit AddShipCommand(Player* player);
  ~AddShipCommand() override = default;
  void Execute(bool is_remote = false) override;

protected:
  Player* m_player;

  [[nodiscard]] bool IsValid() const;
  static void Send();
};