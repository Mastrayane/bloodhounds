#ifndef __TICKER_H__
#define __TICKER_H__

#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <chrono>

namespace util {

class Ticker : public std::enable_shared_from_this<Ticker> {
 public:
  using Strand = boost::asio::strand<boost::asio::io_context::executor_type>;
  using Handler = std::function<void(std::chrono::milliseconds delta)>;

  Ticker(Strand strand, std::chrono::milliseconds period, Handler handler) : m_strand(strand), m_timer(strand), m_period(period), m_handler{handler} {}

  void Start() {
    m_last_tick = std::chrono::steady_clock::now();
    /* Выполнить SchedulTick внутри strand_ */
    // m_timer.expires_at(m_timer.expiry() + m_period);
    m_timer.expires_from_now(m_period);
    m_timer.async_wait(boost::asio::bind_executor(m_strand, [self = shared_from_this()](boost::system::error_code ec) {
      self->OnTick(ec);
    }));
  }

 private:
  void ScheduleTick() {
    /* выполнить OnTick через промежуток времени period_ */
   // m_timer.expires_at(m_timer.expiry() + m_period);
    m_timer.expires_from_now(m_period);
    m_timer.async_wait(boost::asio::bind_executor(m_strand, [self = shared_from_this()](boost::system::error_code ec) {
      self->OnTick(ec);
    }));
  }
  void OnTick(boost::system::error_code ec) {
    auto current_tick = std::chrono::steady_clock::now();
    m_handler(std::chrono::duration_cast<std::chrono::milliseconds>(current_tick - m_last_tick));
    m_last_tick = current_tick;
    ScheduleTick();
  }

  Strand m_strand;
  boost::asio::steady_timer m_timer;
  std::chrono::milliseconds m_period;
  Handler m_handler;
  std::chrono::steady_clock::time_point m_last_tick;
};

}  // namespace util

#endif  // __TICKER_H__