// Copyright (C) 2004 Team Python
// Copyright (C) 2006 Team Evolution
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef WOWPYTHONSERVER_ERRORS_H
#define WOWPYTHONSERVER_ERRORS_H

#include <assert.h>
#include <stdio.h>

// TODO: handle errors better

// An assert isn't necessarily fatal, although if compiled with asserts enabled it will be.
#define WPAssert(assertion) { if (!(assertion)) { fprintf (stderr, "\n%s:%i ASSERTION FAILED:\n  %s\n", __FILE__, __LINE__, #assertion); assert (#assertion &&0); } }
#define WPError(assertion, errmsg) if (! (assertion)) { LOG.outError ("%s:%i ERROR:\n  %s\n", __FILE__, __LINE__, (char *)errmsg); assert (false); }
#define WPWarning(assertion, errmsg) if (! (assertion)) { LOG.outError ("%s:%i WARNING:\n  %s\n", __FILE__, __LINE__, (char *)errmsg); }

// This should always halt everything.  If you ever find yourself wanting to remove the assert (false), switch to WPWarning or WPError
#define WPFatal(assertion, errmsg) if (! (assertion)) { LOG.outError ("%s:%i FATAL ERROR:\n  %s\n", __FILE__, __LINE__, (char *)errmsg); assert (#assertion &&0); THREADS.closeCurrentThread (); }

#endif

