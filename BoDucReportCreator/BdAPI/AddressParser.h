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
    std::vector<std::string> getShippedAddress()
    {
      switch (m_pattern)
      {
      case AddressParser::ePattern::NoMalfunction:
        // check number of lines
        return m_vecPart;
        break;
      case AddressParser::ePattern::DuplAddressSymmetric:
        return std::vector<std::string>( m_vecPart.cbegin(), m_vecPart.cbegin()+m_vecPart.size()/2);
        break;
      case AddressParser::ePattern::DuplAddressNonSymmetric:
        if( m_nbLines==6 && isLineTerminatewithCode(m_vecPart[2]))
        {
          return std::vector<std::string>(m_vecPart.cbegin()+ m_vecPart.size()/2, m_vecPart.cend());
        }
        return m_vecPart;
        break;
      case AddressParser::ePattern::Livraison:
        return std::vector<std::string>();
        break;
      case AddressParser::ePattern::Middle3Lines:
        return std::vector<std::string>();
        break;
      case AddressParser::ePattern::LastLineSrewUp:
        return std::vector<std::string>();
        break;
      case AddressParser::ePattern::FirstLineWrong:
        return std::vector<std::string>();
        break;
      default:
        break;
      }
      return std::vector<std::string>();
    }
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