/*
 * Copyright © 2009 Christopher Eby <kreed@kreed.org>
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

#ifndef TILEVIEW_H
#define TILEVIEW_H

#include <QGraphicsView>

class TileScene;

class TileView : public QGraphicsView {
	Q_OBJECT
public:
	TileView(TileScene *scene);

public slots:
	void zoomIn();
	void zoomOut();
	void fit(const QRectF&);

protected:
	void mouseDoubleClickEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	TileScene *_scene;
};

#endif
