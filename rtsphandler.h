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

#ifndef RTSPHANDLER_H
#define RTSPHANDLER_H

#include <QtNetwork>
#include "smarterdog.h"
#include "rtspconnection.h"
class SmarterDog;
class RtspConnection;

class RtspHandler : public QObject
{
	Q_OBJECT
public:
	explicit RtspHandler(uint16_t port, SmarterDog *controller, QObject *parent = 0);
//	void setRtpSourcePort(uint16_t port);

signals:
	void log(QString message);
	void startStream(QHostAddress destinationHost, uint16_t destionationPort);
	void stopStream();
public slots:

private slots:
	void incomingRtspConnection();
private:
	SmarterDog *controller;
	QTcpServer *rtspServer;
	QList<RtspConnection *> connections;
};

#endif // RTSPHANDLER_H
