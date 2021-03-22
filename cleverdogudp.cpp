/*
This file is part of SmarterDog (C) 2017 Erik de Jong

SmarterDog is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SmarterDog is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SmarterDog. If not, see <https://www.gnu.org/licenses/>
*/

#include "cleverdogudp.h"
#include <arpa/inet.h>
#include <QTcpSocket>

CleverdogUDP::CleverdogUDP(QObject *parent) : QObject(parent)
{
	signallingSocket = new QUdpSocket(parent);
	signallingSocket->bind(10008, QUdpSocket::ShareAddress);
	connect(signallingSocket, SIGNAL(readyRead()), this, SLOT(signallingDatagramPending()));
}

void CleverdogUDP::onboardingScanForDevices()
{
	sendCommand(QHostAddress("255.255.255.255"), cmdOnboardingScan);
}

void CleverdogUDP::onboardingScanWiFi(QHostAddress host, QString cid)
{
	sendCommand(host, cmdOnboardingScanWifi, cid);
}

void CleverdogUDP::onboardingSetWiFi(QHostAddress host, QString cid, QString ssid, QString psk)
{
	QByteArray parameters;
	parameters.append(QByteArray::fromHex("00000000"));
	addStringWithPadding(ssid, &parameters, 32);
	parameters.append(psk);
	parameters.append((char)0x00);
	sendCommand(host, cmdOnboardingSetWifi, cid, parameters);
}

void CleverdogUDP::setServer(QHostAddress host, QString cid, QString hostname, uint16_t port1, uint16_t port2)
{
	QByteArray parameters;
	addStringWithPadding(hostname, &parameters, 32);
	addStringWithPadding(QString::number(port1), &parameters, 8);
	addStringWithPadding(QString::number(port2), &parameters, 8);
	sendCommand(host, cmdOnboardingSetServer, cid, parameters);
}

void CleverdogUDP::scanForDevices()
{
	sendCommand(QHostAddress("255.255.255.255"), cmdScan);
}

void CleverdogUDP::scanForDevice(QString cid)
{
	sendCommand(QHostAddress("255.255.255.255"), cmdScan, cid);
}

void CleverdogUDP::setCid(QHostAddress host, QString cid)
{
	sendCommand(host, cmdSetCid, cid);
}

void CleverdogUDP::upgradeFirmware(QHostAddress host, QString cid)
{
	QByteArray parameters;
	parameters.append(QByteArray::fromHex("C0A80002000050"));
	sendCommand(host, cmdUpgradeFirmware, cid, parameters);
}

void CleverdogUDP::startRtp(QHostAddress host, QString cid, uint16_t port)
{
	QByteArray parameters;
	parameters.append(QByteArray::fromHex("000000000000000000000000000000000000"));
	parameters.append(QString::number(port));
	parameters.append(':');
	parameters.append(QString::number(port));
	parameters.append(QByteArray::fromHex("00"));
	sendCommand(host, cmdStartRtp, cid, parameters);
}

void CleverdogUDP::stopRtp(QHostAddress host, QString cid)
{
	sendCommand(host, cmdStopRtp, cid);
}

void CleverdogUDP::setCid(QString cid)
{
	myCid = cid;
}

void CleverdogUDP::setMacAddress(QString mac)
{
	myMac = mac;
}

void CleverdogUDP::signallingDatagramPending()
{
	QHostAddress host;
	uint16_t port;
	QByteArray datagram;
	QString mac;
	uint16_t command;
	int32_t errorCode;
	QString ssid;
	uint16_t index;
	uint16_t total;
	uint16_t encryption;
	QByteArray parameters;
	QTcpSocket *tcpSocket;
	while (signallingSocket->hasPendingDatagrams()) {
		parameters.clear();
		datagram.resize(signallingSocket->pendingDatagramSize());
		signallingSocket->readDatagram(datagram.data(), datagram.size(), &host, &port);
		/* Check for the magic number */
		if (datagram.left(2) != QByteArray::fromRawData((const char*)&magicNumber, 2)) {
			continue;
		}
		datagram.remove(0, 2);
		command = ntohs(*(uint16_t*)(datagram.data()));
		datagram.remove(0, 2);
		QString cid = QString(datagram.left(16));
		datagram.remove(0, 16);
		switch(command) {
			case cmdOnboardingScan:
				log("cmdOnboardingScan");
				sendCommand(host, cmdOnboardingScanReply, myCid, parameters, port);
			break;
			case cmdOnboardingScanReply:
				/* No further parameters */
				emit log("Found a device\nIP: " + QHostAddress(host.toIPv4Address()).toString() +
					 "\nCID: " + cid);
			break;
			case cmdOnboardingScanWifiSsid:
				// 2 bytes (?) index
				index = ntohs(*(uint16_t*)(datagram.data())) + 1;
				datagram.remove(0, 2);
				// 2 bytes (?) total
				total = ntohs(*(uint16_t*)(datagram.data()));
				datagram.remove(0, 2);
				// 2 bytes (?) ?
				datagram.remove(0, 2);
				// 2 bytes (?) encryption (0x00 = no, 0x03 = wpa personal)
				encryption = ntohs(*(uint16_t*)(datagram.data()));
				datagram.remove(0, 2);
				// 32 bytes SSID
				ssid = datagram.left(32);
				emit log("SSID [" + QString::number(index) + "/" + QString::number(total) + "]: " + ssid);
			break;
			case cmdOnboardingSetWifi:
				sendCommand(host, cmdOnboardingSetWifiAck, myCid, parameters, port);
			break;
			case cmdOnboardingSetWifiAck:
				emit log("SSID set ok");
			break;
			case cmdOnboardingSetServer:
				emit log("Set backend address");
				tcpSocket = new QTcpSocket();
				tcpSocket->connectToHost("47.91.75.6", 443);
				tcpSocket->waitForConnected();
				datagram.clear();
				datagram.append(QByteArray::fromHex("966aa0a005a8")); // ?
				datagram.append("2.4.6.34"); // version
				datagram.append((char)0xac);
				datagram.append(myCid);
				tcpSocket->write(datagram);
				tcpSocket->flush();
				
				datagram.clear();
				datagram.append(QByteArray::fromHex("9b64a0a0ac"));
				datagram.append(myCid);
				datagram.append(QByteArray::fromHex("01a4"));
				datagram.append("abcd"); // SSID
				datagram.append(QByteArray::fromHex("05a8"));
				datagram.append("2.4.6.34"); // version
				datagram.append((char)0xa9);
				datagram.append("DOG-1W-V4"); // hw version
				datagram.append(QByteArray::fromHex("ce59aa85fab1"));
				datagram.append(myMac);
				tcpSocket->write(datagram);
				tcpSocket->close();
				
				tcpSocket->connectToHost("47.91.75.6", 2000);
				tcpSocket->waitForConnected();
			break;
			case cmdError:
				/* Parameters are MAC address, original command id and return code */
				mac = datagram.left(datagram.indexOf((char)0x00));
				datagram.remove(0, datagram.indexOf((char)0x00) + 1);
				command = ntohs(*(uint16_t*)(datagram.data()));
				datagram.remove(0, 2);
				errorCode = ntohl(*(int32_t*)(datagram.data()));
				emit log("Error from device\nIP: " + QHostAddress(host.toIPv4Address()).toString() +
					 "\nCID: " + cid +
					 "\nMAC: " + mac +
					 "\nCommandcode: 0x" + QString::number(command, 16) +
					 "\nReturncode: " + QString::number(errorCode));
			break;
			case cmdScan:
				addStringWithPadding(myMac, &parameters, 18);
				addStringWithPadding("2.4.6.34", &parameters, 16);
				sendCommand(host, cmdScanReply, myCid, parameters, port);
			break;
			case cmdScanReply:
				/* Parameters are MAC address and software version */
				mac = datagram.left(datagram.indexOf((char)0x00));
				datagram.remove(0, datagram.indexOf((char)0x00) + 1);
				QString version = datagram.left(datagram.indexOf((char)0x00));
				emit scanResult(QHostAddress(host.toIPv4Address()), cid, mac, version);
			break;
		}
	}
}

void CleverdogUDP::sendCommand(QHostAddress destinationHost, uint16_t command, QString cid, QByteArray parameters, uint16_t port)
{
	command = htons(command);
	QByteArray datagram;
	/* Add magic number */
	datagram.append(QByteArray::fromRawData((const char*)&magicNumber, 2));
	/* Command */
	datagram.append(QByteArray::fromRawData((const char*)&command, 2));
	/* CID */
	addStringWithPadding(cid, &datagram, 16);
	/* Parameters */
	datagram.append(parameters);
	/* Send datagram */
	signallingSocket->writeDatagram(datagram, destinationHost, port);
}

void CleverdogUDP::addStringWithPadding(QString value, QByteArray *payload, int length)
{
	if (value.length() > length - 1) {
		value = value.left(length- 1);
	}
	payload->append(value);
	for (int i = length - value.length(); i > 0; i--) {
		payload->append((char)0x00);
	}
}
