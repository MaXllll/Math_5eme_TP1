#ifndef MATH_MODELISATION_2D_3D_QT_H
#define MATH_MODELISATION_2D_3D_QT_H

#include <QtWidgets/QMainWindow>
#include "ui_math_modelisation_2d_3d_qt.h"

class Math_modelisation_2D_3D_QT : public QMainWindow
{
	Q_OBJECT

public:
	Math_modelisation_2D_3D_QT(QWidget *parent = 0);
	~Math_modelisation_2D_3D_QT();

private slots:
	void newBSpline();
	void clear();

	void createPointMode();
	void movePointMode();
	void repeatPointMode();

	void BSplineMode();
	void ExtrusionMode();
	void BSurfaceMode();

	void WireFrameMode();

	void TypeExtrusion();

private:
	void createActions();
	void createMenus();
	//QString strippedName(const QString &fullFileName);

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *editModeMenu;
	QMenu *helpMenu;

	QAction *newAct;
	QAction *clearAct;

	QAction *createPointAct;
	QAction *movePointAct;
	QAction *repeatPointAct;

	QAction *BSplineAct;
	QAction *ExtrusionAct;
	QAction *BSurfaceAct;

	QAction *WireFrameAct;
	QAction *TypeExtrusionAct;

private:
	Ui::Math_modelisation_2D_3D_QTClass ui;

};

#endif // MATH_MODELISATION_2D_3D_QT_H
