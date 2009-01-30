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

#ifndef ROW_H
#define ROW_H

#include <QObject>

class Tile;
template<class T> class QList;

class Row : public QObject, private QList<Tile*> {
	Q_OBJECT
public:
	void add(Tile*);
	void destroyTiles();
	void makeDefault();
	void bind();
	void unbind();
	void showCorrect();
	QString entry() const;

signals:
	void newRow(Row*);

public slots:
	void checkRow(Tile*);
	void remove(Tile*);
};

QDataStream &operator<<(QDataStream&, const Row*);

#endif
