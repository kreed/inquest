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

#include "tilescene.h"

#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPair>
#include <QTextStream>
#include "tile.h"

TileScene::TileScene(QObject *parent)
	: QGraphicsScene(parent)
	, _groupSize(16)
	, _showCorrect(true)
	, _autoCheck(true)
	, _wordsLocked(false)
	, _meaningsLocked(false)
	, _wordMode(Sort)
	, _meaningMode(Shuffle)
{
	connect(qApp, SIGNAL(lastWindowClosed()),
	        this, SLOT(dumpState()));
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
	if (_bank.isEmpty() && _words.isEmpty())
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
	foreach(Tile *tile, _words)
		if (!tile->isCorrect())
			out << tile->toPlainText() << '\t' << tile->pair()->toPlainText() << '\n';

	store.close();
}

void TileScene::updateCount()
{
	emit countChanged(_correctCount, _words.size() + _bank.size() - _correctCount);
}

void TileScene::advance()
{
	_correctCount = 0;
	foreach (Tile *tile, _words)
		delete tile;
	foreach (Tile *tile, _meanings)
		delete tile;
	_words.clear();
	_meanings.clear();
	add();
	updateCount();
}

static Tile *matcher(const QList<Tile*> &list, const QString &meaning)
{
	foreach (Tile *tile, list)
		if (tile->toPlainText() == meaning)
			return tile;
	return NULL;
}

Tile *TileScene::addTile(const QString &text, QList<Tile*> &list, bool movable)
{
	Tile *tile = new Tile(text, movable);
	list.append(tile);
	connect(tile, SIGNAL(newPair(Tile*, Tile*)),
	        this, SLOT(onPair(Tile*, Tile*)));
	addItem(tile);
	return tile;
}

void TileScene::add()
{
	while (_words.size() != _groupSize && !_bank.isEmpty()) {
		StringPair pair = _bank.takeAt(rand() % _bank.size());
		addTile(pair.first, _words, !_wordsLocked)->pair(addTile(pair.second, _meanings, !_meaningsLocked),
		                                  matcher(_meanings, pair.second));
	}

	layout();
}

void TileScene::layout(QList<Tile*> &tiles, LayoutMode mode, int margin)
{
	QPointF pos(margin, 5);

	if (mode == Shuffle) {
		QList<Tile*> *copy = new QList<Tile*>(tiles);

		while (!copy->isEmpty()) {
			copy->takeAt(rand() % copy->size())->setPos(pos);
			pos.ry() += 40;
		}

		delete copy;
	} else {
		qSort(tiles.begin(), tiles.end(), Tile::lessThan);

		foreach (Tile *tile, tiles) {
			tile->setPos(pos);
			pos.ry() += 40;
		}
	}
}

void TileScene::removeWord(Tile *word)
{
	if (word->isCorrect())
		--_correctCount;
	word->setPair(NULL);
	word->pair()->setPair(NULL);
	_words.removeOne(word);
	_meanings.removeOne(word->pair());
	delete word->pair();
	delete word;
}

void TileScene::layout()
{
	if (_correctCount && _correctCount != _words.size()) {
		foreach (Tile *tile, _words)
			if (tile->isCorrect())
				removeWord(tile);
		updateCount();
	}

	qreal edge = 0;
	foreach (Tile *tile, _words)
		if (edge < tile->boundingRect().width())
			edge = tile->boundingRect().width();
	
	layout(_words, _wordMode, 5);
	layout(_meanings, _meaningMode, (int)edge + 10);
}

void TileScene::onPair(Tile *tile, Tile *pair)
{
	_correctCount += pair ? 1 : -1;
	if (_showCorrect && _autoCheck) {
		tile->setPair(pair);
		updateCount();
	} else if (_correctCount == _words.size())
		check();
}

void TileScene::checkAdvance()
{
	if (_correctCount == _words.size())
		advance();
	else if (_showCorrect)
		check();
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
	if (_words.size() > 1) {
		Tile *word = _words.takeAt(rand() % _words.size());
		if (!word->isCorrect())
			_bank.append(StringPair(word->toPlainText(), word->pair()->toPlainText()));
		removeWord(word);
		_groupSize = _words.size();
		layout();
		updateCount();
	}
}

void TileScene::check()
{
	foreach (Tile *tile, _words)
		tile->setPair(tile->testPair());
	foreach (Tile *tile, _meanings)
		tile->setPair(tile->testPair());
	updateCount();
}

void TileScene::toggleShowCorrect()
{
	if (_showCorrect && _correctCount != _words.size()) {
		foreach (Tile *tile, _words)
			tile->setPair(NULL);
		emit countChanged(0, _words.size() + _bank.size());
	}
	_showCorrect = !_showCorrect;
}

void TileScene::toggleAutoCheck()
{
	_autoCheck = !_autoCheck;
}

void TileScene::setWordsSorted()
{
	_wordMode = Sort;
	layout();
}

void TileScene::setWordsShuffled()
{
	_wordMode = Shuffle;
	layout();
}

void TileScene::setMeaningsSorted()
{
	_meaningMode = Sort;
	layout();
}

void TileScene::setMeaningsShuffled()
{
	_meaningMode = Shuffle;
	layout();
}

static void lock(const QList<Tile*> &list, bool state)
{
	foreach (Tile *tile, list)
		tile->setFlag(QGraphicsItem::ItemIsMovable, state);
}

void TileScene::toggleWordsLocked()
{
	lock(_words, !(_wordsLocked = !_wordsLocked));
}

void TileScene::toggleMeaningsLocked()
{
	lock(_meanings, !(_meaningsLocked = !_meaningsLocked));
}
