#include <sstream>
// Boost includes
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
// Package includes
#include "BoDucUtility.h"
#include "AddressParser.h"

using namespace bdAPI;

AddressParser::AddressParser( const std::vector<std::string>& aAddressPart)
: m_addrspartTrim(false),
m_pattern(ePattern::NoMalfunction) // default there is no malfunction in the address
{
	if (!m_vecPart.empty())
	{
		m_vecPart.clear();
	}
	m_vecPart.assign( aAddressPart.cbegin(), aAddressPart.cend());
	analyze(); // ...
}

void AddressParser::trimAdrsPart2Right()
{
	using namespace boost;
	using namespace boost::algorithm;

	// trim all lines
	for( std::string& w_lin : m_vecPart)
	{
		trim_right_if(w_lin, is_any_of(","));
	}
	m_addrspartTrim = true;
}

void AddressParser::analyze()
{
	// for now its the symetry part
	m_nbLines = m_vecPart.size();

	// trim all lines
	trimAdrsPart2Right();
	if( checkLivraisonKeyword())
	{
		m_pattern = ePattern::Livraison;
	}
	else if( m_nbLines == 6 || m_nbLines == 4 || m_nbLines == 2)
	{
		// check pair
		if( checkPairLinesEqual(m_nbLines))
		{
			m_pattern = ePattern::DuplAddressSymmetric;
		}
		else
		{
			// case 2-lines, last line might be screw up with mixed city name
			// this mean that our pattern is lastlinesrewup, we need to call 
			// according algorithm to fix the last line
			if( isLastLineScrewUp())
			{
				m_pattern = ePattern::LastLineSrewUp;
			}
			else 
			{
				// for this case no need to go further, line not equal
				// there is no malfunction in the address conversion
				// It might have a malfunction such as sold to address
				// in the first line and second line is the Shippe to
				// Address in one line, need to check if it ends by postal code
				if( m_vecPart.size()==2)
				{
					if( isFirstLineEndWithCode())
					{
						m_pattern = ePattern::FirstLineWrong;
					}
					else
					{
						m_pattern = ePattern::NoMalfunction;
					}
				}
				else //4,6 lines
				{
					// check for COVILAC keyword
					// for now there only one situation for this use case
					// file jani_xxx.csv and i haven't seen this in another 
					// situation (4-lines keyword).
					// Based on the same algorithm as LIVRAISON keyword
					// take the line below the keyword

					// case 4-lines, second line can be screw up
					m_pattern = ePattern::DuplAddressNonSymmetric;
				}
			}
		}
	}
	else if( m_nbLines==3) // no symmetric
	{
		// 3-lines case 
		if( isSecondLineSrewUp())
		{
		   m_pattern = ePattern::Middle3Lines;
		}
		else if( isLastLineScrewUp()) // duplicate city name
		{
			m_pattern = ePattern::LastLineSrewUp; //?? not sure about this one
		}
		else if( isFirstLineEndWithCode()) // first line finish by a postal code
		{
			// case 3-lines if so take last 2
			m_pattern = ePattern::FirstLineWrong;
		}
		else 
		{
			m_pattern = ePattern::NoMalfunction;
		}
	}
	else if (m_nbLines == 5) // most of the time its piece of crap
	{                        // not too many of them, most 2,3,4 lines
//   	else if (is2nd3rdLineHasCode())
// 	  {
// 		   // case 5-lines where second and third have inserted in the shipped to
// 	  }
	}
}

bool AddressParser::checkLivraisonKeyword()
{
	bool w_foundIt = false;
	short m_pos = 0; // initial position at beginning
	// go through each line of the address and check 
	// if one line contains LIVRAISON keyword
//	std::vector<std::string> m_vecPart;
	auto begAddressVec = m_vecPart.cbegin();
	while (begAddressVec!=m_vecPart.cend())
	{
		if( boost::iequals(*begAddressVec,std::string("livraison")))
		{
			w_foundIt = true;
			break; // we found it
		}
		++begAddressVec;
		++m_pos;
	}
	return w_foundIt;
}

bool AddressParser::checkPairLinesEqual( size_t aNbLines)
{
	using namespace std;
	using namespace boost;

	std::pair<std::string, std::string> w_pair1;
	std::pair<std::string, std::string> w_pair2;
	std::pair<std::string, std::string> w_pair3;
	short i = {}; //0

	switch (aNbLines)
	{
	case 6:
		w_pair1 = std::make_pair(m_vecPart[i], m_vecPart[i + 3]); ++i;
		w_pair2 = std::make_pair(m_vecPart[i], m_vecPart[i + 3]); ++i;
		w_pair3 = std::make_pair(m_vecPart[i], m_vecPart[i+3]);
    break;
	case 4:
		w_pair1 = std::make_pair(m_vecPart[i], m_vecPart[i + 2]); ++i;
 		w_pair2 = std::make_pair(m_vecPart[i], m_vecPart[i+2]);
		break;
	case 2:
		break;
	default:
		break;
	}

	// 
	if (aNbLines==6)
	{
		if( equal(w_pair1.first, w_pair1.second) &&
			equal(w_pair2.first, w_pair2.second) &&
			equal(w_pair3.first, w_pair3.second))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (aNbLines == 4)
	{
		if (equal(w_pair1.first, w_pair1.second) &&
			equal(w_pair2.first, w_pair2.second))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (aNbLines == 2)
	{
		if( equal(m_vecPart[0], m_vecPart[1]))
		{
			return true;
		}
		else
		{
			return false;
		}

	}
}

bool AddressParser::isSecondLineSrewUp()
{
	// case 3-lines where we have something inserted the address
	const std::string& w_secondline = m_vecPart[1];
	auto w_lastComma = w_secondline.find_last_of(",");
	if (w_lastComma==std::string::npos)
	{
		return false;
	}
	auto w_beg = w_secondline.cbegin();
	std::advance(w_beg,w_lastComma);
	std::string w_postalCode(w_beg,w_secondline.cend());
	if( checkPostalCode(std::string(w_beg, w_secondline.cend())))
	{
		// line is screwed up
		return true;
	}
	return false;
}

bool AddressParser::isLastLineScrewUp()
{
	// do a split on last line and check if size > 3
	const std::string& w_lastLine = m_vecPart.back();
	std::vector<std::string> w_vecSplit;
	boost::split(w_vecSplit,w_lastLine,boost::is_any_of(","));
  if( w_vecSplit.size()>3)
  {
		// case size==4 and line is not screw up
		// we need to check the third element for white space
		// elem[2] which can start with white spaces or postal 
		// code then followed with white spaces, at this point 
		// we have the last line screwed up.
		// 1. check find first not of '' and check distance
		// 2. 

		const std::string w_thirdElem = w_vecSplit[2];
		auto w_startWithBlank = w_thirdElem.find_first_not_of(' ');
    if( w_thirdElem.find_first_not_of(' ') > 2)
    {
			return true; // line is screw up
    }
		else  // first element is the postal code
		{
			// take 7 character of the string
			auto w_length = w_thirdElem.size();
			auto begPostalCode = w_thirdElem.cbegin() + 1;
			if( w_length>7)
			{
				std::advance(begPostalCode, 7); // postal code end
			}
			else
			{
			  std::advance(begPostalCode, 6); // postal code end
			}
			if( BoDucUtility::isPostalCode(std::string(w_thirdElem.cbegin(), begPostalCode)))
			{
				return true; // case that need to be fixed
			}
			else
			{
			  return false; // everything ok
			}
		}
  }
	return false;
}

bool AddressParser::isFirstLineEndWithCode()
{
	const std::string& w_lastLine = m_vecPart.front();
	std::vector<std::string> w_vecSplit;
	boost::split(w_vecSplit, w_lastLine, boost::is_any_of(","));
	if( checkPostalCode(w_vecSplit.back()))
	{
		return true;
	}
	return false;
}

// i saw one case with client name HyBrillia where 2nd and 3rd line
// were inserted in teh shipped address (from Sold To)
bool AddressParser::isLineTerminatewithCode(const std::string& aStr2Analyze)
{
	std::vector<std::string> w_vecSplit;
	boost::split( w_vecSplit, aStr2Analyze, boost::is_any_of(","));
	if (checkPostalCode(w_vecSplit.back()))
	{
		return true;
	}
	return false;
}
bool AddressParser::hasMalfunctions()
{
	return m_pattern!=ePattern::NoMalfunction;
}

bool AddressParser::checkPostalCode(const std::string& aPcode)
{
	std::locale loc;
	auto w_nbAlpha = std::count_if(aPcode.cbegin(), aPcode.cend(),
		[loc](unsigned char c) { return std::isalpha(c, loc); }
	);

	auto w_nbDigit = std::count_if(aPcode.cbegin(), aPcode.cend(),
		[loc](unsigned char c) { return std::isdigit(c, loc); }
	);
	// postal code has same number of digit and character
	if (w_nbAlpha == w_nbDigit)
	{
		return true;
	}
	return false;
}
