#include "WidgetData/WidgetDataTree.hpp"
#include "Delegate/DelegateValue.hpp"
#include "Data/IValue.hpp"

#include <assert.h>

#include <QEvent.h>
#include <QStandardItemModel>

#include "String/StringStream.hpp"
#include "Algorithm/Exception.hpp"

//Data Tree

DescTree::DescTree() : color(Qt::black)
{
}

WidgetData * CreateTree::operator()(QWidget * widget, WidgetDataCallback * callback) const
{
	return new WidgetDataTree(desc, widget, editable, callback);
}

QString CreateTree::getName() const
{
	return "Tree";
}

WidgetDataTree::WidgetDataTree(const DescTree & desc, QWidget * parent, bool editable, Callback * callback) : 
	WidgetData(editable, callback), impl_(new Impl(desc, parent, this))
{
}

WidgetDataTree::~WidgetDataTree()
{
	//delete impl_;
}

QWidget * WidgetDataTree::widget()
{
	return impl_;
}

void WidgetDataTree::setData(const ValuePtr & value)
{
	impl_->setData(value);
}

void WidgetDataTree::rebuild()
{
	impl_->rebuild();
}

void WidgetDataTree::onChange(const Path & path)
{
	impl_->onChange(path);
}


static void connectItemLink(QWidget * widget, QStandardItemModel * model)
{
	widget->connect(model,		SIGNAL(itemChanged(QStandardItem*)),
					widget,		SLOT(onItemChanged(QStandardItem*)));
}

static void disconnectItemLink(QWidget * widget, QStandardItemModel * model)
{
	widget->disconnect(	model,	SIGNAL(itemChanged(QStandardItem*)),
						widget,	SLOT(onItemChanged(QStandardItem*)));
}

WidgetDataTreeImpl::WidgetDataTreeImpl(const DescTree & desc, QWidget * parent, WidgetData * widgetData) : 
	QTreeView(parent), desc_(desc), model_(NULL), widgetData_(widgetData)
{
}

WidgetDataTreeImpl::~WidgetDataTreeImpl()
{
	delete widgetData_;
}

static void createItemRow(	QStandardItem * item,
							const QString & parameter,
							unsigned int index,
							bool editable,
							unsigned int valueCol,
							Qt::GlobalColor color,
							const ValuePtr & value)
{
	ValueType valueType = value->getType();

	QStandardItem * itemName = item->child(index, 0);
	if(itemName == NULL)
	{
		itemName = new QStandardItem(parameter);
		itemName->setEditable(false);
		if(valueType == VALUE_TYPE_ARRAY || valueType == VALUE_TYPE_TABLE)
		{
			QList<QVariant> dataList;
			dataList.push_back(QVariant(index));
			itemName->setData(dataList);
		}
		item->setChild(index, 0, itemName);
	}

	QString paramStr = itemName->text();
	
	QStandardItem * itemValue = item->child(index, valueCol);
	if(itemValue == NULL)
	{
		itemValue = new QStandardItem();
		itemValue->setEditable(editable && (valueType == VALUE_TYPE_INTEGER || valueType == VALUE_TYPE_REAL));
		QList<QVariant> dataList;
		dataList.push_back(QVariant(index));
		dataList.push_back(QVariant(valueCol));
		dataList.push_back(valueType);
		if(valueType == VALUE_TYPE_INTEGER)
			dataList.push_back(QVariant((qlonglong)value->getLimitsInt()));
		else if(valueType == VALUE_TYPE_REAL)
			dataList.push_back(QVariant((qlonglong)value->getLimitsReal()));
		itemValue->setData(dataList);
		item->setChild(index, valueCol, itemValue);		
		itemValue->setData(color, Qt::TextColorRole);
	}
	
	switch(valueType)
	{
		case VALUE_TYPE_INTEGER:
			{
				const dmLimitsInt * limitsInt = value->getLimitsInt();
				if(limitsInt != NULL)
				{
					switch(limitsInt->type)
					{
						case LIMIT_INT_TYPE_MIN_MAX:
							itemValue->setText(OSS(value->getInteger()));
							break;
						case LIMIT_INT_TYPE_VARIANTS:
							itemValue->setText(limitsInt->data.variants.name[value->getInteger()]);
							break;
						case LIMIT_INT_TYPE_BOOL:
							itemValue->setText(value->getInteger() != 0 ? "v" : "");
							break;
					}
				}
				else
					itemValue->setText(OSS(value->getInteger()));
			}
			break;
		case VALUE_TYPE_REAL:
			itemValue->setText(OSS(value->getReal()));
			break;
		case VALUE_TYPE_ARRAY:
			{
				const size_t arraySize = value->getSize();
				for(size_t i = 0; i != arraySize; ++i)
					createItemRow(itemName, OSS(i), i, editable, valueCol, color, value->indexArray(i));
				if((size_t)itemName->rowCount() > arraySize)
					itemName->removeRows(arraySize, itemName->rowCount() - (int)arraySize);
			}
			break;
		case VALUE_TYPE_TABLE:
			{
				const size_t tableSize = value->getSize();
				for(size_t i = 0; i != tableSize; ++i)
				{
					ItemTable itemTable = value->indexTable(i);
					createItemRow(itemName, itemTable.field, i, editable, valueCol, color, itemTable.value);
				}
				if(itemName->rowCount() > (int)tableSize)
					itemName->removeRows(tableSize, itemName->rowCount() - (int)tableSize);
			}
			break;
	}
}

void WidgetDataTreeImpl::setData(const ValuePtr & valueIn)
{
	value_ = valueIn;
	
	applyData_();

	expandAll();
	resizeColumnToContents(0);
	resizeColumnToContents(1);
}

void WidgetDataTreeImpl::rebuild()
{
	applyData_();
}

static void onItemChanged(WidgetData * widgetData,
						  QStandardItem * item,
						  const ValuePtr & value)
{
	Path path;
	{
		QStandardItem * itemZero = item;
		while(	itemZero != NULL &&
				itemZero->parent() != NULL)
		{
			const QList<QVariant> curItemDataList = itemZero->data().toList();
			const unsigned int index = curItemDataList.at(0).toUInt();
			path.push_front(index);
			itemZero = itemZero->parent();
		}
	}
	if(path.empty())
		return;

	ValuePtr itemValue  = value->index(path);

	ValueType valueType = itemValue->getType();
	if(valueType == VALUE_TYPE_INTEGER)
	{
		const dmLimitsInt * limitsInt = itemValue->getLimitsInt();
		if(	limitsInt != NULL)
		{
			if(limitsInt->type == LIMIT_INT_TYPE_VARIANTS)
			{
				int variantIndex = std::find(limitsInt->data.variants.name, limitsInt->data.variants.name + limitsInt->data.variants.size, item->text()) - limitsInt->data.variants.name;
				itemValue->setInteger(variantIndex);
			}
			else if(limitsInt->type == LIMIT_INT_TYPE_BOOL)
				itemValue->setInteger(item->text().isEmpty() ? 1 : 0);
			else
				itemValue->setInteger(item->text().toUInt());
		}
		else
			itemValue->setInteger(item->text().toUInt());
	}
	else if(valueType == VALUE_TYPE_REAL)
		itemValue->setReal(item->text().toDouble());
	//else
	//	THROW_EXCEPTION(Sim::Exception, "The item is nor integer nor double");
	itemValue->notifyChange();
	widgetData->notifyChange(path);
}

void WidgetDataTreeImpl::onItemChanged(QStandardItem * item)
{
	::onItemChanged(widgetData_, item, value_);
}

void WidgetDataTreeImpl::applyData_()
{
	if(model_ == NULL)
	{
		QStringList labels;
		labels.push_back(tr("Name"));
		labels.push_back(tr("Value"));

		if(model_ != NULL)
			delete model_;
		model_ = new QStandardItemModel();
		model_->setHorizontalHeaderLabels(labels);

		setModel(model_);
	
		DelegateValue * valueDelegate = new DelegateValue(model_);
		setItemDelegate(valueDelegate);
	}

	disconnectItemLink(this, model_);
	QStandardItem * parentItem = model_->invisibleRootItem();
	createItemRow(parentItem, tr("root"), 0, widgetData_->isEditable(), 1, Qt::black, value_);
	connectItemLink(this, model_);
}

static void onChange(QWidget * self,
					 const Path & path, QStandardItemModel * model,
					 const ValuePtr & value, bool editable,
					 unsigned int valCol, Qt::GlobalColor color)
{
	disconnectItemLink(self, model);
	if(path.empty())
		return;
	ValuePtr itemValue = value->index(path);
	QStandardItem * item = model->invisibleRootItem()->child(0, 0);
	for(Path::const_iterator itP = path.begin();
		itP != path.end() - 1;
		++itP)
	{
		const vIndex index = *itP;
		item = item->child(index, 0);
	}
	createItemRow(item, item->text(), path.back(), editable, valCol, color, itemValue);
	connectItemLink(self, model);
}

void WidgetDataTreeImpl::onChange(const Path & path)
{
	::onChange(this, path, model_, value_, widgetData_->isEditable(), 1, Qt::black);
}

//Data Tree Comp

DescTreeComp::DescTreeComp() : colorComp(Qt::red)
{
}

WidgetDataComp * CreateTreeComp::operator()(QWidget * widget, WidgetDataCallback * callback) const
{
	return new WidgetDataCompTree(desc, widget, editable, refEditable, callback);
}

QString CreateTreeComp::getName() const
{
	return "Tree";
}

WidgetDataCompTree::WidgetDataCompTree(const DescTreeComp & desc, QWidget * parent, bool editable, bool refEditable, Callback * callback) : 
	WidgetDataComp(editable, refEditable, callback), impl_(new Impl(desc, parent, this))
{
}

WidgetDataCompTree::~WidgetDataCompTree()
{
	//delete impl_;
}

QWidget * WidgetDataCompTree::widget()
{
	return impl_;
}

void WidgetDataCompTree::setData(const ValuePtr & valueResult)
{
	impl_->setData(valueResult);
}

void WidgetDataCompTree::rebuild()
{
	impl_->rebuild();
}

void WidgetDataCompTree::setDataComp(const ValuePtr & valueResult, const ValuePtr & valueReference)
{
	impl_->setDataComp(valueResult, valueReference);
}

void WidgetDataCompTree::resetReferenceData()
{
	impl_->resetReferenceData();
}

void WidgetDataCompTree::onChange(const Path & path)
{
	impl_->onChange(path);
}

WidgetDataTreeCompImpl::WidgetDataTreeCompImpl(const DescTreeComp & desc, QWidget * parent, WidgetDataComp * widgetDataComp) : 
	QTreeView(parent), desc_(desc), model_(NULL), widgetDataComp_(widgetDataComp)
{
}

WidgetDataTreeCompImpl::~WidgetDataTreeCompImpl()
{
}

void WidgetDataTreeCompImpl::setData(const ValuePtr & valueIn)
{
	valueResult_ = valueIn;

	applyData_();

	expandAll();
	resizeColumnToContents(0);
	resizeColumnToContents(1);	
}

void WidgetDataTreeCompImpl::rebuild()
{
	applyData_();
}

void WidgetDataTreeCompImpl::setDataComp(const ValuePtr & valueResult, const ValuePtr & valueReference)
{
	valueResult_ = valueResult;

	if(	model_ == NULL ||
		valueReference_ == NULL)
	{
		QStringList labels;
		labels.push_back(tr("Parameter"));
		labels.push_back(tr("Result"));
		labels.push_back(tr("Reference"));

		if(model_ != NULL)
			delete model_;
		model_ = new QStandardItemModel();
		model_->setHorizontalHeaderLabels(labels);

		setModel(model_);
		
		DelegateValue * valueDelegate = new DelegateValue(model_);
		setItemDelegate(valueDelegate);
	}

	valueReference_ = valueReference;

	disconnectItemLink(this, model_);
	QStandardItem * parentItem = model_->invisibleRootItem();
	createItemRow(parentItem, tr("root"), 0, widgetDataComp_->isEditable(), 1, desc_.color, valueResult_);
	createItemRow(parentItem, tr("root"), 0, widgetDataComp_->isRefEditable() , 2, Qt::red, valueReference_);
	connectItemLink(this, model_);

	expandAll();
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	resizeColumnToContents(2);
}

void WidgetDataTreeCompImpl::resetReferenceData()
{
	applyData_();
}

void WidgetDataTreeCompImpl::onItemChanged(QStandardItem * item)
{
	int colIndex = item->data().toList().at(1).toUInt();
	::onItemChanged(widgetDataComp_, item, colIndex == 1 ? valueResult_ : valueReference_);
}

void WidgetDataTreeCompImpl::onChange(const Path & path)
{
	::onChange(this, path, model_, valueReference_, true, 2, Qt::red);
}

void WidgetDataTreeCompImpl::applyData_()
{
	if(model_ == NULL || valueReference_ != NULL)
	{
		QStringList labels;
		labels.push_back(tr("Name"));
		labels.push_back(tr("Value"));

		if(model_ != NULL)
			delete model_;
		model_ = new QStandardItemModel();
		model_->setHorizontalHeaderLabels(labels);

		setModel(model_);

		DelegateValue * valueDelegate = new DelegateValue(model_);
		setItemDelegate(valueDelegate);
	}

	if(valueReference_ != NULL)
		valueReference_.reset();

	disconnectItemLink(this, model_);
	QStandardItem * parentItem = model_->invisibleRootItem();
	createItemRow(parentItem, tr("root"), 0, widgetDataComp_->isEditable(), 1, desc_.color, valueResult_);
	connectItemLink(this, model_);
}