#include "QWindow.h"
int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	QWindow window;
	window.show();
	return application.exec();
}
