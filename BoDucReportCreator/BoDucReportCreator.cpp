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
//#include <QtCore/QTextStream>
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
#include "AnalyzerBoxWidget.h"
// Application includes
#include "BoDucReportCreator.h"
#include "BdAPI/BoDucFields.h"
#include "BdAPI/BoDucUtility.h"
#include "BdAPI/BoDucCmdFileReader.h"
#include "BdAPI/BoDucBaseReport.h"

namespace {
	constexpr bool greater_than( const bdAPI::BoDucFields& aField, const bdAPI::BoDucFields& aField2)
	{
		return aField.m_qty > aField2.m_qty;
	}

// 	std::vector<bdAPI::BoDucFields> loadCmdReportInVector( const QString& aFilepath)
// 	{
// 		std::ifstream infile( aFilepath.toStdString().c_str());
// 		if (!infile)
// 		{
// 			std::cerr << "Could not open file for reading\n";
// 		}
// 
// 		std::vector<bdAPI::BoDucFields> w_vecBD;
// 		w_vecBD.reserve(1000); // debugging purpose
// 		std::string line;
// 		std::vector<std::string> row_values;
// 		auto lineCounter = 0;
// 		while (std::getline(infile, line))
// 		{
// 			// header column name want to skip that
// 			if (lineCounter == 0) { lineCounter += 1; continue; }
// 			if (line.empty()) { continue; } // don't treat empty line
// 																			// ...
// 			if (!row_values.empty())
// 			{
// 				row_values.clear();
// 			}
// 			row_values.reserve(7); // number of fields
// 
// 			split(line, '\t', row_values);
// 
// 			// use the mode semantic of the vector
// 			long w_prodCode = atoi(row_values[3].c_str());
// 			float w_qty = atof(row_values[5].c_str());
// 			w_vecBD.push_back( bdAPI::BoDucFields( std::make_tuple(row_values[0], row_values[1], row_values[2],
// 				w_prodCode,row_values[4],w_qty,row_values[6])));
// 		}
// 		if (w_vecBD.capacity() > w_vecBD.size())
// 		{
// 			w_vecBD.shrink_to_fit();
// 		}
// 		return w_vecBD;
//	}

}// End of namespace

namespace bdApp 
{
	BoDucReportCreator::BoDucReportCreator(QWidget *parent)
	: QWidget(parent),
		m_tblWidget(Q_NULLPTR),
		m_listUnite(Q_NULLPTR),
		m_saveSelectBtn(Q_NULLPTR),
		m_bonCreateReport(Q_NULLPTR),
		m_bonLivraisonFile("CommandReport.txt"),  // deprecated
    m_reportFile( QString("CommandReport.txt")),
		m_displaySelect(eDisplayMode::all) // default (deprecated)
	{
		setWindowTitle("BoDucReportCreator");

    // testing purpose
//     QFileInfoList w_test = QDir::drives();
//     auto numOfDrives = w_test.size();
   
    // set some environment variable
    setReportFolder();

    // at start up we load the report file if it exist,
    // otherwise we have to create it???
    if( m_reportFolder.exists(m_reportFile.fileName()))
    {
      QFileInfo w_reportFileInfo(m_reportFolder, m_reportFile.fileName());
      m_vectorOfCmd = bdAPI::BoDucUtility::loadCmdReportInVector( w_reportFileInfo.filePath());
    }

		// set display formating (Widget table) 
		setupViews();

		// create all components for the GUI
		//createOtherWidgets(); // deprecated
		createLayout();
		createConnections(); // deprecated

		// fill all values load capacity for each unit
		unitLoadConfig();  // deprecated

		// set config of the progress bar 
		initProgressBar(); // deprecated

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
			if( m_cmdUnitSaved.insert( std::make_pair(w_unitNo.toInt(), w_vecofSSaved)).second == false)
			{
				// logger some message such as could not insert the following item
				std::cout << "Could not insert cmd unit\n"; // should be a qDebug()
			}
			++w_begList;
		}
	}

	// NOT COMPLETD *******************************
	// at the moment we support 10 unit just do a check on each of them
	// call when user select to save selection
	//TODO: deprecated
// 	void BoDucReportCreator::updateProgress()
// 	{
// 		if (m_currUnityON.compare(m_listUniteAvailable.at(0)) == 0) // identical
// 		{
// 			double w_val2Show = m_progressBar[0]->value() + std::get<2>(m_bdBonFields);
// 			m_progressBar[0]->setValue(w_val2Show);
// 		}
// 		else if (m_currUnityON.compare(m_listUniteAvailable.at(1)) == 0)
// 		{
// 			double w_val2Show = m_progressBar[1]->value() + std::get<2>(m_bdBonFields);
// 			m_progressBar[1]->setValue(w_val2Show);
// 		}
// 		else if (m_currUnityON.compare(m_listUniteAvailable.at(2)) == 0)
// 		{
// 			double w_val2Show = m_progressBar[2]->value() + std::get<2>(m_bdBonFields);
// 			m_progressBar[2]->setValue(w_val2Show);
// 		}
// 		else if (m_currUnityON.compare(m_listUniteAvailable.at(3)) == 0)
// 		{
// 			double w_val2Show = m_progressBar[3]->value() + std::get<2>(m_bdBonFields);
// 			m_progressBar[3]->setValue(w_val2Show);
// 		}
// 		else if (m_currUnityON.compare(m_listUniteAvailable.at(4)) == 0)
// 		{
// 			double w_val2Show = m_progressBar[4]->value() + std::get<2>(m_bdBonFields);
// 			m_progressBar[4]->setValue(w_val2Show);
// 		}
// 	}

	// Design Note (March 19)
	//  Can be done by using a while loop. While !=0 continue
	//  Once found, m_progressBar[i] whatever you want to do.
	//  Extract in a separate method value update and msg box.
	//  
	bool BoDucReportCreator::updateUnitProgress(const float aVal2Update)
	{
		bool ret = true;
    auto i = 0;
    for( const QString& w_unitNo : m_listUniteAvailable)
    {
      if (m_currUnityON.compare(m_listUniteAvailable.at(i)) == 0) // identical
      {
        double w_val2Show = m_progressBar[i]->value() + aVal2Update;
        if( w_val2Show > m_progressBar[i]->maximum())
        {
          QMessageBox msgBox;
          msgBox.setWindowTitle("Exceed Load");
          msgBox.setText("Maximum load is " + QString(std::to_string(m_progressBar[i]->maximum()).c_str()));
          msgBox.exec();
          ret = false;
        }
        m_progressBar[i]->setValue(w_val2Show);
        break;
      }
      ++i;
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
// 	void BoDucReportCreator::savetest()
// 	{
// 		updateProgress();
// 
// 		//QMessageBox(tr("Delete it from table"));
// 		QMessageBox msgBox;
// 		msgBox.setWindowTitle("Delete Command");
// 		msgBox.setText("Sure you want delete it?");
// 		msgBox.setStandardButtons(QMessageBox::Yes);
// 		msgBox.addButton(QMessageBox::No);
// 		msgBox.setDefaultButton(QMessageBox::No);
// 
// 		if (msgBox.exec() == QMessageBox::Yes) {
// 			m_tblWidget->removeRow(m_currowNoSelected); // row number
// 		}
// 		else {
// 			// do something else
// 		}
// 	}

  // connected to signal from AnalyzerBox button (load)  
  void BoDucReportCreator::updateVectorCommand( const std::vector<bdAPI::BoDucFields>& aVecOfCmd)
 {
    // ready to save to data store
    bdAPI::BoDucBaseReport w_tmpFile(m_reportFile);

    // write to report file
    w_tmpFile.createReport(aVecOfCmd);

    // update file database (load the whole file in memory)
    if (!m_vectorOfCmd.empty())
    {
      m_vectorOfCmd.clear(); //empty last cmds
    }

    // signal sent about new commands added to report file
    QFileInfo w_reportFileInfo(m_reportFolder, m_reportFile.fileName());
    m_vectorOfCmd = bdAPI::BoDucUtility::loadCmdReportInVector(w_reportFileInfo.filePath());
  }

  // DESIGN NOTE
  //  this is not the correct way to mananage user selection. There are types
  //  to help in this task (QItemSlectionModel, QItemSelection, QITemSelectionRange)
  //  that keep tracking of user selection. In the implementation above it is very 
  //  hard to keep tracking of these selection because the only relevant information
  //  comes from check box state.  
  //  We also retrieve QTableWidgetItem, no need to do that, by using QModelIndex
  //  we can retrieve the value that we look for (modelIdx->data()) with display role 
  //  default.
  //  Need to get selection form user and then retrieve data with the QItemSelectionModel,
  //  i am not sure but QItemSlectionModel keep track of all user selection (to be validated)
  //  this is a piece of crapt, will refactored using the Qt selection mechanism 
  // it will takes less code and more readable, because this is hardly understaneable!!! 
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

    // loop on all rows of the table view and search for checked box
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
// 				bdTpl w_boducTpl;
// 				QVector<QVariant> w_bdField;
// 				w_bdField.reserve(7);
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
	
  // Next version of the comand selection saving mechanism
  // just testing the selection mechanism. Not sure about what is returned
  // since we have set the behavior to row (return the row) about the index?
  // In this use case, user have selected a command in the view and press 
  // "save" button.
  void BoDucReportCreator::saveUserSelection()
  {
    // unit number
    const auto w_unitNo = m_currUnityON.split(" ")[1].toUInt(); // second is the unit number ("unit 30")

    // do we have a reference semantic here? good question!!
    std::vector<bdAPI::BoDucFields>& w_vec = m_cmdUnitSaved[w_unitNo];

    // ... to be completed
  //  QItemSelectionModel* w_selection = m_tblWidget->selectionModel();
    // retrieve the current selection
  //  QModelIndex w_currentUserSelect = w_selection->currentIndex();
    // data for this item
  //  QVariant w_value = w_currentUserSelect.data();

    // some stuff to be tested
    // select an entire row using an QItemSelection :
    // "select" is a slot of QQItemSelectionModel
//     w_selection->select(
//         QItemSelection(
//            m_tblWidget->model()->index( w_currentUserSelect.row(), 0),
//            m_tblWidget->model()->index( w_currentUserSelect.row(), m_tblWidget->model()->columnCount() - 1)),
//            QItemSelectionModel::Select);

    // what do we have here? index of column? yes it is
    // in a single selection, we have all columns index for a given row
    // we can retrieve the data item with the ... see testItemClick()
    QModelIndexList w_mdlIdx = m_tblWidget->selectionModel()->selectedIndexes();

    // Go through the list of model index
    QListIterator<QModelIndex> w_listMdlIdxIter(w_mdlIdx);
    while( w_listMdlIdxIter.hasNext())
    {
      // 0: check box (not a display role)
      // 1: hour (not a display role)
      // 2: all other item have a display role
      QModelIndex w_mdlIdx = w_listMdlIdxIter.next();  // discard not displayable
//       if( w_mdlIdx.flags() == Qt::ItemFlag::ItemIsEditable || 
//           w_mdlIdx.flags() ==  Qt::ItemFlag::ItemIsUserCheckable)
//       {
//         continue;
//       }

      // correspond to what index? column format is the following:
      bdAPI::BoDucFields w_bdFields;
      QVariant w_val = w_mdlIdx.data();
      if( w_val.isValid() || w_mdlIdx.column() == 1) // first column are not valid data (check box )
      {                                              // not of the logical minding!!!      
        continue;
      }

      // 
      switch( w_mdlIdx.column())
      {
      case 2: //command no
        // 
        w_bdFields.m_noCmd = w_val.toString().toStdString();
        break;
      case 3: //
        // shippedTo
        w_bdFields.m_deliverTo = w_val.toString().toStdString();
        break;
      case 4: //date  
        // date
        w_bdFields.m_datePromise = w_val.toString().toStdString();
        break;
      case 5: // product code
        w_bdFields.m_prodCode = w_val.toInt();
        break;
      case 6: // product descr.
        w_bdFields.m_produit = w_val.toString().toStdString();
        break;
      case 7: // quantity
        w_bdFields.m_qty = w_val.toFloat();
        break;
      case 8: //silo no
        w_bdFields.m_silo = w_val.toString().toStdString();
        break;
      default:
        break;
      }//switch

      // add the command to unit 
      w_vec.push_back(w_bdFields);

      // change background to show that item has been selected (item selected background becoems red)
      // not sure because with the selection mechanism ON, once we have selected a comand in table 
      // view, it is high lighted 
//       QTableWidgetItem* w_tblWgtItem = m_tblWidget->item(w_mdlIdx.row(),w_mdlIdx.column());
//       QBrush w_brush = w_tblWgtItem->background();
//       w_brush.setColor(Qt::darkRed);
    }
    
    // If you want to select a full row, you should use the following :
    // selection->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  }

  // Deprecated
  // loading commands from file (created by the parser)
  //  Design Note;
  //  At initialization all variables regarding working folder are set.
  //  This method is deprecated, this open button is not relevant, 
  // code will be moved in show button
// 	void BoDucReportCreator::open()
// 	{
		// create working folder (shall be an attribute of the class)
    // report folder is the file database location
		//QDir w_reportFolder;	 
// 		QString w_pathNow = QDir::current().absolutePath();
// 		m_reportFolder.setPath(w_pathNow);
//     // looking for report folder (file database) 
//     while (!m_reportFolder.exists(QString("ReportFiles")))
//     {
//       m_reportFolder.cdUp();
//     }
//     m_reportFolder.cd(QString("ReportFiles"));
// 
// 		// multiple file selection support
// 		// now opening file to be processed (name of the file returned)
//   	QString w_filesName;

//    QString w_filesName = QFileDialog::getOpenFileName(this, tr("Open File"),
//        m_reportFolder.absolutePath(),
// 			tr("Text (*.txt *.csv)"));

//#if 1  next version with sorted and duplicated vector
	//	std::vector<bdAPI::BoDucFields> w_vectorOfCmd = m_bdApp.getReportData();
		// we don' need that, because user is interested seeing data just processed
		// no need to store data in a file (they are already in a vector and you read
		// data from file and put it back in a vector). Unless user 
		// Actually, we need this file because Francois use it to retrieve data from 
		// pass days (for example a week ago), if we keep everything in memory, when 
		// app close we loose all the information (save on hard disk).
// 		if (!m_vectorOfCmd.empty())
// 		{
// 			m_vectorOfCmd.clear(); //empty last cmds
// 		}
		// show all cmd in the report files
//		m_vectorOfCmd = bdAPI::BoDucUtility::loadCmdReportInVector(w_filesName);
//		remDuplicateAndSort(m_vectorOfCmd);
//		fillTableWidget2Display();
// #endif
// 	}

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
  // in the next version: QSelectionItemModel 
	void BoDucReportCreator::testItemClick( QTableWidgetItem * aItem)
	{
		QTableWidget*  w_tblWgt = aItem->tableWidget();
		Q_ASSERT( w_tblWgt != Q_NULLPTR);
		QItemSelectionModel* selection = w_tblWgt->selectionModel();

		if( selection->hasSelection())
		{
			//return selected row
			QModelIndexList w_mdl = selection->selectedRows();
			auto w_rowSiz = w_mdl.size();
      QModelIndex w_testCheck = w_mdl.front();
      auto w_rowNo = w_testCheck.row(); // row number selected by user
      QVariant w_datTest = w_testCheck.data();

      QModelIndexList w_colIdx = selection->selectedIndexes();
      auto w_someSiz = w_colIdx.size();
      bdAPI::BoDucFields w_dataStruct;
      QListIterator<QModelIndex> w_colIdxIter(w_colIdx);
      while( w_colIdxIter.hasNext())
      {
        QModelIndex w_idx = w_colIdxIter.next();
        auto w_flags = w_idx.flags();
        if( w_idx.flags() != Qt::ItemFlag::ItemIsUserCheckable)
        {
          QVariant w_dat = w_idx.data();
          if( w_dat.isValid()) // first column are not valid data
          {                    // check box and editable item (empty for now)
            continue;
          }

          // set boduc field
          // command no
          w_dataStruct.m_noCmd = w_dat.toString().toStdString();
          // shipped to
          //
        }
      }

      //
      // code below is ok but not the way to do it, Qt provides types 
      // to retrieve data (see code above)
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

  // Refactoring the selection model 
  // This is the right way to do it. QItemSelectionModel and QItemSelection
  // types to manage user selection (right way to do it)
  // In the current implementation (saveCmdSelection()) we check
  // checkBox state (checked | notChecked) and retireve QWidgetItem  
  //
  // User select by double clicking item ()
  void BoDucReportCreator::userDblClickSelection(QTableWidgetItem* aItem)
  {
    QItemSelectionModel* w_selection = aItem->tableWidget()->selectionModel();

    // some stuff to be tested
    // select an entire row using an QItemSelection :
    // "select" is a slot of QQItemSelectionModel
    //     selection->select(
    //       QItemSelection(
    //         w_tblWgt->model()->index(rowIdx, 0),
    //         w_tblWgt->model()->index(rowIdx, w_tblWgt->model()->columnCount() - 1)),
    //       QItemSelectionModel::Select);

    // If you want to select a full row, you should use the following :
    // selection->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Rows);
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
		m_tblWidget = new QTableWidget(0/*first row*/, 9/*number of columns*/, this); //

		m_tblWidget->setHorizontalHeaderLabels(QStringList() //<< tr("Selected")
			<< tr("Status")
			<< tr("Hour")
			<< tr("Command No")
			<< tr("Shipped To")
			<< tr("Date")
			<< tr("Code")
			<< tr("Product")
			<< tr("Quantity")
			<< tr("Silo"));
		
    m_tblWidget->verticalHeader()->setVisible(false);
		m_tblWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		m_tblWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // selection behavior and mode determine how user selection is manage.
    // not sure yet, but once user have selected an item, press button "save selection"
    // and slot function is call ""
		m_tblWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   // fine, select the entire row
//		m_tblWidget->setSelectionMode(QAbstractItemView::MultiSelection); //
    //m_tblWidget->setSelectionMode(QAbstractItemView::SingleSelection);    single selection
		m_tblWidget->setSelectionMode(QAbstractItemView::NoSelection);    // items cannot be selected
		
    m_tblWidget->setShowGrid(true);
		m_tblWidget->setGeometry(QRect(0, 100, 781, 281));
		m_tblWidget->horizontalHeader()->resizeSection(1, 250);  // not sure about all these 3 below
		m_tblWidget->horizontalHeader()->resizeSection(2, 250);
		m_tblWidget->horizontalHeader()->resizeSection(3, 180);
		//m_tblWidget->setStyleSheet("QTableView {selection-background-color: red;}");
	}

  void BoDucReportCreator::fillTableWidget2Display()
	{
    //+++++++++++++++++++++++++++++++++++++++++++ Testing purpose ++++++++++++++
    // TableWidget its a model/view (default model) provided by Qt framework 
    // a simple 2-table to display information ()
    // Actually our table widget its probably what we need, if this is
    // the case 
    QAbstractItemModel* w_checkDefaultModel = m_tblWidget->model();
    QAbstractTableModel* w_isAbstractTblMdl = dynamic_cast<QAbstractTableModel*>(w_checkDefaultModel);
    if( nullptr != w_isAbstractTblMdl)
    {
      const QModelIndex& w_chekModelIdx = w_checkDefaultModel->index(0, 2); // just debugging
      if (w_chekModelIdx.isValid())
      {
        auto w_row = w_chekModelIdx.row();
        QVariant w_data = w_chekModelIdx.data();
        QItemSelectionModel* w_selctionMdl = m_tblWidget->selectionModel(); // 
      }
    }

    // sanity check
    if( nullptr != w_checkDefaultModel)
    {
      const QModelIndex& w_chekModelIdx  = w_checkDefaultModel->index(0,2); // just debugging
      QItemSelectionModel* w_selctionMdl = m_tblWidget->selectionModel(); // 
    }
    //++++++++++++++++++++++++++++++++++++++++++ End testing +++++++++++++++++++++

		// set number of row to the size of the display vector
		m_tblWidget->setRowCount( m_vecOfCmd2Display.size()); 
		auto w_remainingRows = m_tblWidget->rowCount();

   	// fill displayed table with displayed vector of cmd 
		//QList<QVector<QVariant>> w_listOfVariant = fromBDFieldToQVariant(m_vectorOfCmd);
		QList<QVector<QVariant>> w_listOfVariant = bdAPI::BoDucUtility::fromBDFieldToQVariant(m_vecOfCmd2Display);

		auto i = 0; // create each row of the table
		for( const QVector<QVariant>& w_vecVariant : w_listOfVariant)
		{
      QTableWidgetItem* myTableWidgetItem = Q_NULLPTR;
      for (auto j = 0; j < m_tblWidget->columnCount(); ++j)
      {
        myTableWidgetItem = new QTableWidgetItem;     // i am not sure about this one
        // case  j==0, user check box in the first column
        if (j == 0)
        {
          //	myTableWidgetItem->setData(Qt::Unchecked, Qt::CheckStateRole);
          myTableWidgetItem->setFlags( myTableWidgetItem->flags() | Qt::ItemIsUserCheckable);
          myTableWidgetItem->setCheckState(Qt::Unchecked);
          m_tblWidget->setItem(i, 0, myTableWidgetItem);
          continue;
        }
        // for column 1 (heure) user edit this field and part of the bon de livraison
        if (j == 1)
        {
          myTableWidgetItem->setData(Qt::EditRole, QVariant("0 am"));
          m_tblWidget->setItem(i, j, myTableWidgetItem);
          continue;
        }
        //myTableWidgetItem->data(Qt::CheckStateRole);   
        myTableWidgetItem->setData(Qt::DisplayRole, w_vecVariant[j - 2]);
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

    // replace all those lines above by these below, can i do that?
    QPushButton* w_loadCsvButton = new QPushButton( QString("Load .csv files"));
    QFont w_font = w_loadCsvButton->font();
    w_font.setPixelSize(15);
    w_loadCsvButton->setFont(w_font);
    
    w_rowLayout->addWidget( w_loadCsvButton); // stretch =0 and Qt::Alignment=alignment
    // connect signal/slot message 
    connect( w_loadCsvButton, SIGNAL(clicked()), this, SLOT(loadCsvFiles()));

    QPushButton* w_loadPdfButton = new QPushButton( QString("Load .pdf files"));
    w_font = w_loadPdfButton->font();
    w_font.setPixelSize(15);
    w_loadPdfButton->setFont(w_font);
    w_rowLayout->addWidget(w_loadPdfButton); // see comment above
    // connect signal/slot message
    connect( w_loadPdfButton, SIGNAL(clicked()), this, SLOT(loadPdfFiles()));

    // load type(normal/degel) 
    QPushButton* w_capacitySelectButton = new QPushButton("Normal Load");
    w_font = w_capacitySelectButton->font();
    w_font.setPixelSize(15);
    w_capacitySelectButton->setFont(w_font);
    if( m_capacityLoad != eCapacityMode::normal) // default mode
    {
      w_capacitySelectButton->setText( QString("Degel Load"));
    }

    w_rowLayout->addWidget(w_capacitySelectButton); // see comment above
    connect( w_capacitySelectButton, &QPushButton::clicked, [this, w_capacitySelectButton]
    {
      if (m_capacityLoad == eCapacityMode::normal)
      {
        w_capacitySelectButton->setText( QString("Degel Load"));
        m_capacityLoad = eCapacityMode::degel;
      }
      else
      {
        w_capacitySelectButton->setText( QString("Normal Load"));
        m_capacityLoad = eCapacityMode::normal;
      }
    });

		// settings (combo box to set some of the path needed to make the application work)
    // need pdfminer path (installation of python 2.9 is required and set the path of 
    // the exec to make it work and convert pdf to text)
    QPushButton* w_settingsButton = new QPushButton(QString("Settings"));
    w_font = w_settingsButton->font();
    w_font.setPixelSize(15);
    w_settingsButton->setFont(w_font);
    w_rowLayout->addWidget(w_settingsButton);
    connect( w_settingsButton, SIGNAL(clicked()), this, SLOT(settingPath()));

		// set layout of this box (does Group box taking ownership of the layout??)
    QGroupBox* w_analyzerBox = new QGroupBox;
		w_analyzerBox->setLayout(w_rowLayout);

		return w_analyzerBox;
	}

	// This need to be refactored, new buttons will be added
	// Current design we create 2 horizontal box and ...
	// Want a Box call Panel and another one call Report
	// Separate functionalities in different box
	QGroupBox* BoDucReportCreator::createBonLivraisonBox()
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
		m_dateMinSelected->setMinimumDate(w_minDate.addMonths(-48));
	  m_dateMaxSelected = new QDateEdit;
		m_dateMaxSelected->setCalendarPopup(true);
		m_dateMaxSelected->setMinimumDate(w_minDate.addMonths(-48));
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

    // Bon livraison group  
    QGroupBox* w_creatorBox = new QGroupBox(tr("Bon De Livraison"));
		// now set our box 
		w_creatorBox->setLayout(w_gridLayout);

		return w_creatorBox;
	}

  // Main layout of the widget
 	void BoDucReportCreator::createLayout()
	{
		QVBoxLayout* w_mainLayout = new QVBoxLayout;
		//take ownership of the passed pointer (layout is responsible for deletion)  
    // see QLayout documentation mention that addWidget() use addItem()
    // which takes the ownership of the pointer
		w_mainLayout->addWidget( createAnalyzerBox());
 //   w_mainLayout->addWidget( new AnalyzerBoxWidget()); next version
    AnalyzerBoxWidget* w_analyzeBox = new AnalyzerBoxWidget;
    connect( w_analyzeBox, SIGNAL( commandReportUpdated( const std::vector<bdAPI::BoDucFields>&)),
      this, SLOT( updateVectorCommand( const std::vector<bdAPI::BoDucFields>&)));

		w_mainLayout->addWidget( createBonLivraisonBox());
		w_mainLayout->addWidget( createProgressBarBox() /*m_uniteBox*/);
		w_mainLayout->addWidget( setTableWidgetBox());
		//take ownership 
		w_mainLayout->addLayout( setBottomButtons()); // should addWidget instead
		w_mainLayout->setGeometry( QRect(600, 400, 0, 0));
		setLayout(w_mainLayout); // take ownership
	}

	QBoxLayout* BoDucReportCreator::setBottomButtons()
	{
		// buttons at the bottom of the dialog
    // create buttons at the bottom 
    QDialogButtonBox* w_dialogButtonBox = new QDialogButtonBox;
    QPushButton* w_closeButton = w_dialogButtonBox->addButton(QDialogButtonBox::Close);
    if( nullptr != w_closeButton) // success adding button
    {
      QObject::connect( w_closeButton, SIGNAL(clicked()), this, SLOT(close()));
    }

    QHBoxLayout* w_horizontalDglLayout = new QHBoxLayout;
    w_horizontalDglLayout->addWidget(w_dialogButtonBox);

    // NOTE shall create a group box and  return the pointer   
    // might need to add a stretcher to put the button at the far end right
    // not sure

		return w_horizontalDglLayout;
	}

	QGroupBox* BoDucReportCreator::createProgressBarBox()
	{
		QVBoxLayout* w_vlayoutPbar = new QVBoxLayout;
		// create a layout for progress bar in a vertical layout
		QHBoxLayout* w_hProgressBar1 = new QHBoxLayout;
    for( auto i = 0;i < 5; ++i)
    {
      addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(i).toStdString().c_str());
    }
		w_vlayoutPbar->addLayout(w_hProgressBar1);
		
    QHBoxLayout* w_hProgressBar2 = new QHBoxLayout;
    for( auto i = 5; i < 10; ++i)
    {
      addProgressBar(w_hProgressBar1, m_listUniteAvailable.at(i).toStdString().c_str());
    }
		w_vlayoutPbar->addLayout(w_hProgressBar2);

		// maybe create a group box and return it
    QGroupBox* w_unitBarBox = new QGroupBox;
    w_unitBarBox->setLayout(w_vlayoutPbar);

    return w_unitBarBox;
	}

  void BoDucReportCreator::addProgressBar( QBoxLayout* aProgressBar, const std::string& aUniteNb)
	{
    // i am not sure i understand the code below, i do not think it's used?
    // need to check
		if( QVBoxLayout* w_lablandLayout1 = dynamic_cast<QVBoxLayout*>(aProgressBar))
		{
			// ...
			QLabel* w_barU1 = new QLabel( tr(aUniteNb.c_str()));
			m_progressBar[0] = new QProgressBar;

			// Both label and button together
			//			QVBoxLayout* w_lablandLayout1 = new QVBoxLayout;
			w_lablandLayout1->addWidget(w_barU1);
			w_lablandLayout1->addWidget(m_progressBar[0]);
			aProgressBar->addStretch(2);  // visual effect?
			aProgressBar->addLayout(w_lablandLayout1);
		}
		else if( QHBoxLayout* w_lablandLayout1 = dynamic_cast<QHBoxLayout*>(aProgressBar))
		{
			QLabel* w_barlbl = new QLabel( tr(aUniteNb.c_str()));
//			short i = 0;
		//	static_assert(i < 10 && "");

// 			for (const QString& w_unit : m_listUniteAvailable)
// 			{
				if (m_unitIndex >= 0 && m_unitIndex < m_nbUnit)
				{
					m_progressBar[m_unitIndex] = new QProgressBar;
          // just a test (may want to put vertical)
          Qt::Orientation w_testOrientation = m_progressBar[m_unitIndex]->orientation(); 
          if( m_progressBar[m_unitIndex]->orientation() == Qt::Horizontal)
          {
            w_lablandLayout1->addWidget(w_barlbl);
            w_lablandLayout1->addWidget(m_progressBar[m_unitIndex++]);
            aProgressBar->addStretch(1);
            aProgressBar->addLayout(w_lablandLayout1);
          }
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

  QGroupBox* BoDucReportCreator::setTableWidgetBox()
	{
    QGroupBox* w_cmdBox = new QGroupBox(tr("Liste de Commandes"));
		QVBoxLayout* w_viewLayout = new QVBoxLayout;
		w_viewLayout->addWidget(m_tblWidget);
		w_cmdBox->setLayout(w_viewLayout); // add it to Group box
    return w_cmdBox;
	}

  void BoDucReportCreator::loadPdfFiles()
  {
    // setting default directory to start with 
    // i need to check, which is the root directory of the VS15 project
 //   QString w_defaultDir = QDir::currentPath();

    // now opening file to be processed (name of the file returned)
    m_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
      m_defaultDir,
      tr("Text (*.txt *.csv *.pdf)"));

    // parse file to extract command
    bdAPI::BoDucParser w_fileParser;

    if (!w_fileParser.isFileExtOk(m_filesName)) {
      QMessageBox();
      return;
    }

    // check for transporteur name
//    if (w_fileParser.transporteurNameValid(m_filesName, m_lisTransporteur))
//     {
//     }

   // if(!w_checkFileExt)
    //   		convertPdf2Txt();
  }

	// called when "open" button clicked
	// by default all files for testing are located in a folder named "DataToTest"
	void BoDucReportCreator::loadCsvFiles()
	{
    using namespace boost;

    // no need to do that, it's done when reading file,
    // transporteur name is checked and command discarded 
    /// if does not appear.
//     std::initializer_list<std::string> w_lisTransporteur =
//     {
//       "NIR R-117971-3 TRSP CPB INC",
//       "NIR R-117971-3 TRANSPORT CPB",
//       "NIR R-117971-3 C.P.B.",
//       "BODUC- ST-DAMASE",
//       "NIR R-004489-2 TR. BO-DUC"     //sometime we have empty (blank field) string
//     };

    // now opening files to be processed (user selection)
    QStringList m_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
      m_defaultDir,
      tr("Text (*.txt *.csv *.pdf)"));

    // parse file to extract command
    bdAPI::BoDucParser w_fileParser;

    // check for file extension valid
    if( !w_fileParser.isFileExtOk(m_filesName)) {
      QMessageBox();
      return;
    }

    // parse each commands file 
    std::list<std::string> w_listFilesName;
    QStringList::const_iterator w_begList = m_filesName.constBegin();
    QStringList::const_iterator w_endList = m_filesName.constEnd();
    while( w_begList != w_endList)
    {
      //  std::cout << "Loading the following files: " << w_begList->toStdString() << std::endl;
      w_listFilesName.push_back(w_begList->toStdString());
      ++w_begList;
    }
 
    // loop on each files in the list (fill vector map for processing multiple files in one step)
    // each map contains all command readed from a file (reminder: a can contains 1 or more command)
    auto w_vecofCmd = bdAPI::BoDucCmdFileReader::readFiles( w_listFilesName, std::string("Signature"));

    // number of files selected by user
 		m_fileLoaded = w_vecofCmd.size();
// 		m_bdApp.setNbSelectedFiles(m_fileLoaded);

    // debugging purpose
    std::vector<bdAPI::BoDucFields> w_reportCmd;

    // This represent command extracted from files selected by user.
    // Each file is a map of vector of string (text rep of a command)
    // ready to parse all command and save it to data store
    // list of all files selected by user, check for each command in a file
    for( auto begVecMap = w_vecofCmd.cbegin(); begVecMap != w_vecofCmd.cend(); ++begVecMap)
    {
      // Initialize first command for this file
      // this map represent a file with all the commands  
      std::map<int, std::vector<std::string>>::const_iterator w_cmdFileBeg = begVecMap->cbegin();

      // go through all command in this file (map(int,vecstr))
      while( w_cmdFileBeg != (*begVecMap).cend())
      {
        // list of commands for a given file 
        std::vector<std::string> w_checkCmd = (*w_cmdFileBeg).second;

        // check for some blank lines when converting from pdf to text
        if( std::any_of( w_checkCmd.cbegin(), w_checkCmd.cend(), [](const std::string& aStr) { return aStr.empty(); }))
        {
          w_checkCmd.erase( std::remove_if( w_checkCmd.begin(), w_checkCmd.end(), // delete empty element
             []( const std::string& s) // check if an empty string
             {
                return s.empty();
             }), w_checkCmd.cend());
        }

        // maybe merge those 2 if
        if( !w_fileParser.hasAnyTM_TON(w_checkCmd))
        {
          ++w_cmdFileBeg; // ready for next command
          continue;
        }

        // Need to check for tonnage tags: "TM" or "TON"
//         if( !w_fileParser.isTransporteurNameValid(w_checkCmd, w_lisTransporteur))
//         {
//           ++w_cmdFileBeg; // ready for next command
//           continue;
//         }

        // use the std any algorithm (with a lambda as the predicate to find
        //if any of those are present in files) 
        auto bdField = w_fileParser.extractData(w_checkCmd);
        w_reportCmd.push_back(bdField);
        ++w_cmdFileBeg; // ready for next command 
      }//while-loop
    }//for-loop

    // ready to save to data store
    //bdApp::FileDataStore w_dataStore; 
    //QFile w_reportFile( QString( "commandReport.txt"));
    bdAPI::BoDucBaseReport w_tmpFile(m_reportFile);

    // write to report file
    w_tmpFile.createReport(w_reportCmd);

    // update file database (load the whole file in memory)
    if( !m_vectorOfCmd.empty())
    {
      m_vectorOfCmd.clear(); //empty last cmds
    }

    // update command vector (db has been updated signal)
    // since new command has been added in file data store we have to reload  
    QFileInfo w_reportFileInfo( m_reportFolder, m_reportFile.fileName());
    m_vectorOfCmd = bdAPI::BoDucUtility::loadCmdReportInVector(w_reportFileInfo.filePath());
	}

  void bdApp::BoDucReportCreator::settingPath()
  {
    throw "Not implemented yet";
  }

  // deprecated
  void BoDucReportCreator::createConnections()
	{
		QObject::connect( m_bonCreateReport, SIGNAL(clicked()),                      this, SLOT(createBonLivraison()));
//		QObject::connect( m_listUnite,       SIGNAL(activated(int)),                 this, SLOT(currentUniteON()));
		//QObject::connect( m_saveSelectBtn,   SIGNAL(clicked()),                      this, SLOT(savetest())); deprecated
		QObject::connect(m_saveSelectBtn,    SIGNAL(clicked()),                      this, SLOT(saveCmdSelection()));
//		QObject::connect( m_cptySelectBtn,   SIGNAL(clicked()),                      this, SLOT(setMode()));
		QObject::connect( m_tblWidget,       SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(testItemClick(QTableWidgetItem*)));
//		QObject::connect( m_tblWidget,       SIGNAL(cellChanged(int,int)),           this, SLOT(insertHour(int,int)));
		QObject::connect( m_showButton,      SIGNAL(clicked()),                      this, SLOT(showCmd()));
//		QObject::connect( m_clearButton,     SIGNAL(clicked()),                      this, SLOT(clearContents()));
		QObject::connect( m_clearButton,      SIGNAL(clicked()),                     this, SLOT(clearDispalyedCmd()));
//		QObject::connect(m_clearButton,      SIGNAL(clicked()),                      this, SLOT(removeRow(int)));
		QObject::connect( m_allDateCheck,     SIGNAL(stateChanged()),                this, SLOT(allDateChecked())); // deprecated
	}

	void bdApp::BoDucReportCreator::unitLoadConfig()
	{
    // convention (tuple 3-element)
    // unit no, normal load, degel load -> std::make_tuple( 30,  17.,  14.)
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
		const QString w_pdfilesPath = R"(F:\EllignoContract\BoDuc\QtTestGui\BoDucReportCreator\BoDucReportCreator\Data)";  //pdf files folder
    //const QString w_pdfilesPath = R"(F:\EllignoContract\BoDuc\QtTestGui\BoDucReportCreator\BoDucReportCreator\Data\DataValidation)";  debugging purpose

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
		while( filesListIterator.hasNext())
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

  // this is deprecated
  QBoxLayout* BoDucReportCreator::createHBoxLayoutRow1()
	{
		// Create a first row of buttons
		QHBoxLayout* w_buttonsTop = new QHBoxLayout;

		// List of unite available
		QLabel* w_listUniteLabl = new QLabel(tr("Unite Selectionne"));
    m_listUnite = new QComboBox;
    m_listUnite->addItems(m_listUniteAvailable);
    connect( m_listUnite, SIGNAL( activated(int)), this, SLOT(currentUniteON()));

		// set the current unite that is activated as default
		int w_uniteCurrent = m_listUnite->currentIndex(); // user choice
		m_currUnityON = m_listUnite->itemText(w_uniteCurrent);

    QVBoxLayout* w_uniteComboBox = new QVBoxLayout;
		w_uniteComboBox->addWidget(w_listUniteLabl);
		w_uniteComboBox->addWidget(m_listUnite);
		//	w_uniteComboBox->addStretch(1);
		w_buttonsTop->addLayout(w_uniteComboBox);


// 		QLabel* w_loadCmd = new QLabel(tr("Load Command Report"));
// 		QPushButton* w_loadButton = new QPushButton(tr("Open")); // open file for reading command from report file 
//     QObject::connect(w_loadButton, SIGNAL(clicked()), this, SLOT(open()));

//     QVBoxLayout* w_readCmd = new QVBoxLayout;
// 		w_readCmd->addWidget(w_loadCmd);
// 		w_readCmd->addWidget(w_loadButton);
// 		//		w_readCmd->addStretch(3);
// 		w_buttonsTop->addLayout(w_readCmd);

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
// 		QLabel* w_lblModeSelect = new QLabel(tr("Capacity Mode"));
// 		QVBoxLayout* w_hboxmodeSelection = new QVBoxLayout;
// 		w_hboxmodeSelection->addWidget(w_lblModeSelect);
// 		if (m_capacityLoad == eCapacityMode::normal) // default mode
// 		{
// 			m_cptySelectBtn = new QPushButton("Normal Load");
// 		}
// 		else
// 		{
// 			m_cptySelectBtn = new QPushButton("Degel Load");
// 		}
// 		w_hboxmodeSelection->addWidget(m_cptySelectBtn);
// 		w_buttonsTop->addLayout(w_hboxmodeSelection);

		// now set our box 
		//		m_creatorBox->setLayout(w_buttonsTop);

		return w_buttonsTop;
	}

  // Design Note:
  //   next version refactor keep only date range (start=end a given day,  
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

    // to be removed
		auto myVec = bdAPI::BoDucUtility::remDuplicateAndSort(m_vectorOfCmd);

		// check box state to display mode 
    if( m_allDateCheck->isChecked())
    {
			m_displaySelect = eDisplayMode::all;
			m_vecOfCmd2Display = bdAPI::BoDucUtility::remDuplicateAndSort(m_vectorOfCmd); //m_vectorOfCmd;
    }
		else if( m_date2Check->isChecked())
		{
			m_displaySelect = eDisplayMode::date;

			// retrieve the selected date in a given format 
			QDate w_testDate = m_dateSelect->date();
			QString w_fmtDate = w_testDate.toString(QString("yyyyMMdd"));
			// now select only those that satisfied the selected date
			for( const auto& val : bdAPI::BoDucUtility::remDuplicateAndSort(m_vectorOfCmd) /*m_vectorOfCmd*/)
			{
				std::string w_cpyDate = boost::replace_all_copy( val.m_datePromise, R"(/)", "");

				// loop on the cmd vector
				if( w_fmtDate.compare(QString(w_cpyDate.c_str())) == 0)
				{
					m_vecOfCmd2Display.push_back(val);
				}
			}//for-loop
		}
		else if( m_rngDate2Check->isChecked())
		{
			m_displaySelect = eDisplayMode::dateRange;

			QDate w_minDate = m_dateMinSelected->date();
			QString w_fmtMinDate = w_minDate.toString(QString("yyyyMMdd"));
			QDate w_maxDate = m_dateMaxSelected->date();
			QString w_fmtMaxDate = w_maxDate.toString(QString("yyyyMMdd"));

			// now select only those that satisfied the selected date
			for( const auto& val : bdAPI::BoDucUtility::remDuplicateAndSort(m_vectorOfCmd) /*m_vectorOfCmd*/)
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

  // not sure what  we do here!!
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

  void BoDucReportCreator::setReportFolder()
  {
    QString w_pathNow = QDir::current().absolutePath();
    m_reportFolder.setPath(w_pathNow);
    
    // looking for report folder (file database) 
    while (!m_reportFolder.exists( QString("ReportFiles")))
    {
      m_reportFolder.cdUp();
    }
    m_reportFolder.cd(QString("ReportFiles"));
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
