#pragma once

#include <vector>

namespace bdAPI { class AddressParser; }
namespace bdAPI 
{
	//
	// For now i just put all fix in a bunch of static method
	// 
	class AddressAlgorithm 
	{
	public:
		static std::vector<std::string> fixSymetryDuplAddress( AddressParser& aAddrsParser);

		static std::vector<std::string> fixNonSymetryDuplAddress( const size_t aNbLines, 
			const std::vector<std::string>& aVec);

		// ... to be completed
		static std::vector<std::string> fixLivraisonInserted( AddressParser& aAddrsParser);
		static std::vector<std::string> fixLastLinesCityDupl( AddressParser& aAddrsParser, 
			const std::vector<std::string>& aVec);
		static std::vector<std::string> fixLastLinesCityDupl( const std::vector<std::string>& aVecSplid);
		static std::vector<std::string> fixThreeLinesAddress( AddressParser& aAddrsParser);
		static std::vector<std::string> fixFirstLineMixAddress(AddressParser& aAddrsParser);
		static std::vector<std::string> splitAboutComma(const std::string& aVec);
	private:
		// this function is used when we have a line in the 
		// address that we want to check if it ends with postal
		// code. For example in symmetry case, want to know if
		// address is duplicate or different. Need to check  
		static bool isPostalCode( const std::string& aAddress);
	};
} // End of namespace
