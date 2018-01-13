#pragma once
#include <iostream>
#include <vector>
using namespace std;

template<class T>
struct Less //实现小堆，父节点小于子节点
{ 
	bool operator()(const T& l, const T& r) 
	{ 
		return l < r; 
	} 
}; 

template<class T> //实现大堆，父节点大于子节点
struct Greater 
{ 
	bool operator()(const T& l, const T& r) 
	{ 
		return l > r; 
	} 
}; 

template<class T,class Compare>
class Heap 
{ 
public:
	Heap()
	{}
	Heap(T* a, size_t n)
	{
       _a.reserve(n);//reserve与resize的区别？
	   for(size_t i=0; i<n; i++)
	   {
		 _a.push_back(a[i]); 
	   }
	   for(int i=(_a.size()-2)/2;i>=0;--i)
	   {
		   _AdjustDown(i);
	   }
	}
	void Push(const T& x) 
	{
       _a.push_back(x);
	   _AdjustUp(_a.size()-1);
	   //_AdjustUp(0); 
	}
	void Pop() 
	{
       assert(!_a.empty());
	   swap(_a[0],_a[_a.size()-1]);
	   _a.pop_back();
	   _AdjustDown(0);
	}
	const T& Top() 
	{//
        return _a[0];
	}
	void Display()
	{
		for(size_t i=0;i<_a.size();i++)
		{
			cout<<_a[i]<<" ";
		}
		cout<<endl;
	}
	size_t Size()
	{
		return _a.size();
	}


protected: 
	void _AdjustUp(int child)
	{
		Compare ptr;
		int parent=(child-1)/2;
		while(child>=0)
		{
			if(ptr(_a[child],_a[parent]))
			{
				swap(_a[parent],_a[child]);
				child=parent;
				parent=(child-1)/2;
			}
			else
			{
				break;
			}
		}

	}

	void _AdjustDown(int root) //从根往下开始调整
	{
		Compare ptr;
         int parent=root;
		 int child=parent*2+1;
		 while(child <(int)_a.size())
		 {
			 if((child+1<(int)_a.size())&&(ptr(_a[child+1],_a[child])))
			 {
				 ++child;
			 }
			 if (ptr(_a[child],_a[parent]))
			 {
				 swap(_a[child],_a[parent]);
				 parent=child;
				 child=parent*2+1;//往下走
			 }
			 else
			 {
				 break;
			 }
		 }
    }

	

protected:	
	vector<T>  _a; 
}; 






