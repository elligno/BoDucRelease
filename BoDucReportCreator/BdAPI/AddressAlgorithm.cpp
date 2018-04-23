#include <sstream>
// STL include
#include <algorithm>
// Boost includes
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
// test includes
#include "AddressParser.h"
#include "AddressAlgorithm.h"

namespace bdAPI 
{

	std::vector<std::string> AddressAlgorithm::fixSymetryDuplAddress( AddressParser& aAddrsParser)
	{
		using namespace boost;

		if (aAddrsParser.getNbLines() == 6)
		{
			if( !aAddrsParser.isAddressPartTrimed())
			{
				aAddrsParser.trimAdrsPart2Right();
				std::vector<std::string> w_vecOfAdrsLines = aAddrsParser.getVecPart();
				return std::vector<std::string>({ w_vecOfAdrsLines[3],
					w_vecOfAdrsLines[4],
					w_vecOfAdrsLines[5] });
			}
			else
			{
				std::vector<std::string> w_vecOfAdrsLines = aAddrsParser.getVecPart();
			return std::vector<std::string>({ w_vecOfAdrsLines[3],
				w_vecOfAdrsLines[4],w_vecOfAdrsLines[5] });
			}
		}
		else if (aAddrsParser.getNbLines() == 4)
		{
			std::vector<std::string> w_vecOfAdrsLines = aAddrsParser.getVecPart();
			std::string w_vec1 = trim_right_copy_if(w_vecOfAdrsLines[2], is_any_of(",")); // 
			std::string w_vec2 = trim_right_copy_if(w_vecOfAdrsLines[3], is_any_of(",")); // 

			return std::vector<std::string>({ w_vec1,w_vec2 });
		}
		else if (aAddrsParser.getNbLines() == 2)
		{
			std::vector<std::string> w_vecOfAdrsLines = aAddrsParser.getVecPart();
			std::string w_vec1 = trim_right_copy_if(w_vecOfAdrsLines[1], is_any_of(",")); // 
			return std::vector<std::string>({ w_vecOfAdrsLines });
		}
		else
		{
			return std::vector<std::string>({});
		}
	}

	std::vector<std::string> 
		AddressAlgorithm::fixNonSymetryDuplAddress( const size_t aNbLines, const std::vector<std::string>& aVec)
	{
		using namespace boost;
		using namespace boost::algorithm;

		// check the second lines for nb==4
		if( aNbLines == 4)
		{
			// jani fix
			if( boost::contains( aVec[2], "*** COVILAC***"))
			{
				return std::vector<std::string>({ aVec[0],aVec[3] });;
			}

			std::string w_tmpStr = aVec[1]; // second line 
			// split last lines with token ","
 			std::vector<std::string> w_splitVec;
// 			split(w_splitVec, w_tmpStr, boost::is_any_of(","));
			w_splitVec = splitAboutComma(aVec[1]);
			// remove empty character (lambda function) and then erase algo
			auto remEmpty = [](const std::string& aStr) -> bool { return aStr.empty(); };
			w_splitVec.erase(std::remove_if(w_splitVec.begin(), w_splitVec.end(), remEmpty), w_splitVec.cend());
			assert(3 == w_splitVec.size());			// take last element 
			const std::string& w_lastElem = w_splitVec.back();
			if( isPostalCode(w_lastElem)) // check for postal code
			{
				// if exist, then the end of first address
				// take last 2 lines as valid address
				std::string w_part1 = trim_right_copy_if(aVec[2], is_any_of(","));
				std::string w_part2 = trim_right_copy_if(aVec[3], is_any_of(","));

				return std::vector<std::string>({ w_part1,w_part2 });
			}
		}
		else if (aNbLines==6)
		{
			std::string w_part1 = trim_right_copy_if(aVec[3], is_any_of(","));
			std::string w_part2 = trim_right_copy_if(aVec[4], is_any_of(","));
			std::string w_part3 = trim_right_copy_if(aVec[5], is_any_of(","));

			return std::vector<std::string>({ w_part1,w_part2,w_part3 });
		}
// 		else if( aNbLines==2)
// 		{
// 			// need to check for last line size and see if greater than 4 
// 			const std::string& w_lastLine = aVec.back();
// 			return splitAboutComma(w_lastLine);
// 		}
		return std::vector<std::string>();
	}

	std::vector<std::string> AddressAlgorithm::splitAboutComma( const std::string& aLine2Split)
	{
		// need to check for last line size and see if greater than 4 
		std::vector<std::string> w_vecLastLineSplit;
		boost::split(w_vecLastLineSplit, aLine2Split, boost::is_any_of(","));
// 		if (w_vecLastLineSplit.size() > 3)
// 		{
			return std::vector<std::string>(w_vecLastLineSplit.cbegin(), w_vecLastLineSplit.cend());
// 		}
// 		return std::vector<std::string>();
	}

	std::vector<std::string> AddressAlgorithm::fixLivraisonInserted(AddressParser & aAddrsParser)
	{
		std::vector<std::string> w_retAddress;
		w_retAddress.reserve(6);

		// need to find position of "livraison"
		// then lines below are the address
		// Client name is the first line, but we may need to clean-up
		// by searching inc keyword
		std::vector<std::string> w_adrsPart;
		w_adrsPart.reserve(5);
		const std::vector<std::string>& w_vecAdrs = aAddrsParser.getVecPart();
		auto begIter = w_vecAdrs.cbegin();
		while (begIter != w_vecAdrs.cend())
		{
			if (boost::iequals(*begIter, std::string("livraison")))
			{
				//	w_foundIt = true;
				auto dist = std::distance(w_vecAdrs.cbegin(), begIter);
				auto w_newBeg = w_vecAdrs.cbegin() + dist;
				std::advance(w_newBeg, 1);
				while (w_newBeg!=w_vecAdrs.cend())
				{
					w_adrsPart.push_back(*w_newBeg);
					++w_newBeg;
				}
				break; // we found it
			}
			++begIter;
			//++m_pos;
		}//while-loop

		// now we need to check for client name which is the first line
		const std::string& w_clientName = w_vecAdrs.front();
		std::vector<std::string> w_clientSplit;
		w_clientSplit.reserve(10);
		std::stringstream w_splitClientName(w_clientName);
		std::string w_namePart;
		while( w_splitClientName>>w_namePart)
		{
			if( !boost::iequals(w_namePart,"INC."))
			{
			   w_clientSplit.push_back(w_namePart);
			}
			else
			{
		    w_clientSplit.push_back(std::string("inc"));
				break;
			}
		}
		std::string w_fullName = boost::join(w_clientSplit, " ");
		std::string w_fullAddress = boost::join(w_adrsPart,",");
		std::string w_nameAndAdrs = w_fullName + std::string(" ") + w_fullAddress;
		std::vector<std::string> w_retAddrs;
		w_retAddrs.push_back(w_fullName);
		w_retAddrs.push_back(w_fullAddress);

		return w_retAddrs;
	}

	std::vector<std::string> AddressAlgorithm::fixLastLinesCityDupl( AddressParser & aAddrsParser,
		const std::vector<std::string>& aVecSplitted)
	{
		std::vector<std::string> w_retAddress;
		// need to split last line with token = ',' and then check the third element
		// if start with blank space use the algorithm "find_first_not_of
		// to retrieve the first non blank character. Create a substring with begin
		// at position of the find... to the end
// 		std::vector<std::string> w_vecLastLinePart;
// 		std::vector<std::string> w_vecPart = aAddrsParser.getVecPart();
// 		std::string w_lastLine = w_vecPart[2]; // third element
// 		boost::split(w_vecLastLinePart, w_lastLine, boost::is_any_of(","));
		std::string w_part2check = aVecSplitted[2];
		auto w_found = w_part2check.find_first_not_of(' ');
    // ...
		if( w_part2check.find_first_not_of(' ')>1)
		{
			// need to find first non-blank character
			auto w_found = w_part2check.find_first_not_of(' ');

			if( w_found!=std::string::npos)
			{
				auto begNewString = w_part2check.cbegin();
				std::advance(begNewString, w_found);
				std::string w_finalAddress( begNewString, w_part2check.cend());
				w_retAddress.push_back(std::string(begNewString, w_part2check.cend()));
			}
		}
		else // start with one blank, check for postal code
		{
			// take 7 character of the string
			auto begPostalCode = w_part2check.cbegin()+1;
			std::advance(begPostalCode, 7); // postal code end
			if( isPostalCode(std::string(w_part2check.cbegin(),begPostalCode)))
			{
				// what we do, erase these characters
				w_part2check.erase(w_part2check.cbegin(), begPostalCode);
				// need to find first non-blank character
 				auto w_found = w_part2check.find_first_not_of(' ');
				if (w_found != std::string::npos)
				{
					auto begNewString = w_part2check.cbegin();
					std::advance(begNewString, w_found);
					std::string w_finalAddress(begNewString, w_part2check.cend());
					w_retAddress.push_back(std::string(begNewString, w_part2check.cend()));
				}
			}
		}
		// check size of our last line (debugging )
		w_retAddress.push_back( aVecSplitted[3]);
		w_retAddress.push_back( aVecSplitted[4]);
		return w_retAddress;
	}

	std::vector<std::string> AddressAlgorithm::fixLastLinesCityDupl( const std::vector<std::string>& aVecSplitd)
	{
		std::vector<std::string> w_retAddress;
		w_retAddress.reserve(5);
		// third element of vector splitted (city, prov, element we interested of)
// 		std::string w_string2Split;
// 		if (aVecSplitd.size()==2)
// 		{
// 			w_string2Split = aVecSplitd[1];
// 		}
// 		else
// 		{
// 			w_string2Split = aVecSplitd[2];
// 		}
		auto w_found = aVecSplitd[2].find_first_not_of(' ');
		// take 7 character of the string
		std::string w_erasedStr = aVecSplitd[2];
		auto begPostalCode = aVecSplitd[2].cbegin() + 1;
		std::advance(begPostalCode, 7); // postal code end
		if( isPostalCode(std::string(aVecSplitd[2].cbegin(), begPostalCode)))
		{
			// what we do, erase these characters postal code
		//	std::string w_erasedStr = aVecSplitd[2];
			w_erasedStr.erase(w_erasedStr.begin(), w_erasedStr.begin()+8);
			// need to find first non-blank character
			auto w_found = w_erasedStr.find_first_not_of(' ');
			if (w_found != std::string::npos)
			{
				auto begNewString = w_erasedStr.cbegin();
				std::advance(begNewString, w_found);
				std::string w_finalAddress(begNewString, w_erasedStr.cend());
				w_retAddress.push_back(std::string(begNewString, w_erasedStr.cend()));
				if (aVecSplitd.size()==5)
				{
					w_retAddress.push_back(aVecSplitd[3]);
					w_retAddress.push_back(aVecSplitd[4]); // to complete the address (prov and code)
				}
				return w_retAddress;
			}
		}
		return std::vector<std::string>();
	}

	std::vector<std::string> AddressAlgorithm::fixThreeLinesAddress( AddressParser & aAddrsParser)
	{
		std::vector<std::string> w_vecFixadrs;

		// need to check for second line if it is screw up
		// for example inserted address, need to check for postal code at the end
		// if so then just remove this line and the address is 1-2 line index
		AddressParser::ePattern w_pattern = aAddrsParser.getPattern();
		if (w_pattern==AddressParser::ePattern::Middle3Lines)
		{
			w_vecFixadrs.reserve(2);
			w_vecFixadrs.push_back(aAddrsParser.getVecPart()[0]);
			w_vecFixadrs.push_back(aAddrsParser.getVecPart()[2]);
		}
		else // for now last line with city duplicate
		{
			// check for last line
			std::vector<std::string> w_lastLine = 
				splitAboutComma(aAddrsParser.getVecPart().back());
			// check size > 4
			if( w_lastLine.size()>4)
			{
				// need to fix that last line
				return fixLastLinesCityDupl(w_lastLine);
			}
		}
		return w_vecFixadrs;
	}

	bool AddressAlgorithm::isPostalCode( const std::string& aAddress)
	{
		// split with token = ,
		// take last element of split vector and count 
		// if alpha and if digit by using the count count_if
		// stl algorithm. If both count == 3 we have a postal code

		std::locale loc;
		auto w_nbAlpha = std::count_if(aAddress.cbegin(), aAddress.cend(),
			[loc](unsigned char c) { return std::isalpha(c, loc); }
		);

		auto w_nbDigit = std::count_if(aAddress.cbegin(), aAddress.cend(),
			[loc](unsigned char c) { return std::isdigit(c, loc); } 
		);
		// postal code has same number of digit and character
		if (w_nbAlpha == w_nbDigit)
		{
			return true;
		}
		else
		{
			size_t w_pos = aAddress.find_first_not_of(' ');
			// check last 3 characters (Francois Ethier corrupted postal code)
			auto begCode = aAddress.cbegin();
			if (w_pos==1)
			{
			   std::advance(begCode, 4);// because blank character at the beginning
			}
			else
			{
				std::advance(begCode, 3);// because blank character at the beginning
			}
			w_nbDigit = std::count_if(begCode, aAddress.cend(),
				[loc](unsigned char c) { return std::isdigit(c, loc); }
			);
			if (w_nbDigit == 2)
			{
				return true;
			}
		}
		return false;
	}

} // End of namespace

