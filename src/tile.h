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

#ifndef TILE_H
#define TILE_H

#include <QGraphicsSimpleTextItem>

class Tile;
class Row;

class Tile : public QObject, public QGraphicsSimpleTextItem {
	Q_OBJECT
public:
	Tile(const QString &text, Tile *dup);
	void deleteLater();

	inline bool isCorrect() const { return _row; }
	inline bool isShownCorrect() const { return _green; }
	inline Row *defaultRow() const { return _defaultRow; }
	inline Row *row() const { return _row; }
	inline Tile *dup() const { return _dup; }

	/* We don't call this ourselves; the TileScene tells us to show correct */
	void showCorrect(bool shown = true);
	void setMovable(bool);
	void bind(Row*);
	void unbind();

	Tile *check(int y);

	inline void makeDefault(Row *row) { _defaultRow = row; }

	static bool lessThan(Tile *a, Tile *b) { return a->text() < b->text(); }

signals:
	void dropped(Tile*);
	void removed(Tile*);

protected:
	void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

private:
	Row *_defaultRow;
	Row *_row;
	Tile *_dup;
	bool _movable;
	bool _green;
};

#endif
