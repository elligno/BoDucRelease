// C++ includes
#include <Windows.h>
#include <iostream>
#include <codecvt>
#include <vector>
// boost includes
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>       // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
#include  <boost/algorithm/string/predicate.hpp>
// App includes
#include "BoDucUtility.h"
#include "AddressAlgorithm.h"
#include "AddressParser.h"
#include "AddressBuilder.h"

using namespace bdAPI;

std::vector<std::string> BoDucUtility::TrimRightCommaAndErase( const std::string & aStr2Trim)
{
	using namespace std;
	using namespace boost;

	std::string w_str2Trim(aStr2Trim.cbegin(), aStr2Trim.cend());
	trim_right_if(w_str2Trim, is_any_of(","));
	
	// splitting string to retrieve COxxxxx cmd no
	vector<string> strs;
	split(strs, w_str2Trim, is_any_of(","));

	// not sure about this one, after this we should have a vector of 2 elements
	// because our search is based on a line that contains a pair (Date,No)
	strs.erase( remove_if(strs.begin(), strs.end(), // delete empty element
		[](const string& s)
	{ return s.empty(); }), strs.cend());

	return strs;
}

bool bdAPI::BoDucUtility::isPostalCode(const std::string & aAddress)
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
			if (w_pos == 1)
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

std::string BoDucUtility::AddressFixAlgorithm(const std::vector<std::string> &w_vecOfAddressPart)
{
	using namespace bdAPI;

	std::string w_complAddrs; //whole address
														// starting to parse file
	AddressParser w_testParser(w_vecOfAddressPart); // analyze the address for malfunction
	if (w_testParser.getPattern() == AddressParser::ePattern::NoMalfunction)
	{
		// no point to go further, just return the vec address part as it is
		AddressBuilder w_adrs2Build(w_vecOfAddressPart);
		w_complAddrs = w_adrs2Build.buildAddess();
		return w_complAddrs;
	}
	if (w_testParser.getPattern() == AddressParser::ePattern::Livraison)
	{
		// call the fix algorithm for this pattern, no point to go further
		// once LIVRAISON is removed for the whole address, usually we have 
		// to extract the lines below and take the first line as client name
		// Need to check but we already do that 
		std::vector<std::string> w_adrsFixed =
			AddressAlgorithm::fixLivraisonInserted(w_testParser);

		// build the whole address
		AddressBuilder w_addrsBuilder(w_adrsFixed);
		w_complAddrs = w_addrsBuilder.buildAddess();
		std::cout << "The complete address is: " << w_complAddrs.c_str() << "\n";
		return w_complAddrs;
	}
	if (w_testParser.hasSymmetry())
	{
		std::vector<std::string> w_fixedAddress;
		if (w_testParser.hasMalfunctions())
		{
			AddressParser::ePattern w_checkPattern = w_testParser.getPattern();
			if (w_checkPattern == AddressParser::ePattern::DuplAddressSymmetric)
			{
				// call address malfunction algorithm
				w_fixedAddress = AddressAlgorithm::fixSymetryDuplAddress(w_testParser);

				// build the whole address
				AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::DuplAddressNonSymmetric)
			{ // call address malfunction algorithm
				w_fixedAddress =
					AddressAlgorithm::fixNonSymetryDuplAddress(w_testParser.getNbLines(),
						w_testParser.getVecPart());

				// build the whole address
				AddressBuilder w_addrsBuilder(w_fixedAddress);
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::LastLineSrewUp)
			{
				std::vector<std::string> w_splitLine = AddressAlgorithm::splitAboutComma(w_vecOfAddressPart.back());
				if (std::any_of(w_splitLine.cbegin(), w_splitLine.cend(), [](const std::string& aStr) {return aStr.empty(); }))
				{
					w_splitLine.erase(std::unique(w_splitLine.begin(), w_splitLine.end()), w_splitLine.cend()); // white space
					w_splitLine.pop_back(); // erase last character which is  
				}

				//boost::trim_right_if(w_splitLine);
				assert(5 == w_splitLine.size());
				if (!w_splitLine.empty())
				{
					// we need to split about comma and then pass to algorithm
					w_fixedAddress = AddressAlgorithm::fixLastLinesCityDupl(w_splitLine);
					const std::string& w_val2insert = *w_testParser.getVecPart().cbegin();
					w_fixedAddress.insert(w_fixedAddress.cbegin(), w_val2insert); // insert at the beginning (client name)
					AddressBuilder w_adrs2Build(w_fixedAddress);
					w_complAddrs = w_adrs2Build.buildAddess();
				}
				else if (w_checkPattern == AddressParser::ePattern::FirstLineWrong)
				{
					// this case happen when we have 2-lines 
					if (w_testParser.getVecPart().size() == 2)
					{
						w_complAddrs = w_testParser.getVecPart().back();
					}
				}
			}
		}//end malfunction
		std::cout << "Complete address is: " << w_complAddrs.c_str() << "\n";
	}//end symmetry
	else
	{
		std::vector<std::string> w_fixedAddress;
		if (w_testParser.hasMalfunctions() && w_testParser.getNbLines() == 3)
		{
			// 				if( w_testParser.getNbLines()==3)
			// 				{
			AddressParser::ePattern w_checkPattern = w_testParser.getPattern();
			if (w_checkPattern == AddressParser::ePattern::Middle3Lines)
			{
				w_fixedAddress =
					AddressAlgorithm::fixThreeLinesAddress(w_testParser);
				// move semantic
				AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::LastLineSrewUp)
			{
				w_fixedAddress =
					AddressAlgorithm::fixThreeLinesAddress(w_testParser);
				const std::string& w_beforeLastOne = w_testParser.getVecPart()[1];
				w_fixedAddress.insert(w_fixedAddress.cbegin(), w_beforeLastOne); // insert at the beginning (street)
				const std::string& w_firstOne = *w_testParser.getVecPart().cbegin();
				w_fixedAddress.insert(w_fixedAddress.cbegin(), w_firstOne); // insert at the beginning (client name)
																																		// move semantic
				AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::FirstLineWrong)
			{
				// this case happen when we have 3-lines, take last 2 lines 
				if (w_testParser.getVecPart().size() == 3)
				{
					w_fixedAddress.push_back(w_testParser.getVecPart()[1]);
					w_fixedAddress.push_back(w_testParser.getVecPart()[2]);
					// move semantic
					AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
					w_complAddrs = w_addrsBuilder.buildAddess();
				}
			}
		}//if
		std::cout << "The complete address is: " << w_complAddrs.c_str() << "\n";
	} //else

	return w_complAddrs;
}

std::wstring BoDucUtility::ConvertFromUtf8ToUtf16(const std::string& str)
{
	std::wstring convertedString;
	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 0, 0);
	if (requiredSize > 0)
	{
		std::vector<wchar_t> buffer(requiredSize);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &buffer[0], requiredSize);
		convertedString.assign(buffer.begin(), buffer.end() - 1);
	}

	return convertedString;
}
std::wstring BoDucUtility::FromUtf8ToUtf16(const std::string & str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> cwuf8;
	std::wstring w_strConverted = cwuf8.from_bytes(str);
	return w_strConverted;
}
