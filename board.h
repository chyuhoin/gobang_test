#include <graphics.h>
#include <conio.h>
#include <algorithm>
#include <ctime>
#include <utility>
#include <vector>
#include <cstdlib>
#define inf 0x3f3f3f3f
TCHAR reg[] = _T("悔棋");
TCHAR hint[] = _T("提示");
TCHAR remake[] = _T("重开");
class Board
{
    int bo[20][20], num;
    std::pair<int, int> laststep[20][20], head;
    int getTruePos(int x) { return 30 * x - 10; }
    void getCont(int* s, int n, int* ans, bool isBlack)
    {
        bool sleep = 1;
        int jump = 0;
        int cont = 0;
        int lastjud = 0;
        s[++n] = !isBlack;
        for (int i = 1; i <= n; i++)
        {
            if (s[i] == -1)
            {
                if (jump)
                {
                    if (cont >= 5) ans[5]++;
                    if (cont)
                    {
                        cont += jump;
                        ans[lastjud]--;
                    }
                    if (cont >= 5);
                    else if (sleep && cont) ans[cont - 1]++;
                    else ans[(cont == 3 ? 6 : cont)]++;
                    lastjud = jump = 0;
                }
                else
                {
                    if (cont >= 5) ans[5]++;
                    else if (sleep && cont) ans[lastjud = cont - 1]++;
                    else ans[lastjud = (cont == 3 ? 6 : cont)]++;
                    jump = cont;
                }
                sleep = 0;
                cont = 0;
            }
            else if (s[i] == (!isBlack))
            {
                if (cont >= 5) ans[5]++;
                if (cont && jump)
                {
                    cont += jump;
                    ans[lastjud]--;
                }
                if (cont >= 5);
                else if (!sleep) ans[lastjud = (cont == 3 ? 6 : cont)]++;
                sleep = 1;
                cont = 0;
                lastjud = jump = 0;
            }
            else cont++;
        }
    }
public:
    /*
    清空并重构棋盘
    */
    void setBoard()
    {
        for (int i = 1; i <= 15; i++)
            line(20, getTruePos(i), 440, getTruePos(i));
        for (int i = 1; i <= 15; i++)
            line(getTruePos(i), 20, getTruePos(i), 440);
        setfillcolor(BLACK);
        fillcircle(getTruePos(8), getTruePos(8), 3);
        fillcircle(getTruePos(4), getTruePos(4), 3);
        fillcircle(getTruePos(4), getTruePos(12), 3);
        fillcircle(getTruePos(12), getTruePos(4), 3);
        fillcircle(getTruePos(12), getTruePos(12), 3);
        setfillcolor(WHITE);
        outtextxy(550, 200, reg);
        outtextxy(550, 230, hint);
        outtextxy(550, 260, remake);
        memset(laststep, 0, sizeof(laststep));
        memset(bo, -1, sizeof(bo)); num = 0;
        head = std::make_pair(0, 0);
    }
    /*
    判断(x,y)这个点是不是空的，如果是空的就返回1
    表示可以在这个点下棋
    */
    bool candrop(int x, int y)
    {
        return bo[x][y] == -1;
    }
    /*
    估价函数：一个5算1e分，4算10000分，2算5分，1算1分。
    特别地，活三算500分，冲四算35分，两者皆有另加5000分。
    如果不是活的，相应地退一档。
    返回值是“只算黑棋的估值”减去“只算白棋的估值”
    */
    int evaluate()
    {
        int s[30], n = 0, blans[7], whans[7];
        memset(blans, 0, sizeof(blans));
        memset(whans, 0, sizeof(whans));
        for (int i = 1; i <= 15; i++)
        {
            n = 0;
            for (int j = 1; j <= 15; j++)
                s[++n] = bo[i][j];
            getCont(s, n, blans, 1);
            getCont(s, n, whans, 0);
            n = 0;
            for (int j = 1; j <= 15; j++)
                s[++n] = bo[j][i];
            getCont(s, n, blans, 1);
            getCont(s, n, whans, 0);
        }
        for (int i = 1; i <= 15; i++)
        {
            n = 0;
            for (int j = 0; i + j <= 15; j++)
                s[++n] = bo[i + j][1 + j];
            getCont(s, n, blans, 1);
            getCont(s, n, whans, 0);
            n = 0;
            for (int j = 0; i + j <= 15; j++)
                s[++n] = bo[i + j][15 - j];
            getCont(s, n, blans, 1);
            getCont(s, n, whans, 0);
        }
        for (int i = 2; i <= 15; i++)
        {
            n = 0;
            for (int j = 0; i + j <= 15; j++)
                s[++n] = bo[1 + j][i + j];
            getCont(s, n, blans, 1);
            getCont(s, n, whans, 0);
        }
        for (int i = 2; i <= 15; i++)
        {
            n = 0;
            for (int j = 1; j < i; j++)
                s[++n] = bo[j][i - j];
            getCont(s, n, blans, 1);
            getCont(s, n, whans, 0);
        }
        int ans = 0;
        ans = ans + blans[1] - whans[1];
        ans = ans + (blans[2] - whans[2]) * 5;
        ans = ans + (blans[3] - whans[3]) * 35;
        ans = ans + (blans[4] - whans[4]) * 10000;
        ans = ans + (blans[5] - whans[5]) * 100000000;
        ans = ans + (blans[6] - whans[6]) * 500;
        if (blans[6] + blans[3] >= 2) ans += 5000;
        if (whans[6] + whans[3] >= 2) ans -= 5000;
        return ans;
    }
    /*
    判断局面胜负：1表示黑胜，0表示白胜，-1表示无输赢
    */
    int JudgeWin()
    {
        int num = evaluate();
        if (num >= 10000000) return 1;
        if (num <= -10000000) return 0;
        return -1;
    }
    /*
    下棋函数：在(x,y)位置下棋
    返回值是0，说明棋盘未满；返回1，说明棋盘下满了。
    */
    bool putchess(int x,int y,bool isBlack)
    {
        clearcircle(getTruePos(x), getTruePos(y), 5);
        if (isBlack) circle(getTruePos(x), getTruePos(y), 5);
        else fillcircle(getTruePos(x), getTruePos(y), 6);
        setlinecolor(RED);
        line(getTruePos(x) - 2, getTruePos(y), getTruePos(x) + 2, getTruePos(y));
        line(getTruePos(x), getTruePos(y) - 2, getTruePos(x), getTruePos(y) + 2);
        setlinecolor(BLACK);
        if (isBlack) fillcircle(getTruePos(head.first), getTruePos(head.second), 6);
        else
        {
            setfillcolor(BLACK);
            fillcircle(getTruePos(head.first), getTruePos(head.second), 5);
            setfillcolor(WHITE);
        }
        bo[x][y] = isBlack;
        laststep[x][y] = head;
        head = std::make_pair(x, y);
        return (++num) == 225;
    }
    /*
    假的下棋函数：在(x,y)位置下棋，不画图，只修改bo数组
    仅用于AI搜索
    */
    void FKput(int x, int y, bool isBlack)
    {
        bo[x][y] = isBlack;
    }
    /*
    回溯函数：在(x,y)位置拿起棋，不画图，只修改bo数组
    仅用于AI搜索
    */
    void REput(int x, int y)
    {
        bo[x][y] = -1;
    }
    /*
    获得（对于AI来说）所有能下棋的点。
    “能下棋的点”定义为已经存在的点的两步以内的空点
    */
    std::vector<std::pair<int, int> > getPoint()
    {
        std::vector<std::pair<int, int> > ans;
        if (!num) { ans.push_back(std::make_pair(8, 8)); return ans; }
        for (int i = 1; i <= 15; i++)
        for (int j = 1; j <= 15; j++)
        if (bo[i][j] == -1)
        {
            for (int d = -2; d <= 2; d++)
            for (int m = -2; m <= 2; m++)
            if (i + d <= 15 && j + m <= 15 && i + d > 0 && j + m > 0 && (bo[i + d][j + m] != -1))
            {
                ans.push_back(std::make_pair(i, j));
                goto endcheck;
            }
        endcheck:;
        }
        random_shuffle(ans.begin(), ans.end());
        return ans;
    }
    /*
    悔棋函数，撤销一步操作
    如果返回值为0，说明不能撤销（也就是棋盘为空）
    */
    bool Goback()
    {
        if (!num) return 0;
        loadimage(NULL, _T("bod.jpg"), 640, 480);
        int x = head.first, y = head.second;
        for (int i = 1; i <= 15; i++)
            line(20, getTruePos(i), 440, getTruePos(i));
        for (int i = 1; i <= 15; i++)
            line(getTruePos(i), 20, getTruePos(i), 440);
        setfillcolor(BLACK);
        fillcircle(getTruePos(8), getTruePos(8), 3);
        fillcircle(getTruePos(4), getTruePos(4), 3);
        fillcircle(getTruePos(4), getTruePos(12), 3);
        fillcircle(getTruePos(12), getTruePos(4), 3);
        fillcircle(getTruePos(12), getTruePos(12), 3);
        setfillcolor(WHITE);
        outtextxy(550, 200, reg);
        outtextxy(550, 230, hint);
        outtextxy(550, 260, remake);
        head = laststep[x][y];
        bo[x][y] = -1; num--;
        for (int i = 1; i <= 15; i++)
            for (int j = 1; j <= 15; j++)
            if(~bo[i][j])
            {
                clearcircle(getTruePos(i), getTruePos(j), 5);
                if (bo[i][j]) circle(getTruePos(i), getTruePos(j), 5);
                else fillcircle(getTruePos(i), getTruePos(j), 6);
            }
        setlinecolor(RED);
        line(getTruePos(head.first) - 2, getTruePos(head.second), getTruePos(head.first) + 2, getTruePos(head.second));
        line(getTruePos(head.first), getTruePos(head.second) - 2, getTruePos(head.first), getTruePos(head.second) + 2);
        setlinecolor(BLACK);
        return 1;
    }
};
double Getdis(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
class Artifact
{
    bool isBlack;
    int level;
public:
    Artifact(bool side = 0, int L = 1) { isBlack = side, level = L; }
    int dfs_ab(Board T, bool isBlack, int alpha, int beta, int& chx, int& chy, int deplim)
    {
        if ((!deplim) || T.JudgeWin() != -1) return T.evaluate();
        std::vector<std::pair<int, int> > pois = T.getPoint();
        int fw1, fw2;
        for (int i = 0; i < pois.size(); i++)
        {
            T.FKput(pois[i].first, pois[i].second, isBlack);
            int nxtv = dfs_ab(T, !isBlack, alpha, beta, fw1, fw2, deplim - 1);
            if (isBlack)
            {
                if (nxtv > alpha)
                {
                    alpha =  nxtv;
                    chx = pois[i].first;
                    chy = pois[i].second;
                }
                if (alpha >= beta) return alpha /*+ T.evaluate()*/;
            }
            else
            {
                if (nxtv < beta)
                {
                    beta = nxtv;
                    chx = pois[i].first;
                    chy = pois[i].second;
                }
                if (alpha >= beta) return beta /*+ T.evaluate()*/;
            }
            T.REput(pois[i].first, pois[i].second);
        }
        return (isBlack ? alpha : beta) /*+ T.evaluate()*/;
    }
    bool putchess(Board& T)
    {
        int x, y;
        dfs_ab(T, isBlack, -inf, inf, x, y, level + 1);
        return T.putchess(x, y, isBlack);
    }
    void setRank(int rank)
    {
        level = rank;
    }
    int getRank()
    {
        return level;
    }
    void setside(bool side)
    {
        isBlack = side;
    }
};