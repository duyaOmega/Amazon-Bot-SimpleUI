// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
// #pragma GCC optimize(1)
// #pragma GCC optimize(2)
// #pragma GCC optimize(3)
// #pragma GCC optimize("Ofast")
// #pragma GCC optimize("inline")
// #pragma GCC optimize("-fgcse")
// #pragma GCC optimize("-fgcse-lm")
// #pragma GCC optimize("-fipa-sra")
// #pragma GCC optimize("-ftree-pre")
// #pragma GCC optimize("-ftree-vrp")
// #pragma GCC optimize("-fpeephole2")
// #pragma GCC optimize("-ffast-math")
// #pragma GCC optimize("-fsched-spec")
// #pragma GCC optimize("unroll-loops")
// #pragma GCC optimize("-falign-jumps")
// #pragma GCC optimize("-falign-loops")
// #pragma GCC optimize("-falign-labels")
// #pragma GCC optimize("-fdevirtualize")
// #pragma GCC optimize("-fcaller-saves")
// #pragma GCC optimize("-fcrossjumping")
// #pragma GCC optimize("-fthread-jumps")
// #pragma GCC optimize("-funroll-loops")
// #pragma GCC optimize("-fwhole-program")
// #pragma GCC optimize("-freorder-blocks")
// #pragma GCC optimize("-fschedule-insns")
// #pragma GCC optimize("inline-functions")
// #pragma GCC optimize("-ftree-tail-merge")
// #pragma GCC optimize("-fschedule-insns2")
// #pragma GCC optimize("-fstrict-aliasing")
// #pragma GCC optimize("-fstrict-overflow")
// #pragma GCC optimize("-falign-functions")
// #pragma GCC optimize("-fcse-skip-blocks")
// #pragma GCC optimize("-fcse-follow-jumps")
// #pragma GCC optimize("-fsched-interblock")
// #pragma GCC optimize("-fpartial-inlining")
// #pragma GCC optimize("no-stack-protector")
// #pragma GCC optimize("-freorder-functions")
// #pragma GCC optimize("-findirect-inlining")
// #pragma GCC optimize("-fhoist-adjacent-loads")
// #pragma GCC optimize("-frerun-cse-after-loop")
// #pragma GCC optimize("inline-small-functions")
// #pragma GCC optimize("-finline-small-functions")
// #pragma GCC optimize("-ftree-switch-conversion")
// #pragma GCC optimize("-foptimize-sibling-calls")
// #pragma GCC optimize("-fexpensive-optimizations")
// #pragma GCC optimize("-funsafe-loop-optimizations")
// #pragma GCC optimize("inline-functions-called-once")
// #pragma GCC optimize("-fdelete-null-pointer-checks")
#include<bits/stdc++.h>
using namespace std;
const int Size=8;
int BotColor=-1;//black=1,white=-1;
int BeginTime;
struct Move
{
    int ori_x, ori_y,
    goal_x, goal_y,
    arr_x, arr_y;
};
struct Situation
{
    int mp[8][8];
};
void GetSituation();
int dx[8]={1,0,-1,0,1,1,-1,-1};
int dy[8]={0,1,0,-1,1,-1,1,-1};
int turnID;
inline void GenerateMove(vector<Move> &cur,const Situation &state,int color);
Situation bfs(Situation state,int color);
inline Situation CopyState(const Situation &s)
{
    Situation res;
    memcpy(res.mp, s.mp, sizeof(s.mp));
    return res;
}
// 新增：记录状态修改用于回溯
struct StateBackup 
{
    int ori_x, ori_y, goal_x, goal_y, arr_x, arr_y;
    int val_ori, val_goal, val_arr;
};
// 应用移动并备份状态
inline StateBackup ApplyMoveWithBackup(Situation &s, Move m, int color) 
{
    //cout<<m.ori_x<<" "<<m.ori_y<<endl;
    StateBackup bk;
    bk.ori_x = m.ori_x; bk.ori_y = m.ori_y;
    bk.goal_x = m.goal_x; bk.goal_y = m.goal_y;
    bk.arr_x = m.arr_x; bk.arr_y = m.arr_y;

    bk.val_ori = s.mp[bk.ori_x][bk.ori_y];
    bk.val_goal = s.mp[bk.goal_x][bk.goal_y];
    bk.val_arr = s.mp[bk.arr_x][bk.arr_y];

    s.mp[bk.ori_x][bk.ori_y] = 0;
    s.mp[bk.goal_x][bk.goal_y] = color;
    s.mp[bk.arr_x][bk.arr_y] = 2;
    return bk;
}
// 恢复状态
inline void RestoreMove(Situation &s, const StateBackup &bk) 
{
    s.mp[bk.ori_x][bk.ori_y] = bk.val_ori;
    s.mp[bk.goal_x][bk.goal_y] = bk.val_goal;
    s.mp[bk.arr_x][bk.arr_y] = bk.val_arr;
}

inline void PlaceChess(Situation &state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y,int color)
{
    state.mp[goal_x][goal_y]=color;state.mp[ori_x][ori_y]=0;state.mp[arr_x][arr_y]=2;
}

bool CheckRoad(const Situation &state,int ori_x,int ori_y,int goal_x,int goal_y)
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
bool Check(Situation &state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y,int color)
{
    if (state.mp[goal_x][goal_y]!=0 || ( (arr_x!=ori_x || arr_y!=ori_y) && state.mp[arr_x][arr_y]!=0) || state.mp[ori_x][ori_y]!=color ) return 0;
    if ((unsigned)goal_x >= Size || (unsigned)goal_y >= Size || (unsigned)arr_x >= Size || (unsigned)arr_y >= Size) return 0;
    state.mp[ori_x][ori_y]=0;
    if (!CheckRoad(state,ori_x,ori_y,goal_x,goal_y)) return 0;
    if (!CheckRoad(state,goal_x,goal_y,arr_x,arr_y)) return 0;
    state.mp[ori_x][ori_y]=color;
    return 1;
}
Situation ori_state;
double CalcValue(const Situation &state);
const int PreScore=10000;

#define EXACT 1
#define LOWERBOUND 2
#define UPPERBOUND 3
struct TTEntry {
    int depth;       // 搜索深度
    double score;    // 评估分数
    int flag;     // 分数类型
};

unordered_map<uint64_t, TTEntry> transpositionTable;
const int StateCount = 4; // 4种状态：0/1/-1/2
// 预生成Zobrist随机数表
uint64_t ZobristTable[Size][Size][StateCount];

// 初始化Zobrist表
void initZobrist() 
{
    mt19937_64 rng(20070702); 
    uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    for (int i = 0; i < Size; ++i)for (int j = 0; j < Size; ++j) 
    {
        for (int s = 0; s < StateCount; ++s)
        {
            ZobristTable[i][j][s] = dist(rng);
        }
    }
}
//hash计算，使用Zobrist哈希方法
inline uint64_t hashSituation(const Situation &s) 
{
    uint64_t hash = 0;
    for (int i = 0; i < Size; ++i)for (int j = 0; j < Size; ++j) 
    {
        int val = 0;
        if (s.mp[i][j] == 1)    val = 1;
        else if (s.mp[i][j] == -1) val = 2;
        else if (s.mp[i][j] == 2)  val = 3; 
        hash ^= ZobristTable[i][j][val];
    }
    return hash;
}
double TimeLimit;
int PriorChoose;int search_depth=2;
int max_move_count;
double AlphaBeta(Situation &s, int depth, double alpha, double beta, int color) 
{
    uint64_t hash = hashSituation(s);
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end())
    {
        TTEntry entry = it->second;
        // 若缓存的深度 >= 当前搜索深度，直接使用缓存结果
        if (entry.depth >= depth) 
        {
            if (entry.flag == EXACT) return entry.score;
            else if (entry.flag == LOWERBOUND) alpha = max(alpha, entry.score);
            else if (entry.flag == UPPERBOUND) beta = min(beta, entry.score);
            if (alpha >= beta) 
            {
                return entry.score;
            }
        }
    }

    if(depth == 0) 
    {
        return CalcValue(s);
    }
    if (clock()-BeginTime>TimeLimit*CLOCKS_PER_SEC) return CalcValue(s);
    //或许需要进一步调参卡时
    vector<Move> moves;
    moves.reserve(max_move_count);
    GenerateMove(moves,s,color);
    if(moves.empty()) 
    {
        // 无合法走法，当前颜色输
        return (color == BotColor) ? -PreScore : PreScore;
    }

    vector<pair<Move, double>> moves_with_score;
    moves_with_score.reserve(moves.size());
    for (auto &m : moves) 
    {
        StateBackup bk = ApplyMoveWithBackup(s, m, color);
        double score = CalcValue(s);
        RestoreMove(s, bk);
        moves_with_score.emplace_back(m, score);
    }
    //按分数排序，先搜看起来好的
    sort(moves_with_score.begin(), moves_with_score.end(),
        [](auto &a, auto &b) { return a.second < b.second; });
    
    double result;
    if(color == BotColor) //最大化
    { 
        double MaxValue = -PreScore;
        for(int i=moves_with_score.size()-1;i>=max(0,int(moves_with_score.size())-PriorChoose);--i) 
        //只搜最大30种
        {
            Move now = moves_with_score[i].first;
            StateBackup bk = ApplyMoveWithBackup(s, now, color);
            double value = AlphaBeta(s, depth-1, alpha, beta, -color);
            RestoreMove(s, bk);
            MaxValue = max(MaxValue, value);
            alpha = max(MaxValue, alpha);
            if(beta <= alpha) break;// Beta剪枝,对手能给出的最低得分（beta）≤当前最高得分（alpha），无需再遍历
            
        }
        result = MaxValue;
    } 
    else // 最小化
    { 
        double MinValue = PreScore;
        for(int i=0;i<min(int(moves_with_score.size()),PriorChoose);++i) 
        {
            Move now = moves_with_score[i].first;
            StateBackup bk = ApplyMoveWithBackup(s, now, color);
            double value = AlphaBeta(s, depth-1, alpha, beta, -color);
            RestoreMove(s, bk);
            MinValue = min(MinValue, value);
            beta = min(beta, MinValue);
            if(beta <= alpha) break;// Alpha剪枝,当前最低得分（beta）≤己方能接受的最高得分（alpha），无需再遍历
            
        }
        result = MinValue;
        
    }
    
    TTEntry newEntry;
    newEntry.depth = depth;
    newEntry.score = result;
    if (result <= alpha) {
        newEntry.flag = UPPERBOUND;
    } else if (result >= beta) {
        newEntry.flag = LOWERBOUND;
    } else {
        newEntry.flag = EXACT;
    }
    transpositionTable[hash] = newEntry;

    return result;
}


void Search()
{
    vector<Move> cur_move ;
    cur_move.reserve(2000);
    GenerateMove(cur_move,ori_state, BotColor);
    max_move_count=cur_move.size();
    Move best_move;
    if (turnID<=7) PriorChoose=min(30,(int)cur_move.size()/10);
    else if (turnID<=14) PriorChoose=min(15,(int)cur_move.size()/6);
    else PriorChoose=cur_move.size()/3;
    // PriorChoose=max(30,(int)cur_move.size()/5);
    vector<pair<Move, double>> moves_with_score;
    moves_with_score.reserve(cur_move.size());
    for (const auto& m : cur_move) {
        StateBackup bk = ApplyMoveWithBackup(ori_state, m, BotColor);
        // if (m.ori_x==6&&m.ori_y==2&&m.goal_x==4&&m.goal_y==0&&m.arr_x==5&&m.arr_y==1)
        //     TimeLimit+=0;
        double score = CalcValue(ori_state);
        RestoreMove(ori_state, bk);
        moves_with_score.emplace_back(m, score);
    }

    sort(moves_with_score.begin(), moves_with_score.end(),
        [](auto &a, auto &b) { return a.second < b.second; });
    double pre_best_score,best_score;Move pre_best_move;
    while (clock()-BeginTime<TimeLimit*CLOCKS_PER_SEC)
    {
        if (BotColor==-1) best_score=-PreScore;else best_score=PreScore;
        
        if (BotColor==-1)//white（最大化估价）
        {
            best_move=moves_with_score[moves_with_score.size()-1].first;
            for(int i=moves_with_score.size()-1;i>=max( 0, int(moves_with_score.size() )-PriorChoose  );--i) 
            {
                Move now=moves_with_score[i].first;
                StateBackup bk = ApplyMoveWithBackup(ori_state, now, BotColor);
                double score = AlphaBeta(ori_state, search_depth-1, -PreScore, PreScore, 1);
                RestoreMove(ori_state, bk);
                if(score>best_score)
                {
                    best_score=score;
                    best_move=now;
                }
                moves_with_score[i].second=score;
            }
            sort(moves_with_score.end()-min(PriorChoose,(int)moves_with_score.size()), moves_with_score.end(),
            [](auto &a, auto &b) { return a.second < b.second; });
        }
        else//black（最小化估价，因估价正值对白有利）
        {
            best_score=PreScore;
            best_move=moves_with_score[0].first;
            for(int i=0;i<min(int(moves_with_score.size()),PriorChoose);++i) 
            {
                Move now=moves_with_score[i].first;
                StateBackup bk = ApplyMoveWithBackup(ori_state, now, BotColor);
                double score = AlphaBeta(ori_state, search_depth-1, -PreScore, PreScore, -1);
                RestoreMove(ori_state, bk);
                if(score<best_score)
                {
                    best_score=score;
                    best_move=now;
                }
                moves_with_score[i].second=score;
                // if (now.ori_x==6&&now.ori_y==2&&now.goal_x==4&&now.goal_y==0&&now.arr_x==5&&now.arr_y==1) cout<<score<<endl;
            }
            sort(moves_with_score.begin(), moves_with_score.begin()+min(PriorChoose,(int)moves_with_score.size()),
            [](auto &a, auto &b) { return a.second < b.second; });
            //cout<<search_depth<<endl;
            
        }
        if (clock()-BeginTime<TimeLimit*CLOCKS_PER_SEC)
        {
            pre_best_score=best_score;
            pre_best_move=best_move;
        }
        ++search_depth;
    }
    // cout<<search_depth<<"    "<<cur_move.size()<<"    "<<clock()-BeginTime<<"     "<<best_score<<"    "<<pre_best_score<<endl;
    printf("%d %d %d %d %d %d\n",pre_best_move.ori_x,pre_best_move.ori_y,pre_best_move.goal_x,
        pre_best_move.goal_y,pre_best_move.arr_x,pre_best_move.arr_y);
}
bool Mirrorok=1;
void Mirror(Situation state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y)
{
    int nx=7-ori_x,ny=7-ori_y,ngx=7-goal_x,ngy=7-goal_y,nax=7-arr_x,nay=7-arr_y;
    if (Mirrorok && turnID<=4 && BotColor==-1 && Check(state,nx,ny,ngx,ngy,nax,nay,BotColor) )
    {
        printf("%d %d %d %d %d %d\n",nx,ny,ngx,ngy,nax,nay);
    }
    //如果可以，总是尝试与对手对称走棋，这样是严格不劣的。仅在白方使用。减少前期运算。
    //现在更改为仅在前4回合考虑对称
    //否则，使用随机。
    //现在升级成使用Alpha-Beta搜索。
    else
    {
        Search();
    }
}
int main()
{
    ori_state.mp[0][2]=1;ori_state.mp[5][0]=1;ori_state.mp[2][0]=1;ori_state.mp[7][2]=1;
    ori_state.mp[0][5]=-1;ori_state.mp[2][7]=-1;ori_state.mp[5][7]=-1;ori_state.mp[7][5]=-1;
    vector<Move> W1;GenerateMove(W1,ori_state,1);
    //freopen("Data.txt","w",stdout);
    turnID=1;BotColor=1;
    TimeLimit=1;
    //cout<<W1.size();
    vector<pair<Move,double>>moves_with_score;
    for (auto mw :W1)
    {
        StateBackup bk = ApplyMoveWithBackup(ori_state, mw, 1);
        //printf("%d %d %d %d %d %d\n",mw.ori_x,mw.ori_y,mw.goal_x,mw.goal_y,mw.arr_x,mw.arr_y);
        double score = CalcValue(ori_state);
        RestoreMove(ori_state, bk);
        moves_with_score.emplace_back(mw, score);
    }
    sort(moves_with_score.begin(), moves_with_score.end(),
        [](auto &a, auto &b) { return a.second < b.second; });
    ofstream File0("Data2.txt",ios::out);File0.close();
    // for (int i=moves_with_score.size()-1;i>=max((int)moves_with_score.size()-50,0);--i)
    for (int i=moves_with_score.size()-1;i>=0;--i)
    {
        Move mw=moves_with_score[i].first;
        StateBackup bk = ApplyMoveWithBackup(ori_state, mw, 1);
        //printf("%d %d %d %d %d %d\n",mw.ori_x,mw.ori_y,mw.goal_x,mw.goal_y,mw.arr_x,mw.arr_y);
        // BeginTime=clock();
        // Search();
        ofstream File("BotInput.in",ios::out);
        File<<"1\n"<<mw.ori_x<<" "<<mw.ori_y<<" "<<mw.goal_x<<" "<<mw.goal_y<<" "<<mw.arr_x<<" "<<mw.arr_y<<endl;
        File.close();
        system("Amazon-Bot.exe");
        ifstream File2("BotOutput.out",ios::in);
        int x0,y0,x1,y1,x2,y2;
        File2>>x0>>y0>>x1>>y1>>x2>>y2;
        File2.close();
        ofstream File3("Data2.txt",ios::app);
        File3<<mw.ori_x<<" "<<mw.ori_y<<" "<<mw.goal_x<<" "<<mw.goal_y<<" "<<mw.arr_x<<" "<<mw.arr_y<<endl;
        File3<<x0<<" "<<y0<<" "<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
        RestoreMove(ori_state, bk);
        File3.close();
    }
    
    return 0;
}

void GetSituation()
{
    BotColor=-1;
    cin>>turnID;
    int x0,y0,x1,y1,x2,y2;
    int ox0,oy0,ox1,oy1,ox2,oy2;
	for (int i=1;i<=turnID;++i)
	{
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			BotColor = 1;
		else
        {
			PlaceChess(ori_state,x0, y0, x1, y1, x2, y2,-BotColor);
            ox0=x0;oy0=y0;ox1=x1;oy1=y1;ox2=x2;oy2=y2;
        }
        if (i==turnID) Mirror(ori_state,x0, y0, x1, y1, x2, y2);
		if (i <= turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
				PlaceChess(ori_state,x0, y0, x1, y1, x2, y2, BotColor);
            if (ox0!=7-x0 || oy0!=7-y0 || ox1!=7-x1 || oy1!=7-y1 || ox2!=7-x2 || oy2!=7-y2) Mirrorok=0;
		}
	}
}
inline void GenerateMove(vector<Move> &cur,const Situation &state,int color)
{
    // vector<Move> cur;
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
                if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny] != 0)  break;
                for (int j=0;j<8;++j)
                {
                    int ax=nx,ay=ny;
                    while (true)
                    {
                        ax+=dx[j],ay+=dy[j];
                        if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y) )) break;
                        cur.emplace_back(Move{ori_x,ori_y,nx,ny,ax,ay});
                    }
                }
            }
        }
    }
    // return cur;
}
// 优化BFS：使用静态数组队列替代queue
inline Situation CalcQueenMove(int color, const Situation &state)
{
    Situation res;
    memset(res.mp,0x3f,sizeof res.mp);
    static int qx[64], qy[64]; 
    int head=0, tail=0;
    for (int x = 0; x < Size; ++x)for (int y = 0; y < Size; ++y)
    {
        if (state.mp[x][y] == color)
        {
            qx[tail] = x;
            qy[tail++] = y;
            res.mp[x][y] = 0;
        }
    }
    // BFS计算Queen走法可达距离
    while (head < tail)
    {
        int x=qx[head], y=qy[head++];
        for (int i=0;i<8;++i) 
        {
            int nx=x, ny=y;
            while (true)
            {
                nx += dx[i];
                ny += dy[i];
                if ((unsigned)nx >= Size || (unsigned)ny >= Size ||state.mp[nx][ny]!=0) break;
                if (res.mp[nx][ny] > res.mp[x][y] + 1)
                {
                    res.mp[nx][ny] = res.mp[x][y] + 1;
                    qx[tail] = nx;
                    qy[tail++] = ny;
                }
            }
        }
    }
    return res;
}

inline Situation CalcKingMove(int color, const Situation &state)
{
    Situation res;
    memset(res.mp,0x3f,sizeof res.mp);
    static int qx[64], qy[64];
    int head=0, tail=0;
    for (int x = 0; x < Size; ++x)for (int y = 0; y < Size; ++y)
    {
        if (state.mp[x][y] == color)
        {
            qx[tail] = x;
            qy[tail++] = y;
            res.mp[x][y] = 0;
        }
    }
    // BFS计算King走法可达距离
    while (head < tail)
    {
        int x=qx[head], y=qy[head++];
        for (int i=0;i<8;++i) 
        {
            int nx=x+dx[i], ny=y+dy[i];
            if ((unsigned)nx >= Size || (unsigned)ny >= Size ||state.mp[nx][ny]!=0) continue;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
    }
    return res;
}

// 新增：预计算空白格子移动性
inline void CalcBlankMobility(const Situation &state, int (&blankMobility)[8][8]) 
{
    memset(blankMobility,0, sizeof(blankMobility));
    for (int i = 0; i < Size; ++i) for (int j = 0; j < Size; ++j) 
    {
        if (state.mp[i][j] == 0) 
        {
            for (int k = 0; k < 8; ++k) 
            {
                int nx = i + dx[k];
                int ny = j + dy[k];
                if (nx >= 0 && nx < Size && ny >= 0 && ny < Size && state.mp[nx][ny] == 0) ++blankMobility[i][j];
            }
        }
    }

}

inline int CalcMobility(int x,int y,const Situation &state)
{
    int res[8][8];
    memset(res, -1, sizeof(res));
    static int qx[64], qy[64];
    int head=0, tail=0;
    qx[tail] = x;
    qy[tail++] = y;
    res[x][y] = 0;
    
    while (head < tail)
    {
        int cx = qx[head], cy = qy[head++];
        for (int i=0;i<8;++i) 
        {
            int nx = cx + dx[i], ny = cy + dy[i];
            if ((unsigned)nx >= Size || (unsigned)ny >= Size ||state.mp[nx][ny]!=0) break;
            if (res[nx][ny] == -1)
            {
                res[nx][ny] = res[cx][cy] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
    }
    
    int blankMobility[8][8];
    CalcBlankMobility(state, blankMobility);
    
    int retmob = 0;
    for (int i=0;i<8;++i)
    {
        int nx=x, ny=y;
        while (true)
        {
            nx += dx[i];
            ny += dy[i];
            if ((unsigned)nx >= Size || (unsigned)ny >= Size ||state.mp[nx][ny]!=0) break;
            if (res[nx][ny] > 0) {
                retmob += blankMobility[nx][ny] / res[nx][ny];
            }
        }
    }
    return retmob;
}

inline double CalcValue(const Situation &state)//估价函数，令正值表示白有利
{
    // 基于回合数划分阶段，参考郭琴琴论文《亚马逊棋机器博弈系统中评估函数的研究》，并在8*8棋盘适当减少，且进行调参
    double a, b, c, d, e;
    //经过多次实验，发现本bot在中残局性能下滑严重，考虑调整参数至更注重灵活度
    //算了还不如按论文的来
    if (turnID <= 7) 
    {
        a = 0.14; b = 0.37; c = 0.13; d = 0.13; e = 0.20;
    } 
    else if (turnID <= 14) 
    {
        a = 0.30; b = 0.25; c = 0.20; d = 0.20; e = 0.05;
    } 
    else 
    {
        a = 0.80; b = 0.10; c = 0.05; d = 0.05; e = 0.00;
    }
    double k=(turnID&1)?0.3:-0.3;// 先行方优势
    // 计算双方QueenMove/KingMove矩阵
    Situation WhiteQM = CalcQueenMove(-1,state),
     BlackQM = CalcQueenMove(1,state);
    Situation WhiteKM = CalcKingMove(-1,state),
     BlackKM = CalcKingMove(1,state);
    // 计算territory特征值t1(Queen)、t2(King)（空格控制权评估）
    double t1 = 0.0, t2 = 0.0;
    for (int x = 0; x < Size; ++x) for (int y = 0; y < Size; ++y) 
    {
        if (state.mp[x][y] != 0) continue;
        // t1：Queen走法控制权
        int wqm = WhiteQM.mp[x][y], bqm = BlackQM.mp[x][y];
        if (wqm != 0x3f3f3f3f && bqm != 0x3f3f3f3f) 
            t1 += (wqm==bqm) ? k : ( (wqm < bqm)?1:-1 );
        else if (wqm != 0x3f3f3f3f) t1 += 1.0;
        else if (bqm != 0x3f3f3f3f) t1 -= 1.0;
        // t2：King走法控制权
        int wkm = WhiteKM.mp[x][y], bkm = BlackKM.mp[x][y];
        if (wkm != 0x3f3f3f3f && bkm != 0x3f3f3f3f) 
            t2 += (wkm==bkm) ? k : ( (wkm<bkm)?1:-1 );
        else if (wkm != 0x3f3f3f3f) t2 += 1.0;
        else if (bkm != 0x3f3f3f3f) t2 -= 1.0;
    }
    // 计算position特征值p1(Queen)、p2(King)
    double p1 = 0.0, p2 = 0.0;
    for (int x = 0; x < Size; ++x) for (int y = 0; y < Size; ++y) 
    {
        if (state.mp[x][y] != 0) continue;
        // p1：Queen走法位置差
        int wqm = WhiteQM.mp[x][y], bqm = BlackQM.mp[x][y];
        if (wqm != 0x3f3f3f3f && bqm != 0x3f3f3f3f)
            p1 += 2*(pow(2,-wqm) - pow(2,-bqm));
        // p2：King走法位置差
        int wkm = WhiteKM.mp[x][y], bkm = BlackKM.mp[x][y];
        if (wkm != 0x3f3f3f3f && bkm != 0x3f3f3f3f)
            p2 += min(1.0, max(-1.0,(bkm - wkm)/6.0) );
    }

    // 计算mobility特征值m
    int WhiteMobility = 0, BlackMobility = 0;
    int WhiteMinMobility = 0x3f3f3f3f, BlackMinMobility = 0x3f3f3f3f;
    for (int x = 0; x < Size; ++x) for (int y = 0; y < Size; ++y) 
    {
        if (state.mp[x][y] == -1) 
        { 
            int tmp=CalcMobility(x,y,state);
            WhiteMobility += tmp;
            WhiteMinMobility = min(WhiteMinMobility, tmp);
        }
        else if (state.mp[x][y] == 1) 
        { 
            int tmp=CalcMobility(x,y,state);
            BlackMobility += tmp;
            BlackMinMobility = min(BlackMinMobility, tmp);
        }
    }
    double m = (WhiteMobility - BlackMobility) - 0.5*(WhiteMinMobility - BlackMinMobility);
    // 分阶段加权计算总估值
    double total = a * t1 + b * t2 + c * p1 + d * p2 + e * m;
    return total;
}