// App include
#include "PdfMinerReader.h"
// Boost include
#include <boost/algorithm/string.hpp> // string algorithm

namespace bdAPI 
{

	void PdfMinerReader::readShippedTo( const vecofstr& aFieldValue, BoDucFields& aBoducF)
	{
		// concatenate the the string to build the whole address and set the BoDuc field
		std::string w_wholeAddress;
		for(const auto& val : aFieldValue)
		{
			if( boost::contains(val,"*** COVILAC***"))
			{
				continue;
			}
			w_wholeAddress += val + " ";
		}
		// set boduc stuff
		aBoducF.m_deliverTo = w_wholeAddress;
	}

	void PdfMinerReader::readDeliveryDate( const std::string& aFiedValue, BoDucFields& aBoducF)
	{
		aBoducF.m_datePromise = aFiedValue;
	}

} // End of namespace

