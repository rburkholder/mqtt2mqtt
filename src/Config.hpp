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
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: MQTT2MQTT
 * Created: 2025/12/21 12:31:27
 */

#pragma once

//#include <vector>

#include <ou/mqtt/config.hpp>

namespace config {

struct Values {

  //using vMqtt_t = std::vector<ou::mqtt::Config>;
  //vMqtt_t vMqtt;

  ou::mqtt::Config mqtt_in;
  ou::mqtt::Config mqtt_out;

};

bool Load( const std::string& sFileName, Values& );

} // namespace config
