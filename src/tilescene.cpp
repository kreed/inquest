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

#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPair>
#include <QTextStream>
#include "tilegroup.h"
#include "tile.h"
#include "tilescene.h"

TileScene::TileScene(QObject *parent)
	: QGraphicsScene(parent)
	, _groupSize(16)
	, _placeMode(AutoCheck)
	, _words(new TileGroup(this))
	, _meanings(new TileGroup(this))
{
	connect(qApp, SIGNAL(lastWindowClosed()),
	        this, SLOT(dumpState()));
	_words->_layout = TileGroup::Sort;
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

void TileScene::init()
{
	if (fill(stateFile(), false))
		advance();
}

void TileScene::fill()
{
	QStringList files = QFileDialog::getOpenFileNames(mainWindow, QString(), QString(), "Tab-separated values (*.tsv)");
	if (!files.isEmpty()) {
		foreach (QString file, files)
			if (!fill(file))
				return;

		advance();
	}
}

void TileScene::fillState()
{
	if (fill(stateFile()))
		advance();
}

bool TileScene::fill(const QString &file, bool showError)
{
	QFile store(file);
	if (!store.open(QFile::ReadOnly)) {
		if (showError)
			QMessageBox::warning(mainWindow, "Error reading file", store.errorString());
		return false;
	}

	_bank.clear();
	QTextStream in(&store);

	while (!in.atEnd()) {
		QStringList toks = in.readLine().split('\t');
		if (toks.count() == 2)
			_bank.append(StringPair(toks[0], toks[1]));
	}

	store.close();

	return true;
}

void TileScene::dump()
{
	QString file = QFileDialog::getSaveFileName(mainWindow, QString(), QString(), "Tab-separated values (*.tsv)");
	if (!file.isEmpty())
		dump();
}

void TileScene::dumpState()
{
	if (_bank.isEmpty() && _words->_tiles.isEmpty())
		QFile::remove(stateFile());
	else
		dump(stateFile());
}

void TileScene::dump(const QString &file)
{
	QFile store(file);
	if (!store.open(QFile::WriteOnly))
		QMessageBox::warning(mainWindow, "Error writing file", store.errorString());
	
	QTextStream out(&store);

	foreach(StringPair pair, _bank)
		out << pair.first << '\t' << pair.second << '\n';
	foreach(Tile *tile, _words->_tiles)
		if (!tile->isCorrect())
			out << tile->toPlainText() << '\t' << tile->pair()->toPlainText() << '\n';

	store.close();
}

void TileScene::updateCount()
{
	emit countChanged(_correctCount, _words->size() + _bank.size() - _correctCount);
}

void TileScene::advance()
{
	_correctCount = 0;
	_words->clear();
	_meanings->clear();
	add();
	updateCount();
}

Tile *TileScene::addTile(const QString &text, TileGroup *group)
{
	Tile *tile = group->addTile(text);
	connect(tile, SIGNAL(newPair(Tile*, Tile*)),
	        this, SLOT(onPair(Tile*, Tile*)));
	addItem(tile);
	return tile;
}

void TileScene::add()
{
	while (_words->size() != _groupSize && !_bank.isEmpty()) {
		StringPair pair = _bank.takeAt(rand() % _bank.size());
		addTile(pair.first, _words)->init(addTile(pair.second, _meanings),
		                                  _meanings->whereText(pair.second));
	}

	layout();
}

void TileScene::removeWord(Tile *word)
{
	if (word->isCounted())
		--_correctCount;
	_meanings->removeTile(word->pair());
	_words->removeTile(word);
}

void TileScene::layout()
{
	if (_correctCount == _words->size()) {
		_correctCount = 0;
		foreach (Tile *tile, _words->_tiles)
			tile->setPair(NULL);
		updateCount();
	} else if (_correctCount) {
		foreach (Tile *tile, _words->_tiles)
			if (tile->isCorrect())
				removeWord(tile);
		updateCount();
	}

	qreal edge = 0;
	foreach (Tile *tile, _words->_tiles)
		if (edge < tile->boundingRect().width())
			edge = tile->boundingRect().width();

	_words->layout(5);
	_meanings->layout((int)edge + 10);
}

void TileScene::onPair(Tile *tile, Tile *pair)
{
	_correctCount += pair ? 1 : -1;
	tile->setPair(pair);
	if (_placeMode == AutoCheck) {
		if (pair) {
			tile->showCorrect();
			pair->showCorrect();
		}
		updateCount();
	} else if (_correctCount == _words->size())
		reveal();
}

void TileScene::checkAdvance()
{
	if (_correctCount == _words->size())
		advance();
	else if (_placeMode != NoCheck)
		reveal();
}

void TileScene::addOne()
{
	if (!_bank.isEmpty()) {
		++_groupSize;
		add();
	}
}

void TileScene::removeOne()
{
	if (_words->size() > 1) {
		Tile *word = _words->_tiles.at(rand() % _words->size());
		if (!word->isCorrect())
			_bank.append(StringPair(word->toPlainText(), word->pair()->toPlainText()));
		removeWord(word);
		_groupSize = _words->size();
		layout();
		updateCount();
	}
}

void TileScene::reveal()
{
	_words->reveal();
	_meanings->reveal();
	updateCount();
}

void TileScene::setPlacement(PlacementMode mode)
{
	if (mode != _placeMode) {
		if (mode == NoCheck && _correctCount != _words->size()) {
			_words->reveal(false);
			_meanings->reveal(false);
			emit countChanged(0, _words->size() + _bank.size());
		} else if (mode == AutoCheck)
			reveal();
		_placeMode = mode;
	}
}

void TileScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)
{
	checkAdvance();
}
