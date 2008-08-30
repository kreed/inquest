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

#ifndef TILEGROUP_H
#define TILEGROUP_H

#include <QObject>

class Tile;
class TileScene;
template<class T> class QList;

class TileGroup : public QObject {
	Q_OBJECT
public:
	enum LayoutMode {
		Sort,
		Shuffle
	};

	TileGroup(TileScene *parent = NULL);

	inline LayoutMode layoutMode() const { return _layout; }
	void layout(int margin);
	Tile *addTile(const QString &text);
	void removeTile(Tile *tile);
	void clear();
	void reveal(bool shown = true);
	int size() const;
	Tile *whereText(const QString &text);

signals:
	void layoutChanged();

public slots:
	void toggleMovable();
	void toggleVisible();
	void setSorted();
	void setShuffled();

private:
	QList<Tile*> _tiles;
	bool _movable;
	bool _visible;
	LayoutMode _layout;

	friend class TileScene;
};

#endif
