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

#include "cleverdogbackend.h"

CleverdogBackend::CleverdogBackend(QObject *parent) : QObject(parent)
{
	backendServer = new QTcpServer(parent);
	backendServer->listen(QHostAddress::Any, 443);
	connect(backendServer, SIGNAL(newConnection()), this, SLOT(incomingConnection()));
}

void CleverdogBackend::incomingConnection()
{
	QTcpSocket *connection = backendServer->nextPendingConnection();
	CleverdogBackendConnection *backendConnection = new CleverdogBackendConnection(connection);
	connections.append(backendConnection);
}
