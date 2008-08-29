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
 * See <http://www.gnu.org/licenses/> for the full license text.
 */

#ifndef TILE_H
#define TILE_H

#include <QGraphicsTextItem>

template<class K, class V> class QPair;

class Tile : public QGraphicsTextItem {
	Q_OBJECT
public:
	Tile(const QString &text, bool locked);
	~Tile();

	void init(Tile *other, Tile *duplicate);

	Tile *testPair() const;
	void setPair(Tile *other);
	inline bool isCounted() const { return _currentPair; }
	bool isCorrect() const { return _currentPair && _correctShown; }
	inline Tile *pair() const { return _pair; }
	void showCorrect(bool shown = true);

	static bool lessThan(Tile *a, Tile *b);

signals:
	void newPair(Tile *tile, Tile *pair);

protected:
	void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

private:
	Tile *_pair;
	QList<Tile*> *_duplicatePairs;
	Tile *_currentPair;
	bool _moved;
	bool _movable;
	bool _correctShown;
};


#endif
