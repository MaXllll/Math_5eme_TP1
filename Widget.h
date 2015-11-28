#include "customSlider.h"
#include <QtWidgets\qwidget.h>
#include "Model.h"

class OpenGlWindow;

class Widget : public QWidget
{
	Q_OBJECT

	customSlider* dSlider;
	customSlider* pSlider;

	Model* model;

private slots:
	void sliderValueChanged();

//signals:
//	void valueChanged(float newValue);

public:
	Widget(Model* model);
	void NewCluster();
	void GrahamScan();
	void JarvisMarch();
	void Triangulation();
	void repaint();
	void clear();
	OpenGlWindow* getGLWindow();
};

