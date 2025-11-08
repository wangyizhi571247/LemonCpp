// by @wangyizhi571247
// Lemon.Cpp (ver 0.0)

#include<iostream>
#include<iomanip>
#include<stack>
#include<fstream>
#include<vector>
#include<thread>
#include<graphics.h>
using namespace std;
namespace LIB
{
	const string keywords[]={
		"alignas","alignof","and","and_eq","asm",
		"atomic_cancel","atomic_commit","atomic_noexcept","auto","bitand",
		"bitor","bool","break","case","catch",
		"char","char8_t","char16_t","char32_t","class",
		"compl","concept","const","consteval","constexpr",
		"constinit","const_cast","continue","contract_assert","co_await",
		"co_return","co_yield","decltype","default","delete",
		"do","double","dynamic_cast","else","enum",
		"explicit","export","extern","false","float",
		"for","friend","goto","if","inline",
		"int","long","mutable","namespace","new",
		"noexcept","not","not_eq","nullptr","operator",
		"or","or_eq","private","protected","public",
		"reflexpr","register","reinterpret_cast","requires","return",
		"short","signed","sizeof","static","static_assert",
		"static_cast","struct","switch","synchronized","template",
		"this","thread_local","throw","true","try",
		"typedef","typeid","typename","union","unsigned",
		"using","virtual","void","volatile","wchar_t",
		"while","xor","xor_eq"
	};
	bool checkKeyWord(string word)
	{
		for(string _keyword:keywords) if(_keyword==word) return 1;
		return 0;
	}
	bool isword(char ch){return isalpha(ch)||ch=='_';}
	bool issymbol(char ch){return !isdigit(ch)&&!isword(ch);}
}
int pCtrl()
{
	return keystate(key_control_l)||keystate(key_control_r)||keystate(key_control);
}
int pCtrl(int ch)
{
	return pCtrl()&&keystate(ch);
}
void outnumberxy(int x,int y,int num,PIMAGE img)
{
	int st[10],top=0;
	while(num) st[++top]=num%10,num/=10;
	for(int i=top;i>=1;i--) outtextxy(x,y,(char)(st[i]+'0'),img),x+=8;
}
void copyToClipboard(string str)
{
	HWND hWnd = NULL;
	OpenClipboard(hWnd);
	EmptyClipboard();
	HANDLE hHandle = GlobalAlloc(GMEM_FIXED, 100000);
	char* pData = (char*)GlobalLock(hHandle);
	strcpy(pData, str.c_str());
	SetClipboardData(CF_TEXT, hHandle);
	GlobalUnlock(hHandle);
	CloseClipboard();
}
string getClipboard() {
    if (!OpenClipboard(nullptr)) {
        return "";
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return "";
    }

    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr) {
        CloseClipboard();
        return "";
    }

    string text(pszText);

    GlobalUnlock(hData);
    CloseClipboard();

    return text;
}
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
		_read_conf();
	}
}conf;
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
}colorConf;
int p,upline=1,curx,cury;
string s,filename;
pair<string,string> getRealExname()
{
	string exname="",realname="";
	for(int i=(int)filename.size()-1;i>=0;i--)
	{
		if(filename[i]=='.')
		{
			realname=filename.substr(0,i);
			break;
		}
		exname.insert(exname.begin(),filename[i]);
	}

	return {realname,exname};
}
struct __History
{
	string _s;
	int _p,_curx,_cury,_upline;
	void undo()
	{
		s=_s,p=_p,curx=_curx,cury=_cury,upline=_upline;
	}
	void get()
	{
		_s=s,_p=p,_curx=curx,_cury=cury,_upline=upline;
	}
};
__History getNowRecord()
{
	__History record;
	record.get();
	return record;
}
stack<__History> historyVer;
bool hasfilename;
const int delayLength=100;
const int Width=1024,Height=512,chWid=8,chHei=16;
const int leDelta=4*chWid;
struct chNode
{
	int x,id;
	color_t color;
};
vector<vector<chNode>> _mp;
void init()
{
	s="",p=0,curx=0,cury=1,hasfilename=0,upline=1,filename="";
	_mp.clear();
	while(historyVer.size()) historyVer.pop();
}
bool erase_pre()
{
	if(!p) return 0;
	return s.erase(s.begin()+p-1),p--;
}
bool erase_suf()
{
	if(p==(int)s.size()) return 0;
	return s.erase(s.begin()+p),1;
}
int getCurTab()
{
	int cnt=0;
	for(auto ch:_mp[cury])
	{
		int id=ch.id;
		if(s[id]!='\t') break;
		cnt++;
	}
	return cnt;
}
void insert(char ch)
{
	s.insert(s.begin()+p,ch),p++;

	if(conf.bracketAC)
	{
		if(ch=='(') s.insert(s.begin()+p,')');
		else if(ch=='[') s.insert(s.begin()+p,']');
		else if(ch=='{') s.insert(s.begin()+p,'}');
		else if(ch=='\'') s.insert(s.begin()+p,'\'');
		else if(ch=='\"') s.insert(s.begin()+p,'\"');
	}
}
void insert(string str)
{
	s.insert(p,str),p+=(int)str.size();
}
void insert_paste()
{
	string text=getClipboard();
	s.insert(p,text);
}
void move_left(){p?p--:0;}
void move_right(){p!=(int)s.size()?p++:0;}
void moveup(){upline>1?upline--:0;}
void movedown(){upline++;}
void move_end(){p=_mp[cury].back().id;}
void move_home()
{
	p=_mp[cury].front().id;
	if(conf.homeTabEnd)
	{
		p+=getCurTab();
	}
}
void preprint(string &str)
{
	int cnt=0,x=0,y=1;
	curx=0,cury=1;
	_mp.clear();_mp.push_back({}),_mp.push_back({});
	
	int defaultcolor=colorConf.otherwise;
	if(!conf.useColor) defaultcolor=WHITE;

	// print
	for(auto i:str)
	{
		chNode ch;ch.x=x,ch.id=cnt;ch.color=defaultcolor;
		_mp[y].push_back(ch);
		if(i=='\n') y++,x=0,_mp.push_back({});
		else
		{
			if(i=='\t') x=x/4*4+4;
			else x++;
		}
		cnt++;
		if(cnt==p) curx=x,cury=y;
	}
	
	chNode ch;ch.x=x,ch.id=cnt;
	_mp[y].push_back({ch});
	
	if(!conf.useColor) return;

	// color
	auto colorLine=[&](int line,color_t color)->void
	{
		for(auto &ch:_mp[line]) ch.color=color;
	};

	auto getFirstChar=[&](int line)->chNode
	{
		for(auto ch:_mp[line])
		{
			if(s[ch.id]!='\t'&&s[ch.id]!=' ') return ch;
		}
		return {};
	};

	for(int line=1;line<(int)_mp.size();line++)
	{
		// precompile
		if(str[getFirstChar(line).id]=='#')
		{
			colorLine(line,colorConf.precompile);
			while((int)_mp[line].size()>=2&&str[prev(_mp[line].end(),2)->id]=='\\')
			{
				line++;
				colorLine(line,colorConf.precompile);
			}
			continue;
		}

		// symbol
		for(int col=0;col+1<(int)_mp[line].size();col++)
		{
			chNode &nd=_mp[line][col];
			char ch=str[nd.id];
			if(LIB::issymbol(ch))
			{
				nd.color=colorConf.symbol;
			}
		}
		
		// word
		for(int col=0;col+1<(int)_mp[line].size();col++) if(!LIB::issymbol(str[_mp[line][col].id]))
		{
			int begincol=col;
			string curword;curword+=str[_mp[line][col].id];
			while(!LIB::issymbol(str[_mp[line][col+1].id])) col++,curword+=str[_mp[line][col].id];
			if(LIB::checkKeyWord(curword))
			{
				for(int _col=begincol;_col<=col;_col++)
				{
					_mp[line][_col].color=colorConf.keyword;
				}
			}
			if(isdigit(curword.front()))
			{
				for(int _col=begincol;_col<=col;_col++)
				{
					_mp[line][_col].color=colorConf.number;
				}
			}
		}
	}

	// comment & string
	bool incomment=0,instring=0,inchar=0;
	for(int line=1;line<(int)_mp.size();line++)
	{
		for(int col=0;col+1<(int)_mp[line].size();col++)
		{
			if(str[_mp[line][col].id]=='\\')
			{
				_mp[line][col].color=_mp[line][col+1].color=colorConf.cstring;
				col++;
				continue;
			}
			if(instring||inchar)
			{
				_mp[line][col].color=colorConf.cstring;
				if(instring&&str[_mp[line][col].id]=='\"') instring=0;
				if(inchar&&str[_mp[line][col].id]=='\'') inchar=0;
				continue;
			}
			if(str[_mp[line][col].id]=='/'&&str[_mp[line][col+1].id]=='*')
			{
				_mp[line][col].color=colorConf.comment,_mp[line][col+1].color=colorConf.comment;
				incomment=1,col++;
			}
			else if(incomment&&str[_mp[line][col].id]=='*'&&str[_mp[line][col+1].id]=='/')
			{
				_mp[line][col].color=colorConf.comment,_mp[line][col+1].color=colorConf.comment;
				incomment=0;
			}
			else if(incomment)
			{
				_mp[line][col].color=colorConf.comment;
			}
			else
			{
				if(str[_mp[line][col].id]=='\"')
				{
					_mp[line][col].color=colorConf.cstring,instring=1;
				}
				else if(str[_mp[line][col].id]=='\'')
				{
					_mp[line][col].color=colorConf.cstring,inchar=1;
				}
				else if(str[_mp[line][col].id]=='/'&&str[_mp[line][col+1].id]=='/')
				{
					for(int _col=col;_col<(int)_mp[line].size();_col++)
					{
						_mp[line][_col].color=colorConf.comment;
					}
					break;
				}
			}
		}
	}
}
void move_up()
{
	preprint(s);
	if(cury>1)
	{
		cury--;
		for(auto ch:_mp[cury])
		{
			int x=ch.x,id=ch.id;
			if(x>curx) break;
			p=id;
		}
	}
}
void move_down()
{
	preprint(s);
	if(cury+1<(int)_mp.size())
	{
		cury++;
		for(auto ch:_mp[cury])
		{
			int x=ch.x,id=ch.id;
			if(x>curx) break;
			p=id;
		}
	}
}
void deleteline()
{
	int st=_mp[cury].front().id,en=_mp[cury].back().id;
	p=st;
	if(cury==1&&(int)_mp.size()==2)
	{
		s.erase(s.begin()+st,s.begin()+en);
	}
	else if(cury==(int)_mp.size()-1&&cury!=1)
	{
		p--;
		s.erase(s.begin()+st-1,s.begin()+en);
	}
	else s.erase(s.begin()+st,s.begin()+en+1);
}
void duplicateline()
{
	int st=_mp[cury].front().id,en=_mp[cury].back().id,pos=en;
	if(cury<(int)_mp.size()-1) en--;
	int len=en-st+1;
	string substr="\n"+s.substr(st,len);
	s.insert(pos,substr);
}
void print(string &str)
{
	preprint(str);

	PIMAGE img=newimage(Width,Height);
	setfont(chHei,chWid,"consolas",img);
	
	// for(int line=1;line<(int)_mp.size();line++)
	for(int line=upline;line<(int)_mp.size()&&line<upline+Height/chHei;line++)
	{
		setcolor(conf.lineNum,img);
		outnumberxy(0,(line-upline)*chHei,line,img);
		for(auto ch:_mp[line])
		{
			int x=ch.x,id=ch.id;color_t co=ch.color;
			
			if(x*chWid+leDelta>Width) break;

			wchar_t output=str[id];
			if(conf.showIC&&str[id]=='\t') output=L'→',co=colorConf.invischar;
			if(conf.showIC&&str[id]=='\n') output=L'↓',co=colorConf.invischar;
			if(conf.showIC&&str[id]==' ') output=L'·',co=colorConf.invischar;

			setcolor(co,img);
			outtextxy(x*chWid+leDelta,(line-upline)*chHei,output,img);
		}
	}

	setcolor(WHITE,img);
	int posx=curx*chWid+leDelta,posy=(cury-upline)*chHei;
	line(posx,posy,posx,posy+chHei,img);
	putimage(0,0,img);

	delimage(img);
}
void Flush()
{
	print(s);
}
void savefile()
{
	if(!hasfilename)
	{
		cout<<"file name:";
		getline(cin,filename);
		hasfilename=1;
		setcaption(filename.c_str());
	}
	ofstream fout;
	fout.open(conf.workPath+filename);
	fout<<s;
	fout.close();
	cout<<"[saved]"<<endl;
}
void newfile()
{
	cout<<"save file?(yes:1 no:0):";
	int flag;
	cin>>flag;
	fflush(stdin);
	if(flag) savefile();

	setcaption("[untitled]");

	init();
	cout<<"[new]"<<endl;
}
void _openfile(string _filename,bool workPath=1)
{
	ifstream fin;
	if(workPath) fin.open(conf.workPath+_filename);
	else fin.open(_filename);
	if(!fin.good()) return cout<<"can't find such file\n",void();

	init();
	hasfilename=1,filename=_filename;
	char ch=fin.get();
	while(ch!=EOF) s+=ch,ch=fin.get();
	setcaption(filename.c_str());

	historyVer.push(getNowRecord());
	cout<<"[opened]"<<endl;
}
void openfile()
{
	cout<<"save file?(yes:1 no:0):";
	int flag;
	cin>>flag;
	fflush(stdin);
	if(flag) savefile();

	cout<<"file name:";
	string _filename;
	getline(cin,_filename);

	_openfile(_filename);
}
void copyall()
{
	copyToClipboard(s);
	cout<<"[copied]"<<endl;
}
void _RecordHistory()
{
	string cur="";
	while(1)
	{
		string _s=s;
		if(cur!=_s)
		{
			historyVer.push(getNowRecord()),cur=_s;
		}
		this_thread::sleep_for(chrono::seconds(conf.saveTime));
	}
}
void undo()
{
	if(historyVer.size()) historyVer.top().undo(),historyVer.pop(),Flush();
}
void jumpline()
{
	cout<<"jump to line:";
	int target;
	cin>>target;
	if(target<=0)
	{
		cout<<"invalid"<<endl;
		return;
	}
	target=min(target,(int)_mp.size()-1);
	upline=target;
	cury=target,curx=0;
	p=_mp[cury].front().id;
}
void togglecomment()
{
	// checkcomment

	int flag=-1;
	for(int i=0;i<(int)_mp[cury].size()-1;i++)
	{
		auto ch=_mp[cury][i],nch=_mp[cury][i+1];
		if(s[ch.id]!='\t')
		{
			if(s[ch.id]=='/'&&s[nch.id]=='/')
			{
				flag=ch.id;
			}
			break;
		}
	}

	if(flag==-1) // add
	{
		int pos=_mp[cury].front().id;
		p=pos;
		s.insert(pos,"//");
	}
	else // del
	{
		p=flag;
		s.erase(flag,2);
	}
}
void opencmd()
{
	thread(system,"start cmd").detach();
}
void compile(int run=0)
{
	savefile();

	cout<<"-- start compiling --"<<endl;

	auto __div_name=getRealExname();
	string exname=__div_name.second,realname=__div_name.first;
	
	if(exname!="cpp"&&exname!="c")
	{
		cout<<"not .cpp or .c file"<<endl;
		return;
	}

	auto _compileMain=[&](string filename,string realname,int _run)->void
	{
		string cmd="g++.exe \""+conf.workPath+filename+"\" -o \""+conf.workPath+realname+".exe\" "+conf.compileOp;
		cout<<"compile command: "<<cmd<<endl;
		int flag=system(cmd.c_str());
		cout<<"-- compile finished --"<<endl;

		if(!flag&&_run)
		{
			auto _runMain=[&](string realname)->void
			{
				fflush(stdin),fflush(stdout);
				cout<<"--- running ---"<<endl;
				string _run_cmd="\""+conf.workPath+realname+".exe\"";
				system(_run_cmd.c_str());
				cout<<endl<<"--- finished ---"<<endl;
				fflush(stdin),fflush(stdout);
			};
			thread(_runMain,realname).detach();
		}
	};

	thread(_compileMain,filename,realname,run).detach();
}
void taskkill()
{
	cout<<"[taskkill]"<<endl;

	system("taskkill /im g++.exe /f");
	system("taskkill /im cc1plus.exe /f");

	string realname=getRealExname().first;
	string cmd="taskkill /im "+realname+".exe /f";
	system(cmd.c_str());
}
void _CheckMouse()
{
	while(1)
	{
		mouse_msg msg=getmouse();
		if(msg.is_wheel())
		{
			if(msg.wheel>0) moveup(),Flush();
			else movedown(),Flush();
		}
		if(msg.is_left())
		{
			int _cury=msg.y/chHei+upline,_curx=(msg.x-leDelta)/chWid,_p;
			auto _vec=_mp[_cury];
			for(auto ch:_vec)
			{
				int x=ch.x,id=ch.id;
				if(x>_curx) break;
				_p=id;
			}
			cury=_cury,curx=_curx,p=_p;
			Flush();
		}
		std::this_thread::sleep_for(chrono::milliseconds(5));
	}
}
void _CheckKey()
{
	// these keys do not need sleep after pressing
	// so use getch is enough

	// new thread to avoid strange bugs

	while(1)
	{
		int ch=getch();

		// check Ctrl+?: these will be dealed with in _CheckKeyboard()
		// and avoid Ctrl+M='\n', etc
		if(pCtrl()) continue;

		if(0) cout<<"By @wangyizhi571247"<<endl;

		// normal keys
		else if(ch>=32&&ch<128) insert(ch),Flush();

		// \t and \n
		else if(ch==9) insert('\t'),Flush();
		else if(ch==13)
		{
			int c=getCurTab();
			if(p&&s[p-1]=='{')
			{
				insert('\n');
				insert(string(c+1,'\t'));
				if(p!=(int)s.size()&&s[p]=='}')
				{
					insert('\n');
					insert(string(c,'\t'));

					move_up();
					move_end();
				}
			}
			else
			{
				insert('\n');
				insert(string(c,'\t'));
			}
			Flush();
		}

		// page up&down
		else if(ch==289) moveup(),Flush();
		else if(ch==290) movedown(),Flush();

		// home&end
		else if(ch==292) move_home(),Flush();
		else if(ch==291) move_end(),Flush();

		// up&down&left&right
		else if(ch==293) move_left(),Flush();
		else if(ch==295) move_right(),Flush();
		else if(ch==294) move_up(),Flush();
		else if(ch==296) move_down(),Flush();

		// backspace&delete
		else if(ch==8) erase_pre(),Flush();
		else if(ch==302) erase_suf(),Flush();
	}
}
void _CheckKeyboard()
{
	// only check Ctrl & Fn
	while(1)
	{
		if(0) cout<<"By @wangyizhi571247"<<endl;

		else if(pCtrl('N')) newfile(),Flush(),Sleep(delayLength);
		else if(pCtrl('O')) openfile(),Flush(),Sleep(delayLength);
		else if(pCtrl('S')) savefile(),Flush(),Sleep(delayLength);

		else if(pCtrl('D')) deleteline(),Flush(),Sleep(delayLength);
		else if(pCtrl('E')) duplicateline(),Flush(),Sleep(delayLength);
		
		else if(pCtrl('C')) copyall(),Sleep(delayLength);
		else if(pCtrl('V')) insert_paste(),Flush(),Sleep(delayLength);

		else if(pCtrl('Z')) undo(),Flush(),Sleep(delayLength);
		
		else if(pCtrl('R')) opencmd(),Sleep(delayLength);
		else if(pCtrl('W')) taskkill(),Sleep(delayLength);

		else if(pCtrl('J')) jumpline(),Flush();

		else if(pCtrl(key_slash)) togglecomment(),Flush(),Sleep(delayLength);
		else if(pCtrl(key_left)) ; // todo (dhc)
		else if(pCtrl(key_right)) ; // todo (dhc)
		else if(pCtrl(key_back)) ; // todo
		else if(pCtrl()) ; // do nothing

		else if(keystate(key_f9)) compile(),Sleep(delayLength);
		else if(keystate(key_f11)) compile(1),Sleep(delayLength);
	}
}
int main(int argc,char** argv)
{
	system("title LemonCpp");

	initgraph(Width,Height);
	setcaption("[untitled]");

	if(argc>1) _openfile(argv[1],0);

	Flush();
	
	if(conf.useMouse) thread(_CheckMouse).detach();
	thread(_CheckKey).detach();
	thread(_CheckKeyboard).detach();
	thread(_RecordHistory).detach();

	// check esc
	while(1)
	{
		if(keystate(key_esc))
		{
			int ans;
			cout<<"are you sure?(yes:1 no:0):";
			cin>>ans;
			if(ans) break;
		}
	}

	return 0;
}