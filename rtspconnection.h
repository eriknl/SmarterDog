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

#ifndef RTSPCONNECTION_H
#define RTSPCONNECTION_H

#include <QTcpSocket>
#include <QHostAddress>
#include "rtsphandler.h"
class SmarterDog;

class RtspConnection : public QObject
{
	Q_OBJECT
public:
	typedef enum {
		none,
		describe,
		setup,
		play,
		pause
	} playState;
	RtspConnection(QTcpSocket *rtspClientSocket, SmarterDog *controller);
	void setRtpSourcePort(uint16_t port);
	
	QTcpSocket *rtspClientSocket;
	playState rtspState;
signals:
	void startStream(QString CID, QHostAddress destinationHost, uint16_t destionationPort);
	void stopStream(QString CID, QHostAddress destinationHost, uint16_t destionationPort);
	void log(QString message);
public slots:
	void readyRead();
private:
	static QString getHeaderItem(QString headerItem, QString header);
	static void headerLine(QString *header, QString key, QString value);
	
	SmarterDog *controller;
	QString session;
	uint16_t rtpDestinationPort;
	QHostAddress rtpDestinationHost;
	uint16_t rtpSourcePort;
	QString cameraCid;
};

#endif // RTSPCONNECTION_H
