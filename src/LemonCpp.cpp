// by @wangyizhi571247
// Lemon.Cpp (ver 0.0)

#include<iostream>
#include<iomanip>
#include<stack>
#include<fstream>
#include<vector>
#include<thread>
#include<graphics.h>

#include"char.h"
#include"config.h"
#include"cpp.h"
#include"extester.hpp"

using namespace std;

__config conf;
__color_config colorConf;
namespace LIB
{
	bool isword(char ch){return isalpha(ch)||ch=='_';}
	bool issymbol(char ch){return !isdigit(ch)&&!isword(ch);}
}
int pCtrl()
{
	return keystate(key_control);
}
int pCtrl(int ch)
{
	return pCtrl()&&keystate(ch);
}
int pShift()
{
	return keystate(key_shift);
}
int pCtrlShift(int ch)
{
	return pCtrl()&&pShift()&&keystate(ch);
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
int p,upline=1,curx,cury;
string s,filename;
string getFilePath(string file)
{
	if(file.size()>2&&file[1]==':') return file;
	return conf.workPath+file;
}
pair<string,string> getRealExname(string filename)
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
const int delayLength=200;
const int Width=1024,Height=512,chWid=8,chHei=16;
const int leDelta=4*chWid;
vector<vector<chNode>> _mp;
struct __Chooser
{
	int fixp;
	__Chooser():fixp(-1){}
	bool empty()
	{
		return fixp==-1||fixp==p;
	}
	void setfixp(int p)
	{
		fixp=p;
	}
	void clear()
	{
		setfixp(-1);
	}
	void update()
	{
		setfixp(p);
	}
	int getl()
	{
		return min(fixp,p);
	}
	int getr()
	{
		return max(fixp,p)-1;
	}
	bool checkPos(int p)
	{
		if(empty()) return 0;
		return getl()<=p&&getr()>=p;
	}
	vector<int> getLineNum()
	{
		if(empty()) return vector<int>();
		int l=getl(),r=getr();
		vector<int> res;
		for(int i=1;i<(int)_mp.size();i++)
		{
			int le=_mp[i].front().id,ri=_mp[i].back().id;
			if(max(le,l)<=min(ri,r)) res.push_back(i);
		}
		return res;
	}
}chooser;
void init()
{
	setbkcolor(conf.bgColor);
	s="",p=0,curx=0,cury=1,hasfilename=0,upline=1,filename="";
	chooser.clear();
	_mp.clear();
	while(historyVer.size()) historyVer.pop();
}
void erase_pre()
{
	if(!chooser.empty())
	{
		int l=chooser.getl(),r=chooser.getr();
		s.erase(l,r-l+1),p=l;
	}
	else if(p)
	{
		s.erase(s.begin()+p-1),p--;
	}
}
void erase_suf()
{
	if(!chooser.empty())
	{
		int l=chooser.getl(),r=chooser.getr();
		s.erase(l,r-l+1),p=l;
	}
	else if(p<(int)s.size())
	{
		s.erase(s.begin()+p);
	}
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
void movepageup()
{
	upline-=Height/chHei;
	if(upline<1) upline=1;
	p=_mp[upline].front().id;
}
void movepagedown()
{
	upline+=Height/chHei;
	if(upline>=(int)_mp.size()) upline=(int)_mp.size()-1;
	p=_mp[upline].front().id;
}
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
		chNode ch;ch.x=x,ch.id=cnt;ch.color=defaultcolor;ch.bgcolor=conf.bgColor;
		if(chooser.checkPos(cnt)) ch.bgcolor=0x3c55aaff;
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
			if(CPP::checkKeyWord(curword))
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
void movelineup()
{
	int st=cury,en=cury;

	if(!chooser.empty())
	{
		vector<int> lines=chooser.getLineNum();
		st=lines.front(),en=lines.back();
	}

	// cout<<"(debug) lines=["<<st<<","<<en<<"]"<<endl;

	if(st==1) return;

	// move line st-1 to en
	int bg=_mp[st-1].front().id,ed=_mp[st-1].back().id,tar=_mp[en].back().id+1;

	// cout<<"(debug) move "<<bg<<" "<<ed<<" to "<<tar<<endl;

	int len=ed-bg+1;
	string str=s.substr(bg,len);
	if(en==(int)_mp.size()-1)
	{
		tar--;
		str="\n"+str;
		str.pop_back();
	}

	if(!chooser.empty()) chooser.fixp-=len;
	p-=len;
	s.insert(tar,str);
	s.erase(bg,len);
}
void movelinedown()
{
	int st=cury,en=cury;

	if(!chooser.empty())
	{
		vector<int> lines=chooser.getLineNum();
		st=lines.front(),en=lines.back();
	}

	// cout<<"(debug) lines=["<<st<<","<<en<<"]"<<endl;

	if(en==(int)_mp.size()-1) return;

	// move line st-1 to en
	int bg=_mp[en+1].front().id,ed=_mp[en+1].back().id,tar=_mp[st].front().id;

	
	if(en+1==(int)_mp.size()-1)
	{
		bg--,ed--;
	}

	// cout<<"(debug) move "<<bg<<" "<<ed<<" to "<<tar<<endl;

	int len=ed-bg+1;

	string str=s.substr(bg,len);

	// cerr<<"(debug) text="<<str<<endl;

	if(en+1==(int)_mp.size()-1)
	{
		str.erase(str.begin());
		str+="\n";
	}

	if(!chooser.empty()) chooser.fixp+=len;
	p+=len;
	s.erase(bg,len);
	s.insert(tar,str);

	// cout<<"(debug) p="<<p<<endl;
}
void deleteline()
{
	chooser.clear();
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
	chooser.clear();
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
	setbkcolor(conf.bgColor,img);

	// for(int line=1;line<(int)_mp.size();line++)
	for(int line=upline;line<(int)_mp.size()&&line<upline+Height/chHei;line++)
	{
		setcolor(conf.lineNum,img);
		setfontbkcolor(conf.bgColor,img);
		outnumberxy(0,(line-upline)*chHei,line,img);
		for(auto ch:_mp[line])
		{
			int x=ch.x,id=ch.id;color_t co=ch.color,bgco=ch.bgcolor;
			
			if(x*chWid+leDelta>Width) break;

			wchar_t output=str[id];
			if(conf.showIC&&str[id]=='\t') output=L'→',co=colorConf.invischar;
			if(conf.showIC&&str[id]=='\n') output=L'↓',co=colorConf.invischar;
			if(conf.showIC&&str[id]==' ') output=L'·',co=colorConf.invischar;

			setcolor(co,img);
			setfontbkcolor(bgco,img);
			outtextxy(x*chWid+leDelta,(line-upline)*chHei,output,img);

			if(str[id]=='\t')
			{
				setfontbkcolor(bgco,img);
				outtextxy(x*chWid+leDelta,(line-upline)*chHei,string(4,' ').c_str(),img);
			}
		}
	}

	setcolor(~conf.bgColor,img);
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
	fout.open(getFilePath(filename));
	fout<<s;
	fout.close();
	// cout<<"[saved]"<<endl;
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
	// cout<<"[new]"<<endl;
}
void _openfile(string _filename)
{
	_filename=getFilePath(_filename);

	ifstream fin(_filename);
	if(!fin.good()) return cout<<"can't find such file\n",void();

	init();
	hasfilename=1,filename=_filename;
	char ch=fin.get();
	while(ch!=EOF) s+=ch,ch=fin.get();
	setcaption(filename.c_str());

	historyVer.push(getNowRecord());
	// cout<<"[opened]"<<endl;
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
void chooseAll()
{
	chooser.setfixp(0);
	p=(int)s.size();
}
void copy()
{
	if(chooser.empty()) 
	{
		copyToClipboard(s);
	}
	else
	{
		int l=chooser.getl(),r=chooser.getr();
		string text=s.substr(l,r-l+1);
		copyToClipboard(text);
	}
	// cout<<"[copied]"<<endl;
}
void cut()
{
	if(!chooser.empty())
	{
		copy(),erase_pre();
		chooser.clear();
	}
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
	chooser.clear();
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
	auto checkcomment=[&](int y)->int
	{
		for(int i=0;i<(int)_mp[y].size()-1;i++)
		{
			auto ch=_mp[y][i],nch=_mp[y][i+1];
			if(s[ch.id]!='\t')
			{
				if(s[ch.id]=='/'&&s[nch.id]=='/')
				{
					return ch.id;
				}
				break;
			}
		}
		return -1;
	};

	auto addcomment=[&](int y)->void
	{
		// cout<<"(debug) addcomment "<<y<<endl;

		int pos=_mp[y].front().id;
		p=pos;
		s.insert(pos,"//");

		preprint(s);
	};

	auto delcomment=[&](int y)->void
	{
		// cout<<"(debug) delcomment "<<y<<endl;

		p=_mp[y].front().id;
		int flag=checkcomment(y);
		s.erase(flag,2);

		preprint(s);
	};

	if(!chooser.empty())
	{
		bool flag=0; // is add
		auto chooseLine=chooser.getLineNum();

		// cout<<"(debug) choose=["<<chooser.getl()<<","<<chooser.getr()<<"]"<<endl;
		// cout<<"(debug) chooseLine=[ ";
		// for(int i:chooseLine) cout<<i<<" ";
		// cout<<"]"<<endl;

		for(int y:chooseLine)
		{
			if(checkcomment(y)==-1)
			{
				flag=1;
				break;
			}
		}

		if(flag)
		{
			for(int y:chooseLine)
			{
				addcomment(y);
			}
		}
		else
		{
			for(int y:chooseLine)
			{
				delcomment(y);
			}
		}
	}
	else
	{
		if(checkcomment(cury)==-1) // add
		{
			addcomment(cury);
		}
		else // del
		{
			delcomment(cury);
		}
	}

	chooser.clear();
}
void opencmd()
{
	thread(system,"start cmd").detach();
}
void compile(int run=0)
	// run=0: compile only
	// run=1: compile & run
	// run=2: start extester
{
	savefile();

	cout<<"-- start compiling --"<<endl;

	auto __div_name=getRealExname(getFilePath(filename));
	string exname=__div_name.second,realname=__div_name.first;
	
	if(exname!="cpp"&&exname!="c")
	{
		cout<<"not .cpp or .c file"<<endl;
		return;
	}

	auto _compileMain=[&](string filename,string realname,int _run)->void
	{
		string cmd="g++.exe \""+getFilePath(filename)+"\" -o \""+getFilePath(realname)+".exe\" "+conf.compileOp;
		cout<<"compile command: "<<cmd<<endl;
		int flag=system(cmd.c_str());
		cout<<"-- compile finished --"<<endl;

		if(flag) return;

		if(_run==1)
		{
			auto _runMain=[&](string realname)->void
			{
				fflush(stdin),fflush(stdout);
				cout<<"--- running ---"<<endl;
				string _run_cmd="start coderunner.exe \""+getFilePath(realname)+".exe\"";
				// cout<<_run_cmd<<endl;
				system(_run_cmd.c_str());
				cout<<endl<<"--- finished ---"<<endl;
				fflush(stdin),fflush(stdout);
			};
			thread(_runMain,realname).detach();
		}
		else if(_run==2)
		{
			Extester::main(getFilePath(filename),getFilePath(realname)+".exe",s,_mp);
		}
	};

	thread(_compileMain,filename,realname,run).detach();
}
void taskkill()
{
	// cout<<"[taskkill]"<<endl;
	system("taskkill /im g++.exe /f");
	system("taskkill /im cc1plus.exe /f");
}
void _CheckMouse()
{
	while(1) if(mousemsg())
	{
		mouse_msg msg=getmouse();
		if(msg.is_wheel())
		{
			if(msg.wheel>0) moveup(),Flush();
			else movedown(),Flush();
		}
		auto setmouse=[&]()->void
		{
			int _cury=msg.y/chHei+upline,_curx=(msg.x-leDelta)/chWid,_p;

			if(_cury>=(int)_mp.size()) _cury=(int)_mp.size()-1;

			auto _vec=_mp[_cury];
			for(auto ch:_vec)
			{
				int x=ch.x,id=ch.id;
				if(x>_curx) break;
				_p=id;
			}
			cury=_cury,curx=_curx,p=_p;
		};

		if(msg.is_left())
		{
			setmouse();
		
			if(pShift())
			{
				if(chooser.empty()) chooser.update();
			}
			else
			{
				chooser.clear();
			}

			Flush();

			while(0)
			// while(1)
				if(mousemsg())
			{
				mouse_msg msg=getmouse();

				if(!msg.is_left())
				{
					break;
				}

				cout<<"(debug) chooser=["<<chooser.getl()<<","<<chooser.getr()<<"]"<<endl;
				setmouse();
				Flush();
				std::this_thread::sleep_for(chrono::milliseconds(10));
			}

			// check chooser
		}
		std::this_thread::sleep_for(chrono::milliseconds(10));
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
		
		// Ctrl+left/right/up/down
		// Ctrl+otherkeys be dealed with in _CheckKeyboard()
		// and avoid Ctrl+M='\n', etc

		if(pCtrl())
		{
			// if(0) ;
			// // else if(ch==) togglecomment(),Flush(),Sleep(delayLength);
			// else if(ch==294) moveup(),Flush(),Sleep(delayLength);
			// else if(ch==296) movedown(),Flush(),Sleep(delayLength);
			// // else if(ch==) ; // todo (dhc) (!fzs)
			// // else if(ch==) ; // todo (dhc) (!fzs)
			continue;
		}
		
		if(0) cout<<"By @wangyizhi571247"<<endl;

		// not "move" keys
		else if((ch>=32&&ch<128)||ch==9||ch==13||ch==8||ch==302)
		{
			// cout<<"(debug) insert"<<"\'"<<(char)ch<<"\'"<<endl;

			if(ch>=32&&ch<128) insert(ch);

			// \t and \n
			else if(ch==9)
			{
				insert('\t');
			}
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
			}

			// backspace&delete
			else if(ch==8) erase_pre();
			else if(ch==302) erase_suf();

			chooser.clear();
			chooser.update();
			Flush();
		}
		// "move" keys
		else if(ch>=289&&ch<=296)
		{
			// check chooser
			if(pShift())
			{
				if(chooser.empty()) chooser.update();
			}
			else
			{
				chooser.clear();
			}

			if(0) ;

			// page up&down
			else if(ch==289) movepageup();
			else if(ch==290) movepagedown();

			// home&end
			else if(ch==292) move_home();
			else if(ch==291) move_end();

			// up&down&left&right
			else if(ch==293) move_left();
			else if(ch==295) move_right();
			else if(ch==294) move_up();
			else if(ch==296) move_down();

			// cout<<"(debug) chooser= "<<chooser.getl()<<" "<<chooser.getr()<<endl;
			Flush();
		}

		this_thread::sleep_for(chrono::milliseconds(10));
	}
}
void _CheckKeyboard()
{
	// only check Ctrl & Fn
	while(1)
	{
		if(0) cout<<"By @wangyizhi571247"<<endl;

		// check ctrl+shift+? first
		else if(pCtrlShift(key_up)) movelineup(),Flush(),Sleep(delayLength);
		else if(pCtrlShift(key_down)) movelinedown(),Flush(),Sleep(delayLength);

		// ctrl+?
		else if(pCtrl('N')) newfile(),Flush(),Sleep(delayLength);
		else if(pCtrl('O')) openfile(),Flush(),Sleep(delayLength);
		else if(pCtrl('S')) savefile(),Flush(),Sleep(delayLength);

		else if(pCtrl('A')) chooseAll(),Flush(),Sleep(delayLength);

		else if(pCtrl('D')) deleteline(),Flush(),Sleep(delayLength);
		else if(pCtrl('E')) duplicateline(),Flush(),Sleep(delayLength);
		
		else if(pCtrl('C')) copy(),Sleep(delayLength);
		else if(pCtrl('X')) cut(),Flush(),Sleep(delayLength);
		else if(pCtrl('V')) insert_paste(),Flush(),Sleep(delayLength);

		else if(pCtrl('Z')) undo(),Flush(),Sleep(delayLength);
		
		else if(pCtrl('R')) opencmd(),Sleep(delayLength);
		else if(pCtrl('W')) taskkill(),Sleep(delayLength);

		else if(pCtrl('J')) jumpline(),Flush();

		else if(pCtrl(key_slash)) togglecomment(),Flush(),Sleep(delayLength);
		else if(pCtrl(key_up)) moveup(),Flush(),Sleep(delayLength);
		else if(pCtrl(key_down)) movedown(),Flush(),Sleep(delayLength);
		else if(pCtrl(key_left)) ; // todo (dhc) (!fzs)
		else if(pCtrl(key_right)) ; // todo (dhc) (!fzs)
		else if(pCtrl(key_back)) ; // todo
		
		else if(pCtrl()) ; // do nothing
		
		// fn
		else if(keystate(key_f9)) compile(),Sleep(delayLength);
		else if(keystate(key_f11)) compile(1),Sleep(delayLength);
		else if(keystate(key_f12)) compile(2),Sleep(delayLength);

		this_thread::sleep_for(chrono::milliseconds(10));
	}
}
int main(int argc,char** argv)
{
	// system("cd");

	system("title LemonCpp");

	initgraph(Width,Height);
	setcaption("[untitled]");

	// if(argc>1) _openfile(argv[1]);

	/*debug*/
	// if(0)
	// {
	// 	_openfile("CF1555F.cpp");
	// }

	Flush();

	/*extester debug*/
	// if(0)
	// {
	// 	Extester::main("C:/Users/sbghj/Desktop/wangyizhi/code/CF1555F.cpp","C:/Users/sbghj/Desktop/wangyizhi/code/CF1555F.exe",s,_mp);
	// 	system("pause");
	// 	return 0;
	// }
	
	if(conf.useMouse) thread(_CheckMouse).detach();
	thread(_CheckKey).detach();
	thread(_CheckKeyboard).detach();
	thread(_RecordHistory).detach();

	// check esc
	while(1)
	{
		if(keystate(key_esc))
		{
			// break;
			int ans;
			cout<<"are you sure?(yes:1 no:0):";
			cin>>ans;
			if(ans) break;
		}

		this_thread::sleep_for(chrono::milliseconds(10));
	}

	return 0;
}