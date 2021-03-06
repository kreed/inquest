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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Col;
class QAction;
class QGraphicsView;
class QMenu;
class TileScene;
class TileView;
template<class T> class QList;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow();

	static MainWindow *instance;

public slots:
	void updateCount(int correct, int remaining);

protected slots:
	void addRemoveMenu(Col*);

protected:
	void resizeEvent(QResizeEvent*);

private:
	QAction *_count;
	QList<QAction*> _groups;
	QMenu *_settingsMenu;
	TileScene *_scene;
	TileView *_view;
};

#endif
