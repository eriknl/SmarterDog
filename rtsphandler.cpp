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

#include "rtsphandler.h"

RtspHandler::RtspHandler(uint16_t port, SmarterDog *controller, QObject *parent) : QObject(parent), controller(controller)
{
	rtspServer = new QTcpServer(parent);
	rtspServer->listen(QHostAddress::Any, port);
	connect(rtspServer, SIGNAL(newConnection()), this, SLOT(incomingRtspConnection()));
}

//void RtspHandler::setRtpSourcePort(uint16_t port)
//{
//	rtpSourcePort = port;
//}

void RtspHandler::incomingRtspConnection()
{
	RtspConnection *connection = new RtspConnection(rtspServer->nextPendingConnection(), controller);
	connections.append(connection);
	connect(connection, SIGNAL(startStream(QString,QHostAddress,uint16_t)), controller, SLOT(startStream(QString,QHostAddress,uint16_t)));
	connect(connection, SIGNAL(stopStream(QString,QHostAddress,uint16_t)), controller, SLOT(stopStream(QString,QHostAddress,uint16_t)));
	connect(connection, SIGNAL(log(QString)), controller, SLOT(log(QString)));
}
