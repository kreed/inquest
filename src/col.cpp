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
#include "row.h"
#include "tile.h"
#include "tilescene.h"

Col::Col(TileScene *parent, int group, LayoutMode mode)
	: QObject(parent)
	, _movable(true)
	, _visible(true)
	, _layout(mode)
	, _group(group)
	, _height(0)
	, _width(0)
{
}

Tile *Col::find(const QString &text)
{
	foreach (Tile *tile, _tiles)
		if (tile->text() == text)
			return tile;
	return NULL;
}

Tile *Col::addTile(const QString &text)
{
	Tile *tile = new Tile(text, find(text));
	tile->setVisible(_visible);
	tile->setMovable(_movable);
	connect(tile, SIGNAL(removed(Tile*)),
	        this, SLOT(removeTile(Tile*)));
	_tiles.append(tile);
	_width = qMax(_width, tile->boundingRect().width());
	emit itemCountChanged(_tiles.size());
	return tile;
}

void Col::clear()
{
	foreach (Tile *tile, _tiles)
		tile->deleteLater();
	_tiles.clear();
	_width = 0;
}

void Col::calcWidth()
{
	foreach (Tile *tile, _tiles)
		_width = qMax(_width, tile->boundingRect().width());
}

void Col::removeTile(Tile *tile)
{
	_tiles.removeOne(tile);
	emit itemCountChanged(_tiles.size());
	if (tile->boundingRect().width() == _width)
		calcWidth();
}

void Col::reveal(bool shown)
{
	foreach (Tile *tile, _tiles)
		if (tile->isCorrect())
			tile->showCorrect(shown);
}

void Col::setSorted()
{
	_layout = Sort;
	emit layoutChanged();
}

void Col::setShuffled()
{
	_layout = Shuffle;
	emit layoutChanged();
}

void Col::toggleMovable()
{
	_movable = !_movable;
	foreach (Tile *tile, _tiles)
		tile->setMovable(_movable);
}

void Col::toggleVisible()
{
	_visible = !_visible;
	foreach (Tile *tile, _tiles)
		tile->setVisible(_visible);
}

void Col::layout()
{
	switch (_layout) {
	case Shuffle:
		for (int i = 0, len = _tiles.size(); i != len - 1; ++i)
			_tiles.swap(i, i + rand() % (len - i));
		break;
	case Sort:
		qSort(_tiles.begin(), _tiles.end(), Tile::lessThan);
		break;
	}
}

void Col::place(int xoffset, int yoffset)
{
	QPointF pos(xoffset, yoffset);

	foreach (Tile *tile, _tiles) {
		tile->setPos(pos);
		pos.ry() += tile->boundingRect().height() * 1.5;
	}

	_height = pos.y();
}

void Col::reset()
{
	foreach (Tile *tile, _tiles)
		tile->row()->unbind();
}

Tile *Col::randTile()
{
	return _tiles.at(rand() % _tiles.size());
}
