#include <iostream>

#include "BoDucFields.h"

bdAPI::BoDucFields::BoDucFields() 
: m_noCmd(""),
  m_datePromise(""),
  m_deliverTo(""),
  m_produit(""),
  m_silo(""),
  m_prodCode(0), // same line as TM (1,/123456789,  qty, silo) page, product code, quantity and silo number/string
	//m_silo(0),   "TM" tonne metrique must be present otherwise the cmd is discarded
	m_qty(0.f)
{
	std::cout << "";
}

//TODO should declare a member 
// Design Note
//  To complicated, really need a move semantic,
//  would be easier to pass it as a reference 
bdAPI::BoDucFields::BoDucFields( bdTpl&& aTpl)
{
  m_bdTpl = std::move(aTpl);
	// to be completed??
	m_noCmd       = std::get<0>(m_bdTpl);
	m_deliverTo   = std::get<1>(m_bdTpl);
	m_datePromise = std::get<2>(m_bdTpl);
	m_prodCode    = std::get<3>(m_bdTpl);
	m_produit     = std::get<4>(m_bdTpl);
	m_qty         = std::get<5>(m_bdTpl);
	m_silo        = std::get<6>(m_bdTpl);

//  m_bdTpl = std::move(aTpl);
}

// ==================================
// Address Parser implementation
// ==================================

