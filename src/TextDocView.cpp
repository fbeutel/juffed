/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "TextDocView.h"

//	Qt headers
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QTextBlock>
#include <QtGui/QTextEdit>

#include <Qsci/qscilexer.h>
#include <Qsci/qsciscintilla.h>

//	local headers
#include "LexerStorage.h"
#include "Log.h"
#include "TextDoc.h"
#include "TextDocSettings.h"
#include "types.h"

#include <math.h>

class MyQScintilla : public QsciScintilla {
public:
	MyQScintilla(QWidget* parent) : QsciScintilla(parent) {
		contextMenu_ = new QMenu();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_CUT));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_COPY));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_PASTE));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_UNDO));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_EDIT_REDO));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND_NEXT));
		contextMenu_->addAction(CommandStorage::instance()->action(ID_FIND_PREV));
		contextMenu_->addSeparator();
		contextMenu_->addAction(CommandStorage::instance()->action(ID_GOTO_LINE));
	}
	virtual ~MyQScintilla() {
		delete contextMenu_;
	}
	
protected:
	virtual void dragEnterEvent(QDragEnterEvent* e) {
		if (!e->mimeData()->hasUrls())
			QsciScintilla::dragEnterEvent(e);
	}

	virtual void dropEvent(QDropEvent* e) {
		if (!e->mimeData()->hasUrls())
			QsciScintilla::dropEvent(e);
	}

	virtual void contextMenuEvent(QContextMenuEvent* e) {
		contextMenu_->exec(e->globalPos());
	}

private:
	QMenu* contextMenu_;
};

QTextDocument::FindFlags textDocFlags(DocFindFlags f) {
	QTextDocument::FindFlags flags(0);
	if (f.matchCase)
		flags |= QTextDocument::FindCaseSensitively;
	if (f.backward)
		flags |= QTextDocument::FindBackward;
	return flags;
}

class TDViewInterior {
public:
	TDViewInterior(QWidget* parent) {
		lineNumVisible_ = true;
		adjustedByWidth_ = false;
		
		edit_ = new MyQScintilla(parent);
		edit_->setUtf8(true);
		edit_->setCaretLineVisible(true);
		edit_->setIndentationGuides(true);
		edit_->setCaretLineBackgroundColor(QColor(230, 230, 250));
		edit_->setIndentationGuidesForegroundColor(QColor(200, 200, 200));
		edit_->setFolding(QsciScintilla::BoxedTreeFoldStyle);
		edit_->setAutoIndent(true);
		edit_->setBackspaceUnindents(true);
		
		edit_->setMarginLineNumbers(1, true);

		edit_->setMarginWidth(2, 12);
		edit_->setMarginMarkerMask(1, 7);
		edit_->markerDefine(QsciScintilla::RightTriangle, 1);
		edit_->markerDefine(QsciScintilla::Background, 2);
		edit_->setMarkerForegroundColor(QColor(100, 100, 100));
		edit_->setMarkerBackgroundColor(QColor(100, 200, 100));
		
		edit_->setBackspaceUnindents(true);
		edit_->setBraceMatching(QsciScintilla::SloppyBraceMatch);
		edit_->setMatchedBraceBackgroundColor(QColor(255, 255, 120));

		parent->setFocusProxy(edit_);
		parent->connect(edit_, SIGNAL(cursorPositionChanged(int, int)), parent, SIGNAL(cursorPositionChanged(int, int)));
	}
	~TDViewInterior() {
		delete edit_;
	}
	
	QsciScintilla* edit_;
	
	bool lineNumVisible_;
	bool adjustedByWidth_;
	IntList markers_;
};



TextDocView::TextDocView(QWidget* parent) : DocView(parent) {
	vInt_ = new TDViewInterior(this);

	connect(vInt_->edit_, SIGNAL(textChanged()), SLOT(updateLineNums()));
	
	updateLineNums();
}

TextDocView::~TextDocView() {
	delete vInt_;
}

void TextDocView::setDocument(Document* doc) {
	DocView::setDocument(doc);
	
	TextDoc* tDoc = qobject_cast<TextDoc*>(doc);
	if (tDoc == 0) {
		Log::debug("Wrong document");
		Log::print("Wrong document");
		return;
	}
	
	connect(vInt_->edit_, SIGNAL(modificationChanged(bool)), doc, SLOT(setModified(bool)));
	connect(vInt_->edit_, SIGNAL(modificationChanged(bool)), this, SIGNAL(modified(bool)));

	QsciLexer* lexer = LexerStorage::instance()->getLexerByFileName(doc->fileName(), TextDocSettings::font());
	vInt_->edit_->setLexer(lexer);
	vInt_->edit_->recolor();
	if (lexer != 0) {
		lexer->refreshProperties();
	}
}

void TextDocView::rehighlight() {
}

void TextDocView::setModified(bool mod) {
	vInt_->edit_->setModified(mod);
}

void TextDocView::resizeEvent(QResizeEvent*) {
	int h = height();
	int w = width();

	vInt_->edit_->setGeometry(0, 0, w, h);
}

void TextDocView::updateLineNums() {
	QString str = QString("   %1").arg(lineCount());
	if (vInt_->lineNumVisible_)
		vInt_->edit_->setMarginWidth(1, str);
	else
		vInt_->edit_->setMarginWidth(1, 0);
}

void TextDocView::setText(const QString& text) {
	vInt_->edit_->setText(text);
	updateLineNums();
}

void TextDocView::getText(QString& text) const {
	text = vInt_->edit_->text();
}

bool TextDocView::lineNumIsVisible() const {
	return vInt_->lineNumVisible_;
}

void TextDocView::setLineNumVisible(bool visible) {
	vInt_->lineNumVisible_ = visible;
	updateLineNums();
}

bool TextDocView::isAdjustedByWidth() const {
	return vInt_->adjustedByWidth_;
}

void TextDocView::setAdjustedByWidth(bool adjust) {
	vInt_->adjustedByWidth_ = adjust;
	if (adjust)
		vInt_->edit_->setWrapMode(QsciScintilla::WrapWord);
	else
		vInt_->edit_->setWrapMode(QsciScintilla::WrapNone);
}

void TextDocView::getCursorPos(int& row, int& col) const {
	vInt_->edit_->getCursorPosition(&row, &col);
}

void TextDocView::gotoLine(int line) const {
	vInt_->edit_->setCursorPosition(line - 1, 0);
}

int TextDocView::lineCount() const {
	return vInt_->edit_->lines();
}

void TextDocView::applySettings() {
	QFont font = TextDocSettings::font();
	vInt_->edit_->setFont(font);

	QsciLexer* lexer = vInt_->edit_->lexer();
	if (lexer != 0) {
		lexer->setFont(font, -1);
	}

	vInt_->edit_->setTabWidth(TextDocSettings::tabStopWidth());

	int lInd = TextDocSettings::lineLengthIndicator();
	if (lInd > 0) {
		vInt_->edit_->setEdgeMode(QsciScintilla::EdgeLine);
		vInt_->edit_->setEdgeColumn(lInd);	
	}
	else {
		vInt_->edit_->setEdgeMode(QsciScintilla::EdgeNone);
	}
}



////////////////////////////////////////////////////////////
//	EDIT
void TextDocView::cut() {
	vInt_->edit_->cut();
}

void TextDocView::copy() {
	vInt_->edit_->copy();
}

void TextDocView::paste() {
	vInt_->edit_->paste();
}

void TextDocView::undo() {
	vInt_->edit_->undo();
}

void TextDocView::redo() {
	vInt_->edit_->redo();
}
//	EDIT
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//	FIND
void prepareForFind(QsciScintilla* edit, const QString& str, bool back) {
	if (back) {
		if (edit->hasSelectedText()) {
			int fromRow, fromCol, toRow, toCol;
			edit->getSelection(&fromRow, &fromCol, &toRow, &toCol);
			if (fromRow == toRow) {
				QString selection = edit->selectedText();
				if (selection.compare(str) == 0)
					edit->setCursorPosition(fromRow, fromCol);
			}
		}
	}
}

bool TextDocView::continueOverTheEnd(bool back) {
	QString msg;
	int row(0), col(0);
	if (back) {
		msg = tr("The search has reached the beginning of file.\nContinue from the end?");
		row = lineCount() - 1;
		col = vInt_->edit_->text(row).length();
	}
	else {
		msg = tr("The search has reached the end of file.\nContinue from the beginning?");
		row = 0;
		col = 0;
	}
	QMessageBox::StandardButton choice = QMessageBox::question(this, tr("Find"),
			msg, QMessageBox::Ok | QMessageBox::Cancel);
	
	if (choice == QMessageBox::Ok) {
		vInt_->edit_->setCursorPosition(row, col);
		return true;
	}
	else {
		return false;
	}
}

void TextDocView::find(const QString& str, bool isRegExp, DocFindFlags flags) {
	prepareForFind(vInt_->edit_, str, flags.backward);
	
	bool found = vInt_->edit_->findFirst(str, isRegExp, flags.matchCase, false, false, !flags.backward);
	if (!found) {
		//	not found
		if (continueOverTheEnd(flags.backward))
			find(str, isRegExp, flags);
	}
}

enum Answer { Yes, No, All, Cancel };

Answer confirm(QWidget* w) {
	Answer answer = No;
	QMessageBox::StandardButton btn = QMessageBox::question(w, QObject::tr("Confirmation"), 
					QObject::tr("Replace this text?"), 
					QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::Cancel, 
					QMessageBox::Yes);
		switch (btn) {
		case QMessageBox::Yes:
			answer = Yes;
				break;
		case QMessageBox::No:
			answer = No;
			break;
		case QMessageBox::Cancel:
			answer = Cancel;
			break;
		case QMessageBox::YesToAll:
			answer = All;
			break;
		default: ;
	}

	return answer;
}

bool TextDocView::doReplace(const QString& text, bool& replaceAll) {
	if (!replaceAll) {
		//	ask confirmation if replace all hasn't been chosen yet
		Answer conf = confirm(this);
		if (conf == Cancel)
			return false;
		else if (conf == Yes)
			vInt_->edit_->replace(text);
		else if (conf == All) {
			replaceAll = true;
			vInt_->edit_->replace(text);
		}
	}
	else {
		//	just replace, because there has been chosen "select all"
		vInt_->edit_->replace(text);
	}
	return true;
}

void TextDocView::replace(const QString& from, bool isRegExp, const QString& to, DocFindFlags flags) {
	prepareForFind(vInt_->edit_, from, flags.backward);
	
	bool cancelled = false;
	bool replaceAll = false;
	while (vInt_->edit_->findFirst(from, isRegExp, flags.matchCase, false, false, !flags.backward)) {
		if (!doReplace(to, replaceAll)) {
			cancelled = true;
			break;
		}
	}
	if (!cancelled) {
		//	reached the end or the beginning
		if (continueOverTheEnd(flags.backward))
			replace(from, isRegExp, to, flags);
	}
}
//	FIND & REPLACE
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//	MARKERS
void TextDocView::toggleMarker() {
	int line(-1), col(-1);
	vInt_->edit_->getCursorPosition(&line, &col);
	line++;
	if (vInt_->markers_.contains(line)) {
		vInt_->markers_.removeAll(line);
		vInt_->edit_->markerDelete(line - 1, 1);
		vInt_->edit_->markerDelete(line - 1, 2);
	}
	else {
		vInt_->markers_.push_back(line);
		qSort(vInt_->markers_.begin(), vInt_->markers_.end());
		vInt_->edit_->markerAdd(line - 1, 1);
		vInt_->edit_->markerAdd(line - 1, 2);
	}
}

void TextDocView::gotoNextMarker() {
	if (vInt_->markers_.count() == 0)
		return;

	int line(-1), col(-1);
	vInt_->edit_->getCursorPosition(&line, &col);
	line++;
	foreach (int marker, vInt_->markers_) {
		if (marker > line) {
			//	As soon as markers are sorted,
			//	the condition "marker" > "line" will be 
			//	true for the first marker after the 
			//	current line
			gotoLine(marker);
			return;
		}
	}
	
	//	next marker is not found, go to the first one
	gotoLine( vInt_->markers_.first() );
}

void TextDocView::gotoPrevMarker() {
	if (vInt_->markers_.count() == 0)
		return;

	int line(-1), col(-1);
	vInt_->edit_->getCursorPosition(&line, &col);
	line++;
	int targetLine = -1;
	foreach (int marker, vInt_->markers_) {
		if (marker < line) {
			targetLine = marker;
		}
		else {
			//	As soon as markers are sorted,
			//	if "marker" becomes > than "line"
			//	then the previous marker was the closest
			//	one that precedes current line

			//	If there was no preceding markers,
			//	we should go to the last one

			if (targetLine >= 0) {
				gotoLine(targetLine);
			}
			else {
				gotoLine( vInt_->markers_.last() );
			}
			return;
		}
	}
}

void TextDocView::removeAllMarkers() {
	vInt_->markers_.clear();
	vInt_->edit_->markerDeleteAll();
}

IntList TextDocView::markers() const {
	return vInt_->markers_;
}

QString TextDocView::markedLine(int line) const {
	return vInt_->edit_->text(line - 1);
}
//	MARKERS
////////////////////////////////////////////////////////////
