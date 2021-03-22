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

#ifndef CLEVERDOGCAMERA_H
#define CLEVERDOGCAMERA_H

#include <QHostAddress>
#include "mediarecaster.h"

class CleverdogCamera
{
public:
	CleverdogCamera(QHostAddress address = QHostAddress::Null, QString cid = "", QString mac = "", QString firmware = "");
	QHostAddress address;
	QString cid;
	QString mac;
	QString firmware;
	MediaRecaster *recaster;
private:
	
};

#endif // CLEVERDOGCAMERA_H
