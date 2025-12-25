intro:
* currently a generic reflector
* will probably evolve to be victron centric due to particular messages
* will probably deprecate once apparition can handle multiple mqtt brokers

to build:

    # pre-requisites:
    #  boost from git@github.com:rburkholder/libs-build.git
    #  git@github.com:rburkholder/repertory.git

    git clone git@github.com:rburkholder/mqtt2mqtt.git
    cd mqtt2mqtt
    mkdir build
    cd build
    cmake ..
    cd ..

    # compose a mqtt2mqtt.cfg as below
    # then run:

    build/src/mqtt2mqtt


mqtt2mqtt.cfg file:

    mqtt_in_id = <unique id>
    mqtt_in_host = <hostname/ip address>
    #mqtt_in_topic = # # is comment, cannot be escaped, so use coded default wildcard

    mqtt_out_id = <unique id>
    mqtt_out_host = <hostname/ip address>
    mqtt_out_username = <username>
    mqtt_out_password = <password>
    mqtt_out_topic = <topic prefix>  # no trailing slash
    
Todo:
* implement the victron keepalive and settings messages
* the application can then run without victron's portal also operating
* keepalive needs to be posted with 'suppress-republish' once a minute
  * probably each 50 or 55 minutes to preven the 60 second expiry
