#pragma once

// Qt includes
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QMultiMap>
#include "ui_BoDucReportCreator.h"
// App. includes
#include "BdAPI/BoDucFields.h"
#include "BdAPI/BoDucApp.h"

// forward declaration
class QWidget;
class QGroupBox;
class QComboBox;
class QTextEdit;
class QDateEdit;
class QCheckBox;
class QBoxLayout;
class QVBoxLayout;
class QPushButton;
class QProgressBar;
class QTableWidget;
class QDialogButtonBox; // not used for now
class QTableWidgetItem;

#define nbUnit 10 // no don't do that!!!

namespace bdApp 
{
	class BoDucReportCreator : public QWidget
	{
		Q_OBJECT

		// set the capacity load according to time period of the year
		enum class eCapacityMode
		{
			normal=0, // default mode (most of the year)
			degel=1   // usually spring time
		};
		enum class eDisplayMode
		{
			all=0, // display all cmd from file
			date=1, // display cmd from user selection date
			dateRange=2 // display cmd form a date range 
		};
	public:
		BoDucReportCreator( QWidget *parent = Q_NULLPTR);
		BoDucReportCreator( const BoDucReportCreator& aOther)=delete;
		BoDucReportCreator& operator= ( const BoDucReportCreator& aOther) = delete;

		public slots:
		//void highlightChecked(QListWidgetItem* item);
		void parseCmd();
		void saveCmdToFile();
		void loadCmdFromFile();
		void OpenCsvFile();
		void savetest(); // do what??
		void open();     // open command file for reading
		void setMode();
		void showCmd();
		void allDateChecked();
		void clearDispalyedCmd();

	 //	void selectedItem();
	//	void clickedItem();
		void currentUniteON();
		void createBonReport();
		void testItemClick(QTableWidgetItem* aItem);
	private:
		// struct to hold data (bon livraison report )
		using tplbonlivraison = std::tuple<QString/*address*/, QString/*product*/, double/*qty*/, short/*silo*/>;
		using tplunitAndLoad = std::tuple<unsigned/*unit no*/, double/*normal load*/, double/*degel load*/>;
		using mapunitfileds = std::map<unsigned/*unit no*/, std::vector<bdAPI::BoDucFields>/*unit data*/>;

	//	QListWidget* m_listWidget;
		QTableWidget* m_tblWidget;
		QStringList m_TableHeader;
		QComboBox* m_listUnite;
		QDialogButtonBox* m_dialogButtonBox;
		QGroupBox* m_cmdBox;
		QGroupBox* m_uniteBox;
		QPushButton* m_closeButton;
		QPushButton* m_clearButton;
		QPushButton* m_showButton;
		QDateEdit* m_dateSelect;
		QDateEdit* m_dateMinSelected;
		QDateEdit* m_dateMaxSelected;
		QCheckBox* m_date2Check;
		QCheckBox* m_rngDate2Check;
		QCheckBox* m_allDateCheck;

		// Bon de livraison box
		QPushButton* m_loadButton;
		QPushButton* m_saveSelectBtn;
		QPushButton* m_bonCreateReport;
	  // progress bar 
		static const int m_nbUnit=10;
		QProgressBar* m_progressBar[m_nbUnit];
		QVBoxLayout* m_columnLayout[5];
		QStringList* m_buttonTitle;
		QString m_currentUnite;
		QList<QVector<QVariant>> m_read4test;
		std::vector<bdAPI::BoDucFields> m_vectorOfCmd;      // all cmd from file
		std::vector<bdAPI::BoDucFields> m_vecOfCmd2Display; // selected cmd from user 

		// some helper functions
		void createTableWidget(); // deprecated
		void fillTableWidget2Display();
		void createOtherWidgets();
		void createLayout();
		void setUniteBox();
		QGroupBox* createAnalyzerBox();
		QGroupBox* createCreatorBox();

		// progress bar handler (move semantic)
		void addProgressBar(QBoxLayout* w_vProgressBar, const std::string& aUniteNb);
		void setHProgressBar();
		QBoxLayout* setBottomButtons();
		void setupViews();
		void saveDataIntoTable();
		void setTableWidgetBox();
		void updateProgress();
		void initProgressBar();
		// deprecated, removed in the next refactoring 
		void createDataReport();
    // ...
		void createConnections();
   
//		QPushButton* m_saveSelectBtn;
		QPushButton* m_analyzeLoadButton;
		QPushButton* m_procButton;
		QPushButton* m_openButton;
		QPushButton* m_saveButton;     // when user push this button, report is saved

		QPushButton* m_exit;
		QTextEdit* m_console;
		// ...
		int m_pbarmin = 0;
		int m_pbarmax = 0;
		int m_numButtons = 4;
		int m_numPbar = 10;
		int m_spacinVal = 15; 

		int m_currowNoSelected;
		QGroupBox* m_analyzerBox;
		QGroupBox* m_creatorBox;
		QString m_currUnityON;
		QString m_fileName;        // name of the csv file 
		QStringList m_filesName;   // list of all csv files name
		QString m_bonLivraisonFile;
		unsigned short m_fileLoaded;
		eDisplayMode m_displaySelect;

		// vector of all command fields that will be used 
		QVector<QVariant> m_vecVariant;
		tplbonlivraison m_bdBonFields; // testing 
		
		// key is the unit no and all values for report creation
		QMultiMap<QString, tplbonlivraison> m_unitBonLivraisonData;

		// list of unit that are available
		QStringList m_listUniteAvailable = { QString("Unit 30"), QString("Unit 33"), 
			QString("Unit 103"), QString("Unit 110"), QString("Unit 111"),
			QString("Unit 112"), QString("Unit 114"), QString("Unit 115"),
			QString("Unit 116"), QString("Unit 117") };
	
		// hold a vector of cmd for each unit (create bon livraison)
		mapunitfileds m_cmdUnitSaved;
		int m_unitIndex=0;
	//	std::tuple<unsigned/*unit no*/, double/*normal load*/, double/*degel load*/> m_unitAndCapacityLoad;

		std::list<tplunitAndLoad> m_listLoadValuesPerUnit;
		void unitLoadConfig();
		void setUnitCapacityLoad();
		QPushButton* m_cptySelectBtn;
		QPushButton * m_cancelButton;
	private:
		Ui::BoDucReportCreatorClass ui;
		bdAPI::BoDucApp m_bdApp;
		void convertPdf2Txt();
		eCapacityMode m_capacityLoad = eCapacityMode::normal;
		void remDuplicateAndSort( std::vector<bdAPI::BoDucFields>& aVecTotrim);

		QBoxLayout* createHBoxLayoutRow1();
	};
} // End of namespace
