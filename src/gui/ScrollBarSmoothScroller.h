

#ifndef __SCROLLBAR_SMOOTH_SCROLLER_H
#define __SCROLLBAR_SMOOTH_SCROLLER_H

#include <QObject>
#include <QScrollBar>
#include <QTimer>
#include <QEvent>

class ScrollBarSmoothScroller : public QObject
{
	Q_OBJECT
private:
	QTimer _timer;
	QScrollBar *_scrollee;
	int _destination;

private slots:
	void onScrollBarAction(int action);
	void updateAnimationState();

protected:
	/// Used to process scroll events on the scrollbar
	virtual bool eventFilter(QObject *watched, QEvent *event);

public:
	ScrollBarSmoothScroller(QObject *parent = 0);
	void setScrollBar(QScrollBar *bar);
	QScrollBar *scrollBar() const { return _scrollee; }
};

#endif
