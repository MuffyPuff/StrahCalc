#include "mufmenu.h"


using Muf::translation;

MufMenu::MufMenu(QMenuBar* menu, QObject* parent) : QObject(parent)
{
	mMenuBar = menu;

	createActions();
	createMenus();
	updateText();
}

void
MufMenu::updateText(const QString& lang)
{
	Q_UNUSED(lang);

	mToolsMenu->setTitle(translation("tools"));

	mClear->setText(translation("clear"));
	mClear->setStatusTip(translation("clear_long"));

	mCopyImg->setText(translation("copy_img"));
	mCopyImg->setStatusTip(translation("copy_img_long"));

	mCopyRes->setText(translation("copy_res"));
	mCopyRes->setStatusTip(translation("copy_res_long"));

	mSettings->setText(translation("settings"));
	mSettings->setStatusTip(translation("settings_long"));

	mAbout->setText(translation("about"));
	mAbout->setStatusTip(translation("about"));

	mExit->setText(translation("exit"));
	mExit->setStatusTip(translation("exit"));

}

void
MufMenu::createActions()
{
//	mClear = new QAction(translation("clear"), this);
//	mClear->setStatusTip(translation("clear_long"));

//	mCopyImg = new QAction(translation("copy_img"), this);
//	mCopyImg->setStatusTip(translation("copy_img_long"));

//	mCopyRes = new QAction(translation("copy_res"), this);
//	mCopyRes->setStatusTip(translation("copy_res_long"));

//	mSettings = new QAction(translation("settings"), this);
//	mSettings->setStatusTip(translation("settings_long"));

//	mAbout = new QAction(translation("about"), this);
//	mAbout->setStatusTip(translation("about"));

//	mExit = new QAction(translation("exit"), this);
//	mExit->setStatusTip(translation("exit"));

	mClear = new QAction(this);
	mCopyRes = new QAction(this);
	mCopyImg = new QAction(this);
	mSettings = new QAction(this);
	mAbout = new QAction(this);
	mExit = new QAction(this);
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
