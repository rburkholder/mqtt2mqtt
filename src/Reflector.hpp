/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    Reflector.hpp
 * Author:  raymond@burkholder.net
 * Project: MQTT2MQTT
 * Created: 2025/12/21 12:37:00
 */

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace config {
  class Values;
}

namespace ou {
  class Mqtt;
}

namespace asio = boost::asio; // from <boost/asio/context.hpp>

class Reflector {
public:
  Reflector( const config::Values&, asio::io_context& );
  ~Reflector();
protected:
private:

  const config::Values& m_choices;
  asio::io_context& m_io_context;

  std::unique_ptr<ou::Mqtt> m_pMqttIn;
  std::unique_ptr<ou::Mqtt> m_pMqttOut;

  using work_guard_t = asio::executor_work_guard<asio::io_context::executor_type>;
  using pWorkGuard_t = std::unique_ptr<work_guard_t>;

  pWorkGuard_t m_pWorkGuard;

  asio::signal_set m_signals;

  std::string m_sSystemSerialNumber;

  boost::asio::steady_timer m_timerKeepAlive;

  std::string m_sKeepAlive_Topic;

  using fKeepAlive_t = std::function<void( const boost::system::error_code& )>;
  fKeepAlive_t m_fKeepAlive;

  //void KeepAlive( const boost::system::error_code& );
  void Signals( const boost::system::error_code&, int );

};

