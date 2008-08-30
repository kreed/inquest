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

#include "tilegroup.h"
#include "tile.h"
#include "tilescene.h"

TileGroup::TileGroup(TileScene *parent)
	: QObject(parent)
	, _movable(true)
	, _visible(true)
	, _layout(Shuffle)
{
	connect(this, SIGNAL(layoutChanged()),
	        parent, SLOT(layout()));
}

Tile *TileGroup::addTile(const QString &text)
{
	Tile *tile = new Tile(text, _movable, _visible);
	_tiles.append(tile);
	return tile;
}

int TileGroup::size() const
{
	return _tiles.size();
}

void TileGroup::clear()
{
	foreach (Tile *tile, _tiles)
		delete tile;
	_tiles.clear();
}

Tile *TileGroup::whereText(const QString &text)
{
	foreach (Tile *tile, _tiles)
		if (tile->toPlainText() == text)
			return tile;
	return NULL;
}

void TileGroup::removeTile(Tile *tile)
{
	tile->setPair(NULL);
	_tiles.removeOne(tile);
	delete tile;
}

void TileGroup::reveal(bool shown)
{
	foreach (Tile *tile, _tiles)
		if (tile->isCounted())
			tile->showCorrect(shown);
}

void TileGroup::setSorted()
{
	_layout = Sort;
	emit layoutChanged();
}

void TileGroup::setShuffled()
{
	_layout = Shuffle;
	emit layoutChanged();
}

void TileGroup::toggleMovable()
{
	_movable = !_movable;
	foreach (Tile *tile, _tiles)
		tile->setMovable(_movable);
}

void TileGroup::toggleVisible()
{
	_visible = !_visible;
	foreach (Tile *tile, _tiles)
		tile->setVisible(_visible);
}

void TileGroup::layout(int margin)
{
	QPointF pos(margin, 5);

	if (_layout == Shuffle) {
		QList<Tile*> *copy = new QList<Tile*>(_tiles);

		while (!copy->isEmpty()) {
			copy->takeAt(rand() % copy->size())->setPos(pos);
			pos.ry() += 40;
		}

		delete copy;
	} else {
		qSort(_tiles.begin(), _tiles.end(), Tile::lessThan);

		foreach (Tile *tile, _tiles) {
			tile->setPos(pos);
			pos.ry() += 40;
		}
	}
}


