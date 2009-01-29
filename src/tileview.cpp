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

#include <QWheelEvent>
#include "tilescene.h"
#include "tileview.h"

TileView::TileView(TileScene *scene)
	: QGraphicsView(scene)
	, _scene(scene)
{
	connect(_scene, SIGNAL(sceneRectChanged(const QRectF&)),
	        this, SLOT(fit(const QRectF&)));
}

void TileView::zoomIn()
{
	scale(1.2, 1.2);
}

void TileView::zoomOut()
{
	scale(0.8, 0.8);
}

void TileView::fit(const QRectF &rect)
{
	fitInView(rect, Qt::KeepAspectRatio);
}

void TileView::mouseDoubleClickEvent(QMouseEvent*)
{
	_scene->checkAdvance();
}

void TileView::wheelEvent(QWheelEvent *ev)
{
	if (ev->orientation() != Qt::Vertical)
		goto pass;

	if (ev->modifiers() & Qt::ControlModifier) {
		if (ev->delta() < 0)
			zoomOut();
		else
			zoomIn();
	} else if (ev->modifiers() & Qt::ShiftModifier) {
		if (ev->delta() < 0)
			_scene->removeOne();
		else
			_scene->addOne();
	} else
		goto pass;

	ev->accept();
	return;

pass:
	QGraphicsView::wheelEvent(ev);
}
