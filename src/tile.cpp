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

#include <QBrush>
#include "row.h"
#include "tile.h"

Tile::Tile(const QString &text, Tile *dup)
	: QGraphicsSimpleTextItem(text)
	, _defaultRow(NULL)
	, _row(NULL)
	, _movable(false)
	, _green(false)
{
	if (dup) {
		_dup = dup->_dup ? dup->_dup : dup;
		dup->_dup = this;
	} else
		_dup = NULL;
}

void Tile::deleteLater()
{
	if (_dup) {
		Tile *e = this;
		while (e->_dup != this)
			e = e->_dup;
		e->_dup = _dup;
	}

	emit removed(this);
	QObject::deleteLater();
}

void Tile::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
	emit dropped(this);
	QGraphicsItem::mouseReleaseEvent(ev);
}

Tile *Tile::check(int y)
{
	Tile *tile = this;

	do {
		if (tile->y() > y - 10 && tile->y() < y + 5 && !tile->_green)
			return tile;
	} while ((tile = tile->_dup) && tile != this);

	return NULL;
}

void Tile::unbind()
{
	if (!_row)
		return;

	_row = NULL;

	if (_green)
		showCorrect(false);
}

void Tile::bind(Row *row)
{
	if (_row)
		_row->unbind();

	_row = row;
}

void Tile::showCorrect(bool shown)
{
	static const QBrush green = QBrush(QColor(0, 255, 0, 85));

	if (shown != _green) {
		setBrush(shown ? green : Qt::black);
		setFlag(QGraphicsItem::ItemIsMovable, !shown);
		_green = shown;
		update();
	}
}

void Tile::setMovable(bool movable)
{
	setFlag(QGraphicsItem::ItemIsMovable, movable && !_green);
	_movable = movable;
}
