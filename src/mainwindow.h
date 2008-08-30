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
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QGraphicsView;
class QLabel;
class TileScene;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow();

public slots:
	void zoomIn();
	void zoomOut();
	void updateCount(int correct, int remaining);

protected:
	void mouseDoubleClickEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	TileScene *_scene;
	QGraphicsView *_view;
	QAction *_count;
};

extern MainWindow *mainWindow;

#endif
