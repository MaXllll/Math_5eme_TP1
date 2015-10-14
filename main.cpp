#include "math_modelisation_2d_3d_qt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Math_modelisation_2D_3D_QT w;
	w.show();
	return a.exec();
}
