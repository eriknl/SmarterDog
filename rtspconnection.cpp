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

#include <QRegularExpression>

#include "rtspconnection.h"

RtspConnection::RtspConnection(QTcpSocket *rtspClientSocket, SmarterDog *controller) : rtspClientSocket(rtspClientSocket), controller(controller)
{
	rtspState = none;
	connect(rtspClientSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void RtspConnection::setRtpSourcePort(uint16_t port)
{
	rtpSourcePort = port;
}

void RtspConnection::readyRead()
{
	QByteArray incoming = rtspClientSocket->readAll();
	emit log("Read: " + incoming);
	emit log("===========================================================");
	incoming = incoming.replace('\n', "");
//	QByteArray incoming = rtspClientSocket->readAll();
    QString requestLine = incoming.left(incoming.indexOf('\r'));
	QString request = requestLine.left(requestLine.indexOf(' '));
	QString body = "";
	QString reply = "RTSP/1.0 200 OK\r\n";
	headerLine(&reply, "CSeq", getHeaderItem("cseq", incoming));
	if (request == "OPTIONS") {
		headerLine(&reply, "Public", "DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE");
	} else if (request == "DESCRIBE") {
		rtspState = describe;
		headerLine(&reply, "Content-Type", "application/sdp");
		headerLine(&reply, "Transport", "RTP/AVP/UDP;unicast");
		
		body.append("v=0\r\n");
		body.append("s=test stream\r\n");
		body.append("o=- 0 0 IN IP4 192.168.0.6\r\n");
		body.append("t=0\r\n");
//		body.append("a=recvonly\r\n");
		body.append("m=video 0 RTP/AVP 96\r\n");
		body.append("a=rtpmap:96 H264/90000\r\n");
	} else if (request == "SETUP") {
		rtspState = setup;
		QString url = requestLine.split(' ').at(1);
		emit log("URL " + url + " requested");
		QString transport = getHeaderItem("transport", incoming).replace("\r","\r\n");
		emit log("Transport: " + transport);
		emit log("\n*******\n");
		rtpDestinationHost = rtspClientSocket->peerAddress();
		QRegularExpression client_port = QRegularExpression("[\\s\\S]+client_port=(\\d+)-\\d+");
		QRegularExpressionMatch match = client_port.match(transport);
		if (match.hasMatch()) {
			rtpDestinationPort = match.captured(1).toInt();
			emit log ("Client port: " + match.captured(1));
		}
		QRegularExpression cid = QRegularExpression("[\\s\\S]+[cC][iI][dD]=(\\d+)");
		match = cid.match(url);
		if (match.hasMatch()) {
			cameraCid = match.captured(1);
			emit log ("CID: " + match.captured(1));
		}
		rtpSourcePort = controller->getRtpSourcePort("123");
//		rtpSourcePort = controller->getRtpSourcePort(cameraCid);
		if (transport.length() > 0) {
			transport = transport.append(";");
		}
		transport = transport.append("server_port=" + QString::number(rtpSourcePort) + "-" + QString::number(rtpSourcePort + 1) + ";ssrc=00000000");
		headerLine(&reply, "Transport", transport);
		session = "01030507";
		headerLine(&reply, "Session", session);
	} else if (request == "PLAY") {
		rtspState = play;
		emit startStream(cameraCid, rtpDestinationHost, rtpDestinationPort);
	} else if (request == "PAUSE") {
		rtspState = pause;
	} else if (request == "TEARDOWN") {
		rtspState = none;
		emit stopStream(cameraCid, rtpDestinationHost, rtpDestinationPort);
	} else {
//		emit log("Unknown message: " + incoming);
		reply = "RTSP/1.0 405 Method Not Allowed\r\n";
//		return;
	}
	headerLine(&reply, "Content-Length", QString::number(body.length()));
	reply.append("\r\n");
	reply.append(body);
	emit log("===========================================================");
	emit log("Write: " + reply);
	emit log("===========================================================");
	rtspClientSocket->write(QByteArray(reply.toLatin1()));
}

QString RtspConnection::getHeaderItem(QString headerItem, QString header)
{
	QRegularExpression regex = QRegularExpression(headerItem + ": ([\\s\\S]*?)\\r",  QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionMatch match = regex.match(header);
	if (match.hasMatch()) {
		return match.captured(1);
	}
	return "";
}

void RtspConnection::headerLine(QString *header, QString key, QString value)
{
	header->append(QString("%1: %2\r\n").arg(key, value));
}
