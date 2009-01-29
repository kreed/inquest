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

#include "row.h"
#include "tile.h"

void Row::destroyTiles()
{
	foreach (Tile *tile, *this)
		tile->deleteLater();
}

void Row::bind()
{
	foreach (Tile *tile, *this)
		tile->setRow(this);
}

void Row::unbind()
{
	foreach (Tile *tile, *this)
		tile->setRow(NULL);
}

void Row::makeDefault()
{
	foreach (Tile *tile, *this)
		tile->_defaultRow = this;
}

void Row::removeOne(Tile *tile)
{
	QList<Tile*>::removeOne(tile);
	if (isEmpty())
		delete this;
}

void Row::showCorrect()
{
	foreach (Tile *tile, *this)
		tile->showCorrect();
}
