#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
bool isFileExists(string filePath) 
{
    // 以只读模式打开文件
    FILE *file = fopen(filePath.c_str(), "r");
    if (file != NULL) {
        fclose(file);  // 打开成功，关闭文件
        return true;
    }
    return false;  // 打开失败，文件不存在
}
struct Move
{
    int ori_x;int ori_y;
    int goal_x;int goal_y;
    int arr_x;int arr_y;
};
struct Situation
{
    int mp[8][8];
}ori_state;
int Size=8;int dx[8]={1,0,-1,0,1,1,-1,-1};int dy[8]={0,1,0,-1,1,-1,1,-1};
void GetSituation(string FileName);
int GenerateMove(Situation state,int color);
void MainMenu();
void ReadSave()
{
    bool SaveExists[32];memset(SaveExists,0,sizeof SaveExists);
    string FileName="D:\\Code\\Amazon\\UIandBOT\\save01.sav";
    for (int i=1;i<=20;++i) 
    {
        FileName[29]=char('0'+i%10);FileName[28]=char('0'+i/10);
        // for (int j=0;j<=9;++j) cout<<FileName[j];
        // cout<<endl<<FileName<<endl;
        if (isFileExists(FileName)) SaveExists[i]=1;
    }
    //cout<<SaveExits[1];
    printf("当前有效存档有：\n");
    for (int i=1;i<=20;++i) 
    {
        if (SaveExists[i]) 
        {
            if (i<10) {printf("0");printf("%d ",i);}
            else printf("%d ",i);
        }
    }
    printf("\n");
    printf("输入存档编号以加载对应存档：\n");
    int SaveID;cin>>SaveID;
    FileName[29]=char('0'+SaveID%10);FileName[28]=char('0'+SaveID/10);
    GetSituation(FileName);
}
int BotColor;int turnID;int NowColor;
void PlaceChess(Situation &state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y,int color)
{
    state.mp[goal_x][goal_y]=color;state.mp[ori_x][ori_y]=0;state.mp[arr_x][arr_y]=2;
}
void DrawGame()
{
    system("cls");
    printf("当前盘面：\n");
    printf("  0 1 2 3 4 5 6 7 <-x轴\n");
    for (int i=0;i<8;++i)
    {
        printf("%d",i);
        printf("|");
        for (int j=0;j<8;++j) 
        {
            if (ori_state.mp[j][i]==-1) 
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
                printf("W");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else if (ori_state.mp[j][i]==1) 
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE);
                printf("B");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else if (ori_state.mp[j][i]==2) 
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
                printf("O");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            else printf(" ");
            printf("|");
        }
        printf("%d",i);
        printf("\n  ");
        for (int j=0;j<8;++j) printf("-+");
        printf("\n");
    }
    printf("  0 1 2 3 4 5 6 7 <-x轴\n");
}
double TimeLimit;
void BotConsider()
{
    int num=GenerateMove(ori_state,BotColor);
    if (num==0)
    {
        printf("AI已无棋可走！恭喜您获得胜利。\n试试挑战更难的AI吧！\n");
        printf("将在2s后回到主菜单。");
        _sleep(2*CLOCKS_PER_SEC);
        MainMenu();
    }
    printf("AI 思考中，将在%.2lfs内落子",TimeLimit);
    system("D:\\Code\\Amazon\\UIandBOT\\Amazon-Bot.exe");
    ifstream Bot("BotOutput.out",ios::in);
    int x0,y0,x1,y1,x2,y2;
    Bot >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
    PlaceChess(ori_state,x0,y0,x1,y1,x2,y2,BotColor);
    Bot.close();
    ofstream Now("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::app);
    Now<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
    Now.close();
}
void ExitGame()
{
    printf("感谢您使用本UI界面\n");
    printf("开发者：2500013177\n");
    ofstream fout("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::out); 
    fout.close();
    _sleep(2*CLOCKS_PER_SEC);
    exit(0);
}
int totstep=1;
void SaveGame()
{
    bool SaveExists[32];memset(SaveExists,0,sizeof SaveExists);
    string FileName="D:\\Code\\Amazon\\UIandBOT\\save01.sav";
    for (int i=1;i<=20;++i) 
    {
        FileName[29]=char('0'+i%10);FileName[28]=char('0'+i/10);
        if (isFileExists(FileName)) SaveExists[i]=1;
    }
    printf("当前空余存档位有：\n");
    for (int i=1;i<=20;++i) 
    {
        if (!SaveExists[i]) 
        {
            if (i<10) {printf("0");printf("%d ",i);}
            else printf("%d ",i);
        }
    }
    printf("\n");
    printf("输入存档位编号以存档到对应位置：\n");
    printf("提示：输入已存在的存档编号将不可逆覆盖存档！\n");
    int id;cin>>id;FileName[29]=char('0'+id%10);FileName[28]=char('0'+id/10);
    ifstream xIN("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::in);
    ofstream xOUT(FileName.c_str(),ios::out);
    xIN>>turnID;
    xOUT<<turnID<<endl;
    int x0,y0,x1,y1,x2,y2;
	for (int i=1;i<=turnID;++i)
	{
		xIN >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
        xOUT<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
		if (i <= turnID - 1)
		{
			xIN >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            xOUT<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
		}
	}
    xIN.close();xOUT.close();
    MainMenu();
}
bool CheckRoad(Situation state,int ori_x,int ori_y,int goal_x,int goal_y)
{
    if (goal_x==ori_x&&goal_y==ori_y) return 0;
    if (goal_x==ori_x) 
    {
        for (int j=min(ori_y,goal_y);j<=max(ori_y,goal_y);++j) if (state.mp[ori_x][j]!=0) return 0;
    }
    else if (goal_y==ori_y) 
    {
        for (int j=min(ori_x,goal_x);j<=max(ori_x,goal_x);++j) if (state.mp[j][ori_y]!=0) return 0;
    }
    else 
    {
        int dx=goal_x-ori_x,dy=goal_y-ori_y;
        if (abs(dx)!=abs(dy)) return 0;
        int fhx=(dx>0)?1:-1;int fhy=(dy>0)?1:-1;
        while (ori_x!=goal_x)
        {
            ori_x+=fhx;ori_y+=fhy;
            if (state.mp[ori_x][ori_y]!=0) return 0;
        }
    }
    return 1;
}
bool Check(Situation state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y,int color)
{
    if (goal_x>=Size || goal_x<0 || goal_y>=Size || goal_y<0 || arr_x>=Size || arr_x<0 || arr_y>=Size || arr_y<0) return 0;
    if (state.mp[goal_x][goal_y]!=0 || ( (arr_x!=ori_x || arr_y!=ori_y) && state.mp[arr_x][arr_y]!=0) || state.mp[ori_x][ori_y]!=color ) return 0;
    state.mp[ori_x][ori_y]=0;
    if (!CheckRoad(state,ori_x,ori_y,goal_x,goal_y)) return 0;
    if (!CheckRoad(state,goal_x,goal_y,arr_x,arr_y)) return 0;
    state.mp[ori_x][ori_y]=color;
    return 1;
}
void HumanConsider()
{
    int num=GenerateMove(ori_state,-BotColor);
    if (num==0)
    {
        printf("您已无棋可走！很遗憾您输了。\n");
        printf("将在2s后回到主菜单。");
        _sleep(2*CLOCKS_PER_SEC);
        MainMenu();
    }
    printf("请落子，格式为x0 y0 x1 y1 x2 y2\n");
    printf("您是");if (BotColor==-1) printf("黑方，控制棋子为B");else printf("白方，控制棋子为W");
    printf("\nO表示障碍（箭），B表示黑方棋子，W表示白方棋子");
    printf("\n如需存档，请输入-1\n如需直接结束游戏（不存档）请输入-2（这将回到主菜单）\n");
    int x0,y0,x1,y1,x2,y2;
    cin>>x0;
    if (x0==-1) SaveGame();
    else if (x0==-2) MainMenu();
    else
    {
        cin>> y0 >> x1 >> y1 >> x2 >> y2;
        while (!Check(ori_state,x0,y0,x1,y1,x2,y2,-BotColor))
        {
            printf("棋步不合法！请重新输入：");
            cin>> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
        }
        PlaceChess(ori_state,x0,y0,x1,y1,x2,y2,-BotColor);
        ofstream Now("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::app);
        Now<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
        Now.close();
    }
    //------ 更新input的turnid
    {
    ifstream fIN("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::in);
    ofstream fOUT("D:\\Code\\Amazon\\UIandBOT\\Temp.tmp",ios::out);
    fIN>>turnID;
    fOUT<<turnID+1<<endl;
    int x0,y0,x1,y1,x2,y2;
	for (int i=1;i<=turnID+1;++i)
	{
		fIN >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
        fOUT<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
		if (i <= turnID)
		{
			fIN >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            fOUT<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
		}
	}
    fIN.close();fOUT.close();
    ifstream xIN("D:\\Code\\Amazon\\UIandBOT\\Temp.tmp",ios::in);
    ofstream xOUT("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::out);
    xIN>>turnID;
    xOUT<<turnID<<endl;

	for (int i=1;i<=turnID;++i)
	{
		xIN >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
        xOUT<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
		if (i <= turnID - 1)
		{
			xIN >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            xOUT<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
		}
	}
    xIN.close();xOUT.close();
    }
}

void BeginGame()
{
    printf("设置AI难度：输入AI思考时间上限：\n");
    printf("或者，使用预设难度。简单请输入1；正常请输入3；困难请输入5：");
    cin>>TimeLimit;
    ofstream File("TimeLimit.in",ios::out);
    File<<TimeLimit;
    File.close();
    DrawGame();
    while (1)
    {
        ++totstep;
        if (NowColor==BotColor) BotConsider();
        else HumanConsider();
        DrawGame();
        NowColor=-NowColor;
    }
}
void NewGame()
{
    
    printf("请选择您的先后手：输入1选择黑方先手，2选择白方后手：");
    int tmp;cin>>tmp;if (tmp==1) BotColor=-1;else BotColor=1;
    NowColor=1;
    ofstream fout("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::out); 
    fout<<"1\n-1 -1 -1 -1 -1 -1\n";
    fout.close();
    BeginGame();
}
void MainMenu()
{
    system("chcp 65001");
    printf("        亚马逊棋        \n");
    printf("01.     加载存档        \n");
    printf("02.     新 对 局        \n");
    printf("03.     退出游戏        \n");
    printf("输入1/2/3以开始游玩亚马逊\n");
    memset(ori_state.mp,0,sizeof ori_state.mp);
    ori_state.mp[0][2]=1;ori_state.mp[5][0]=1;ori_state.mp[2][0]=1;ori_state.mp[7][2]=1;
    ori_state.mp[0][5]=-1;ori_state.mp[2][7]=-1;ori_state.mp[5][7]=-1;ori_state.mp[7][5]=-1;
    totstep=1;BotColor=-1;turnID=0;
    int tmp;cin>>tmp;
    if (tmp==1) ReadSave();
    else if (tmp==2) NewGame();
    else if (tmp==3) ExitGame();
}
int main()
{
    MainMenu();
    return 0;
}
void GetSituation(string FileName)
{
    //freopen(FileName.c_str(),"r",stdin);
    ifstream File(FileName, ios::in);
    ofstream Now("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::out);
    BotColor=-1;//白
    File>>turnID;
    Now<<turnID<<endl;
    int x0,y0,x1,y1,x2,y2;
	for (int i=1;i<=turnID;++i)
	{
		File >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
        Now<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
		if (x0 == -1)
			BotColor = 1;
		else
        {
			PlaceChess(ori_state,x0, y0, x1, y1, x2, y2,-BotColor);
            ++totstep;
            NowColor=BotColor;
        }
		if (i <= turnID - 1)
		{
			File >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
            Now<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
			if (x0 >= 0)
				PlaceChess(ori_state,x0, y0, x1, y1, x2, y2, BotColor);
            NowColor=-BotColor;
            ++totstep;
		}
	}
    File.close();
    Now.close();
    BeginGame();
}
int GenerateMove(Situation state,int color)
{
    vector<Move> cur;
    for (int ori_x=0;ori_x<Size;++ori_x)
    for (int ori_y=0;ori_y<Size;++ori_y)
    {
        if (state.mp[ori_x][ori_y] != color)
                continue;
        for (int i=0;i<8;++i)
        {
            int nx=ori_x,ny=ori_y;
            while (true)
            {
                nx+=dx[i],ny+=dy[i];
                if (nx>=Size||nx<0||ny>=Size||ny<0||state.mp[nx][ny]!=0) break;
                for (int j=0;j<8;++j)
                {
                    int ax=nx,ay=ny;
                    while (true)
                    {
                        ax+=dx[j],ay+=dy[j];
                        if (ax>=Size||ax<0||ay>=Size||ay<0|| (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y) )) break;
                        cur.emplace_back(Move{ori_x,ori_y,nx,ny,ax,ay});
                    }
                }
            }
        }
    }
    return cur.size();
}