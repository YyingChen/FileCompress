#pragma once

#include "Heap.h"

template<class W>
struct HuffmanTreeNode
{
    HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;
	W _w;//Ȩֵ

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
	HuffmanTree(W*a,size_t n,const W& invaild)//��һ�����飬������������
	{

		struct NodeCompare//�ڲ��� //�ý�������Ȩֵ������
		{
			//����operater()
			bool operator()(const Node* left,const Node* right) const
			{
				return left->_w<right->_w;//charinfoû��С������Ҫ����operator<
			}
		};
        Heap<Node*,NodeCompare> minHeap;//����Ҫ�Ž�㲻��Ȩֵ
		
		for(size_t i=0;i<n;++i)
		{
			if (a[i]!=invaild)//a[i]��������charinfo���Զ�������û��!=��Ҫ����//invaild�������count=0
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

			Node* parent=new Node(left->_w+right->_w);//charinfo���ܽ������
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


