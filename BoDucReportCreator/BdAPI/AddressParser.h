#pragma once

#include <vector>

namespace bdAPI 
{
	// To parse the address and search for the attributes such as:
	// pattern, postal code
	// according to the information returned by the parser, user
	// can take some action to correct or do something
	//
	class AddressParser 
	{ 
	public:
		enum class ePattern 
		{
			NoMalfunction=0,
			DuplAddressSymmetric=1,    // same address
			DuplAddressNonSymmetric=2, // not same address
			Livraison=3,               // livraison keyword inserted in the address
			Middle3Lines=4,            // second line of address is screw up 
			LastLineSrewUp=5,          // mixed address (city name)
			FirstLineWrong=6           // (2,3-lines) first line is wrong address, don't consider it
		};
	public:
		AddressParser( const std::vector<std::string>& aAddressPart);
		bool hasSymmetry() const { return m_vecPart.size()%2==0 ? true : false; }
		bool hasMalfunctions();
		bool isAddressPartTrimed() const { return m_addrspartTrim; }
		size_t getNbLines() const { return m_nbLines; }
		ePattern getPattern() const { return m_pattern; }
		const std::vector<std::string>& getVecPart() const { return m_vecPart; }
		short getPosLivraison() const { return m_pos; }
		void trimAdrsPart2Right();
	private:
		void analyze(); // set attribute of the address (check malfucntion in the address)
		bool checkLivraisonKeyword(); // check if LIVRAION key word is part of the address
		bool checkPairLinesEqual(size_t aNbLines);
		bool checkPostalCode(const std::string& aPcode);
		bool isSecondLineSrewUp();
		bool isLastLineScrewUp();
		bool isFirstLineEndWithCode();
		bool isLineTerminatewithCode( const std::string& aStr2Analyze);
		ePattern m_pattern;
		size_t m_nbLines;
		std::vector<std::string> m_vecPart;
		bool m_addrspartTrim;
		short m_pos;
	};
} // End of namespace