#################################
# cmake file for building SIO
# @author Frank Gaede, DESY
# @author Jan Engels, DESY
# @author Remi Ete, DESY
#################################

# project name
PROJECT( FIO )

# project version
SET( FIO_VERSION_MAJOR ${LCIO_VERSION_MAJOR} )
SET( FIO_VERSION_MINOR ${LCIO_VERSION_MINOR} )
SET( FIO_VERSION_PATCH ${LCIO_VERSION_PATCH} )

# library *nix style versioning
SET( FIO_SOVERSION "${FIO_VERSION_MAJOR}.${FIO_VERSION_MINOR}" )
SET( FIO_VERSION "${FIO_SOVERSION}.${FIO_VERSION_PATCH}" )

INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG( "-std=c++14" COMPILER_SUPPORTS_CXX14 )

IF( COMPILER_SUPPORTS_CXX14 )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )
ELSE()
  MESSAGE( SEND_ERROR "${PROJECT_NAME} requires C++14 support. Please upgrade your compiler !" )
ENDIF()

FIND_PACKAGE( ZLIB REQUIRED )
#ZLIB_INCLUDE_DIR - where to find zlib.h, etc.
#ZLIB_LIBRARIES   - List of libraries when using zlib.
#ZLIB_FOUND       - True if zlib found.

INCLUDE( TestBigEndian )
TEST_BIG_ENDIAN( BIG_ENDIAN )

IF( NOT BIG_ENDIAN )
    ADD_DEFINITIONS( "-D__LITTLE_ENDIAN__" )
ENDIF()

# TODO: should we add this back ?
#IF( LCIO_DCAP )
#    INCLUDE_DIRECTORIES( ${DCAP_INCLUDE_DIRS} )
#    ADD_DEFINITIONS( "-DSIO_USE_DCAP" )
#ENDIF()

INCLUDE_DIRECTORIES(
    ${FIO_SOURCE_DIR}/include
    ${ZLIB_INCLUDE_DIR}
)

AUX_SOURCE_DIRECTORY( src FIO_SRCS )

ADD_SHARED_LIBRARY( fio ${SIO_SRCS} )
TARGET_LINK_LIBRARIES( fio ${ZLIB_LIBRARIES} )
INSTALL_SHARED_LIBRARY( fio DESTINATION lib )

