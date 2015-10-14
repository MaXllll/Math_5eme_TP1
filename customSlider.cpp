#include <QtWidgets\qboxlayout.h>
#include <QtWidgets\qslider.h>
#include <QtWidgets\qlabel.h>
#include "customSlider.h"


customSlider::customSlider(float value, float min, float max, bool textOnLeft, float granularity, std::string title)
{
	QLayout* layout;

	this->title = title;
	this->min = min;
	this->max = max;
	sliderGranularity = granularity;
	setLayout(layout = textOnLeft ? (QLayout*) new QHBoxLayout : new QVBoxLayout);
	layout->addWidget(label = new QLabel);
	label->setMidLineWidth(35);
	layout->addWidget(slider = new QSlider);
	label->setAlignment(Qt::AlignCenter);
	slider->setOrientation(Qt::Horizontal);
	slider->setMinimum(0);
	slider->setMaximum(sliderGranularity);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged()));
	setValue(value);
	this->setFixedHeight(60);

}

float customSlider::value() const
{
	return min + (max - min) * (slider->value() / sliderGranularity);
}

void customSlider::setValue(float newValue)
{
	float spot = (newValue - min) / (max - min);
	slider->setValue(spot * sliderGranularity);
}

void customSlider::sliderValueChanged()
{
	label->setText(QString(this->title.c_str()) + " : " + QString::number(value()));
	emit valueChanged(value());
}