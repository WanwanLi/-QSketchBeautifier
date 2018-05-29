#include <QMenuBar>
#include <QApplication>
#include <QBoxLayout>
#include "QCanvas.h"

class QWindow : public QWidget
{
	Q_OBJECT
	public:	
	QWindow(QWidget* widget=0);

	private slots:
	bool openImage();
	bool saveImage();

	private:
	QCanvas* canvas;
	QMenuBar* newQMenuBar();
	QBoxLayout* newQBoxLayout();
	void closeEvent(QCloseEvent* event);
};
