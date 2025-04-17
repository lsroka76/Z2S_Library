# Zigbee <=> SUPLA gateway
This project is my personal attempt to build a gateway between various Zigbee end devices and SUPLA IoT ecosystem.

## Table of Contents:

1. [SUPLA - overview](#supla)
2. [Zigbee - overview](#zigbee)
3. [Zigbee <=> SUPLA gateway - overview](#zigbee--supla-gateway-1)
4. [Prerequisites](#prerequisites)
5. [Installation](#installation)

## SUPLA
[SUPLA](https://github.com/SUPLA)  is an open-source Internet of Things (IoT) platform that empowers users to create their own smart home or automation systems. It offers a flexible and cost-effective way to control various devices remotely. The system is based on open software and hardware principles, encouraging community development and customization. Users can build their own devices using popular platforms like Raspberry Pi, Arduino, ESP8266 and ESP32. These devices can then be connected to the Supla cloud or a self-hosted server via Wi-Fi or LAN.
One of the significant advantages of Supla is its open architecture. This fosters a large and active community of developers and enthusiasts who contribute to the platform's growth. The availability of source code on platforms like GitHub encourages innovation and allows users to tailor the system to their specific needs. Users also have the option to set up their own independent server for enhanced privacy and control.  
[(ToC)](#table-of-contents)
  
## Zigbee
Zigbee is a low-power, low-cost wireless communication protocol designed for short-range data transfer in personal area networks. It operates on the IEEE 802.15.4 standard and is well-suited for applications requiring long battery life and secure networking. A key feature of Zigbee is its mesh network topology, where devices can communicate with each other, extending the network range and improving reliability through multiple pathways. This makes it ideal for smart home applications like lighting control, security systems, and smart thermostats.
  
Zigbee supports a large number of devices on a single network, often up to 65,000 nodes, making it highly scalable for extensive deployments. It offers reliable and secure communication with features like AES-128 encryption. Compared to other wireless technologies like Wi-Fi and Bluetooth, Zigbee typically consumes less power, which is crucial for battery-operated sensors and devices. Its cost-effectiveness and ease of installation have contributed to its widespread adoption in various IoT applications, including industrial automation and healthcare monitoring. The Zigbee Alliance (now the Connectivity Standards Alliance) continues to develop and promote the standard, ensuring interoperability between different manufacturers' devices.  
[(ToC)](#table-of-contents)
  
## Zigbee <=> SUPLA gateway
Zigbee's strength lies in the vast number and diverse range of available sensors, catering to numerous smart applications. This extensive ecosystem includes low-cost options for temperature, humidity, motion, contact, light, and many other environmental and security parameters. The variety extends to specialized sensors like those for air quality, water leak detection, and even presence detection. Due to its widespread adoption and open standard, numerous manufacturers produce Zigbee sensors, driving down costs and increasing accessibility. This affordability and wide selection empower users to build comprehensive and customized smart environments without significant expense. The low power nature of Zigbee also makes these battery-operated sensors practical for long-term, widespread deployment.  
  
I've taken an effort to design and build a kind of bidirectional bridge directly between Zigbee devices and SUPLA, thus eliminating the need for some proxy solutions like [Zigbee2MQTT](https://www.zigbee2mqtt.io).

That gateway, as discussed on the [Supla forum](https://forum.supla.org/viewforum.php?f=127), enables the integration of devices using the Zigbee wireless communication protocol with the Supla smart home system. This allows users to control and monitor a wider range of smart devices, which often include sensors, lights, and switches that utilize the low-power and mesh networking capabilities of Zigbee. The gateway essentially acts as a bridge, translating the Zigbee communication into a format that the Supla cloud and applications can understand and interact with.  
[(ToC)](#table-of-contents)

## Prerequisites
- ESP32 C6 module 4MB - without OTA, >= 8 MB - with OTA,
- SUPLA Cloud account,
- some Zigbee sensors,
- WiFi network with 2.4 GHz coverage, 
- lot of patience ;-).
  
[(ToC)](#table-of-contents)

## Installation
1. You can download source code from this GitHub account and compile it by yourself, or
   
2. you can download binary files from latest release and upload to your device with your favorite ESP flashing software:
   
   - Files labeled "merged" should be uploaded at address 0x0000 since they are complete ESP image - they will overwrite any data, nevertheless it's recommended to erase flash before upload,
     
   - files without label "merged" should be uploaded at address 0x10000 or through OTA (**gateway_IP/upload**) - OTA works only for modules with 8 MB of flash or more.  
[(ToC)](#table-of-contents)

