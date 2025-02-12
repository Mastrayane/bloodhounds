#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>

using namespace std::string_literals;

#include "json_loader.h"
#include "model.h"
#include "ticker.h"
#include "command_parse.h"
#include "model_serialization.h"
#include "conn_pool.h"
#include "conn_fact.h"

class Application {
  Application() = delete;
  Application(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(const Application&) = delete;
  Application& operator=(Application&&) = delete;

  static constexpr unsigned timeout = 30000u;

 public:
  using InputArchive = boost::archive::text_iarchive;
  using OutputArchive = boost::archive::text_oarchive;

  Application(boost::asio::io_context& ioc, const c_parse::Args& args);
  ~Application();

  void SetManualTicker(bool enable = true);
  bool IsManualTicker() const;

  void SetRandomSpawn(bool enable = true);

  const std::filesystem::path& GetContentDir() const noexcept;

  std::optional<std::reference_wrapper<const model::Map>> FindMap(const model::Map::Id& id) const noexcept;

  const std::vector<model::Map>& GetMaps() const noexcept;

  model::Player& JoinGame(const model::Map::Id& id, const std::string& user_name);

  std::optional<std::reference_wrapper<model::Player>> FindPlayer(const model::Token& t);

  const model::PlayerList::Container& GetPlayers() const noexcept;

  void Update(std::chrono::milliseconds ms);

  void SaveState();

  void LoadState(const std::filesystem::path& path);

  // return player info from database
  std::vector<model::PlayerInfo> GetPlayerInfoList(size_t start, size_t max_items);

  boost::asio::strand<boost::asio::io_context::executor_type> strand;

 private:
  boost::asio::io_context& m_ioc;
  std::filesystem::path m_dir_to_content;
  model::Game m_game;
  model::PlayerList m_player_list;
  bool m_manual_ticker;
  std::ofstream m_ofstream;

  size_t m_max_player_id;
  size_t m_max_dog_id;

  std::filesystem::path m_state_file;

  std::shared_ptr<dbconn::ConnectionPool> m_conn_pool;

};

#endif  // __APPLICATION_H__