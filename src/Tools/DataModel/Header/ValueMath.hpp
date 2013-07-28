#ifndef _ValueMath_hpp_
#define _ValueMath_hpp_

#include "Value.hpp"

#include "Math/Vec3.hpp"
#include "Math/Mat3x3.hpp"

namespace Sim { namespace Math {

template<typename A, typename T, class Scalars> class LinearInterpolator;
template<typename A, typename T, class Scalars> class BilinearInterpolator;
template<typename T, size_t order> class Polynom;
template<typename T, size_t order1, size_t order2> class FamilyOfPolynoms;

} }

namespace DataModel
{

inline ValueArray * createValue(Sim::Math::Vec3 & vec3, ChangeHandler * changeHandler)
{
	return createValueArrayRanIt((btScalar*)(vec3.m_floats), (btScalar*)(vec3.m_floats) + 3, changeHandler);
}

inline ValueArray * createValue(Sim::Math::Matrix3x3 & matrix3x3, ChangeHandler * changeHandler)
{
	return createValueArrayRanIt(&(matrix3x3[0]), &(matrix3x3[0]) + 3, changeHandler);
}

template<class LinearInterpolator>
class ValueLinearInterpolatorArg : public Value
{
public:
	ValueLinearInterpolatorArg(	LinearInterpolator * linearInterpolator,
								typename LinearInterpolator::Index index,
								ChangeHandler * changeHandlerIn) :	Value(changeHandlerIn),
																	linearInterpolator_(linearInterpolator),
																	index_(index)
	{
	}
	virtual Type		getType	() const { return VALUE_TYPE_REAL; }
	virtual dmReal 		getReal	() const { return linearInterpolator_->getElement(index_).arg; }
	virtual const dmLimitsReal * getLimitsReal() const { return NULL; }
	virtual void		setReal	(dmReal val)
	{
		LinearInterpolator::Argument oldArg = linearInterpolator_->getElement(index_).arg;
		if(oldArg != oldArg)
			linearInterpolator_->set(index_, (LinearInterpolator::Argument)val, LinearInterpolator::Value());
		else
			linearInterpolator_->set(index_, (LinearInterpolator::Argument)val, linearInterpolator_->getElement(index_).val);
		(*changeHandler)(this);
	}
private:
	LinearInterpolator * linearInterpolator_;
	typename LinearInterpolator::Index index_;
};

template<class LinearInterpolator>
ValueTable * createValueLinearInterpolatorElement(LinearInterpolator & linearInterpolator,
												  const char * captionArg,
												  const char * captionVal,
												  typename LinearInterpolator::Index index,
												  ChangeHandler * changeHandler)
{
	ItemTable itemTable[] = {
		ItemTable(captionArg, new ValueLinearInterpolatorArg<LinearInterpolator>(&linearInterpolator, index, changeHandler)),
		ItemTable(captionVal, createValue(linearInterpolator.getElement(index).val, changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

template<class LinearInterpolator>
class ValueLinearInterpolator : public Value
{
public:
	ValueLinearInterpolator(LinearInterpolator * linearInterpolator,
							const char * captionArg,
							const char * captionVal,
							ChangeHandler * changeHandlerIn) :
		Value(changeHandlerIn), linearInterpolator_(linearInterpolator),
		captionArg_(captionArg), captionVal_(captionVal)
	{
		build_();
	}	
	~ValueLinearInterpolator()
	{
		clear_();
	}
	virtual Type getType() const
	{
		return VALUE_TYPE_ARRAY;
	}
	virtual dmIndex getSize() const
	{
		return linearInterpolator_->size();
	}
	virtual Value * index(dmIndex index)
	{
		return elements_[index];
	}
	virtual const Value * index(dmIndex index) const
	{
		return elements_[index];
	}
	virtual Value * indexArray(dmIndex index)
	{
		return elements_[index];
	}
	virtual const Value* indexArray(dmIndex index) const
	{
		return elements_[index];
	}
	virtual Value* addItem(dmIndex index)
	{
		linearInterpolator_->insert(index,
									std::numeric_limits<LinearInterpolator::Argument>::quiet_NaN(),
									LinearInterpolator::Value());
		rebuild_();
		return elements_[index];
	}
	virtual void removeItem(dmIndex index)
	{
		linearInterpolator_->remove(index);
		rebuild_();
	}
private:
	void clear_()
	{
		for(Elements::iterator itE = elements_.begin();
			itE != elements_.end();
			++itE)
			delete *itE;
	}
	void build_()
	{
		typename const LinearInterpolator::Size size = linearInterpolator_->size();
		elements_.resize(size);
		for(typename LinearInterpolator::Index i = 0;
			i < size;
			++i)
			elements_[i] = createValueLinearInterpolatorElement(*linearInterpolator_,
																captionArg_,
																captionVal_,
																i,
																changeHandler);
	}
	void rebuild_()
	{
		clear_();
		build_();
	}
	typedef std::vector<ValueTable*> Elements;
	Elements elements_;
	LinearInterpolator * linearInterpolator_;
	const char * captionArg_;
	const char * captionVal_;
};

template<typename A, typename T, class Scalars>
ValueLinearInterpolator<Sim::Math::LinearInterpolator<A, T, Scalars>> * 
	createValueLinearInterpolator(Sim::Math::LinearInterpolator<A, T, Scalars> & linearInterpolator,
				const char * captionArg,
				const char * captionVal,
				ChangeHandler * changeHandler)
{
	return new ValueLinearInterpolator<Sim::Math::LinearInterpolator<A, T, Scalars>>(	&linearInterpolator,
																						captionArg,
																						captionVal,
																						changeHandler);
}

template<class BilinearInterpolator>
class ValueBilinearInterpolatorCell : public Value
{
public:
	ValueBilinearInterpolatorCell(	BilinearInterpolator * bilinearInterpolator,
									typename BilinearInterpolator::Index index,
									ChangeHandler * changeHandlerIn) :	Value(changeHandlerIn),
																		bilinearInterpolator_(bilinearInterpolator),
																		index_(index)
	{
	}
	virtual Type		getType	() const { return VALUE_TYPE_REAL; }
	virtual dmReal 		getReal	() const { return bilinearInterpolator_->getColArg(index_.col); }
	virtual const dmLimitsReal * getLimitsReal() const { return NULL; }
	virtual void		setReal	(dmReal val)
	{
		BilinearInterpolator::Argument rowArg = bilinearInterpolator_->getRowArg(index_.row);
		BilinearInterpolator::Argument oldColArg = bilinearInterpolator_->getColArg(index_.col);
		if(oldColArg != oldColArg)
		{
			for(size_t row = 0; row < bilinearInterpolator_->size().row; ++row)
			{
				const BilinearInterpolator::Index nextIndex(row, index_.col);
				bilinearInterpolator_->setColArg(nextIndex.col, val);
				bilinearInterpolator_->setValue(nextIndex,
												bilinearInterpolator_->eval(rowArg, val,
																			BilinearInterpolator::Index(nextIndex.row, nextIndex.col > 0 ? nextIndex.col - 1 : 0),
																			BilinearInterpolator::Index(nextIndex.row, std::min(nextIndex.col + 1, bilinearInterpolator_->size().col - 1))));
			}
		}
		else
		{
			bilinearInterpolator_->setColArg(index_.col, val);
			bilinearInterpolator_->setValue(index_, bilinearInterpolator_->getValue(index_));
		}
		(*changeHandler)(this);
	}
private:
	BilinearInterpolator * bilinearInterpolator_;
	typename BilinearInterpolator::Index index_;
};

template<class BilinearInterpolator>
ValueTable * createValueBiinearInterpolatorCell(BilinearInterpolator & bilinearInterpolator,
												typename BilinearInterpolator::Index index,
												const char * captionArg,
												const char * captionVal,												
												ChangeHandler * changeHandler)
{
	ItemTable itemTable[] = {
		ItemTable(captionArg, new ValueBilinearInterpolatorCell<BilinearInterpolator>(&bilinearInterpolator, index, changeHandler)),
		ItemTable(captionVal, createValue(bilinearInterpolator.getValue(index), changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

template<class BilinearInterpolator> class ValueBilinearInterpolator;

template<class BilinearInterpolator>
class ValueBilinearInterpolatorCol : public Value
{
public:
	ValueBilinearInterpolatorCol(	BilinearInterpolator * bilinearInterpolator,
									typename BilinearInterpolator::Index::SubIndex row,
									const char * captionArg,
									const char * captionVal,
									ValueBilinearInterpolator<BilinearInterpolator> * valueBilinearInterpolator,
									ChangeHandler * changeHandlerIn) :
	Value(changeHandlerIn), bilinearInterpolator_(bilinearInterpolator), row_(row),
	captionArg_(captionArg), captionVal_(captionVal), valueBilinearInterpolator_(valueBilinearInterpolator)
	{
		build_();
	}	
	~ValueBilinearInterpolatorCol()
	{
		clear_();
	}
	virtual Type getType() const
	{
		return VALUE_TYPE_ARRAY;
	}
	virtual dmIndex getSize() const
	{
		return bilinearInterpolator_->size().col;
	}
	virtual Value * index(dmIndex index)
	{
		return elements_[index];
	}
	virtual const Value * index(dmIndex index) const
	{
		return elements_[index];
	}
	virtual Value * indexArray(dmIndex index)
	{
		return elements_[index];
	}
	virtual const Value* indexArray(dmIndex index) const
	{
		return elements_[index];
	}
	virtual Value* addItem(dmIndex index)
	{
		valueBilinearInterpolator_->insertCol(index);
		return elements_[index];
	}
	virtual void removeItem(dmIndex index)
	{
		bilinearInterpolator_->removeColArg(index);
		rebuild_();
	}
	virtual void rebuild(dmIndex index)
	{
		rebuild_();
	}
private:
	void clear_()
	{
		for(Elements::iterator itE = elements_.begin();
			itE != elements_.end();
			++itE)
			delete *itE;
	}
	void build_()
	{
		typename const BilinearInterpolator::Size::SubIndex colCount = bilinearInterpolator_->size().col;
		elements_.resize(colCount);
		for(typename BilinearInterpolator::Index::SubIndex i = 0;
			i < colCount;
			++i)
			elements_[i] = createValueBiinearInterpolatorCell(	*bilinearInterpolator_,
																BilinearInterpolator::Index(row_, i),
																captionArg_,
																captionVal_,
																changeHandler);
	}
	void rebuild_()
	{
		clear_();
		build_();
	}
	typedef std::vector<ValueTable*> Elements;
	Elements elements_;
	BilinearInterpolator * bilinearInterpolator_;
	typename BilinearInterpolator::Index::SubIndex row_;
	const char * captionArg_;
	const char * captionVal_;
	ValueBilinearInterpolator<BilinearInterpolator> * valueBilinearInterpolator_;
};

template<typename A, typename T, class Scalars>
ValueBilinearInterpolatorCol<Sim::Math::BilinearInterpolator<A, T, Scalars>> * 
	createValueBilinearInterpolatorCol(	Sim::Math::BilinearInterpolator<A, T, Scalars> & bilinearInterpolator,
										typename Sim::Math::BilinearInterpolator<A, T, Scalars>::Index::SubIndex row,
										const char * captionArg,
										const char * captionVal,
										ValueBilinearInterpolator<Sim::Math::BilinearInterpolator<A, T, Scalars>> * valueBilinearInterpolator,
										ChangeHandler * changeHandler)
{
	return new ValueBilinearInterpolatorCol<Sim::Math::BilinearInterpolator<A, T, Scalars>>(&bilinearInterpolator,
																							row,
																							captionArg,
																							captionVal,
																							valueBilinearInterpolator,
																							changeHandler);
}

template<typename A, typename T, class Scalars>
ValueTable * createValueBilinearInterpolatorRow(	Sim::Math::BilinearInterpolator<A, T, Scalars> & bilinearInterpolator,
													typename Sim::Math::BilinearInterpolator<A, T, Scalars>::Index::SubIndex row,
													const char * captionRowArg,
													const char * captionColArg,
													const char * captionVal,
													ValueBilinearInterpolator<Sim::Math::BilinearInterpolator<A, T, Scalars>> * valueBilinearInterpolator,
													ChangeHandler * changeHandler)
{
	ItemTable itemTable[] = {
		ItemTable(captionRowArg,	createValue(bilinearInterpolator.getRowArg(row), changeHandler)),
		ItemTable(captionVal,		createValueBilinearInterpolatorCol(bilinearInterpolator, row, captionColArg, captionVal, valueBilinearInterpolator, changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

template<class BilinearInterpolator>
class ValueBilinearInterpolator : public Value
{
public:
	ValueBilinearInterpolator(	BilinearInterpolator * bilinearInterpolator,
								const char * captionRowArg,
								const char * captionColArg,
								const char * captionVal,
								ChangeHandler * changeHandlerIn) :
	Value(changeHandlerIn), bilinearInterpolator_(bilinearInterpolator),
		captionRowArg_(captionRowArg), captionColArg_(captionColArg), captionVal_(captionVal)
	{
		build_();
	}	
	~ValueBilinearInterpolator()
	{
		clear_();
	}
	virtual Type getType() const
	{
		return VALUE_TYPE_ARRAY;
	}
	virtual dmIndex getSize() const
	{
		return bilinearInterpolator_->size().row;
	}
	virtual Value * index(dmIndex index)
	{
		return elements_[index];
	}
	virtual const Value * index(dmIndex index) const
	{
		return elements_[index];
	}
	virtual Value * indexArray(dmIndex index)
	{
		return elements_[index];
	}
	virtual const Value* indexArray(dmIndex index) const
	{
		return elements_[index];
	}
	virtual Value* addItem(dmIndex index)
	{
		return NULL;
	}
	virtual void removeItem(dmIndex index)
	{
	}
	void insertCol(dmIndex index)
	{
		bilinearInterpolator_->insertColArg(index, std::numeric_limits<BilinearInterpolator::Argument>::quiet_NaN());
		for(Elements::iterator itE = elements_.begin();
			itE != elements_.end();
			++itE)
		{
			ValueBilinearInterpolatorCol<BilinearInterpolator> * valueBilinearInterpolatorCol = (ValueBilinearInterpolatorCol<BilinearInterpolator>*)((*itE)->indexTable(1).value);
			valueBilinearInterpolatorCol->rebuild(index);
		}
	}
private:
	void clear_()
	{
		for(Elements::iterator itE = elements_.begin();
			itE != elements_.end();
			++itE)
			delete *itE;
	}
	void build_()
	{
		typename const BilinearInterpolator::Size::SubIndex colCount = bilinearInterpolator_->size().row;
		elements_.resize(colCount);
		for(typename BilinearInterpolator::Index::SubIndex i = 0;
			i < colCount;
			++i)
			elements_[i] = createValueBilinearInterpolatorRow(	*bilinearInterpolator_,
																i,
																captionRowArg_,
																captionColArg_,
																captionVal_,
																this,
																changeHandler);
	}
	void rebuild_()
	{
		clear_();
		build_();
	}
	typedef std::vector<ValueTable*> Elements;
	Elements elements_;
	BilinearInterpolator * bilinearInterpolator_;
	typename BilinearInterpolator::Index::SubIndex row_;
	const char * captionRowArg_;
	const char * captionColArg_;
	const char * captionVal_;
};

template<typename A, typename T, class Scalars>
ValueBilinearInterpolator<Sim::Math::BilinearInterpolator<A, T, Scalars>> * 
	createValueBilinearInterpolator(	Sim::Math::BilinearInterpolator<A, T, Scalars> & bilinearInterpolator,
										const char * captionRowArg,
										const char * captionColArg,
										const char * captionVal,
										ChangeHandler * changeHandler)
{
	return new ValueBilinearInterpolator<Sim::Math::BilinearInterpolator<A, T, Scalars>>(	&bilinearInterpolator,
																							captionRowArg,
																							captionColArg,
																							captionVal,
																							changeHandler);
}

template<typename T, size_t order>
inline ValueArray * createValue(Sim::Math::Polynom<T, order> & polynom, ChangeHandler * changeHandler)
{
	return createValue(polynom.getCoeffs(), Sim::Math::Polynom<T, order>::COEFS_COUNT, changeHandler);
}

template<typename T, size_t order1, size_t order2>
inline ValueArray * createValue(Sim::Math::FamilyOfPolynoms<T, order1, order2> & familyOfPolynoms, ChangeHandler * changeHandler)
{
	return createValue(familyOfPolynoms.getPolynoms(), Sim::Math::FamilyOfPolynoms<T, order1, order2>::COEFFS_COUNT, changeHandler);
}


}

#endif _ValueMath_hpp_