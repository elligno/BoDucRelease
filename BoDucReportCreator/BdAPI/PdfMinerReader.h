#pragma once

#include "BoDucCsvReader.h"

namespace bdAPI 
{
	class PdfMinerReader : public BoDucReader
	{
	public:
		// alias
		using vecofstr = std::vector<std::string>;
	public:
		PdfMinerReader() {}
		~PdfMinerReader() {}
		
		// ...
		virtual void readNoCmd(const vecofstr& aCmdVec, BoDucFields& aBoducF) override {/*no implemenetation*/}
		virtual void readNoCmd(const std::string& aCmdVec, BoDucFields& aBoducF) override {/*no implemenetation*/ }
		// 
		virtual void readShippedTo(const vecofstr& aFieldValue, BoDucFields& aBoducF) override;
		virtual void readDeliveryDate(const std::string& aFiedValue, BoDucFields& aBoducF) override;
		virtual void readProdDescr(const std::string& aCmdVec, BoDucFields& aBoducF) override {}
		virtual void readQtySilo(const std::string & aFieldValue, BoDucFields& aBoducF) override {}
		virtual void readProdCode(const std::string& aFieldValue, BoDucFields& aBoducF) override {}
	};
} // End of namespace
