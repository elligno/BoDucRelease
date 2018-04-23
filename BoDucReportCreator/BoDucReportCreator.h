#pragma once

// Qt includes
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QMultiMap>
#include "ui_BoDucReportCreator.h"
// App. include
#include "BdAPI/BoDucApp.h"

// forward declaration
class QWidget;
class QGroupBox;
class QComboBox;
class QTextEdit;
class QBoxLayout;
class QVBoxLayout;
class QPushButton;
class QProgressBar;
class QTableWidget;
class QDialogButtonBox; // not used for now
class QTableWidgetItem;

namespace bdApp 
{
	class BoDucReportCreator : public QWidget
	{
		Q_OBJECT

	public:
		BoDucReportCreator(QWidget *parent = Q_NULLPTR);

		public slots:
		//void highlightChecked(QListWidgetItem* item);
		void parseCmd();
		void saveCmdToFile();
		void loadCmdFromFile();
		void OpenCsvFile();
		void savetest(); // do what??
		void open();     // open command file for reading
	 //	void selectedItem();
	//	void clickedItem();
		void currentUniteON();
		void createBonReport();
		void testItemClick(QTableWidgetItem* aItem);
	private:
		// struct to hold data (bon livraison report )
		using tplbonlivraison = std::tuple<QString, QString, double, short>;

	//	QListWidget* m_listWidget;
		QTableWidget* m_tblWidget;
		QStringList m_TableHeader;
		QComboBox* m_listUnite;
		QDialogButtonBox* m_dialogButtonBox;
		QGroupBox* m_cmdBox;
		QGroupBox* m_uniteBox;
		QPushButton* m_closeButton;
		QPushButton* m_clearButton;

		// Bon de livraison box
		QPushButton* m_loadButton;
		QPushButton* m_saveSelectBtn;
		QPushButton* m_bonCreateReport;

	  // progress bar 
		QProgressBar* m_progressBar[5];
		QVBoxLayout* m_columnLayout[5];
		QStringList* m_buttonTitle;
		QString m_currentUnite;
		QList<QVector<QVariant>> m_read4test;

		// some helper functions
		void createTableWidget();
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
		int m_numPbar = 5;
		int m_spacinVal = 15; 

		int m_currowNoSelected;
		QGroupBox* m_analyzerBox;
		QGroupBox* m_creatorBox;
		QString m_currUnityON;
		QString m_fileName;        // name of the csv file 
		QStringList m_filesName;   // list of all csv files name
		QString m_bonLivraisonFile;
		unsigned short m_fileLoaded;

		// vector of all command fields that will be used 
		QVector<QVariant> m_vecVariant;
		tplbonlivraison m_bdBonFields; // testing 
		
		// key is the unit no and all values for report creation
		QMultiMap<QString, tplbonlivraison> m_unitBonLivraisonData;
		QStringList m_listUniteAvailable = { QString("Unit 110"), QString("Unit 111"),
			QString("Unit 112"), QString("Unit 115"), QString("Unit 117") };
	private:
		Ui::BoDucReportCreatorClass ui;
		bdAPI::BoDucApp m_bdApp;
		void convertPdf2Txt();
	};
} // End of namespace
