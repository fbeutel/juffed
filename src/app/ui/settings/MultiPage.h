/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#ifndef __JUFF_MULTIPAGE_H__
#define __JUFF_MULTIPAGE_H__

#include <QtGui/QWidget>

class MultiPageInterior;
class SettingsPage;
class QString;
class QTreeWidgetItem;

class MultiPage : public QWidget {
Q_OBJECT
public:
	MultiPage(QWidget* = 0);
	virtual ~MultiPage();

	SettingsPage* addPage(const QString& pageTitle, SettingsPage*);
	void addChildPage(const QString& parentTitle, const QString& pageTitle, SettingsPage*);
	int pageCount() const;
	SettingsPage* currentPage() const;
	SettingsPage* page(const QString&) const;
	int currentIndex() const;
	void selectPage(int);
	QStringList getChildrenTitles(const QString&);

protected slots:
	void changeCurrentItem(QTreeWidgetItem*, QTreeWidgetItem* = 0);

private:
	MultiPageInterior* mpInt_;
};

#endif // __JUFF_MULTIPAGE_H__
