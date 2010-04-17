/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef __JUFF_FIND_DLG_H__
#define __JUFF_FIND_DLG_H__

#include "Types.h"
#include "ui_FindDlg.h"

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QTextDocument>

class FindDlg : public QDialog {
Q_OBJECT
public:
	FindDlg(QWidget*, bool);
	virtual ~FindDlg();

	QString text() const;
	QString replaceTo() const;
	Juff::SearchParams params() const;
	bool isReplaceMode() const { return uiFind.replaceChk->isChecked(); }
	void setText(const QString&);

	static QString lastText() { return lastString_; }
	static QString lastReplaceText() { return lastReplaceText_; }
	static bool lastMatchCase() { return matchCase_; }
	static bool lastRegExpMode() { return regExpMode_; }
	static bool lastWholeWords() { return wholeWords_; }

public slots:
	void setReplaceMode(bool);

protected slots:
	void multiLineChecked(bool);
	void regExpChecked(bool);

protected:
	virtual void keyPressEvent(QKeyEvent*);

private:

	Ui::FindDlg uiFind;

	static QString lastString_;
	static QString lastReplaceText_;
	static bool matchCase_;
	static bool backward_;
	static bool regExpMode_;
	static bool wholeWords_;
	static bool multiLine_;
	static QStringList strings_;
	static QStringList replaces_;
};


#endif // __JUFF_FIND_DLG_H__