#pragma once

// Qt includes
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QDir>
//#include <QMultiMap>
#include "ui_BoDucReportCreator.h"

// App. includes
#include "BdAPI/BoDucFields.h"
//#include "BdAPI/BoDucApp.h"
#include "BdAPI/BoDucParser.h"

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

class AnalyzerBoxWidget;

#define nbUnit 10 // no don't do that!!!

namespace bdApp 
{
	class BoDucReportCreator : public QWidget
	{
		Q_OBJECT

  public:
		// set the capacity load according to time period of the year
		enum class eCapacityMode
		{
			normal=0, // default mode (most of the year)
			degel=1   // usually spring time
		};

    // deprecated (piece of crapt)
// 		enum class eDisplayMode
// 		{
// 			all=0, // display all cmd from file
// 			date=1, // display cmd from user selection date
// 			dateRange=2 // display cmd form a date range 
//		};
	public:
		BoDucReportCreator( QWidget *parent = Q_NULLPTR);
		BoDucReportCreator( const BoDucReportCreator& aOther)=delete;
    BoDucReportCreator& operator= (const BoDucReportCreator& aOther) = delete;
    ~BoDucReportCreator();

  signals:
    // notify the unit progress bar that we have updated unit load 
    void updateUnitProressBar( const QString&, double);

  public slots:
		//void highlightChecked(QListWidgetItem* item);
	//	void loadPdfFiles();
//		void loadCsvFiles();
 //   void settingPath(); 
    void updateFileDataStore();  // update command in memory (vector)

   // void savetest();  do what?? deprecated
		void saveCmdSelection();  // deprecated
    void saveUserSelection();  // connect to button "save selection"
//		void open();      open command file for reading
		void setCapacityMode( /*eCapacityMode aCptyMode*/);
		void showCmd();
		//void allDateChecked();
		void clearDispalyedCmd();

	 //	void selectedItem();
	//	void clickedItem();
		void currentUniteON();
		void createBonLivraison();
		void testItemClick( QTableWidgetItem* aItem); // deprecated
    void userDblClickSelection( QTableWidgetItem* aItem); // tobe removed
		void insertHour(int rowNo, int columnNo); // ???
	private:
    // Design Note
    // all these tuple type should be wrapped in a class
		// struct to hold data (bon livraison report )
    // it certainly need a serious clean-up, hardly understaneable!! 
		using tplbonlivraison = std::tuple<QString/*address*/, QString/*product*/, double/*qty*/, short/*silo*/>;
		using boducBon2Write  = std::tuple<unsigned /*unit*/, QString/*hour*/, QString/*address*/, QString/*product*/, double/*qty*/, short/*silo*/>;
		using tplunitAndLoad  = std::tuple<unsigned/*unit no*/, double/*normal load*/, double/*degel load*/>;
		using mapunitfileds   = std::map<unsigned/*unit no*/, std::vector<bdAPI::BoDucFields>/*unit data*/>;
		using mapunitfields   = std::map<unsigned/*unit no*/, std::vector<std::pair<QString,bdAPI::BoDucFields>>/*unit data*/>;
		using mapofunitbon    = std::map<unsigned/*unit no*/, boducBon2Write>;

	//	QListWidget* m_listWidget;
		QTableWidget* m_tblWidget;
		QStringList m_TableHeader;
		QComboBox* m_listUnite;
		QGroupBox* m_uniteBox;
		QPushButton* m_clearButton;
		QPushButton* m_showButton;
//		QDateEdit* m_dateSelect;
		QDateEdit* m_dateMinSelected;
		QDateEdit* m_dateMaxSelected;
//		QCheckBox* m_date2Check;
		QCheckBox* m_rngDate2Check;
//		QCheckBox* m_allDateCheck;

		// Bon de livraison box
		QPushButton* m_saveSelectBtn;
		QPushButton* m_bonCreateReport;
	  
    // progress bar 
		static constexpr auto m_nbUnit = 10; 
		//QProgressBar* m_progressBar[m_nbUnit];
		//QVBoxLayout* m_columnLayout[5];
//		QStringList* m_buttonTitle;
		QString m_currentUnite;
		QList<QVector<QVariant>> m_read4test;
	
    std::vector<bdAPI::BoDucFields> m_vectorOfCmd;      // all cmd from file
		std::vector<bdAPI::BoDucFields> m_vecOfCmd2Display; // selected cmd from user 

		// some helper functions
		//void createTableWidget();  deprecated
		void fillTableWidget2Display();
		void createLayout();
		
    //void setUniteBox();
		void initMapUserSelection();
		QGroupBox* createAnalyzerBox();
		QGroupBox* createBonLivraisonBox();
		QGroupBox* createProgressBarBox();
    QGroupBox* setTableWidgetBox();

    // progress bar handler (move semantic)
//		void addProgressBar( QBoxLayout* w_vProgressBar, const std::string& aUniteNb);
    
    QBoxLayout* setBottomButtons();
		void setupViews();
//		void saveDataIntoTable();
//		void updateProgress();
		bool updateUnitProgress(const float aVal2Update);
//		void initProgressBar();
		// deprecated, removed in the next refactoring 
//		void createDataReport();

// deprecated
		void createConnections();

//		QPushButton* m_exit;

    // ...
		int m_pbarmin = 0;
		int m_pbarmax = 0;
		int m_numButtons = 4;
		int m_numPbar = 10;
		int m_spacinVal = 15; 

		int m_currowNoSelected;
		//QGroupBox* m_analyzerBox;
		QGroupBox* m_creatorBox;
		QString m_currUnityON;
		QString m_fileName;        // name of the csv file 
		QStringList m_filesName;   // list of all csv/pdf files name
		QString m_bonLivraisonFile; // deprecated
    QFile m_reportFile;
    unsigned short m_fileLoaded;
//		eDisplayMode m_displaySelect;

		// vector of all command fields that will be used 
		QVector<QVariant> m_vecVariant;
		tplbonlivraison m_bdBonFields; // testing 
		
		// key is the unit no and all values for report creation
		QMultiMap<QString, tplbonlivraison> m_unitBonLivraisonData;

		// list of unit that are available list initializer (supported by vs2015 or later)
		//TODO: shall be done in a config file (user can modify without building the app) 
		QStringList m_listUniteAvailable = 
		{ QString("Unit 30"), QString("Unit 33"),
			QString("Unit 103"), QString("Unit 110"), QString("Unit 111"),
			QString("Unit 112"), QString("Unit 114"), QString("Unit 115"),
			QString("Unit 116"), QString("Unit 117") 
		};
    
    /** unit no is the key and value is a pair for normal/degel capacity */
    using unitloadCapact = std::map<int/*unit no*/, std::pair<double/*Normal*/, double/*Degel*/>>;
    
    //  Should be put in file and loaded at app startup 
    //  Convention: (unit, normal, degel)
    unitloadCapact m_unitLoadCapacity{ { 30,{ 17.,14.}}, { 33,{ 31.,25.}},
    { 103,{ 32.,25.}}, { 110,{ 32.,25.}}, { 111,{ 26.,21.}},{ 112, { 38.5,32.}},
    { 114,{ 38.5,31.}},{ 115,{ 32.,25.}}, { 116,{ 14.,11.}},{ 117,{ 38.5,32.}} };

    /** store all progress bar in a map with corresponding unit no*/
    using unitpbar = std::map<int/*unit no*/, QProgressBar*>;
    //unitpbar w_unitPbar;
    unitpbar m_unitPbar;

		// hold a vector of cmd for each unit (create bon livraison)
		mapunitfileds m_cmdUnitSaved;
	//	int m_unitIndex=0;
	//	std::tuple<unsigned/*unit no*/, double/*normal load*/, double/*degel load*/> m_unitAndCapacityLoad;

		std::list<tplunitAndLoad> m_listLoadValuesPerUnit;
	//	void unitLoadConfig();
		void setUnitCapacityLoad();
		//QPushButton* m_cptySelectBtn;
	
private:
		Ui::BoDucReportCreatorClass ui;
		eCapacityMode m_capacityLoad = eCapacityMode::normal;
    AnalyzerBoxWidget* m_analyzerBox; // group box that contains widget to load and parse command
 
    // mapping between a file and number of command 
    //std::map<std::string, size_t> m_nbOfCmdInFile;
    QString m_defaultDir = QDir::currentPath();
    // location of the file database
    QDir m_reportFolder;
    void setReportFolder();
    //void setQProcessEnv( /*const QString& aPythonPath, const QString& aPdfPath*/);
	};
} // End of namespace
