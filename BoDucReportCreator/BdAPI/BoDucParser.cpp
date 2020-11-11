
// C++ include
#include <iostream>
// boost includes
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
// App includes
#include "BoDucParser.h"
#include "BoDucFields.h"
#include "PdfMinerAlgo.h"

namespace bdAPI 
{
	BoDucParser::BoDucParser(BoDucBonLivraisonAlgorithm * aReader)
  : m_parserAlgo(aReader)
	{
		m_bdStruct.reserve(50); // default 
	}
	void BoDucParser::extractData( const mapIntVecstr& aListOfCmd,
		BoDucBonLivraisonAlgorithm* aBonLivraisonAlgo)
	{
		auto w_begMap = aListOfCmd.cbegin();
		while( w_begMap != aListOfCmd.cend()) // process each cmd parsed from csv file
		{
			vecofstr w_cmd2Proceed = w_begMap->second;

      // when converting from pdf to txt, it introduce empty string element
      // delete all empty string element to be able to parse with our algorithm
      if( std::any_of( w_cmd2Proceed.cbegin(), w_cmd2Proceed.cend(), [](const std::string& aStr) { return aStr.empty(); }))
      {
        w_cmd2Proceed.erase( std::remove_if(w_cmd2Proceed.begin(), w_cmd2Proceed.end(), // delete empty element
          [] ( const std::string& s) // check if an empty string
        {
          return s.empty();
        }), w_cmd2Proceed.cend());
      }

      if (!std::any_of(w_cmd2Proceed.cbegin(), w_cmd2Proceed.cend(),
        [](const std::string& aStr)
      {
        // check for a valid
        return boost::contains(aStr, std::string("TM")) || boost::contains(aStr, std::string("TON"));
      }))
      {
        ++w_begMap; // next in the list
        continue;   //"TM" or "TON" not used in this command (not valid)
      }

      // filling the BoDuc struct	
      BoDucFields w_boducStruct;
      if (aBonLivraisonAlgo != nullptr)
      {
        aBonLivraisonAlgo->fillBoDucFields(w_cmd2Proceed, w_boducStruct);
      }
      // deprecated
      aBonLivraisonAlgo->addBoDucField(w_boducStruct);
      ++w_begMap; // next in the list
		}//while-loop
	}

	void BoDucParser::extractData(const mapIntVecstr & aListOfCmd)
	{
		// not implemented yet
	}

	bool BoDucParser::useTM( const std::vector<std::string>& aVecOfCmdLines)
	{
		using namespace std;
		using namespace boost;
		using namespace boost::algorithm;

		bool retTM = false;
		auto lineIter = aVecOfCmdLines.cbegin();
		do
		{
			lineIter++;
		} while( !contains(*lineIter, std::string("produit et description")));

		using dist = iterator_traits<std::string::iterator>::difference_type;
		dist w_startStr = distance(aVecOfCmdLines.cbegin(), lineIter);
		auto begVec = aVecOfCmdLines.cbegin() + w_startStr;
		while (begVec != aVecOfCmdLines.cend())
		{
			if( contains( *begVec, std::string("TM")) || contains(*begVec, std::string("TON")))
			{
				retTM = true;
				break;
			}
			++begVec;
		}

		return retTM;
	}
} // End of namespace

