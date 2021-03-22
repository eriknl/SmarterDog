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

#ifndef CLEVERDOGUDP_H
#define CLEVERDOGUDP_H

#include <QUdpSocket>

class CleverdogUDP : public QObject
{
	Q_OBJECT
public:
	explicit CleverdogUDP(QObject *parent = 0);
	void onboardingScanForDevices();
	void onboardingScanWiFi(QHostAddress host, QString cid);
	void onboardingSetWiFi(QHostAddress host, QString cid, QString ssid, QString psk);
	
	void setServer(QHostAddress host, QString cid, QString hostname, uint16_t port1, uint16_t port2);
	
	void scanForDevices();
	void scanForDevice(QString cid);
	void setCid(QHostAddress host, QString cid);
	void upgradeFirmware(QHostAddress host, QString cid);
	void startRtp(QHostAddress host, QString cid, uint16_t port);
	void stopRtp(QHostAddress host, QString cid);
	
	
	void setCid(QString cid);
	void setMacAddress(QString mac);
signals:
	void log(QString message);
	void scanResult(QHostAddress host, QString CID, QString mac, QString firmware);
private slots:
	void signallingDatagramPending();
private:
	typedef enum {
		cmdOnboardingScan =		0x0000,
		cmdOnboardingScanReply =	0x0001,
		cmdOnboardingScanWifi =		0x0002,
		cmdOnboardingScanWifiSsid =	0x0003,
		cmdOnboardingSetWifi =		0x0004,
		cmdOnboardingSetWifiAck =	0x0005,
		cmdOnboardingSetServer =	0x000d,
		cmdOnboardingSetTimezone =	0x000e,
		cmdError =			0x1003,
		cmdScan =			0x1004,
		cmdSetCid =			0x1005,
		cmdUpgradeFirmware =		0x1006,
		cmdStartRtp =			0x1007,
		cmdStopRtp =			0x1008,
		cmdScanReply =			0x100e
	} cleverdogUdpCommands;

	void sendCommand(QHostAddress destinationHost, uint16_t command, QString cid = "", QByteArray parameters = QByteArray(), uint16_t port = 10008);
	void addStringWithPadding(QString value, QByteArray *payload, int length);
	
	QUdpSocket *signallingSocket;
	const unsigned char magicNumber[2] = {0x4d, 0x4a};
	QString myCid;
	QString myMac;
};

#endif // CLEVERDOGUDP_H
