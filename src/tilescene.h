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

#ifndef TILESCENE_H
#define TILESCENE_H

#include <QGraphicsScene>

class Tile;
template<class K, class V> class QPair;
template<class T> class QList;

typedef QPair<QString, QString> StringPair;

class TileScene : public QGraphicsScene {
	Q_OBJECT
public:
	enum LayoutMode {
		Sort,
		Shuffle
	};
	enum PlacementMode {
		AutoCheck,
		ManualCheck,
		NoCheck
	};

	TileScene(QObject *parent = NULL);

	QString stateFile() const;
	void init();
	inline LayoutMode wordMode() const { return _wordMode; }
	inline LayoutMode meaningMode() const { return _meaningMode; }

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
	void setWordsSorted();
	void setWordsShuffled();
	void setMeaningsSorted();
	void setMeaningsShuffled();
	void toggleWordsLocked();
	void toggleMeaningsLocked();
	void setPlacement(PlacementMode);
	void setPlacementAuto() { setPlacement(AutoCheck); }
	void setPlacementManual() { setPlacement(ManualCheck); }
	void setPlacementNo() { setPlacement(NoCheck); }

protected slots:
	void onPair(Tile*, Tile*);

private:
	void add();
	Tile *addTile(const QString &text, QList<Tile*> &list, bool movable);
	void advance();
	void reveal();
	void layout(QList<Tile*> &tiles, LayoutMode mode, int margin);
	void updateCount();
	void removeWord(Tile *tile);

	int _groupSize;
	int _correctCount;
	bool _wordsLocked;
	bool _wordsShown;
	bool _meaningsLocked;
	bool _meaningsShown;
	LayoutMode _wordMode;
	LayoutMode _meaningMode;
	PlacementMode _placeMode;

	QList<StringPair> _bank;
	QList<Tile*> _words;
	QList<Tile*> _meanings;
};

#endif
