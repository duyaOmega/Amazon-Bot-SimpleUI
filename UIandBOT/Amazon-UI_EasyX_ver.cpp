#include<bits/stdc++.h>
#include<windows.h>
#include<graphics.h>
#include<conio.h>
#include<tchar.h>
using namespace std;
#undef UNICODE
#undef _UNICODE
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
struct Button {
    int left,top,right,bottom;
    char text[128];        // 按钮文本
    void (*onClick)();  // 点击回调函数
};

// 创建按钮的封装函数
Button createButton(int left,int top,int right,int bottom, const char* text, void (*onClick)()) {
    Button btn;
    btn.left = left;
    btn.top = top;
    btn.right = right;
    btn.bottom = bottom;
    strcpy(btn.text,text);
    btn.onClick = onClick;
    return btn;
}

// 绘制按钮（使用默认样式）
void drawButton(const Button& btn) {
    // 绘制按钮背景（蓝色）
    setfillcolor(RGB(100, 149, 237));
    fillrectangle(btn.left, btn.top, btn.right, btn.bottom);
    
    // 绘制按钮文字（白色）
    settextcolor(WHITE);
    settextstyle(48,0,"华文行楷");
    setbkmode(TRANSPARENT);
    RECT r={btn.left, btn.top, btn.right, btn.bottom};
    drawtext(btn.text,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

// 检查鼠标是否在按钮区域内
bool isButtonClicked(const Button& btn, int mx, int my) {
    return (mx >= btn.left && mx <= btn.right &&
            my >= btn.top && my <= btn.bottom);
}

// 按钮点击事件处理（在主循环中调用）
void handleButtonClick(const Button& btn, int mx, int my) {
    if (isButtonClicked(btn, mx, my) && btn.onClick) {
        btn.onClick();  // 执行点击回调
    }
}
void ReadSave()
{
    bool SaveExists[32];memset(SaveExists,0,sizeof SaveExists);
    string FileName="D:\\Code\\Amazon\\UIandBOT\\save01.sav";
    for (int i=1;i<20;++i) 
    {
        FileName[29]=char('0'+i%10);FileName[28]=char('0'+i/10);
        // for (int j=0;j<=9;++j) cout<<FileName[j];
        // cout<<endl<<FileName<<endl;
        if (isFileExists(FileName)) SaveExists[i]=1;
    }
    //cout<<SaveExits[1];
    settextstyle(32,0,"宋体");
    RECT r={0,660,800,700};drawtext("当前有效存档位有：\n",&r,DT_CENTER);
    char emp[128];int p=0;
    for (int i=1;i<20;++i) 
    {
        if (SaveExists[i]) 
        {
            if (i<10) 
            {
                //printf("0");printf("%d ",i);
                emp[++p]='0';emp[++p]='0'+i;emp[++p]=',';
            }
            else 
            {
                //printf("%d ",i);
                emp[++p]='1';emp[++p]='0'+i%10;emp[++p]=',';
            }
        }
    }
    //printf("\n");
    r={0,710,800,750};drawtext(emp+1,&r,DT_CENTER|DT_WORDBREAK);
    r={0,760,800,800};drawtext("输入存档编号以加载对应存档：\n",&r,DT_CENTER|DT_WORDBREAK);
    char input[100]; 
    InputBox(input,50,"输入存档编号以加载对应存档：");
    int SaveID=stoi(input);
    FileName[29]=char('0'+SaveID%10);FileName[28]=char('0'+SaveID/10);
    GetSituation(FileName);
}
int BotColor;int turnID;int NowColor;
void PlaceChess(Situation &state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y,int color)
{
    state.mp[goal_x][goal_y]=color;state.mp[ori_x][ori_y]=0;state.mp[arr_x][arr_y]=2;
}
int SquareSize = 600 / 8; // 每个格子大小 (75px)
void DrawGame()
{
    //initgraph(600, 600,EX_SHOWCONSOLE);
    setbkcolor(RGB(219, 195, 35)); // 棋盘底色（类似木质）
    cleardevice();
    
    //  棋盘状态：0=空, 1=黑棋, -1=白棋, 2=蓝棋
  
    
    int pieceRadius = SquareSize * 0.35; // 棋子大小（约35%格子）

    //  1. 画棋盘背景
    for (int i=0;i<=8;++i) line(0,i * SquareSize,8*SquareSize,i*SquareSize);
    for (int i=0;i<=8;++i) line(i * SquareSize,0,i*SquareSize,8*SquareSize);
   
    //  2. 画三种棋子（根据ori_state状态）
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int state = ori_state.mp[i][j];
            if (state == 0) continue; // 空位跳过
            
            // 计算棋子中心坐标
            int centerX = i * SquareSize + SquareSize / 2;
            int centerY = j * SquareSize + SquareSize / 2;
            
            if (state == 1) { // 黑棋
                setfillcolor(BLACK);
                solidcircle(centerX, centerY, pieceRadius);
            } else if (state == -1) { // 白棋
                setfillcolor(WHITE);
                solidcircle(centerX, centerY, pieceRadius);
            } else if (state == 2) { // 蓝棋
                setfillcolor(BLUE);
                solidcircle(centerX, centerY, pieceRadius);
            }
        }
    }

    //3. 画棋盘外框（红框更醒目）
    setcolor(RED);
    rectangle(0, 0, 600, 600);

    // 4. 提示信息（可选）
    setcolor(BLACK);
    settextcolor(BLACK);
    settextstyle(20, 0,_T("宋体"));
    // outtextxy(10, 10, _T("黑棋(1) | 白棋(2) | 蓝棋(3)"));
}
double TimeLimit;
void BotConsider()
{
    int num=GenerateMove(ori_state,BotColor);
    if (num==0)
    {
        //printf("AI已无棋可走！恭喜您获得胜利。\n试试挑战更难的AI吧！\n");
        //printf("将在2s后回到主菜单。");
        RECT r={0,600,600,800};drawtext("AI已无棋可走！恭喜您获得胜利。\n试试挑战更难的AI吧！",&r,DT_CENTER);
        Sleep(2*CLOCKS_PER_SEC);
        MainMenu();
    }
    //printf("AI 思考中，将在%.2lfs内落子",TimeLimit);
    RECT r={0,600,600,800};drawtext("AI 思考中...请等待",&r,DT_CENTER);
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
    settextstyle(26,0,"宋体");
    RECT r={0,660,600,700};drawtext("感谢您使用本UI界面",&r,DT_CENTER);
    r={0,710,600,750};drawtext("开发者：2500013177",&r,DT_CENTER);
    ofstream fout("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::out); 
    fout.close();
    Sleep(2*CLOCKS_PER_SEC);
    exit(0);
}
int totstep=1;
void SaveGame()
{
    bool SaveExists[32];memset(SaveExists,0,sizeof SaveExists);
    string FileName="D:\\Code\\Amazon\\UIandBOT\\save01.sav";
    for (int i=1;i<20;++i) 
    {
        FileName[29]=char('0'+i%10);FileName[28]=char('0'+i/10);
        if (isFileExists(FileName)) SaveExists[i]=1;
    }
    settextstyle(32,0,"宋体");
    RECT r={0,660,800,700};drawtext("当前空余存档位有：\n",&r,DT_CENTER);
    char emp[128];int p=0;
    for (int i=1;i<20;++i) 
    {
        if (!SaveExists[i]) 
        {
            if (i<10) 
            {
                //printf("0");printf("%d ",i);
                emp[++p]='0';emp[++p]='0'+i;emp[++p]=',';
            }
            else 
            {
                //printf("%d ",i);
                emp[++p]='1';emp[++p]='0'+i%10;emp[++p]=',';
            }
        }
    }
    //printf("\n");
    r={0,710,800,790};drawtext(emp+1,&r,DT_CENTER|DT_WORDBREAK);
    char input[100]; 
    InputBox(input,50,"输入存档位编号以存档到对应位置\n输入已存在的存档编号将不可逆覆盖存档！");
    int id=stoi(input);
    //printf("输入存档位编号以存档到对应位置：\n");
    //printf("提示：输入已存在的存档编号将不可逆覆盖存档！\n");
    FileName[29]=char('0'+id%10);FileName[28]=char('0'+id/10);
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
    DrawGame();
    int num=GenerateMove(ori_state,-BotColor);
    if (num==0)
    {
        RECT r={0,600,600,650};drawtext("您已无棋可走！很遗憾您输了。\n将在2s后回到主菜单。",&r,DT_CENTER);
        Sleep(2*CLOCKS_PER_SEC);
        MainMenu();
    }
    settextstyle(24,0,"宋体");
    RECT r={0,600,600,660};drawtext("请落子，先选择要走的棋子，再依次点击目标位置、障碍放置位置",&r,DT_CENTER|DT_WORDBREAK);
    r={0,680,600,740};drawtext("选择的棋子将以红色标出，落子位置以绿色标出",&r,DT_CENTER|DT_WORDBREAK);
    if (BotColor==-1) 
    {
        r={0,750,600,800};
        drawtext("您是黑方，控制棋子为黑色",&r,DT_CENTER);
    }
    else 
    {
        r={0,750,600,800};
        drawtext("您是白方，控制棋子为白色",&r,DT_CENTER);
    }
        
    // printf("\nO表示障碍（箭），B表示黑方棋子，W表示白方棋子");
    // printf("\n如需存档，请输入-1\n如需直接结束游戏（不存档）请输入-2（这将回到主菜单）\n");
    int x0,y0,x1,y1,x2,y2;
    //cin>>x0;
    Button savegame=createButton(620,100,780,200,"存档",SaveGame);
    Button mainmenu=createButton(620,300,780,400,"主菜单",MainMenu);
    drawButton(savegame);drawButton(mainmenu);
    // if (x0==-1) SaveGame();
    // else if (x0==-2) MainMenu();
    //bool flag1=0;
    while (1)
    {
        ExMessage tmp=getmessage(EX_MOUSE);
        if (tmp.lbutton!=1)
        {   
            continue;
        }
        handleButtonClick(savegame, tmp.x, tmp.y);
        handleButtonClick(mainmenu, tmp.x, tmp.y);
        // cin>> y0 >> x1 >> y1 >> x2 >> y2;
        x0=tmp.x/SquareSize;y0=tmp.y/SquareSize;
        if (x0>7||y0>7) continue;
        if (ori_state.mp[x0][y0]!=-BotColor) continue;
        int centerX = x0 * SquareSize + SquareSize / 2;
        int centerY = y0 * SquareSize + SquareSize / 2;
        
        setfillcolor(RED);
        solidcircle(centerX, centerY, SquareSize*0.35);
        for (int k=0;k<8;++k)
        {
            int nx=x0;int ny=y0;
            while (1)
            {
                nx+=dx[k];ny+=dy[k];
                if (Check(ori_state,x0,y0,nx,ny,x0,y0,-BotColor))
                {
                    circle(nx*SquareSize+SquareSize/2,ny*SquareSize+SquareSize/2,SquareSize*0.35);
                }
                else break;
            }
        }
        while (1) 
        {
            ExMessage tmp2=getmessage(EX_MOUSE);if (tmp2.lbutton!=1) continue;
            x1=tmp2.x/SquareSize;y1=tmp2.y/SquareSize;
            if (x1>7||y1>7) continue;
            // int centerX = x1 * SquareSize + SquareSize / 2;
            // int centerY = y1 * SquareSize + SquareSize / 2;
            // setfillcolor(GREEN);
            // solidcircle(x1*SquareSize+SquareSize/2,y1*SquareSize+ quareSize/2,SquareSize*0.35);
            break;
        }
        for (int k=0;k<8;++k)
        {
            int nx=x0;int ny=y0;
            while (1)
            {
                nx+=dx[k];ny+=dy[k];
                if (Check(ori_state,x0,y0,nx,ny,x0,y0,-BotColor))
                {
                    clearcircle(nx*SquareSize+SquareSize/2,ny*SquareSize+SquareSize/2,SquareSize*0.40);
                }
                else break;
            }
        }
        setfillcolor(GREEN);
        solidcircle(x1*SquareSize+SquareSize/2,y1*SquareSize+SquareSize/2,SquareSize*0.35);
        for (int k=0;k<8;++k)
        {
            int nx=x1;int ny=y1;
            while (1)
            {
                nx+=dx[k];ny+=dy[k];
                if (Check(ori_state,x0,y0,x1,y1,nx,ny,-BotColor))
                {
                    circle(nx*SquareSize+SquareSize/2,ny*SquareSize+SquareSize/2,SquareSize*0.35);
                }
                else break;
            }
        }
        while (1) 
        {
            ExMessage tmp3=getmessage(EX_MOUSE);if (tmp3.lbutton!=1) continue;
            x2=tmp3.x/SquareSize;y2=tmp3.y/SquareSize;
            if (x2>7||y2>7) continue;
            int centerX = x2 * SquareSize + SquareSize / 2;
            int centerY = y2 * SquareSize + SquareSize / 2;
            setfillcolor(BLUE);
            solidcircle(centerX, centerY, SquareSize*0.35);
            break;
        }

        

        if (!Check(ori_state,x0,y0,x1,y1,x2,y2,-BotColor)) 
        {
            HumanConsider();
            return;
        }
        PlaceChess(ori_state,x0,y0,x1,y1,x2,y2,-BotColor);
        ofstream Now("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::app);
        Now<< x0 <<" "<< y0 <<" "<< x1 <<" "<< y1 <<" "<< x2 <<" "<< y2<<endl;
        Now.close();
        break;
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
    cleardevice();
    settextstyle(36,0,"宋体");
    RECT r={100,25,700,125};drawtext("设置AI难度：输入AI思考时间上限：",&r,DT_CENTER);
    r={100,150,700,250};drawtext("预设难度。简单请输入1；正常请输入3；困难请输入5：",&r,DT_CENTER|DT_WORDBREAK);
    //cin>>TimeLimit;
    char input[100]; 
    
    InputBox(input,50,"输入AI思考时间上限：\n预设难度。简单请输入1；正常请输入3；困难请输入5：");
    TimeLimit=atof(input);
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
void fb(){BotColor=-1;}
void fw(){BotColor=1;}
void NewGame()
{
    cleardevice();
    RECT r={100,100,700,300};drawtext("请选择您的先后手",&r,DT_CENTER);
    Button black=createButton(100,400,700,525,"黑方先手",fb);
    drawButton(black);
    Button white=createButton(100,550,700,675,"白方后手",fw);
    drawButton(white);
    Sleep(500);
    while (1)
    {
        ExMessage tmp=getmessage(EX_MOUSE);
        if (tmp.lbutton==1)
        {
            
            // 处理按钮点击
            handleButtonClick(black, tmp.x, tmp.y);
            handleButtonClick(white, tmp.x, tmp.y);
        }
        if (BotColor!=0) break;
    }
    //printf("请选择您的先后手：输入1选择黑方先手，2选择白方后手：");
    //int tmp;cin>>tmp;if (tmp==1) BotColor=-1;else BotColor=1;
    NowColor=1;
    ofstream fout("D:\\Code\\Amazon\\UIandBOT\\BotInput.in",ios::out); 
    if (BotColor==1) fout<<"1\n-1 -1 -1 -1 -1 -1\n";
    else fout<<"1\n";
    fout.close();
    BeginGame();
}
void MainMenu()
{
    
    initgraph(800,800);cleardevice();
    // system("chcp 65001");
    // drawtext("你好，EasyX！", 100, 100, RED);
    RECT r={100,150,300,250};//left;top;right;bottom;
    //drawtext(_T("test"),&r, DT_CENTER);

    settextstyle(48,0,"华文行楷");
    r={100,100,700,300};drawtext("        亚马逊棋        \n",&r, DT_CENTER);


    // r={0,625,200,650};drawtext("01.     加载存档        \n",&r, DT_CENTER);
    // r={0,650,200,675};drawtext("02.     新 对 局        \n",&r, DT_CENTER);
    // r={0,675,200,700};drawtext("03.     退出游戏        \n",&r, DT_CENTER);
    
    Button readsave=createButton(100,300,700,400,"加载存档",ReadSave);
    drawButton(readsave);
    Button newgame=createButton(100,425,700,525,"新对局",NewGame);
    drawButton(newgame);
    Button exitgame=createButton(100,550,700,650,"退出游戏",ExitGame);
    drawButton(exitgame);

    
    memset(ori_state.mp,0,sizeof ori_state.mp);
    ori_state.mp[0][2]=1;ori_state.mp[5][0]=1;ori_state.mp[2][0]=1;ori_state.mp[7][2]=1;
    ori_state.mp[0][5]=-1;ori_state.mp[2][7]=-1;ori_state.mp[5][7]=-1;ori_state.mp[7][5]=-1;
    totstep=1;turnID=0;
    while (1)
    {
        ExMessage tmp=getmessage(EX_MOUSE);
        if (tmp.lbutton==1)
        {
            
            // 处理按钮点击
            handleButtonClick(readsave, tmp.x, tmp.y);
            handleButtonClick(newgame, tmp.x, tmp.y);
            handleButtonClick(exitgame, tmp.x, tmp.y);
        }
    }
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