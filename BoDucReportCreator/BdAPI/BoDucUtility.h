#pragma once

// C++ includes
#include <string>
#include <vector>
// Qt includes
#include <QList>
#include <QVariant>

namespace bdAPI 
{
	class BoDucUtility 
	{
	public:
		static std::vector<std::string> TrimRightCommaAndErase( const std::string& aStr2Trim);
		// Used when we have a line in the address that
		// we want to check if it ends with postal code
		static bool isPostalCode(const std::string& aAddress);
		// fix to address malfunctions
		static std::string AddressFixAlgorithm( const std::vector<std::string> &w_vecOfAddressPart);
		// read file of command
		std::wstring ConvertFromUtf8ToUtf16(const std::string& str);
		// same as above but using C++11/14 features
		static std::wstring FromUtf8ToUtf16(const std::string & str);
	};
} // End of namespace
