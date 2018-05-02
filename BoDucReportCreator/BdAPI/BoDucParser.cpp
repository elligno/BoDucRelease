
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
//#include "BoDucBonLivraisonAlgorithm.h"

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
			if( PdfMinerAlgo* w_castCheck = dynamic_cast<PdfMinerAlgo*>(aBonLivraisonAlgo))
			{
				auto begVec = w_cmd2Proceed.begin();
				while( begVec != w_cmd2Proceed.cend())
				{
					if( boost::contains(*begVec, std::string("TM")) || boost::contains(*begVec, std::string("TON")))
					{
						//retTM = true;
						break;
					}
					else
					{
						++begVec;
					}
				}
			}
			else
			{
				if( !useTM(w_cmd2Proceed))
				{
					++w_begMap; // next in the list
					continue; //"TM" not used in this command
				}
			}

			// filling the BoDuc struct	
			BoDucFields w_boducStruct;
			if( aBonLivraisonAlgo != nullptr)
			{
				aBonLivraisonAlgo->fillBoDucFields(w_cmd2Proceed, w_boducStruct);
			}
			// deprecated
			aBonLivraisonAlgo->addBoDucField( w_boducStruct);
			++w_begMap; // next in the list

			// refactoring (new version)
			//m_bdStruct.push_back(w_boducStruct);

#if _DEBUG
			std::cout << "\n";
			std::cout << "BoDuc command No is: " << w_boducStruct.m_noCmd << "\n";
			std::cout << "Command shipped to: " << w_boducStruct.m_deliverTo << "\n";
			std::cout << "Deliver date is: " << w_boducStruct.m_datePromise << "\n";
			std::cout << "Product ordered is: " << w_boducStruct.m_produit << "\n";
			std::cout << "Quantity ordered is: " << w_boducStruct.m_qty << "\n";
			std::cout << "Silo number is: " << w_boducStruct.m_silo << "\n";
#endif
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

