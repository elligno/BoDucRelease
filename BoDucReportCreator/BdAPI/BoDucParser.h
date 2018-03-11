#pragma once

// C++ include
#include <string>
// STL includes
#include <vector>
#include <map>

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
		// copy and assign ctor not allowed
		BoDucParser( const BoDucParser& aOther) = delete;
		BoDucParser& operator= ( const BoDucParser& aOther) = delete;

		// main algorithm to parse the csv file format of BoDuc
		virtual void extractData( const mapIntVecstr& aListOfCmd, BoDucBonLivraisonAlgorithm* aReader);
	private:
		//	virtual void ShippedToAlgo() {/*not implemented yet*/}
		bool useTM( const std::vector<std::string>& aVecOfCmdLines);
	};
} // End of namespace
