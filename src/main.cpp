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
 * File:    main.cpp
 * Author:  raymond@burkholder.net
 * Project: MQTT2MQTT
 * Created: 2025/12/21 12:36:42
 */

#include <iostream>

#include <string>

#include <boost/asio/io_context.hpp>

#include "Reflector.hpp"
#include "Config.hpp"

namespace asio = boost::asio;

int main( int argc, char **argv ) {

  static const std::string c_sConfigFilename( "mqtt2mqtt.cfg" );

  std::cout << "MQTT2MQTT (c)2025 One Unified Net Limited" << std::endl;

  config::Values choices;

  if ( Load( c_sConfigFilename, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  asio::io_context io_context;

  try {
    Reflector reflector( choices, io_context );
    io_context.run();
  }
  catch(...) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

