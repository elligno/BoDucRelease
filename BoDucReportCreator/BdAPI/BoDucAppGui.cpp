// C++ includes
#include <fstream>
#include <iostream>
// Qt includes
#include <QDirIterator>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QTextEdit>
// GUI include
#include "BoDucFields.h"
#include "BoDucAppGui.h"

//TODO: this file need a serious clean-up (remove unused code and comment)
BoDucAppGui::BoDucAppGui( QWidget *parent)
: QWidget(parent),
  m_open(Q_NULLPTR),
  m_exit(Q_NULLPTR),
  m_process(Q_NULLPTR),
  m_load(Q_NULLPTR),
  m_save(Q_NULLPTR),
  m_layout(Q_NULLPTR),
  m_buttonTitle(Q_NULLPTR),
  m_spacinVal(15), // default value 
  m_numButtons(5)  // ... 
//   m_processButton(Q_NULLPTR),
// 	m_loadButton(Q_NULLPTR),
// 	m_saveButton(Q_NULLPTR),
// 	m_cmdFileName(""),
// 	m_appDir(Q_NULLPTR)
{
	ui.setupUi(this); // don't know what for

	// Widget title
	setWindowTitle( QString("BoDuc Command Parser"));
	setGeometry(0, 0, 600, 400);
	createGUI();
	connectButtons();
	Q_ASSERT(m_layout != Q_NULLPTR);
	setLayout(m_layout);

	// create file as default when starting the application
	QDateTime w_currDateTime = QDateTime::currentDateTime();
	QDate w_currentDate = w_currDateTime.date();
	QString w_todayDate = w_currentDate.toString("ddMyy");  //use the following format

	// build file name with date, a new file name is use every day
	// during the day user will be use the same file as he process
	m_bonLivraisonFile = QString("BonLivraison_");
//	m_bonLivraisonFile += w_todayDate;  add date at the end of the name
	m_bonLivraisonFile += ".txt";
}

// this is the start method of our test environment
void BoDucAppGui::parseCmd()
{
	m_console->append("Parsing Files");

	m_bdApp.process();
	
	m_console->append("Finished processed the file");

	// if process is done successfully, might as well save it to file
	if( !m_save->isEnabled())
	{
		m_save->setEnabled(true);
	}
	// make it ready for next command file
	if( !m_load->isEnabled()) // not sure about this one
	{
		m_load ->setEnabled(false);
	}
	m_process->setEnabled(false);
}

// this is 
void BoDucAppGui::saveCmdToFile()
{
	// creating the report with default file name (set when app is started)
	createDataReport();
	if (!m_open->isEnabled())
	{
		m_open->setEnabled(true);
	}
	m_save->setEnabled(false);
}

void BoDucAppGui::loadCmdFromFile()
{
	// open the file to read
// 	if( m_fileName.isNull() || m_fileName.isEmpty())
// 	{
// 		qWarning("File was not selected");
// 		QMessageBox w_loadMsg;
// 	}

// 	QFileInfo w_fileInfo(m_fileName);
// 	QString w_fileFolder = w_fileInfo.path();
// 	QString w_fileExt = w_fileInfo.suffix();
// 
// 	QFile w_file(m_fileName);
// 	if( !w_file.open( QIODevice::ReadOnly | QIODevice::Text))
// 	{
// 		QMessageBox::information(this, tr("Unable to open file"),
// 			w_file.errorString());
// 
// 		return;
// 	}

	// in comment now, not sure how i am going to integrate that
// 	QTextStream in(&w_file);
// 	while (!in.atEnd()) {
// 		QString line = in.readLine();
// 		processLine(line.toStdString()); must be something like that!! need to think about it
// 	}
	
	if( m_filesName.size() > 1) // user selected more than one files
	{
		std::list<std::string> w_listFilesName;
		QStringList::const_iterator w_begList = m_filesName.constBegin();
		QStringList::const_iterator w_endList = m_filesName.constEnd();
		while (w_begList!=w_endList)
		{
			std::cout << "Loading the following files: " << w_begList->toStdString() << std::endl;
			w_listFilesName.push_back(w_begList->toStdString()); 
			++w_begList;
		}

// 		QStringListIterator w_filesNameIterator(m_filesName);
// 		while( w_filesNameIterator.hasNext())
// 		{
// 			std::cout << "Processing the following files: " << w_filesNameIterator.next().toLocal8Bit().constData() << std::endl;
// 			w_listFilesName.push_back( w_filesNameIterator.next().toLocal8Bit().constData()); // implicit conversion from const char* to std::string
// 		}
		m_console->append("");
		m_console->append("Starting to load files");

		// loop on each files in the list (fill vector map for processing multiple files in one step)
		m_bdApp.readFiles( w_listFilesName, std::string("Signature"));
		
		m_console->append("Finished loading files");

		// writing to console (i need a loop to process this) put it in comment for now, need to check later.
	//	m_console->append( QString(""));
	//	m_console->append( QString("There is ") + QString(std::to_string(w_nbCmdCheck).c_str()) + QString(" command to process"));
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
		m_console->append( QString(""));
		m_console->append( QString("There is ") + QString(std::to_string(w_nbCmdCheck).c_str()) + QString(" command to process"));
		m_console->append( QString("Finished to load data"));
		m_console->append( QString("Ready to proceed to parse command, then press process button"));
	}

	m_console->append("Ready to  process these files");

	if( !m_process->isEnabled())
	{
		m_process->setEnabled(true);
	}
	if (m_load->isEnabled())
	{
		m_load->setEnabled(false);
	}
}

// called when "open" button clicked
// by default all files for testing are located in a folder named "DataToTest"
void BoDucAppGui::OpenCsvFile()
{
	// create a file dialog 
	//m_openFile = new QFileDialog;

	// setting default directory to start with (think it correspond to QtTesting folder)
	// i need to check, which is the root directory of the VS15 project
	QString w_defaultDir = QDir::currentPath();

	std::cout << "Current of the opening file is : " << w_defaultDir.toStdString() << std::endl;

	// before we go 
	// set default dir Not sure about this ?? really need that?
	//m_openFile->setDirectory(w_defaultDir);

	// Actually what we want is to check if a directory exist, especially data folder 
// 	QDir w_tstDir(w_defaultDir);
// 	if( !w_tstDir.exists(QString("DataToTest")))
// 	{
// 		qWarning("Could not open file for reading\n");
// 	}

	// original version
	// now opening file to be processed (name of the file returned)
// 	m_fileName = QFileDialog::getOpenFileName( this, tr("Open File"),
// 		w_defaultDir,
// 		tr("Text (*.txt *.csv)"));

// 	m_console->append("You have selected the following file: ");
// 	m_console->append(m_fileName);

	// multiple file selection support
	// now opening file to be processed (name of the file returned)
	m_filesName = QFileDialog::getOpenFileNames(this, tr("Open File"),
		w_defaultDir,
		tr("Text (*.txt *.csv)"));

	// number of files selected by user
	m_fileLoaded = m_filesName.size();
	m_bdApp.setNbSelectedFiles(m_fileLoaded);

	if( m_filesName.size()==1)
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
	if( !m_load->isEnabled())
	{
		m_load->setEnabled(true);
	}

	m_open->setEnabled(false);
	m_console->append( QString("Click load button to read file ..."));
}

void BoDucAppGui::createGUI()
{
	// configure text editor
	m_console = new QTextEdit;
	m_console->setOverwriteMode(false);
	m_console->setText("Please select file by opening and then load");

	// configuring buttons 
	m_layout = new QGridLayout;
	m_rowLayout = new QHBoxLayout;

	// Set all buttons with initial state to false except Exit button
	createHBoxButtons();

	// how it is layout? if there is no other widget 
	// the text editor take the whole space
	// as first test we set a row with 4 element 
	m_layout->addLayout(m_rowLayout, 0, 0); // first row
	m_layout->addWidget(m_console, 1, 0); //second row
}

void BoDucAppGui::createHBoxButtons()
{
	//
	// creating some vertical layout that will be part of the row

	// list initializer ctor with default buttons name 
	m_buttonTitle = new QStringList({ "Exit","Process","Load","Save","Open" });
	QStringList::const_iterator w_butName = m_buttonTitle->cbegin();

	for( int i = 0; i < m_numButtons; ++i)
	{
		m_columnLayout[i] = new QVBoxLayout;
	}

	// first element of the row
	m_exit = new QPushButton(tr(w_butName->toStdString().c_str()));
	if (!m_exit->isEnabled())
	{
		m_exit->setEnabled(true);
	}
	m_columnLayout[0]->addWidget(m_exit);
	m_rowLayout->addLayout(m_columnLayout[0], 10/*stretch*/);
	m_columnLayout[0]->addSpacing(m_spacinVal);
	++w_butName;

	// second element of the row
	m_process = new QPushButton(tr(w_butName->toStdString().c_str()));
	if (m_process->isEnabled())
	{
		m_process->setEnabled(false);
	}
	m_columnLayout[1]->addWidget(m_process);
	m_rowLayout->addLayout(m_columnLayout[1], 10/*stretch*/);
	m_columnLayout[1]->addSpacing(m_spacinVal);
	++w_butName;

	// third element of the row
	m_load = new QPushButton(tr(w_butName->toStdString().c_str()));
	if (m_load->isEnabled())
	{
		m_load->setEnabled(false);
	}
	m_columnLayout[2]->addWidget(m_load);
	m_rowLayout->addLayout(m_columnLayout[2], 10/*stretch*/);
	m_columnLayout[2]->addSpacing(m_spacinVal);
	++w_butName;

	// third element of the row
	m_save = new QPushButton(tr(w_butName->toStdString().c_str()));
	if (m_save->isEnabled())
	{
		m_save->setEnabled(false);
	}
	m_columnLayout[3]->addWidget(m_save);
	m_rowLayout->addLayout(m_columnLayout[3], 10/*stretch*/);
	m_columnLayout[3]->addSpacing(m_spacinVal);
	++w_butName;

	// Open file 
	m_open = new QPushButton(tr(w_butName->toStdString().c_str()));
	if (!m_open->isEnabled())
	{
		m_open->setEnabled(true);
	}
	m_columnLayout[4]->addWidget(m_open);
	m_rowLayout->addLayout(m_columnLayout[4], 10/*stretch*/);
	m_columnLayout[4]->addSpacing(m_spacinVal);

}

void BoDucAppGui::createDataReport()
{
	m_console->append("\n");
	m_console->append("We are creating the report for this command");

	// creating the report for data processing
	std::vector<bdGui::BoDucFields> w_data2Report = m_bdApp.getReportData();

	// sanity check
	if( w_data2Report.empty())
	{
		std::cout << "We have a problem to create the report\n";
	}

  //TODO: need to set working folder outside of the createDataReport method
	// and not hard coded (using string literal)
	// C:\JeanWorks\\BoDuc\App\\BoDucApp\BoDucAppGui i think correspond to current
	QDir w_currentDir = QDir::current(); // but we want a folder above
	QString w_dirName = w_currentDir.dirName(); // check
	//QDir w_workingDir(QString(R"(C:\JeanWorks\\BoDuc\App\BoDucApp\ReportFiles)"));
	QDir w_reportFolder;
// 	if( w_currentDir!=w_workingDir)
// 	{
		// we need to move up
		if( w_currentDir.cdUp()) // folder higher of project folder (BoDucApp)
		{
			QString w_currNow = QDir::current().dirName(); // project folder
			QString w_pathNow = w_currentDir.absolutePath(); // BoDucApp
			
			w_reportFolder.setPath(w_pathNow);
			QString w_checkWork = w_reportFolder.dirName();

			// we should be at the BoDucApp level
			// need to check if folder exist, if not then create it 
			if( !w_reportFolder.exists( QString("ReportFiles")))
			{
				if(w_reportFolder.mkdir( QString("ReportFiles")))
				{
					w_reportFolder.cd(QString("ReportFiles"));
					std::cout << "Folder ReportFiles created sucessfully\n";
				}
			}
			else // set to the report folder
			{
				w_reportFolder.cd(QString("ReportFiles"));
			}
		}
//	}

	m_console->append("\n");
	m_console->append("Report directory is:" + w_reportFolder.absolutePath());
	m_console->append("\n");

	QFileInfo w_fileRep(w_reportFolder, m_bonLivraisonFile);
	
	// file doesn't exist
	if( !w_reportFolder.exists(m_bonLivraisonFile)) 
	{
		QString w_filePath = w_fileRep.absoluteFilePath();
		// create a new file with the given name
		QFile w_data( w_fileRep.absoluteFilePath());

		// then create it write report
		if( w_data.open(QFile::WriteOnly | QFile::Text))
		{
			QTextStream out(&w_data);

			// first time we open it, create header
			out << "No Command " << "\t" << "Shipped To " << "\t" << "Deliver Date " << "\t"
				<< "Code" << "\t" << "Product " << "\t" << "Quantity " << "\t" << "Silo" << "\n";

			for( bdGui::BoDucFields& w_bfield : w_data2Report)
			{
				out << w_bfield.m_noCmd.c_str() << "\t" << w_bfield.m_deliverTo.c_str() << "\t" << w_bfield.m_datePromise.c_str() << "\t"
					<< w_bfield.m_prodCode << "\t" << w_bfield.m_produit.c_str() << "\t" << w_bfield.m_qty << "\t" << w_bfield.m_silo.c_str() << "\n";
			}
			if(w_data.isOpen())
			{
				w_data.close(); // make sure we close we may want to re-open
			}
		}
	}
	else // file exist, just append data at the end
	{
		QFile w_data; // set file name of an existing one
		w_data.setFileName(w_fileRep.absoluteFilePath());

		if( w_data.open( QFile::WriteOnly | QFile::Append | QFile::Text))
		{
			QTextStream out(&w_data);
			out << "\n";

			for( bdGui::BoDucFields& w_bfield : w_data2Report)
			{
				out << w_bfield.m_noCmd.c_str() << "\t" << w_bfield.m_deliverTo.c_str() << "\t" << w_bfield.m_datePromise.c_str() << "\t"
					<< w_bfield.m_prodCode << "\t" << w_bfield.m_produit.c_str() << "\t" << w_bfield.m_qty << "\t" << w_bfield.m_silo.c_str() << "\n";
			}
			if( w_data.isOpen())
			{
				w_data.close();
			}
		}
	}
}

void BoDucAppGui::connectButtons()
{
	QObject::connect(m_exit,    SIGNAL(clicked()), this, SLOT(close()));           // close window
	QObject::connect(m_save,    SIGNAL(clicked()), this, SLOT(saveCmdToFile()));   //save file name
	QObject::connect(m_process, SIGNAL(clicked()), this, SLOT(parseCmd()));        // do some processing
	QObject::connect(m_load,    SIGNAL(clicked()), this, SLOT(loadCmdFromFile())); // set file name to be read
	QObject::connect(m_open,    SIGNAL(clicked()), this, SLOT(OpenCsvFile()));     //
}
