#include "plottingwidget.h"

PlottingWidget::PlottingWidget(QWidget *parent) : QWidget(parent)
{
	setMinimumSize(500, 500);

	createCentral();
	createPlot();
	createControls();

	loop = new QTimer(this);
	connect(loop, &QTimer::timeout, this,
			[this](){slider->setValue(this->currentIndex() + 1);});

}

void PlottingWidget::setData(const ArgumentForDraw &data)
{
	m_data = data;

	int nx = m_data.iMax;
	int ny = m_data.jMax;

	colorMap->data()->setSize(nx, ny);
	slider->setMaximum(m_data.tMax - 1);
	slider->setValue(0);
	m_speed = 1;

	plot->rescaleAxes();
}

void PlottingWidget::startDrawing()
{
	loop->stop();
	loop->start(int (m_data.tStep * 1000.0 / m_speed));
	if (currentIndex() < m_data.tMax - 1) {
		slider->setValue(currentIndex() + 1);
	}
}

void PlottingWidget::pauseDrawing()
{
	loop->stop();
}

void PlottingWidget::stopDrawing()
{
	loop->stop();
	slider->setValue(0);
}


void PlottingWidget::drawCurrentLayer()
{
	int nx = m_data.iMax;
	int ny = m_data.jMax;
	double x, y, z;

	for (int xIndex = 0; xIndex < nx; ++xIndex)
	{
	  for (int yIndex = 0; yIndex < ny; ++yIndex)
	  {
		colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
		z = m_data.allLayers[currentIndex()](xIndex, yIndex);
		colorMap->data()->setCell(xIndex, yIndex, z);
	  }
	}
	colorMap->rescaleDataRange();

	plot->replot();
}

void PlottingWidget::createCentral()
{
	main = new QVBoxLayout(this);
	down = new QHBoxLayout();

	setLayout(main);
	layout()->addItem(down);
}

void PlottingWidget::createPlot()
{
	plot = new QCustomPlot(this);

	plot->axisRect()->setupFullAxesBox(true);
	plot->xAxis->setLabel("x");
	plot->yAxis->setLabel("y");

	colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);
	plot->addPlottable(colorMap);

	colorScale = new QCPColorScale(plot);
	plot->plotLayout()->addElement(0, 1, colorScale);
	colorScale->setType(QCPAxis::atRight);
	colorMap->setColorScale(colorScale);

	colorMap->setGradient(QCPColorGradient::gpPolar);
	colorMap->rescaleDataRange();

	marginGroup = new QCPMarginGroup(plot);
	plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
	colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

	plot->rescaleAxes();

	layout()->addWidget(plot);
}

void PlottingWidget::createControls()
{
	slider = new QSlider(Qt::Horizontal, this);
	slider->setMinimum(0);
	slider->setMaximum(1);
	slider->setValue(0);

	play = new QPushButton("Play", this);
	pause = new QPushButton("Pause", this);
	stop = new QPushButton("Stop", this);

	speed = new QDoubleSpinBox(this);
	speed->setMinimum(0.1);
	speed->setMaximum(100);
	speed->setSingleStep(0.1);
	speed->setValue(1);

	down->addWidget(play);
	down->addWidget(pause);
	down->addWidget(stop);
	down->addWidget(slider);
	down->addWidget(speed);


	connect(play, &QPushButton::clicked, this, &PlottingWidget::startDrawing);
	connect(pause, &QPushButton::clicked, this, &PlottingWidget::pauseDrawing);
	connect(stop, &QPushButton::clicked, this, &PlottingWidget::stopDrawing);
	connect(slider, &QSlider::valueChanged, this, &PlottingWidget::setCurrentIndex);
	connect(slider, &QSlider::valueChanged, this, &PlottingWidget::drawCurrentLayer);
	connect(speed, (void (QDoubleSpinBox:: *)(double))&QDoubleSpinBox::valueChanged,
			this, &PlottingWidget::setSpeed);
}

int PlottingWidget::currentIndex() const
{
	return m_currentIndex;
}

void PlottingWidget::setCurrentIndex(int currentIndex)
{
	m_currentIndex = currentIndex;
}

void PlottingWidget::setSpeed(double d)
{
	m_speed = d;
}

