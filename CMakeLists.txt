
cmake_minimum_required(VERSION 2.8)
 
PROJECT(EventQtSlotConnect)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}) 

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.cxx)

QT4_WRAP_UI(UISrcs ${UI_FILES})
QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
add_executable(EventQtSlotConnect MACOSX_BUNDLE ${CXX_FILES} ${UISrcs} ${MOCSrcs})
 
if(VTK_LIBRARIES)
  if(${VTK_VERSION} VERSION_LESS "6")
    target_link_libraries(EventQtSlotConnect QVTK ${VTK_LIBRARIES})
  else()
    target_link_libraries(EventQtSlotConnect ${VTK_LIBRARIES})
  endif()
else()
  target_link_libraries(EventQtSlotConnect vtkHybrid QVTK vtkViews ${QT_LIBRARIES})
endif()

if( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(EventQtSlotConnect ITKReview ${ITK_LIBRARIES})
else( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(EventQtSlotConnect ${ITK_LIBRARIES})
endif( "${ITK_VERSION_MAJOR}" LESS 4 )

