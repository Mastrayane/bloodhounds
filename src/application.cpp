#include "application.h"

#include <cassert>
#include <chrono>
#include <iostream>

#include "logger.h"
#include "ticker.h"
#include "model_serialization.h"

Application::Application(boost::asio::io_context &ioc, const c_parse::Args &args)
    : m_ioc(ioc),
      strand(boost::asio::make_strand(ioc)),
      m_max_player_id{0},
      m_max_dog_id{0},
      m_dir_to_content(args.www_root),
      m_state_file(args.state_file) {
  // required pathes
    bool both_files_exist = std::filesystem::exists(args.config_file) && std::filesystem::exists(args.www_root);

    if (!both_files_exist) {
        throw std::logic_error("Wrong path, config="s + args.config_file + ", content="s + args.www_root);
    }

  // create game form json config
  m_game = json_loader::LoadGame(args.config_file);

  // random spawn
  m_game.SetRandomSpawn(args.random_spawn);

  // auto update
  if (args.tick_period) {
    std::make_shared<util::Ticker>(this->strand, std::chrono::milliseconds(args.tick_period), std::bind(&Application::Update, this, std::placeholders::_1))->Start();
    m_manual_ticker = false;
  } else {
    m_manual_ticker = true;
  }

  // load state
  if (std::filesystem::exists(args.state_file)) {
    LoadState(args.state_file);
  }

  // auto save
  if (!args.state_file.empty() && args.save_state_period) {
    std::make_shared<util::Ticker>(this->strand, std::chrono::milliseconds(args.save_state_period), std::bind(&Application::SaveState, this))->Start();
  }

  // DB
  dbconn::ConnectionFactory conn_fact(args.db_url);
  //  m_conn_pool = std::make_shared<dbconn::ConnectionPool>(std::thread::hardware_concurrency(), conn_fact);
  m_conn_pool = std::make_shared<dbconn::ConnectionPool>(10, std::move(conn_fact));

  m_player_list.SetRecorder([this](model::PlayerInfo player_info) {
    auto conn = m_conn_pool->GetConnection();
    pqxx::work w(*conn);
    try {
      auto res = w.exec_prepared(dbconn::ConnectionFactory::insert_data,
                                 player_info.name,
                                 player_info.score,
                                 player_info.play_time);
      assert(res.affected_rows() != 0);
      w.commit();
    } catch (const std::exception &e) {
      w.abort();
      std::cerr << e.what() << std::endl;
      throw;
    }
  });
}

Application::~Application() {
  // todo
  // close resource
}

void Application::SetManualTicker(bool enable) {
  m_manual_ticker = enable;
}

bool Application::IsManualTicker() const {
  return m_manual_ticker;
}

void Application::SetRandomSpawn(bool enable) {
  m_game.SetRandomSpawn(enable);
}

const std::filesystem::path &Application::GetContentDir() const noexcept {
  return m_dir_to_content;
}

std::optional<std::reference_wrapper<const model::Map>> Application::FindMap(const model::Map::Id &id) const noexcept {
  return m_game.FindMap(id);
}

const model::Game::Maps &Application::GetMaps() const noexcept {
  return m_game.GetMaps();
}

model::Player &Application::JoinGame(const model::Map::Id &id, const std::string &user_name) {
  //  get if exist or get created session
  auto sess = m_game.GetSession(id);
  if (!sess) {
    throw std::runtime_error("Fail to open or create ssesion");
  }

  auto &player = m_player_list.CreatePlayer(user_name, m_max_player_id++);

  auto dog = std::make_shared<model::Dog>(model::Dog::Id(m_max_dog_id++));

  player.SetDog(dog);
  player.SetSession(sess);

  return player;
}

std::optional<std::reference_wrapper<model::Player>> Application::FindPlayer(const model::Token &t) {
  return m_player_list.FindPlayer(t);
}

const model::PlayerList::Container &Application::GetPlayers() const noexcept {
  return m_player_list.GetContainer();
}

void Application::Update(std::chrono::milliseconds ms) {
  // if (!m_manual_ticker && (ms.count() > timeout)) {  // fail if timeout
  //   throw std::runtime_error("Time for aplication update state is very long");
  // }

  m_game.Update(ms);
  m_player_list.Update();
}

void Application::SaveState() {
  if (m_state_file.empty()) {
    return;
  }

  if (std::filesystem::exists(m_state_file)) {
    // todo rename
    std::filesystem::path bak_file = m_state_file;
    bak_file += ".bak";
    std::filesystem::rename(m_state_file, bak_file);
  }

  std::ofstream file(m_state_file);
  if (!file.is_open()) {
    throw std::logic_error("Fail to open file - "s + m_state_file.string());
  }

  OutputArchive output_archive{file};

  output_archive << m_max_player_id;
  output_archive << m_max_dog_id;

  model::GameSessionSer ser(m_game.GetSessionList(), m_player_list.GetContainer());

  output_archive << ser;
  file.close();
}

void Application::LoadState(const std::filesystem::path &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::logic_error("Fail to open file - "s + path.string());
  }

  InputArchive input_archive{file};

  input_archive >> m_max_player_id;
  input_archive >> m_max_dog_id;

  model::GameSessionSer ser;
  input_archive >> ser;

  ser.UpdateGame(m_game, m_player_list);
}

std::vector<model::PlayerInfo> Application::GetPlayerInfoList(size_t start, size_t max_items) {
    if (max_items > dbconn::ConnectionFactory::MaxItemReq) {
        throw std::invalid_argument("max_items exceeds the maximum allowed value");
    }

  std::vector<model::PlayerInfo> players;
  players.reserve(dbconn::ConnectionFactory::MaxItemReq);

  auto conn = m_conn_pool->GetConnection();
  pqxx::read_transaction r(*conn);
  auto result = r.exec_prepared(dbconn::ConnectionFactory::get_data, start, max_items);

  for (const auto &row : result) {
    std::string name;
    int score;
    double time;
    row.at(std::string(dbconn::ColName::name)).to(name);
    row.at(std::string(dbconn::ColName::score)).to(score);
    row.at(std::string(dbconn::ColName::play_time)).to(time);

    players.emplace_back(name, score, time);
  }

  return players;  // rvo
}
