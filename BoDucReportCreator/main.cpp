#include "BoDucReportCreator.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	bdApp::BoDucReportCreator w;
	w.show();
  return a.exec();
}
