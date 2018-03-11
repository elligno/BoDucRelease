#pragma once

// C++ includes
#include <iostream>
#include <string>
// STL includes
#include <map>
#include <vector>

namespace bdAPI 
{
	struct BoDucFields
	{
		// default ctor
		BoDucFields();

		std::string m_noCmd;
		std::string m_datePromise;
		std::string m_deliverTo;
		std::string m_produit;
		signed long m_prodCode;
		//int m_silo;  not always an integer (can be 2&3, 4-A and a string such as "Un seul silo")
		std::string m_silo;
		float m_qty; // better to set as a string
	};
} // End of namespace