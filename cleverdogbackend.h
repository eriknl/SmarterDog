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

#ifndef CLEVERDOGBACKEND_H
#define CLEVERDOGBACKEND_H
#include <QtNetwork>
#include "cleverdogbackendconnection.h"

class CleverdogBackendConnection;

class CleverdogBackend : public QObject
{
	Q_OBJECT
public:
	explicit CleverdogBackend(QObject *parent = 0);
	
signals:
	
public slots:
private slots:
	void incomingConnection();
private:
	QTcpServer *backendServer;
	QList<CleverdogBackendConnection *> connections;
};

#endif // CLEVERDOGBACKEND_H
