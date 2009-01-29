/*
 * Copyright © 2008-2009 Christopher Eby <kreed@kreed.org>
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
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "col.h"
#include "mainwindow.h"
#include <QAction>
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include "tilescene.h"
#include "tileview.h"

MainWindow *MainWindow::instance = NULL;

static void addToggle(QMenu *menu, const QString &text,
                      QObject *recv, const char *member,
                      bool checked, QActionGroup *group = NULL)
{
	QAction *action = menu->addAction(text, recv, member);
	action->setCheckable(true);
	action->setChecked(checked);
	if (group)
		group->addAction(action);
}

MainWindow::MainWindow()
{
	_scene = new TileScene(this);
	_view = new TileView(_scene);

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
	view->addAction("Zoom In\tCtrl+ScrollDown", _view, SLOT(zoomIn()));
	view->addAction("Zoom Out\tCtrl+ScrollUp", _view, SLOT(zoomOut()));

	view->addSeparator();

	view->addAction("Fewer Words\tShift+ScrollDown", _scene, SLOT(removeOne()));
	view->addAction("More Words\tShift+ScrollUp", _scene, SLOT(addOne()));

	view->addSeparator();

	view->addAction("Relayout", _scene, SLOT(layout()))
		->setShortcut(QKeySequence(QKeySequence::Refresh));


	QMenu *tiles = menu->addMenu("Tiles");
	tiles->addAction("Reset", _scene, SLOT(reset()))
		->setShortcut(QKeySequence("Backspace"));
	tiles->addAction("Check/Proceed", _scene, SLOT(checkAdvance()))
		->setShortcut(QKeySequence("Space"));
	tiles->addAction("Skip", _scene, SLOT(skip()))
		->setShortcut(QKeySequence("Tab"));


	_settingsMenu = menu->addMenu("Settings");

	_settingsMenu->addSeparator()->setText("Checking Mode");

	QActionGroup *group = new QActionGroup(this);
	addToggle(_settingsMenu, "Check on Place", _scene, SLOT(setPlacementAuto()), true, group);
	addToggle(_settingsMenu, "Check on Space Press", _scene, SLOT(setPlacementManual()), false, group);
	addToggle(_settingsMenu, "Check when All Correct", _scene, SLOT(setPlacementNo()), false, group);

	_settingsMenu->addSeparator();

	_count = menu->addAction("", _scene, SLOT(checkAdvance()));
	connect(_scene, SIGNAL(countChanged(int, int)),
	        this, SLOT(updateCount(int, int)));

	connect(_scene, SIGNAL(addRemoveGroup(Col*)),
	        this, SLOT(addRemoveMenu(Col*)));

	setMenuBar(menu);

	_scene->init();

	instance = this;
}

void MainWindow::addRemoveMenu(Col *tiles)
{
	if (tiles) {
		QMenu *menu = _settingsMenu->addMenu(QString("Column %1").arg(tiles->index() + 1));
		addToggle(menu, "Movable", tiles, SLOT(toggleMovable()), tiles->movable());
		addToggle(menu, "Visible", tiles, SLOT(toggleVisible()), tiles->visible());
		menu->addSeparator()->setText("Order Mode");
		QActionGroup *group = new QActionGroup(menu);
		addToggle(menu, "Sort", tiles, SLOT(setSorted()), tiles->layoutMode() == Col::Sort, group);
		addToggle(menu, "Shuffle", tiles, SLOT(setShuffled()), tiles->layoutMode() == Col::Shuffle, group);
		_groups.append(menu->menuAction());
	} else
		_settingsMenu->removeAction(_groups.takeLast());
}

void MainWindow::updateCount(int correct, int remaining)
{
	QString text = QString("%2/%1").arg(remaining);
	text = correct == -1 ? text.arg("??") : text.arg(correct);
	_count->setText(text);
}

void MainWindow::resizeEvent(QResizeEvent *ev)
{
	_view->fit(_scene->sceneRect());
	QMainWindow::resizeEvent(ev);
}
