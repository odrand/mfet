/***************************************************************************
                          modifyconstraintstudentssetmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "modifyconstraintstudentssetmaxhoursdailyform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsSetMaxHoursDailyForm::ModifyConstraintStudentsSetMaxHoursDailyForm(QWidget* parent, ConstraintStudentsSetMaxHoursDaily* ctr): GenericConstraintForm(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	this->_ctr=ctr;

	weightLineEdit->setText(utils::strings::number(ctr->weightPercentage));

    InterfaceUtils::setConstraintGroupToRadioButtons(ctr->constraintGroup(), essentialRadioButton, importantRadioButton, desirableRadioButton);
	
	updateStudentsComboBox(parent);

	maxHoursSpinBox->setMinimum(1);
	maxHoursSpinBox->setMaximum(TContext::get()->instance.nHoursPerDay);
	maxHoursSpinBox->setValue(ctr->maxHoursDaily);

	constraintChanged();
}

ModifyConstraintStudentsSetMaxHoursDailyForm::~ModifyConstraintStudentsSetMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMaxHoursDailyForm::updateStudentsComboBox(QWidget* parent){
	studentsComboBox->clear();
	int i=0, j=-1;
	for(int m=0; m<TContext::get()->instance.yearsList.size(); m++){
		StudentsYear* sty=TContext::get()->instance.yearsList[m];
		studentsComboBox->addItem(sty->name);
		if(sty->name==this->_ctr->students)
			j=i;
		i++;
		for(int n=0; n<sty->groupsList.size(); n++){
			StudentsGroup* stg=sty->groupsList[n];
			studentsComboBox->addItem(stg->name);
			if(stg->name==this->_ctr->students)
				j=i;
			i++;
			if(defs::SHOW_SUBGROUPS_IN_COMBO_BOXES) for(int p=0; p<stg->subgroupsList.size(); p++){
				StudentsSubgroup* sts=stg->subgroupsList[p];
				studentsComboBox->addItem(sts->name);
				if(sts->name==this->_ctr->students)
					j=i;
				i++;
			}
		}
	}
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->students);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);

	constraintChanged();
}

void ModifyConstraintStudentsSetMaxHoursDailyForm::constraintChanged()
{
}

void ModifyConstraintStudentsSetMaxHoursDailyForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->students);
		return;
	}

	double weight;
	QString tmp=weightLineEdit->text();
	utils::strings::weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("m-FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=TContext::get()->instance.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("m-FET information"),
			tr("Invalid students set"));
		return;
	}


    Enums::ConstraintGroup group(InterfaceUtils::getConstraintGroupFromRadioButtons(essentialRadioButton, importantRadioButton, desirableRadioButton));
    this->_ctr->setConstraintGroup(group);

	this->_ctr->weightPercentage=weight;
	this->_ctr->students=students_name;
	this->_ctr->maxHoursDaily=maxHoursSpinBox->value();

	TContext::get()->instance.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&TContext::get()->instance);
	
	this->close();
}

void ModifyConstraintStudentsSetMaxHoursDailyForm::cancel()
{
	this->close();
}
