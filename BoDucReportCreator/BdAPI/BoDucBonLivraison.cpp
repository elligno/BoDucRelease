#include "BoDucBonLivraison.h"

namespace bdAPI 
{
  BoDucBonLivraison::BoDucBonLivraison(unsigned aUnitNo) 
  : m_unitNo(aUnitNo),
    m_nbCmd(0)
  {
    m_vecCmd.reserve(25);
    //      m_bonLivraisonData.reserve(25);
  }

  QVector<BoDucBonLivraison::tplcmdata> BoDucBonLivraison::fromVecCmd2VecTpl() const
  {
     QVector<BoDucBonLivraison::tplcmdata> w_bonLivraisonData;
    w_bonLivraisonData.reserve(m_vecCmd.size());
    // make sure that we don't carry data from previous handling

    // now ready to fill data for bon livraison
    for( const bdAPI::BoDucFields& bdValue : m_vecCmd)
    {
      // NOTE i am not sure of the behavior, are calling the move ctor? or the initializer list? will see
      // but i thing we are calling the move ctor, then it means the vector empty at the end
      w_bonLivraisonData.push_back( std::make_tuple( bdValue.deliverTo(), bdValue.produit(), bdValue.m_qty, bdValue.silo().toShort()));
    }
    return w_bonLivraisonData;
  }
} // End of namespace

