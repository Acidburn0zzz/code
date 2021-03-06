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

add_definitions(-DIN_NEXUS_CPP_LIBRARY=1 ${NX_CPP})

#Make NeXus CPP Bindings Static Library

set (HEADERS NeXusFile.hpp NeXusException.hpp NeXusStream.hpp)
set (SOURCES NeXusFile.hpp NeXusFile.cpp NeXusException.hpp NeXusException.cpp NeXusStream.hpp NeXusStream.cpp)

set_property(SOURCE ${SOURCES} APPEND PROPERTY COMPILE_FLAGS ${NX_CFLAGS})

add_library (NeXus_CPP_Static_Library STATIC ${HEADERS} ${SOURCES})

set_target_properties(NeXus_CPP_Static_Library PROPERTIES OUTPUT_NAME NeXusCPPStatic)

target_link_libraries(NeXus_CPP_Static_Library NeXus_Static_Library 
					${READLINE_LINK} ${M_LINK} ${DL_LINK} ${PTHREAD_LINK} ${DF_LINK}
                      ${TERMCAP_LINK} ${HISTORY_LINK} ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES})

#Make NeXus CPP Bindings Shared Library

add_library (NeXus_CPP_Shared_Library SHARED ${HEADERS} ${SOURCES})

#Note - library version needs to be got from somewhere?
# may want to use DEBUG_OUTPUT_NAME and RELEASE_OUTPUT_NAME
# of define CMAKE_DEBUG_POSTFIX globally
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties(NeXus_CPP_Shared_Library PROPERTIES OUTPUT_NAME libNeXusCPP-0
                      VERSION 1.0 SOVERSION 4 )
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties(NeXus_CPP_Shared_Library PROPERTIES OUTPUT_NAME NeXusCPP
                      VERSION 1.0 SOVERSION 4)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

target_link_libraries(NeXus_CPP_Shared_Library NeXus_Shared_Library 
                       ${READLINE_LINK} ${M_LINK} ${DL_LINK} ${PTHREAD_LINK} ${DF_LINK}
                      ${TERMCAP_LINK} ${HISTORY_LINK} ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES})

install (TARGETS NeXus_CPP_Static_Library NeXus_CPP_Shared_Library
         RUNTIME DESTINATION bin COMPONENT Runtime
         LIBRARY DESTINATION lib COMPONENT Runtime
         ARCHIVE DESTINATION lib/nexus COMPONENT Development)

INSTALL (FILES ${HEADERS} DESTINATION ${NX_INCLUDE}/ COMPONENT Development)

#if(WIN32)
#install_pdb (NeXus_CPP_Shared_Library)
#endif()



