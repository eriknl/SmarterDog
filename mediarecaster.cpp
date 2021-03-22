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

#include "mediarecaster.h"
#include <arpa/inet.h>
#include <QDateTime>

MediaRecaster::MediaRecaster(QObject *parent) : QObject(parent)
{
	rtpSocket = new QUdpSocket(parent);
	rtpSocket->bind(0, QUdpSocket::ShareAddress);
	connect(rtpSocket, SIGNAL(readyRead()), this, SLOT(mediaDatagramPending()));
	connect(&rtcpTimer, SIGNAL(timeout()), this, SLOT(rtcpTimerTimeout()));
	rtcpTimer.setSingleShot(true);
}

uint16_t MediaRecaster::getRtpPort()
{
	return rtpSocket->localPort();
}

void MediaRecaster::setRtpDestination(QHostAddress host, uint16_t port)
{
	rtpRemoteHost = host;
	rtpRemotePort = port;
}

void MediaRecaster::mediaDatagramPending()
{
	while (rtpSocket->hasPendingDatagrams()) {
		QByteArray datagramIn;
		datagramIn.resize(rtpSocket->pendingDatagramSize());
		rtpSocket->readDatagram(datagramIn.data(), datagramIn.size(), &this->rtcpRemoteHost, &this->rtcpRemotePort);
		rtpSocket->writeDatagram(datagramIn.data() + 4, datagramIn.size() - 4, this->rtpRemoteHost, this->rtpRemotePort);
		if (!rtcpTimer.isActive()) {
			rtcpTimer.start(1000);
		}
	}
}

void MediaRecaster::rtcpTimerTimeout(MediaRecaster::QPrivateSignal)
{
	QByteArray rtcpPacket;
	uint8_t padding[] = {0x00, 0x00, 0x01, 0x00};
	rtcpPacket.append((const char*)padding, 4);
	rtcpPacket.append((char)0x80);	// RTP v2
	rtcpPacket.append((char)0xc8);	// RTCP sender report packet type
	uint16_t length = 0x0006;	// (6 + 1) * 4 = 28 bytes
	rtcpPacket.append((const char*)&length, 2);
	uint32_t ssrc = htonl(0x00000002);
	rtcpPacket.append((const char*)&ssrc, 4);
	// https://github.com/MugenSAS/osc-cpp-qt/blob/master/tools/NtpTimestamp.cpp
	uint64_t ntpMSecs = QDateTime::currentMSecsSinceEpoch() + 2208988800000ll;
	uint32_t seconds = htonl(ntpMSecs / 1000);
	uint32_t fraction = htonl((0x100000000 * (ntpMSecs % 1000)) / 1000);
	rtcpPacket.append((const char*)&seconds, 4);
	rtcpPacket.append((const char*)&fraction, 4);
	quint32 rtpTime = 0;
	rtcpPacket.append((const char*)&rtpTime, 4);
	uint32_t packetCount = 0;
	rtcpPacket.append((const char*)&packetCount, 4);
	uint32_t octetCount = 0;
	rtcpPacket.append((const char*)&octetCount, 4);
	rtpSocket->writeDatagram(rtcpPacket.data(), rtcpPacket.size(), this->rtcpRemoteHost, this->rtcpRemotePort);
}
