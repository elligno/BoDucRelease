#pragma once

// Qt includes
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QDir>
// App. includes
#include "ui_BoDucReportCreator.h"
#include "BdAPI/BoDucFields.h"
#include "BdAPI/BoDucBonLivraison.h"

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
//class QDialogButtonBox;  not used for now
class QTableWidgetItem;

class AnalyzerBoxWidget;

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

	public:
		BoDucReportCreator( QWidget *parent = Q_NULLPTR);
		BoDucReportCreator( const BoDucReportCreator& aOther)=delete;
    BoDucReportCreator& operator= (const BoDucReportCreator& aOther) = delete;
    ~BoDucReportCreator();

  signals:
    // notify the unit progress bar that we have updated unit load 
    void updateUnitProressBar( const QString&, double);

  public slots:
    void updateFileDataStore();  // update command in memory (vector)
    void saveUserSelection( QTableWidgetItem* aItem);  // connect to button "save selection"
		void setCapacityMode( /*eCapacityMode aCptyMode*/);
		void showCmd();
		void clearDispalyedCmd();
		void currentUniteON();
		void createBonLivraison();
		void insertHour(int rowNo, int columnNo); // ???

	private:
    // Design Note
    // all these tuple type should be wrapped in a class
		// struct to hold data (bon livraison report )
    // it certainly need a serious clean-up, hardly understaneable!! 
		using tplbonlivraison = std::tuple<QString/*address*/, QString/*product*/, double/*qty*/, short/*silo*/>; // deprecated
//		using boducBon2Write  = std::tuple<unsigned /*unit*/, QString/*hour*/, QString/*address*/, QString/*product*/, double/*qty*/, short/*silo*/>;
//		using tplunitAndLoad  = std::tuple<unsigned/*unit no*/, double/*normal load*/, double/*degel load*/>;
		using mapunitfileds   = std::map<unsigned/*unit no*/, std::vector<bdAPI::BoDucFields>/*unit data*/>; // deprecated
    using mapunitbon      = std::map<unsigned/*unit no*/, bdAPI::BoDucBonLivraison/*unit data*/>;
//		using mapunitfields   = std::map<unsigned/*unit no*/, std::vector<std::pair<QString,bdAPI::BoDucFields>>/*unit data*/>;
	//	using mapofunitbon    = std::map<unsigned/*unit no*/, boducBon2Write>;

	//	QListWidget* m_listWidget;
		QTableWidget* m_tblWidget;
		QStringList m_TableHeader;
		QComboBox* m_listUnite;
		QGroupBox* m_uniteBox;
		QPushButton* m_clearButton;
		QPushButton* m_showButton;
		QDateEdit* m_dateMinSelected;
		QDateEdit* m_dateMaxSelected;

		// Bon de livraison box
//		QPushButton* m_saveSelectBtn;
//		QPushButton* m_bonCreateReport;
	  
    // progress bar 
		static constexpr auto m_nbUnit = 10; 
		QString m_currentUnite;
		QList<QVector<QVariant>> m_read4test;
	
    std::vector<bdAPI::BoDucFields> m_vectorOfCmd;      // all cmd from file
		std::vector<bdAPI::BoDucFields> m_vecOfCmd2Display; // selected cmd from user 

		// some helper functions
		//void createTableWidget();  deprecated
		void fillTableWidget2Display();
		void createLayout();
		
    //void setUniteBox();
		QGroupBox* createBonLivraisonBox();
		QGroupBox* createProgressBarBox();
    QGroupBox* setTableWidgetBox();   
    QBoxLayout* setBottomButtons();
		void setupViews();

		bool updateUnitProgress( const float aVal2Update);

    // deprecated
		void createConnections();

    // ...
		const int m_pbarmin = 0;
		const int m_pbarmax = 0;
		const int m_numButtons = 4;
		const int m_numPbar = 10;
		int m_spacinVal = 15; 

		int m_currowNoSelected;
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
		mapunitfileds m_cmdUnitSaved; // deprecated
    mapunitbon m_unitBonLivraison;
	//	int m_unitIndex=0;
	//	std::tuple<unsigned/*unit no*/, double/*normal load*/, double/*degel load*/> m_unitAndCapacityLoad;

//		std::list<tplunitAndLoad> m_listLoadValuesPerUnit;
	//	void unitLoadConfig();
		void setUnitCapacityLoad();
		//QPushButton* m_cptySelectBtn;
	
private:
		Ui::BoDucReportCreatorClass ui;
		eCapacityMode m_capacityLoad = eCapacityMode::normal;
    AnalyzerBoxWidget* m_analyzerBox; // group box that contains widget to load and parse command
    QString m_defaultDir = QDir::currentPath();
    // location of the file database
    QDir m_reportFolder;
    void setReportFolder();
    //void setQProcessEnv( /*const QString& aPythonPath, const QString& aPdfPath*/);
    void initMapUserSelection(); // deprecated
    void createMapBonLivraison();
	};
} // End of namespace
