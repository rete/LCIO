
#include <exception> 
#include <iostream>
#include <cstdlib>
#include "IMPL/LCIOExceptionHandler.h"

namespace IMPL {

  void LCIOExceptionHandler::setupHandlers() {
<<<<<<< HEAD
    // thread safe functions ... but deprecated functions ...
    // std::set_unexpected( &LCIOExceptionHandler::handler ) ;
    // std::set_terminate( &LCIOExceptionHandler::handler ) ;
=======
    // thread safe functions ...
    std::set_unexpected( &LCIOExceptionHandler::handler ) ;
    std::set_terminate( &LCIOExceptionHandler::handler ) ;
>>>>>>> 72badf9a804de11c61dd5b9f33f1e22894d378f4
  }
  
  void LCIOExceptionHandler::handler() {
    try {
      throw ;
    }
    catch( std::exception& e) {      
      std::cout << " A runtime error has occured : " 
          << e.what() 
          << std::endl
          << " the program will have to be terminated - sorry." << std::endl ;
      exit(1) ;
    }
  }
}