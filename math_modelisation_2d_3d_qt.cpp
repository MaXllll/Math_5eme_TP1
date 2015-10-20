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

	model.mode = model.GRAHAMSCAN;

	layout->addWidget(mainWidget = new Widget(&model));
	centralWidget()->setLayout(layout);

	createActions();
	createMenus();

	//readSettings();
}

void Math_modelisation_2D_3D_QT::newCluster()
{
	mainWidget->NewCluster();
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
	//model.splineMode = model.REPEATPOINT;
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::GrahamScan()
{
	mainWidget->GrahamScan();
	mainWidget->repaint();
}
void Math_modelisation_2D_3D_QT::createActions()
{
	//New Menu
	newAct = new QAction(tr("&New Cluster"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("New Cluster Points"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newCluster()));

	//Clear
	clearAct = new QAction(tr("&Clear"), this);
	clearAct->setShortcuts(QKeySequence::Copy);
	clearAct->setStatusTip(tr("Clear"));
	connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));

	//Edit Menu
	grahamScanAct = new QAction(tr("&Graham-Scan"), this);
	grahamScanAct->setStatusTip(tr("Graham-Scan"));
	connect(grahamScanAct, SIGNAL(triggered()), this, SLOT(GrahamScan()));

}

void Math_modelisation_2D_3D_QT::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(clearAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));

	editMenu->addAction(grahamScanAct);

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
}

Math_modelisation_2D_3D_QT::~Math_modelisation_2D_3D_QT()
{

}

