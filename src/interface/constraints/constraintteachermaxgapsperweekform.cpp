/***************************************************************************
                          constraintteachermaxgapsperweekform.cpp  -  description
                             -------------------
    begin                : July 15 , 2007
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

#include "textmessages.h"

#include "constraintteachermaxgapsperweekform.h"
#include "addconstraintteachermaxgapsperweekform.h"
#include "modifyconstraintteachermaxgapsperweekform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintTeacherMaxGapsPerWeekForm::ConstraintTeacherMaxGapsPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);

	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(constraintChanged(int)));
	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(removeConstraintPushButton, SIGNAL(clicked()), this, SLOT(removeConstraint()));
	connect(modifyConstraintPushButton, SIGNAL(clicked()), this, SLOT(modifyConstraint()));
	connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
	connect(constraintsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyConstraint()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	teachersComboBox->addItem("");
	for(int i=0; i<TContext::get()->instance.teachersList.size(); i++){
		Teacher* tch=TContext::get()->instance.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}

	this->filterChanged();
}

ConstraintTeacherMaxGapsPerWeekForm::~ConstraintTeacherMaxGapsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintTeacherMaxGapsPerWeekForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
		ConstraintTeacherMaxGapsPerWeek* ctna=(ConstraintTeacherMaxGapsPerWeek*) ctr;
		return ctna->teacherName==teachersComboBox->currentText() || teachersComboBox->currentText()=="";
	}
	else
		return false;
}

void ConstraintTeacherMaxGapsPerWeekForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<TContext::get()->instance.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=TContext::get()->instance.timeConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListWidget->addItem(ctr->getDescription());
		}
	}

	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		constraintChanged(-1);
}

void ConstraintTeacherMaxGapsPerWeekForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=nullptr);
	currentConstraintTextEdit->setPlainText(ctr->getDetailedDescription());
}

void ConstraintTeacherMaxGapsPerWeekForm::addConstraint()
{
	AddConstraintTeacherMaxGapsPerWeekForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintTeacherMaxGapsPerWeekForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("m-FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeacherMaxGapsPerWeekForm form(this, (ConstraintTeacherMaxGapsPerWeek*)ctr);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintTeacherMaxGapsPerWeekForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("m-FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription();
	
	QListWidgetItem* item;

	switch( MessagesManager::confirmation( this, tr("m-FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		TContext::get()->instance.removeTimeConstraint(ctr);
		
		visibleConstraintsList.removeAt(i);
		constraintsListWidget->setCurrentRow(-1);
		item=constraintsListWidget->takeItem(i);
		delete item;
		
		break;
	case 1: // The user clicked the Cancel button or pressed Escape
		break;
	}
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}
