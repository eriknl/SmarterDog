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

#ifndef SMARTERDOG_H
#define SMARTERDOG_H

#include <QObject>
#include <QTextStream>
#include <QMap>

#include "cleverdogbackend.h"
#include "cleverdogcamera.h"
#include "cleverdogudp.h"
#include "mediarecaster.h"
#include "rtsphandler.h"
class RtspHandler;

class SmarterDog : public QObject
{
	Q_OBJECT
public:
	explicit SmarterDog(QObject *parent = 0);
	void scan();
	void stream(QStringList cidList);
	uint16_t getRtpSourcePort(QString CID);
	
signals:
	void finished();

public slots:
	void log(QString message);
	void scanResult(QHostAddress host, QString CID, QString mac, QString firmware);
	void startStream(QString CID, QHostAddress destinationHost, uint16_t destionationPort);
	void stopStream(QString CID, QHostAddress destinationHost, uint16_t destionationPort);
private slots:
	void terminate();
	void scanTimeout();
private:
	int scanTime;
	QMap<QString, CleverdogCamera*> cameras;
	CleverdogBackend *cleverdogBackend;
	CleverdogUDP *cleverdogHandler;
	QTextStream *qout;
	RtspHandler *rtspServer;
};

#endif // SMARTERDOG_H
