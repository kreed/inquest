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

#ifndef TILESCENE_H
#define TILESCENE_H

#include <QGraphicsScene>

class Col;
class Row;
class Tile;
template<class T> class QList;

class TileScene : public QGraphicsScene {
	Q_OBJECT
public:
	enum PlacementMode {
		AutoCheck,
		ManualCheck,
		NoCheck
	};

	TileScene(QObject *parent = NULL);
	void init();

	void place();

	QString stateFile() const;

signals:
	void countChanged(int correct, int remaining);
	void addRemoveGroup(Col*);

public slots:
	void addOne();
	void removeOne();
	void checkAdvance();
	void skip();
	void fill();
	void fillState(bool error = true);
	bool fill(const QString&, bool showError = true);
	void dump();
	void dumpState();
	void dump(const QString&);
	void layout();
	void reset();
	void quitNow();
	void setPlacement(PlacementMode);
	void setPlacementAuto() { setPlacement(AutoCheck); }
	void setPlacementManual() { setPlacement(ManualCheck); }
	void setPlacementNo() { setPlacement(NoCheck); }

protected slots:
	void onBind(Row*);
	void setRowCount(int);

private:
	void add();
	Tile *addTile(const QString &text, Col *group);
	void removeTile(Tile *tile);
	void setColCount(int);
	void updateCounts();
	void shiftCorrectCount(int delta);
	void reveal(bool show = true);
	void stripCorrect();
	void advance();

	int _colCount;
	int _rowCount;
	int _curRowCount;
	int _correctCount;
	PlacementMode _placeMode;

	QList<QString> _bank;
	QList<Col*> _cols;
};

#endif
