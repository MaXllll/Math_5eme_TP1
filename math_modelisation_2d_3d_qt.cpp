#include "math_modelisation_2d_3d_qt.h"
#include <qboxlayout.h>
#include "Widget.h"
#include <qdebug.h>

Widget* mainWidget;
Model model;

Math_modelisation_2D_3D_QT::Math_modelisation_2D_3D_QT(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	QHBoxLayout* layout = new QHBoxLayout;

	model.mode = model.BSPLINE;

	layout->addWidget(mainWidget = new Widget(&model));
	centralWidget()->setLayout(layout);

	createActions();
	createMenus();

	//readSettings();
}

void Math_modelisation_2D_3D_QT::newBSpline()
{
	mainWidget->newbSpline();
}

void Math_modelisation_2D_3D_QT::clear()
{
	mainWidget->clear();
}

void Math_modelisation_2D_3D_QT::createPointMode()
{
	model.splineMode = model.CREATEPOINT;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::movePointMode()
{
	model.splineMode = model.MOVEPOINT;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::repeatPointMode()
{
	model.splineMode = model.REPEATPOINT;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::BSplineMode()
{
	model.mode = model.BSPLINE;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::ExtrusionMode()
{
	model.mode = model.EXTRUSION;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::BSurfaceMode()
{
	model.mode = model.BSURFACE;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::WireFrameMode()
{
	model.wireFrame = !model.wireFrame;
	mainWidget->repaint();
}


void Math_modelisation_2D_3D_QT::TypeExtrusion()
{
	model.extrusionType = !model.extrusionType;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::createActions()
{
	//New Menu
	newAct = new QAction(tr("&New B-spline"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a B Spline"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newBSpline()));

	//Clear
	clearAct = new QAction(tr("&Clear"), this);
	clearAct->setShortcuts(QKeySequence::Copy);
	clearAct->setStatusTip(tr("Clear"));
	connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));

	//Edit Menu
	createPointAct = new QAction(tr("&Create Point"), this);

	BSplineAct = new QAction(tr("&BSpline"), this);
	//createPointAct->setShortcuts(QKeySequence::New);
	BSplineAct->setStatusTip(tr("BSpline mode"));
	connect(BSplineAct, SIGNAL(triggered()), this, SLOT(BSplineMode()));

	ExtrusionAct = new QAction(tr("&Extrusion"), this);
	//movePointAct->setShortcuts(QKeySequence::New);
	ExtrusionAct->setStatusTip(tr("Extrusion mode"));
	connect(ExtrusionAct, SIGNAL(triggered()), this, SLOT(ExtrusionMode()));

	BSurfaceAct = new QAction(tr("&BSurface"), this);
	//repeatPointAct->setShortcuts(QKeySequence::New);
	BSurfaceAct->setStatusTip(tr("Bezier Surface Mode"));
	connect(BSurfaceAct, SIGNAL(triggered()), this, SLOT(BSurfaceMode()));

	//createPointAct->setShortcuts(QKeySequence::New);
	createPointAct->setStatusTip(tr("Create new Point mode"));
	connect(createPointAct, SIGNAL(triggered()), this, SLOT(createPointMode()));

	movePointAct = new QAction(tr("&Move Point"), this);
	//movePointAct->setShortcuts(QKeySequence::New);
	movePointAct->setStatusTip(tr("Move a point mode"));
	connect(movePointAct, SIGNAL(triggered()), this, SLOT(movePointMode()));

	repeatPointAct = new QAction(tr("&Repeat Point"), this);
	//repeatPointAct->setShortcuts(QKeySequence::New);
	repeatPointAct->setStatusTip(tr("Repeat a point mode"));
	connect(repeatPointAct, SIGNAL(triggered()), this, SLOT(repeatPointMode()));

	WireFrameAct = new QAction(tr("&Wire Frame"), this);
	WireFrameAct->setShortcuts(QKeySequence::Find);
	WireFrameAct->setStatusTip(tr("Wire Frame mode"));
	connect(WireFrameAct, SIGNAL(triggered()), this, SLOT(WireFrameMode()));

	TypeExtrusionAct = new QAction(tr("&Extrusion Type"), this);
	TypeExtrusionAct->setShortcuts(QKeySequence::Replace);
	TypeExtrusionAct->setStatusTip(tr("Extrusion type"));
	connect(TypeExtrusionAct, SIGNAL(triggered()), this, SLOT(TypeExtrusion()));

}

void Math_modelisation_2D_3D_QT::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(clearAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));

	editModeMenu = editMenu->addMenu(tr("&Mode"));
	editModeMenu->addAction(BSplineAct);
	editModeMenu->addAction(ExtrusionAct);
	editModeMenu->addAction(BSurfaceAct);

	editModeMenu = editMenu->addMenu(tr("&Spline Mode"));
	editModeMenu->addAction(createPointAct);
	editModeMenu->addAction(movePointAct);
	editModeMenu->addAction(repeatPointAct);

	editMenu->addAction(WireFrameAct);
	editMenu->addAction(TypeExtrusionAct);

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
}

Math_modelisation_2D_3D_QT::~Math_modelisation_2D_3D_QT()
{

}

