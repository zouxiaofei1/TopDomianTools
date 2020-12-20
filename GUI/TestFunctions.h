//为了减短GUI.cpp长度
//一部分调试用函数、测试中的函数将被放在这里
//by zouxiaofei1 2015 - 2021

#pragma once
#include "stdafx.h"
#include "GUI.h"
#include "mystd.h"
#pragma warning(disable:4996)
#pragma warning(disable:26812)

//void s() { MessageBox(NULL, 0, 0, NULL); }
//void s(LPCWSTR a) { MessageBox(NULL, a, 0, NULL); }//调试用MessageBox
//void s(LPCSTR a) { MessageBoxA(NULL, a, 0, NULL); }
//void s(int a)//当程序正式发布时可以去掉这几个函数
//{
//	wchar_t tmp[20];
//	myitow(a, tmp, 0);
//	MessageBox(NULL, tmp, L"", NULL);
//}
//
//void s2(LPCWSTR a) { OutputDebugString(a); }//调试用OutputDebugString
//void s2(int a)
//{
//	wchar_t tmp[34];
//	myitow(a, tmp, MAX_NUM); mywcscat(tmp, L"\n");
//	OutputDebugString(tmp);
//}
//void s2() { OutputDebugString(L"0\n"); }
//
//#define beg tw a;
//#define edd a.s();
//class tw//调试用的微秒级计时器
//{
//public:
//	tw()//定义时自动开始
//		: elapsed_(0)
//	{
//		QueryPerformanceFrequency(&freq_);
//		QueryPerformanceCounter(&begin_time_);
//	}
//	~tw() {}
//public:
//	void s()
//	{//结束时自动打印时间
//		LARGE_INTEGER end_time;
//		QueryPerformanceCounter(&end_time);
//		elapsed_ += (end_time.QuadPart - begin_time_.QuadPart) * 1000000 / freq_.QuadPart;
//		::s((int)(elapsed_));
//	}
//	double ms()
//	{
//		return elapsed_ / 1000.0;
//	}
//	long long elapsed_;
//private:
//	LARGE_INTEGER freq_;
//	LARGE_INTEGER begin_time_;
//
//};


//
//为了减少程序体积，从网上抄来的红黑树map代码
//

#pragma warning (disable:6011)
namespace fbcstd {

	template<class T1, class T2>
	struct pair {
		typedef T1 first_type;
		typedef T2 second_type;

		// default construct
		pair() : first(), second() {}
		// construct from compatible pair
		template<class U, class V>
		pair(const pair<U, V>& pr) : first(pr.first), second(pr.second) {}
		// construct from specified values
		pair(const first_type& a, const second_type& b) : first(a), second(b) {}

		// assign from copied pair
		pair& operator = (const pair& pr)
		{
			first = pr.first;
			second = pr.second;

			return (*this);
		}

		T1 first;
		T2 second;
	}; // struct pair

	// relational operators for pair
	template<class T1, class T2>
	inline bool operator == (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return (lhs.first == rhs.first && lhs.second == rhs.second);
	}

	template<class T1, class T2>
	inline bool operator != (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return (!(lhs == rhs));
	}

	template<class T1, class T2>
	inline bool operator < (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return (lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second));
	}
	template<class T1, class T2>
	inline bool operator > (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return (rhs < lhs);
	}
	template<class T1, class T2>
	inline bool operator <= (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return (!(rhs < lhs));
	}
	template<class T1, class T2>
	inline bool operator >= (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
	{
		return (!(lhs < rhs));
	}
} // namespace fbcstd

enum COLOR { RED, BLACK };
template<class K, class V>
struct RBTreeNode {
	RBTreeNode<K, V>* _pLeft;
	RBTreeNode<K, V>* _pRight;
	RBTreeNode<K, V>* _pParent;
	fbcstd::pair<K, V> _value;
	COLOR _color;

	RBTreeNode(const K& key = K(), const V& value = V(), COLOR color = RED)
	{
		_pLeft = NULL;
		_pRight = NULL;
		_pParent = NULL;
		_value(key, value);
	}
};


template<class K, class V>
class RBTreeiterator
{
	typedef RBTreeNode<K, V> Node;
	typedef Node* PNode;
	typedef RBTreeiterator<K, V> Self;

public:
	RBTreeiterator(PNode pNode = NULL)
		: _pNode(pNode)
	{}

	RBTreeiterator(const Self& s)
		: _pNode(s._pNode)
	{}

	fbcstd::pair<K, V>& operator*()
	{
		return _pNode->_value;
	}

	fbcstd::pair<K, V>* operator->()
	{
		return &(operator*());
	}

	Self& operator++()
	{
		RBTreeItIncrement();
		return *this;
	}

	Self operator++(int)
	{
		Self temp(*this);
		RBTreeItIncrement();
		return temp;
	}

	Self& operator--()
	{
		RBTreeItDecrement();
		return *this;
	}

	Self operator--(int)
	{
		Self temp(*this);
		RBTreeItDecrement();
		return temp;
	}


	bool operator!=(const Self& s)
	{
		return _pNode != s._pNode;
	}

private:
	void RBTreeItIncrement()
	{
		if (_pNode->_pRight)
		{
			_pNode = _pNode->_pRight;
			while (_pNode->_pLeft)
				_pNode = _pNode->_pLeft;
		}
		else
		{
			PNode pParent = _pNode->_pParent;
			while (pParent->_pRight == _pNode)
			{
				_pNode = pParent;
				pParent = _pNode->_pParent;
			}

			// 如果树的根节点没有右孩子的情况且迭代器起始位置在根节点
			if (_pNode->_pRight != pParent)
				_pNode = pParent;
		}
	}

	void RBTreeItDecrement()
	{
		if (_pNode->_pParent->_pParent == _pNode && RED == _pNode->_color)
		{
			_pNode = _pNode->_pRight;
		}
		else if (_pNode->_pLeft)
		{
			// 在当前节点左子树中找最大的结点
			_pNode = _pNode->_pLeft;
			while (_pNode->_pRight)
				_pNode = _pNode->_pRight;
		}
		else
		{
			PNode pParent = _pNode;
			while (pParent->_pLeft == _pNode)
			{
				_pNode = pParent;
				pParent = _pNode->_pParent;
			}

			_pNode = pParent;
		}
	}

private:
	PNode _pNode;
};


template<class K, class V>
class RBTree {
	//typedef RBTreeiterator<K, V> Iterator;
	typedef RBTreeNode<K, V> Node;
	typedef Node* PNode;
public:
	typedef RBTreeiterator<K, V> Iterator;
public:
	void InitRBTree()
	{
		_pHead = (Node*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Node));
	}
	Iterator Begin()
	{
		return Iterator(_pHead->_pLeft);
	}
	Iterator End()
	{
		return Iterator(_pHead);
	}
	inline PNode& GetRoot()
	{
		return _pHead->_pParent;
	}
	fbcstd::pair<Iterator, bool> InsertUnique(const fbcstd::pair<K, V>& value)
	{
		PNode& _pRoot = GetRoot();
		PNode newNode = NULL;
		if (_pRoot == 0) {
			Node* a = (Node*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Node));
			a->_value.first = value.first;
			a->_value.second = value.second;
			a->_color = BLACK;// s(3);
			newNode = _pRoot = a;
			//newNode = _pRoot = &a;
			_pRoot->_pParent = _pHead;

		}
		else {

			PNode pCur = _pRoot;
			PNode pParent = pCur;
			while (pCur) {
				if (pCur->_value.first < value.first) {
					pParent = pCur;
					pCur = pCur->_pRight;
				}
				else if (pCur->_value.first > value.first) {
					pParent = pCur;
					pCur = pCur->_pLeft;
				}
				else
					return fbcstd::pair<Iterator, bool>(Iterator(pCur), false);
			}
			Node* a = (Node*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Node));
			a->_value.first = value.first;
			a->_value.second = value.second;
			a->_color = RED;
			newNode = pCur = a;
			if (value.first < pParent->_value.first)
				pParent->_pLeft = pCur;
			else
				pParent->_pRight = pCur;
			pCur->_pParent = pParent;
			while (pParent && pParent->_color == RED) {
				PNode grandParent = pParent->_pParent;
				if (pParent == grandParent->_pLeft) {
					PNode pUncle = grandParent->_pRight;
					if (pUncle && pUncle->_color == RED) {
						pParent->_color = BLACK;
						pUncle->_color = BLACK;
						grandParent->_color = RED;
						grandParent = pCur;
						pParent = pCur->_pParent;
					}
					else {
						if (pCur == pParent->_pRight) {
							rotateL(pParent);
							PNode t = pCur;
							pCur = pParent;
							pParent = t;
						}
						grandParent->_color = RED;
						pParent->_color = BLACK;
						rotateR(grandParent);
					}
				}
				else {
					PNode pUncle = grandParent->_pLeft;
					if (pUncle && pUncle->_color == RED) {
						pParent->_color = BLACK;
						pUncle->_color = BLACK;
						grandParent->_color = RED;
						grandParent = pCur;
						pParent = pCur->_pParent;
					}
					else {
						if (pCur == pParent->_pLeft) {
							rotateR(pParent);
							PNode t = pCur;
							pCur = pParent;
							pParent = t;
							//fbcstd::swap(pCur, pParent);
						}
						grandParent->_color = RED;
						pParent->_color = BLACK;
						rotateL(grandParent);
					}
				}
			}
		}

		_pRoot->_color = BLACK;
		_pHead->_pLeft = MostLeft();
		_pHead->_pRight = MostRight();
		fbcstd::pair<Iterator, bool> a;
		a.first = Iterator(newNode);
		a.second = true;
		return a;
	}

	bool Empty()const
	{
		return NULL == GetRoot();
	}

	size_t Size()const
	{
		size_t count = 0;
		Iterator it = Iterator(_pHead->_pLeft);
		Iterator ed = Iterator(_pHead);
		while (it != ed) {
			++count;
			++it;
		}
		return count;
	}

	void InOrder()
	{
		_InOrder(GetRoot());
	}
	bool IsRBTree()
	{
		PNode& _pRoot = GetRoot();
		if (NULL == _pRoot)
			return true;
		if (RED == _pRoot->_color) {
			return false;
		}
		// 统计单条路径中黑色结点的个数
		size_t blackCount = 0;
		PNode pCur = _pRoot;
		while (pCur) {
			if (BLACK == pCur->_color)
				++blackCount;
			pCur = pCur->_pLeft;
		}
		size_t pathCount = 0;
		return _IsRBTree(_pRoot, pathCount, blackCount);
	}
private:
	void rotateL(PNode pParent)
	{
		PNode pSubR = pParent->_pRight;
		PNode pSubRL = pSubR->_pLeft;
		pParent->_pRight = pSubRL;
		if (pSubRL)
			pSubRL->_pParent = pParent;
		pSubR->_pLeft = pParent;
		PNode pPParent = pParent->_pParent;
		pParent->_pParent = pSubR;
		pSubR->_pParent = pPParent;
		if (_pHead == pPParent) {
			GetRoot() = pSubR;
		}
		else {
			if (pPParent->_pLeft == pParent)
				pPParent->_pLeft = pSubR;
			else
				pPParent->_pRight = pSubR;
		}
	}
	void rotateR(PNode pParent)
	{
		PNode pSubL = pParent->_pLeft;
		PNode pSubLR = pSubL->_pRight;
		pParent->_pLeft = pSubLR;
		if (pSubLR)
			pSubLR->_pParent = pParent;
		pSubL->_pRight = pParent;
		PNode pPParent = pParent->_pParent;
		pParent->_pParent = pSubL;
		pSubL->_pParent = pPParent;
		if (_pHead == pPParent) {
			GetRoot() = pSubL;
		}
		else {
			if (pPParent->_pLeft == pParent)
				pPParent->_pLeft = pSubL;
			else
				pPParent->_pRight = pSubL;
		}
	}
	void _InOrder(PNode pRoot)
	{
		if (pRoot) {
			_InOrder(pRoot->_pLeft);
			_InOrder(pRoot->_pRight);
		}
	}

	bool _IsRBTree(PNode pRoot, size_t n, size_t blackCount)
	{
		if (NULL == pRoot)
			return true;
		if (BLACK == pRoot->_color)
			++n;
		PNode pParent = pRoot->_pParent;
		if (pParent && RED == pRoot->_color && RED == pParent->_color) {
			return false;
		}
		if (NULL == pRoot->_pLeft && NULL == pRoot->_pRight) {
			if (n != blackCount) {
				return false;
			}
		}
		return _IsRBTree(pRoot->_pLeft, n, blackCount) && _IsRBTree(pRoot->_pRight, n, blackCount);
	}
	PNode MostLeft()
	{
		PNode pCur = GetRoot();
		if (NULL == pCur)
			return NULL;
		while (pCur->_pLeft)
			pCur = pCur->_pLeft;
		return pCur;
	}
	PNode MostRight()
	{
		PNode pCur = GetRoot();
		if (NULL == pCur)
			return NULL;
		while (pCur->_pRight)
			pCur = pCur->_pRight;
		return pCur;
	}
	PNode _pHead;
};

template<class K, class V>

class Map {
public:
	typedef fbcstd::pair<K, V> valueType;
	typename typedef RBTree<K, V>::Iterator Iterator;
public:

	fbcstd::pair<Iterator, bool> Insert(const valueType& v)
	{
		return _t.InsertUnique(v);
	}

	bool Empty()const
	{
		return _t.Empty();
	}

	size_t Size()const
	{
		return _t.Size();
	}

	V& operator[](const K& key)
	{
		Iterator ret = _t.InsertUnique(fbcstd::pair<K, V>(key, V())).first;
		return (*ret).second;
	}

	Iterator Begin()
	{
		return _t.Begin();
	}

	Iterator End()
	{
		return _t.End();
	}

	RBTree<K, V> _t;
};