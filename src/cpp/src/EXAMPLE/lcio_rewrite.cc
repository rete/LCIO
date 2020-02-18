#include "lcio.h"

#include "IO/LCWriter.h"
#include "IO/LCReader.h"
#include "IO/LCEventListener.h"
#include "IO/LCRunListener.h"

using namespace lcio ;

class RunEventProcessor : public LCRunListener, public LCEventListener {
  
protected:
  LCWriter* lcWrt {nullptr} ;
  
public:
  
  RunEventProcessor(const RunEventProcessor&) = delete ;
  RunEventProcessor operator=(const RunEventProcessor&) = delete ;

  RunEventProcessor( const std::string &fname ) {
    
    // open outputfile
    lcWrt = LCFactory::getInstance()->createLCWriter() ;

    try { 
      lcWrt->setCompressionLevel(7);
      lcWrt->open( fname , LCIO::WRITE_NEW ) ; 
    } 
    
    catch(IOException& e){
      std::cout << "[RunEventProcessor()] Can't open file for writing -  " << e.what()  << std::endl ;
      throw e ; 
    }
    
  }
  
  ~RunEventProcessor(){
    lcWrt->close()  ;
    delete lcWrt ;
  }
  
  void processEvent( LCEvent* evt ) { 
    lcWrt->writeEvent( evt ) ;
  }

  void modifyEvent( LCEvent * /*evt*/ ) {

  }

  void processRunHeader( LCRunHeader* run ) {
    lcWrt->writeRunHeader( run ) ;
  }

  void modifyRunHeader(LCRunHeader*  /*run*/) {
    
  }
} ;

//=============================================================================

int main(int argc, char** argv ){
  
  // create reader and writer for input and output streams 
  LCReader* lcReader = LCFactory::getInstance()->createLCReader() ;
    
  if ( argc != 3 ) {
    std::cout << "Usage lcio_rewrite <infile> <outfile>" << std::endl ;
  }
  // read file name from command line 
  std::string infname = argv[1] ;
  std::string outfname = argv[2] ;

  lcReader->open( infname ) ;

  {
    RunEventProcessor evtProc ( outfname ) ;
      
    lcReader->registerLCRunListener( &evtProc ) ; 
    lcReader->registerLCEventListener( &evtProc ) ; 
      
    lcReader->readStream() ;    
  } 
    
  lcReader->close() ;
  delete lcReader ;
  return 0 ;
}

//=============================================================================

