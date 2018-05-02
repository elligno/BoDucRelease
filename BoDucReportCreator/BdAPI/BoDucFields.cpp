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
bdAPI::BoDucFields::BoDucFields( bdTpl && aTpl)
{
	// to be completed
	m_noCmd       = std::get<0>(aTpl);
	m_deliverTo   = std::get<1>(aTpl);
	m_datePromise = std::get<2>(aTpl);
	m_prodCode    = std::get<3>(aTpl);
	m_produit     = std::get<4>(aTpl);
	m_qty         = std::get<5>(aTpl);
	m_silo        = std::get<6>(aTpl);
}

// ==================================
// Address Parser implementation
// ==================================

