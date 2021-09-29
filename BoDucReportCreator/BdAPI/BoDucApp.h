#pragma once

// C++ includes
#include <string>
#include <vector>
#include <list>
#include <map>
// App include
#include "BoDucFields.h"

// Good reference: https://solarianprogrammer.com/2011/10/16/cpp-11-raw-strings-literals-tutorial/

namespace bdAPI { class BoDucBonLivraisonAlgorithm; }
namespace bdAPI 
{
  // deprecated, it has been refactored
	class BoDucApp 
	{
	public:
		// type of file (command)  
		enum class eFileType 
		{
			csv=0, 
			pdf=1
		};

		// alias for vector  
		using vecofstr = std::vector<std::string>;
		using mapIntVecstr = std::map<int, vecofstr>;
		using diff_type = std::iterator_traits<vecofstr::iterator>::difference_type;

	public:
		//  virtual ctor, 
		BoDucApp( const std::vector<std::string>& aVecOfilePath);
		BoDucApp( const std::string& aFile = "request-2.csv");
		BoDucApp( const BoDucApp&) = delete;
		BoDucApp& operator= (const BoDucApp&) = delete;

		void setWorkingDir( const std::string& aWorkPath = R"(C:\JeanWorks\BoDuc\Data\190617\)")
		{
			m_workDir = aWorkPath;
		}
		const std::string& getWorkingDir() const { return m_workDir; }
		void saveFile( const std::string& aFile2Save) {/*not implemented yet*/}
//		void countNbCmd();
		void setNbSelectedFiles(unsigned short aNbFilesSelectd) { m_userSelectF = aNbFilesSelectd; }
		unsigned short getNbSelectedFiles() const { return m_userSelectF; }
		void setPrintOut() {/*not implemented yet*/ }
		void setAParseAlgrithm( BoDucBonLivraisonAlgorithm* aAlgo2Set)
		{
			m_bdParseAlgorithm = aAlgo2Set;
		}
		// return the map container for the all command to be processed
		inline const mapIntVecstr readedMap()
		{
			return std::map<int, std::vector<std::string>>(m_mapintVec.cbegin(), m_mapintVec.cend());
		}

		// return data that was processed
		std::vector<BoDucFields> getReportData() 
		{ return std::vector<BoDucFields>(m_reportData.cbegin(),m_reportData.cend()); }

    // return the number of command to process
	//	virtual void process();
		
		// parse and fill data structure 
		virtual void readFile( const std::string& aFileAnPath,
		                       const std::string& aSplitBill = "Ordered on");

		virtual void readFiles( const std::list<std::string>& aFilesNameWithPath,
			const std::string& aSplitBill = "Ordered on");

		// Use the BoDuc writer for report formatting (deprecated)
		virtual void createReport( const std::string& aBonDeLivraison = "BonDeLivraison.txt");

		// return the number of command that is being processed
		size_t nbOfCmd( /*const std::string& aCmdFile*/);
		size_t getNbCmdForThisFile( const std::string& aCmdFile);

		void setFileType(eFileType aFileExt) { m_fileExt = aFileExt; }
		eFileType getFileExt() const { return m_fileExt; }

	private:
		std::vector<std::string> m_vecfilePath;
		std::string m_fileName;
		std::string m_workDir;
		std::string m_bonLivraison;
		BoDucFields m_readPrm;
		mapIntVecstr m_mapintVec;
		std::map<std::string, size_t> m_nbOfCmdInFile; 
		std::vector<mapIntVecstr> m_vecOfMap; // multiple file selection support
		std::vector<BoDucFields> m_reportData;
		BoDucBonLivraisonAlgorithm* m_bdParseAlgorithm;
		unsigned short m_userSelectF;
		eFileType m_fileExt;
	};
} // End of namespace
