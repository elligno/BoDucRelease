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
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHeaderView>
#include <QtCore/QFile>
#include <QDateEdit>
#include <QComboBox>
#include <QLabel>
//#include <QTextStream>
#include <QDateTime>
#include <QDirIterator>
#include <QFileDialog>
#include <QFont>
#include <QCheckBox>
#include <QProxyStyle>
// Boost includes
#include <boost/range/adaptors.hpp>
#include <boost/algorithm/string.hpp> // string algorithm
// Application Widget includes
#include "AnalyzerBoxWidget.h"
// Application includes
#include "BoDucReportCreator.h"
#include "BdAPI/BoDucUtility.h"
#include "BdAPI/BoDucBaseReport.h"
#include "BdAPI/BoDucBonLivraisonReport.h"

namespace {
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
	BoDucReportCreator::BoDucReportCreator( QWidget *parent/*=nullptr*/)
	: QWidget(parent),
    m_analyzerBox(Q_NULLPTR),
		m_tblWidget(Q_NULLPTR),
		m_listUnite(Q_NULLPTR),
		m_bonLivraisonFile("CommandReport.txt"),  // deprecated
    m_reportFile( QString("CommandReport.txt"))
//		m_displaySelect(eDisplayMode::all) // default (deprecated)
	{
		setWindowTitle("BoDucReportCreator");
   
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

		// deprecated (use vector)
		initMapUserSelection();

    // new version with BonLivraison type 
    createMapBonLivraison();

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

  // deprecated
	void BoDucReportCreator::initMapUserSelection()
	{
		// initialize some data structure ()
		auto i = 0;
		QStringList::const_iterator w_begList = m_listUniteAvailable.cbegin();
		while( w_begList != m_listUniteAvailable.cend())
		{
			QString w_unit = m_listUniteAvailable.value(i++);
			QStringList w_splittedStr = w_unit.split(" ");
			auto w_unitNo = w_splittedStr[1]; // white space should have 2 elements 

			// build map structure with key = unit number and value a vector of command field  
			// used to build "bon de livraison" report per unit (each unit has a "bon de livraison") 
			std::vector<bdAPI::BoDucFields> w_vecofSSaved;
			w_vecofSSaved.reserve(100);
			if( m_cmdUnitSaved.insert( std::make_pair( w_unitNo.toInt(), w_vecofSSaved)).second == false)
			{
				// logger some message such as could not insert the following item
				std::cout << "Could not insert cmd unit\n"; // should be a qDebug()
			}
			++w_begList;
		}
	}

  void BoDucReportCreator::createMapBonLivraison()
  {
    // initialize some data structure ()
    auto i = 0;
    QStringList::const_iterator w_begList = m_listUniteAvailable.cbegin();
    while( w_begList != m_listUniteAvailable.cend())
    {
      QString w_unit = m_listUniteAvailable.value(i++); /// unit xx format 
      QStringList w_splittedStr = w_unit.split(" ");
      auto w_unitNo = w_splittedStr[1]; // take second element of the split
      if( m_unitBonLivraison.insert( std::make_pair( w_unitNo.toInt(), bdAPI::BoDucBonLivraison(w_unitNo.toInt()))).second == false)
      {
        // logger some message such as could not insert the following item
        std::cout << "Could not insert cmd unit\n"; // should be a qDebug()
      }
      ++w_begList;
    }
  }

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
  }

  // Next version of the command selection saving mechanism
  // just testing the selection mechanism. Not sure about what is returned
  // since we have set the behavior to row (return the row) about the index?
  // In this use case, user have selected a command in the view and press 
  // "save" button.
  void BoDucReportCreator::saveUserSelection(QTableWidgetItem* aItem)
  {
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
    QModelIndexList w_mdlIdxList = m_tblWidget->selectionModel()->selectedIndexes();

    // ... to be completed
    bdAPI::BoDucFields w_bdFields;

    auto w_rowNo = 0;

    // Go through the list of model index (actually column)
    QListIterator<QModelIndex> w_listMdlIdxIter(w_mdlIdxList);
    while( w_listMdlIdxIter.hasNext())
    {
      // 0: check box (not a display role)
      // 1: hour (not a display role)
      // 2: all other item have a display role
      QModelIndex w_mdlIdx = w_listMdlIdxIter.next();  // discard not displayable

      // debugging purpose (first column which is the check box should return not valid)
      // test below shall check for !w_val.isValid() i.e. not valid
      QVariant w_valDbg = w_mdlIdx.data();
      w_rowNo = w_mdlIdx.row(); // row number selected by user
      auto checkDbg = m_tblWidget->rowCount(); // debugging purpose

      // correspond to what index? column format is the following:
      QVariant w_val = w_mdlIdx.data();
      if( !w_val.isValid() || w_mdlIdx.column() == 1) // first column are not valid data (check box )
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

      // change background to show that item has been selected (item selected background becomes red)
      // not sure because with the selection mechanism ON, once we have selected a command in table 
      // view, it is high lighted 
//       QTableWidgetItem* w_tblWgtItem = m_tblWidget->item(w_mdlIdx.row(),w_mdlIdx.column());
//       QBrush w_brush = w_tblWgtItem->background();
//       w_brush.setColor(Qt::darkRed);
    }//while-loop (column)

    // +++++++++++++++++++++++++++++++++++++++ new stuff
    // map of bon livraison
    bdAPI::BoDucBonLivraison& w_bonLivraison = m_unitBonLivraison[w_unitNo];
    w_bonLivraison.addCmd( w_bdFields);
    // +++++++++++++++++++++++++++++++++++++++

    // need to show 
    if( updateUnitProgress( w_bdFields.qty()))
    {
      //continue;  we don't want keep it in the cmd to display 
    // add the command to unit 
      w_vec.push_back(w_bdFields);

      // delete row from display widget and its items
      m_tblWidget->removeRow(w_rowNo); // row number user selection
                    
      // warn user that data sore updated, need to press 'ok'
      QMessageBox msgBox;
      msgBox.setText("Command selection saved.");
      msgBox.exec();
    }
    else // Exceed capacity
    {
      // warn user that data sore updated, need to press 'ok'
      QMessageBox msgBox;
      msgBox.setText("Command selection not saved.");
      msgBox.exec();
    }
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
	}

	// Design Note:
	//  this method will be refactored and renamed by   
	//  create report for each transport unit
	void BoDucReportCreator::createBonLivraison()
	{
    // input file dialog?? user select a folder to save bon livraison report 
    // this is a path or a name? i would say that's a path
    QString w_dirUserSelect = QFileDialog::getExistingDirectory( this, tr("Open Directory"),
      QDir::currentPath(),
      QFileDialog::ShowDirsOnly
      | QFileDialog::DontResolveSymlinks);
   
    QString w_uniteON = m_listUnite->currentText();
    QStringList w_splitedUnit = w_uniteON.split(" ");
    bdAPI::BoDucBonLivraison w_bonToReport = m_unitBonLivraison[w_splitedUnit[1].toUInt()];
    bdAPI::BonLivraisonReport w_test2CreateReport;
    w_test2CreateReport.setFolderName(w_dirUserSelect);
    w_test2CreateReport.createReport(w_bonToReport);

 
// 		QMessageBox::StandardButton reply;
// 		reply = QMessageBox::question(this, "Livraison Final", "",
// 			QMessageBox::Yes | QMessageBox::No);
// 		if( reply == QMessageBox::Yes) {
// 			// delete all commands
// 			w_unitVec.clear();
// // 			qDebug() << "Yes was clicked";
// // 			QApplication::quit();
// 		}
// 		else {
// 		//	qDebug() << "Yes was *not* clicked";
// 		}
	}

#if 0
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
      }//while-loop

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
#endif

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
    // These set methods allow the creation of the QModelIndex, how Qt store data.
    // In our case, since we have the selection row and single, it means model index 
    // is i = row and 0,...,8 = 9 columns
    // TableWidget (View) returns the selection model which in turn hold how data is managed.
    // This is done by the QModelIndex
		m_tblWidget->setSelectionBehavior( QAbstractItemView::SelectRows);   // fine, select the entire row
//		m_tblWidget->setSelectionMode(QAbstractItemView::MultiSelection);  
    m_tblWidget->setSelectionMode( QAbstractItemView::SingleSelection);  //single selection
		//m_tblWidget->setSelectionMode(QAbstractItemView::NoSelection);     items cannot be selected
		
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
        myTableWidgetItem = new QTableWidgetItem;     
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

        // According to Qt documentation of QTableWidget
        // table takes ownership of the item (responsible for its deletion).
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
    QLabel* w_listUniteLabl = new QLabel( tr("Unite Selectionne"));
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

    QLabel* w_lblsaveSelect = new QLabel( tr("Saved Selection"));
 //   QVBoxLayout* w_hboxsaveSelection = new QVBoxLayout;
    w_uniteComboBox->addWidget(w_lblsaveSelect);
    QPushButton* w_saveSelectBtn = new QPushButton("Show Bon Livraison");
    // need to connect to a slot that create a table widget
    w_uniteComboBox->addWidget(w_saveSelectBtn);
    w_buttonsBot->addLayout(w_uniteComboBox);    // takes the ownership??
    w_buttonsBot->addSpacing(50);

    QLabel* w_listUniteLabl1 = new QLabel(tr("Select Unit"));
//     QComboBox* w_listUnit1 = new QComboBox;
//     w_listUnit1->addItems(m_listUniteAvailable);
//     // set the current unite that is activated as default
//     int w_uniteCurrent1 = m_listUnite->currentIndex(); // user choice
//     QString w_currUnityON = m_listUnite->itemText(w_uniteCurrent1); // be carefull with this, need to be changed
    QPushButton* w_clearBon = new QPushButton(tr("Clear Bon Livraison"));
    //connect(w_clearBon,SIGNAL(clicked()),this,SLOT(clearCmdBon()));
    
    QVBoxLayout* w_uniteComboBox1 = new QVBoxLayout;
    w_uniteComboBox1->addWidget(w_listUniteLabl1);
    w_uniteComboBox1->addWidget(w_clearBon);
    w_uniteComboBox1->addSpacing(10);

    QLabel* w_createBonLbl = new QLabel( tr( "Bon Livraison"));
 //   QVBoxLayout* w_bonCreator = new QVBoxLayout;
    w_uniteComboBox1->addWidget(w_createBonLbl);
    // 		QComboBox* w_listCbox = new QComboBox;
    // 		w_listCbox->addItems(w_listUniteAvailable);
    QPushButton* w_bonCreateReport = new QPushButton("Create Report");
    QObject::connect( w_bonCreateReport, SIGNAL(clicked()), this, SLOT(createBonLivraison()));

    w_uniteComboBox1->addWidget(w_bonCreateReport);
    w_buttonsBot->addLayout(w_uniteComboBox1);    // takes the ownership??
    w_buttonsBot->addSpacing(50);

		QLabel* w_dateRngLbl = new QLabel(tr("Date Range"));
		QVBoxLayout* w_vDateRngBox = new QVBoxLayout;
    QDate w_minDate(QDate::currentDate());
		m_dateMinSelected = new QDateEdit;
		m_dateMinSelected->setCalendarPopup(true);
		m_dateMinSelected->setMinimumDate(w_minDate.addMonths(-72));
	  m_dateMaxSelected = new QDateEdit;
		m_dateMaxSelected->setCalendarPopup(true);
		m_dateMaxSelected->setMinimumDate(w_minDate.addMonths(-72));
		w_vDateRngBox->addWidget(w_dateRngLbl);
		w_vDateRngBox->addWidget(m_dateMinSelected);
		w_vDateRngBox->addWidget(m_dateMaxSelected);
		w_buttonsBot->addLayout(w_vDateRngBox);
    w_buttonsBot->addSpacing(50);

		QLabel* w_showLbl = new QLabel(tr("Show Selection"));
		QVBoxLayout* w_vBox2Show = new QVBoxLayout;
		m_showButton = new QPushButton(tr("Show Command Selection"));
		
		w_vBox2Show->addWidget(w_showLbl);
		w_vBox2Show->addWidget(m_showButton);
//		w_vBox2Show->addLayout(w_checkHBox);
//		w_buttonsBot->addLayout(w_vBox2Show);

		QLabel* w_clearLbl = new QLabel(tr("Clear Selection"));
//		QVBoxLayout* w_clearSelect = new QVBoxLayout;
		m_clearButton = new QPushButton(tr("Clear Command Selection"));
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

  // deprecated
  void BoDucReportCreator::createConnections()
	{
    QObject::connect(m_tblWidget,        SIGNAL(itemClicked(QTableWidgetItem*)),        this, SLOT(saveUserSelection(QTableWidgetItem*)));
    //		QObject::connect( m_tblWidget,       SIGNAL(cellChanged(int,int)),           this, SLOT(insertHour(int,int)));
		QObject::connect( m_showButton,      SIGNAL(clicked()),                      this, SLOT(showCmd()));
//		QObject::connect( m_clearButton,     SIGNAL(clicked()),                      this, SLOT(clearContents()));
		QObject::connect( m_clearButton,     SIGNAL(clicked()),                     this, SLOT(clearDispalyedCmd()));
	}

	void bdApp::BoDucReportCreator::setUnitCapacityLoad()
	{
    auto w_pbarUnitMapBeg = m_unitPbar.cbegin();
    auto w_pbarUnitMapEnd = m_unitPbar.cend();
    while( w_pbarUnitMapBeg != w_pbarUnitMapEnd)
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
    // not sure how it will be set?? think about it 
    // QDir::setCurrent(myPath); set by user??
    QString w_pathNow = QDir::current().absolutePath();
    m_reportFolder.setPath(w_pathNow);

#if 0 // just for debugging purpose
    QMessageBox msgBox;
    msgBox.setText(w_pathNow);
    msgBox.exec();
#endif
    
    // looking for report folder (file database) 
    while( !m_reportFolder.exists( QString("ReportFiles")))
    {
      m_reportFolder.cdUp();
    }
    m_reportFolder.cd( QString("ReportFiles"));
  }

  // clear all cmd in the display window
	void bdApp::BoDucReportCreator::clearDispalyedCmd()
	{
		m_tblWidget->clearContents();
		m_tblWidget->setRowCount(0); // call removeRows()
	}
} // End of namespace
