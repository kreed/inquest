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

#include "tile.h"

Tile::Tile(const QString &text, bool movable, bool visible)
	: QGraphicsTextItem(text)
	, _duplicatePairs(NULL)
	, _currentPair(NULL)
	, _moved(false)
	, _correctShown(false)
{
	setMovable(movable);
	setVisible(visible);
}

Tile::~Tile()
{
	if (_duplicatePairs) {
		_duplicatePairs->removeOne(_pair);
		if (_duplicatePairs->isEmpty())
			delete _duplicatePairs;
	}
}

void Tile::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
	_moved = true;
	Tile *pair = testPair();
	if (pair != _currentPair)
		emit newPair(this, pair);
	QGraphicsItem::mouseReleaseEvent(ev);
}

Tile *Tile::testPair() const
{
	if (_moved) {
		if (_duplicatePairs) {
			foreach (Tile *dup, *_duplicatePairs)
				if (dup->y() - 15 < y() && dup->y() + 25 > y())
					return dup;
		} else {
			if (_pair->y() - 15 < y() && _pair->y() + 25 > y())
				return _pair;
		}
	}
	return NULL;
}

void Tile::setPair(Tile *other)
{
	if (other == _currentPair)
		return;

	Tile *oldPair = _currentPair;
	_currentPair = other;

	if (oldPair && oldPair->_currentPair == this)
		oldPair->setPair(NULL);

	if (other && other->_currentPair != this)
		other->setPair(this);

	if (!other && _correctShown)
		showCorrect(false);
}

void Tile::showCorrect(bool shown)
{
	if (shown != _correctShown) {
		setDefaultTextColor(shown ? QColor(0, 255, 0, 85) : Qt::black);
		setFlag(QGraphicsItem::ItemIsMovable, !shown);
		_correctShown = shown;
		update();
	}
}

void Tile::init(Tile *other, Tile *dupDef)
{
	_pair = other;
	other->_pair = this;

	if (dupDef) {
		Tile *dupWord = dupDef->_pair;

		if (dupDef->_duplicatePairs) {
			_duplicatePairs = dupWord->_duplicatePairs;
			other->_duplicatePairs = dupDef->_duplicatePairs;
		} else {
			_duplicatePairs = dupWord->_duplicatePairs = new QList<Tile*>;
			_duplicatePairs->append(dupDef);
			other->_duplicatePairs = dupDef->_duplicatePairs = new QList<Tile*>;
			other->_duplicatePairs->append(dupWord);
		}

		_duplicatePairs->append(other);
		other->_duplicatePairs->append(this);
	}
}

bool Tile::lessThan(Tile *a, Tile *b)
{
	return a->toPlainText() < b->toPlainText();
}

void Tile::setMovable(bool movable)
{
	setFlag(QGraphicsItem::ItemIsMovable, movable && !_correctShown);
	_movable = movable;
}
