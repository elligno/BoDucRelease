#pragma once

/// App include
#include "VictoReader.h"

namespace bdAPI 
{
	// re-implement the algorithm to parse address "Shipped To"
	// take into account a more generic version of malfunctions
	// support. 
	class BoDucReaderFix : public VictoReader 
	{
	public:
		BoDucReaderFix() = default;
	protected:
		std::string rebuildShippedTo( const vecofstr& aAddressPart) override;
	};
} // End of namespace
