#include "board.h"
#include <cstdlib>
TCHAR bw[] = _T("Black Win!");
TCHAR ww[] = _T("White Win!");
TCHAR dr[] = _T("Draw!");
TCHAR ranks[][10] = { _T("Easy"), _T("Normal"), _T("Hard"), _T("Lunatic") };
TCHAR sides[][6] = { _T("White"), _T("Black") };
class Game
{
private:
    Board T;
    Artifact AI;
    bool humanside;//1表示人执黑，0表示人执白
    bool suicide;
    int hintrank;
    int getTruePos(int x) { return 30 * x - 10; }
    void regret(bool isPVE)//悔棋
    {
        T.Goback();
        if (isPVE) T.Goback();
    }
    bool givehint(bool isBlack)//提示
    {
        Artifact Siri(isBlack, hintrank);
        return Siri.putchess(T);
    }
    /*
    人类下棋的具体操作
    包含悔棋、提示、重开三个操作
    返回1，表示不能再下了
    */
    int humanput(Board& B, bool isBlack, bool isPVE)
    {
        MOUSEMSG m;
        while (1)
        {
            m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN)
            {
                if (m.x >= 550 && m.x <= 580 && m.y >= 200 && m.y <= 215)
                {
                    regret(isPVE);
                    if (!isPVE) return 0;
                }
                if (m.x >= 550 && m.x <= 580 && m.y >= 230 && m.y <= 255) return givehint(isBlack);
                if (m.x >= 550 && m.x <= 580 && m.y >= 260 && m.y <= 275) { suicide = 1; return 1; }
                for (int i = 1; i <= 15; i++)
                for (int j = 1; j <= 15; j++)
                    if (Getdis(getTruePos(i), getTruePos(j), m.x, m.y) < 10)
                    {
                        if(T.candrop(i,j))
                        return B.putchess(i, j, isBlack);
                    }
            }
        }
    }
    /*
    人类玩家下棋
    1表示游戏结束，0表示还可以继续下
    */
    bool humanplay(int isBlack, int isPVE)
    {
        bool isdr = humanput(T, isBlack, isPVE);
        int res = T.JudgeWin();
        if (res == 1) outtextxy(300, 200, bw);
        else if (res == 0) outtextxy(300, 200, ww);
        else if (isdr) { outtextxy(300, 200, dr); return 1; }
        if (~res) return 1;
        return 0;
    }
    /*
    电脑下棋
    1表示游戏结束，0表示还可以继续下
    */
    bool AIplay()
    {
        bool isdr = AI.putchess(T);
        int res = T.JudgeWin();
        if (res == 1) outtextxy(300, 200, bw);
        else if (res == 0) outtextxy(300, 200, ww);
        else if (isdr) { outtextxy(300, 200, dr); return 1; }
        if (~res) return 1;
        return 0;
    }
    void settingspn()
    {
        TCHAR emrank[] = _T("电脑水平");
        TCHAR htrank[] = _T("提示水平");
        TCHAR mypos[] = _T("我方执子");
        TCHAR goback[] = _T("返回");
        cleardevice();
        loadimage(NULL, _T("bgp.jpg"), 640, 480);
        outtextxy(450, 240, emrank);
        outtextxy(550, 240, ranks[AI.getRank()]);

        outtextxy(450, 270, htrank);
        outtextxy(550, 270, ranks[hintrank]);

        outtextxy(450, 300, mypos);
        outtextxy(550, 300, sides[humanside]);

        outtextxy(450, 330, goback);
    }
    void changeAI()
    {
        int AIrank = AI.getRank();
        AIrank = (AIrank + 1) % 4;
        AI.setRank(AIrank);
        settingspn();
    }
    void changeHT()
    {
        hintrank = (hintrank + 1) % 4;
        settingspn();
    }
    void changeSD()
    {
        humanside = !humanside;
        AI.setside(!humanside);
        settingspn();
    }
public:
    Game() { humanside = 1; suicide = 0; hintrank = 1; }
    void titlespn()
    {
        TCHAR pve[] = _T("人机对弈");
        TCHAR pvp[] = _T("双人对弈");
        TCHAR stg[] = _T("设置");
        loadimage(NULL, _T("bgp.jpg"), 640, 480);
        outtextxy(500, 240, pve);
        outtextxy(500, 270, pvp);
        outtextxy(500, 300, stg);
    }
    void PVE()
    {
    hachimari:
        loadimage(NULL, _T("bod.jpg"), 640, 480);
        T.setBoard(); suicide = 0;
        if (humanside)
        {
            for (int i = 1;; i++)
            {
                if (humanplay(humanside, 1)) break;
                if (AIplay()) break;
            }
        }
        else
        {
            for (int i = 1;; i++)
            {
                if (AIplay()) break;
                if (humanplay(humanside, 1)) break;
            }
        }
        if (suicide) goto hachimari;
        MOUSEMSG m;
        do
        {
            m = GetMouseMsg();
        } while (m.uMsg != WM_LBUTTONDOWN);
        cleardevice();
    }
    void PVP()
    {
    hachimari:
        loadimage(NULL, _T("bod.jpg"), 640, 480);
        T.setBoard(); suicide = 0;
        for (int i = 1;; i++)
        {
            if (humanplay(i & 1, 0)) break;
        }
        if (suicide) goto hachimari;
        MOUSEMSG m;
        do
        {
            m = GetMouseMsg();
        } while (m.uMsg != WM_LBUTTONDOWN);
        cleardevice();
    }
    void settings()
    {
        settingspn();
        MOUSEMSG m;
        while (1)
        {
            m = GetMouseMsg();
            if (m.uMsg == WM_LBUTTONDOWN)
            {
                if (m.x >= 550 && m.x <= 610 && m.y >= 240 && m.y <= 255) changeAI();
                else if (m.x >= 550 && m.x <= 610 && m.y >= 270 && m.y <= 285) changeHT();
                else if (m.x >= 550 && m.x <= 610 && m.y >= 300 && m.y <= 315) changeSD();
                else if (m.x >= 450 && m.x <= 480 && m.y >= 330 && m.y <= 345) return;
            }
        }
        
    }
}MA;

int main()
{
    // 初始化图形窗口
    initgraph(640, 480);
   
    MOUSEMSG m;
    setlinecolor(BLACK);
    srand(time(0));
    
    MA.titlespn();
    while (1)
    {
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN)
        {
            if (m.x >= 500 && m.x <= 560 && m.y >= 240 && m.y <= 255) MA.PVE();
            else if (m.x >= 500 && m.x <= 560 && m.y >= 270 && m.y <= 285) MA.PVP();
            else if (m.x >= 500 && m.x <= 530 && m.y >= 300 && m.y <= 315) MA.settings();
            MA.titlespn();
        }
    }
    
    //_getch();
    // 关闭图形窗口
    closegraph();
    return 0;
}