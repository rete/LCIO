#ifndef SIO_SIOSIMCALCOLLECTIONHANDLER_H
#define SIO_SIOSIMCALCOLLECTIONHANDLER_H 1

// -- std headers
#include <string>
#include <memory>

// -- lcio headers
#include "EVENT/LCCollection.h"
#include "EVENT/LCIO.h"

// -- sio headers
#include <sio/block.h>

namespace SIO {

  class SIOObjectHandler ;

  /** Handler for LCCollection/LCCollectionIOVec objects for SIO.
   *
   * @author gaede
   * @version $Id: SIOCollectionHandler.h,v 1.8 2005-04-15 08:37:42 gaede Exp $
   */
  class SIOSimCalCollectionHandler : public sio::block {
  public:
    SIOSimCalCollectionHandler() = delete ;
    SIOSimCalCollectionHandler(const SIOSimCalCollectionHandler&) = delete;
    SIOSimCalCollectionHandler& operator=(const SIOSimCalCollectionHandler&) = delete ;
    ~SIOSimCalCollectionHandler() = default ;

  public:
    /// Constructor with collection (block) name
    SIOSimCalCollectionHandler( const std::string& name ) ;

    /// Get the collection type
    const std::string &type() const ;

    /// Set the collection to read/write
    void setCollection( EVENT::LCCollection *col ) ;

    // from sio::block
    void read( sio::read_device &device, sio::version_type vers ) override ;
    void write( sio::write_device &device ) override ;

  private:
    /// The collection to read/write
    EVENT::LCCollection                *_collection {nullptr} ;
    const std::string                   _collectionType{EVENT::LCIO::SIMCALORIMETERHIT} ;
  };

} // namespace
#endif /* ifndef SIO_SIOSIMCALCOLLECTIONHANDLER_H */
