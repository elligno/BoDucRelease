#pragma once

#include <vector>

#include "QBoDucFields.h"
#include "BoDucCmdText.h"

namespace bdAPI
{
  /** Base class for all algorithm that parse BoDucStruct 
	 * that has been read from a csv file format. To be completed!!
	*/
	class ExtractDataAlgorithm
	{
	public:
		/** Destructor*/
		virtual ~ExtractDataAlgorithm() = default;
   
    /** return the BoDucFields ... to be completed*/ 
		virtual void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) = 0;

    /** Returns a BoDucField struct (fill the data structure) read from a text command format*/
    virtual QBoDucFields fillBoDucFields( const BoDucCmdText& aCmdVec) = 0;
	};
} // End of namespace
