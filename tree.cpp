
#include  <string>
#include <iostream>
#include  <memory>

using namespace std;

struct Node
{
	string  cont;
	string  pos;
	int  tone;
	shared_ptr<Node>  child;

};


unique_ptr<Node> make_node()
{
	return  unique_ptr<Node>(new Node());
}


int main()
{
	 Node n;
	 n.cont="hello";

	 n.child=make_node();
	
	 cout<<n.child->cont<<endl;
	return 0;
}

