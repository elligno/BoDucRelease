#pragma once

#include "BoDucBonLivraisonAlgorithm.h"

namespace bdAPI 
{
	class PdfMinerAlgo : public BoDucBonLivraisonAlgorithm
	{
	public:
		PdfMinerAlgo() {}
		~PdfMinerAlgo() {}
		void fillBoDucFields( const std::vector<std::string>& aCmdVec, BoDucFields& aBoDucField) override;
		void addBoDucField( const BoDucFields& aField2Add) override;
		virtual bool containsBoDucField(const BoDucFields& aField2Look);
		virtual bool removeBoDucField(const BoDucFields& aField2);
		virtual void getBoDucStruct(std::vector<BoDucFields>& aVec2Fill);
		virtual  size_t getnbOfCmd();
	private:
		std::vector<BoDucFields> m_vecOfStruct;
	};
} // End of namespace
