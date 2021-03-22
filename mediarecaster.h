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

#ifndef MEDIARECASTER_H
#define MEDIARECASTER_H

#include <QUdpSocket>
#include <QTimer>

class MediaRecaster : public QObject
{
	Q_OBJECT
public:
	explicit MediaRecaster(QObject *parent = 0);
	uint16_t getRtpPort();
	void setRtpDestination(QHostAddress host, uint16_t port);
signals:
	void log(QString message);
private slots:
	void mediaDatagramPending();
	void rtcpTimerTimeout(QPrivateSignal);
private:
	QUdpSocket *rtpSocket;
	QTimer rtcpTimer;
	QHostAddress rtpRemoteHost;
	uint16_t rtpRemotePort;
	QHostAddress rtcpRemoteHost;
	uint16_t rtcpRemotePort;
};

#endif // MEDIARECASTER_H
