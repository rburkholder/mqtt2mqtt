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
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
* Project: MQTT2MQTT
 * Created: 2025/12/21 12:31:27
  */

#include <vector>
#include <fstream>
#include <exception>
//#include <type_traits>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {

  static const std::string sValue_Mqtt_In_Id(    "mqtt_in_id" );
  static const std::string sValue_Mqtt_In_Host(  "mqtt_in_host" );
  static const std::string sValue_Mqtt_In_Topic( "mqtt_in_topic" );

  static const std::string sValue_Mqtt_Out_Id(       "mqtt_out_id" );
  static const std::string sValue_Mqtt_Out_Host(     "mqtt_out_host" );
  static const std::string sValue_Mqtt_Out_Topic(    "mqtt_out_topic" ); // prepend
  static const std::string sValue_Mqtt_Out_UserName( "mqtt_out_username" );
  static const std::string sValue_Mqtt_Out_Password( "mqtt_out_password" );

  //template<typename T>
  //void log( const std::string& name, typename std::enable_if<std::is_pod<T&>::value>::type& dest ) {
  //  BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  //}

  void log( const std::string& name, int dest ) {
    BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  }

  void log( const std::string& name, const std::string& dest ) {
    BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
  }

  //void log( const std::string& name, const vName_t& dest ) {
  //  for ( const vName_t::value_type& value: dest ) {
  //    BOOST_LOG_TRIVIAL(info) << name << " = " << value;
  //  }
  //}

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = vm[name].as<T>();
      //BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
      log( name, dest );
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
      bOk = false;
    }
    return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Values& values ) {

  bool bOk( true );

  using vAnalogInIx_t = std::vector<uint16_t>;
  vAnalogInIx_t vAnalogInIx;

  try {

    po::options_description config( "MQTT2MQTT Config" );
    config.add_options()

      ( sValue_Mqtt_In_Id.c_str(), po::value<std::string>( &values.mqtt_in.sId ), "mqtt in client id" )
      ( sValue_Mqtt_In_Host.c_str(), po::value<std::string>( &values.mqtt_in.sHost )->default_value( "localhost" ), "mqtt in host address or name" )
      ( sValue_Mqtt_In_Topic.c_str(), po::value<std::string>( &values.mqtt_in.sTopic )->default_value( "#" ), "mqtt in topic" )

      ( sValue_Mqtt_Out_Id.c_str(), po::value<std::string>( &values.mqtt_out.sId ), "mqtt out client id" )
      ( sValue_Mqtt_Out_Host.c_str(), po::value<std::string>( &values.mqtt_out.sHost )->default_value( "localhost" ), "mqtt out host address or name" )
      ( sValue_Mqtt_Out_UserName.c_str(), po::value<std::string>( &values.mqtt_out.sUserName ), "mqtt out username" )
      ( sValue_Mqtt_Out_Password.c_str(), po::value<std::string>( &values.mqtt_out.sPassword ), "mqtt out password" )
      ( sValue_Mqtt_Out_Topic.c_str(), po::value<std::string>( &values.mqtt_out.sTopic )->default_value( "" ), "mqtt out topic" )

      ;
    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_In_Id, values.mqtt_in.sId );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_In_Host, values.mqtt_in.sHost );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_In_Topic, values.mqtt_in.sTopic );

      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Out_Id, values.mqtt_out.sId );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Out_Host, values.mqtt_out.sHost );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Out_UserName, values.mqtt_out.sUserName );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Out_Password, values.mqtt_out.sPassword );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Out_Topic, values.mqtt_out.sTopic );

    }
  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;
}

} // namespace config
