#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<graphics.h>
#include<math.h>
#include<windows.h>
#include<conio.h>

int flag = 0;
int ChessMap[20][20] = { 0 };// 
typedef struct {
    int mapX;
    int mapY;
    int color;
}Step;
Step chessStack[400];
int top = -1;

// 绘制当前回合提示
void drawPlayerTip() {
    setfillcolor(WHITE);
    solidrectangle(515, 20, 585, 45);
    settextcolor(RED);
    settextstyle(12, 0, "宋体");
    setbkmode(TRANSPARENT);
    outtextxy(505, 22, flag % 2 == 0 ? "当前回合：黑棋" : "当前回合：白棋");
}
// 悔棋功能
int regretStep() {
    if (top == -1) return 0;
    Step last = chessStack[top--];
    ChessMap[last.mapX][last.mapY] = 0;

    setfillcolor(WHITE);
    solidcircle(last.mapY * 25, last.mapX * 25, 11);
    setlinecolor(BLACK);
    int x = last.mapY * 25, y = last.mapX * 25;
    line(x, y - 12, x, y + 12);
    line(x - 12, y, x + 12, y);

    flag--;
    drawPlayerTip();
    return 1;
}

// 初始化/重置游戏（支持重新开始）
void resetGame() {
    // 清空棋盘数组
    for (int i = 1; i <= 19; i++) {
        for (int j = 1; j <= 19; j++) {
            ChessMap[i][j] = 0;
        }
    }
    top = -1;
    flag = 0;//清空悔棋栈
    cleardevice();
    loadimage(NULL, "wzq.jpg"); 
    setlinecolor(BLACK);
    for (int i = 1; i <= 19; i++) {
        line(25, 25 * i, 475, 25 * i);
        line(25 * i, 25, 25 * i, 475);
    }
    line(475, 25, 475, 475);
    line(25, 475, 475, 475);
    setlinecolor(RED);
    setfillcolor(LIGHTGRAY);
    solidrectangle(550, 140, 590, 170);
    solidrectangle(550, 200, 590, 230);
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    outtextxy(555, 150, "悔棋");
    outtextxy(555, 210, "退出");
    setbkmode(TRANSPARENT);
    settextcolor(BROWN);
    outtextxy(515, 60, "玩家1:黑棋");
    outtextxy(515, 100, "玩家2:白棋");
    drawPlayerTip();
}

// 胜负判断
int judge(int MapX, int MapY) {
    int temp = 2 - flag % 2;

    // 横向判断
    for (int i = MapX - 4, j = MapY; i <= MapX; i++) {
        if (i >= 1 && i + 4 <= 19 && temp == ChessMap[i][j] &&
            temp == ChessMap[i + 1][j] && temp == ChessMap[i + 2][j] &&
            temp == ChessMap[i + 3][j] && temp == ChessMap[i + 4][j])
            return 1;
    }

    // 纵向判断
    for (int i = MapX, j = MapY - 4; j <= MapY; j++) {
        if (j >= 1 && j + 4 <= 19 && temp == ChessMap[i][j] &&
            temp == ChessMap[i][j + 1] && temp == ChessMap[i][j + 2] &&
            temp == ChessMap[i][j + 3] && temp == ChessMap[i][j + 4])
            return 1;
    }

    // 左上-右下斜向
    for (int i = MapX - 4, j = MapY - 4; i <= MapX && j <= MapY; i++, j++) {
        if (i >= 1 && i + 4 <= 19 && j >= 1 && j + 4 <= 19 &&
            temp == ChessMap[i][j] && temp == ChessMap[i + 1][j + 1] &&
            temp == ChessMap[i + 2][j + 2] && temp == ChessMap[i + 3][j + 3] &&
            temp == ChessMap[i + 4][j + 4])
            return 1;
    }

    // 右上-左下斜向
    for (int i = MapX - 4, j = MapY + 4; i <= MapX && j >= MapY; i++, j--) {
        if (i >= 1 && i + 4 <= 19 && j - 4 >= 1 &&
            temp == ChessMap[i][j] && temp == ChessMap[i + 1][j - 1] &&
            temp == ChessMap[i + 2][j - 2] && temp == ChessMap[i + 3][j - 3] &&
            temp == ChessMap[i + 4][j - 4])
            return 1;
    }

    return 0;
}

void playGame() {
    int CHESSX = 0, CHESSY = 0;
    int MapX = 0, MapY = 0;
    MOUSEMSG Msg = { 0 };

    while (1) {
        if (PeekMouseMsg(&Msg)) {
            if (Msg.uMsg == WM_LBUTTONDOWN) {
                //  检测悔棋按钮
                if (Msg.x >= 550 && Msg.x <= 590 && Msg.y >= 140 && Msg.y <= 170) {
                    if (regretStep()) printf("悔棋成功！\n");
                    else MessageBox(NULL, "无棋可悔！", "提示", MB_OK);
                    continue;
                }
                //检测退出按钮
                if (Msg.x >= 550 && Msg.x <= 590 && Msg.y >= 200 && Msg.y <= 230) {
                    if (MessageBox(NULL, "确定要退出游戏吗？", "提示", MB_YESNO) == IDYES) {
                        closegraph();
                        exit(0);
                    }
                    continue;
                }
                // 像素→点位映射
                MapY = round((Msg.x - 25.0) / 25) + 1;
                MapX = round((Msg.y - 25.0) / 25) + 1;

                // 边界校验
                if (MapX < 1 || MapX > 19 || MapY < 1 || MapY > 19) {
                    MessageBox(NULL, "超出棋盘范围！", "提示", MB_OK);
                    continue;
                }

                // 空位校验
                if (ChessMap[MapX][MapY] != 0) {
                    MessageBox(NULL, "该位置已有棋子！", "提示", MB_OK);
                    continue;
                }

                // 绘制棋子
                CHESSX = MapY * 25;
                CHESSY = MapX * 25;
                if (flag % 2 == 0) {
                    setfillcolor(BLACK);
                    ChessMap[MapX][MapY] = 1;
                }
                else {
                    setfillcolor(WHITE);
                    ChessMap[MapX][MapY] = 2;
                }
                solidcircle(CHESSX, CHESSY, 10);

                // 记录落子
                chessStack[++top].mapX = MapX;
                chessStack[top].mapY = MapY;
                chessStack[top].color = ChessMap[MapX][MapY];

                // 切换回合+判断胜负
                flag++;
                drawPlayerTip();
                if (judge(MapX, MapY)) {
                    MessageBox(NULL, flag % 2 == 1 ? "黑棋获胜！" : "白棋获胜！", "Game Over", MB_OK);
                    // 询问是否重新开始
                    if (MessageBox(NULL, "是否重新开始游戏？", "提示", MB_YESNO) == IDYES) {
                        resetGame(); // 重置游戏
                        continue;
                    }
                    else {
                        break; // 退出游戏
                    }
                }
            }
        }
    }
}


int main() {
    initgraph(600, 500, SHOWCONSOLE);
    resetGame(); 
    playGame();
    closegraph();
    return 0;
}
	  
	  

 
	  
	  
	  
	  
	  