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

#include "col.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QGraphicsSceneWheelEvent>
#include <QMessageBox>
#include <QTextStream>
#include "row.h"
#include "tile.h"
#include "tilescene.h"

TileScene::TileScene(QObject *parent)
	: QGraphicsScene(parent)
	, _colCount(0)
	, _rowCount(16)
	, _curRowCount(0)
	, _correctCount(0)
	, _placeMode(AutoCheck)
{
	connect(qApp, SIGNAL(lastWindowClosed()),
	        this, SLOT(dumpState()));
}

void TileScene::init()
{
	setColCount(2);
	connect(_cols.at(0), SIGNAL(itemCountChanged(int)),
	        this, SLOT(setRowCount(int)));
	fillState(false);
}

void TileScene::setRowCount(int v)
{
	_curRowCount = v;
}

void TileScene::updateCounts()
{
	if (_correctCount == _curRowCount)
		emit countChanged(_correctCount, _bank.size());
	else if (_placeMode == NoCheck)
		emit countChanged(-1, _curRowCount + _bank.size());
	else
		emit countChanged(_correctCount, _curRowCount + _bank.size() - _correctCount);
}

void TileScene::shiftCorrectCount(int delta)
{
	if (delta == 0)
		_correctCount = 0;
	else
		_correctCount += delta;

	updateCounts();
}

void TileScene::quitNow()
{
	disconnect(qApp, SIGNAL(lastWindowClosed()),
	        this, SLOT(dumpState()));
	qApp->closeAllWindows();
}

QString TileScene::stateFile() const
{
	return QDir::homePath() + "/.config/inquest.state";
}

void TileScene::fill()
{
	QStringList files = QFileDialog::getOpenFileNames(MainWindow::instance, QString(), QString(), "Tab-separated values (*.tsv);;All Files (*)");
	if (!files.isEmpty()) {
		foreach (QString file, files)
			if (!fill(file))
				return;

		advance();
	}
}

void TileScene::fillState(bool error)
{
	if (fill(stateFile(), error))
		advance();
}

bool TileScene::fill(const QString &file, bool showError)
{
	QFile store(file);
	if (!store.open(QFile::ReadOnly)) {
		if (showError)
			QMessageBox::critical(MainWindow::instance, "Error reading file", store.errorString());
		return false;
	}

	_bank.clear();
	QTextStream in(&store);

	QString line = in.readLine();

	int count = line.count('\t');
	if (count < 1) {
		if (showError)
			QMessageBox::critical(MainWindow::instance, "Error reading file", "File '" + file + "' does not look like a tab-separated value file.");
		return false;
	}

	do {
		if (line.count('\t') >= count)
			_bank.append(line);
		line = in.readLine();
	} while (!in.atEnd());

	store.close();

	setColCount(count + 1);

	return true;
}

void TileScene::dump()
{
	QString file = QFileDialog::getSaveFileName(MainWindow::instance, QString(), QString(), "Tab-separated values (*.tsv)");
	if (!file.isEmpty())
		dump(file);
}

void TileScene::dumpState()
{
	if (_bank.isEmpty() && !_curRowCount)
		QFile::remove(stateFile());
	else
		dump(stateFile());
}

void TileScene::dump(const QString &file)
{
	QFile store(file);
	if (!store.open(QFile::WriteOnly))
		QMessageBox::critical(MainWindow::instance, "Error writing file", store.errorString());

	QTextStream out(&store);

	foreach(const QString entry, _bank)
		out << entry << '\n';
	foreach(Tile *tile, *_cols.at(0)->tiles())
		if (!tile->isShownCorrect())
			out << tile->defaultRow()->entry() << '\n';

	store.close();
}

void TileScene::advance()
{
	foreach (Col *col, _cols)
		col->clear();
	add();
	shiftCorrectCount(0);
	layout();
}

Tile *TileScene::addTile(const QString &text, Col *col)
{
	Tile *tile = col->addTile(text);
	addItem(tile);
	return tile;
}

void TileScene::add()
{
	while (_curRowCount != _rowCount && !_bank.isEmpty()) {
		QStringList entry = _bank.takeAt(rand() % _bank.size()).split('\t');
		Row *row = new Row;
		for (int i = 0; i != _colCount; ++i)
			row->add(addTile(entry[i], _cols[i]));
		row->makeDefault();
		connect(row, SIGNAL(newRow(Row*)),
		        this, SLOT(onBind(Row*)));
	}

	place();
}

void TileScene::removeTile(Tile *tile)
{
	bool correct = tile->isCorrect();
	tile->defaultRow()->destroyTiles();
	if (correct)
		shiftCorrectCount(-1);
}

void TileScene::stripCorrect()
{
	if (!_correctCount)
		return;

	foreach (Tile *tile, *_cols.at(0)->tiles())
		if (tile->isShownCorrect())
			removeTile(tile);

	shiftCorrectCount(0);
}

void TileScene::layout()
{
	if (_correctCount == _curRowCount) {
		reset();
		return;
	}

	stripCorrect();

	foreach (Col *col, _cols)
		col->layout();

	place();
}

void TileScene::place()
{
	qreal tileWidth = 0;
	foreach (Col *group, _cols)
		tileWidth += group->width();

	qreal xstep = qMax(10.0, (width() - tileWidth) / (_colCount - 1));
	tileWidth = 0;

	foreach (Col *group, _cols) {
		group->place(tileWidth, 0);
		tileWidth += group->width() + xstep;
	}

	setSceneRect(0, 0, tileWidth - xstep, _cols.at(0)->height());
}

void TileScene::reset()
{
	_cols.at(0)->reset();
	place();
	shiftCorrectCount(0);
}

void TileScene::skip()
{
	foreach (Tile *tile, *_cols.at(0)->tiles())
		if (!tile->isShownCorrect())
			_bank.append(tile->defaultRow()->entry());
	advance();
}

void TileScene::onBind(Row *row)
{
	shiftCorrectCount(row ? 1 : -1);
	if (_placeMode == AutoCheck && row)
		row->showCorrect();
	else if (_correctCount == _curRowCount)
		reveal();
}

void TileScene::checkAdvance()
{
	if (_correctCount == _curRowCount)
		advance();
	else if (_placeMode != NoCheck)
		reveal();
}

void TileScene::addOne()
{
	if (!_bank.isEmpty()) {
		++_rowCount;
		add();
	}
}

void TileScene::removeOne()
{
	if (_curRowCount > 1) {
		Tile *tile = _cols.at(0)->randTile();
		if (!tile->isShownCorrect())
			_bank.append(tile->defaultRow()->entry());
		removeTile(tile);
		_rowCount = _curRowCount;
		place();
	}
}

void TileScene::reveal(bool show)
{
	foreach (Col *group, _cols)
		group->reveal(show);
	updateCounts();
}

void TileScene::setColCount(int count)
{
	if (count > _colCount)
		for (int i = _colCount; i != count; ++i) {
			Col *col = new Col(this, i, i ? Col::Shuffle : Col::Sort);
			connect(col, SIGNAL(layoutChanged()),
			        this, SLOT(layout()));
			_cols.append(col);
			emit addRemoveGroup(col);
		}
	else if (count < _colCount)
		for (int i = count; i != _colCount; ++i) {
			delete _cols.takeLast();
			emit addRemoveGroup(NULL);
		}

	_colCount = count;
}

void TileScene::setPlacement(PlacementMode mode)
{
	if (mode != _placeMode) {
		_placeMode = mode;
		if (mode == NoCheck && _correctCount != _curRowCount)
			reveal(false);
		else if (mode == AutoCheck)
			reveal();
		updateCounts();
	}
}
