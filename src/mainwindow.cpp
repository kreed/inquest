/*
 * Copyright © 2008 Christopher Eby <kreed@kreed.org>
 *
 * This file is part of Inquest.
 *
 * Inquest is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Inquest is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See <http://www.gnu.org/licenses/> for the full license text.
 */

#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QGraphicsView>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QWheelEvent>
#include <QWidgetAction>
#include "tilescene.h"

MainWindow *mainWindow;

void addToggle(QMenu *menu, QActionGroup *group, const QString &text,
               QObject *recv, const char *member, bool checked = false)
{
	QAction *action = menu->addAction(text, recv, member);
	action->setCheckable(true);
	if (checked)
		action->setChecked(true);
	group->addAction(action);
}

MainWindow::MainWindow()
	: QMainWindow()
	, _scene(new TileScene(this))
	, _view(new QGraphicsView(_scene))
{
	setCentralWidget(_view);

	QMenuBar *menu = new QMenuBar(this);

	QMenu *file = menu->addMenu("File");

	file->addAction("Load Words...", _scene, SLOT(fill()))
		->setShortcut(QKeySequence(QKeySequence::Open));
	file->addAction("Load Previous Session", _scene, SLOT(fillState()));

	file->addSeparator();

	file->addAction("Save Words...", _scene, SLOT(dump()))
		->setShortcut(QKeySequence(QKeySequence::Save));
	file->addAction("Save Session Now", _scene, SLOT(dumpState()));

	file->addSeparator();

	file->addAction("Quit, Saving Session", qApp, SLOT(closeAllWindows()))
		->setShortcut(QKeySequence("Ctrl+Q"));
	file->addAction("Quit, Discarding Session", _scene, SLOT(quitNow()))
		->setShortcut(QKeySequence("Shift+Ctrl+Q"));


	QMenu *view = menu->addMenu("View");
    view->addAction("Zoom In\tCtrl+ScrollDown", this, SLOT(zoomIn()));
    view->addAction("Zoom Out\tCtrl+ScrollUp", this, SLOT(zoomOut()));

	view->addSeparator();

    view->addAction("Fewer Words\tShift+ScrollDown", _scene, SLOT(removeOne()));
    view->addAction("More Words\tShift+ScrollUp", _scene, SLOT(addOne()));

	view->addSeparator();

	view->addAction("Relayout", _scene, SLOT(layout()))
		->setShortcut(QKeySequence(QKeySequence::Refresh));


	QMenu *settings = menu->addMenu("Settings");

	QMenu *mode = settings->addMenu("Words");
	mode->addAction("Locked", _scene, SLOT(toggleWordsLocked()))
		->setCheckable(true);
	mode->addSeparator()->setText("Order Mode");
	QActionGroup *group = new QActionGroup(this);
	QAction *sort = group->addAction(mode->addAction("Sort", _scene, SLOT(setWordsSorted())));
	QAction *shuffle = group->addAction(mode->addAction("Shuffle", _scene, SLOT(setWordsShuffled())));
	sort->setCheckable(true);
	shuffle->setCheckable(true);
	(_scene->wordMode() == TileScene::Sort ? sort : shuffle)->setChecked(true);

	mode = settings->addMenu("Meanings");
	mode->addAction("Locked", _scene, SLOT(toggleMeaningsLocked()))
		->setCheckable(true);
	mode->addSeparator()->setText("Order Mode");
	group = new QActionGroup(this);
	sort = group->addAction(mode->addAction("Sort", _scene, SLOT(setMeaningsSorted())));
	shuffle = group->addAction(mode->addAction("Shuffle", _scene, SLOT(setMeaningsShuffled())));
	sort->setCheckable(true);
	shuffle->setCheckable(true);
	(_scene->meaningMode() == TileScene::Sort ? sort : shuffle)->setChecked(true);

	settings->addSeparator()->setText("Checking Mode");

	group = new QActionGroup(this);
	addToggle(settings, group, "Check on Place", _scene, SLOT(setPlacementAuto()), true);
	addToggle(settings, group, "Check on Space Press", _scene, SLOT(setPlacementManual()));
	addToggle(settings, group, "Check when All Correct", _scene, SLOT(setPlacementNo()));

	menu->addAction("Check/Advance", _scene, SLOT(checkAdvance()))
		->setShortcut(QKeySequence("Space"));

	_count = menu->addAction("", _scene, SLOT(checkAdvance()));
	connect(_scene, SIGNAL(countChanged(int, int)),
	        this, SLOT(updateCount(int, int)));

	setMenuBar(menu);

	_scene->init();

	mainWindow = this;
}

void MainWindow::zoomIn()
{
	_view->scale(1.2, 1.2);
}

void MainWindow::zoomOut()
{
	_view->scale(0.8, 0.8);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent*)
{
	_scene->checkAdvance();
}

void MainWindow::wheelEvent(QWheelEvent *ev)
{
	if (ev->orientation() == Qt::Vertical) {
		if (ev->modifiers() & Qt::ControlModifier) {
			if (ev->delta() < 0)
				zoomOut();
			else
				zoomIn();
			ev->accept();
		} else if (ev->modifiers() & Qt::ShiftModifier) {
			if (ev->delta() < 0)
				_scene->removeOne();
			else
				_scene->addOne();
			ev->accept();
		}
	} else
		QMainWindow::wheelEvent(ev);
}

void MainWindow::updateCount(int correct, int remaining)
{
	_count->setText(QString("%1/%2").arg(correct).arg(remaining));
}
