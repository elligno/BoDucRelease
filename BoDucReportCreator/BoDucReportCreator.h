#pragma once

#include <QtWidgets/QWidget>
#include "ui_BoDucReportCreator.h"

class BoDucReportCreator : public QWidget
{
	Q_OBJECT

public:
	BoDucReportCreator(QWidget *parent = Q_NULLPTR);

private:
	Ui::BoDucReportCreatorClass ui;
};
