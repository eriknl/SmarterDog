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

#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>

#include "smarterdog.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("SmarterDog");
	QCoreApplication::setApplicationVersion("1.0");
	QTextStream qout(stdout);
	QCommandLineParser parser;
	parser.setApplicationDescription("SmarterDog WiFi camera control tool");
	parser.addHelpOption();
	parser.addVersionOption();
	QCommandLineOption scanOption(QStringList() << "s" << "scan", "Scan for Cleverdog devices");
	parser.addOption(scanOption);
	QCommandLineOption streamOption(QStringList() << "S" << "stream", "Stream video for selected CIDs");
	parser.addOption(streamOption);
	QCommandLineOption cidOption(QStringList() << "c" << "cid",
		    QCoreApplication::translate("main", "Comma separated list of CIDs to use for streaming"),
		    QCoreApplication::translate("main", "CIDs"));
	parser.addOption(cidOption);
	parser.process(app);
	
	SmarterDog smarterDogHandler;
	QObject::connect(&smarterDogHandler, SIGNAL(finished()), &app, SLOT(quit()));
	if (parser.isSet(scanOption)) {
		qout << "Scanning for Cleverdog devices" << endl;
		smarterDogHandler.scan();
	} else if (parser.isSet(streamOption)) {
		if (!parser.isSet(cidOption)) {
			qout << "Must supply CIDs for streaming" << endl;
			return -1;
		}
		qout << "Streaming for CIDs " << parser.value(cidOption) << endl;
		smarterDogHandler.stream(parser.value(cidOption).split(","));
	} else {
		qout << "Select either scanning or streaming" << endl;
		return -1;
	}
	
	return app.exec();
}
