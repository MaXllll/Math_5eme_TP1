#include "math_modelisation_2d_3d_qt.h"
#include <qboxlayout.h>
#include "Widget.h"
#include <qdebug.h>
#include "OpenGlWindow.h"

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
	mainWidget->getGLWindow()->clearCurrentPointAA();
	model.mode = model.GRAHAMSCAN,
	mainWidget->GrahamScan();
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::JarvisMarch()
{
	mainWidget->getGLWindow()->clearCurrentPointAA();
	model.mode = model.JARVIS,
	mainWidget->JarvisMarch();
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::Triangulation()
{
	model.mode = model.TRIANGULATION,
	mainWidget->Triangulation(false);
	mainWidget->repaint();
}

void Math_modelisation_2D_3D_QT::Flipping()
{
	model.mode = model.FLIPPING,
	//mainWidget->Triangulation(true);
	mainWidget->repaint();
}


void Math_modelisation_2D_3D_QT::Voronoi()
{
	model.mode = model.VORONOI,
	//mainWidget->Voronoi();
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
	jarvisMarchAct = new QAction(tr("&JarvisMarch"), this);
	jarvisMarchAct->setStatusTip(tr("JarvisMarch"));
	connect(jarvisMarchAct, SIGNAL(triggered()), this, SLOT(JarvisMarch()));

	grahamScanAct = new QAction(tr("&Graham-Scan"), this);
	grahamScanAct->setStatusTip(tr("Graham-Scan"));
	connect(grahamScanAct, SIGNAL(triggered()), this, SLOT(GrahamScan()));

	triangulation = new QAction(tr("&Triangulation"), this);
	triangulation->setStatusTip(tr("Triangulation"));
	connect(triangulation, SIGNAL(triggered()), this, SLOT(Triangulation()));

	flipping = new QAction(tr("&Flipping"), this);
	flipping->setStatusTip(tr("Flipping"));
	connect(flipping, SIGNAL(triggered()), this, SLOT(Flipping()));

	voronoi = new QAction(tr("&Voronoi"), this);
	voronoi->setStatusTip(tr("Voronoi"));
	connect(voronoi, SIGNAL(triggered()), this, SLOT(Voronoi()));

	createPoint = new QAction(tr("&createPoint"), this);
	createPoint->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	createPoint->setStatusTip(tr("Create Point"));
	connect(createPoint, SIGNAL(triggered()), this, SLOT(createPointMode()));

	movePoint = new QAction(tr("&movePoint"), this);
	movePoint->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	//clearAct->setShortcuts(new QKeySequence(Qt::CTRL + Qt::Key_C));
	movePoint->setStatusTip(tr("Move Point"));
	connect(movePoint, SIGNAL(triggered()), this, SLOT(movePointMode()));

}

void Math_modelisation_2D_3D_QT::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(clearAct);

	editMenu = menuBar()->addMenu(tr("&Point"));

	editMenu->addAction(createPoint);
	editMenu->addAction(movePoint);

	editMenu = menuBar()->addMenu(tr("&Edit"));

	editMenu->addAction(jarvisMarchAct);
	editMenu->addAction(grahamScanAct);
	editMenu->addAction(triangulation);
	editMenu->addAction(flipping);
	editMenu->addAction(voronoi);




	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
}

Math_modelisation_2D_3D_QT::~Math_modelisation_2D_3D_QT()
{

}

