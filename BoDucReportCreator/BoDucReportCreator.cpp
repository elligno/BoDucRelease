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
#include <QComboBox>
#include <QProcess>
#include <QLabel>
#include <QTextStream>
#include <QDateTime>
#include <QDirIterator>
#include <QFileDialog>
#include <QFont>
// Application include
#include "BoDucReportCreator.h"

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
				if (i == 3)
				{
					int w_int = atoi(v.c_str());
					w_vec2Fill.push_back(QVariant(w_int));
				}
				else if (i == 5)
				{
					// double value
					double w_dbl = atof(v.c_str());
					w_vec2Fill.push_back(QVariant(w_dbl));
				}
				else if (i == 6)
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
		m_console(Q_NULLPTR),
		m_bonCreateReport(Q_NULLPTR),
		m_bonLivraisonFile("CommandReport.txt")
	{
		setWindowTitle("BdApp Bon De Livraison (creator)");

		// set display formating (Widget table) 
		setupViews();

		// create all components for the GUI
		createOtherWidgets();
		createLayout();
		createConnections();

		// set config of the progress bar 
		initProgressBar();

		ui.setupUi(this);
	}

	// at the moment we support 4-5 unit just do a check on each of them
	// call when user select to save selection
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

	// just configuring the progress bar to display
	void BoDucReportCreator::initProgressBar()
	{
		// create all progress bar
		for( auto i=0; i<m_numPbar;++i)
		{
			if (!m_progressBar[i]->isEnabled())
			{
				m_progressBar[i]->setEnabled(true);
			}
			if (m_progressBar[i]->value() != 0)
			{
				// set initial value
				m_progressBar[i]->setValue(0.);
			}
			if (m_progressBar[i]->minimum() != 0)
			{
				m_progressBar[i]->setMinimum(0);
			}
			if (m_progressBar[i]->maximum() != 10)
			{
				m_progressBar[i]->setMaximum(10);
			}
		}
	}

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
		// for now we are just testing, so we fill
		// some data to test our implementation
		//createTableWidget();
	}
	// loading commands form file (created by the parser)
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
		
		m_read4test = readFileTab(w_filesName); // from file
		createTableWidget();
	}

  void BoDucReportCreator::currentUniteON()
	{
		// user change unity selection
		int w_uniteCurrent = m_listUnite->currentIndex(); // user choice
		m_currUnityON = m_listUnite->itemText(w_uniteCurrent);
	}

	// create report for each transport unit
	void BoDucReportCreator::createBonReport()
	{
		//unite ON
		QString w_uniteON = m_listUnite->currentText();

		// If you want to retrieve all the values for a single key, 
		// you can use values(const Key &key), which returns a QList<T>:

		//QDir w_reportFolder; 
		QString w_path = QDir::currentPath();
		QString w_bonLivraisonFile = w_uniteON + "_BonLiveraison.txt";
		QFileInfo w_fileRep(w_path, w_bonLivraisonFile);
		// create a new file with the given name
		QFile w_data(w_fileRep.absoluteFilePath());
		QList<tplbonlivraison> values = m_unitBonLivraisonData.values(w_uniteON);
		if (!w_fileRep.exists(w_bonLivraisonFile))
		{
			QTextStream out(&w_data);
			// then create it write report
			if (w_data.open(QFile::WriteOnly | QFile::Text))
			{
				for (int i = 0; i < values.size(); ++i)
				{
					tplbonlivraison w_val2File = values.at(i);
					out << std::get<0>(w_val2File) << "\t" << std::get<1>(w_val2File) << "\t"
						<< std::get<2>(w_val2File) << "\t" << std::get<3>(w_val2File) << "\t" << "\n";
				}
			}
		}
		else
		{
			// open and append it
			if (w_data.open(QFile::WriteOnly | QFile::Text | QFile::Append))
			{
				QTextStream out(&w_data);
				for (int i = 0; i < values.size(); ++i)
				{
					tplbonlivraison w_val2File = values.at(i);
					out << std::get<0>(w_val2File) << "\t" << std::get<1>(w_val2File) << "\t"
						  << std::get<2>(w_val2File) << "\t" << std::get<3>(w_val2File) << "\t" << "\n";
				}
			}
		}
		w_data.close();
	}

	// selected a row with mouse
	void BoDucReportCreator::testItemClick( QTableWidgetItem * aItem)
	{
		QTableWidget*  w_tblWgt = aItem->tableWidget();
		Q_ASSERT(w_tblWgt != Q_NULLPTR);
		QItemSelectionModel* select = w_tblWgt->selectionModel();
		if (select->hasSelection())
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
			for (auto col = 0; col < w_tblWgt->columnCount(); ++col)
			{
				QTableWidgetItem* w_chckItem = w_tblWgt->item(m_currowNoSelected, col);
				QVariant w_val = w_chckItem->data(Qt::DisplayRole);
				// BoDuc selection
				if (col == 1)
				{
					std::get<0>(m_bdBonFields) = w_val.toString();
				}
				else if (col == 4)
				{
					std::get<1>(m_bdBonFields) = w_val.toString();
				}
				else if (col == 5)
				{
					std::get<2>(m_bdBonFields) = w_val.toDouble();
				}
				else if (col == 6)
				{
					std::get<3>(m_bdBonFields) = w_val.toInt();
				}

				w_cmd2Report.push_back(w_val);
				// add it to the list
			}

			// just a test
			w_checkMap.insert(m_currowNoSelected, m_bdBonFields);
			// store all data selected by user to be retrieved 
			// when creating bon de livraison (file format) 
			// key is the unit name that is currently selected
			m_unitBonLivraisonData.insert(m_currUnityON, m_bdBonFields);
		}
	}
	void BoDucReportCreator::setupViews()
	{
		m_tblWidget = new QTableWidget(200, 7, this); //
		m_tblWidget->setHorizontalHeaderLabels(QStringList() //<< tr("Selected")
			<< tr("No Command")
			<< tr("Shipped To")
			<< tr("Delivery date")
			<< tr("Code")
			<< tr("Product")
			<< tr("Quantity")
			<< tr("Silo"));
		m_tblWidget->verticalHeader()->setVisible(false);
		m_tblWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		m_tblWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_tblWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tblWidget->setSelectionMode(QAbstractItemView::SingleSelection);
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

	void BoDucReportCreator::createTableWidget()
	{
		auto i = 0; // create each row of the table
		for (const QVector<QVariant>& w_vecVariant : m_read4test)
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
	// Top group box, actually it's the original buttons layout
	// Open, load, save, process (buttons)
	QGroupBox* BoDucReportCreator::createAnalyzerBox()
	{
 		QHBoxLayout* w_rowLayout = new QHBoxLayout;

    // list initializer ctor with default buttons name 
		m_buttonTitle = new QStringList({ /*"Exit",*/"Process","Load","Save","Open" });
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
	QGroupBox* BoDucReportCreator::createCreatorBox()
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

		// now set our box 
		m_creatorBox->setLayout(w_buttonsTop);

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
		m_clearButton = m_dialogButtonBox->addButton(QDialogButtonBox::Cancel);
	}
	void BoDucReportCreator::createLayout()
	{
		// display box (command display in the right panel)
		setTableWidgetBox();

		// display the progress horizontally
		setHProgressBar();

		// text editor (console)
		m_console = new QTextEdit;
		m_console->setOverwriteMode(false);
		m_console->setCurrentFont( QFont("Helvetica")); // debugging purpose
		m_console->setFontPointSize(10); // font size when writing to console 
		m_console->setText("Please select file by opening and then load");

		// main dialog
		QVBoxLayout* w_mainLayout = new QVBoxLayout;
		//w_mainLayout->addLayout(w_buttonsTop);
		w_mainLayout->addWidget(createAnalyzerBox());
		w_mainLayout->addWidget(createCreatorBox());
		w_mainLayout->addWidget(m_uniteBox);
		w_mainLayout->addWidget(m_cmdBox);
		w_mainLayout->addWidget(m_console); // text editor 
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
		// create a layout for progress bar in a vertical layout
		QHBoxLayout* w_hProgressBar = new QHBoxLayout;

		addProgressBar(w_hProgressBar, m_listUniteAvailable.at(0).toStdString().c_str());
		addProgressBar(w_hProgressBar, m_listUniteAvailable.at(1).toStdString().c_str());
		addProgressBar(w_hProgressBar, m_listUniteAvailable.at(2).toStdString().c_str());
		addProgressBar(w_hProgressBar, m_listUniteAvailable.at(3).toStdString().c_str());
		addProgressBar(w_hProgressBar, m_listUniteAvailable.at(4).toStdString().c_str());
		m_uniteBox->setLayout(w_hProgressBar);
	}
	void BoDucReportCreator::setUniteBox()
	{
		// create a layout for progress bar in a vertical layout
		QVBoxLayout* w_vProgressBar = new QVBoxLayout;

		addProgressBar(w_vProgressBar, std::string("Unite 1"));
		addProgressBar(w_vProgressBar, std::string("Unite 2"));
		addProgressBar(w_vProgressBar, std::string("Unite 3"));
		addProgressBar(w_vProgressBar, std::string("Unite 4"));
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
		m_console->append("Parsing Files of command");

		m_bdApp.process();

		m_console->append("Finished processed the file");

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
			m_console->append("");
			m_console->append("Starting to load files");

			// loop on each files in the list (fill vector map for processing multiple files in one step)
			m_bdApp.readFiles(w_listFilesName, std::string("Signature"));

			m_console->append("Finished loading files");
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
			m_console->append(QString(""));
			m_console->append(QString("There is ") + QString(std::to_string(w_nbCmdCheck).c_str()) + QString(" command to process"));
			m_console->append(QString("Finished to load data"));
			m_console->append(QString("Ready to proceed to parse command, then press process button"));
		}

		m_console->append("Ready to  process these files");

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

		m_console->append("You have selected the following file(s): ");
		for (const QString& w_fileName : m_filesName)
		{
			m_console->append(w_fileName);
		}

		// activate button, can now start parsing the .csv file
		if (!m_analyzeLoadButton->isEnabled())
		{
			m_analyzeLoadButton->setEnabled(true);
		}

		m_openButton->setEnabled(false);
		m_console->append(QString("Click load button to read file ..."));
	}
	void BoDucReportCreator::createConnections()
	{
		QObject::connect( m_closeButton,     SIGNAL(clicked()),                      this, SLOT(close()));
		QObject::connect( m_clearButton,     SIGNAL(clicked()),                      this, SLOT(cancel()));
		QObject::connect( m_loadButton,      SIGNAL(clicked()),                      this, SLOT(open()));
		QObject::connect( m_bonCreateReport, SIGNAL(clicked()),                      this, SLOT(createBonReport()));
		QObject::connect( m_listUnite,       SIGNAL(activated(int)),                 this, SLOT(currentUniteON()));
		QObject::connect( m_saveSelectBtn,   SIGNAL(clicked()),                      this, SLOT(savetest()));
		QObject::connect( m_tblWidget,       SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(testItemClick(QTableWidgetItem*)));
		// Analyze buttons
		QObject::connect(m_saveButton,        SIGNAL(clicked()), this, SLOT(saveCmdToFile()));   //save file name
		QObject::connect(m_procButton,        SIGNAL(clicked()), this, SLOT(parseCmd()));        // do some processing
		QObject::connect(m_analyzeLoadButton, SIGNAL(clicked()), this, SLOT(loadCmdFromFile())); // set file name to be read
		QObject::connect(m_openButton,        SIGNAL(clicked()), this, SLOT(OpenCsvFile()));     //
	}
	void BoDucReportCreator::createDataReport()
	{
		m_console->append("\n");
		m_console->append("We are creating the report for this command");

		// creating the report for data processing
		std::vector<bdAPI::BoDucFields> w_data2Report = m_bdApp.getReportData();

		// sanity check
		if (w_data2Report.empty())
		{
			//std::cout << "We have a problem to create the report\n";
			QColor w_fontClor = m_console->textColor(); // shall display such a message 
			m_console->append("We have a problem to create the report"); // to be completed
		}

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
					m_console->append("Folder ReportFiles created sucessfully");
					//std::cout << "Folder ReportFiles created sucessfully\n";
				}
			}
			else // set to the report folder
			{
				w_reportFolder.cd(QString("ReportFiles"));
			}
		}
		m_console->append("\n");
		m_console->append("Report directory is:" + w_reportFolder.absolutePath());
		m_console->append("\n");

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

	void BoDucReportCreator::convertPdf2Txt()
	{

		// convert to txt file
		const QString pythonScript = R"(F:\EllignoContract\BoDuc\pdfminerTxt\pdfminer-20140328\build/scripts-2.7\pdf2txt.py)";
		const QString w_pdfilesPath = R"(F:\EllignoContract\BoDuc\QtTestGui\BoDucReportCreator\BoDucReportCreator\Data)"; // pdf files folder

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
} // End of namespace
