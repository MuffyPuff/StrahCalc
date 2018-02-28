#include "mufmenu.h"


using Muf::translation;

MufMenu::MufMenu(QMenuBar* menu, QObject* parent) : QObject(parent)
{
	mMenuBar = menu;

	createActions();
	createMenus();
}

void
MufMenu::createActions()
{
	mClear = new QAction(translation("clear"), this);
	mClear->setStatusTip(translation("clear_long"));

	mCopyImg = new QAction(translation("copy_img"), this);
	mCopyImg->setStatusTip(translation("copy_img_long"));

	mCopyRes = new QAction(translation("copy_res"), this);
	mCopyRes->setStatusTip(translation("copy_res_long"));

	mSettings = new QAction(translation("settings"), this);
	mSettings->setStatusTip(translation("settings_long"));

	mAbout = new QAction(translation("about"), this);
	mAbout->setStatusTip(translation("about"));

	mExit = new QAction(translation("exit"), this);
	mExit->setStatusTip(translation("exit"));
}

void
MufMenu::createMenus()
{
	mToolsMenu = mMenuBar->addMenu(translation("tools"));
	mToolsMenu->addAction(mClear);
	mToolsMenu->addSeparator();
	mToolsMenu->addAction(mCopyImg);
	mToolsMenu->addAction(mCopyRes);
	mToolsMenu->addSeparator();
	mToolsMenu->addAction(mSettings);
	mToolsMenu->addAction(mAbout);
	mToolsMenu->addAction(mExit);
}
