#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>

#include<graphics.h>

using namespace std;

struct __config
{
	int bracketAC; // bracket auto completion
	int homeTabEnd; // home to tab end
	string compileOp; // compile options
	int lineNum; // line number
	int useColor; // color text
	int showIC; // show invisible chars
	int saveTime; // save history text length (s)
	int useMouse;
	string workPath; // path of workspace
	color_t bgColor;
	void _read_conf(const string filename="LemonCpp.config")
	{
		ifstream fin(filename);
		if(!fin.good())
		{
			cout<<"didn't find \"LemonCpp.config\""<<endl;
			return;
		}
		string str;
		while(fin>>str)
		{
			if(str=="//") getline(fin,str);
			else if(str=="bracketAC") fin>>bracketAC;
			else if(str=="homeTabEnd") fin>>homeTabEnd;
			else if(str=="compileOp") getline(fin,compileOp);
			else if(str=="lineNum") fin>>hex>>lineNum;
			else if(str=="useColor") fin>>useColor;
			else if(str=="showIC") fin>>showIC;
			else if(str=="saveTime") fin>>saveTime;
			else if(str=="useMouse") fin>>useMouse;
			else if(str=="workPath")
			{
				getline(fin,workPath);
				while(workPath.size()&&workPath.front()==' ') workPath.erase(workPath.begin());
			}
			else if(str=="bgColor") fin>>hex>>bgColor;
		}
	}
	__config()
	{
		bracketAC=0;
		homeTabEnd=0;
		compileOp="";
		lineNum=YELLOW;
		useColor=0;
		showIC=0;
		saveTime=60;
		useMouse=0;
		workPath="";
		bgColor=BLACK;
		_read_conf();
	}
};
struct __color_config
{
	color_t number; // in-code number
	color_t keyword; // keyword
	color_t precompile; // pre-compile sentences
	color_t symbol; // symbol
	color_t comment; // comment
	color_t cstring; // const string and char
	color_t invischar; // invisible char
	color_t otherwise;
	void _read_color_conf(const string filename="LemonCppColor.config")
	{
		ifstream fin(filename);
		string str;
		while(fin>>str)
		{
			if(str=="//") getline(fin,str);
			else if(str=="number") fin>>hex>>number;
			else if(str=="keyword") fin>>hex>>keyword;
			else if(str=="precompile") fin>>hex>>precompile;
			else if(str=="symbol") fin>>hex>>symbol;
			else if(str=="comment") fin>>hex>>comment;
			else if(str=="cstring") fin>>hex>>cstring;
			else if(str=="invischar") fin>>hex>>invischar;
			else if(str=="otherwise") fin>>hex>>otherwise;
		}
	}
	__color_config()
	{
		number=WHITE;
		keyword=WHITE;
		precompile=WHITE;
		otherwise=WHITE;
		symbol=WHITE;
		comment=WHITE;
		cstring=WHITE;
		invischar=EGERGBA(255,255,255,191);
		_read_color_conf();
	}
};