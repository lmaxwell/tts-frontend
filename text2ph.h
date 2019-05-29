
#include <cstring>
#include "../libscws/scws.h"
#include <cstdlib>
#define SCWS_PREFIX     "/usr/local/scws"
#include <vector>
#include <string>
#include <map>
using namespace std;
typedef struct _UttTreeNode
{
	char *content;
	char pos[15];
	char tone[2];
	struct _UttTreeNode *child;
	struct _UttTreeNode *parent;
	struct _UttTreeNode *previous;
	struct _UttTreeNode *next;
}UttTreeNode;

typedef struct _UttTree
{
	UttTreeNode *root;
}UttTree;

typedef struct _WordList
{
	UttTreeNode *head;
}WordList;

typedef struct _PinyinList
{
	UttTreeNode *head;
}PinyinList;

typedef struct _PhoneList
{
	UttTreeNode *head;
}PhoneList;

UttTreeNode* newNode();
UttTreeNode* buildTree(UttTreeNode *root,char *inText,map<string,vector<string> > pyDict,map<string,vector<string> > phDict,PhoneList &phoneList,PinyinList &pinyinList,WordList &wordList);
int loadPyDict(const char* filePath,map<string,vector<string> > &dict);
int loadPhDict(string file_path,map<string,vector<string> > &dict);
void getFullLabel(PhoneList phoneList,vector<string> &vLabel);
void getPinyins(PinyinList pinyinList,char *pinyins);
void getWords(WordList wordList,char *words);


