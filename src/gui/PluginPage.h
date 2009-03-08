/*
JuffEd - A simple text editor
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

#ifndef _PLUGIN_PAGE_H_
#define _PLUGIN_PAGE_H_

class QCheckBox;

#include <QtGui/QWidget>

class PluginPage : public QWidget {
Q_OBJECT
public:
	PluginPage(const QString& pluginName, QWidget* page);
	virtual ~PluginPage();
	
	bool pageEnabled() const;
	QString name() const { return name_; }

public slots:
	void enablePage(bool);

private:
	QCheckBox* usePluginChk_;
	QWidget* page_;
	bool enabled_;
	QString name_;
};

#endif