
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
  public:
    // return all information about bon livraison
    using tplcmdata = std::tuple<QString/*address*/, QString/*product*/, float/*qty*/, short/*silo*/>;

    struct BonLivraisonData
    {
      // aggregate intialization = {...,...,...,...}
      BonLivraisonData() = default;
      BonLivraisonData( const BonLivraisonData& aOther) = default;

      QString m_address;
      QString m_product;
      float m_qty;
      short m_siloNo;
    };
  public:
    // needed when returning from map in BoDucReportCreator 
    BoDucBonLivraison() = default;

    /** */
    explicit BoDucBonLivraison( unsigned aUnitNo);

    /** what about copy/assignment ctor? default will do?*/
    unsigned unitNo() const { return m_unitNo; }
    
    // add a structure that hold data? 
    // addCmd(tplbonlivraison) with command number??
    void addCmd( const BoDucFields& aField2Add) { m_vecCmd.push_back(aField2Add); }

    /** remove by command number such as "CO123456"*/
    bool removeCmd() { throw "Not implemented yet"; }

    /** look for a command by command order with following format: "CO123456" */
    bool containsCmd( const QString& aCmdNo) { throw "Not implemented yet"; }

    /** number of command for this bon livraison*/
    QVector<tplcmdata>::size_type nbOfCmd() const
    { return m_vecCmd.size(); }

    /** return bon livraison data*/
    QVector<tplcmdata> getBonLivraison() const
    {
      // first need to fill bon livraison with data from vector BoDucFields
      QVector<tplcmdata> w_bonLivraisonData = fromVecCmd2VecTpl();

      return w_bonLivraisonData;
    }

    /** check if any cmd has been added */
    bool isEmpty() const { return m_vecCmd.isEmpty(); }

    // reset bon livraison
    void clear()
    {
      if( !m_vecCmd.empty())
      {
        m_vecCmd.clear();
      }
    }

    float getBonLivraisonTotalQty() const 
    {
      auto w_qty = 0.f;
      QVector<tplcmdata> w_bonLivraisonData = fromVecCmd2VecTpl();
      // return quantity for this bon livraison (sum qty over all cmd)
      for( const tplcmdata& w_bonData : w_bonLivraisonData)
      {
        w_qty += std::get<2>(w_bonData);
      }

      // total quantity for this bon livraison
      return w_qty; 
    }

    // return information about 
    // status of the bon livraison: final or under construction
    // iterate through all command
    // access to specific command
  private:
    unsigned m_unitNo;                           /**< unit number */
    // container of data to be able to create the bon livraison report
    unsigned m_nbCmd;                            /**< number of command */
    QVector<BoDucFields> m_vecCmd;               /**< list of all command*/
   // QVector<tplcmdata> m_bonLivraisonData;       /**< bon livraison data report */
   // QVector<BonLivraisonData> m_bonLivraisonData;
    // size
    // capacity
    /** ... to be completed*/
    QVector<tplcmdata> fromVecCmd2VecTpl() const;
  };
} // End of namespace