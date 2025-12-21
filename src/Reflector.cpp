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
 * File:    Loop.cpp
 * Author:  raymond@burkholder.net
 * Project: AD2MQTT
 * Created: 2025/12/11 21:10:03
 */

#include <unistd.h>
#include <limits.h>

#include <iostream>

#include <boost/log/trivial.hpp>

#include <boost/asio/post.hpp>

#include <ou/mqtt/mqtt.hpp>

#include "Reflector.hpp"
#include "Config.hpp"

// TODO:
//   accept 'set' for writes
//   accept 'get' for refresh, specific key for single value
//   track changes and emit delta

Reflector::Reflector( const config::Values& choices, asio::io_context& io_context )
: m_choices( choices ), m_io_context( io_context )
, m_signals( io_context, SIGINT ) // SIGINT is called '^C'
{
/*
  int rc;
  char szHostName[ HOST_NAME_MAX + 1 ];
  rc = gethostname( szHostName, HOST_NAME_MAX + 1 );
  if ( 0 != rc ) {
    m_sMqttId = szHostName;
  }
  else {
    assert( 0 < choices.mqtt.sId.size() );
    m_sMqttId = choices.mqtt.sId;
  }
*/
  m_pWorkGuard = std::make_unique<work_guard_t>( asio::make_work_guard( io_context ) );

  // https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/signal_set.html

  //signals.add( SIGKILL ); // not allowed here
  m_signals.add( SIGHUP ); // use this as a config change?
  //signals.add( SIGINFO ); // control T - doesn't exist on linux
  m_signals.add( SIGTERM );
  m_signals.add( SIGQUIT );
  m_signals.add( SIGABRT );

  m_signals.async_wait( [this]( const boost::system::error_code& error_code, int signal_number ){
    Signals( error_code, signal_number );
  } );

  static const std::string SystemTopic( "N/+/system/+/Serial" );

  try {
    m_pMqttOut = std::make_unique<ou::Mqtt>( choices.mqtt_out, choices.mqtt_out.sId );
    m_pMqttIn = std::make_unique<ou::Mqtt>( choices.mqtt_in, choices.mqtt_in.sId );
    m_pMqttIn->Subscribe(
      SystemTopic,
      [this]( const std::string_view& sTopic, const std::string_view& sMessage ){

        boost::asio::post(
          m_io_context,
          [this](){
            m_pMqttIn->UnSubscribe( SystemTopic );
          } );

        const std::string message( sMessage );
        const auto colon( sMessage.find_first_of( ':') );
        const auto endQuote( sMessage.find_last_of( '"' ) );
        assert( colon < endQuote );
        const std::string system( sMessage.substr( colon + 2, endQuote - 1 - colon - 1 ) );
        assert( 12 == system.length() );
        const std::string keepalive( "R/" + system + "/keepalive" );

        boost::asio::post(
          m_io_context,
          [this,keepalive](){
            m_pMqttIn->Publish(
              keepalive, "",
              []( bool b, int i ){
                std::cout << "result1 " << b << ',' << i << std::endl;
              } );
          } );

        boost::asio::post(
          m_io_context,
          [this](){
            m_pMqttIn->Subscribe(
              m_choices.mqtt_in.sTopic,
              [this]( const std::string_view& sTopic, const std::string_view& sMessage ){
                const std::string topic( m_choices.mqtt_out.sTopic + '/' + std::string( sTopic ) );
                const std::string message( sMessage );
                boost::asio::post(
                  m_io_context,
                  [this, topic, message](){
                    m_pMqttOut->Publish(
                      topic, message,
                      []( bool b, int i ){
                        //std::cout << "result2 " << b << ',' << i << std::endl;
                      } );
                  }
                );
              } );
          } );

      } );
  }
  catch ( const ou::Mqtt::runtime_error& e ) {
    BOOST_LOG_TRIVIAL(error) << "mqtt error: " << e.what() << '(' << e.rc << ')';
    throw e;
  }

  std::cout << "ctrl-c to end" << std::endl;

}

void Reflector::Signals( const boost::system::error_code& error_code, int signal_number ) {

  BOOST_LOG_TRIVIAL(info)
    << "signal"
    << "(" << error_code.category().name()
    << "," << error_code.value()
    << "," << signal_number
    << "): "
    << error_code.message()
    ;

  bool bContinue( true );

  switch ( signal_number ) {
    case SIGHUP:
      BOOST_LOG_TRIVIAL(info) << "sig hup no-op";
      break;
    case SIGTERM:
      BOOST_LOG_TRIVIAL(info) << "sig term";
      bContinue = false;
      break;
    case SIGQUIT:
      BOOST_LOG_TRIVIAL(info) << "sig quit";
      bContinue = false;
      break;
    case SIGABRT:
      BOOST_LOG_TRIVIAL(info) << "sig abort";
      bContinue = false;
      break;
    case SIGINT:
      BOOST_LOG_TRIVIAL(info) << "sig int";
      bContinue = false;
      break;
    case SIGPIPE:
      BOOST_LOG_TRIVIAL(info) << "sig pipe";
      bContinue = false;
      break;
    default:
      break;
  }

  if ( bContinue ) {
    m_signals.async_wait( [this]( const boost::system::error_code& error_code, int signal_number ){
    Signals( error_code, signal_number );
  } );
  }
  else {
    m_pWorkGuard.reset();
    bContinue = false;
  }
}

Reflector::~Reflector() {
  m_pWorkGuard.reset();
  m_pMqttIn.reset();
  m_pMqttOut.reset();
}

