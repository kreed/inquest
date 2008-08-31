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

#ifndef TILESCENE_H
#define TILESCENE_H

#include <QGraphicsScene>

class Tile;
class TileGroup;
template<class K, class V> class QPair;
template<class T> class QList;

typedef QPair<QString, QString> StringPair;

class TileScene : public QGraphicsScene {
	Q_OBJECT
public:
	enum PlacementMode {
		AutoCheck,
		ManualCheck,
		NoCheck
	};

	TileScene(QObject *parent = NULL);

	QString stateFile() const;
	void init();

	inline TileGroup *words() const { return _words; }
	inline TileGroup *meanings() const { return _meanings; }

signals:
	void countChanged(int correct, int remaining);

public slots:
	void addOne();
	void removeOne();
	void checkAdvance();
	void fill();
	void fillState();
	bool fill(const QString&, bool showError = true);
	void dump();
	void dumpState();
	void dump(const QString&);
	void layout();
	void quitNow();
	void setPlacement(PlacementMode);
	void setPlacementAuto() { setPlacement(AutoCheck); }
	void setPlacementManual() { setPlacement(ManualCheck); }
	void setPlacementNo() { setPlacement(NoCheck); }

protected slots:
	void onPair(Tile*, Tile*);

protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);

private:
	void add();
	Tile *addTile(const QString &text, TileGroup *group);
	void advance();
	void reveal();
	void updateCount();
	void removeWord(Tile *tile);

	int _groupSize;
	int _correctCount;
	PlacementMode _placeMode;

	QList<StringPair> _bank;
	TileGroup *_words;
	TileGroup *_meanings;
};

#endif
