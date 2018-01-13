#pragma once
#include <stdio.h>
#include <assert.h>
#include <string>
#include<algorithm>
#include "HuffmanTree.h"

using namespace std;
typedef long long LongType;

struct CharInfo//����Ҫ���ַ��ʹ���
{
	char _ch;//�ַ�,��׼�ַ�128��,��չ֮���ַ�256��<��Щ�ַ�������>
	LongType _count;//���ִ�����size_t ��ʾ4���ֽڣ�42��9ǧ��
    string _code;//����������

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
//�ַ����� unicode �����ַ���ʾһ������ utf-8��utf-16
//gdk
class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
	struct ConfigInfo//��ѹ������
	{
		char _ch;
		size_t _count;
	};

public:
	FileCompress()
	{
		for (size_t i=0;i<256;++i)
		{
			_infos[i]._ch=i;//������char����ʾ��ʱ�������ߣ�0-255 ASCII
			_infos[i]._count=0;
			//codeû�취��ʼ����ֻ�����ɹ��������ſ���
		}
	}
	void Compress(const char*filename)
	{
		assert(filename);
		FILE* fout=fopen(filename,"rb");//�����ƶ�д
		assert(fout);

		//1.ͳ���ַ����ֵĴ���.
		char ch=fgetc(fout);
		//���ļ��ж�ȡ�ַ�ʱ��
		//�����˼ٵ�EOF��־��ĳ�ַ�Ϊ��������ַ���ȴ��EOF������ͬ��λ����
		//����������ļ�����ʱ�����ö����Ʒ�ʽ��д��
		while (!feof(fout))//ch!=EOF
		{
			//�ַ���ֵ�Ƕ��پ�Ӧ����_infos�ĵڼ���
			_infos[(unsigned char)ch]._count++;//�ַ������˼��Σ�λ�þͱ����˼���
			ch=fgetc(fout);
		}

		//2��������������,�ڵ㲻���д�����Ҫ���ַ�
		CharInfo invaild;
		invaild._count=0;
        HuffmanTree<CharInfo> tree(_infos,256,invaild);

		 //3�����ɹ���������(��·���йأ�-������ �Ӹ��ڵ㵹�������� ���õ��ı������þ��ǹ���������
		 // GenerateHuffmanCode(tree.GetRoot());
		 string code;
		 GenerateHuffmanCode(tree.GetRoot(),code);
         //4��ѹ��
		 //��ѹ���ļ�

		 string compressFile=filename;
		 compressFile+=".huffman";
		 FILE* fIn=fopen(compressFile.c_str(),"wb");
		
		 assert(fIn);


		 //д�ַ����ֵĴ�����ѹ���ļ�
		 ConfigInfo info;
		 for (size_t i = 0; i < 256; ++i)
		 {
			 if (_infos[i]._count >0)
			 {
				 //��������ʽд�ַ���

				 info._ch = _infos[i]._ch;
				 info._count = _infos[i]._count;
				 fwrite(&info, sizeof(ConfigInfo), 1, fIn);//

			 }
		 }
		 info._count = 0;//��������һ��0�������ָ�
		 fwrite(&info, sizeof(ConfigInfo), 1, fIn);

		 //ѹ��
		 char value=0;
		 int count=0;
		 fseek(fout,0,SEEK_SET);//���ļ���ʼ
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
        //cout << "ѹ���ַ�����" << count << endl;
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
		  //��Ч�����
		  string &code=_infos[(unsigned char)root->_w._ch]._code;

		  if (root->_left==NULL&&root->_right==NULL)
		  {
			  //����code
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
			  reverse(code.begin(),code.end());//reverse������Ҫ���õĵ�����
			  return;
		  }
		  GenerateHuffmanCode(root->_left);
		  GenerateHuffmanCode(root->_right);

		  //_infos[root->_w._ch]._code=code;//�ַ���Ӧ�ı���

	}
    ////���ڵ㵽��ǰλ�õı���,û����һ�������ã���������string
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

		GenerateHuffmanCode(root->_left,code+'0');//�ݹ����ɱ���
		GenerateHuffmanCode(root->_right,code+'1');
	}
	
	void Uncompress(const char*filename)
	{
        assert(filename);
        //ȥ��׺
		string uncompressFile=filename;
		size_t pos=uncompressFile.rfind(".");//�����Һ�׺
        assert(pos!=string::npos);

		uncompressFile = uncompressFile.substr(0,pos);//��ȡ�Ӵ�
		uncompressFile+=".unhuffman";

		FILE*fout = fopen(filename, "rb");
		assert(fout);
	    FILE* fin=fopen(uncompressFile.c_str(),"wb");
        //string.c_str��Borland��װ��String���е�һ�������������ص�ǰ�ַ��������ַ���ַ��
		assert(fin);
	
		//�ȶ���������Ϣ---�ַ����ֵĴ���
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

		//�ؽ�huffman tree
		CharInfo invaild;
		invaild._count=0;
		HuffmanTree<CharInfo> tree(_infos,256,invaild);
		Node* root=tree.GetRoot();
		LongType charCount=root->_w._count;
		
        
		//��ѹ��
		
		char value=fgetc(fout);
		Node* cur=root;
		while (!feof(fout))
		{
          for (int pos=7;pos>=0;--pos)
		  {
              if (value &(1<<pos))//�����1
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
		//cout << "��ѹ���ַ�����" << count << endl;
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