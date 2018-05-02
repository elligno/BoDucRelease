#pragma once

#include <vector>

#include "BoDucFields.h"

namespace bdAPI
{
	// Base class for all algorithm that parse BoDucStruct 
	// that has been read from a csv file format. To be completed!!
	//
	class BoDucBonLivraisonAlgorithm
	{
	public:
		// shall i provide a virtual dtor??
		virtual ~BoDucBonLivraisonAlgorithm() = default;
		// return the BoDucFields ... to be completed
		virtual void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) = 0;
		//TODO create a class VectorBoDucFields which implement these methods
		virtual void addBoDucField( const BoDucFields& aField2Add) = 0; // deprecated
		virtual bool containsBoDucField( const BoDucFields& aField2Look) = 0; 
		virtual bool removeBoDucField( const BoDucFields& aField2) = 0;
		virtual void getBoDucStruct( std::vector<BoDucFields>& aVec2Fill) = 0;
		virtual  size_t getnbOfCmd() = 0;
	};
} // End of namespace
