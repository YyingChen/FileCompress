#pragma once

#include "Heap.h"

template<class W>
struct HuffmanTreeNode
{
    HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;
	W _w;//权值

	HuffmanTreeNode(const W&w)
		:_w(w)
		,_left(NULL)
		,_right(NULL)
		,_parent(NULL)

	{}
};
template<class W>
class HuffmanTree
{
    typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}
	HuffmanTree(W*a,size_t n,const W& invaild)//给一组数组，构建哈夫曼树
	{

		struct NodeCompare//内部类 //用结点里面的权值构建堆
		{
			//重载operater()
			bool operator()(const Node* left,const Node* right) const
			{
				return left->_w<right->_w;//charinfo没有小于所以要重载operator<
			}
		};
        Heap<Node*,NodeCompare> minHeap;//堆里要放结点不放权值
		
		for(size_t i=0;i<n;++i)
		{
			if (a[i]!=invaild)//a[i]的类型是charinfo，自定义类型没有!=，要重载//invaild代表的是count=0
			{
				minHeap.Push(new Node(a[i]));
			}
			
		}
     
		while(minHeap.Size()>1)
		{
			Node* left=minHeap.Top();
			minHeap.Pop();
			Node* right=minHeap.Top();
			minHeap.Pop();

			Node* parent=new Node(left->_w+right->_w);//charinfo不能进行相加
			parent->_left=left;
			parent->_right=right;
			left->_parent=parent;
			right->_parent=parent;


			minHeap.Push(parent);
		}

		_root=minHeap.Top();
		

	}
	Node* GetRoot()
	{
		return _root;
	}
protected:
	Node* _root;
};


