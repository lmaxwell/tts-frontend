/*
 * =====================================================================================
 *
 *       Filename:  text2ph.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年05月12日 11时39分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Xian, 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "text2ph.h"
#include <stdio.h>
#include <fstream>
#include <sstream>

#include<iostream>
using namespace std;
int loadPyDict(const char* filePath,map<string,vector<string> > &dict)
{
	ifstream infile(filePath);
	string s;

	while(getline(infile,s))
	{
		stringstream ss(s);
		string character;
		vector<string> pinyin;
		string buf;
		ss>>character;
		while(ss>>buf)
		{
			pinyin.push_back(buf);
		}
		//cout<<character<<" "<<pinyin<<endl;
		dict[character]=pinyin;
	}
	
}
int loadPhDict(string file_path,map<string,vector<string> > &dict)
{
	ifstream infile(file_path.c_str());
	string s;

	while(getline(infile,s))
	{
		stringstream ss(s);
		string py;
		vector<string> phs;
		string buf;
		ss>>py;
	//	cout<<py<<" ";
		while(ss>>buf)
		{
			phs.push_back(buf);
	//		cout<<buf<<" ";
		}
//		cout<<endl;
		dict[py]=phs;
	}
	return 0;
}

int seg2py(string str,vector<string> &py,map<string,vector<string> > dict)
{
	if(str == "," || str =="." || str =="，" ||str=="。" ||str==" ")
			{
				//cout<<"sp ";	
				py.push_back("sp");
				return 0;
			}
	if(dict.find(str)!=dict.end())
			{
			//	cout<<dict[str];
				py.assign(dict[str].begin(),dict[str].end());			
			}
			else
			{
				for(int i=0;i<str.length();i+=3)
				{

				string character=str.substr(i,3);
				if(dict.find(character)!=dict.end())
				{
			//		cout<<dict[character];
					py.insert(py.end(),dict[character].begin(),dict[character].end());
				}
				else
				{
				//	cout<<"unknown";
					py.push_back("unknown");
				}
				}

			}
			return 0;
}
int py2phone(string py,vector<string> &phs,map<string,vector<string> > dict)
{
	if(dict.find(py)!=dict.end())
	{
			phs.assign(dict[py].begin(),dict[py].end());	
	}
	else
	{
		phs.push_back(py);
	}

	return 0;

}

UttTreeNode* newNode()
{
	UttTreeNode *p=new UttTreeNode;
	p->content=NULL;
	p->child=NULL;
	p->parent=NULL;
	p->previous=NULL;
	p->next=NULL;
	return p;
}

UttTreeNode* buildTree(UttTreeNode *root,char *inText,map<string,vector<string> > pyDict,map<string,vector<string> > phDict,PhoneList &phoneList,PinyinList &pinyinList,WordList &wordList) 
{
	//输入文本
	//root->content=new char[strlen(inText)+1];
	//strcpy(root->content,inText);
	char *text=inText;
	//scws 分词&词性
	scws_t s;
  	scws_res_t res, cur;
  	if (!(s = scws_new())) {
    printf("ERROR: cann't init the scws!\n");
    exit(-1);
  }
  scws_set_charset(s, "utf8");
  scws_set_dict(s, "/usr/local/scws/etc/dict.utf8.xdb", SCWS_XDICT_XDB);
  scws_set_rule(s, "/usr/local/scws/etc/rules.utf8.ini");
  scws_send_text(s, text, strlen(text));
	//WordList wordList;
	wordList.head=newNode();
	root->child=wordList.head;
	wordList.head->parent=root;
	UttTreeNode *pWord=wordList.head;
	char temp[100];
  while (res = cur = scws_get_result(s))
  {
    while (cur != NULL)
    {
     // printf("WORD: %.*s/%s (IDF = %4.2f)\n", cur->len, text+cur->off, cur->attr, cur->idf);
	  strncpy(temp,text+cur->off,cur->len);
	  temp[cur->len]=0;
	 pWord->next=newNode();
	  pWord->next->previous=pWord;
	  pWord=pWord->next;
	  pWord->content=new char[strlen(temp)+1];
	  strcpy(pWord->content,temp);
	  strcpy(pWord->pos,cur->attr);
	  cur = cur->next;
    }
    scws_free_result(res);
  }
  
//  pWord=wordList.head;
//  while(pWord->next!=NULL)
//  {
//	pWord=pWord->next;
//  	printf("%s\\%s\n",pWord->content,pWord->pos);
//  }

  //拼音
 pWord=wordList.head;
// PinyinList pinyinList;
 pinyinList.head=newNode();
 UttTreeNode *pPinyin=pinyinList.head;

 vector<string> pys;
 string word;
 int ipy;
 while(pWord->next!=NULL)
 {
 	pWord=pWord->next;
	word=pWord->content;

	seg2py(word,pys,pyDict);

	for(ipy=0;ipy<pys.size();ipy++)
	{
		
		pPinyin->next=newNode();
		pPinyin->next->previous=pPinyin;
		pPinyin=pPinyin->next;
		pPinyin->content=new char[pys[ipy].length()];
		if(pys[ipy]=="sp")
		{
			strcpy(pPinyin->content,pys[ipy].c_str());
			strcpy(pPinyin->tone,"XX");
		}
		else
		{
		
		strncpy(pPinyin->content,pys[ipy].c_str(),pys[ipy].length()-1);
		pPinyin->content[pys[ipy].length()-1]=0;
		strcpy(pPinyin->tone,pys[ipy].substr(pys[ipy].length()-1,1).c_str());
		}
		pPinyin->parent=pWord;
		if(ipy==0)
			pWord->child=pPinyin;
	}
	pys.clear();

 }
// pPinyin=pinyinList.head;
//  while(pPinyin->next!=NULL)
//  {
//	pPinyin=pPinyin->next;
//  	printf("%s %s\n",pPinyin->content,pPinyin->tone);
//  }

  //音素
  //PhoneList phoneList;
  phoneList.head=newNode();
  UttTreeNode *pPhone=phoneList.head;

	pPinyin=pinyinList.head;
	string pinyin;
	vector<string> phs;
	int iph;
	while(pPinyin->next!=NULL)
	{
		pPinyin=pPinyin->next;
		pinyin=pPinyin->content;
		py2phone(pinyin,phs,phDict);
		for(iph=0;iph<phs.size();iph++)
		{
			pPhone->next=newNode();
			pPhone->next->previous=pPhone;
			pPhone=pPhone->next;
			pPhone->content=new char[phs[iph].length()+1];
			strncpy(pPhone->content,phs[iph].c_str(),phs[iph].length());
			pPhone->content[phs[iph].length()]=0;
			pPhone->parent=pPinyin;
			if(iph==0)
				pPinyin->child=pPhone;
			

		}
	}
// pPhone=phoneList.head;
//  while(pPhone->next!=NULL)
//  {
//	pPhone=pPhone->next;
//  	printf("%s \n",pPhone->content);
//  }

  return root;
}

void getFullLabel(PhoneList phoneList,vector<string> &vLabel)
{
	UttTreeNode *pPhone=phoneList.head;
	UttTreeNode *pPinyin;
	UttTreeNode *pWord;
	string label;
	label="XX-sil+";
	label+=pPhone->next->content;
	vLabel.push_back(label);
	while(pPhone->next!=NULL)
	{
		pPhone=pPhone->next;
		if(pPhone->previous==phoneList.head)
		{
			label="sil-";
		}
		else 
		{
			label=pPhone->previous->content;
			label+="-";
		}
		label+=pPhone->content;
		if(pPhone->next==NULL)
		{
			label+="+sil";
		}
		else
		{
			label+="+";
			label+=pPhone->next->content;
		}
		
		pPinyin=pPhone->parent;
		if(pPinyin->previous->previous==NULL)
		{
			label+="/A:XX_";
		}
		else 
		{
			label+="/A:";
			label+=pPinyin->previous->tone;
			label+="_";
		}
		label+=pPinyin->tone;
		if(pPinyin->next==NULL)
		{
			label+="@XX";
		}
		else
		{
			label+="@";
			label+=pPinyin->next->tone;
		}
		
		pWord=pPinyin->parent;
		if(pWord->previous->previous==NULL)
		{
			label+="/B:XX_";
		}
		else 
		{
			label+="/B:";
			label+=pWord->previous->pos;
			label+="_";
		}
		label+=pWord->pos;
		if(pWord->next==NULL)
		{
			label+="@XX";
		}
		else
		{
			label+="@";
			label+=pWord->next->pos;
		}


		vLabel.push_back(label);
	}
	label=pPhone->content;
	label+="-sil+XX";
	vLabel.push_back(label);

}

void getPinyins(PinyinList pinyinList,char *pinyins)
{
	UttTreeNode *pPinyin=pinyinList.head;
	while(pPinyin->next!=NULL)
	{
		pPinyin=pPinyin->next;
		strcat(pinyins,pPinyin->content);
		strcat(pinyins,pPinyin->tone);
		strcat(pinyins," ");
	}


}

void getWords(WordList wordList,char *words)
{
	UttTreeNode *pWord=wordList.head;
	while(pWord->next!=NULL)
	{
		pWord=pWord->next;
		strcat(words,pWord->content);
		strcat(words,"\\");
		strcat(words,pWord->pos);
		strcat(words," ");
	}
}
