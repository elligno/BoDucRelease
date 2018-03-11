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
//		static QList<QVector<QVariant>> readFileTab();
		// split data separated by tab
//		static void split( const std::string &s, char delim, std::vector<std::string> &elems);
	};
} // End of namespace
