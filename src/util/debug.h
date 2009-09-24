/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see file COPYING); if not, see 
 *      http://www.gnu.org/licenses 
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <QString>

#ifndef WIN32
	#define XDEBUG
#endif


extern int allowDebugLog;


#ifdef XDEBUG
void dlogS (const QString &s);
void dlogf (const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void dlogfVA (const char *fmt, va_list ap);
#else
# define dlogS(...)
# define dlogf(...)
# define dlogfVA(...)
#endif


#endif
