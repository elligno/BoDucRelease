#include "BoDucReaderfix.h"
#include "BoDucUtility.h"

// just testing the algorithm (shall pass vector as const)
// namespace jb11 { std::string testParserAlgorithm( const std::vector<std::string>&vecOfAddressPart); }
namespace bdAPI 
{
	std::string BoDucReaderFix::rebuildShippedTo(const vecofstr & aAddressPart)
	{
		std::string w_rebuildStr;

		if( aAddressPart.size() == 5) // these are temporary fix
		{
			// we need to take the last 2 in most cases
			using criter = std::reverse_iterator<std::vector<std::string>::const_iterator>;
			criter w_lastPart = aAddressPart.crbegin() + 1; // start one before last
			auto w_start = aAddressPart.cbegin();
			w_rebuildStr += *w_start;   // should be the client name, but not sure
			w_rebuildStr += *w_lastPart + std::string(" ");
			w_lastPart = aAddressPart.crbegin();
			w_rebuildStr += *w_lastPart;
		}
		else if( aAddressPart.size() == 1) // shall do for this case, don't see any malfucntion
		{
			// no special treatment, take it as it is
			w_rebuildStr += aAddressPart[0];
		}
		else
		{
			// for now it return a rebuild string, but it should 
			// return the vector of address part (string) 
			w_rebuildStr = BoDucUtility::AddressFixAlgorithm(aAddressPart);
		}
		return w_rebuildStr;
	}
} // End of namespace
