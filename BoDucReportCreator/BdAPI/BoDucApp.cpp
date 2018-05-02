// C++ includes
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <stdlib.h>
// boost includes
#include <boost/algorithm/string.hpp> // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
// Qt includes
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
// app. includes
#include "BoDucApp.h"
#include "BoDucParser.h"
#include "VictoBonLivraison.h"
#include "PdfMinerAlgo.h"
#include "BoDucUtility.h"

// static declaration
namespace { // check if it is a valid character
	bool charPredicate(char c)
	{
		return !(isalpha(c) || c == '_');
	}
}

namespace bdAPI 
{
	BoDucApp::BoDucApp( const std::vector<std::string>& aVecOfilePath)
	: m_fileName(""),
		m_workDir(R"(C:\JeanWorks\BoDuc\ReadAndSor\DataToTest\)"),
		m_bonLivraison("BonDeLivraison.txt"),
		m_vecfilePath(aVecOfilePath),
		m_readPrm(), // BoDuc field
		m_bdParseAlgorithm(nullptr)
	{
	}

	BoDucApp::BoDucApp( const std::string& aFile /*= "request-2.csv"*/)
	: m_fileName(aFile),
		m_workDir(""),
		m_bonLivraison("BonDeLivraison.txt"),
		m_readPrm(), // BoDuc field
		m_bdParseAlgorithm(nullptr)
	{
		std::cout << "Reading the following file: " << m_workDir + m_fileName << "\n";
	}

	void BoDucApp::process()
	{
		using namespace std;
		using BoDucDataAlgo = unique_ptr<BoDucBonLivraisonAlgorithm>;
		
		// parser (How to parse the BoDuc command)
		BoDucParser w_boducParseAlgo;

		// create the algorithm to parse 
		// Design Note: 
		// must be done outside of this method, this way we can set it from GUI user interface
    // user may want to use another algorithm for some reason, for now "VictoReader" seem  
		// to work in most of cases, but it may change in the future.
		BoDucDataAlgo w_bonLivraison = nullptr;  //make_unique<VictoBonLivraison>();
		if( eFileType::csv==getFileExt())
		{
			w_bonLivraison.reset( new VictoBonLivraison());
		}
		else if( eFileType::pdf == getFileExt())
		{
			w_bonLivraison.reset( new PdfMinerAlgo());
		}
		else
		{
			return; // or set a default algo
		}

		auto w_filesInUse = getNbSelectedFiles();
		
		//TODO: need to change the signature
		if (w_filesInUse==1)
		{
			// retrieve values 
			w_boducParseAlgo.extractData(m_mapintVec, w_bonLivraison.get());
	//		w_bonLivraison->getBoDucStruct(m_reportData);
		}
		else
		{
			// support to multiple files selection (vector of map)
			for( mapIntVecstr w_mapofcmd : m_vecOfMap)
			{
				// retrieve values 
				w_boducParseAlgo.extractData(w_mapofcmd, w_bonLivraison.get());

				// retrieve BoDuc fields
				//std::vector<BoDucFields> w_testFields;
		//		w_bonLivraison->getBoDucStruct(m_reportData);
			}
		}

		// TODO: Design Note
		// shall be call outside the if-else to avoid to copy over and over
		// only to do it once, algorithm instance takes care of pilling up
		w_bonLivraison->getBoDucStruct(m_reportData);

		// new version (copy vector)
		//m_reportData = w_boducParseAlgo.getBdFields();

		// application stay open and you read files, BoDucApp never destroy
		// then all member never get destroy or empty. Need to force empty.
		if( !m_vecOfMap.empty())
		{
			m_vecOfMap.clear();
		}
	}

	void BoDucApp::readFile( const std::string& aFileAnPath, const std::string& aSplitBill)
	{
		using namespace std;
		using namespace boost;
		using namespace boost::algorithm;

		// create alias
		using vecofstr = std::vector<std::string>;
		
		// lambda (anonymous) function declaration
		auto checkTransportName = [](const std::string& aStr2Look) -> bool
		{
			// Transporteur name (BoDuc)
			return (contains(aStr2Look, "NIR R-117971-3 TRSP CPB INC")
				|| contains(aStr2Look, "NIR R-117971-3 TRANSPORT CPB")
				|| contains(aStr2Look, "NIR R-117971-3 C.P.B.")
				|| contains(aStr2Look, "BODUC- ST-DAMASE")
				|| contains(aStr2Look, "NIR R-004489-2 TR. BO-DUC"));     //sometime we have empty (blank field) string
		};

		if (!m_fileName.empty())
		{
			m_fileName.clear();
		}

		// check for accent in file name, if so fix it
		if (std::find_if(aFileAnPath.cbegin(), aFileAnPath.cend(), charPredicate) != aFileAnPath.cend())
		{
			// we have special character, need to fix name without accent
			std::wstring w_utf8FileName = BoDucUtility::FromUtf8ToUtf16(aFileAnPath);
			m_fileName.assign(w_utf8FileName.cbegin(), w_utf8FileName.cend());
		}
		else
		{
			// csv file that we are processing
			m_fileName.assign(aFileAnPath.cbegin(), aFileAnPath.cend());
		}

		// declare vector to store our string from csv document 
		vecofstr w_vecStr;
		w_vecStr.reserve(50); // reserve enough space for storing each line ??

		// first time we go through we fill the map
		// but next we go in (didn't close the app),
		// we must empty the container ... i do not understand
		if (!m_mapintVec.empty())
		{
			// clear all content
			m_mapintVec.clear();

			// sanity check
			if (m_mapintVec.size() != 0)
			{
				std::cerr << "Map of lines not empty\n";
			}
		}

		// Command in one file, reading the command by splitting with the "Ordered on"
		short i(0); // set to zero as default, otherwise set to whatever comes up
		ifstream w_readVSV(m_fileName.c_str());
		if (w_readVSV) // check if its open, then read line-by-line 
		{
			for (string line; std::getline(w_readVSV, line);)
			{
				if (i == 0) // first line is obsolete
				{
					// check if contains "Ordered on" as first line
					// NOTE "request-2.csv" we split the bill with this token, 
					// and format was that first line contains this string
					if (contains(line, "Ordered on"))
					{
						++i;  // next line
						continue; // i don't see why we should increment it
					}
				}//if(i==0)
				w_vecStr.push_back(line);

				// NOTE we assume that we are at the last line of the command
				// then we check if the carrier string name is part of the whole 
				// command (if so, add it to the map otherwise skip it)
				// IMPORTANT this algorithm assume that we are at the end or the 
				// last line (split into separate command is based on this assumption)
				// if not the case then it won't work!!
				if (contains(line, aSplitBill))
				{
					// check for carrier name some are different and don't need to be treated
					// Francois mentioned that field can be blank, need to be careful
					if (any_of(w_vecStr.cbegin(), w_vecStr.cend(), checkTransportName))
					{
						m_mapintVec.insert(make_pair(i++, w_vecStr));
						w_vecStr.clear();
					}
					else
					{
						QMessageBox w_msgBname;
						QFileInfo w_filExtract(aFileAnPath.c_str());
						const QString w_fileName = w_filExtract.fileName();
						QString w_strMsg = QString("Not a valid transporteur name in ") + w_fileName;
						w_msgBname.setText(w_strMsg);
						w_msgBname.setIcon(QMessageBox::Warning);
						w_msgBname.exec();
						// ready for the next iteration
						w_vecStr.clear();
					}
				}
			}//for-loop
		}//if
		else // file could not be opened because name invalid or something else
		{
			QMessageBox msgBox;
			QFileInfo w_filExtract(aFileAnPath.c_str());
			const QString w_fileName = w_filExtract.fileName();
			QString w_strMsg = QString("The document ") + w_fileName + QString(" could not be opened.");
			msgBox.setText(w_strMsg);
			msgBox.setInformativeText("We try to fix it, do you want to proceed?");
			msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			msgBox.setIcon(QMessageBox::Warning);
			int ret = msgBox.exec();
		}
	}

	// Reading command file and fill the vector of string for each command.
	// Added to a map to it is used  
	void BoDucApp::readFiles( const std::list<std::string>& aFilesNameWithPath, const std::string& aSplitBill)
	{
		// Design Note:
		//  We will need to add support to pdf files conversion. For now, 
		//  we launch the python script for each file and produce a txt file. 
		//  Steps
		//  check files extension (must be all the same type) 
		//  if pdf file, create a temporary list of the new converted files 
		//  once its done process them 
		//  
		auto w_begListIter = aFilesNameWithPath.cbegin();
		while( w_begListIter != aFilesNameWithPath.cend())
		{
			const std::string& w_fileName = *w_begListIter;
			// call readFile
			readFile( w_fileName, std::string("Signature"));

			// check for number of command to proceed
			size_t w_numCmd = nbOfCmd();
			// check if it exist
			if( m_nbOfCmdInFile.find(w_fileName) != m_nbOfCmdInFile.cend())
			{
				continue; // not sure about this one!!
			}
			m_nbOfCmdInFile.insert( std::make_pair(*w_begListIter,w_numCmd));

			// push m_mapIntVec into vector
			// vector provide a push_back() that support the move semantic  
			// since we don't need the content of the map for next iteration
			// might as well to move its content, is that make sense?
			// don't copy something that i am not going to use 
			m_vecOfMap.push_back( std::move(m_mapintVec)); // can i do that? why not

			// sanity check
			assert(0 == m_mapintVec.size()); // according to the move semantic
			++w_begListIter; // next in the list
		}

		// now supporting multiple file selection from user
	}

	// deprecated
	void BoDucApp::createReport( const std::string & aBonDeLivraison)
	{
		// do we need to set the path for saving result
	//	BoDucWriter w_bonLivraison; next version
		// what we need is to check if file already exist
		// if the file exist then we re-open and append at the end
		// if it doesn't exist, then create a new file and open it for writing

		std::ofstream w_bonLivraison(aBonDeLivraison/*aOutputFile*/, std::ios::out);
    //assert(w_bonLivraison.is_open());
#if 1
		// write title or header
		w_bonLivraison << "No Command " << "\t" << "Shipped To " << "\t" << "Deliver Date " << "\t" <<
			"Product " << "\t" << "Quantity " << "\t" << "Silo" << "\n";

		for( const BoDucFields& w_bfield : m_reportData)
		{
				w_bonLivraison << w_bfield.m_noCmd << "\t" << w_bfield.m_deliverTo << "\t" << w_bfield.m_datePromise
					<< "\t" << w_bfield.m_produit << "\t" << w_bfield.m_qty << "\t" << w_bfield.m_silo << "\n";
		}
#endif

		// IMPORTANT 
		//  
	}

	size_t BoDucApp::nbOfCmd( /*const std::string& aCmdFile*/)
	{
		size_t w_nbCmd = 0;
		std::ifstream w_readCSV(m_fileName.c_str()/*aCmdFile.c_str()*/);
		if (w_readCSV) // check if its open, then read line-by-line 
		{
			for (std::string line; std::getline(w_readCSV, line);)
			{
				// check if contains "Signature"
				if (boost::algorithm::contains(line, std::string("Signature")))
				{
					++w_nbCmd;
				}
			}//for-loop
		}

		return w_nbCmd;
	}

	size_t BoDucApp::getNbCmdForThisFile( const std::string & aCmdFile)
	{
		// use the find algorithm of the map to retrieve the number
		// of command with key value the name of the file
		return 0; // debugging purpose
	}

#if 0
	void BoDucParsingAlgo::readFile(const std::string& aFileAnPath, const std::string& aSplitBill)
	{
		using namespace std;
		using namespace boost;
		using namespace boost::algorithm;

		// create alias
		using vecofstr = std::vector<std::string>;

		// declare vector to store our string from cvs document 
		vecofstr w_vecStr;
		w_vecStr.reserve(50); // reserve enough space for storing each line ??

    // Command in one file, reading the command by splitting with the "Ordered on"
		short i(0);
		ifstream w_readVSV(aFileAnPath.c_str());
		if (w_readVSV) // check if its open, then read line-by-line 
		{
			for (string line; std::getline(w_readVSV, line);)
			{
				if (i == 0) // first line is obsolete
				{
					// check if contains "Ordered on" as first line
					// NOTE "request-2.csv" we split the bill with this token, 
					// and format was that first line contains this string
					if (contains(line, "Ordered on"))
					{
						++i;  // next line
						continue; // i don't see why we should increment it
					}
				}//if(i==0)
				w_vecStr.push_back(line);

				// NOTE we assume that we are at the last line of the command
				// then we check if the carrier string name is part of the whole 
				// command (if so, add it to the map otherwise skip it)
				// IMPORTANT this algorithm assume that we are at the end or the 
				// last line (split into separate command is based on this assumption)
				// if not the case then it won't work!!
				if (contains(line, aSplitBill))
				{
					// check for carrier name some are different and don't need to be treated
					// Francois mentioned that field can be blank, need to be careful
					// Also, we must check also for the "TM" tonne metrique, if not present
					// the cmd is discarded
					if (any_of(w_vecStr.cbegin(), w_vecStr.cend(),
						[](const std::string& aStr2Look) -> bool     // lambda (anonymous) function
					{
						return (contains(aStr2Look, "NIR R-117971-3 TRSP CPB INC")
							|| contains(aStr2Look, "NIR R-117971-3 TRANSPORT CPB")); // sometime we have empty (blank field) string
					}
					)//any_of
						)//if
					{
						m_mapintVec.insert(make_pair(i++, w_vecStr));
						w_vecStr.clear();
					}
					else
					{
						/*w_mapofCmd.insert(std::make_pair(i++,w_vecStr));*/
						w_vecStr.clear();
					}
				}
			}//for-loop
		}//if
	}
#endif
} // End of namespace

