#ifndef MUFMENU_H
#define MUFMENU_H

#include <QObject>

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QLabel>

#include "global.h"

class MufMenu : public QObject
{
	Q_OBJECT
public:
	explicit MufMenu(QMenuBar* menu, QObject* parent = nullptr);

private:
	void createActions();
	void createMenus();

signals:

public slots:

private slots:

public:
	QMenuBar*       mMenuBar;

	QMenu*          mToolsMenu;
	QAction*        mClear;
	QAction*        mCopyImg;
	QAction*        mCopyRes;
	QAction*        mSettings;
	QAction*        mAbout;
	QAction*        mExit;
};

#endif // MUFMENU_H
