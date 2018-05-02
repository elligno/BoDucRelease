#pragma once

// C++ include
#include <string>
// STL includes
#include <vector>
#include <map>
// App include
#include "BoDucFields.h"

// forward declaration
namespace bdAPI { class BoDucBonLivraisonAlgorithm; }

namespace bdAPI 
{
	// abstract base class
	class BoDucParser
	{
	public:
		// create alias
		using vecofstr = std::vector<std::string>;
		using mapIntVecstr = std::map<int, vecofstr>;
	public:
		BoDucParser() = default;
		// part of refactoring
		BoDucParser( BoDucBonLivraisonAlgorithm* aReader);
		// copy and assign ctor not allowed
		BoDucParser( const BoDucParser& aOther) = delete;
		BoDucParser& operator= ( const BoDucParser& aOther) = delete;

		// main algorithm to parse the csv file format of BoDuc (deprecated)
		virtual void extractData( const mapIntVecstr& aListOfCmd, BoDucBonLivraisonAlgorithm* aReader);
		// part of refactoring
		virtual void extractData(const mapIntVecstr& aListOfCmd);
		// return vector of al cmd
		std::vector<BoDucFields> getBdFields() { return m_bdStruct; }
	private:
		//shall be in the BdApp class
		bool useTM( const std::vector<std::string>& aVecOfCmdLines);

		// part of the refactoring
		BoDucBonLivraisonAlgorithm* m_parserAlgo; 
		std::vector<BoDucFields> m_bdStruct;
	};
} // End of namespace