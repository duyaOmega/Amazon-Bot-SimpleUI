#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize(1)
#pragma GCC optimize(2)
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("-fgcse")
#pragma GCC optimize("-fgcse-lm")
#pragma GCC optimize("-fipa-sra")
#pragma GCC optimize("-ftree-pre")
#pragma GCC optimize("-ftree-vrp")
#pragma GCC optimize("-fpeephole2")
#pragma GCC optimize("-ffast-math")
#pragma GCC optimize("-fsched-spec")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("-falign-jumps")
#pragma GCC optimize("-falign-loops")
#pragma GCC optimize("-falign-labels")
#pragma GCC optimize("-fdevirtualize")
#pragma GCC optimize("-fcaller-saves")
#pragma GCC optimize("-fcrossjumping")
#pragma GCC optimize("-fthread-jumps")
#pragma GCC optimize("-funroll-loops")
//#pragma GCC optimize("-fwhole-program")
#pragma GCC optimize("-freorder-blocks")
#pragma GCC optimize("-fschedule-insns")
#pragma GCC optimize("inline-functions")
#pragma GCC optimize("-ftree-tail-merge")
#pragma GCC optimize("-fschedule-insns2")
#pragma GCC optimize("-fstrict-aliasing")
//#pragma GCC optimize("-fstrict-overflow")
#pragma GCC optimize("-falign-functions")
//#pragma GCC optimize("-fcse-skip-blocks")
#pragma GCC optimize("-fcse-follow-jumps")
#pragma GCC optimize("-fsched-interblock")
#pragma GCC optimize("-fpartial-inlining")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("-freorder-functions")
#pragma GCC optimize("-findirect-inlining")
#pragma GCC optimize("-fhoist-adjacent-loads")
#pragma GCC optimize("-frerun-cse-after-loop")
#pragma GCC optimize("inline-small-functions")
#pragma GCC optimize("-finline-small-functions")
#pragma GCC optimize("-ftree-switch-conversion")
#pragma GCC optimize("-foptimize-sibling-calls")
#pragma GCC optimize("-fexpensive-optimizations")
//#pragma GCC optimize("-funsafe-loop-optimizations")
#pragma GCC optimize("inline-functions-called-once")
#pragma GCC optimize("-fdelete-null-pointer-checks")
//#pragma GCC optimize("-flto")  // 链接时优化
//#pragma GCC optimize("-flto-partition=none")  // LTO分区策略
//#pragma GCC optimize("-fvisibility=hidden")  // 默认隐藏符号
//#pragma GCC optimize("-fPIC")  // 位置无关代码  
#pragma GCC optimize("-fomit-frame-pointer")  // 省略帧指针
//#pragma GCC optimize("-fmerge-all-constants")  // 合并所有常量
#pragma GCC optimize("-fno-stack-protector")  // 禁用栈保护（已在前面启用）
//#pragma GCC optimize("-march=native")  // 本地架构优化
#include<iostream>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<ctime>
#include<random>
#include<queue>
#include<cstring>
using namespace std;
constexpr int Size=8;
int BotColor=-1;//black=1,white=-1;
#define Black 1
#define White -1
int BeginTime;
struct __attribute__((packed)) Move
{
    char ori_x, ori_y,
         goal_x, goal_y,
         arr_x, arr_y;
};
// 对齐优化：确保结构体内存对齐以提高访问速度
struct alignas(32) Situation
{
    signed char mp[8][8];
};
void GetSituation();
constexpr int dx[8]={1,0,-1,0,1,1,-1,-1};
constexpr int dy[8]={0,1,0,-1,1,-1,1,-1};
int turnID;
inline void GenerateMove(vector<Move> &cur,const Situation &state,int color);
Situation bfs(Situation state,int color);
// 使用aligned_alloc优化内存拷贝
inline Situation CopyState(const Situation &s)
{
    Situation res;
    // 使用编译器内置函数进行快速内存拷贝
    __builtin_memcpy(res.mp, s.mp, sizeof(s.mp));
    return res;
}
// 新增：记录状态修改用于回溯
// 使用packed属性优化内存布局
struct __attribute__((packed)) StateBackup 
{
    char ori_x, ori_y, goal_x, goal_y, arr_x, arr_y;
    char val_ori, val_goal, val_arr;
};
// 应用移动并备份状态 - 优化内存访问顺序
inline StateBackup ApplyMoveWithBackup(Situation &s, Move m, int color) 
{
    StateBackup bk;
    // 批量赋值优化
    bk.ori_x = m.ori_x; bk.ori_y = m.ori_y;
    bk.goal_x = m.goal_x; bk.goal_y = m.goal_y;
    bk.arr_x = m.arr_x; bk.arr_y = m.arr_y;

    // 批量读取优化
    bk.val_ori = s.mp[bk.ori_x][bk.ori_y];
    bk.val_goal = s.mp[bk.goal_x][bk.goal_y];
    bk.val_arr = s.mp[bk.arr_x][bk.arr_y];

    // 批量写入优化
    s.mp[bk.ori_x][bk.ori_y] = 0;
    s.mp[bk.goal_x][bk.goal_y] = static_cast<signed char>(color);
    s.mp[bk.arr_x][bk.arr_y] = 2;
    return bk;
}
// 恢复状态 - 优化内存访问顺序
inline void RestoreMove(Situation &s, const StateBackup &bk) 
{
    // 批量写入优化以提高缓存命中率
    s.mp[bk.ori_x][bk.ori_y] = bk.val_ori;
    s.mp[bk.goal_x][bk.goal_y] = bk.val_goal;
    s.mp[bk.arr_x][bk.arr_y] = bk.val_arr;
}

// 优化内存访问顺序以提高缓存命中率
inline void PlaceChess(Situation &state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y,int color)
{
    // 按照内存访问顺序重新排列以提高缓存局部性
    state.mp[ori_x][ori_y] = 0;
    state.mp[goal_x][goal_y] = static_cast<signed char>(color);
    state.mp[arr_x][arr_y] = 2;
}

bool CheckRoad(const Situation &state,int ori_x,int ori_y,int goal_x,int goal_y)
{
    if (goal_x==ori_x&&goal_y==ori_y) return false;
    
    // 位运算优化：使用位运算代替条件判断
    const bool same_row = (goal_x == ori_x);
    const bool same_col = (goal_y == ori_y);
    
    if (same_row) 
    {
        const int start = (ori_y < goal_y) ? ori_y : goal_y;
        const int end = (ori_y > goal_y) ? ori_y : goal_y;
        for (short j=start; j<=end; ++j) 
            if (state.mp[ori_x][j] != 0) return false;
    }
    else if (same_col) 
    {
        const int start = (ori_x < goal_x) ? ori_x : goal_x;
        const int end = (ori_x > goal_x) ? ori_x : goal_x;
        for (short j=start; j<=end; ++j) 
            if (state.mp[j][ori_y] != 0) return false;
    }
    else 
    {
        const int dx = goal_x - ori_x;
        const int dy = goal_y - ori_y;
        
        // 使用位运算优化绝对值比较
        const int abs_dx = (dx ^ (dx >> 31)) - (dx >> 31);
        const int abs_dy = (dy ^ (dy >> 31)) - (dy >> 31);
        
        if (abs_dx != abs_dy) return false;
        
        const int fhx = (dx >> 31) | 1;  // 取符号位并设置为1或-1
        const int fhy = (dy >> 31) | 1;
        
        int x = ori_x, y = ori_y;
        while (x != goal_x)
        {
            x += fhx; y += fhy;
            if (state.mp[x][y] != 0) return false;
        }
    }
    return true;
}
bool Check(Situation &state,int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y,int color)
{
    // 边界检查优化：提前进行边界检查
    if ((unsigned)goal_x >= Size || (unsigned)goal_y >= Size || 
        (unsigned)arr_x >= Size || (unsigned)arr_y >= Size) return false;
    
    // 条件检查优化：重新排列条件以尽早返回
    if (state.mp[ori_x][ori_y] != color) return false;
    if (state.mp[goal_x][goal_y] != 0) return false;
    
    // 箭头位置检查优化
    if ((arr_x != ori_x || arr_y != ori_y) && state.mp[arr_x][arr_y] != 0) return false;
    
    // 保存原始值以恢复状态
    const char original_value = state.mp[ori_x][ori_y];
    state.mp[ori_x][ori_y] = 0;
    
    // 路径检查优化
    const bool road1_ok = CheckRoad(state, ori_x, ori_y, goal_x, goal_y);
    if (!road1_ok) {
        state.mp[ori_x][ori_y] = original_value;
        return false;
    }
    
    const bool road2_ok = CheckRoad(state, goal_x, goal_y, arr_x, arr_y);
    state.mp[ori_x][ori_y] = original_value;  // 恢复原始状态
    
    return road2_ok;
}
Situation ori_state;
double CalcValue(const Situation &state,const int color);
constexpr int PreScore=10000;

#define EXACT 1
#define LOWERBOUND 2
#define UPPERBOUND 3
// 使用packed属性优化内存布局，提高缓存效率
struct __attribute__((packed)) TTEntry {
    int depth;       // 搜索深度
    double score;    // 评估分数
    int flag;        // 分数类型
};

unordered_map<uint64_t, TTEntry> transpositionTable;
const int StateCount = 4; // 4种状态：0/1/-1/2
// 预生成Zobrist随机数表
uint64_t ZobristTable[Size][Size][StateCount];

// 初始化Zobrist表 - 循环展开优化
void initZobrist() 
{
    mt19937_64 rng(20070702); 
    uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    for (short i = 0; i < Size; ++i) {
        for (short j = 0; j < (Size>>1); ++j) 
        {
            // 展开StateCount循环
            ZobristTable[i][j][0] = ZobristTable[i][Size-j-1][0] =dist(rng);
            ZobristTable[i][j][1] = ZobristTable[i][Size-j-1][1] =dist(rng);
            ZobristTable[i][j][2] = ZobristTable[i][Size-j-1][2] =dist(rng);
            ZobristTable[i][j][3] = ZobristTable[i][Size-j-1][3] =dist(rng);
        }
    }
}
//hash计算，使用Zobrist哈希方法 - 优化条件判断
inline uint64_t hashSituation(const Situation &s) 
{
    uint64_t hash = 0;
    for (short i = 0; i < Size; ++i) {
        for (short j = 0; j < Size; ++j) 
        {
            // 使用switch代替多重if-else提升性能
            switch (s.mp[i][j]) {
                case 1:  hash ^= ZobristTable[i][j][1]; break;
                case -1: hash ^= ZobristTable[i][j][2]; break;
                case 2:  hash ^= ZobristTable[i][j][3]; break;
                default: hash ^= ZobristTable[i][j][0]; break; // 0的情况
            }
        }
    }
    return hash;
}
double TimeLimit=0.98;
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
        return CalcValue(s,-color);//因为此时本方并没有走棋，根据定义应该传入-Color
    }
    
    // 时间控制优化：提前计算时间限制
    const clock_t time_limit = TimeLimit*CLOCKS_PER_SEC;
    if (clock()-BeginTime > time_limit) return CalcValue(s,-color);
    
    //或许需要进一步调参卡时
    vector<Move> moves;
    moves.reserve(max_move_count);
    GenerateMove(moves,s,color);
    if(moves.empty()) 
    {
        // 无合法走法，当前颜色输
        return (color == Black) ? PreScore : -PreScore;
    }

    const short move_count = moves.size();
    vector<pair<Move, double>> moves_with_score;
    moves_with_score.reserve(move_count);
    
    // 评估函数优化：避免重复计算
    for (short i = 0; i < move_count; ++i) 
    {
        StateBackup bk = ApplyMoveWithBackup(s, moves[i], color);
        double score = CalcValue(s,color);
        RestoreMove(s, bk);
        moves_with_score.emplace_back(moves[i], score);
    }
    
    //按分数排序，先搜看起来好的
    sort(moves_with_score.begin(), moves_with_score.end(),
        [](const pair<Move, double> &a, const pair<Move, double> &b) { return a.second < b.second; });
    
    double result;
    //if((color == BotColor && BotColor==-1)||(color!=BotColor&&BotColor==1)) //最大化
    if (color==White)
    { 
        double MaxValue = -PreScore;
        const int start_idx = static_cast<int>(move_count) - 1;
        const int end_idx = max(0, static_cast<int>(move_count) - PriorChoose);
        
        for (short i=start_idx; i>=end_idx; --i) 
        //只搜最大PriorChoose种
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
    else // 最小化,color==Black
    { 
        double MinValue = PreScore;
        const int end_idx = min(static_cast<int>(move_count), PriorChoose);
        
        for (short i=0; i<end_idx; ++i) 
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
    
    // 转置表更新优化：减少不必要的拷贝
    TTEntry &entry = transpositionTable[hash]; // 直接引用或创建
    entry.depth = depth;
    entry.score = result;
    if (result <= alpha) {
        entry.flag = UPPERBOUND;
    } else if (result >= beta) {
        entry.flag = LOWERBOUND;
    } else {
        entry.flag = EXACT;
    }

    return result;
}

void Search()
{
    if (turnID==1 && BotColor==1){ printf("2 0 2 5 5 5"); return; }
    vector<Move> cur_move ;
    cur_move.reserve(2000);
    GenerateMove(cur_move,ori_state, BotColor);
    max_move_count=cur_move.size();
    
    // 提前计算大小，避免重复调用size()
    const short move_count = cur_move.size();
    
    Move best_move;
    if (turnID<=7) PriorChoose=min(16,static_cast<int>(move_count)/15);
    else if (turnID<=14) PriorChoose=min(12,static_cast<int>(move_count)/8);
    else PriorChoose=static_cast<int>(move_count)/5;
    
    if (PriorChoose<6) PriorChoose=6;
    if (PriorChoose>move_count) PriorChoose=move_count;
    // 预分配内存优化
    vector<pair<Move, double>> moves_with_score;
    moves_with_score.reserve(move_count);
    
    // 评估函数优化
    for (short i = 0; i < move_count; ++i) {
        StateBackup bk = ApplyMoveWithBackup(ori_state, cur_move[i], BotColor);
        double score = CalcValue(ori_state,BotColor);
        RestoreMove(ori_state, bk);
        moves_with_score.emplace_back(cur_move[i], score);
    }

    // 排序优化：使用const引用
    sort(moves_with_score.begin(), moves_with_score.end(),
        [](const pair<Move, double> &a, const pair<Move, double> &b) { return a.second < b.second; });
    
    double pre_best_score,best_score;Move pre_best_move;
    
    // 时间限制
    const clock_t time_limit = static_cast<clock_t>(TimeLimit*CLOCKS_PER_SEC);
    while (clock()-BeginTime < time_limit)
    {
        if (BotColor==-1) best_score=-PreScore;else best_score=PreScore;
        
        if (BotColor==-1)//white（最大化估价）
        {
            const size_t last_idx = move_count - 1;
            best_move=moves_with_score[last_idx].first;
            
            const int start_idx = static_cast<int>(move_count)-1;
            const int end_idx = max(0, static_cast<int>(move_count)-PriorChoose);
            
            // 单线程搜索
            for (short i=start_idx; i>=end_idx; --i) 
            {
                Move now=moves_with_score[i].first;
                StateBackup bk = ApplyMoveWithBackup(ori_state, now, BotColor);
                double score = AlphaBeta(ori_state, search_depth-1, -PreScore, PreScore, 1);
                RestoreMove(ori_state, bk);
                if(score>best_score)
                {
                    best_score=score;
                    best_move=now;
                    // cout<<score<<endl;
                }
                moves_with_score[i].second=score;
                // printf("%d %d %d %d %d %d\n",now.ori_x,now.ori_y,now.goal_x,now.goal_y,now.arr_x,now.arr_y);
                // cout<<score<<endl;
            }
            
            const int sort_start = static_cast<int>(move_count) - min(PriorChoose, static_cast<int>(move_count));
            sort(moves_with_score.begin() + sort_start, moves_with_score.end(),
            [](const pair<Move, double> &a, const pair<Move, double> &b) { return a.second < b.second; });
        }
        else//black（最小化估价，因估价正值对白有利）
        {
            best_score=PreScore;
            best_move=moves_with_score[0].first;
            
            const int end_idx = min(static_cast<int>(move_count), PriorChoose);
            
            // 单线程搜索
            for (short i=0; i<end_idx; ++i) 
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
                // printf("%d %d %d %d %d %d\n",now.ori_x,now.ori_y,now.goal_x,now.goal_y,now.arr_x,now.arr_y);
                // cout<<score<<endl;

                moves_with_score[i].second=score;

                // if (now.ori_x==2&&now.ori_y==0&&now.goal_x==4&&now.goal_y==0&&now.arr_x==2&&now.arr_y==0)
                // {
                //     cout<<score<<endl;
                // }
            }
            
            const int sort_end = min(PriorChoose, static_cast<int>(move_count));
            sort(moves_with_score.begin(), moves_with_score.begin()+sort_end,
            [](const pair<Move, double> &a, const pair<Move, double> &b) { return a.second < b.second; });
        }
        
        if (clock()-BeginTime < time_limit)
        {
            pre_best_score=best_score;
            pre_best_move=best_move;
        }
        ++search_depth;
    }
    
    printf("%d %d %d %d %d %d\n",pre_best_move.ori_x,pre_best_move.ori_y,pre_best_move.goal_x,pre_best_move.goal_y,pre_best_move.arr_x,pre_best_move.arr_y);
    
    //debug
    cout<<search_depth<<" "<<cur_move.size()<<" "<<PriorChoose<<" "<<clock()-BeginTime<<" "<<best_score<<" "<<pre_best_score<<endl;
}
bool Mirrorok=1;
void Mirror(int ori_x,int ori_y,int goal_x,int goal_y,int arr_x,int arr_y)
{
    int nx=7-ori_x,ny=7-ori_y,ngx=7-goal_x,ngy=7-goal_y,nax=7-arr_x,nay=7-arr_y;
    // if (Mirrorok && turnID<=4 && BotColor==-1 && Check(state,nx,ny,ngx,ngy,nax,nay,BotColor) )
    // {
    //     printf("%d %d %d %d %d %d\n",nx,ny,ngx,ngy,nax,nay);
    // }
    //如果可以，总是尝试与对手对称走棋，这样是严格不劣的。仅在白方使用。减少前期运算。
    //现在更改为仅在前4回合考虑对称
    //否则，使用随机。
    //现在升级成使用Alpha-Beta搜索。
    // else 
    if (turnID==2 && BotColor==1)
    {
        freopen("./data/Data.txt","r",stdin);
        for (short i=1;i<=50;++i)
        {
            cin>>nx>>ny>>ngx>>ngy>>nax>>nay;
            if (ori_x==nx&&ori_y==ny&&goal_x==ngx&&goal_y==ngy&&arr_x==nax&&arr_y==nay)
            {
                cin>>nx>>ny>>ngx>>ngy>>nax>>nay;
                printf("%d %d %d %d %d %d\n",nx,ny,ngx,ngy,nax,nay);
                return;
            }
            else cin>>nx>>ny>>ngx>>ngy>>nax>>nay;
        }
        Search();
    }
    else if (turnID==1 && BotColor==-1)
    {
        freopen("./data/Data2.txt","r",stdin);
        for (short i=1;i<=50;++i)
        {
            cin>>nx>>ny>>ngx>>ngy>>nax>>nay;
            if (ori_x==nx&&ori_y==ny&&goal_x==ngx&&goal_y==ngy&&arr_x==nax&&arr_y==nay)
            {
                cin>>nx>>ny>>ngx>>ngy>>nax>>nay;
                printf("%d %d %d %d %d %d\n",nx,ny,ngx,ngy,nax,nay);
                return;
            }
            else cin>>nx>>ny>>ngx>>ngy>>nax>>nay;
        }
        Search();
    }
    else
    {
        Search();
    }
}

int main()
{
    BeginTime=clock();
    srand(time(0));
    initZobrist();
    ori_state.mp[0][2]=1;ori_state.mp[5][0]=1;ori_state.mp[2][0]=1;ori_state.mp[7][2]=1;
    ori_state.mp[0][5]=-1;ori_state.mp[2][7]=-1;ori_state.mp[5][7]=-1;ori_state.mp[7][5]=-1;
    // for (int i=0;i<64;++i) pow_table[i]=1.0/(1<<i);
    GetSituation();
    
    return 0;
}
//int totstep=0;
void GetSituation()
{
    BotColor=-1;
    cin>>turnID;
    if (turnID==1) TimeLimit=1.98;
    int x0,y0,x1,y1,x2,y2;
    //int ox0,oy0,ox1,oy1,ox2,oy2;
	for (short i=1;i<=turnID;++i)
	{
		cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
		if (x0 == -1)
			BotColor = 1;
		else
        {
			PlaceChess(ori_state,x0, y0, x1, y1, x2, y2,-BotColor);
            //++totstep;
            //ox0=x0;oy0=y0;ox1=x1;oy1=y1;ox2=x2;oy2=y2;
        }
        if (i==turnID) Mirror(x0, y0, x1, y1, x2, y2);
		if (i <= turnID - 1)
		{
			cin >> x0 >> y0 >> x1 >> y1 >> x2 >> y2;
			if (x0 >= 0)
            {
				PlaceChess(ori_state,x0, y0, x1, y1, x2, y2, BotColor);
                //++totstep;
            }
            //if (ox0!=7-x0 || oy0!=7-y0 || ox1!=7-x1 || oy1!=7-y1 || ox2!=7-x2 || oy2!=7-y2) Mirrorok=0;
            
		}
	}
}
inline void GenerateMove(vector<Move> &cur,const Situation &state,int color)
{
    // vector<Move> cur;
    // 循环展开优化：将8个方向的循环展开
    for (short ori_x=0;ori_x<Size;++ori_x)
    for (short ori_y=0;ori_y<Size;++ori_y)
    {
        if (state.mp[ori_x][ori_y] != color)
                continue;
        
        // 方向0: (1, 0)
        int nx=ori_x, ny=ori_y;
        while (true) {
            nx+=1;
            if ((unsigned)nx >= Size || state.mp[nx][ny] != 0) break;
            // 箭头方向循环展开
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
                }
            }
        }
        
        // 方向1: (0, 1)
        nx=ori_x, ny=ori_y;
        while (true) {
            ny+=1;
            if ((unsigned)ny >= Size || state.mp[nx][ny] != 0) break;
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
                }
            }
        }
        
        // 方向2: (-1, 0)
        nx=ori_x, ny=ori_y;
        while (true) {
            nx-=1;
            if ((unsigned)nx >= Size || state.mp[nx][ny] != 0) break;
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
                }
            }
        }
        
        // 方向3: (0, -1)
        nx=ori_x, ny=ori_y;
        while (true) {
            ny-=1;
            if ((unsigned)ny >= Size || state.mp[nx][ny] != 0) break;
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
                }
            }
        }
        
        // 方向4: (1, 1)
        nx=ori_x, ny=ori_y;
        while (true) {
            nx+=1, ny+=1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny] != 0) break;
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
                }
            }
        }
        
        // 方向5: (1, -1)
        nx=ori_x, ny=ori_y;
        while (true) {
            nx+=1, ny-=1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny] != 0) break;
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
                }
            }
        }
        
        // 方向6: (-1, 1)
        nx=ori_x, ny=ori_y;
        while (true) {
            nx-=1, ny+=1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny] != 0) break;
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
                }
            }
        }
        
        // 方向7: (-1, -1)
        nx=ori_x, ny=ori_y;
        while (true) {
            nx-=1, ny-=1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny] != 0) break;
            for (short j=0; j<8; ++j) {
                int ax=nx, ay=ny;
                while (true) {
                    ax+=dx[j], ay+=dy[j];
                    if ((unsigned)ax >= Size || (unsigned)ay >= Size || (state.mp[ax][ay]!=0 && (ax!=ori_x || ay!=ori_y))) break;
                    cur.emplace_back(Move{static_cast<char>(ori_x),static_cast<char>(ori_y),static_cast<char>(nx),static_cast<char>(ny),static_cast<char>(ax),static_cast<char>(ay)});
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
    
    // 使用展开循环优化
    for (short x = 0; x < Size; ++x) {
        for (short y = 0; y < Size; ++y)
        {
            if (state.mp[x][y] == color)
            {
                qx[tail] = x;
                qy[tail++] = y;
                res.mp[x][y] = 0;
            }
        }
    }
    
    // BFS计算Queen走法可达距离 - 循环展开优化
    while (head < tail)
    {
        int x=qx[head], y=qy[head++];
        
        // 方向0: (1, 0)
        int nx=x, ny=y;
        while (true)
        {
            nx += 1;
            if ((unsigned)nx >= Size || state.mp[nx][ny]!=0) break;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
        
        // 方向1: (0, 1)
        nx=x, ny=y;
        while (true)
        {
            ny += 1;
            if ((unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
        
        // 方向2: (-1, 0)
        nx=x, ny=y;
        while (true)
        {
            nx -= 1;
            if ((unsigned)nx >= Size || state.mp[nx][ny]!=0) break;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
        
        // 方向3: (0, -1)
        nx=x, ny=y;
        while (true)
        {
            ny -= 1;
            if ((unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
        
        // 方向4: (1, 1)
        nx=x, ny=y;
        while (true)
        {
            nx += 1; ny += 1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
        
        // 方向5: (1, -1)
        nx=x, ny=y;
        while (true)
        {
            nx += 1; ny -= 1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
        
        // 方向6: (-1, 1)
        nx=x, ny=y;
        while (true)
        {
            nx -= 1; ny += 1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
            if (res.mp[nx][ny] > res.mp[x][y] + 1)
            {
                res.mp[nx][ny] = res.mp[x][y] + 1;
                qx[tail] = nx;
                qy[tail++] = ny;
            }
        }
        
        // 方向7: (-1, -1)
        nx=x, ny=y;
        while (true)
        {
            nx -= 1; ny -= 1;
            if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
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

inline Situation CalcKingMove(int color, const Situation &state)
{
    Situation res;
    memset(res.mp,0x3f,sizeof res.mp);
    static int qx[64], qy[64];
    int head=0, tail=0;
    
    // 使用展开循环优化
    for (short x = 0; x < Size; ++x) {
        for (short y = 0; y < Size; ++y)
        {
            if (state.mp[x][y] == color)
            {
                qx[tail] = x;
                qy[tail++] = y;
                res.mp[x][y] = 0;
            }
        }
    }
    
    // BFS计算King走法可达距离 - 循环展开优化
    while (head < tail)
    {
        int x=qx[head], y=qy[head++];
        
        // 方向0: (1, 0)
        int nx=x+1, ny=y;
        if ((unsigned)nx < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向1: (0, 1)
        nx=x, ny=y+1;
        if ((unsigned)ny < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向2: (-1, 0)
        nx=x-1, ny=y;
        if ((unsigned)nx < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向3: (0, -1)
        nx=x, ny=y-1;
        if ((unsigned)ny < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向4: (1, 1)
        nx=x+1, ny=y+1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向5: (1, -1)
        nx=x+1, ny=y-1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向6: (-1, 1)
        nx=x-1, ny=y+1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向7: (-1, -1)
        nx=x-1, ny=y-1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res.mp[nx][ny] > res.mp[x][y] + 1)
        {
            res.mp[nx][ny] = res.mp[x][y] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
    }
    return res;
}

// 新增：预计算空白格子移动性 - 循环展开优化
inline void CalcBlankMobility(const Situation &state, int (&blankMobility)[8][8]) 
{
    memset(blankMobility,0, sizeof(blankMobility));
    for (short i = 0; i < Size; ++i) {
        for (short j = 0; j < Size; ++j) 
        {
            if (state.mp[i][j] == 0) 
            {
                // 循环展开优化
                int nx, ny;
                
                // 方向0: (1, 0)
                nx = i + 1; ny = j;
                if (nx < Size && state.mp[nx][ny] == 0) ++blankMobility[i][j];
                
                // 方向1: (0, 1)
                nx = i; ny = j + 1;
                if (ny < Size && state.mp[nx][ny] == 0) ++blankMobility[i][j];
                
                // 方向2: (-1, 0)
                nx = i - 1; ny = j;
                if (nx >= 0 && state.mp[nx][ny] == 0) ++blankMobility[i][j];
                
                // 方向3: (0, -1)
                nx = i; ny = j - 1;
                if (ny >= 0 && state.mp[nx][ny] == 0) ++blankMobility[i][j];
                
                // 方向4: (1, 1)
                nx = i + 1; ny = j + 1;
                if (nx < Size && ny < Size && state.mp[nx][ny] == 0) ++blankMobility[i][j];
                
                // 方向5: (1, -1)
                nx = i + 1; ny = j - 1;
                if (nx < Size && ny >= 0 && state.mp[nx][ny] == 0) ++blankMobility[i][j];
                
                // 方向6: (-1, 1)
                nx = i - 1; ny = j + 1;
                if (nx >= 0 && ny < Size && state.mp[nx][ny] == 0) ++blankMobility[i][j];
                
                // 方向7: (-1, -1)
                nx = i - 1; ny = j - 1;
                if (nx >= 0 && ny >= 0 && state.mp[nx][ny] == 0) ++blankMobility[i][j];
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
    
    // BFS计算 - 循环展开优化
    while (head < tail)
    {
        int cx = qx[head], cy = qy[head++];
        
        // 方向0: (1, 0)
        int nx = cx + 1, ny = cy;
        if ((unsigned)nx < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向1: (0, 1)
        nx = cx; ny = cy + 1;
        if ((unsigned)ny < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向2: (-1, 0)
        nx = cx - 1; ny = cy;
        if ((unsigned)nx < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向3: (0, -1)
        nx = cx; ny = cy - 1;
        if ((unsigned)ny < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向4: (1, 1)
        nx = cx + 1; ny = cy + 1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向5: (1, -1)
        nx = cx + 1; ny = cy - 1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向6: (-1, 1)
        nx = cx - 1; ny = cy + 1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
        
        // 方向7: (-1, -1)
        nx = cx - 1; ny = cy - 1;
        if ((unsigned)nx < Size && (unsigned)ny < Size && state.mp[nx][ny]==0 && res[nx][ny] == -1)
        {
            res[nx][ny] = res[cx][cy] + 1;
            qx[tail] = nx;
            qy[tail++] = ny;
        }
    }
    
    int blankMobility[8][8];
    CalcBlankMobility(state, blankMobility);
    
    int retmob = 0;
    // 移动性计算 - 循环展开优化
    
    // 方向0: (1, 0)
    int nx=x, ny=y;
    while (true)
    {
        nx += 1;
        if ((unsigned)nx >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    // 方向1: (0, 1)
    nx=x, ny=y;
    while (true)
    {
        ny += 1;
        if ((unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    // 方向2: (-1, 0)
    nx=x, ny=y;
    while (true)
    {
        nx -= 1;
        if ((unsigned)nx >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    // 方向3: (0, -1)
    nx=x, ny=y;
    while (true)
    {
        ny -= 1;
        if ((unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    // 方向4: (1, 1)
    nx=x, ny=y;
    while (true)
    {
        nx += 1; ny += 1;
        if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    // 方向5: (1, -1)
    nx=x, ny=y;
    while (true)
    {
        nx += 1; ny -= 1;
        if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    // 方向6: (-1, 1)
    nx=x, ny=y;
    while (true)
    {
        nx -= 1; ny += 1;
        if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    // 方向7: (-1, -1)
    nx=x, ny=y;
    while (true)
    {
        nx -= 1; ny -= 1;
        if ((unsigned)nx >= Size || (unsigned)ny >= Size || state.mp[nx][ny]!=0) break;
        if (res[nx][ny] > 0) {
            retmob += blankMobility[nx][ny] / res[nx][ny];
        }
    }
    
    return retmob;
}

inline double CalcValue(const Situation &state,const int color)//估价函数，令正值表示白有利
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
    
    // 先行方优势
    // double k = (BotColor==-1) ? 0.3 : -0.3; // 白方为+
    // k *= ((color==BotColor)? -1:1);//若color=Botcolor，即Bot已落子，则先行方为对方，*-1
    double k=(color==White) ? -0.3 : 0.3;
    
    // 计算双方QueenMove/KingMove矩阵 - 减少拷贝开销
    const Situation WhiteQM = CalcQueenMove(-1, state);
    const Situation BlackQM = CalcQueenMove(1, state);
    const Situation WhiteKM = CalcKingMove(-1, state);
    const Situation BlackKM = CalcKingMove(1, state);
    
    // 计算territory特征值t1(Queen)、t2(King)（空格控制权评估）
    double t1 = 0.0, t2 = 0.0;
    // 计算position特征值p1(Queen)、p2(King)
    double p1 = 0.0, p2 = 0.0;
    
    // 合并循环以减少循环开销
    for (short x = 0; x < Size; ++x) {
        for (short y = 0; y < Size; ++y) 
        {
            if (state.mp[x][y] != 0) continue;
            
            // t1：Queen走法控制权
            const int wqm = WhiteQM.mp[x][y], bqm = BlackQM.mp[x][y];
            const bool wqm_valid = (wqm != 0x3f3f3f3f);
            const bool bqm_valid = (bqm != 0x3f3f3f3f);
            
            if (wqm_valid && bqm_valid) {
                t1 += (wqm == bqm) ? k : ((wqm < bqm) ? 1 : -1);
            } else if (wqm_valid) {
                t1 += 1.0;
            } else if (bqm_valid) {
                t1 -= 1.0;
            }
            
            // t2：King走法控制权
            const int wkm = WhiteKM.mp[x][y], bkm = BlackKM.mp[x][y];
            const bool wkm_valid = (wkm != 0x3f3f3f3f);
            const bool bkm_valid = (bkm != 0x3f3f3f3f);
            
            if (wkm_valid && bkm_valid) 
            {
                t2 += (wkm == bkm) ? k : ((wkm < bkm) ? 1 : -1);
            } 
            else if (wkm_valid) t2 += 1.0;
            else if (bkm_valid) t2 -= 1.0;
            
            
            // p1：Queen走法位置差
            if (wqm_valid && bqm_valid) {
                // 使用快速幂近似代替pow函数
                const double wqm_pow = 1.0 / (1 << wqm);  // 2^(-wqm)
                const double bqm_pow = 1.0 / (1 << bqm);  // 2^(-bqm)
                p1 += 2 * (wqm_pow - bqm_pow);
            }
            else if (wqm_valid && (!bqm_valid)) p1+=2.0/ (1 << wqm);
            else if ((!wqm_valid) && bqm_valid) p1-=2.0/ (1 << bqm);



            // p2：King走法位置差
            if (wkm_valid && bkm_valid) {
                const double diff = static_cast<double>(bkm - wkm) / 6.0;
                // 使用条件表达式代替min/max函数调用
                const double clamped_diff = (diff > 1.0) ? 1.0 : ((diff < -1.0) ? -1.0 : diff);
                p2 += clamped_diff;
            }
        }
    }

    // 计算mobility特征值m
    double m=0;
    if (turnID<=14)
    {
    int WhiteMobility = 0, BlackMobility = 0;
    int WhiteMinMobility = 0x3f3f3f3f, BlackMinMobility = 0x3f3f3f3f;
    
    for (short x = 0; x < Size; ++x) {
        for (short y = 0; y < Size; ++y) 
        {
            if (state.mp[x][y] == -1) 
            { 
                const int tmp = CalcMobility(x, y, state);
                WhiteMobility += tmp;
                // 使用位运算优化min函数
                WhiteMinMobility = (tmp < WhiteMinMobility) ? tmp : WhiteMinMobility;
            }
            else if (state.mp[x][y] == 1) 
            { 
                const int tmp = CalcMobility(x, y, state);
                BlackMobility += tmp;
                BlackMinMobility = (tmp < BlackMinMobility) ? tmp : BlackMinMobility;
            }
        }
    }
    
    // 使用位运算优化减法
    const int mobility_diff = WhiteMobility - BlackMobility;
    const int min_mobility_diff = WhiteMinMobility - BlackMinMobility;
    m = static_cast<double>(mobility_diff) - 0.5 * static_cast<double>(min_mobility_diff);
    }
    // 分阶段加权计算总估值
    const double total = a * t1 + b * t2 + c * p1 + d * p2 + e * m;
    // if (abs(total+2.97)<0.1)
    // {
    //     int debug0=0;
    // }
    return total;
}
