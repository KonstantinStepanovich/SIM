#ifndef _WidgetDataTree_hpp_
#define _WidgetDataTree_hpp_

#include "WidgetData/WidgetData.hpp"

#include <QString.h>
#include <QTreeView.h>

class QStandardItem;
class QStandardItemModel;

//Data Tree

struct DescTree
{
	DescTree();
	Qt::GlobalColor color;
};

class CreateTree : public WidgetDataCreate
{
public:
	typedef DescTree Desc;
	virtual WidgetData * operator()(QWidget * widget, WidgetDataCallback * callback) const;
	virtual QString getName() const;
	Desc desc;
};

class WidgetDataTreeImpl;

class WidgetDataTree : public WidgetData
{
public:
	typedef WidgetDataTreeImpl Impl;
	WidgetDataTree(const DescTree & desc, QWidget * parent, bool editableIn, Callback * callbackIn = NULL);
	~WidgetDataTree();
	virtual QWidget * widget();
	virtual void setData(const ValuePtr & value);
	virtual void onChange(const Path & path);
	virtual void rebuild();
private:
	Impl * impl_;
};

class WidgetDataTreeImpl : public QTreeView
{
	Q_OBJECT
public:
	WidgetDataTreeImpl(const DescTree & desc, QWidget * parent, WidgetData * widgetDataIn);
	~WidgetDataTreeImpl();
	void setData(const ValuePtr & valueIn);
	void onChange(const Path & path);
	void rebuild();
protected:
	void onItemChanged(QStandardItem * item);
private:
	void applyData_();
	const DescTree & desc_;
	ValuePtr value_;
	QStandardItemModel * model_;
	WidgetData * widgetData_;
};

//Data Tree Comp

struct DescTreeComp : public DescTree
{
	DescTreeComp();
	Qt::GlobalColor colorComp;
};

class CreateTreeComp : public WidgetDataCompCreate
{
public:
	typedef DescTreeComp Desc;
	virtual WidgetDataComp * operator()(QWidget * widget, WidgetDataCallback * callback) const;
	virtual QString getName() const;
	Desc desc;
};

class WidgetDataTreeCompImpl;

class WidgetDataCompTree : public WidgetDataComp
{
public:
	typedef WidgetDataTreeCompImpl Impl;
	WidgetDataCompTree(const DescTreeComp & desc, QWidget * parent, bool editableIn, bool refEditableIn, Callback * callbackIn = NULL);
	~WidgetDataCompTree();
	virtual QWidget * widget();
	virtual void setData(const ValuePtr & valueResult);
	virtual void setDataComp(const ValuePtr & valueResult, const ValuePtr & dataNavReference);
	virtual void resetReferenceData();
	virtual void rebuild();
	virtual void onChange(const Path & path);
private:
	Impl * impl_;
};

class WidgetDataTreeCompImpl : public QTreeView
{
	Q_OBJECT
public:
	WidgetDataTreeCompImpl(const DescTreeComp & desc, QWidget * parent, WidgetDataComp * widgetDataComp);
	~WidgetDataTreeCompImpl();
	void setData(const ValuePtr & valueIn);
	void setDataComp(const ValuePtr & dataNavResult, const ValuePtr & valueReference);
	void resetReferenceData();
	void rebuild();
	void onItemChanged(QStandardItem * item);
	void onChange(const Path & path);
private:
	void applyData_();
	const DescTreeComp & desc_;
	ValuePtr valueResult_;
	QStandardItemModel * model_;
	WidgetDataComp * widgetDataComp_;
	ValuePtr valueReference_;
};

#endif _WidgetDataTree_hpp_