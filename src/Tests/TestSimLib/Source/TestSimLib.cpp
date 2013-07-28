#include "stdafx.h"

#include "Algorithm/Tree.hpp"

#include <vector>
#include <list>
#include <set>

template<	typename T,
			template<typename, typename> class Cont>
void fillTree(Sim::tree<T, Cont> & tree)
{
	Sim::tree<T, Cont>::iterator it;
	it = tree.insert(tree.root(), 0);
	it.up();
	tree.push_back(it, 2);
	tree.insert(tree.child_begin(it), 1);
	tree.push_back(it, 30);
	it = --tree.child_end(it);
	it.up();
		tree.push_back(it, 31);
		tree.push_back(it, 32);
	it.down();
	tree.push_back(it, 40);
	it = --tree.child_end(it);
	it.up();
		tree.push_back(it, 41);
		tree.push_back(it, 42);
	it.down();

	it = tree.root();
	it.up();
	++it;
	++it;
	tree.erase(it);
}

template<	typename T,
			template<typename, typename> class Cont>
void printTree(const typename Sim::tree<T, Cont> & tr, typename Sim::tree<T, Cont>::const_iterator it)
{
	printf("%d\n", *it);
	it.up();
	for(it = tr.child_begin(it);
		it != tr.child_end(it);
		it.next())
	{
		printTree<T, Cont>(tr, it);
	}
	it.down();
}

template<	typename T,
			template<typename, typename> class Cont>
void testTree()
{
	Sim::tree<T, Cont> tree;
	fillTree(tree);
	printTree<T, Cont>(tree, tree.root());
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Testing SIM::tree<int, std::vector>...\n");
	testTree<int, std::vector>();
	printf("Testing SIM::tree<int, std::list>...\n");
	testTree<int, std::list>();
}