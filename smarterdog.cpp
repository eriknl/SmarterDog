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

#include "smarterdog.h"

SmarterDog::SmarterDog(QObject *parent) : QObject(parent)
{
	qout = new QTextStream(stdout);
	cleverdogHandler = new CleverdogUDP(parent);
	connect(cleverdogHandler, SIGNAL(scanResult(QHostAddress,QString,QString,QString)), this, SLOT(scanResult(QHostAddress,QString,QString,QString)));
	connect(cleverdogHandler, SIGNAL(log(QString)), this, SLOT(log(QString)));
	cleverdogBackend = new CleverdogBackend();
    scanTime = 1000; // end scans after 1 second
}

void SmarterDog::scan()
{
	cleverdogHandler->scanForDevices();
	QTimer::singleShot(scanTime, this, SLOT(terminate()));
	*qout << "Scan results:" << endl;
}

void SmarterDog::stream(QStringList cidList)
{
	rtspServer = new RtspHandler(8086, this);
	// Fetch current address for selected cameras
	foreach (QString cid, cidList) {
		CleverdogCamera *camera = new CleverdogCamera();
		cameras.insert(cid, camera);
	}
	cleverdogHandler->scanForDevices();
	QTimer::singleShot(scanTime, this, SLOT(scanTimeout()));
}

uint16_t SmarterDog::getRtpSourcePort(QString CID)
{
	if (cameras.contains(CID)) {
		return cameras.value(CID)->recaster->getRtpPort();
	}
	return 0;
}

void SmarterDog::log(QString message)
{
	*qout << message << endl;
}

void SmarterDog::scanResult(QHostAddress host, QString CID, QString mac, QString firmware)
{
	if (CID != "" && mac != "" && firmware != "") {
		CleverdogCamera *camera = new CleverdogCamera(host, CID, mac, firmware);
		if (cameras.contains(CID)) {
			delete cameras.value(CID);
		}
		cameras.insert(CID, camera);
		log(QString("%1 %2 %3 %4").arg(host.toString(), CID, mac, firmware));
	}
}

void SmarterDog::startStream(QString CID, QHostAddress destinationHost, uint16_t destionationPort)
{
	if (cameras.contains(CID)) {
		CleverdogCamera *camera = cameras.value(CID);
		camera->recaster->setRtpDestination(destinationHost, destionationPort);
		cleverdogHandler->startRtp(camera->address, camera->cid, camera->recaster->getRtpPort());
	}
}

void SmarterDog::stopStream(QString CID, QHostAddress destinationHost, uint16_t destionationPort)
{
	if (cameras.contains(CID)) {
		CleverdogCamera *camera = cameras.value(CID);
		cleverdogHandler->stopRtp(camera->address, camera->cid);
	}
}

void SmarterDog::terminate()
{
	emit finished();
}

void SmarterDog::scanTimeout()
{
	foreach (CleverdogCamera *camera, cameras) {
		if (camera->address.isNull()) {
			log("Camera " + cameras.key(camera) + " not detected");
		}
	}
}
