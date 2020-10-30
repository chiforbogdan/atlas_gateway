[![License](https://img.shields.io/badge/license-GPL%20v3.0%20or%20later-brightgreen.svg)](https://github.com/chiforbogdan/atlas_gateway/blob/master/LICENSE)

# ATLAS IoT Security Platform - general information
ATLAS consists in a 3-tier IoT security platform which offers the following modules:
* A lightweight software client which runs on the IoT device ([ATLAS_Client])
* A gateway software which runs on the network edge and manages all the clients from the network ([ATLAS_Gateway])
* A cloud platform which allows managing the gateways and the clients ([ATLAS_Cloud])
An Android management application which allows IoT command authorization ([ATLAS_Android])

ATLAS provides security management for a fleet of IoT devices and enables a reputation based Sensing-as-a-service platform. It also offers the capability to inspect the IoT device telemetry values and supports the CoAP lightweight protocol for the communication between the IoT device and the gateway.
On the IoT data plane layer, ATLAS provides an API which can be integrated with a user application and offers the following capabilities:
* Install a firewall rule on the gateway side
* Send packet statistics to the gateway and cloud
* Get the device with the most trusted reputation within a category and provide reputation feedback

## ATLAS Gateway
ATLAS Gateway is a software which runs on the network edge and manages all the IoT clients from the network. It enables the collection of telemetry data from connected IoT nodes (that have ATLAS Client software) and offeres a method of filtering malicious nodes through an integrated reputation based scoring architecture. ATLAS Gateway aggregates data from all the registered IoT nodes and synchronizes the information in real-time with the ATLAS Cloud module. ATLAS Gateway uses the CoAPs protocol to communicate with the downstream IoT nodes and it uses MQTTs protocol to communicate with the upstream cloud component.

----

### How to build it
Generally, there are two steps are involved:
* Step 1. Install required dependencies by executing the __dependencies.sh__ script from the __scripts__ folder
* Step 2. Execute the __build.sh__ script from __root__ folder

We used ([Ubuntu_MATE_20.04.1LTS]) on an ODROID-XU4 platform to run this application.
Depending on the platform you are using, minor adjustments might be necessary to be made. See the output messages shown during execution of the above mentioned scripts in case of errors.

----

### How to use it
```
./atlas_gateway ----cloudHostname <ATLAS_CLOUD_HOSTNAME_OR_IP> --cloudPort <ATLAS_CLOUD_PORT> --cloudCertFile <ATLAS_CLOUD_CERT_TRUSTSTORE_FILE> --coapPort <ATLAS_GATEWAY_COAP_LISTEN_PORT> --httpCertFile <ATLAS_GATEWAY_INTERNAL_HTTP_SERVER_CERT_FILE> --httpPrivKeyFile <ATLAS_GATEWAY_INTERNAL_HTTP_SERVER_PRIV_KEY_FILE> --httpPort <ATLAS_GATEWAY_INTERNAL_HTTP_SERVER_PORT>```

Arguments:
* __--help__ &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; _Display help message_
* __--cloudHostname__ &nbsp;&nbsp; _IP address or hostname of the ATLAS Cloud broker - used to connect ATLAS Gateway with the ATLAS Cloud back-end_
* __--cloudPort__ &nbsp;&nbsp;&nbsp;&nbsp; _Port of ATLAS Cloud broker - used to connect ATLAS Gateway with the ATLAS Cloud back-end_
* __--cloudCertFile__ &nbsp; _Truststore certificate file for ATLAS Cloud broker (this is the **client.truststore.pem** file generated by the ATLAS Cloud component, please see the README file from [ATLAS_Cloud] for more details)_
* __--coapPort__ &nbsp;&nbsp;&nbsp; _Listen port number for the ATLAS Gateway CoAP server - used to connect ATLAS Gateway with the ATLAS Client instances_
* __--httpCertFile__ &nbsp; _Server certificate file for the gateway internal HTTPs server (generated by the build script)_
* __--httpPrivKeyFile__ &nbsp; _Private key file for the gateway internal HTTPs server (generated by the build script)_
* __--httpPort__ &nbsp; _Listen port for the gateway internal HTTPs server_

Example: 
```
./atlas_gateway --cloudHostname 127.0.0.1 --cloudPort 8883 --cloudCertFile client.truststore.pem --coapPort 10100 --httpCertFile 127.0.0.1.crt --httpPrivKeyFile 127.0.0.1.key --httpPort 3333
```

----

### Supplementary modules
Besides the main software module mentioned above, the project will build another 3 supplementary software components: _libatlas_gateway_mosquitto_plugin.so_, _atlas_gateway_cli_ and _atlas_gateway_reputation_test_.

#### libatlas_gateway_mosquitto_plugin.so
This is the MQTT application layer firewall module of ATLAS Gateway: it receives firewall rules from ATLAS Gateway and inspects every data plane MQTT packet. Also, this module reports firewall rule statistics (ingress/egress accepted and rejected packets) to ATLAS Gateway. This is a plug-in for a modified version of Eclipse Mosquitto MQTT broker ([ATLAS_Mosquitto]).
It is used by starting the aforementioned version of Mosquitto Broker through the following command:
````
mosquitto -c mosquitto-atlas.conf
````
where _mosquitto-atlas.conf_ is a configuration file present in the _build_ folder (added after running the _build.sh_ script). Please make sure the `auth_plugin` variable from the _mosquitto-atlas.conf_ indicates the absolute path of *libatlas_gateway_mosquitto_plugin.so*.

#### atlas_gateway_cli
This modules enables the enrollment of ATLAS Client instances (using the identity - *[atlas_client.identity file]* and PSK - *[atlas_client.psk file]* information generated by the ATLAS Client software module) at the ATLAS Gateway layer.
It can be used as follows:
````
./atlas_gateway_cli --identity <ATLAS_CLIENT_IDENTITY> --psk <ATLAS_CLIENT_PSK>
````
where _ATLAS_CLIENT_IDENTITY_ and _ATLAS_CLIENT_PSK_ are the contents of _.identity_ and _.psk_ files generated by each ATLAS Client instance.
Example:
````
./atlas_gateway_cli --identity "4fa32a5a-8dff-427f-8d6d-4b7010bea7e0" --psk "ZKUTQu5244b/wvuuW23N8oaDE6Eci20o7TnYbXFcd44="
````

#### atlas_gateway_reputation_test
This module provides means for independently testing the reputation-based filtering mechanism implemented in ATLAS Gateway. It offers several test scenarios, each with different parameters that can be customized. For more information on how to use this tool, see the README file from the *src/reputation_impl/tester/* directory.

----

### Authors
ATLAS Gateway was developed by:
* Bogdan-Cosmin Chifor
* Stefan-Ciprian Arseni
* Ioana Cismas
* Mihai Coca
* Mirabela Medvei

ATLAS project is sponsored by [UEFISCDI].

----

### License
GNU General Public License v3.0 or later.

See LICENSE file to read the full text.

[ATLAS_Client]: https://github.com/chiforbogdan/atlas_client
[ATLAS_Gateway]: https://github.com/chiforbogdan/atlas_gateway
[ATLAS_Cloud]: https://github.com/chiforbogdan/atlas_cloud
[ATLAS_Mosquitto]: https://github.com/chiforbogdan/mosquitto/tree/atlas_plugin
[ATLAS_Android]: https://github.com/chiforbogdan/atlas_android
[UEFISCDI]: https://uefiscdi.gov.ro/
[Ubuntu_MATE_20.04.1LTS]: https://wiki.odroid.com/odroid-xu4/os_images/linux/ubuntu_5.4/mate/20200818
