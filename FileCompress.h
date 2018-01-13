#pragma once
#include <stdio.h>
#include <assert.h>
#include <string>
#include<algorithm>
#include "HuffmanTree.h"

using namespace std;
typedef long long LongType;

struct CharInfo//树中要放字符和次数
{
	char _ch;//字符,标准字符128个,扩展之后字符256个<有些字符看不见>
	LongType _count;//出现次数，size_t 表示4个字节，42亿9千万
    string _code;//哈夫曼编码

	bool operator !=(const CharInfo&info)const
	{
		return _count!=info._count;
	}
	CharInfo operator+(const CharInfo&info)const
	{
		CharInfo ret;
		ret._count=_count+info._count;
		return ret;
	}
	bool operator <(const CharInfo&info)const
	{
		return _count<info._count;
	}
	
};
//字符编码 unicode 两个字符表示一个汉字 utf-8和utf-16
//gdk
class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
	struct ConfigInfo//解压缩配置
	{
		char _ch;
		size_t _count;
	};

public:
	FileCompress()
	{
		for (size_t i=0;i<256;++i)
		{
			_infos[i]._ch=i;//类型是char，显示的时候按类型走，0-255 ASCII
			_infos[i]._count=0;
			//code没办法初始化，只有生成哈夫曼树才可以
		}
	}
	void Compress(const char*filename)
	{
		assert(filename);
		FILE* fout=fopen(filename,"rb");//二进制读写
		assert(fout);

		//1.统计字符出现的次数.
		char ch=fgetc(fout);
		//从文件中读取字符时，
		//遇到了假的EOF标志（某字符为有意义的字符，却和EOF有着相同的位）。
		//解决方法：文件操作时均采用二进制方式读写。
		while (!feof(fout))//ch!=EOF
		{
			//字符的值是多少就应该在_infos的第几个
			_infos[(unsigned char)ch]._count++;//字符出现了几次，位置就被加了几次
			ch=fgetc(fout);
		}

		//2、构建哈夫曼树,节点不仅有次数还要有字符
		CharInfo invaild;
		invaild._count=0;
        HuffmanTree<CharInfo> tree(_infos,256,invaild);

		 //3、生成哈夫曼编码(跟路径有关）-三叉链 从根节点倒着往上走 最后得到的编码逆置就是哈夫曼编码
		 // GenerateHuffmanCode(tree.GetRoot());
		 string code;
		 GenerateHuffmanCode(tree.GetRoot(),code);
         //4、压缩
		 //打开压缩文件

		 string compressFile=filename;
		 compressFile+=".huffman";
		 FILE* fIn=fopen(compressFile.c_str(),"wb");
		
		 assert(fIn);


		 //写字符出现的次数到压缩文件
		 ConfigInfo info;
		 for (size_t i = 0; i < 256; ++i)
		 {
			 if (_infos[i]._count >0)
			 {
				 //二进制形式写字符串

				 info._ch = _infos[i]._ch;
				 info._count = _infos[i]._count;
				 fwrite(&info, sizeof(ConfigInfo), 1, fIn);//

			 }
		 }
		 info._count = 0;//在最后加上一个0，用来分隔
		 fwrite(&info, sizeof(ConfigInfo), 1, fIn);

		 //压缩
		 char value=0;
		 int count=0;
		 fseek(fout,0,SEEK_SET);//从文件开始
		 ch=fgetc(fout);
		
		 while (!feof(fout))
		 {
            string code=_infos[(unsigned char)ch]._code;
			for (size_t i=0;i<code.size();++i)
			{
				value<<=1;
				
				if (code[i]=='1')
				{
					value|=1;
				}
				else//'0'
				{
                     value|=0;  
				}
				++count;
				if (count==8)
				{
                  fputc(value,fIn);
				  value=0;
				  count=0;
				}
			}
			ch=fgetc(fout);
		 }
        //cout << "压缩字符数：" << count << endl;
		 if (count!=0)
		 {
             value<<=(8-count);
			 fputc(value,fIn);
		 }

		 fclose(fIn);
		 fclose(fout);
		 

	}

	void GenerateHuffmanCode(Node* root)
	{
          if (root==NULL)
          {
			  return;
          }
		 // string code;
		  //高效，简洁
		  string &code=_infos[(unsigned char)root->_w._ch]._code;

		  if (root->_left==NULL&&root->_right==NULL)
		  {
			  //生成code
			  Node*cur =root;
			  Node* parent=cur->_parent;
			  while (parent)
			  {
				  if (parent->_left==cur)
				  {
					  code.push_back('0');
				  }
				  else
				  {
					  code.push_back('1');
				  }
				  cur=parent;
				  parent=cur->_parent;
			  }
			  reverse(code.begin(),code.end());//reverse传的是要逆置的迭代器
			  return;
		  }
		  GenerateHuffmanCode(root->_left);
		  GenerateHuffmanCode(root->_right);

		  //_infos[root->_w._ch]._code=code;//字符对应的编码

	}
    ////根节点到当前位置的编码,没有上一个方法好，不用生成string
	void GenerateHuffmanCode(Node* root,string code)
	{
		if (root==NULL)
		{
			return;
		}

		if(root->_left==NULL&&root->_right==NULL)
		{
			_infos[(unsigned char)root->_w._ch]._code=code;
			return;
		}

		GenerateHuffmanCode(root->_left,code+'0');//递归生成编码
		GenerateHuffmanCode(root->_right,code+'1');
	}
	
	void Uncompress(const char*filename)
	{
        assert(filename);
        //去后缀
		string uncompressFile=filename;
		size_t pos=uncompressFile.rfind(".");//倒着找后缀
        assert(pos!=string::npos);

		uncompressFile = uncompressFile.substr(0,pos);//获取子串
		uncompressFile+=".unhuffman";

		FILE*fout = fopen(filename, "rb");
		assert(fout);
	    FILE* fin=fopen(uncompressFile.c_str(),"wb");
        //string.c_str是Borland封装的String类中的一个函数，它返回当前字符串的首字符地址。
		assert(fin);
	
		//先读入配置信息---字符出现的次数
		ConfigInfo info;
		while (1)
		{
			fread(&info, sizeof(ConfigInfo), 1, fout);
			if (info._count == 0)
			{
				break;
			}
			else
			{
				_infos[(unsigned char)info._ch]._ch = info._ch;
				_infos[(unsigned char)info._ch]._count= info._count;
			}
		}

		//重建huffman tree
		CharInfo invaild;
		invaild._count=0;
		HuffmanTree<CharInfo> tree(_infos,256,invaild);
		Node* root=tree.GetRoot();
		LongType charCount=root->_w._count;
		
        
		//解压缩
		
		char value=fgetc(fout);
		Node* cur=root;
		while (!feof(fout))
		{
          for (int pos=7;pos>=0;--pos)
		  {
              if (value &(1<<pos))//结果是1
              {
                  cur=cur->_right;
              }
			  else//0
			  {
                  cur=cur->_left;
			  }
			  if(cur->_left==NULL&&cur->_right==NULL)
			  {
				  fputc(cur->_w._ch,fin);
				  cur=root;
				  --charCount;
				  if (charCount==0)
				  {
					  break;
				  }
			  }
          }
		  value=fgetc(fout);
		}
		fclose(fin);
		fclose(fout);
		//cout << "解压缩字符数：" << count << endl;
	}
protected:
	CharInfo _infos[256];
};

void TestFileCompress()
{
	
	FileCompress fc1;
	fc1.Compress("if.jpg");
	FileCompress fc2;
	fc2.Uncompress("if.jpg.huffman");
}