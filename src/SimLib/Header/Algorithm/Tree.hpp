#ifndef _Tree_hpp_
#define _Tree_hpp_

#include <xmemory>
#include <algorithm>
#include <assert.h>

namespace Sim
{

template<	typename _Ty,
			template<typename, typename> class _Cont,
			class _Alloc = std::allocator<_Ty> >
class tree
{
	typedef _Ty value_type;
	typedef _Alloc alloc;
	class _Node;
	typedef _Cont<_Node, _Alloc> _Nodes;
	struct _Node
	{
		_Node(const _Alloc & Alval,_Node * Parent) : _Childs(Alval), _Parent(Parent) {;}
		_Node(const _Alloc & Alval,_Node * Parent, _Ty Value) : _Childs(Alval), _Parent(Parent), _Value(Value) {;}
		_Ty _Value;
		_Node * _Parent;
		_Nodes _Childs;
	};
public:
	class iterator;
	class const_iterator
	{
		friend class tree<_Ty, _Cont>;
		const_iterator(const _Node * Parent, typename _Nodes::const_iterator It) : _Parent(Parent), _It(It)
		{
		}
	public:
		const_iterator()
		{
		}
		const_iterator(iterator It) : _Parent(It._Parent), _It(It._It)
		{
		}
		void next()
		{
			_It++;
		}
		void prev()
		{
			_It--;
		}
		void up()
		{
			_Parent = &*_It;
			_It = _It->_Childs.begin();
		}
		void down()
		{
			_Parent = _Parent->_Parent;
			_It = _Parent->_Childs.begin();			
		}
		iterator & operator--()
		{
			prev();
			return *this;
		}
		iterator & operator++()
		{
			next();
			return *this;
		}
		bool operator == (const_iterator it) const
		{
			return _Parent == it._Parent && _It == it._It;
		}
		bool operator != (const_iterator it) const { return !(*this == it); }
		const _Ty & operator *() { return _It->_Value; }
		const _Ty * operator ->() { return &_It->_Value; }
	private:
		friend class tree<_Ty, _Cont>;
		const _Node * _Parent;
		typename _Nodes::const_iterator _It;
	};

	class iterator
	{
		friend class tree<_Ty, _Cont>;
		iterator(_Node * Parent, typename _Nodes::iterator It) : _Parent(Parent), _It(It)
		{
		}
	public:
		iterator()
		{
		}
		void next()
		{
			_It++;
		}
		void prev()
		{
			_It--;
		}
		void up()
		{
			_Parent = &*_It;
			_It = _It->_Childs.begin();
		}
		void down()
		{
			_Parent = _Parent->_Parent;
			_It = _Parent->_Childs.begin();			
		}
		iterator & operator--()
		{
			prev();
			return *this;
		}
		iterator & operator++()
		{
			next();
			return *this;
		}
		//operator const_iterator() const
		//{
		//	return const_iterator(_Parent, _It);
		//}
		bool operator == (iterator it) const
		{
			return _Parent == it._Parent && _It == it._It;
		}
		bool operator != (iterator it) const { return !(*this == it); }
		_Ty & operator *() { return _It->_Value; }
		_Ty * operator ->() { return &_It->_Value; }
	private:
		friend class tree<_Ty, _Cont>;
		_Node * _Parent;
		typename _Nodes::iterator _It;
	};

	tree() : _Size(0), _Alval(_Alloc()), _Root(_Alval, NULL) {;}
	tree(const _Alloc & Alval) : _Size(0), _Alval(Alval), _Root(_Alval, NULL) {;}
	~tree() {;}
	tree branch(iterator It) const
	{
		tree<_Ty, _Cont> branched;
		branched._Root.push_back(*It._It);
		return
	}
	iterator insert(iterator It, _Ty value)
	{
		assert(It != root() || _Root._Childs.empty());
		++_Size;
		return iterator(It._Parent, It._Parent->_Childs.insert(It._It, _Node(_Alval, It._Parent, value)));
	}
	void push_back(iterator It, _Ty value)
	{
		It._Parent->_Childs.push_back(_Node(_Alval, It._Parent, value));
	}
	iterator erase(iterator It)
	{
		--_Size;
		return iterator(It._Parent, It._Parent->_Childs.erase(It._It));
	}
	iterator root() { return iterator(&_Root, _Root._Childs.begin()); }
	const_iterator root() const { return const_iterator(&_Root, _Root._Childs.begin()); }
	iterator child_begin(iterator it)
	{
		return iterator(it._Parent, it._Parent->_Childs.begin());
	}
	const_iterator child_begin(const_iterator it) const
	{
		return const_iterator(it._Parent, it._Parent->_Childs.begin());
	}
	iterator child_end(iterator it)
	{
		return iterator(it._Parent, it._Parent->_Childs.end());
	}
	const_iterator child_end(const_iterator it) const
	{
		return const_iterator(it._Parent, it._Parent->_Childs.end());
	}
	inline size_t size() const { return _Size; }
private:
	_Alloc _Alval;
	_Node _Root;
	size_t _Size;
};

}

#endif _Tree_hpp_