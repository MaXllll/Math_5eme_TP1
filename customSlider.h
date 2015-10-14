#include <QtWidgets\qwidget.h>
#include <QtWidgets\qslider.h>
#include <QtWidgets\qlabel.h>
#include <string.h>

class customSlider : public QWidget
{
	Q_OBJECT

	QSlider* slider;
	QLabel* label;
	float sliderGranularity;
	float min;
	float max;
	std::string title;

private slots:
	void sliderValueChanged();

signals:
	void valueChanged(float newValue);

public:
	customSlider(float value = 0.0f, float min = 20, float max = 500, bool textOnLeft = false, float granularity = 40.0, std::string title = "Pas");
	float value() const;
	void setValue(float newValue);
};

