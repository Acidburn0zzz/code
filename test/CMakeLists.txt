## Process this file with cmake
#====================================================================
#  NeXus - Neutron & X-ray Common Data Format
#  
#  CMakeLists for building the NeXus library and applications.
#
#  Copyright (C) 2011 Stephen Rankin
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
#
#====================================================================

SET (NXEXAMPLE_FILES napif4_test.f  napif5_test.f  napif_test.f  napi_test.c  napi_test_cpp.cxx  NXtest.f90)

INSTALL (FILES ${NXEXAMPLE_FILES} DESTINATION ${NXEXAMPLEDIR} COMPONENT Examples)

add_executable(napi_test_hdf5 napi_test.c)
target_link_libraries(napi_test_hdf5 NeXus_Shared_Library)

add_test(napi_test_hdf5 ${EXECUTABLE_OUTPUT_PATH}/napi_test_hdf5)
#set_property(TEST napi_test_hdf5 PROPERTY PASS_REGULAR_EXPRESSION kkk)
