#include<iostream>
#include<vector>
using namespace std;
int system(string _cmd)
{
	return system(_cmd.c_str());
}
int system(vector<string> _cmd)
{
	string cmd;
	for(int i=0;i<(int)_cmd.size();i++)
	{
		cmd+=_cmd[i];
		if(i!=(int)_cmd.size()-1) cmd+=" && ";
	}
	return system(cmd);
}
int main(int argc,char **argv)
{
	if(argc<=1)
	{
		cerr<<"invalid input"<<endl;
		return 1;
	}

	string filename=argv[1];
	system("title "+filename);

	string workpath=filename;
	
	while(workpath.back()!='/'&&workpath.back()!='\\') workpath.pop_back();

	int ret=system({"cd \""+workpath+"\"","\""+filename+"\""});

	cerr<<endl<<"--------------------------------"<<endl;
	cerr<<"Process exited after "<<1.*clock()/CLOCKS_PER_SEC<<" seconds with return value "<<ret<<"."<<endl<<endl;

	system("pause");

	return 0;
}