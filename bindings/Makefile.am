## Process this file with automake to produce Makefile.in
#====================================================================
#  NeXus - Neutron & X-ray Common Data Format
#  
#  $Id$
#
#  Makefile for coordinating the generation of language bindings
#  
#  Copyright (C) 2004 Freddie Akeroyd
#  
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
# 
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free 
#  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
#  MA  02111-1307  USA
#             
#  For further information, see <http://www.nexusformat.org>
#
#====================================================================
if HAVE_CPP
CPPSUB = cpp
endif
if HAVE_F77
F77SUB = f77
endif
if HAVE_F90
F90SUB = f90
endif
if HAVE_JAVAC
JAVASUB = java
endif
if HAVE_SWIG
SWIGSUB = swig
endif
if HAVE_IDL
IDLSUB = idl
endif
if HAVE_PYTHON
PYSUB = python
endif
if HAVE_MATLAB
MATSUB = matlab
endif
SUBDIRS = $(CPPSUB) $(F77SUB) $(F90SUB) $(JAVASUB) $(SWIGSUB) $(IDLSUB) $(PYSUB) # $(MATSUB)
