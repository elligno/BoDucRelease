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

// ==================================
// Address Parser implementation
// ==================================

