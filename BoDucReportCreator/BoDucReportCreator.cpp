// C++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <forward_list>
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
#include <QProxyStyle>
// Boost includes
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
// Application Widget includes
#include "AnalyzerBoxWidget.h"
// Application includes
#include "BoDucReportCreator.h"
#include "BdAPI/BoDucFields.h"
#include "BdAPI/BoDucUtility.h"
#include "BdAPI/BoDucCmdFileReader.h"
#include "BdAPI/BoDucBaseReport.h"

namespace {

  // Pdfminer tool (convert pdf files to text format)
  //  Reference web: installation procedure 
//  const QString g_pythonScript = R"(F:\EllignoContract\BoDuc\pdfminerTxt\pdfminer-20140328\build\scripts-2.7\pdf2txt.py)";


	constexpr bool greater_than( const bdAPI::BoDucFields& aField, const bdAPI::BoDucFields& aField2)
	{
		return aField.m_qty > aField2.m_qty;
	}

//   display percentage in vertical style (Progress bar)
//     A fix could be using a QProxyStyle
//     (fast made, give it more love if used for real)
//     set with
  // Usage 
//     ui->progressBar->setStyle(new Progressproxy);
  class Progressproxy : public QProxyStyle
  {
  public:
    Progressproxy() : QProxyStyle() {}
  public:
    virtual void drawControl( ControlElement element, const QStyleOption *opt, QPainter *p,
      const QWidget *w) const override
    {

      if (element == CE_ProgressBarLabel) {
        const QStyleOptionProgressBar *real = qstyleoption_cast<const QStyleOptionProgressBar*>(opt);
        // check real is not null...
        QStyleOptionProgressBar pb = *real;
        pb.rect = QRect(0, 0, w->width(), w->height());
        proxy()->drawItemText( p, pb.rect, Qt::AlignCenter, pb.palette,
                               pb.state & State_Enabled, pb.text, QPalette::Text);
        return;
      }

      QProxyStyle::drawControl(element, opt, p, w);
    }
  };
}// End of namespace

namespace bdApp 
{
	BoDucReportCreator::BoDucReportCreator(QWidget *parent)
	: QWidget(parent),
    m_analyzerBox(Q_NULLPTR),
		m_tblWidget(Q_NULLPTR),
		m_listUnite(Q_NULLPTR),
		m_saveSelectBtn(Q_NULLPTR),
		m_bonCreateReport(Q_NULLPTR),
		m_bonLivraisonFile("CommandReport.txt"),  // deprecated
    m_reportFile( QString("CommandReport.txt"))
//		m_displaySelect(eDisplayMode::all) // default (deprecated)
	{
		setWindowTitle("BoDucReportCreator");

    // testing purpose
//     QFileInfoList w_test = QDir::drives();
//     auto numOfDrives = w_test.size();
   
    // set some environment variable
    setReportFolder();

    // at start up we load the report file if it exist,
    // otherwise we have to create it??? 
    // file data store loaded in memory, ...
    if( m_reportFolder.exists( m_reportFile.fileName()))
    {
      QFileInfo w_reportFileInfo(m_reportFolder, m_reportFile.fileName());
      m_vectorOfCmd = bdAPI::BoDucUtility::loadCmdReportInVector( w_reportFileInfo.filePath());
    }

		// set display formating (Widget table) and user selection config  
		setupViews();

		// create all components for the GUI
		//createOtherWidgets(); // deprecated
    // main layout of the main window (in this implementation it is not the case)
    // but in the future the application inherit of the main widow
		createLayout(); 
		createConnections(); // deprecated

    // initialize load capacity for each unit according to capacity mode (default is normal)
    setUnitCapacityLoad();

		// ...
		initMapUserSelection();

		// create the user interface
		ui.setupUi(this);
	}

  BoDucReportCreator::~BoDucReportCreator()
  {
    // delete map element progress bar
    std::for_each( m_unitPbar.begin(),m_unitPbar.end(),
      [] ( auto aPbar)
    {
      if( std::is_pointer<decltype(aPbar.second)>::value) 
      {
        if( nullptr != aPbar.second)
        {
          delete aPbar.second;
          aPbar.second = nullptr;
        }
      }
    });
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
	bool BoDucReportCreator::updateUnitProgress( const float aVal2Update)
	{
		bool ret = true;
    // Unit xx format, xx unit no
    auto w_unitNo = m_currUnityON.split(" ")[1].toInt(); 
    double w_val2Show = m_unitPbar[w_unitNo]->value() + aVal2Update;
    if( w_val2Show > m_unitPbar[w_unitNo]->maximum())
    {
      QMessageBox msgBox;
      msgBox.setWindowTitle("Exceed Load");
      msgBox.setText("Maximum load is " + QString(std::to_string(m_unitPbar[w_unitNo]->maximum()).c_str()));
      msgBox.exec();
      ret = false;
    }
    m_unitPbar[w_unitNo]->setValue(w_val2Show);

		return ret;
	}

	// IMPORTANT maximum value for each unit is determined by the time period (normal/degel)
	// There is a document "Unite + capacite de charge.pdf" where specs for each mode is defined. 
	// Each unit has its own value
	// just configuring the progress bar to display
// 	void BoDucReportCreator::initProgressBar()
// 	{
// 		setUnitCapacityLoad();
// 	}

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
  void BoDucReportCreator::updateFileDataStore()
 {
    // ready to save to data store
    bdAPI::BoDucBaseReport w_tmpFile(m_reportFile);

    auto w_vecOfCmd = m_analyzerBox->getCommandLoaded();
    
    // write to report file (should it be update report)
    w_tmpFile.createReport( w_vecOfCmd);

    // warn user that data sore updated, need to press 'ok'
    QMessageBox msgBox;
    msgBox.setText("File Data Store has been updated.");
    msgBox.exec();

    // update file database (load the whole file in memory)
    if( !m_vectorOfCmd.empty())
    {
      m_vectorOfCmd.clear(); //empty last cmds
    }

    // signal sent about new commands added to report file
    QFileInfo w_reportFileInfo(m_reportFolder, m_reportFile.fileName());
    m_vectorOfCmd = bdAPI::BoDucUtility::loadCmdReportInVector(w_reportFileInfo.filePath());

    std::cout << "\n";
  }

  // DESIGN NOTE
  //  this is not the correct way to manage user selection. There are types
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
 				}//for-loop (column)

				// need to show 
				if( !updateUnitProgress(std::get<5>(w_bdTuple)))
				{
					continue; // we don't want keep it in the cmd to display 
				}

 				 // need to fill vector with data (vector to display)
        // NOTE i do not think that i move anything here, because 'w_bdTuple' is a "lvalue"
        // can't bind 'lvalue' to 'rvalue', std::move is not relevant or has no effect 
        // need to check first but shall remove the std::move  
 				w_vec.push_back( bdAPI::BoDucFields( std::move(w_bdTuple)));
 			}//if
			
       // delete row from display widget and its items
			m_tblWidget->removeRow(rCount);          // row number
			auto checkDbg = m_tblWidget->rowCount(); // debugging purpose
 		}//for-loop (row)
  }
	
  // Next version of the command selection saving mechanism
  // just testing the selection mechanism. Not sure about what is returned
  // since we have set the behavior to row (return the row) about the index?
  // In this use case, user have selected a command in the view and press 
  // "save" button.
  void BoDucReportCreator::saveUserSelection()
  {
    // unit number (avoid creating a working variable)
    //std::vector<bdAPI::BoDucFields>& w_vec = m_cmdUnitSaved[m_listUnite->itemText(m_listUnite->currentIndex()).split(" ")[1].toUInt()];

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

// If you want to select a full row, you should use the following :
// selection->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Rows);

    // what do we have here? index of column? yes it is
    // in a single selection, we have all columns index for a given row
    // we can retrieve the data item with the ... see testItemClick()
    QModelIndexList w_mdlIdx = m_tblWidget->selectionModel()->selectedIndexes();

    // Go through the list of model index (actually column)
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

      // debugging purpose (first column which is the check box should return not valid)
      // test below shall check for !w_val.isValid() i.e. not valid
      QVariant w_valDbg = w_mdlIdx.data();

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

      // change background to show that item has been selected (item selected background becomes red)
      // not sure because with the selection mechanism ON, once we have selected a command in table 
      // view, it is high lighted 
//       QTableWidgetItem* w_tblWgtItem = m_tblWidget->item(w_mdlIdx.row(),w_mdlIdx.column());
//       QBrush w_brush = w_tblWgtItem->background();
//       w_brush.setColor(Qt::darkRed);
    }//while-loop (column)
    
    // If you want to select a full row, you should use the following :
    // selection->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  }

  // slot (temporary until we move all stuff of the progress bar in a separate widget)
  void BoDucReportCreator::setCapacityMode( /*eCapacityMode aCptyMode*/)
  {
    // At startup the application capacity mode is set to normal,
    // if user change to degel, then to reset capacity load parameters 
    if( m_capacityLoad == eCapacityMode::normal)
    {
      m_capacityLoad = eCapacityMode::degel;
      setUnitCapacityLoad();
    }
    else
    {
      m_capacityLoad = eCapacityMode::normal;
      setUnitCapacityLoad();
    }
  }

  // design Note: maybe return the unit number instead of the string representation
  // this way avoid to split each time the string to retrieve the unit no
  void BoDucReportCreator::currentUniteON()
	{
		// user change unity selection
		int w_uniteCurrent = m_listUnite->currentIndex(); // user choice
		m_currUnityON = m_listUnite->itemText(w_uniteCurrent);

    // string format is: "unit xx", 
    //  split about white space and return last element converted to uint 
    //return m_listUnite->itemText(w_uniteCurrent).split(" ")[1].toUInt();
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
	void BoDucReportCreator::testItemClick( QTableWidgetItem* aItem)
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

      // iterate through ModelIndex (selection)
      // currently we support single selection model
      // list contains only one element, but eventually 
      // we will support multiple selection
      QListIterator<QModelIndex> w_colIdxIter( w_colIdx);
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

	// This need to be refactored, new buttons will be added
	// Current design we create 2 horizontal box and ...
	// Want a Box call Panel and another one call Report
	// Separate functionalities in different box
	QGroupBox* BoDucReportCreator::createBonLivraisonBox()
	{
		// ... to be completed
		QHBoxLayout* w_buttonsBot = new QHBoxLayout;

		// List of unite available
// 		QLabel* w_dateLabl = new QLabel(tr("Date Selectionne"));
// 		QVBoxLayout* w_date2show = new QVBoxLayout;
// 		m_dateSelect = new QDateEdit;
// 		m_dateSelect->setCalendarPopup(true); // not what it does?
// 		QDate w_minDate(QDate::currentDate());
// 		//w_minDate.addMonths(-6);
// 		m_dateSelect->setMinimumDate(w_minDate.addMonths(-8));
// 
// 		w_date2show->addWidget(w_dateLabl);
// 		w_date2show->addWidget(m_dateSelect);

// List of unite available
    QLabel* w_listUniteLabl = new QLabel(tr("Unite Selectionne"));
    QFont w_font = w_listUniteLabl->font();
    w_font.setPixelSize(17);
    w_listUniteLabl->setFont(w_font);
    m_listUnite = new QComboBox;
    m_listUnite->addItems(m_listUniteAvailable);
    connect( m_listUnite, SIGNAL(activated(int)), this, SLOT(currentUniteON()));

    // set the current unite that is activated as default
    int w_uniteCurrent = m_listUnite->currentIndex(); // user choice
    m_currUnityON = m_listUnite->itemText(w_uniteCurrent);

    QVBoxLayout* w_uniteComboBox = new QVBoxLayout;
    w_uniteComboBox->addWidget(w_listUniteLabl);
    w_uniteComboBox->addWidget(m_listUnite);
    w_uniteComboBox->addSpacing(20);
		// horizontal box layout 
		//w_buttonsBot->addLayout(w_uniteComboBox);

    QLabel* w_lblsaveSelect = new QLabel( tr("Save Selection"));
 //   QVBoxLayout* w_hboxsaveSelection = new QVBoxLayout;
    w_uniteComboBox->addWidget(w_lblsaveSelect);
    m_saveSelectBtn = new QPushButton("Save Select");
    w_uniteComboBox->addWidget(m_saveSelectBtn);
    w_buttonsBot->addLayout(w_uniteComboBox);    // takes the ownership??
    w_buttonsBot->addSpacing(50);

    QLabel* w_listUniteLabl1 = new QLabel(tr("Select Unit"));
    QComboBox* w_listUnit1 = new QComboBox;
    w_listUnit1->addItems(m_listUniteAvailable);
    // set the current unite that is activated as default
    int w_uniteCurrent1 = m_listUnite->currentIndex(); // user choice
    QString w_currUnityON = m_listUnite->itemText(w_uniteCurrent1); // be carefull with this, need to be changed

    QVBoxLayout* w_uniteComboBox1 = new QVBoxLayout;
    w_uniteComboBox1->addWidget(w_listUniteLabl1);
    w_uniteComboBox1->addWidget(w_listUnit1);
    w_uniteComboBox1->addSpacing(10);

    QLabel* w_createBonLbl = new QLabel(tr("Cree Bon Livraison"));
 //   QVBoxLayout* w_bonCreator = new QVBoxLayout;
    w_uniteComboBox1->addWidget(w_createBonLbl);
    // 		QComboBox* w_listCbox = new QComboBox;
    // 		w_listCbox->addItems(w_listUniteAvailable);
    QPushButton* w_bonCreateReport = new QPushButton("Proceed");
    w_uniteComboBox1->addWidget(w_bonCreateReport);
    w_buttonsBot->addLayout(w_uniteComboBox1);    // takes the ownership??
    w_buttonsBot->addSpacing(50);

		QLabel* w_dateRngLbl = new QLabel(tr("Date Range"));
		QVBoxLayout* w_vDateRngBox = new QVBoxLayout;
    QDate w_minDate(QDate::currentDate());
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
    w_buttonsBot->addSpacing(50);

		QLabel* w_showLbl = new QLabel(tr("Show Selection"));
		QVBoxLayout* w_vBox2Show = new QVBoxLayout;
		m_showButton = new QPushButton(tr("Show"));
		
    // Date **************************
//     QHBoxLayout* w_checkHBox = new QHBoxLayout;
// 		m_date2Check = new QCheckBox;
// 		m_date2Check->setCheckable(true);
// 		m_date2Check->setCheckState(Qt::Unchecked);
// 		w_checkHBox->addWidget(m_date2Check);
// 		QLabel* w_dateHLbl = new QLabel(tr("Date"));
// 		w_checkHBox->addWidget(w_dateHLbl);
//     w_checkHBox->addSpacing(50);
// 		
// 		m_rngDate2Check = new QCheckBox;
// 		m_rngDate2Check->setCheckable(true);
// 		m_rngDate2Check->setCheckState(Qt::Unchecked);
// 		w_checkHBox->addWidget(m_rngDate2Check);
// 		QLabel* w_rngDateHLbl = new QLabel(tr("Range Date"));
// 		w_checkHBox->addWidget(w_rngDateHLbl);
//     w_checkHBox->addSpacing(50);

// 		w_vBox2Show->addWidget(w_showLbl);
// 		w_vBox2Show->addWidget(m_showButton);
// 		w_vBox2Show->addLayout(w_checkHBox);
// 		w_buttonsBot->addLayout(w_vBox2Show);

// 		m_allDateCheck = new QCheckBox;
// 		m_allDateCheck->setCheckable(true);
// 		m_allDateCheck->setCheckState(Qt::Unchecked);
// 		w_checkHBox->addWidget(m_allDateCheck);
// 		QLabel* w_allDataLbl = new QLabel(tr("All date"));
// 		w_checkHBox->addWidget(w_allDataLbl);
//     w_checkHBox->addStretch();
    // Date **************************

		w_vBox2Show->addWidget(w_showLbl);
		w_vBox2Show->addWidget(m_showButton);
//		w_vBox2Show->addLayout(w_checkHBox);
//		w_buttonsBot->addLayout(w_vBox2Show);

		QLabel* w_clearLbl = new QLabel(tr("Clear Selection"));
//		QVBoxLayout* w_clearSelect = new QVBoxLayout;
		m_clearButton = new QPushButton(tr("Clear"));
    w_vBox2Show->addWidget(w_clearLbl);
    w_vBox2Show->addWidget(m_clearButton);
		w_buttonsBot->addLayout(w_vBox2Show);
    w_buttonsBot->addSpacing(50);

    // only one row
// 		QGridLayout* w_gridLayout = new QGridLayout;
// 		w_gridLayout->addLayout(w_row1, 0, 0);
// 		w_gridLayout->addLayout(w_buttonsBot, 1, 0);

    // Bon livraison group  
    QGroupBox* w_creatorBox = new QGroupBox(tr("Bon De Livraison"));
    QFont w_crBoxFont = w_creatorBox->font();
    w_crBoxFont.setPixelSize(20);
    w_creatorBox->setFont(w_crBoxFont);
		
    // now set our box 
		//w_creatorBox->setLayout(w_gridLayout);
    w_creatorBox->setLayout(w_buttonsBot);

		return w_creatorBox;
	}

  // Main layout of the widget
 	void BoDucReportCreator::createLayout()
	{
		QVBoxLayout* w_mainLayout = new QVBoxLayout;
		//take ownership of the passed pointer (layout is responsible for deletion)  
    // see QLayout documentation mention that addWidget() use addItem()
    // which takes the ownership of the pointer
	//	w_mainLayout->addWidget( createAnalyzerBox());

    //   w_mainLayout->addWidget( new AnalyzerBoxWidget()); next version
    m_analyzerBox = new AnalyzerBoxWidget(this);
    
    // app notify when user finished loaded new files to be processed 
    w_mainLayout->addWidget(m_analyzerBox);

    connect(m_analyzerBox, SIGNAL( commandLoaded()),
      this, SLOT( updateFileDataStore()));

    // progress notify when user change capacity load (normal/degel)
    connect(m_analyzerBox, SIGNAL(capacityLoadChanged(/*eCapacityMode*/)), 
      this, SLOT(setCapacityMode(/*eCapacityMode*/)));

		w_mainLayout->addWidget( createBonLivraisonBox());
		w_mainLayout->addWidget( createProgressBarBox() /*m_uniteBox*/);
		// make use of the new widget for progress bar
    
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
    // create a layout for progress bar in a vertical layout
		QHBoxLayout* w_hProgressBar = new QHBoxLayout;

    auto i = 0; 
    for( const auto& key : m_unitLoadCapacity | boost::adaptors::map_keys) 
    {
      auto w_pbarNo = m_listUniteAvailable.at(i).split(" ")[1].toInt();
      if( key==w_pbarNo) // check if we have same unit
      {
        QLabel* w_pbarLabel = new QLabel(tr(m_listUniteAvailable.at(i++).toStdString().c_str()));
        QFont w_pbarFont = w_pbarLabel->font();
        w_pbarFont.setPixelSize(17);
        w_pbarFont.setBold(true);
        w_pbarLabel->setFont(w_pbarFont);
        m_unitPbar[key] = new QProgressBar; // insert in the map
//        Qt::Orientation w_testOrientation = m_unitPbar[key]->orientation();
        m_unitPbar[key]->setOrientation(Qt::Vertical);
        m_unitPbar[key]->setTextVisible(true);
        QVBoxLayout* w_vlayoutPbar = new QVBoxLayout;
        w_vlayoutPbar->addWidget(w_pbarLabel);
        w_vlayoutPbar->addWidget(m_unitPbar[key]);
        w_hProgressBar->addLayout(w_vlayoutPbar); // take the ownership of vlayout
      }
    }
    
    // maybe create a group box and return it
    QGroupBox* w_unitBarBox = new QGroupBox( tr("Unit Capacity"));
    QFont w_font = w_unitBarBox->font();
    w_font.setPixelSize(20);
    w_unitBarBox->setFont(w_font);
    w_unitBarBox->setFixedHeight(150); // capacity box height fixed, should use a variable constexpr
    w_unitBarBox->setLayout(w_hProgressBar); //take the ownership of hlayout

    return w_unitBarBox;
	}

  QGroupBox* BoDucReportCreator::setTableWidgetBox()
	{
    QGroupBox* w_cmdBox = new QGroupBox(tr("Liste de Commandes"));
    QFont w_font = w_cmdBox->font();
    w_font.setPixelSize(20);
    w_cmdBox->setFont(w_font);
		QVBoxLayout* w_viewLayout = new QVBoxLayout;
		w_viewLayout->addWidget(m_tblWidget);
		w_cmdBox->setLayout(w_viewLayout); // add it to Group box
    return w_cmdBox;
	}

#if 0
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
    bdAPI::BoDucParser w_fileParser(bdAPI::BoDucParser::eFileType::pdf);

//     QMessageBox msgError;
//     msgError.setText("CRITICAL ERROR!\nThe McRib is no longer available!");
//     msgError.setIcon(QMessageBox::Critical);
//     msgError.setWindowTitle("Danger");
//     msgError.exec();

    // check for supported file extension
    if( !w_fileParser.isFileExtOk(m_filesName)) {
      QMessageBox::StandardButton reply;
      reply = QMessageBox::warning( this, "Wrong file extension selected", "Need to select pdf file?",
        QMessageBox::Yes | QMessageBox::No);
      if (reply == QMessageBox::Yes) {
        m_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
          m_defaultDir,
          tr("Text (*.txt *.csv *.pdf)"));
      }
      else { // user doesn't want to continue
        //	qDebug() << "Yes was *not* clicked";
        QApplication::quit();
      }
    }

    // convert pdf to txt format (swap pdf to txt)
    // Design Note: add a test to check if conversion succeed?
    // return a bool ? true : false or the list of converted files (QStringList)
    // need to call setQProcessEnv()
    QProcessEnvironment w_procEnv = QProcessEnvironment::systemEnvironment();
    w_procEnv.insert( "PYTHONPATH", "C:\\Python27\\Lib");
    w_procEnv.insert( "PYTHONHOME", "C:\\Python27");

    // convert pdf to txt format
    QStringList w_filesConverted = 
      bdAPI::BoDucUtility::convertPdf2Txt( m_filesName, g_pythonScript, w_procEnv, this);

    //QFile::remove();
    // move file to different folder
//     file.rename( g_pdfilesPath+"/"+ QString("DataValidation")+ "/" +fileInfo.fileName());
//     auto complPath = fileInfo.absoluteFilePath();
//     QDir w_wrkDir(g_pdfilesPath);
//     QFileInfo w_finfo(*w_begList);
//     auto fileName = w_finfo.fileName();
//     QFile w_file( w_wrkDir.absolutePath() + "/" + fileName);
//     QFileInfo w_checkWholePath(w_file);
//     auto fileCheck = w_checkWholePath.absoluteFilePath();

//     while (w_begList != w_endList)
//     {
//       //  std::cout << "Loading the following files: " << w_begList->toStdString() << std::endl;
//       w_listFilesName.push_back( w_begList->toStdString());
//       ++w_begList;
//     }

    // read files (...) and return list of files (text format) 
    auto w_vecofCmd = bdAPI::BoDucCmdFileReader::readFiles( w_filesConverted);

#if _DEBUG
    // deleting txt file (these are temporaries, so might as well to delete it)
    QStringListIterator w_listConvertFilesIter(w_filesConverted);
    while( w_listConvertFilesIter.hasNext())
    {
      QFile file(w_listConvertFilesIter.next());
      QFileInfo fileInfo(file);
      if( QFile::remove( fileInfo.absoluteFilePath()))
      {
        // we definitively need a log file
        std::cout << "Could not remove file from location\n";
      }
    }
#endif // 

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
        if (std::any_of(w_checkCmd.cbegin(), w_checkCmd.cend(), [](const std::string& aStr) { return aStr.empty(); }))
        {
          w_checkCmd.erase(std::remove_if(w_checkCmd.begin(), w_checkCmd.end(), // delete empty element
            [](const std::string& s) // check if an empty string
          {
            return s.empty();
          }), w_checkCmd.cend());
        }

        // maybe merge those 2 if
        if (!w_fileParser.hasAnyTM_TON(w_checkCmd))
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

    // Since we have changed the file data store, we need to reload it
    // clear cache memory before the re-load and then reload the whole file
    // update file database (load the whole file in memory)
    if( !m_vectorOfCmd.empty())
    {
      m_vectorOfCmd.clear(); //empty last cmds
    }

    // update command vector (db has been updated signal)
    // since new command has been added in file data store we have to reload  
    QFileInfo w_reportFileInfo(m_reportFolder, m_reportFile.fileName());
    m_vectorOfCmd = bdAPI::BoDucUtility::loadCmdReportInVector( w_reportFileInfo.filePath());
  }
#endif

  // deprecated
  void BoDucReportCreator::createConnections()
	{
		QObject::connect( m_bonCreateReport, SIGNAL(clicked()),                      this, SLOT(createBonLivraison())); // deprecated
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
//		QObject::connect( m_allDateCheck,     SIGNAL(stateChanged()),                this, SLOT(allDateChecked())); // deprecated
	}

	void bdApp::BoDucReportCreator::setUnitCapacityLoad()
	{
    auto w_pbarUnitMapBeg = m_unitPbar.cbegin();
    auto w_pbarUnitMapEnd = m_unitPbar.cend();
    while (w_pbarUnitMapBeg != w_pbarUnitMapEnd)
    {
      if (!w_pbarUnitMapBeg->second->isEnabled())
      {
        w_pbarUnitMapBeg->second->setEnabled(true);
      }
      if (w_pbarUnitMapBeg->second->value()!=0)
      {
        w_pbarUnitMapBeg->second->setValue(0);
      }
      if (w_pbarUnitMapBeg->second->minimum()!=0)
      {
        w_pbarUnitMapBeg->second->setMinimum(0);
      }
      if (m_capacityLoad == eCapacityMode::normal)
      {
        w_pbarUnitMapBeg->second->setMaximum(m_unitLoadCapacity[w_pbarUnitMapBeg->first].first); // normal load: pair(normal,degel) which is first 
      }
      else // degel
      {
        w_pbarUnitMapBeg->second->setMaximum(m_unitLoadCapacity[w_pbarUnitMapBeg->first].second); // degel load: pair(normal,degel) which is second
      }
      ++w_pbarUnitMapBeg;
    }//while-loop
	}

  // Design Note:
  //   next version refactor keep only date range (start=end a given day,  
	void BoDucReportCreator::showCmd()
	{
		if( m_tblWidget->rowCount()!=0)
		{
			// delete all existing row in the table (need to do it?)
      // since we set the row count to zero?? see below
			m_tblWidget->clearContents();
      
      // when setting row count and the size is less than
      // table row count, the unwanted is discarded, in this 
      // case the whole table row is discarded, what we want
			m_tblWidget->setRowCount(0); // set everything to zero
		}                              

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
      fillTableWidget2Display();
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
		m_tblWidget->clearContents();
		m_tblWidget->setRowCount(0); // call removeRows()
	}
} // End of namespace
