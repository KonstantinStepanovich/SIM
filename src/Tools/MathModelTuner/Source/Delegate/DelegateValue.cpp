/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "Delegate/DelegateValue.hpp"
#include "Data/IValue.hpp"
#include "DataModelAPI.h"

#include <limits>

#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox.h>

#include "String/StringStream.hpp"

class IDelegateWidgetProxy
{
public:
	virtual void read(QWidget * editor, const QModelIndex & index) const = 0;
	virtual void write(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const = 0;
};

class DelegateWidgetProxySpinBox : public IDelegateWidgetProxy
{
public:
	virtual void read(QWidget * editor, const QModelIndex & index) const
	{
		QSpinBox * spinBox = (QSpinBox*)editor;
		int value = index.model()->data(index, Qt::EditRole).toInt();
		spinBox->setValue(value);
	}
	virtual void write(QWidget * editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QSpinBox * spinBox = (QSpinBox*)editor;
		int value = spinBox->value();
		model->setData(index, OSS(value), Qt::EditRole);
	}
	static const DelegateWidgetProxySpinBox & instance()
	{
		static const DelegateWidgetProxySpinBox delegateWidgetProxySpinBox;
		return delegateWidgetProxySpinBox;
	}
};

class DelegateWidgetProxyDoubleSpinBox : public IDelegateWidgetProxy
{
public:
	virtual void read(QWidget * editor, const QModelIndex & index) const
	{
		QDoubleSpinBox * doubleSpinBox = (QDoubleSpinBox*)editor;
		double value = index.model()->data(index, Qt::EditRole).toDouble();
		doubleSpinBox->setValue(value);
	}
	virtual void write(QWidget * editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QDoubleSpinBox * doubleSpinBox = (QDoubleSpinBox*)editor;
		double value = doubleSpinBox->value();
		model->setData(index, OSS(value), Qt::EditRole);
	}
	static const DelegateWidgetProxyDoubleSpinBox & instance()
	{
		static const DelegateWidgetProxyDoubleSpinBox delegateWidgetProxySpinBoxDouble;
		return delegateWidgetProxySpinBoxDouble;
	}
};

class DelegateWidgetProxyComboBox : public IDelegateWidgetProxy
{
public:
	virtual void read(QWidget * editor, const QModelIndex & index) const
	{
		QComboBox * comboBox = (QComboBox*)editor;
		QString value = index.model()->data(index, Qt::EditRole).toString();
		const QList<QVariant> curItemDataList = index.model()->data(index, Qt::UserRole + 1).toList();
		void * data = (void*)curItemDataList.at(3).toLongLong();
		const dmLimitsInt * limitsInt = (dmLimitsInt*)data;
		int itmeIndex = std::find(limitsInt->data.variants.name, limitsInt->data.variants.name + limitsInt->data.variants.size, value) - limitsInt->data.variants.name;
		comboBox->setCurrentIndex(itmeIndex);
	}
	virtual void write(QWidget * editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QComboBox * comboBox = (QComboBox*)editor;
		int itmeIndex = comboBox->currentIndex();
		const QList<QVariant> curItemDataList = index.model()->data(index, Qt::UserRole + 1).toList();
		void * data = (void*)curItemDataList.at(3).toLongLong();
		const dmLimitsInt * limitsInt = (dmLimitsInt*)data;
		model->setData(index, limitsInt->data.variants.name[itmeIndex], Qt::EditRole);
	}
	static const DelegateWidgetProxyComboBox & instance()
	{
		static const DelegateWidgetProxyComboBox delegateWidgetProxyComboBox;
		return delegateWidgetProxyComboBox;
	}
};

class DelegateWidgetProxyCheckBox : public IDelegateWidgetProxy
{
public:
	virtual void read(QWidget * editor, const QModelIndex & index) const
	{
		QCheckBox * checkBox = (QCheckBox*)editor;
		QString value = index.model()->data(index, Qt::EditRole).toString();
		checkBox->setChecked(!value.isEmpty());
	}
	virtual void write(QWidget * editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QCheckBox * checkBox = (QCheckBox*)editor;
		int value = checkBox->isChecked() ? 1 : 0;
		model->setData(index, value != 0 ? "v" : "", Qt::EditRole);
	}
	static const DelegateWidgetProxyCheckBox & instance()
	{
		static const DelegateWidgetProxyCheckBox delegateWidgetProxyCheckBox;
		return delegateWidgetProxyCheckBox;
	}
};

DelegateValue::DelegateValue(QObject *parent)
    : QItemDelegate(parent), delegateWidget_(NULL)
{
}

DelegateValue::~DelegateValue()
{
}

QWidget *DelegateValue::createEditor(	QWidget * parent,
										const QStyleOptionViewItem & option,
										const QModelIndex & index ) const
{
	const QList<QVariant> curItemDataList = index.model()->data(index, Qt::UserRole + 1).toList();
	if(curItemDataList.size() > 2)
	{
		ValueType valueType = (ValueType)curItemDataList.at(2).toUInt();
		void * data = (void*)curItemDataList.at(3).toLongLong();
		if(valueType == VALUE_TYPE_INTEGER)
		{
			const dmLimitsInt * limitsInt = (dmLimitsInt*)data;
			if(limitsInt != NULL)
			{
				switch(limitsInt->type)
				{
					case LIMIT_INT_TYPE_MIN_MAX:
						delegateWidget_ = &DelegateWidgetProxySpinBox::instance();
						if(limitsInt != NULL)
						{
							QSpinBox * spinBox = new QSpinBox(parent);
							spinBox->setMinimum(limitsInt->data.minMax.min);
							spinBox->setMaximum(limitsInt->data.minMax.max);
							spinBox->setSingleStep(1);
							return spinBox;
						}
					case LIMIT_INT_TYPE_VARIANTS:
						delegateWidget_ = &DelegateWidgetProxyComboBox::instance();
						{
							QComboBox * comboBox = new QComboBox(parent);
							for(unsigned int i = 0; i < limitsInt->data.variants.size; ++i)
								comboBox->addItem(limitsInt->data.variants.name[i], i);
							return comboBox;
						}
					case LIMIT_INT_TYPE_BOOL:
						delegateWidget_ = &DelegateWidgetProxyCheckBox::instance();
						return new QCheckBox(parent);
					default:
						delegateWidget_ = &DelegateWidgetProxySpinBox::instance();
						{							
							QSpinBox * spinBox = new QSpinBox(parent);
							spinBox->setMinimum(std::numeric_limits<int>::min());
							spinBox->setMaximum(std::numeric_limits<int>::max());
							spinBox->setSingleStep(1);
							return spinBox;
						}
				}
			}
		}
		else if(valueType == VALUE_TYPE_REAL)
		{
			delegateWidget_ = &DelegateWidgetProxyDoubleSpinBox::instance();
			QDoubleSpinBox * doubleSpinBox = new QDoubleSpinBox(parent);
			dmLimitsReal * limitsReal = (dmLimitsReal*)data;
			doubleSpinBox->setMinimum(limitsReal != NULL ? limitsReal->min : std::numeric_limits<double>::min());
			doubleSpinBox->setMaximum(limitsReal != NULL ? limitsReal->max : std::numeric_limits<double>::max());
			doubleSpinBox->setDecimals(8);
			doubleSpinBox->setSingleStep(1.0E-4);
			return doubleSpinBox;
		}
	}
	
	QSpinBox * spinBox = new QSpinBox(parent);
	delegateWidget_ = &DelegateWidgetProxySpinBox::instance();
	spinBox->setMinimum(std::numeric_limits<int>::min());
	spinBox->setMaximum(std::numeric_limits<int>::max());
	spinBox->setSingleStep(1);
	return new QSpinBox(parent);
}

void DelegateValue::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
	delegateWidget_->read(editor, index);
}

void DelegateValue::setModelData(QWidget *editor,
								   QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
	delegateWidget_->write(editor, model, index);
}

void DelegateValue::updateEditorGeometry(	QWidget *editor,
											const QStyleOptionViewItem &option,
											const QModelIndex & index ) const
{
	editor->setGeometry(option.rect);
}
