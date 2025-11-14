/*
<ex>
[tc] tc name
[in]
inputfile
[out]
ansfile
(tcs...)
</ex>
*/

#include<iostream>
#include<fstream>
#include<vector>
#include<filesystem>
#include<ctime>

class chNode;

namespace Extester
{

using namespace std;

pair<bool,string> fileComper(string s1,string s2)
{
	stringstream sin1(s1),sin2(s2);
	string tmp1,tmp2;
	while(1)
	{
		int flag=0;
		if(!(sin1>>tmp1)) flag|=2;
		if(!(sin2>>tmp2)) flag|=1;
		// cout<<"(debug:fileComper) "<<tmp1<<" "<<tmp2<<" "<<flag<<endl;
		if(tmp1!=tmp2)
		{
			return {1,""};
		}
		if(flag==3) break;
	}
	return {0,""};
}

string readFile(string file)
{
	string str;
	ifstream fin(file);
	char ch;
	while((ch=fin.get())!=EOF) str+=ch;
	return str;
}

struct __Temp_file_dir_init
{
	__Temp_file_dir_init()
	{
		filesystem::create_directory("TempFile");
	}
}__temp_file_dir_init;

struct Text
{
	bool useFile;
	string text;
	Text()
	{
		useFile=0;
	}
};

void writeText(string filename,Text text)
{
	if(text.useFile)
	{
		// i miss her
	}
	else
	{
		ofstream fout(filename);
		fout<<text.text<<endl;
	}
}

struct TestCase
{
	string name;
	Text in,ans;
};

// <tag>
string getTag(string &s,vector<vector<chNode>> &_mp,int line)
{
	if(s[_mp[line].front().id]!='<') return "";
	string opt;
	auto it=next(_mp[line].begin());
	while(it->id<(int)s.size()&&s[it->id]!='>'&&s[it->id]!='\n') opt+=s[it->id],it++;
	return opt;
}

// [mark] val
// @[mark,endpos,val]
tuple<string,int,string> getMark(string &s,int pos,int en)
{
	string key,val;
	while(pos<en&&s[pos]!='[') pos++;
	if(pos==en) return {"",pos,""};
	// s[pos]=='[' -> true
	pos++;
	while(pos<en&&s[pos]!=']'&&s[pos]!='\n') key+=s[pos],pos++;
	if(pos==en) return {key,en,""};
	while(pos<en&&(s[pos]==']'||s[pos]==' '||s[pos]=='\n')) pos++;
	while(pos<en&&s[pos]!='[') val+=s[pos],pos++;
	return {key,pos,val};
}

vector<TestCase> getTests(string s,vector<vector<chNode>> _mp)
{
	int line=1;
	
	for(;line<(int)_mp.size();line++) if(getTag(s,_mp,line)=="ex") break;
	if(line==(int)_mp.size()) return {};
	
	int pos=_mp[++line].front().id,En=(int)s.size();
	for(;line<(int)_mp.size();line++) if(getTag(s,_mp,line)=="/ex") break;

	if(line!=(int)_mp.size()) En=_mp[line].front().id;

	vector<TestCase> tests;
	while(pos<En)
	{
		auto [key,en,val]=getMark(s,pos,En);
		if(key=="tc")
		{
			while(val.size()&&(val.back()=='\n'||val.back()==' '||val.back()=='\t')) val.pop_back();
			tests.push_back(TestCase());
			tests.rbegin()->name=val;
		}
		else if(key=="in")
		{
			if(tests.size())
			{
				tests.rbegin()->in.text=val;
			}
		}
		else if(key=="out")
		{
			if(tests.size())
			{
				tests.rbegin()->ans.text=val;
			}
		}
		pos=en;
	}

	// cout<<"(debug) ExTesterMain"<<endl;
	
	// for(auto test:tests)
	// {
	// 	cout<<"name:"<<test.name<<endl;
	// 	cout<<"input:"<<endl<<test.in.text<<endl;
	// 	cout<<"output:"<<endl<<test.ans.text<<endl;
	// }

	return tests;
}

int main(string filename,string exename,string s,vector<vector<chNode>> _mp)
{
	cout<<"--- Example Tester ---"<<endl;

	vector<TestCase> tests=getTests(s,_mp);
	int acCnt=0;

	for(auto test:tests)
	{
		// cout<<"(debug)running"<<endl;
		writeText("TempFile/temp.in",test.in);
		string cmd="\""+exename+"\" <TempFile/temp.in >TempFile/temp.out";
		// cout<<"(debug) cmd="<<cmd<<endl;
		system(cmd.c_str());

		string output=readFile("TempFile/temp.out");
		// cout<<"(debug) "<<output<<" "<<test.ans.text<<endl;
		cout<<test.name<<":";
		if(fileComper(output,test.ans.text).first)
		{
			cout<<"WA"<<endl;
		}
		else
		{
			cout<<"AC"<<endl;
			acCnt++;
		}
	}
	cout<<acCnt<<"/"<<tests.size()<<endl<<endl;

	cout<<"-------------------------"<<endl;
	return 0;
}

} // namespace Extester

/*
g++.exe LemonCpp.cpp -o LemonCpp.exe -std=c++20 -fsyntax-only
*/