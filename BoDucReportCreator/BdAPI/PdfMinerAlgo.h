#pragma once

#include "ExtractDataAlgorithm.h"

namespace bdAPI 
{
	class PdfMinerAlgo : public ExtractDataAlgorithm
	{
	public:
    PdfMinerAlgo() = default;
    ~PdfMinerAlgo() = default;
		
    void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) override;

    /** */
    QBoDucFields fillBoDucFields( const BoDucCmdText& aCmdText) override { return QBoDucFields{}; }

// 		void addBoDucField( const BoDucFields& aField2Add) override;
// 		virtual bool containsBoDucField(const BoDucFields& aField2Look);
// 		virtual bool removeBoDucField(const BoDucFields& aField2);
// 		virtual void getBoDucStruct(std::vector<BoDucFields>& aVec2Fill);
	};
} // End of namespace
