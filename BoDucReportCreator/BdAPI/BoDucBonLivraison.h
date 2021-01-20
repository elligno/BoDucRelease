
#pragma once

// C++ include
#include <tuple>
// Qt includes
#include <QString>
#include <QVector>
// App include
#include "BdAPI/BoDucFields.h"

namespace bdAPI 
{
  /** Hold all data for the creation of Bon livraison. */

  // in BoDucReportCreator creator in prototype version		
  // shall be in the BoDucBonLivraisonMgr
  // key is the unit no and all values for report creation
 //  QMultiMap<QString, tplbonlivraison> m_unitBonLivraisonData;
  // Itereable class, when creating the BonLivraison report,
  // retrieve the data form BonLivraison.
  class BoDucBonLivraison 
  {
    // data provided  
    using tplbonlivraison = std::tuple<unsigned /*unit no*/, QString/*address*/, QString/*product*/, float/*qty*/, short/*silo*/>;
  public:
    /** */
    explicit BoDucBonLivraison( unsigned aUnitNo) :m_unitNo(aUnitNo) 
    {
      m_vecCmd.reserve(25);
      m_bonLivraisonData.reserve(25);
    }

    // what about copy/assignment ctor? default will do?
    unsigned unitNo() const { return m_unitNo; }
    
    // add a structure that hold data? 
    // addCmd(tplbonlivraison) with command number??
    void addCmd( const BoDucFields& aField2Add) { m_vecCmd.push_back(aField2Add); }

    // remove by command number such as "CO123456"
    bool removeCmd() { throw "Not implemented yet"; }

    /** look for a command by command order with following format: "CO123456" */
    bool containsCmd( const QString& aCmdNo) { throw "Not implemented yet"; }

    /** number of command */
    unsigned nbOfCmd() const { return m_nbCmd; }

    /** return bon livraison data*/
    QVector<tplbonlivraison> getBonLivraisonCmd()
    {
      // first need to fill bon livraison with data from vector command
      fromVecCmd2VecTpl();

      return m_bonLivraisonData;
    }

    /** check if any cmd has been added */
    bool isEmpty() const { return m_bonLivraisonData.isEmpty(); }

    // return information about 
    // status of the bon livraison: final or under construction
    // iterate through all command
    // access to specific command
  private:
    unsigned m_unitNo;                           /**< unit number */
    // container of data to be able to create the bon livraison report
    unsigned m_nbCmd;                            /**< number of command */
    QVector<BoDucFields> m_vecCmd;               /**< list of all command*/
    QVector<tplbonlivraison> m_bonLivraisonData; /**< bon livraison data report */
    // size
    // capacity
    /** */
    void fromVecCmd2VecTpl()
    {
      if( m_vecCmd.isEmpty())
      {
        return;
      }

      // make sure that we don't carry data from previous handling
      if (!m_bonLivraisonData.isEmpty())
      {
        m_bonLivraisonData.clear();
      }

      // now ready to fill data for bon livraison
      for( auto& w_cmdData : m_vecCmd)
      {
        // NOTE i am not sure of the behavior, are calling the move ctor? or the initializer list? will see
        // but i thing we are calling the move ctor, then it means the vector empty at the end
        m_bonLivraisonData.push_back( {m_unitNo, w_cmdData.deliverTo(), w_cmdData.produit(), w_cmdData.m_qty, w_cmdData.silo().toShort()});
      }
    }
  };
} // End of namespace