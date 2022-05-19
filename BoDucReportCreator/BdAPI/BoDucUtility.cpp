// C++ includes
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <vector>
// boost includes
#include <boost/lambda/if.hpp>    //??not sure if need it
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/algorithm/string.hpp>       // string algorithm
#include <boost/algorithm/string/split.hpp> // splitting algo
#include  <boost/algorithm/string/predicate.hpp>
// Qt includes
#include <QVector>
#include <QFileInfo>
// App includes
#include "BoDucUtility.h"
#include "AddressAlgorithm.h"
#include "AddressParser.h"
#include "AddressBuilder.h"

using namespace bdAPI;

namespace {

  // Just some helpers to read file with data separated tab 
  void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s); // split to white space (see Bjarne Stroutstrup book "C++ Language And Practice")
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
  }
}// End of namespace anonymous

std::vector<std::string> BoDucUtility::TrimRightCommaAndErase( const std::string & aStr2Trim)
{
	using namespace std;
	using namespace boost;

	std::string w_str2Trim(aStr2Trim.cbegin(), aStr2Trim.cend());
	trim_right_if(w_str2Trim, is_any_of(","));
	
	// splitting string to retrieve COxxxxx cmd no
	vector<string> strs;
	split(strs, w_str2Trim, is_any_of(","));

	// not sure about this one, after this we should have a vector of 2 elements
	// because our search is based on a line that contains a pair (Date,No)
	strs.erase( remove_if(strs.begin(), strs.end(), // delete empty element
		[](const string& s)
	{ return s.empty(); }), strs.cend());

	return strs;
}

bool bdAPI::BoDucUtility::isPostalCode(const std::string & aAddress)
{
		// split with token = ,
		// take last element of split vector and count 
		// if alpha and if digit by using the count count_if
		// stl algorithm. If both count == 3 we have a postal code

		std::locale loc;
		auto w_nbAlpha = std::count_if( aAddress.cbegin(), aAddress.cend(),
			[loc](unsigned char c) { return std::isalpha(c, loc); }
		);

		auto w_nbDigit = std::count_if(aAddress.cbegin(), aAddress.cend(),
			[loc](unsigned char c) { return std::isdigit(c, loc); }
		);
		// postal code has same number of digit and character
		if (w_nbAlpha == w_nbDigit)
		{
			return true;
		}
		else
		{
			size_t w_pos = aAddress.find_first_not_of(' ');
			// check last 3 characters (Francois Ethier corrupted postal code)
			auto begCode = aAddress.cbegin();
			if (w_pos == 1)
			{
				std::advance(begCode, 4);// because blank character at the beginning
			}
			else
			{
				std::advance(begCode, 3);// because blank character at the beginning
			}
			w_nbDigit = std::count_if(begCode, aAddress.cend(),
				[loc](unsigned char c) { return std::isdigit(c, loc); }
			);
			if (w_nbDigit == 2)
			{
				return true;
			}
		}
		return false;
}

QStringList BoDucUtility::convertPdf2Txt( const QStringList& aListOfFiles, const QString& aPythonPath, /*const QString& aPdfPath,*/
                                          QProcessEnvironment env, QObject* aQprocess)
{
  // convert to txt file
//   const QString pythonScript = R"(F:\EllignoContract\BoDuc\pdfminerTxt\pdfminer-20140328\build\scripts-2.7\pdf2txt.py)";
//   const QString w_pdfilesPath = R"(F:\EllignoContract\BoDuc\QtTestGui\BoDucReportCreator\BoDucReportCreator\Data)";  //pdf files folder
//                                                                                                                      
  //const QString w_pdfilesPath = R"(F:\EllignoContract\BoDuc\QtTestGui\BoDucReportCreator\BoDucReportCreator\Data\DataValidation)";  debugging purpose
  //     QDir w_txtFilesFolder;
  //     QString w_pathNow = QDir::current().absolutePath();
  //     w_txtFilesFolder.setPath(w_pathNow);
  //     QString w_repDirName = w_txtFilesFolder.dirName();
  // directory validation
  //if( QDir::exists(w_pdfilesPath)) {}

  QFileInfo w_wkrDir(aListOfFiles.front());
  QString w_absPdfPath = w_wkrDir.absolutePath();
//   QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
//   env.insert("PYTHONPATH", "C:\\Python27\\Lib");
//   env.insert("PYTHONHOME", "C:\\Python27");
   QProcess w_process(aQprocess);
   w_process.setProcessEnvironment(env);
   // Sets the working directory to dir. QProcess will start the process in this directory.
   // The default behavior is to start the process in the working directory of the calling process.
   //w_process.setWorkingDirectory(aPdfPath);  otherwise set path working dir of app
   w_process.setWorkingDirectory(w_absPdfPath);
  
   QStringList w_listofTxtFiles;
  // number of files selected by user
  QStringListIterator filesListIterator(aListOfFiles);
  while (filesListIterator.hasNext())
  {
    // String list contains the whole path
    QFileInfo w_fileInfo(filesListIterator.next());
    QString w_fname = w_fileInfo.fileName();
    QString w_bname = w_fileInfo.baseName();
    QString w_absPath = w_fileInfo.absoluteFilePath();
    QString w_complPdfFile = w_fname; // filename with corresponding extension
    QString w_complTxtFile = w_bname + ".txt";
    // Add a space at beginning of the file name 
    QString w_txtPath = w_fileInfo.canonicalPath() + R"(/ )" + w_complTxtFile;
  //  QString w_txtPath = aPdfPath + R"(/ )" + w_complTxtFile;
    w_listofTxtFiles.push_back(w_txtPath);

    QString w_ofile("-o ");
    QStringList params;
    //w_process.start("Python", params);
    //			QStringList params;
    //std::cout << filesListIterator.next().constData() << std::endl;
    params << aPythonPath << w_ofile + w_complTxtFile << w_complPdfFile;

    w_process.start("Python", params);
    if( w_process.waitForFinished(-1))
    {
      QByteArray p_stdout = w_process.readAll();
      QByteArray p_stderr = w_process.readAllStandardError();
      if (!p_stderr.isEmpty())
        std::cout << "Python error:" << p_stderr.data();

      //		qDebug() << "Python result=" << p_stdout;
      if (!p_stdout.isEmpty())
      {
        std::cout << "Python conversion:" << p_stdout.data();
      }
      p_stdout; // write to console
    }
    // kill process
    w_process.close();
  }//while-loop
  // swap content, we are interested by .txt files (parsing) 
  //w_listofTxtFiles.swap(m_filesName);
  return w_listofTxtFiles;
  // 		QDir w_checkFile(w_defaultDir);
  // 		w_checkFile.cd("Data");
  // 		QStringList w_dirList = w_checkFile.entryList();
  // 		auto w_szz = w_dirList.size();
  // 		for( const auto& w_fileName : w_dirList)
  // 		{
  // 			std::string w_str = w_fileName.toStdString();
  // 			if( std::isspace(static_cast<char>(w_str[0])))
  // 			{
  // 				std::string w_noSpace(std::next(w_str.cbegin()), w_str.cend());
  // 				QString w_newName(w_noSpace.c_str());
  // 				w_checkFile.rename(w_fileName,QString("sas.txt"));
  // 			}
  // 		}

  // Design Note
  //  we need to change the extension of pdf file to be able to use our algorithm
  //  for each of the file name in the list, retrieve extension 
}

std::string BoDucUtility::AddressFixAlgorithm(const std::vector<std::string> &w_vecOfAddressPart)
{
	using namespace bdAPI;

	std::string w_complAddrs; //whole address
														// starting to parse file
	AddressParser w_testParser(w_vecOfAddressPart); // analyze the address for malfunction
	if (w_testParser.getPattern() == AddressParser::ePattern::NoMalfunction)
	{
		// no point to go further, just return the vec address part as it is
		AddressBuilder w_adrs2Build(w_vecOfAddressPart);
		w_complAddrs = w_adrs2Build.buildAddess();
		return w_complAddrs;
	}
	if (w_testParser.getPattern() == AddressParser::ePattern::Livraison)
	{
		// call the fix algorithm for this pattern, no point to go further
		// once LIVRAISON is removed for the whole address, usually we have 
		// to extract the lines below and take the first line as client name
		// Need to check but we already do that 
		std::vector<std::string> w_adrsFixed =
			AddressAlgorithm::fixLivraisonInserted(w_testParser);

		// build the whole address
		AddressBuilder w_addrsBuilder(w_adrsFixed);
		w_complAddrs = w_addrsBuilder.buildAddess();
		std::cout << "The complete address is: " << w_complAddrs.c_str() << "\n";
		return w_complAddrs;
	}
	if (w_testParser.hasSymmetry())
	{
		std::vector<std::string> w_fixedAddress;
		if (w_testParser.hasMalfunctions())
		{
			AddressParser::ePattern w_checkPattern = w_testParser.getPattern();
			if (w_checkPattern == AddressParser::ePattern::DuplAddressSymmetric)
			{
				// call address malfunction algorithm
				w_fixedAddress = AddressAlgorithm::fixSymetryDuplAddress(w_testParser);

				// build the whole address
				AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::DuplAddressNonSymmetric)
			{ // call address malfunction algorithm
				w_fixedAddress =
					AddressAlgorithm::fixNonSymetryDuplAddress(w_testParser.getNbLines(),
						w_testParser.getVecPart());

				// build the whole address
				AddressBuilder w_addrsBuilder(w_fixedAddress);
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::LastLineSrewUp)
			{
				std::vector<std::string> w_splitLine = AddressAlgorithm::splitAboutComma(w_vecOfAddressPart.back());
				if (std::any_of(w_splitLine.cbegin(), w_splitLine.cend(), [](const std::string& aStr) {return aStr.empty(); }))
				{
					w_splitLine.erase(std::unique(w_splitLine.begin(), w_splitLine.end()), w_splitLine.cend()); // white space
					w_splitLine.pop_back(); // erase last character which is  
				}

				//boost::trim_right_if(w_splitLine);
				assert(5 == w_splitLine.size());
				if (!w_splitLine.empty())
				{
					// we need to split about comma and then pass to algorithm
					w_fixedAddress = AddressAlgorithm::fixLastLinesCityDupl(w_splitLine);
					const std::string& w_val2insert = *w_testParser.getVecPart().cbegin();
					w_fixedAddress.insert(w_fixedAddress.cbegin(), w_val2insert); // insert at the beginning (client name)
					AddressBuilder w_adrs2Build(w_fixedAddress);
					w_complAddrs = w_adrs2Build.buildAddess();
				}
				else if (w_checkPattern == AddressParser::ePattern::FirstLineWrong)
				{
					// this case happen when we have 2-lines 
					if (w_testParser.getVecPart().size() == 2)
					{
						w_complAddrs = w_testParser.getVecPart().back();
					}
				}
			}
		}//end malfunction
		std::cout << "Complete address is: " << w_complAddrs.c_str() << "\n";
	}//end symmetry
	else
	{
		std::vector<std::string> w_fixedAddress;
		if (w_testParser.hasMalfunctions() && w_testParser.getNbLines() == 3)
		{
			// 				if( w_testParser.getNbLines()==3)
			// 				{
			AddressParser::ePattern w_checkPattern = w_testParser.getPattern();
			if (w_checkPattern == AddressParser::ePattern::Middle3Lines)
			{
				w_fixedAddress =
					AddressAlgorithm::fixThreeLinesAddress(w_testParser);
				// move semantic
				AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::LastLineSrewUp)
			{
				w_fixedAddress =
					AddressAlgorithm::fixThreeLinesAddress(w_testParser);
				const std::string& w_beforeLastOne = w_testParser.getVecPart()[1];
				w_fixedAddress.insert(w_fixedAddress.cbegin(), w_beforeLastOne); // insert at the beginning (street)
				const std::string& w_firstOne = *w_testParser.getVecPart().cbegin();
				w_fixedAddress.insert(w_fixedAddress.cbegin(), w_firstOne); // insert at the beginning (client name)
																																		// move semantic
				AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
				w_complAddrs = w_addrsBuilder.buildAddess();
			}
			else if (w_checkPattern == AddressParser::ePattern::FirstLineWrong)
			{
				// this case happen when we have 3-lines, take last 2 lines 
				if (w_testParser.getVecPart().size() == 3)
				{
					w_fixedAddress.push_back(w_testParser.getVecPart()[1]);
					w_fixedAddress.push_back(w_testParser.getVecPart()[2]);
					// move semantic
					AddressBuilder w_addrsBuilder(std::vector<std::string>(w_fixedAddress.cbegin(), w_fixedAddress.cend()));
					w_complAddrs = w_addrsBuilder.buildAddess();
				}
			}
		}//if
		std::cout << "The complete address is: " << w_complAddrs.c_str() << "\n";
	} //else

	return w_complAddrs;
}

// By using QTextStream i am not sure if we need it (see documentation of QTextSTream)
// QString mange the utf-8 string codec character (we have to test)
// This utility
std::wstring BoDucUtility::ConvertFromUtf8ToUtf16( const std::string& str)
{
	std::wstring convertedString;
	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 0, 0);
	if (requiredSize > 0)
	{
		std::vector<wchar_t> buffer(requiredSize);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &buffer[0], requiredSize);
		convertedString.assign(buffer.begin(), buffer.end() - 1);
	}

	return convertedString;
}

std::wstring BoDucUtility::FromUtf8ToUtf16(const std::string & str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> cwuf8;
	std::wstring w_strConverted = cwuf8.from_bytes(str);
	return w_strConverted;
}

std::vector<BoDucFields> BoDucUtility::loadCmdReportInVector( const QString & aFilepath)
{
  std::ifstream infile(aFilepath.toStdString().c_str());
  // check if it is open
  if( !infile) 
  {
    // Should use a log file
    std::cerr << "Could not open file for reading\n";
  }

  std::vector<bdAPI::BoDucFields> w_vecBD;
  w_vecBD.reserve(1000); // debugging purpose
  std::string line;
  std::vector<std::string> row_values;
  auto lineCounter = 0;
  while( std::getline(infile, line))
  {
    // header column name want to skip that
    if (lineCounter == 0) { lineCounter += 1; continue; }
    if (line.empty()) { continue; } // don't treat empty line
                                    // ...
    if (!row_values.empty())
    {
      row_values.clear();
    }

    // BoDucField (number of fields)
    row_values.reserve(7); 

    // values separated by tab, not csv yet
    split(line, '\t', row_values);

    // use the mode semantic of the vector
    long w_prodCode = ::atoi(row_values[3].c_str());
    float w_qty = ::atof(row_values[5].c_str());

    w_vecBD.push_back( bdAPI::BoDucFields( std::make_tuple(row_values[0], row_values[1], row_values[2],
      w_prodCode, row_values[4], w_qty, row_values[6])));
  }

  if( w_vecBD.capacity() > w_vecBD.size())
  {
    w_vecBD.shrink_to_fit();
  }

  if( infile.is_open())
  {
    infile.close();
  }

  return w_vecBD;
}

QList<QVector<QVariant>> BoDucUtility::fromBDFieldToQVariant( const std::vector<bdAPI::BoDucFields>& abdF)
{
  QVector<QVariant> w_vec2Fill;
  QList<QVector<QVariant>> w_listOfVariant;
  w_vec2Fill.reserve(7); // number of fields

  auto lineCounter = 0;
  if( !w_vec2Fill.empty())
  {
    w_vec2Fill.clear();
  }

  auto i = 0;
  // always const and ref to avoid copy
  for( const auto& v : abdF)
  {
    w_vec2Fill.push_back(QVariant(v.m_noCmd.c_str())); //0
    w_vec2Fill.push_back(QVariant(v.m_deliverTo.c_str())); //1
    w_vec2Fill.push_back(QVariant(v.m_datePromise.c_str())); //2
    w_vec2Fill.push_back(QVariant(v.m_prodCode));
    w_vec2Fill.push_back(QVariant(v.m_produit.c_str()));//4
    w_vec2Fill.push_back(QVariant(v.m_qty));
    w_vec2Fill.push_back(QVariant(v.m_silo.c_str())); //6

#if 0
    if (i == 3) // product code
    {
      //				int w_int = atoi(v.m_prodCode.c_str());
      w_vec2Fill.push_back(QVariant(v.m_prodCode));
    }
    else if (i == 5) // qty
    {
      // double value
      //double w_dbl = atof(v.c_str());
      w_vec2Fill.push_back(QVariant(v.m_qty));
    }
    // 				else if (i == 6) //silo number
    // 				{
    // 					//int w_int = atoi(v.c_str());
    // 					w_vec2Fill.push_back(QVariant(v.m_silo.c_str()));
    // 				}
    else
    {
      switch (i)
      {
      case 0:
        w_vec2Fill.push_back(QVariant(v.m_noCmd.c_str())); //0
        break;
      case 1:
        w_vec2Fill.push_back(QVariant(v.m_deliverTo.c_str())); //1
        break;
      case 2:
        w_vec2Fill.push_back(QVariant(v.m_datePromise.c_str())); //2
        break;
      case 4:
        w_vec2Fill.push_back(QVariant(v.m_produit.c_str()));//4
        break;
      case 6:
        w_vec2Fill.push_back(QVariant(v.m_silo.c_str())); //6
        break;
      default:
        break;
      }
      //w_vec2Fill.push_back(QVariant(v.m_noCmd.c_str())); //0
      //w_vec2Fill.push_back(QVariant(v.m_deliverTo.c_str())); //1
      //w_vec2Fill.push_back(QVariant(v.m_datePromise.c_str())); //2
      //w_vec2Fill.push_back(QVariant(v.m_produit.c_str()));//4
      //w_vec2Fill.push_back(QVariant(v.m_silo.c_str())); //6
    }
    ++i;
#endif
    w_listOfVariant.push_back(w_vec2Fill);
    w_vec2Fill.clear();
  }//for-loop

  return w_listOfVariant;
}

// may return move semantic
std::vector<BoDucFields> BoDucUtility::remDuplicateAndSort( const std::vector<BoDucFields>& aVecTotrim)
{
  // vector to be sorted
  std::vector<BoDucFields> w_vecSorted( aVecTotrim.cbegin(), aVecTotrim.cend());

  // need to check if we have duplicate (compare command number)
  // first sort by the command No, once sorted duplicate element 
  // will be adjacent to each other 
  std::sort( w_vecSorted.begin(), w_vecSorted.end(),
    []( BoDucFields& aField1, BoDucFields& aField2) -> bool
  {
    return std::greater<std::string>().operator()(aField1.m_noCmd, aField2.m_noCmd);
  });

  // remove all duplicate (since all element are sorted), we can make unique
  // by removing adjacent element ...
  w_vecSorted.erase( std::unique(w_vecSorted.begin(), w_vecSorted.end()), w_vecSorted.cend());

  // trimming data before displaying in the GUI
  std::for_each(w_vecSorted.begin(), w_vecSorted.end(),
    []( BoDucFields& aBdField)
  {
    // Sorting dates represented as a string of the form YYYYMMDD 
    // or an integer whose decimal representation is YYYYMMDD is trivial. 
    // Just use std::sort and you will get the correct sort order.
    // First remove the back slash
    boost::algorithm::replace_all(aBdField.m_datePromise, R"(/)", "");
  });

#if _DEBUG
  std::cout << "Back slash removed\n";
  for( const auto& val : aVecTotrim)
  {
    std::cout << val;
  }
#endif
  // ready to sort by date since we have only unique command
  std::cout << "We are ready to sort our vector\n";
  std::sort( w_vecSorted.begin(), w_vecSorted.end());           // ascending order less recent to more recent
                                                               // need to reverse the order of the vector (descending order)
  std::reverse( std::begin(w_vecSorted), std::end(w_vecSorted)); // reverse the order more recent to less 

  // now we need to put back slash back
  auto putItBack = [](bdAPI::BoDucFields& aStr) // lambda
  {
    aStr.m_datePromise = aStr.m_datePromise.substr(0, 4) + R"(/)" +
    aStr.m_datePromise.substr(4, 2) + R"(/)" +
    aStr.m_datePromise.substr(6, 2);
  };
  std::for_each(w_vecSorted.begin(), w_vecSorted.end(), putItBack);
#if _DEBUG
  std::for_each(w_vecSorted.cbegin(), w_vecSorted.cend(), std::cout << boost::lambda::_1 << "\n");
#endif

  return w_vecSorted;
}
