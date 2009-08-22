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

#include "Juff.h"

namespace Juff {

QString commandIdToString(CommandID id) {
	if ( id < ID_LAST_ITEM && id > ID_SEPARATOR )
		return QString::number((int)id);
	else
		return "";
}

CommandID stringToCommandId(const QString& str) {
	int n = str.toInt();
	if ( n < ID_LAST_ITEM && n > ID_SEPARATOR )
		return (CommandID)n;
	else
		return ID_NONE;
}

} // namespace Juff
