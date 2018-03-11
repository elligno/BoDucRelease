#pragma once

// C++ includes 
#include <iostream>
#include <string>
//#include <fstream>
// stl includes
#include <map>
#include <vector>
// app include
#include "BoDucFields.h"

namespace bdAPI
{
	// Abstract class
	//
	// Class that provides services for reading the BoDuc cmd csv file
	// The format of this file is not always the same and sometimes we
	// need to override some reading algorithm (mainly these algorithms)
	// are based on the following fields
	//
	class BoDucReader
	{
	public:
		// will need to set some keyword of ... in the parsing process
		//	bool containsMoreThanOne() const { return true; }
		virtual void readNoCmd( const std::vector<std::string>& aCmdVec, BoDucFields& aBoducF) = 0;
		virtual void readNoCmd( const std::string& aCmdVec, BoDucFields& aBoducF) = 0;
		virtual void readShippedTo( const std::vector<std::string>& aCompleteAddress, BoDucFields& aBoducF) = 0;
		virtual void readDeliveryDate( const std::string& aFieldValue, BoDucFields& aBoducF) = 0;
		virtual void readProdDescr( const std::string& aCmdVec, BoDucFields& aBoducF) = 0;
		virtual void readQtySilo( const std::string & aFieldValue, BoDucFields& aBoducF) = 0;
		virtual void readProdCode( const std::string& aFieldValue, BoDucFields& aBoducF) = 0;
	};
} // End of namespace
