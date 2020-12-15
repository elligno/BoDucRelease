
#pragma once

#include <tuple>

#include <QString>

namespace bdAPI 
{
  /** Hold all data for the creation of Bon livraison. */

  // in BoDucReportCreator creator in prototype version		
  // shall be in the BoDucBonLivraisonMgr
  // key is the unit no and all values for report creation
 //  QMultiMap<QString, tplbonlivraison> m_unitBonLivraisonData;
  class BoDucBonLivraison 
  {
    // not sure if we will keep it
    using tplbonlivraison = std::tuple<QString/*address*/, QString/*product*/, double/*qty*/, short/*silo*/>;
  public:
    /** */
    BoDucBonLivraison( unsigned aUnitNo) :m_unitNo(aUnitNo) {}

    // what about copy/assignment ctor? default will do?
    unsigned unitNo() const { return m_unitNo; }
    
    // add a structure that hold data? 
    // addCmd(tplbonlivraison) with comand number??
    void addCmd() { throw "Not implemented yet"; }

    // remove by command number such as "CO123456"
    bool removeCmd() { throw "Not implemented yet"; }

    // look for a command by number such as "CO123456"
    bool containsCmd() { throw "Not implemented yet"; }

    // number of command
    unsigned nbOfCmd() const { return m_nbCmd; }

    // return information about 
  private:
    unsigned m_unitNo;
    // container of data to be able to create the bon livraison report
    unsigned m_nbCmd;
    tplbonlivraison m_bonData;
    // size
    // capacity
  };
} // End of namespace