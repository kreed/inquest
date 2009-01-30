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

#include <QList>
#include "row.h"
#include "tile.h"

void Row::add(Tile *tile)
{
	connect(tile, SIGNAL(dropped(Tile*)),
	        this, SLOT(checkRow(Tile*)));
	connect(tile, SIGNAL(removed(Tile*)),
	        this, SLOT(remove(Tile*)));
	append(tile);
}

void Row::destroyTiles()
{
	QListIterator<Tile*> i(*this);
	while (i.hasNext())
		i.next()->deleteLater();
}

void Row::bind()
{
	QListIterator<Tile*> i(*this);
	while (i.hasNext())
		i.next()->bind(this);
}

void Row::unbind()
{
	QListIterator<Tile*> i(*this);
	while (i.hasNext())
		i.next()->unbind();
	delete this;
}

void Row::makeDefault()
{
	QListIterator<Tile*> i(*this);
	while (i.hasNext())
		i.next()->makeDefault(this);
}

void Row::remove(Tile *tile)
{
	removeOne(tile);
	if (isEmpty())
		delete this;
}

void Row::showCorrect()
{
	QListIterator<Tile*> i(*this);
	while (i.hasNext())
		i.next()->showCorrect();
}

void Row::checkRow(Tile *start)
{
	Row *oldRow = start->row();
	Row *row = new Row;
	Tile *checked;
	Tile *curColTile = start;
	int y = start->y();

	do {
		Tile *tile;
		QListIterator<Tile*> i(*this);
		while (i.hasNext()) {
			tile = i.next();
			if (tile == curColTile)
				row->append(start);
			else if ((checked = tile->check(y)))
				row->append(checked);
			else
				goto iter;
		}

		row->bind();
		goto sig;

	iter:
		row->clear();
	} while ((curColTile = curColTile->dup()) && curColTile != start);

	delete row;
	row = NULL;
	if (start->row())
		start->row()->unbind();

sig:
	if (row != oldRow)
		emit newRow(row);
}

QString Row::entry() const
{
	QString result;
	QListIterator<Tile*> i(*this);
	while (i.hasNext())
		result += i.next()->text() + '\t';
	result.chop(1);
	return result;
}

QDataStream &operator<<(QDataStream &stream, const Row *row)
{
	return stream << row->entry();
}
