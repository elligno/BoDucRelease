// C++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
// Qt includes
#include <QtWidgets/QWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHeaderView>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QTextEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QProcess>
#include <QLabel>
#include <QTextStream>
#include <QDateTime>
#include <QDirIterator>
#include <QFileDialog>
#include <QFont>
#include <QCheckBox>
// Boost includes
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
// test includes
// Application include
#include "BoDucReportCreator.h"
#include "BdAPI/BoDucFields.h"
#include "BdAPI/BoDucUtility.h"

namespace {
	constexpr bool greater_than( const bdAPI::BoDucFields& aField, const bdAPI::BoDucFields& aField2)
	{
		return aField.m_qty > aField2.m_qty;
	}

	// Just some helpers to read file with data separated tab 
	void split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss;
		ss.str(s); // split to white space (see Bjarne Stroutstrup book "C++ Language And Practice")
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
	}

	// TODO: deprecated
	// Read each line of the command file
	QList<QVector<QVariant>> readFileTab( const QString& aFilepath)
	{
		using namespace std;
		QList<QVector<QVariant>> w_list2Ret;

		// raw string (to be completed)
		const std::string w_filePath = R"(F:\EllignoContract\BoDuc\BoDucDev\BoDucApp01\ReportFiles\)";
		std::string w_fileName = w_filePath + std::string("BonLivraison_201217_withAlgo.txt");
		std::ifstream infile(aFilepath.toStdString().c_str());
		std::string line;
		if (!infile)
		{
			std::cerr << "Could not open file for reading\n";
		}

		QVector<QVariant> w_vec2Fill;
		vector<string> row_values;
		w_vec2Fill.reserve(7); // number of fields

		auto lineCounter = 0;
		while (std::getline(infile, line))
		{
			// header column name want to skip that
			if( lineCounter == 0) { lineCounter += 1; continue; }
			if( line.empty())     { continue; } // don't treat empty line
			// ...
			if (!row_values.empty())
			{
				row_values.clear();
			}
			row_values.reserve(7); // number of fields

			split(line, '\t', row_values);
			if (!w_vec2Fill.empty())
			{
				w_vec2Fill.clear();
			}
			auto i = 0;
			// always const and ref to avoid copy
			for (const auto& v : row_values)
			{
				if (i == 3) // product code
				{
					int w_int = atoi(v.c_str());
					w_vec2Fill.push_back(QVariant(w_int));
				}
				else if (i == 5) // qty
				{
					// double value
					double w_dbl = atof(v.c_str());
					w_vec2Fill.push_back(QVariant(w_dbl));
				}
				else if (i == 6) //silo number
				{
					int w_int = atoi(v.c_str());
					w_vec2Fill.push_back(QVariant(w_int));
				}
				else
				{
					w_vec2Fill.push_back(QVariant(v.c_str()));
				}
				++i;
			}//for-loop
			w_list2Ret.push_back(w_vec2Fill);
		}//while-loop

		return w_list2Ret;
	}
	std::vector<bdAPI::BoDucFields> loadCmdReportInVector( const QString& aFilepath)
	{
		std::ifstream infile( aFilepath.toStdString().c_str());
		if (!infile)
		{
			std::cerr << "Could not open file for reading\n";
		}

		std::vector<bdAPI::BoDucFields> w_vecBD;
		w_vecBD.reserve(1000); // debugging purpose
		std::string line;
		std::vector<std::string> row_values;
		auto lineCounter = 0;
		while (std::getline(infile, line))
		{
			// header column name want to skip that
			if (lineCounter == 0) { lineCounter += 1; continue; }
			if (line.empty()) { continue; } // don't treat empty line
																			// ...
			if (!row_values.empty())
			{
				row_values.clear();
			}
			row_values.reserve(7); // number of fields

			split(line, '\t', row_values);

			// use the mode semantic of the vector
			long w_prodCode = atoi(row_values[3].c_str());
			float w_qty = atof(row_values[5].c_str());
			w_vecBD.push_back( bdAPI::BoDucFields( std::make_tuple(row_values[0], row_values[1], row_values[2],
				w_prodCode,row_values[4],w_qty,row_values[6])));
		}
		if (w_vecBD.capacity() > w_vecBD.size())
		{
			w_vecBD.shrink_to_fit();
		}
		return w_vecBD;
	}

	QList<QVector<QVariant>> fromBDFieldToQVariant( const std::vector<bdAPI::BoDucFields>& abdF)
	{
		QVector<QVariant> w_vec2Fill;
		QList<QVector<QVariant>> w_listOfVariant;
		w_vec2Fill.reserve(7); // number of fields

		auto lineCounter = 0;
		if (!w_vec2Fill.empty())
		{
			w_vec2Fill.clear();
		}
		auto i = 0;
		// always const and ref to avoid copy
		for (const auto& v : abdF)
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
}// End of namespace

namespace bdApp 
{
	BoDucReportCreator::BoDucReportCreator(QWidget *parent)
	: QWidget(parent),
		m_tblWidget(Q_NULLPTR),
		m_cmdBox(Q_NULLPTR),
		m_saveButton(Q_NULLPTR),
		m_closeButton(Q_NULLPTR),
		m_procButton(Q_NULLPTR), 
	  m_analyzeLoadButton(Q_NULLPTR),
	  m_openButton(Q_NULLPTR),
		m_listUnite(Q_NULLPTR),
		m_saveSelectBtn(Q_NULLPTR),
//		m_console(Q_NULLPTR),
		m_bonCreateReport(Q_NULLPTR),
		m_cptySelectBtn(Q_NULLPTR),
		m_bonLivraisonFile("CommandReport.txt"),
		m_displaySelect(eDisplayMode::all) // default
	{

	//	m_listUniteAvailable.push_back(QString("Unit 30"));
		//m_listUniteAvailable.push_back(QString("Unit 30"));
// 		m_listUniteAvailable.push_back(QString("Unit 33"));
// 		m_listUniteAvailable.push_back(QString("Unit 103"));
// 		m_listUniteAvailable.push_back(QString("Unit 110"));
// 		m_listUniteAvailable.push_back(QString("Unit 111"));
// 		m_listUniteAvailable.push_back(QString("Unit 112")); 
// 		m_listUniteAvailable.push_back(QString("Unit 114")); 
// 		m_listUniteAvailable.push_back(QString("Unit 115"));
// 		m_listUniteAvailable.push_back(QString("Unit 116"));
// 		m_listUniteAvailable.push_back(QString("Unit 117"));

		setWindowTitle("BdApp Bon De Livraison (creator)");

		// set display formating (Widget table) 
		setupViews();

		// create all components for the GUI
		createOtherWidgets();
		createLayout();
		createConnections();

		// fill all values load capacity for each unit
		unitLoadConfig();

		// set config of the progress bar 
		initProgressBar();

		// ...
		initMapUserSelection();

		// create the user interface
		ui.setupUi(this);
	}

	void BoDucReportCreator::initMapUserSelection()
	{
		// initialize some data structure ()
		auto i = 0;
		QStringList::const_iterator w_begList = m_listUniteAvailable.cbegin();
		while (w_begList != m_listUniteAvailable.cend())
		{
			QString w_unit = m_listUniteAvailable.value(i++);
			QStringList w_splittedStr = w_unit.split(" ");
			auto w_unitNo = w_splittedStr[1]; // white space should have 2 element 

			// build map structure with key = unit number and value a vector of command field  
			// used to build "bon de livraison" report per unit (each unit has a "bon de livraison") 
			std::vector<bdAPI::BoDucFields> w_vecofSSaved;
			w_vecofSSaved.reserve(100);
			if (m_cmdUnitSaved.insert( std::make_pair(w_unitNo.toInt(), w_vecofSSaved)).second == false)
			{
				// logger some message such as could not insert the following item
				std::cout << "Could not insert cmd unit\n";
			}
			++w_begList;
		}
	}
	// NOT COMPLETD *******************************
	// at the moment we support 10 unit just do a check on each of them
	// call when user select to save selection
	//TODO: deprecated
	void BoDucReportCreator::updateProgress()
	{
		if (m_currUnityON.compare(m_listUniteAvailable.at(0)) == 0) // identical
		{
			double w_val2Show = m_progressBar[0]->value() + std::get<2>(m_bdBonFields);
			m_progressBar[0]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(1)) == 0)
		{
			double w_val2Show = m_progressBar[1]->value() + std::get<2>(m_bdBonFields);
			m_progressBar[1]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(2)) == 0)
		{
			double w_val2Show = m_progressBar[2]->value() + std::get<2>(m_bdBonFields);
			m_progressBar[2]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(3)) == 0)
		{
			double w_val2Show = m_progressBar[3]->value() + std::get<2>(m_bdBonFields);
			m_progressBar[3]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(4)) == 0)
		{
			double w_val2Show = m_progressBar[4]->value() + std::get<2>(m_bdBonFields);
			m_progressBar[4]->setValue(w_val2Show);
		}
	}

	// Design Note (March 19)
	//  Can be done by using a while loop. While !=0 continue
	//  Once found, m_progressBar[i] whatever you want to do.
	//  Extract in a separate method value update and msg box.
	//  
	bool BoDucReportCreator::updateUnitProgress(const float aVal2Update)
	{
		bool ret = true;
		if (m_currUnityON.compare(m_listUniteAvailable.at(0)) == 0) // identical
		{
			double w_val2Show = m_progressBar[0]->value() + aVal2Update;
			if (w_val2Show>m_progressBar[0]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[0]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[0]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(1)) == 0)
		{
			double w_val2Show = m_progressBar[1]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[1]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[1]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[1]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(2)) == 0)
		{
			double w_val2Show = m_progressBar[2]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[2]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[2]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[2]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(3)) == 0)
		{
			double w_val2Show = m_progressBar[3]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[3]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[3]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[3]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(4)) == 0)
		{
			double w_val2Show = m_progressBar[4]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[4]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[4]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[4]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(5)) == 0)
		{
			double w_val2Show = m_progressBar[5]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[5]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[5]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[5]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(6)) == 0)
		{
			double w_val2Show = m_progressBar[6]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[6]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[6]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[6]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(7)) == 0)
		{
			double w_val2Show = m_progressBar[7]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[7]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[7]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[7]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(8)) == 0)
		{
			double w_val2Show = m_progressBar[8]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[8]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[8]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[8]->setValue(w_val2Show);
		}
		else if (m_currUnityON.compare(m_listUniteAvailable.at(9)) == 0)
		{
			double w_val2Show = m_progressBar[9]->value() + aVal2Update;
			if (w_val2Show > m_progressBar[9]->maximum())
			{
				QMessageBox msgBox;
				msgBox.setWindowTitle("Exceed Load");
				msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[9]->maximum()).c_str()));
				msgBox.exec();
				ret=false;
			}
			m_progressBar[9]->setValue(w_val2Show);
		}
		return ret;
	}

	// IMPORTANT maximum value for each unit is determined by the time period (normal/degel)
	// There is a document "Unite + capacite de charge.pdf" where specs for each mode is defined. 
	// Each unit has its own value
	// just configuring the progress bar to display
	void BoDucReportCreator::initProgressBar()
	{
		setUnitCapacityLoad();
	}
  //TODO: deprecated
	void BoDucReportCreator::savetest()
	{
		updateProgress();

		//QMessageBox(tr("Delete it from table"));
		QMessageBox msgBox;
		msgBox.setWindowTitle("Delete Command");
		msgBox.setText("Sure you want delete it?");
		msgBox.setStandardButtons(QMessageBox::Yes);
		msgBox.addButton(QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		if (msgBox.exec() == QMessageBox::Yes) {
			m_tblWidget->removeRow(m_currowNoSelected); // row number
		}
		else {
			// do something else
		}
	}
 	void BoDucReportCreator::saveCmdSelection()
 	{
// 		// alias <cmdNo, shippedTo, deliveryDate, prod code, prod, qty, silo>
 		using bdTpl = std::tuple<std::string, std::string, std::string, long, std::string, float, std::string>; 

		// Once command has been selected and user satisfy,
		// click "save" button to store those in memory. Only 
		// when user click "proceed" that the bon livraison file
		// is created. 
		// code to save the user selection to our map vector where key = unit number
		// 
		// *******************************************************************************
		// Design Note
		//  Since we want to create bon livraison with checked item
		//  loop on all row and check 
		//  
		// unit number
		QString w_curUnit = m_listUnite->currentText();
		auto w_unitNo = w_curUnit.split(" ")[1]; // second is the number
		unsigned w_No = w_unitNo.toUInt();
		std::vector<bdAPI::BoDucFields>& w_vec = m_cmdUnitSaved[w_No];
		auto w_rowCount = m_tblWidget->rowCount();
 		for( auto rCount = 0; rCount < w_rowCount; ++rCount) // each saved cmd
 		{
			QTableWidgetItem* w_checkedItem = m_tblWidget->item(rCount, 0);
			if( nullptr == w_checkedItem)
			{
				return; // at the end of cmd to display
			}

			// now check state value, if it is checked then add it to the map
			// otherwise go to the next one 
	//		Qt::CheckState w_chkState = w_checkedItem->checkState();
			if( Qt::CheckState::Checked != w_checkedItem->checkState())
			{
				continue;
			}
			// need it to delete the row once saved
			auto w_curowSelected = m_tblWidget->currentRow();
			QTableWidgetItem* w_hourItem = m_tblWidget->item(rCount, 1); 
			QString w_cmdHour = w_hourItem->text();

// 			// unit number
// 			QString w_curUnit = m_listUnite->currentText();
// 			auto w_unitNo = w_curUnit.split(" ")[1]; // second is the number
// 			unsigned w_No = w_unitNo.toUInt();
			if( m_cmdUnitSaved.find(w_No) != m_cmdUnitSaved.cend())
			{
				// retrieve vector of cmd for this unit
//				std::vector<bdAPI::BoDucFields>& w_vec = m_cmdUnitSaved[w_No];
				bdTpl w_boducTpl;
				QVector<QVariant> w_bdField;
				w_bdField.reserve(7);
				bdTpl w_bdTuple;
				auto colNumber = m_tblWidget->columnCount();
				for( auto colNo = 2; colNo < m_tblWidget->columnCount(); ++colNo)
				{
					QTableWidgetItem* w_colItem = m_tblWidget->item(rCount, colNo);
				  QVariant w_val = w_colItem->data(Qt::DisplayRole);
					switch (colNo)
					{
					case 2: //command no
						// 
						std::get<0>(w_bdTuple) = w_val.toString().toStdString();
						break;
					case 3: //
						// shippedTo
						std::get<1>(w_bdTuple) = w_val.toString().toStdString();
						break;
					case 4: //date  
						// date
						std::get<2>(w_bdTuple) = w_val.toString().toStdString();
						break;
					case 5: // product code
						std::get<3>(w_bdTuple) = w_val.toInt();
						break;
					case 6: // product descr.
						std::get<4>(w_bdTuple) = w_val.toString().toStdString();
						break;
					case 7: // quantity
						std::get<5>(w_bdTuple) = w_val.toFloat();
						break;
					case 8: //silo no
						std::get<6>(w_bdTuple) = w_val.toString().toStdString();
						break;
					default://???
						break;
					}//switch
 				}//for-loop

			//	float w_progressVal = m_progressBar[0]->value() + std::get<5>(w_bdTuple);

				// need to show 
				if( !updateUnitProgress(std::get<5>(w_bdTuple)))
				{
					continue; // we don't want keep it in the cmd to display 
				}
//				updateUnitProgress(std::get<5>(w_bdTuple));

 				 // need to fill vector with data (vector to display)
 				w_vec.push_back( bdAPI::BoDucFields(std::move(w_bdTuple)));
 			}//if
			// delete row from display widget and its items
			m_tblWidget->removeRow(rCount); // row number
			auto checkDbg = m_tblWidget->rowCount(); // debugging purpose
 		}//row loop
  }
	// loading commands from file (created by the parser)
	void BoDucReportCreator::open()
	{
		// create working folder
		QDir w_reportFolder;	 
		QString w_pathNow = QDir::current().absolutePath();
		w_reportFolder.setPath(w_pathNow);
		QString w_repDirName = w_reportFolder.dirName();

		// multiple file selection support
		// now opening file to be processed (name of the file returned)
		QString w_filesName;

		// we should be at the BoDucApp level
		// need to check if folder exist, if not then create it 
		if( !w_reportFolder.exists( QString("ReportFiles")))
		{
			QString w_pathNow = QDir::current().absolutePath();
			w_pathNow += QString(R"(/ReportFiles)");
			w_reportFolder.setPath(w_pathNow);
			QString w_repDirName = w_reportFolder.dirName();

			w_filesName = QFileDialog::getOpenFileName(this, tr("Open File"),
				w_repDirName,
				tr("Text (*.txt *.csv)"));
		}
		else
		{
			w_reportFolder.setPath(w_pathNow);
			QString w_repDirName = w_reportFolder.dirName();

			w_filesName = QFileDialog::getOpenFileName(this, tr("Open File"),
				w_repDirName,
				tr("Text (*.txt *.csv)"));
		}

		// deprecated
//		m_read4test = readFileTab(w_filesName); // from file
		// deprecated
//		createTableWidget();

#if 1 // next version with sorted and duplicated vector
	//	std::vector<bdAPI::BoDucFields> w_vectorOfCmd = m_bdApp.getReportData();
		// we don' need that, because user is interested seeing data just processed
		// no need to store data in a file (they are already in a vector and you read
		// data from file and put it back in a vector). Unless user 
		// Actually, we need this file because Francois use it to retrieve data from 
		// pass days (for example a week ago), if we keep everything in memory, when 
		// app close we loose all the information (save on hard disk).
		if (!m_vectorOfCmd.empty())
		{
			m_vectorOfCmd.clear(); //empty last cmds
		}
		// show all cmd in the report files
		m_vectorOfCmd = loadCmdReportInVector(w_filesName);
//		remDuplicateAndSort(m_vectorOfCmd);
//		fillTableWidget2Display();
#endif
	}

	void BoDucReportCreator::setMode()
	{
		if (m_capacityLoad==eCapacityMode::normal)
		{
			m_cptySelectBtn->setText(QString("Degel Load"));
			m_capacityLoad = eCapacityMode::degel;
		}
		else
		{
			m_cptySelectBtn->setText(QString("Normal Load"));
			m_capacityLoad = eCapacityMode::normal;
		}
	}

  void BoDucReportCreator::currentUniteON()
	{
		// user change unity selection
		int w_uniteCurrent = m_listUnite->currentIndex(); // user choice
		m_currUnityON = m_listUnite->itemText(w_uniteCurrent);
	}

	// Design Note:
	//  this method will be refactored and renamed by   
	//  create report for each transport unit
	void BoDucReportCreator::createBonLivraison()
	{
		//unite ON
		QString w_uniteON = m_listUnite->currentText();
		QStringList w_splitedUnit = w_uniteON.split(" ");
		// If you want to retrieve all the values for a single key, 
		// you can use values(const Key &key), which returns a QList<T>:
// *****************************
		QList<tplbonlivraison> w_unitDataBon;
		// retrieve vector of cmd for this unit that user has saved
		std::vector<bdAPI::BoDucFields>& w_unitVec = m_cmdUnitSaved[w_splitedUnit[1].toUInt()];
		for( const bdAPI::BoDucFields& val : w_unitVec)
		{
			w_unitDataBon.push_back( std::make_tuple( QString(val.m_deliverTo.c_str()),
				QString(val.m_produit.c_str()),
				val.m_qty, QString(val.m_silo.c_str()).toUShort()));
		}
// *****************************
		// retrieve data to create bon de livraison for each unit
		// shippedTo, product, qty, silo
//		QList<tplbonlivraison> values = m_unitBonLivraisonData.values(w_uniteON); original

		//QDir w_reportFolder; 
		QString w_path = QDir::currentPath();
		QString w_bonLivraisonFile = w_uniteON + "_BonLiveraison.txt";
		QFileInfo w_fileRep( w_path, w_bonLivraisonFile);
		// create a new file with the given name
		QFile w_data(w_fileRep.absoluteFilePath());
		if( !w_fileRep.exists(w_bonLivraisonFile))
		{
			QTextStream out(&w_data);
			// then create it write report
			if( w_data.open(QFile::WriteOnly | QFile::Text))
			{
				for (int i = 0; i < w_unitDataBon.size(); ++i)
				{
					tplbonlivraison w_val2File = w_unitDataBon.at(i);
					out << w_uniteON << "\t" << std::get<0>(w_val2File) << "\t" << std::get<1>(w_val2File) << "\t"
						<< std::get<2>(w_val2File) << "\t" << std::get<3>(w_val2File) << "\t" << "\n";
				}
			}
		}
		else
		{
			// open and append it
			if( w_data.open(QFile::WriteOnly | QFile::Text | QFile::Append))
			{
				QTextStream out(&w_data);
				for (int i = 0; i < w_unitDataBon.size(); ++i)
				{
					tplbonlivraison w_val2File = w_unitDataBon.at(i);
					out << w_uniteON << "\t" <<  std::get<0>(w_val2File) << "\t" << std::get<1>(w_val2File) << "\t"
						  << std::get<2>(w_val2File) << "\t" << std::get<3>(w_val2File) << "\t" << "\n";
				}
			}
		}
		w_data.close();

		// new stuff
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Livraison Final", "",
			QMessageBox::Yes | QMessageBox::No);
		if( reply == QMessageBox::Yes) {
			// delete all commands
			w_unitVec.clear();
// 			qDebug() << "Yes was clicked";
// 			QApplication::quit();
		}
		else {
		//	qDebug() << "Yes was *not* clicked";
		}
	}

	// TODO: deprecated
	// selected a row with mouse (deprecated)
	void BoDucReportCreator::testItemClick( QTableWidgetItem * aItem)
	{
		QTableWidget*  w_tblWgt = aItem->tableWidget();
		Q_ASSERT(w_tblWgt != Q_NULLPTR);
		QItemSelectionModel* select = w_tblWgt->selectionModel();
		if( select->hasSelection())
		{
			//return selected row
			QModelIndexList w_mdl = select->selectedRows();
			auto w_rowSiz = w_mdl.size();

			m_currowNoSelected = aItem->row();
			QVector<QVariant> w_cmd2Report;
			w_cmd2Report.reserve(w_tblWgt->columnCount());
			// nor sure about this one
			QMap<int, tplbonlivraison> w_checkMap;

			// fill the struct for report
			for( auto col = 0; col < w_tblWgt->columnCount(); ++col)
			{
				QTableWidgetItem* w_chckItem = w_tblWgt->item(m_currowNoSelected, col);
				QVariant w_val = w_chckItem->data(Qt::DisplayRole);

				// BoDuc selection
				if (col == 1)
				{
					// shippedTo
					std::get<0>(m_bdBonFields) = w_val.toString();
				}
				else if (col == 4)
				{
					// product description
					std::get<1>(m_bdBonFields) = w_val.toString();
				}
				else if (col == 5)
				{
					// quantity
					std::get<2>(m_bdBonFields) = w_val.toDouble();
				}
				else if (col == 6)
				{
					// silo
					std::get<3>(m_bdBonFields) = w_val.toInt();
				}

				w_cmd2Report.push_back(w_val);
				// add it to the list
			}

			w_checkMap.insert(m_currowNoSelected, m_bdBonFields);

			// store all data selected by user to be retrieved 
			// when creating bon de livraison (file format) 
			// key is the unit name that is currently selected
			m_unitBonLivraisonData.insert(m_currUnityON, m_bdBonFields);
		}
	}
	void BoDucReportCreator::insertHour(int rowNo, int columnNo)
	{
		QTableWidgetItem* w_tblItem = m_tblWidget->item(rowNo, columnNo);
		QVariant w_hourInserted = w_tblItem->data(Qt::EditRole);
		QString w_hourText = w_hourInserted.toString(); // shall be the item selected is hour
		if (w_hourText.isEmpty())
		{
			qDebug("");
		}
	}
#if 0 // new version
	void BoDucReportCreator::itemSelected( QTableWidgetItem * aItem)
	{
		QTableWidget*  w_tblWgt = aItem->tableWidget();
		Q_ASSERT(w_tblWgt != Q_NULLPTR);

		bdAPI::BoDucFields w_field;
		w_field.m_noCmd = w_val.toString().toStdString();

		QItemSelectionModel* select = w_tblWgt->selectionModel();
		if (select->hasSelection())
		{
			//return selected row
			QModelIndexList w_mdl = select->selectedRows();
			auto w_rowSiz = w_mdl.size();

			m_currowNoSelected = aItem->row();
			QVector<QVariant> w_cmd2Report;
			w_cmd2Report.reserve(w_tblWgt->columnCount());
		}
		// just a test
		QString w_curUnit = m_listUnite->currentText();
		auto w_unitNo = w_curUnit.split(" ")[1]; // second is the number
		unsigned w_No = w_unitNo.toUInt();
		if (m_cmdUnitSaved.find(w_No) != m_cmdUnitSaved.cend())
		{
			std::vector<bdAPI::BoDucFields>& w_vec = m_cmdUnitSaved[w_No];
			// need to fill vector with data
			w_vec.push_back(bdAPI::BoDucFields());
		}
	}
#endif // end of new version
	void BoDucReportCreator::setupViews()
	{
		// we set first 0 row and 8 column which is our standard
		m_tblWidget = new QTableWidget(0, 9, this); //
		m_tblWidget->setHorizontalHeaderLabels(QStringList() //<< tr("Selected")
			<< tr("Status")
			<< tr("Heure")
			<< tr("Command No")
			<< tr("Shipped To")
			<< tr("Date")
			<< tr("Code")
			<< tr("Produit")
			<< tr("Quantite")
			<< tr("Silo"));
		m_tblWidget->verticalHeader()->setVisible(false);
		m_tblWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		m_tblWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_tblWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
//		m_tblWidget->setSelectionMode(QAbstractItemView::MultiSelection);
		m_tblWidget->setSelectionMode(QAbstractItemView::NoSelection);
		m_tblWidget->setShowGrid(true);
		m_tblWidget->setGeometry(QRect(0, 100, 781, 281));
		m_tblWidget->horizontalHeader()->resizeSection(1, 250);
		m_tblWidget->horizontalHeader()->resizeSection(2, 250);
		m_tblWidget->horizontalHeader()->resizeSection(3, 180);
		//m_tblWidget->setStyleSheet("QTableView {selection-background-color: red;}");
	}
	void BoDucReportCreator::saveDataIntoTable()
	{
		if( !m_tblWidget)
			return;

		const int currentRow = m_tblWidget->rowCount();
		m_tblWidget->setRowCount(currentRow + 1);
	}

	// TODO: deprecated
	void BoDucReportCreator::createTableWidget()
	{
		auto i = 0; // create each row of the table
		for( const QVector<QVariant>& w_vecVariant : m_read4test)
		{
			QTableWidgetItem* myTableWidgetItem = Q_NULLPTR;
			for (auto j = 0; j < m_tblWidget->columnCount(); ++j)
			{
				myTableWidgetItem = new QTableWidgetItem;     // i am not sure about this one
																											//myTableWidgetItem->data(Qt::CheckStateRole);  // 
				myTableWidgetItem->setData(Qt::DisplayRole, w_vecVariant[j]);
				m_tblWidget->setItem(i, j, myTableWidgetItem);
			}
			++i;
		}
	}
	void BoDucReportCreator::fillTableWidget2Display()
	{
		// set number of row to the size of the display vector
		m_tblWidget->setRowCount( m_vecOfCmd2Display.size()); 
		auto w_remainingRows = m_tblWidget->rowCount();

   	// fill displayed table with displayed vector of cmd 
		//QList<QVector<QVariant>> w_listOfVariant = fromBDFieldToQVariant(m_vectorOfCmd);
		QList<QVector<QVariant>> w_listOfVariant = fromBDFieldToQVariant(m_vecOfCmd2Display);

		auto i = 0; // create each row of the table
		for( const QVector<QVariant>& w_vecVariant : w_listOfVariant)
		{
			QTableWidgetItem* myTableWidgetItem = Q_NULLPTR;
			for( auto j = 0; j < m_tblWidget->columnCount(); ++j)
			{
				myTableWidgetItem = new QTableWidgetItem;     // i am not sure about this one
				// case  j==0, user check box in the first column
				if( j == 0)
				{
				//	myTableWidgetItem->setData(Qt::Unchecked, Qt::CheckStateRole);
					myTableWidgetItem->setFlags(myTableWidgetItem->flags() | Qt::ItemIsUserCheckable);
					myTableWidgetItem->setCheckState(Qt::Unchecked);
					m_tblWidget->setItem(i, 0, myTableWidgetItem);
					continue;
				}
				// for column 1 (heure) user edit this field and part of the bon de livraison
				if( j==1)
				{
					myTableWidgetItem->setData(Qt::EditRole,QVariant("0 am"));
					m_tblWidget->setItem(i, j, myTableWidgetItem);
					continue;
				}
			  //myTableWidgetItem->data(Qt::CheckStateRole);   
				myTableWidgetItem->setData(Qt::DisplayRole, w_vecVariant[j-2]);
				m_tblWidget->setItem(i, j, myTableWidgetItem);
			}
			++i;
		}

		// clear all data in the vector cmd to display ready for next query
	}
	// Top group box, actually it's the original buttons layout
	// Open, load, save, process (buttons)
	QGroupBox* BoDucReportCreator::createAnalyzerBox()
	{
 		QHBoxLayout* w_rowLayout = new QHBoxLayout;

    // list initializer ctor with default buttons name 
		m_buttonTitle = new QStringList; // ({ /*"Exit",*/"Process","Load","Save","Open" });
		m_buttonTitle->push_back("Process");
		m_buttonTitle->push_back("Load");
		m_buttonTitle->push_back("Save");
		m_buttonTitle->push_back("Open");
		QStringList::const_iterator w_butName = m_buttonTitle->cbegin();

		for (int i = 0; i < m_numButtons; ++i)
		{
			m_columnLayout[i] = new QVBoxLayout;
		}

		// first element of the row
// 		m_exit = new QPushButton(tr(w_butName->toStdString().c_str()));
// 		if (!m_exit->isEnabled())
// 		{
// 			m_exit->setEnabled(true);
// 		}
// 		m_columnLayout[0]->addWidget(m_exit);
// 		m_rowLayout->addLayout(m_columnLayout[0], 10/*stretch*/);
// 		m_columnLayout[0]->addSpacing(m_spacinVal);
// 		++w_butName;

		// second element of the row
		m_procButton = new QPushButton(tr(w_butName->toStdString().c_str()));
		if (m_procButton->isEnabled())
		{
			m_procButton->setEnabled(false);
		}
		m_columnLayout[0]->addWidget(m_procButton);
		w_rowLayout->addLayout(m_columnLayout[0], 10/*stretch*/);
		m_columnLayout[0]->addSpacing(m_spacinVal);
		++w_butName;

		// third element of the row
		m_analyzeLoadButton = new QPushButton(tr(w_butName->toStdString().c_str()));
		if (m_analyzeLoadButton->isEnabled())
		{
			m_analyzeLoadButton->setEnabled(false);
		}
		m_columnLayout[1]->addWidget(m_analyzeLoadButton);
		w_rowLayout->addLayout(m_columnLayout[1], 10/*stretch*/);
		m_columnLayout[1]->addSpacing(m_spacinVal);
		++w_butName;

		// third element of the row
		m_saveButton = new QPushButton(tr(w_butName->toStdString().c_str()));
		if (m_saveButton->isEnabled())
		{
			m_saveButton->setEnabled(false);
		}
		m_columnLayout[2]->addWidget(m_saveButton);
		w_rowLayout->addLayout(m_columnLayout[2], 10/*stretch*/);
		m_columnLayout[2]->addSpacing(m_spacinVal);
		++w_butName;

		// Open file 
		m_openButton = new QPushButton(tr(w_butName->toStdString().c_str()));
		if( !m_openButton->isEnabled())
		{
			// at initialization user need to open file first, 
			// all other buttons are disabled or set to false
			m_openButton->setEnabled(true);
		}
		m_columnLayout[3]->addWidget(m_openButton);
		w_rowLayout->addLayout(m_columnLayout[3], 10/*stretch*/);
		m_columnLayout[3]->addSpacing(m_spacinVal);

		// set layout of this box
		m_analyzerBox->setLayout(w_rowLayout);

		return m_analyzerBox;
	}

	// This need to be refactored, new buttons will be added
	// Current design we create 2 horizontal box and ...
	// Want a Box call Panel and another one call Report
	// Separate functionnalities in different box
	QGroupBox* BoDucReportCreator::createCreatorBox()
	{
		// First Row
		QBoxLayout* w_row1 = createHBoxLayoutRow1();

		// Second row
		QHBoxLayout* w_buttonsBot = new QHBoxLayout;

		// List of unite available
		QLabel* w_dateLabl = new QLabel(tr("Date Selectionne"));
		QVBoxLayout* w_date2show = new QVBoxLayout;
		m_dateSelect = new QDateEdit;
		m_dateSelect->setCalendarPopup(true); // not what it does?
		QDate w_minDate(QDate::currentDate());
		//w_minDate.addMonths(-6);
		m_dateSelect->setMinimumDate(w_minDate.addMonths(-8));

		w_date2show->addWidget(w_dateLabl);
		w_date2show->addWidget(m_dateSelect);
		// horizontal box layout 
		w_buttonsBot->addLayout(w_date2show);

		QLabel* w_dateRngLbl = new QLabel(tr("Date Range"));
		QVBoxLayout* w_vDateRngBox = new QVBoxLayout;
		m_dateMinSelected = new QDateEdit;
		m_dateMinSelected->setCalendarPopup(true);
		m_dateMinSelected->setMinimumDate(w_minDate.addMonths(-8));
	  m_dateMaxSelected = new QDateEdit;
		m_dateMaxSelected->setCalendarPopup(true);
		m_dateMaxSelected->setMinimumDate(w_minDate.addMonths(-8));
		w_vDateRngBox->addWidget(w_dateRngLbl);
		w_vDateRngBox->addWidget(m_dateMinSelected);
		w_vDateRngBox->addWidget(m_dateMaxSelected);
		w_buttonsBot->addLayout(w_vDateRngBox);

		QLabel* w_showLbl = new QLabel(tr("Show Selection"));
		QVBoxLayout* w_vBox2Show = new QVBoxLayout;
		m_showButton = new QPushButton(tr("Show"));
		QHBoxLayout* w_checkHBox = new QHBoxLayout;

		m_date2Check = new QCheckBox;
		m_date2Check->setCheckable(true);
		m_date2Check->setCheckState(Qt::Unchecked);
		w_checkHBox->addWidget(m_date2Check);
		QLabel* w_dateHLbl = new QLabel(tr("Date"));
		w_checkHBox->addWidget(w_dateHLbl);
		
		m_rngDate2Check = new QCheckBox;
		m_rngDate2Check->setCheckable(true);
		m_rngDate2Check->setCheckState(Qt::Unchecked);
		w_checkHBox->addWidget(m_rngDate2Check);
		QLabel* w_rngDateHLbl = new QLabel(tr("Range Date"));
		w_checkHBox->addWidget(w_rngDateHLbl);
// 		w_vBox2Show->addWidget(w_showLbl);
// 		w_vBox2Show->addWidget(m_showButton);
// 		w_vBox2Show->addLayout(w_checkHBox);
// 		w_buttonsBot->addLayout(w_vBox2Show);

		m_allDateCheck = new QCheckBox;
		m_allDateCheck->setCheckable(true);
		m_allDateCheck->setCheckState(Qt::Unchecked);
		w_checkHBox->addWidget(m_allDateCheck);
		QLabel* w_allDataLbl = new QLabel(tr("All date"));
		w_checkHBox->addWidget(w_allDataLbl);

		w_vBox2Show->addWidget(w_showLbl);
		w_vBox2Show->addWidget(m_showButton);
		w_vBox2Show->addLayout(w_checkHBox);
		w_buttonsBot->addLayout(w_vBox2Show);

		QLabel* w_clearLbl = new QLabel(tr("Clear Selection"));
		QVBoxLayout* w_clearSelect = new QVBoxLayout;
		m_clearButton = new QPushButton(tr("Clear"));
		w_clearSelect->addWidget(w_clearLbl);
		w_clearSelect->addWidget(m_clearButton);
		w_buttonsBot->addLayout(w_clearSelect);

		QGridLayout* w_gridLayout = new QGridLayout;
		w_gridLayout->addLayout(w_row1, 0, 0);
		w_gridLayout->addLayout(w_buttonsBot, 1, 0);

		// now set our box 
		m_creatorBox->setLayout(w_gridLayout);

		return m_creatorBox;
	}
	void BoDucReportCreator::createOtherWidgets()
	{
		// create the group box
		m_cmdBox = new QGroupBox(tr("Liste de Commandes"));
		m_uniteBox = new QGroupBox(tr("Liste des Unites"));
		m_analyzerBox = new QGroupBox(tr("Analyse des commandes"));
		m_creatorBox = new QGroupBox(tr("Bon De Livraison"));

		// create buttons at the bottom 
		m_dialogButtonBox = new QDialogButtonBox;
		//m_saveButton = m_buttonBox->addButton(QDialogButtonBox::Save);
		m_closeButton = m_dialogButtonBox->addButton(QDialogButtonBox::Close);
		m_cancelButton = m_dialogButtonBox->addButton(QDialogButtonBox::Cancel);
	}
	void BoDucReportCreator::createLayout()
	{
		// display box (command display in the right panel)
		setTableWidgetBox();

		// display the progress horizontally
		setHProgressBar();
#if 0
		// text editor (console)
		m_console = new QTextEdit;
		m_console->setOverwriteMode(false);
		m_console->setCurrentFont( QFont("Helvetica")); // debugging purpose
		m_console->setFontPointSize(10); // font size when writing to console 
		m_console->setText("Please select file by opening and then load");
#endif

		// main dialog
		QVBoxLayout* w_mainLayout = new QVBoxLayout;
		//w_mainLayout->addLayout(w_buttonsTop);
		w_mainLayout->addWidget(createAnalyzerBox());
		w_mainLayout->addWidget(createCreatorBox());
		w_mainLayout->addWidget(m_uniteBox);
		w_mainLayout->addWidget(m_cmdBox);
		//w_mainLayout->addWidget(m_console);  text editor 
		w_mainLayout->addLayout(setBottomButtons());
		w_mainLayout->setGeometry(QRect(600, 400, 0, 0));
		setLayout(w_mainLayout);
	}
	QBoxLayout* BoDucReportCreator::setBottomButtons()
	{
		// buttons at the bottom of the dialog
		QHBoxLayout* w_horizontalLayout = new QHBoxLayout;
		w_horizontalLayout->addWidget(m_dialogButtonBox);

		return w_horizontalLayout;
	}
	void BoDucReportCreator::setHProgressBar()
	{
		QVBoxLayout* w_vlayoutPbar = new QVBoxLayout;
		// create a layout for progress bar in a vertical layout
		QHBoxLayout* w_hProgressBar1 = new QHBoxLayout;
		addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(0).toStdString().c_str());
		addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(1).toStdString().c_str());
		addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(2).toStdString().c_str());
		addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(3).toStdString().c_str());
		addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(4).toStdString().c_str());
	//	m_uniteBox->setLayout(w_hProgressBar);
		w_vlayoutPbar->addLayout(w_hProgressBar1);
		QHBoxLayout* w_hProgressBar2 = new QHBoxLayout;
		addProgressBar(w_hProgressBar2, m_listUniteAvailable.at(5).toStdString().c_str());
		addProgressBar(w_hProgressBar2, m_listUniteAvailable.at(6).toStdString().c_str());
		addProgressBar(w_hProgressBar2, m_listUniteAvailable.at(7).toStdString().c_str());
		addProgressBar(w_hProgressBar2, m_listUniteAvailable.at(8).toStdString().c_str());
		addProgressBar(w_hProgressBar2, m_listUniteAvailable.at(9).toStdString().c_str());
		w_vlayoutPbar->addLayout(w_hProgressBar2);
		m_uniteBox->setLayout(w_vlayoutPbar);
	}
	void BoDucReportCreator::setUniteBox()
	{
		// create a layout for progress bar in a vertical layout
		QVBoxLayout* w_vProgressBar = new QVBoxLayout;

		addProgressBar(w_vProgressBar, std::string("Unite 110"));
		addProgressBar(w_vProgressBar, std::string("Unite 111"));
		addProgressBar(w_vProgressBar, std::string("Unite 112"));
		addProgressBar(w_vProgressBar, std::string("Unite 115"));
		m_uniteBox->setLayout(w_vProgressBar);
	}
	void BoDucReportCreator::addProgressBar( QBoxLayout* aProgressBar, const std::string& aUniteNb)
	{
		if( QVBoxLayout* w_lablandLayout1 = dynamic_cast<QVBoxLayout*>(aProgressBar))
		{
			// ...
			QLabel* w_barU1 = new QLabel(tr(aUniteNb.c_str()));
			m_progressBar[0] = new QProgressBar;

			// Both label and button together
			//			QVBoxLayout* w_lablandLayout1 = new QVBoxLayout;
			w_lablandLayout1->addWidget(w_barU1);
			w_lablandLayout1->addWidget(m_progressBar[0]);
			aProgressBar->addStretch(2);
			aProgressBar->addLayout(w_lablandLayout1);
		}
		else if (QHBoxLayout* w_lablandLayout1 = dynamic_cast<QHBoxLayout*>(aProgressBar))
		{
			QLabel* w_barlbl = new QLabel(tr(aUniteNb.c_str()));
//			short i = 0;
		//	static_assert(i < 10 && "");

// 			for (const QString& w_unit : m_listUniteAvailable)
// 			{
				if (m_unitIndex >=0 && m_unitIndex<m_nbUnit)
				{
					m_progressBar[m_unitIndex] = new QProgressBar;
					w_lablandLayout1->addWidget(w_barlbl);
					w_lablandLayout1->addWidget(m_progressBar[m_unitIndex++]);
					aProgressBar->addStretch(1);
					aProgressBar->addLayout(w_lablandLayout1);
				}
// 			}
#if 0
			if (QString::compare(m_listUniteAvailable.at(0), QString(aUniteNb.c_str())) == 0)
			{
				m_progressBar[0] = new QProgressBar;
				w_lablandLayout1->addWidget(w_barlbl);
				w_lablandLayout1->addWidget(m_progressBar[0]);
				aProgressBar->addStretch(1);
				aProgressBar->addLayout(w_lablandLayout1);
			}
			else if (QString::compare(m_listUniteAvailable.at(1), QString(aUniteNb.c_str())) == 0)
			{
				m_progressBar[1] = new QProgressBar;
				w_lablandLayout1->addWidget(w_barlbl);
				w_lablandLayout1->addWidget(m_progressBar[1]);
				aProgressBar->addStretch(1);
				aProgressBar->addLayout(w_lablandLayout1);
			}
			else if (QString::compare(m_listUniteAvailable.at(2), QString(aUniteNb.c_str())) == 0)
			{
				m_progressBar[2] = new QProgressBar;
				w_lablandLayout1->addWidget(w_barlbl);
				w_lablandLayout1->addWidget(m_progressBar[2]);
				aProgressBar->addStretch(1);
				aProgressBar->addLayout(w_lablandLayout1);
			}
			else if (QString::compare(m_listUniteAvailable.at(3), QString(aUniteNb.c_str())) == 0)
			{
				m_progressBar[3] = new QProgressBar;
				w_lablandLayout1->addWidget(w_barlbl);
				w_lablandLayout1->addWidget(m_progressBar[3]);
				aProgressBar->addStretch(1);
				aProgressBar->addLayout(w_lablandLayout1);
			}
			else if (QString::compare(m_listUniteAvailable.at(4), QString(aUniteNb.c_str())) == 0)
			{
				m_progressBar[4] = new QProgressBar;
				w_lablandLayout1->addWidget(w_barlbl);
				w_lablandLayout1->addWidget(m_progressBar[4]);
				aProgressBar->addStretch(1);
				aProgressBar->addLayout(w_lablandLayout1);
			}
		}
		else
		{
			return;
		}
#endif
		}
	}
	void BoDucReportCreator::setTableWidgetBox()
	{
		QVBoxLayout* w_viewLayout = new QVBoxLayout;
		w_viewLayout->addWidget(m_tblWidget);
		m_cmdBox->setLayout(w_viewLayout); // add it to Group box
	}
	// this is the start method of our test environment
	void BoDucReportCreator::parseCmd()
	{
	//	m_console->append("Parsing Files of command");

		m_bdApp.process();

//		m_console->append("Finished processed the file");

		// if process is done successfully, might as well save it to file
		if (!m_saveButton->isEnabled())
		{
			m_saveButton->setEnabled(true);
		}
		// make it ready for next command file
		if (!m_analyzeLoadButton->isEnabled()) // not sure about this one
		{
			m_analyzeLoadButton->setEnabled(false);
		}
		m_procButton->setEnabled(false);
	}

  // creating the report with default file name (set when app is started)
	void BoDucReportCreator::saveCmdToFile()
	{
		// deprecated 
		createDataReport();

		if (!m_openButton->isEnabled())
		{
			m_openButton->setEnabled(true);
		}
		m_saveButton->setEnabled(false);
	}
	void BoDucReportCreator::loadCmdFromFile()
	{
		// support to pdf file type we need a different algo for reading
		// check file extension QtFileInfo and std::all_of()
// 		bool w_checkFileExt = std::all_of( m_filesName.constBegin(), m_filesName.constEnd(),
// 			[](const QString& aFileName)
// 		{
// 			QFileInfo w_file2Look(aFileName);
// 			QString w_fExt = w_file2Look.completeSuffix();
// 			return (w_fExt.compare("txt")==0); // equal (set to txt for debugging purpose, but it should be pdf)
// 		});
// 
// 		if (w_checkFileExt)
// 		{
// 			m_bdApp.setFileType(bdAPI::BoDucApp::eFileType::pdf);
// 		}
// 		else
// 		{
// 			m_bdApp.setFileType(bdAPI::BoDucApp::eFileType::csv);
// 		}

		if( m_filesName.size() > 1) // user selected more than one files
		{
			std::list<std::string> w_listFilesName;
			QStringList::const_iterator w_begList = m_filesName.constBegin();
			QStringList::const_iterator w_endList = m_filesName.constEnd();
			while( w_begList != w_endList)
			{
				std::cout << "Loading the following files: " << w_begList->toStdString() << std::endl;
				w_listFilesName.push_back(w_begList->toStdString());
				++w_begList;
			}
//			m_console->append("");
//			m_console->append("Starting to load files");

			// loop on each files in the list (fill vector map for processing multiple files in one step)
			m_bdApp.readFiles(w_listFilesName, std::string("Signature"));

//			m_console->append("Finished loading files");
		}
		else // only one file
		{
			// actually what we do here, we split bill into separate cmd 
			// (case where we have multiple command in the same file)
			// not sure the behavior for single files (command are already 
			// in single file) and we process each file one after the other.  
			//m_bdApp.readFile( m_fileName.toStdString(), std::string("Signature"));
			QString w_checkName = m_filesName.front();
			m_bdApp.readFile(m_filesName.front().toStdString(), std::string("Signature")); // first in the list (only one) 
			size_t w_nbCmdCheck = m_bdApp.nbOfCmd();

			// writing to console 
// 			m_console->append(QString(""));
// 			m_console->append(QString("There is ") + QString(std::to_string(w_nbCmdCheck).c_str()) + QString(" command to process"));
// 			m_console->append(QString("Finished to load data"));
// 			m_console->append(QString("Ready to proceed to parse command, then press process button"));
		}

//		m_console->append("Ready to  process these files");

		if (!m_procButton->isEnabled())
		{
			m_procButton->setEnabled(true);
		}
		if (m_analyzeLoadButton->isEnabled())
		{
			m_analyzeLoadButton->setEnabled(false);
		}
	}

	// called when "open" button clicked
	// by default all files for testing are located in a folder named "DataToTest"
	void BoDucReportCreator::OpenCsvFile()
	{
		// setting default directory to start with 
		// i need to check, which is the root directory of the VS15 project
		QString w_defaultDir = QDir::currentPath();

		std::cout << "Current of the opening file is : " << w_defaultDir.toStdString() << std::endl;

		// multiple file selection support
		// now opening file to be processed (name of the file returned)
		m_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
			w_defaultDir,
			tr("Text (*.txt *.csv *.pdf)"));

		bool w_checkFileExt = std::all_of(m_filesName.constBegin(), m_filesName.constEnd(),
			[](const QString& aFileName)
		{
			QFileInfo w_file2Look(aFileName);
			QString w_fExt = w_file2Look.completeSuffix();
			return (w_fExt.compare("pdf") == 0); // equal (set to txt for debugging purpose, but it should be pdf)
		});

		if( w_checkFileExt)
		{
			m_bdApp.setFileType(bdAPI::BoDucApp::eFileType::pdf);
  		convertPdf2Txt();
		}
		else
		{
			m_bdApp.setFileType(bdAPI::BoDucApp::eFileType::csv);
		}

   	// number of files selected by user
		m_fileLoaded = m_filesName.size();
		m_bdApp.setNbSelectedFiles(m_fileLoaded);

		if (m_filesName.size() == 1)
		{
			if (m_filesName.size() == 1)
				m_fileName = m_filesName.front(); // only file in the list

			qWarning("Could not open file for reading");
		}

//		m_console->append("You have selected the following file(s): ");
// 		for (const QString& w_fileName : m_filesName)
// 		{
//			m_console->append(w_fileName);
//		}

		// activate button, can now start parsing the .csv file
		if (!m_analyzeLoadButton->isEnabled())
		{
			m_analyzeLoadButton->setEnabled(true);
		}

		m_openButton->setEnabled(false);
//		m_console->append(QString("Click load button to read file ..."));
	}
	void BoDucReportCreator::createConnections()
	{
		QObject::connect( m_closeButton,     SIGNAL(clicked()),                      this, SLOT(close()));
		QObject::connect( m_cancelButton,     SIGNAL(clicked()),                     this, SLOT(cancel()));
		QObject::connect( m_loadButton,      SIGNAL(clicked()),                      this, SLOT(open()));
		QObject::connect( m_bonCreateReport, SIGNAL(clicked()),                      this, SLOT(createBonLivraison()));
		QObject::connect( m_listUnite,       SIGNAL(activated(int)),                 this, SLOT(currentUniteON()));
		//QObject::connect( m_saveSelectBtn,   SIGNAL(clicked()),                      this, SLOT(savetest())); deprecated
		QObject::connect(m_saveSelectBtn,    SIGNAL(clicked()),                      this, SLOT(saveCmdSelection()));
		QObject::connect( m_cptySelectBtn,   SIGNAL(clicked()),                      this, SLOT(setMode()));
		QObject::connect( m_tblWidget,       SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(testItemClick(QTableWidgetItem*)));
//		QObject::connect( m_tblWidget,       SIGNAL(cellChanged(int,int)),           this, SLOT(insertHour(int,int)));
		QObject::connect( m_showButton,      SIGNAL(clicked()),                      this, SLOT(showCmd()));
//		QObject::connect( m_clearButton,     SIGNAL(clicked()),                      this, SLOT(clearContents()));
		QObject::connect( m_clearButton,      SIGNAL(clicked()),                     this, SLOT(clearDispalyedCmd()));
//		QObject::connect(m_clearButton,      SIGNAL(clicked()),                      this, SLOT(removeRow(int)));
		QObject::connect( m_allDateCheck,     SIGNAL(stateChanged()),                this, SLOT(allDateChecked()));
		// Analyze buttons
		QObject::connect( m_saveButton,        SIGNAL(clicked()),                    this, SLOT(saveCmdToFile()));   //save file name
		QObject::connect( m_procButton,        SIGNAL(clicked()),                    this, SLOT(parseCmd()));        // do some processing
		QObject::connect( m_analyzeLoadButton, SIGNAL(clicked()),                    this, SLOT(loadCmdFromFile())); // set file name to be read
		QObject::connect( m_openButton,        SIGNAL(clicked()),                    this, SLOT(OpenCsvFile()));     //
	}
	void BoDucReportCreator::createDataReport()
	{
// 		m_console->append("\n");
// 		m_console->append("We are creating the report for this command");

		// creating the report for data processing
		std::vector<bdAPI::BoDucFields> w_data2Report = m_bdApp.getReportData();

		// sanity check
// 		if (w_data2Report.empty())
// 		{
// 			//std::cout << "We have a problem to create the report\n";
// 			QColor w_fontClor = m_console->textColor(); // shall display such a message 
// 	//		m_console->append("We have a problem to create the report"); // to be completed
// 		}

		//TODO: need to set working folder outside of the createDataReport method
		// and not hard coded (using string literal)
		// C:\JeanWorks\\BoDuc\App\\BoDucApp\BoDucAppGui i think correspond to current
		QDir w_currentDir = QDir::current(); // but we want a folder above
		QString w_dirName = w_currentDir.dirName(); // check
		//QDir w_workingDir(QString(R"(C:\JeanWorks\\BoDuc\App\BoDucApp\ReportFiles)"));
		QDir w_reportFolder;
		// we need to move up why??
		if( w_currentDir.cdUp()) // folder higher of project folder (BoDucApp)
		{
			QString w_currNow = QDir::current().dirName(); // project folder
			QString w_pathNow = w_currentDir.absolutePath(); // BoDucApp

			w_reportFolder.setPath(w_pathNow);
			QString w_checkWork = w_reportFolder.dirName();

			// we should be at the BoDucApp level
			// need to check if folder exist, if not then create it 
			if( !w_reportFolder.exists(QString("ReportFiles")))
			{
				if (w_reportFolder.mkdir(QString("ReportFiles")))
				{
					w_reportFolder.cd(QString("ReportFiles"));
					//m_console->append("Folder ReportFiles created sucessfully");
					//std::cout << "Folder ReportFiles created sucessfully\n";
				}
			}
			else // set to the report folder
			{
				w_reportFolder.cd(QString("ReportFiles"));
			}
		}
// 		m_console->append("\n");
// 		m_console->append("Report directory is:" + w_reportFolder.absolutePath());
// 		m_console->append("\n");

		QFileInfo w_fileRep(w_reportFolder, m_bonLivraisonFile);

		// file doesn't exist
		if( !w_reportFolder.exists(m_bonLivraisonFile))
		{
			QString w_filePath = w_fileRep.absoluteFilePath();
			// create a new file with the given name
			QFile w_data(w_fileRep.absoluteFilePath());

			// then create it write report
			if (w_data.open(QFile::WriteOnly | QFile::Text))
			{
				QTextStream out(&w_data);

				// first time we open it, create header
				out << "No Command " << "\t" << "Shipped To " << "\t" << "Deliver Date " << "\t"
					<< "Code" << "\t" << "Product " << "\t" << "Quantity " << "\t" << "Silo" << "\n";

				for (bdAPI::BoDucFields& w_bfield : w_data2Report)
				{
					out << w_bfield.m_noCmd.c_str() << "\t" << w_bfield.m_deliverTo.c_str() << "\t" << w_bfield.m_datePromise.c_str() << "\t"
						<< w_bfield.m_prodCode << "\t" << w_bfield.m_produit.c_str() << "\t" << w_bfield.m_qty << "\t" << w_bfield.m_silo.c_str() << "\n";
				}
				if (w_data.isOpen())
				{
					w_data.close(); // make sure we close we may want to re-open
				}
			}
		}
		else // file exist, just append data at the end
		{
			QFile w_data; // set file name of an existing one
			w_data.setFileName(w_fileRep.absoluteFilePath());

			if (w_data.open(QFile::WriteOnly | QFile::Append | QFile::Text))
			{
				QTextStream out(&w_data);
				out << "\n";

				for( bdAPI::BoDucFields& w_bfield : w_data2Report)
				{
					out << w_bfield.m_noCmd.c_str() << "\t" << w_bfield.m_deliverTo.c_str() << "\t" << w_bfield.m_datePromise.c_str() << "\t"
						<< w_bfield.m_prodCode << "\t" << w_bfield.m_produit.c_str() << "\t" << w_bfield.m_qty << "\t" << w_bfield.m_silo.c_str() << "\n";
				}
				if (w_data.isOpen())
				{
					w_data.close();
				}
			}
		}
	}

	void bdApp::BoDucReportCreator::unitLoadConfig()
	{
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 30,  17.,  14.)); //unit 30
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 33,  31.,  25.)); //unit 33
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 103, 32.,  25.)); //unit 103
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 110, 32.,  25.)); //unit 110
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 111, 26.,  21.)); //unit 111
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 112, 38.5, 32.)); //unit 112
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 114, 38.5, 31.)); //unit 114
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 115, 32.,  25.)); //unit 115
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 116, 14.,  11.)); //unit 116
		m_listLoadValuesPerUnit.push_back( std::make_tuple( 117, 38.5, 32.)); //unit 117
	}

	void bdApp::BoDucReportCreator::setUnitCapacityLoad()
	{
		short i = 0;
		std::for_each( m_listLoadValuesPerUnit.cbegin(), m_listLoadValuesPerUnit.cend(),
			[this,&i]( const tplunitAndLoad& aUnitLoad)
		{
			if (!m_progressBar[i]->isEnabled())
			{
				m_progressBar[i]->setEnabled(true);
			}
			if (m_progressBar[i]->value() != 0)
			{
				// set initial value
				m_progressBar[i]->setValue(0);
			}
			if (m_progressBar[i]->minimum() != 0)
			{
				m_progressBar[i]->setMinimum(0);
			}

			if (m_capacityLoad == eCapacityMode::normal)
			{
				m_progressBar[i++]->setMaximum(std::get<1>(aUnitLoad));
			}
			else // degel
			{
				m_progressBar[i++]->setMaximum(std::get<2>(aUnitLoad));
			}
		});
	}

	void BoDucReportCreator::convertPdf2Txt()
	{
		// convert to txt file
		const QString pythonScript = R"(F:\EllignoContract\BoDuc\pdfminerTxt\pdfminer-20140328\build\scripts-2.7\pdf2txt.py)";
		//const QString w_pdfilesPath = R"(F:\EllignoContract\BoDuc\QtTestGui\BoDucReportCreator\BoDucReportCreator\Data)";  pdf files folder
    const QString w_pdfilesPath = R"(F:\EllignoContract\BoDuc\QtTestGui\BoDucReportCreator\BoDucReportCreator\Data\DataValidation)"; // debugging purpose

//     QDir w_txtFilesFolder;
//     QString w_pathNow = QDir::current().absolutePath();
//     w_txtFilesFolder.setPath(w_pathNow);
//     QString w_repDirName = w_txtFilesFolder.dirName();

    // directory validation
    //if( QDir::exists(w_pdfilesPath)) {}

		QProcess w_process(this);
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		env.insert("PYTHONPATH", "C:\\Python27\\Lib");
		env.insert("PYTHONHOME", "C:\\Python27");
		w_process.setProcessEnvironment(env);
		// Sets the working directory to dir. QProcess will start the process in this directory.
		// The default behavior is to start the process in the working directory of the calling process.
		w_process.setWorkingDirectory(w_pdfilesPath); // otherwise set path working dir of app

		QStringList w_listofTxtFiles;
		// number of files selected by user
		QStringListIterator filesListIterator(m_filesName);
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
			w_listofTxtFiles.push_back(w_txtPath);

			QString w_ofile("-o ");
			QStringList params;
			//w_process.start("Python", params);
			//			QStringList params;
			//std::cout << filesListIterator.next().constData() << std::endl;
			params << pythonScript << w_ofile + w_complTxtFile << w_complPdfFile;

			w_process.start("Python", params);
			if (w_process.waitForFinished(-1))
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
		}
		// swap content, we are interested by .txt files (parsing) 
		w_listofTxtFiles.swap(m_filesName);
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

  QBoxLayout* BoDucReportCreator::createHBoxLayoutRow1()
	{
		// Create a first row of buttons
		QHBoxLayout* w_buttonsTop = new QHBoxLayout;

		// List of unite available
		QLabel* w_listUniteLabl = new QLabel(tr("Unite Selectionne"));
		m_listUnite = new QComboBox;

		m_listUnite->addItems(m_listUniteAvailable);

		QVBoxLayout* w_uniteComboBox = new QVBoxLayout;
		w_uniteComboBox->addWidget(w_listUniteLabl);
		w_uniteComboBox->addWidget(m_listUnite);
		//	w_uniteComboBox->addStretch(1);
		w_buttonsTop->addLayout(w_uniteComboBox);

		// set the current unite that is activated as default
		int w_uniteCurrent = m_listUnite->currentIndex(); // user choice
		m_currUnityON = m_listUnite->itemText(w_uniteCurrent);

		QLabel* w_loadCmd = new QLabel(tr("Load Command Report"));
		m_loadButton = new QPushButton(tr("Open")); // open file for reading command from report file 
		QVBoxLayout* w_readCmd = new QVBoxLayout;
		w_readCmd->addWidget(w_loadCmd);
		w_readCmd->addWidget(m_loadButton);
		//		w_readCmd->addStretch(3);
		w_buttonsTop->addLayout(w_readCmd);

		QLabel* w_createBon = new QLabel(tr("Cree Bon Livraison"));
		QVBoxLayout* w_bonCreator = new QVBoxLayout;
		w_bonCreator->addWidget(w_createBon);
		// 		QComboBox* w_listCbox = new QComboBox;
		// 		w_listCbox->addItems(w_listUniteAvailable);
		m_bonCreateReport = new QPushButton("Proceed");
		w_bonCreator->addWidget(m_bonCreateReport);
		//	w_bonCreator->addStretch(2);
		w_buttonsTop->addLayout(w_bonCreator);

		// Save selection
		QLabel* w_lblsaveSelect = new QLabel(tr("Save Selection"));
		QVBoxLayout* w_hboxsaveSelection = new QVBoxLayout;
		w_hboxsaveSelection->addWidget(w_lblsaveSelect);
		m_saveSelectBtn = new QPushButton("Save Select");
		w_hboxsaveSelection->addWidget(m_saveSelectBtn);
		//	w_hboxsaveSelection->addStretch(2);
		w_buttonsTop->addLayout(w_hboxsaveSelection);

		// load capacity mode
		QLabel* w_lblModeSelect = new QLabel(tr("Capacity Mode"));
		QVBoxLayout* w_hboxmodeSelection = new QVBoxLayout;
		w_hboxmodeSelection->addWidget(w_lblModeSelect);
		if (m_capacityLoad == eCapacityMode::normal) // default mode
		{
			m_cptySelectBtn = new QPushButton("Normal Load");
		}
		else
		{
			m_cptySelectBtn = new QPushButton("Degel Load");
		}
		w_hboxmodeSelection->addWidget(m_cptySelectBtn);
		w_buttonsTop->addLayout(w_hboxmodeSelection);

		// now set our box 
		//		m_creatorBox->setLayout(w_buttonsTop);

		return w_buttonsTop;
	}

	void BoDucReportCreator::remDuplicateAndSort(std::vector<bdAPI::BoDucFields>& aVecTotrim)
	{
		// need to check if we have duplicate (compare command number)
		std::sort(aVecTotrim.begin(), aVecTotrim.end(),
			[](bdAPI::BoDucFields& aField1, bdAPI::BoDucFields& aField2) -> bool
		{
			return std::greater<std::string>().operator()(aField1.m_noCmd, aField2.m_noCmd);
		});

		// remove all duplicate
		aVecTotrim.erase( std::unique(aVecTotrim.begin(), aVecTotrim.end()), aVecTotrim.cend());

		// trimming data before displaying in the GUI
		std::for_each( aVecTotrim.begin(), aVecTotrim.end(),
			[](bdAPI::BoDucFields& aBdField)
		{
			// Sorting dates represented as a string of the form YYYYMMDD 
			// or an integer whose decimal representation is YYYYMMDD is trivial. 
			// Just use std::sort and you will get the correct sort order.
			// First remove the back slash
			boost::algorithm::replace_all(aBdField.m_datePromise, R"(/)", "");
		});

#if _DEBUG
		std::cout << "Back slash removed\n";
		for (const auto& val : aVecTotrim)
		{
			std::cout << val;
		}
#endif
		// ready to sort by date since we have only unique command
		std::cout << "We are ready to sort our vector\n";
		std::sort(aVecTotrim.begin(), aVecTotrim.end()); // ascending order less recent to more recent
	  // need to reverse the order of the vector (descending order)
		std::reverse( std::begin(aVecTotrim), std::end(aVecTotrim)); // reverse the order more recent to less 

		// now we need to put back slash back
		auto putItBack = [](bdAPI::BoDucFields& aStr) // lambda
		{
			aStr.m_datePromise = aStr.m_datePromise.substr(0, 4) + R"(/)" +
				aStr.m_datePromise.substr(4, 2) + R"(/)" +
				aStr.m_datePromise.substr(6, 2);
		};
		std::for_each(aVecTotrim.begin(), aVecTotrim.end(), putItBack);
#if _DEBUG
		std::for_each(aVecTotrim.cbegin(), aVecTotrim.cend(), std::cout << boost::lambda::_1 << "\n");
#endif
	}
	void BoDucReportCreator::showCmd()
	{
		if( m_tblWidget->rowCount()!=0)
		{
			// delete all existing row in the table
			m_tblWidget->clearContents();
			m_tblWidget->setRowCount(0); // set everything to zero
		}

		// create a vector of BoDucFields cmd to show
		// switch case with 3 choices: all, date and date range
    // get date from selection
		// compare date with 
		remDuplicateAndSort(m_vectorOfCmd);

		// check box state to display mode 
    if( m_allDateCheck->isChecked())
    {
			m_displaySelect = eDisplayMode::all;
			m_vecOfCmd2Display = m_vectorOfCmd;
    }
		else if (m_date2Check->isChecked())
		{
			m_displaySelect = eDisplayMode::date;

			// retrieve the selected date in a given format 
			QDate w_testDate = m_dateSelect->date();
			QString w_fmtDate = w_testDate.toString(QString("yyyyMMdd"));
			// now select only those that satisfied the selected date
			for (const auto& val : m_vectorOfCmd)
			{
				std::string w_cpyDate = boost::replace_all_copy( val.m_datePromise, R"(/)", "");

				// loop on the cmd vector
				if (w_fmtDate.compare(QString(w_cpyDate.c_str())) == 0)
				{
					m_vecOfCmd2Display.push_back(val);
				}
			}
		}
		else if( m_rngDate2Check->isChecked())
		{
			m_displaySelect = eDisplayMode::dateRange;

			QDate w_minDate = m_dateMinSelected->date();
			QString w_fmtMinDate = w_minDate.toString(QString("yyyyMMdd"));
			QDate w_maxDate = m_dateMaxSelected->date();
			QString w_fmtMaxDate = w_maxDate.toString(QString("yyyyMMdd"));

			// now select only those that satisfied the selected date
			for( const auto& val : m_vectorOfCmd)
			{
				std::string w_cpyDate = boost::replace_all_copy(val.m_datePromise, R"(/)", "");

				// loop on the cmd vector
				if( QString(w_cpyDate.c_str()) >= w_fmtMinDate && QString(w_cpyDate.c_str()) <= w_fmtMaxDate)
				{
					m_vecOfCmd2Display.push_back(val);
				}
			}
		}
    		
		// debugging purpose
		switch (m_displaySelect)
		{
		case bdApp::BoDucReportCreator::eDisplayMode::all:
			// show all cmd in the report files
	//		m_vectorOfCmd = loadCmdReportInVector(w_filesName);
	//		remDuplicateAndSort(m_vectorOfCmd);
	//		m_vecOfCmd2Display = m_vectorOfCmd;
			fillTableWidget2Display();
			break;
		case bdApp::BoDucReportCreator::eDisplayMode::date:
			fillTableWidget2Display();
			break;
		case bdApp::BoDucReportCreator::eDisplayMode::dateRange:
			fillTableWidget2Display();
			break;
		default:
			break;
		}
		//m_vecOfCmd2Display.clear();  ready for next selection
	}

	void BoDucReportCreator::allDateChecked()
	{
		if (m_allDateCheck->checkState() == Qt::Unchecked)
		{
			m_allDateCheck->setCheckState(Qt::Checked);
		}
		else
		{
			m_allDateCheck->setCheckState(Qt::Unchecked);
		}
	}
  // clear all cmd in the display window
	void bdApp::BoDucReportCreator::clearDispalyedCmd()
	{
		// these are for debugging purpose 
// 		auto w_numberOfRows = m_tblWidget->rowCount();
// 		auto w_v2displaySiz = m_vecOfCmd2Display.size();
		m_tblWidget->clearContents();
		m_tblWidget->setRowCount(0); // call removeRows()

		// ... to be completed
// 		for( auto i=0; i<m_vecOfCmd2Display.size();++i)
// 		{
// 			for (auto j=0; j<m_tblWidget->columnCount();++j)
// 			{
// 				QTableWidgetItem* w_toDel = m_tblWidget->item(i, j);
// 				if (nullptr != w_toDel)
// 				{
// 					delete w_toDel;
// 				}
// 			}//item loop
// 			m_tblWidget->removeRow(i);
// 		}

		// debugging purpose (should have zero count)
		// if not, maybe set row count to 0, force it
		auto w_remainingRows = m_tblWidget->rowCount();
		// call m_tblWidget to retrieve number of rows
		// then call removeRow for each one
		m_vecOfCmd2Display.clear();
		auto vevcSize = m_vecOfCmd2Display.size();
		// again make sure that everything is set to 0 
		
		// need to uncheck selection box
		if (m_date2Check->isChecked())
		{
			m_date2Check->setCheckState(Qt::CheckState::Unchecked);
		}
		if(m_rngDate2Check->isChecked())
		{
			m_rngDate2Check->setCheckState(Qt::CheckState::Unchecked);
		}
		if (m_allDateCheck->isChecked())
		{
			m_allDateCheck->setCheckState(Qt::CheckState::Unchecked);
		}
	}
} // End of namespace
