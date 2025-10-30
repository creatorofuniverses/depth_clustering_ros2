## Copyright (C) 2020  I. Bogoslavskyi, C. Stachniss
##
## GNU-GPL licence that follows one of libQGLViewer.

# Qt-dependent visualizer disabled for headless builds
# add_library(visualization SHARED visualizer.cpp)
# target_link_libraries(visualization
#                       identifiable
#                       ${QGLVIEWER_LIBRARY}
#                       ${MY_QT_LIBRARIES}
#                       ${OPENGL_gl_LIBRARY}
#                       ${OPENGL_glu_LIBRARY})

# cloud_saver doesn't require Qt, build it for headless ROS2 nodes
if(${PCL_FOUND})
  add_library(cloud_saver SHARED cloud_saver.cpp)
  target_link_libraries(cloud_saver 
                        ${PCL_LIBRARIES} 
                        ${OpenCV_LIBS}
                        ${Boost_LIBRARIES})
  if(ament_cmake_FOUND)
    install(TARGETS cloud_saver
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)
  endif()
endif()
