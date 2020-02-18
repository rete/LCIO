#include "SIO/SIOSimCalCollectionHandler.h"

// -- lcio headers
#include "EVENT/LCIO.h"
#include "EVENT/MCParticle.h"
#include "EVENT/SimCalorimeterHit.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IOIMPL/SimCalorimeterHitIOImpl.h"
#include "IOIMPL/LCCollectionIOVec.h"
#include "SIO/SIOSimCalHitHandler.h"
#include "IMPL/LCFlagImpl.h"
#include "SIO/LCSIO.h"
#include "SIO/SIOObjectHandler.h"
#include "SIO/SIOLCParameters.h"

// -- sio headers
#include <sio/io_device.h>
#include <sio/version.h>

namespace SIO {

  SIOSimCalCollectionHandler::SIOSimCalCollectionHandler( const std::string& colname ) :
    sio::block( colname, LCSIO::blockVersion() ) {
    /* nop */
  }
  
  //----------------------------------------------------------------------------

  const std::string &SIOSimCalCollectionHandler::type() const {
    return _collectionType ;
  }
  
  //----------------------------------------------------------------------------

  void SIOSimCalCollectionHandler::setCollection( EVENT::LCCollection *col ) {
    _collection = col ;
  }
  
  //----------------------------------------------------------------------------

  void SIOSimCalCollectionHandler::read( sio::read_device &device, sio::version_type vers ) {
    if( vers > SIO_VERSION_ENCODE( 2, 13 ) ) {
      std::cout << "Reading column wise version" << std::endl ;
      IOIMPL::LCCollectionIOVec* ioCol = dynamic_cast<IOIMPL::LCCollectionIOVec*>( _collection ) ;
      if( nullptr == ioCol ) {
        SIO_THROW( sio::error_code::invalid_argument, "Expected collection of type LCCollectionIOVec!" ) ;
      }
      unsigned int flag {} ;
      SIO_SDATA( device ,  flag ) ;
      ioCol->setFlag( flag ) ;
      if( vers > SIO_VERSION_ENCODE( 1, 1 ) ) {
        SIOLCParameters::read( device, ioCol->parameters(), vers ) ;
      }
      // read out number of objects in the collection
      int nObj ;
      SIO_DATA( device ,  &nObj , 1  ) ;
      // reserve the space for the pointers to all objects
      ioCol->resize( nObj ) ;
      if( flag & ( 1 << EVENT::LCCollection::BITSubset ) ) {
        // read out only pointers
        for( int i=0 ; i< nObj ; i ++ ) {
          SIO_PNTR( device , &(*ioCol)[i]  ) ;
        }
      }
      else {
        // read out all objects in the collection
        std::vector<IOIMPL::SimCalorimeterHitIOImpl*> collectionVec {} ;
        collectionVec.reserve( nObj ) ;
        for( int i=0 ; i< nObj ; i ++ ) {
          auto obj = new IOIMPL::SimCalorimeterHitIOImpl() ;
          (*ioCol)[i] = obj ;
          collectionVec.push_back( obj ) ;
        }
        try {
          IMPL::LCFlagImpl lcflag(flag) ;
          for( auto &hit : collectionVec ) {
            SIO_DATA( device ,  &(hit->_cellID0) , 1  ) ;
          }
          if( lcflag.bitSet( EVENT::LCIO::CHBIT_ID1 ) || ( SIO_VERSION_MAJOR(vers)==0 && SIO_VERSION_MINOR(vers)==8) ) {
            for( auto &hit : collectionVec ) {
              SIO_DATA( device ,  &(hit->_cellID1) , 1  ) ;
            }
          }
          for( auto &hit : collectionVec ) {
            SIO_DATA( device ,  &(hit->_energy) , 1  ) ;
          }
          if( lcflag.bitSet( EVENT::LCIO::CHBIT_LONG ) ) {
            for( auto &hit : collectionVec ) {
              SIO_DATA( device ,  hit->_position , 3  ) ;
            }
          }
          for( auto &hit : collectionVec ) {
            int nCon ;
            SIO_DATA( device ,  &nCon , 1  ) ;
            hit->_vec.reserve(nCon) ;
            for(int i=0; i< nCon ; i++) {
              hit->_vec.push_back( new IMPL::MCParticleCont() ) ;
              SIO_PNTR( device , &(hit->_vec[i]->Particle)  ) ;
            }
            for(int i=0; i< nCon ; i++) {
              SIO_DATA( device , &(hit->_vec[i]->Energy) , 1 ) ;
            }
            for(int i=0; i< nCon ; i++) {
              SIO_DATA( device , &(hit->_vec[i]->Time) , 1 ) ;
            }
            if( lcflag.bitSet( EVENT::LCIO::CHBIT_STEP )) {
              for(int i=0; i< nCon ; i++) {
                SIO_DATA( device , &(hit->_vec[i]->Length) , 1 ) ;
              }
              for(int i=0; i< nCon ; i++) {
                SIO_DATA( device , &(hit->_vec[i]->PDG) , 1 ) ;
              }
              for(int i=0; i< nCon ; i++) {
                SIO_DATA( device , &(hit->_vec[i]->StepPosition[0]) , 3 ) ;
              }
            }
          }
          for( auto &hit : collectionVec ) {
            SIO_PTAG( device , dynamic_cast<const EVENT::SimCalorimeterHit*>(hit) ) ;
          }
        }
        catch( std::exception &e ) {
          std::stringstream ss ; ss << "Couldn't read out object of type '" << _collectionType ;
          SIO_RETHROW( e, sio::error_code::io_failure, ss.str() ) ;
        }
        catch( ... ) {
          std::stringstream ss ; ss << "Couldn't read out object of type '" << _collectionType ;
          SIO_THROW( sio::error_code::io_failure, ss.str() ) ;
        }
      }
    }
    else {
      SIO::SIOSimCalHitHandler handler {} ;
      IOIMPL::LCCollectionIOVec* ioCol = dynamic_cast<IOIMPL::LCCollectionIOVec*>( _collection ) ;
      if( nullptr == ioCol ) {
        SIO_THROW( sio::error_code::invalid_argument, "Expected collection of type LCCollectionIOVec!" ) ;
      }
      handler.initReading( device, _collection, vers ) ;
      // read out number of objects in the collection
      int nObj ;
      SIO_DATA( device ,  &nObj , 1  ) ;
      // reserve the space for the pointers to all objects
      ioCol->resize( nObj ) ;
      if( handler.flag() & ( 1 << EVENT::LCCollection::BITSubset ) ) {
        // read out only pointers
        for( int i=0 ; i< nObj ; i ++ ) {
          SIO_PNTR( device , &(*ioCol)[i]  ) ;
        }
      }
      else {
        // std::cout << "Reading a full collection" << std::endl ;
        // read out all objects in the collection
        for( int i=0 ; i< nObj ; i ++ ) {
          (*ioCol)[i] = handler.create() ;
          try {
            handler.read( device , (*ioCol)[i] , vers ) ;
          }
          catch( std::exception &e ) {
            std::stringstream ss ; ss << "Couldn't read out object of type '" << handler.collectionType() << "' at index " << i ;
            SIO_RETHROW( e, sio::error_code::io_failure, ss.str() ) ;
          }
          catch( ... ) {
            std::stringstream ss ; ss << "Couldn't read out object of type '" << handler.collectionType() << "' at index " << i ;
            SIO_THROW( sio::error_code::io_failure, ss.str() ) ;
          }
        }
      }
    }
  }
  
  //----------------------------------------------------------------------------
  
  void SIOSimCalCollectionHandler::write( sio::write_device &device ) {
    auto flag = _collection->getFlag() ;
    SIO_SDATA( device , flag ) ;
    SIOLCParameters::write( device , _collection->getParameters() ) ;
    // write number of objects
    int nObj = _collection->getNumberOfElements() ;
    SIO_DATA( device,  &nObj , 1  ) ;
    if( flag & ( 1 << EVENT::LCCollection::BITSubset ) ) {
      // write only pointers
      for( int i=0 ; i< nObj ; i ++ ) {
        auto ptr = _collection->getElementAt(i) ;
        SIO_PNTR( device , &ptr ) ;
      }
    }
    else {
      // write all the objects
      std::vector<const EVENT::SimCalorimeterHit*> collectionVec {} ;
      collectionVec.reserve( nObj ) ;
      IMPL::LCFlagImpl lcflag( flag ) ;
      for( unsigned int e=0 ; e<nObj ; e++ ) {
        collectionVec.push_back( static_cast<const EVENT::SimCalorimeterHit*>( _collection->getElementAt(e) ) ) ;
      }
      for( auto &hit : collectionVec ) {
        SIO_SDATA( device, hit->getCellID0()  ) ;
      }
      if( lcflag.bitSet( EVENT::LCIO::CHBIT_ID1 ) ){
        for( auto &hit : collectionVec ) {
          SIO_SDATA( device, hit->getCellID1()  ) ;
        }
      }
      for( auto &hit : collectionVec ) {
        SIO_SDATA( device, hit->getEnergy()  ) ;
      }
      if( lcflag.bitSet( EVENT::LCIO::CHBIT_LONG ) ){
        for( auto &hit : collectionVec ) {
          SIO_DATA( device,  hit->getPosition() , 3 ) ;
        }
      }
      for( auto &hit : collectionVec ) {
        int nMC = hit->getNMCContributions() ;
        SIO_SDATA( device, nMC ) ;
        for(int i=0; i<nMC ;i++) {
          auto ptr = hit->getParticleCont(i) ;
          SIO_PNTR( device , &ptr ) ; 
        }
        for(int i=0; i<nMC ;i++) { 
          SIO_SDATA( device, hit->getEnergyCont(i)  ) ; 
        }
        for(int i=0; i<nMC ;i++) { 
          SIO_SDATA( device, hit->getTimeCont(i)  ) ;
        }
        if( lcflag.bitSet( EVENT::LCIO::CHBIT_STEP ) ) {
          for(int i=0; i<nMC ;i++) {
            SIO_SDATA( device, hit->getLengthCont(i)  ) ;
          }
          for(int i=0; i<nMC ;i++) {
            SIO_SDATA( device, hit->getPDGCont(i)  ) ;
          }
          for(int i=0; i<nMC ;i++) {
            float* sp = const_cast<float*> ( hit->getStepPosition(i) ) ;
            SIO_DATA( device,  sp , 3 ) ;
          }
        }
      }
      for( auto &hit : collectionVec ) {
        SIO_PTAG( device , hit ) ;
      }
    }
  }

} // namespace
