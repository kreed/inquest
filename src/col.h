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

#ifndef COL_H
#define COL_H

#include <QObject>

class Row;
class Tile;
class TileScene;
template<class T> class QList;

class Col : public QObject {
	Q_OBJECT
public:
	enum LayoutMode {
		Sort,
		Shuffle
	};

	Col(TileScene *parent, int group, LayoutMode);

	Tile *addTile(const QString &text);

	void reset();
	void reveal(bool shown);
	void clear();

	Tile *find(const QString &text);
	Tile *randTile();

	void layout();
	void place(int x, int y);

	inline bool movable() const { return _movable; }
	inline bool visible() const { return _visible; }
	inline int index() const { return _group; }
	inline LayoutMode layoutMode() const { return _layout; }
	inline QList<Tile*> *tiles() { return &_tiles; }
	inline qreal height() const { return _height; }
	inline qreal width() const { return _width; }

signals:
	void layoutChanged();
	void itemCountChanged(int);

public slots:
	void toggleMovable();
	void toggleVisible();
	void setSorted();
	void setShuffled();

protected slots:
	void removeTile(Tile *tile);

private:
	void calcWidth();

	QList<Tile*> _tiles;
	bool _movable;
	bool _visible;
	LayoutMode _layout;
	int _group;
	qreal _height;
	qreal _width;
};

#endif
