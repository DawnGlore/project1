#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

//函数声明段

void DrawMenu();	//绘制主菜单界面
void DrawModeMenu();	//绘制游戏模式选择界面
void DrawBoard();	//绘制棋盘
void DrawChess(int x, int y, bool isBlack);	//在指定位置绘制棋子
bool CheckWin(int x, int y);	//检查当前落子是否导致胜利
void ResetGame();	//重置当前棋局
int Evaluate(int x, int y, int player);	//评估棋盘上某个位置的得分（用于电脑）
pair<int, int>ComputerMove(bool isFirstMove);	//电脑计算最佳落子位置
void PlayGame();	//执行游戏逻辑
void ShowWinText(int blackScore, int whiteScore, bool isGameOver);	//显示胜利信息
void ResetGameForNewSession();	//重置整个游戏对话（包括比分等）

//全局变量声明段

#define GRID_SIZE 15	//棋盘大小
#define CELL_SIZE 30	//每个格子的大小
#define MARTIN_X 50	    //棋盘左上角x坐标
#define MARTIN_Y 50	    //棋盘左上角y坐标

int board[GRID_SIZE][GRID_SIZE] = { 0 };	//棋盘状态，0表示空，1表示黑棋，2表示白棋
bool isBlackTurn = true;	//当前是否为黑棋回合
bool gameOver = false;	   //游戏是否结束
bool gameRunning = true;	//游戏是否正在运行
bool isPlayerVsComputer = false;	//是否为人机对战模式


//绘制UI段

int gameMode = 1;	//当前选择的游戏模式（1局/3局/5局/7局）
int blackWin = 0, whiteWin = 0;	//黑棋和白棋的胜利次数
int currentRound = 1;	//当前局数
const int MAX_ROUND[] = { 1,3,5,7 };	//不同游戏模式的最大局数

void DrawMenu()
{
	cleardevice();
	setbkcolor(RGB(255,228,225));	//设置背景
	cleardevice();

	//绘制标题
	settextcolor(BLACK);
	settextstyle(40, 0, _T("宋体"));
	int titleX = (640 - textwidth(_T("五子棋游戏"))) / 2;
	outtextxy(titleX, 50, _T("五子棋游戏"));

	//绘制按钮
	int buttonWidth = 300;
	int buttonHeight = 50;
	int buttonSpacing = 30;
	int startY = 300;

	int button1X = (640 - buttonWidth) / 2;
	setfillcolor(RGB(200, 200, 200));
	solidrectangle(button1X, startY, button1X + buttonWidth, startY + buttonHeight);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(20, 0, _T("宋体"));
	int text1X = button1X + (buttonWidth - textwidth(_T("人人对战"))) / 2;
	int text1Y = startY + (buttonHeight - textheight(_T("人人对战"))) / 2;
	outtextxy(text1X, text1Y, _T("人人对战"));

	int button2X = button1X;
	int button2Y = startY + buttonHeight + buttonSpacing;
	setfillcolor(RGB(200,200,200));
	solidrectangle(button2X,button2Y,button2X+buttonWidth,button2Y+buttonHeight);
	settextcolor(BLACK);
	settextstyle(20, 0, _T("宋体"));
	int text2X = button2X + (buttonWidth - textwidth(_T("人机对战"))) / 2;
	int text2Y = button2Y + (buttonHeight - textheight(_T("人机对战"))) / 2;
	outtextxy(text2X, text2Y, _T("人机对战"));

	//检测获取鼠标点击状态
	ExMessage msg;
	bool modeSelected = false;
	while (!modeSelected)
	{
		if (peekmessage(&msg, EM_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)	//检测鼠标左键是否点击
			{
				if (msg.x >= button1X && msg.x <= button1X + buttonWidth && msg.y >= startY && msg.y <= startY + buttonHeight)	//如果点击“人人对战”按钮
				{
					isPlayerVsComputer = false;
					modeSelected = true;
					DrawModeMenu(); //不是人机对战，确认模式选择，进入下一决胜模式选择模块
				}
				else if (msg.x >= button2X && msg.x <= button2X + buttonWidth && msg.y >= button2Y && msg.y <= button2Y + buttonHeight)	//如果点击“人机对战”按钮
				{
					isPlayerVsComputer = true;
					modeSelected = true;
					DrawModeMenu();	//是人机对战，同上进行跳转
				}
			}
		}
	}
}

void DrawModeMenu()
{
	//清除屏幕，绘制新的界面
	cleardevice();
	setbkcolor(RGB(255, 228, 225));
	cleardevice();

	settextcolor(BLACK);
	settextstyle(25, 0, _T("宋体"));
	int titleY = (640 - textwidth(_T("请选择游戏模式："))) / 2;
	outtextxy(titleY, 50, _T("请选择游戏模式："));

	//同DrawMenu步骤，对按钮进行绘制
	int buttonWidth = 300;
	int buttonHeight = 50;
	int buttonSpacing = 30;
	int modeButtonStartY = 150;

	const TCHAR* modeLabels[] = { _T("一局定胜负"),_T("三局两胜"),_T("五局三胜"),_T("七局四胜") };
	for (int i = 0; i < 4; ++i)	//为简化代码，直接利用循环函数一次性绘制四个按钮
	{
		int buttonX = (640 - buttonWidth) / 2;
		int buttonY = modeButtonStartY + i * (buttonHeight + buttonSpacing);
		setfillcolor(RGB(200, 200, 200));
		solidrectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
		settextcolor(BLACK);
		settextstyle(20, 0, _T("宋体"));
		int textX = buttonX + (buttonWidth - textwidth(modeLabels[i])) / 2;
		int textY = buttonY + (buttonHeight - textheight(modeLabels[i])) / 2;
		outtextxy(textX, textY, modeLabels[i]);
	}

	//依旧检测获取鼠标点击状态
	ExMessage msg;
	bool modeSelected = false;
	while (!modeSelected)
	{
		if (peekmessage(&msg, EM_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)	//同
			{
				for (int i = 0; i < 4; ++i)
				{
					int buttonX = (640 - buttonWidth) / 2;
					int buttonY = modeButtonStartY + i * (buttonHeight + buttonSpacing);
					if (msg.x >= buttonX && msg.x <= buttonX + buttonWidth && msg.y >= buttonY && msg.y <= buttonY + buttonHeight)	//检测按下了哪个按钮
					{
						gameMode = i + 1;
						modeSelected = true;
						ResetGameForNewSession();
						break;
					}
				}
			}
		}
	}

	gameRunning = true;
	DrawBoard();
}

void DrawBoard()
{
	//设置棋盘背景大小
	setfillcolor(RGB(255, 211, 155));
	solidrectangle(10, 40, 480, 510);

	//画格子
	int start1 = 50;
	int start2 = 50;
	setlinecolor(BLACK);
	for (int i = 0; i <= 14; ++i)
	{
		line(50, start1, 470, start1);
		start1 += 30;
	}
	for (int j = 0; j <= 14; ++j)
	{
		line(start2, 50, start2, 470);
		start2 += 30;
	}

	//画五个星位
	setfillcolor(BLACK);
	solidcircle(140, 140, 4);
	solidcircle(380, 140, 4);
	solidcircle(140, 380, 4);
	solidcircle(380, 380, 4);
	solidcircle(260, 260, 4);

	//绘制数字
	setbkmode(TRANSPARENT);
	TCHAR num[5];
	int x = 50;
	int y = 50;
	for (int i = 15; i >= 1; i--)
	{
		_stprintf_s(num, _T("%d"), i);
		settextcolor(BLACK);
		settextstyle(15, 10, _T("宋体"));
		int h = textheight(num);
		int w = textwidth(num);
		outtextxy(30 - w / 2, y - h / 2, num);
		y += 30;
	}
	//绘制字母
	TCHAR ch[2];
	for (int i = 0; i <15; i++)
	{
		ch[0] = _T('A') + i;
		ch[1] = _T('\0');
		settextcolor(BLACK);
		settextstyle(15, 10, _T("宋体"));
		int h = textheight(ch);
		int w = textwidth(ch);
		outtextxy(x - w / 2, 490 - h / 2, ch);
		x += 30;
	}
}

void DrawChess(int x, int y, bool isBlack)
{
	//根据当前是黑棋还是白棋来画棋子
	if (isBlack)
	{
		setfillcolor(BLACK);
		setlinecolor(BLACK);
	}
	else
	{
		setfillcolor(WHITE);
		setlinecolor(BLACK);
	}
	fillcircle(MARTIN_X + x * CELL_SIZE, MARTIN_Y + y * CELL_SIZE, 10);
}

void ShowWinText(int blackScore,int whiteScore,bool isGameOver)
{
	//清除屏幕绘制转场
	cleardevice();
	setbkcolor(RGB(255, 228, 225));
	cleardevice();
	settextcolor(BLACK);
	settextstyle(30, 0, _T("宋体"));
	TCHAR winText[100];
	if (isGameOver)	//如果整场游戏结束
	{
		//显示最终获胜信息
		_stprintf_s(winText, _T("游戏结束！%s得了MVP！"), blackScore > whiteScore ? _T("黑棋") : _T("白棋"));
	}
	else //如果还没结束
	{
		//仅显示本局结束后获胜方及双方的得分
		_stprintf_s(winText, _T("本局结束！当前比分：黑棋%d - 白棋%d"), blackScore, whiteScore);
	}
	//令字幕显示在中间
	int textX = (640 - textwidth(winText)) / 2;
	int textY = 300;
	outtextxy(textX, textY, winText);

	//检测获取键盘输入状态
	bool returnToMenu = false;
	while (!returnToMenu)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)	//如果回车键被按下
		{
			if (isGameOver)	//如果整场游戏都结束
			{
				gameRunning = false;	//直接退出
			}
			else
			{
				ResetGame();	//未结束则继续开启下一局
				break;
			}
		}
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)	//如果按下Esc键
		{
			gameRunning = false;
			returnToMenu = true;	//返回主界面并退出
			break;
		}
		else if (GetAsyncKeyState('R') & 0x8000)	//如果按下R键
		{
			returnToMenu = true;	//返回主界面重新选择
		}
		Sleep(100);
	}

	//返回主界面并重置整场游戏
	if (returnToMenu && gameRunning)
	{
		ResetGameForNewSession();
		DrawMenu();
	}
	
}

//游戏功能段

bool CheckWin(int x, int y)
{
	//检测四个方向
	int dir[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
	int count;

	//正向检测
	for (int d = 0; d < 4; d++)
	{
		count = 1;

		//每下一棋立即检测周围
		for (int i = 1; i <= 4; i++)
		{
			int nx = x + i * dir[d][0];
			int ny = y + i * dir[d][1];
			if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE || board[nx][ny] != board[x][y])	//如果周围没有能连成直线的棋子
			{
				break;	//直接退出循环
			}
			count++;	//有就加一
			if (count >= 5)	//如果连成直线的棋子达到五颗
			{
				gameOver = true;
				return true;	//游戏结束
			}
		}

		//负向检测，注释同上
		for (int i = 1; i <= 4; i++)
		{
			int nx = x - i * dir[d][0];
			int ny = y - i * dir[d][1];
			if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE || board[nx][ny] != board[x][y])
			{
				break;
			}
			count++;
			if (count >= 5)
			{
				gameOver = true;
				return true;
			}
		}
	}
	return false;
}

void ResetGame()
{
	//遍历棋盘的每个位置
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			board[i][j] = 0;	//将每个位置的棋子都清零
		}
	}

	//重置游戏状态
	gameOver = false;	
	cleardevice();
	DrawBoard();
}

void ResetGameForNewSession()
{
	//依旧清零棋盘
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			board[i][j] = 0;
		}
	}

	//直接将整个游戏全部重置为初始状态
	gameOver = false;
	blackWin = 0;
	whiteWin = 0;
	currentRound = 1;
	isBlackTurn = true;
}

//人机设计段

int Evaluate(int x, int y, int player)
{
	//设置方向、得分、对手检测
	int dir[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
	int score = 0;
	int opponent = (player == 1) ? 2 : 1;

	//使用与CheckWin函数类似的正反遍历机制
	for (int d = 0; d < 4; d++)
	{
		int count = 1;
		int emptyBefore = 0, emptyAfter = 0;
		int blockBefore = 0, blockAfter = 0;
		bool isOpenFour = false, isOpenThree = false;
		bool opponentOpenThree = false, opponentOpenFour = false;

		for (int i = 1; i <= 4; i++)
		{
			int nx = x + i * dir[d][0];
			int ny = y + i * dir[d][1];
			if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE)
			{
				break;
			}
			if (board[nx][ny] == player)
			{
				count++;
			}
			else if (board[nx][ny] == 0)
			{
				emptyAfter++;
				if (count == 3 && emptyAfter == 1)
				{
					isOpenThree = true;
				}

				if (count == 4 && emptyAfter == 1)
				{
					isOpenFour = true;
				}
			}
			else
			{
				blockAfter++;
				break;
			}
		}

		for (int i = 1; i <= 4; i++)
		{
			int nx = x - i * dir[d][0];
			int ny = y - i * dir[d][1];
			if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE)
			{
				break;
			}
			if (board[nx][ny] == player)
			{
				count++;
			}
			else if (board[nx][ny] == 0)
			{
				emptyBefore++;
				if (count == 3 && emptyAfter == 1)
				{
					isOpenThree = true;
				}

				if (count == 4 && emptyAfter == 1)
				{
					isOpenFour = true;
				}
			}
			else
			{
				blockBefore++;
				break;
			}
		}

		//评估分数
		if (count >= 5)
		{
			score += 10000;	//五连珠
		}
		else if (isOpenFour)
		{
			score += 1000;	//活四
		}
		else if (count == 4 && (emptyBefore + emptyAfter) > 0)
		{
			score += 500;	//冲四
		}
		else if (isOpenThree)
		{
			score += 100;	//活三
		}
		else if (count == 3 && (emptyBefore + emptyAfter) > 1)
		{
			score += 50;	//眠三
		}
		else if (count == 2 && (emptyBefore + emptyAfter) > 2)
		{
			score += 10;	//活二
		}

		//考虑对手威胁
		if (blockBefore == 0 && blockAfter == 0 && count >= 4)
		{
			//如果对手可以形成五连珠，给予很高的负分
			int opponentCount = 0;
			int opponentEmptyBefore = 0, opponentEmptyAfter = 0;

			for (int i = 1; i <= 4; i++)
			{
				int nx = x + i * dir[d][0];
				int ny = y + i * dir[d][1];
				if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE)
				{
					break;
				}
				if (board[nx][ny] == opponent)
				{
					opponentCount++;
				}
				else if (board[nx][ny] == 0)
				{
					opponentEmptyAfter++;
					if (opponentCount == 3 && (opponentEmptyBefore + opponentEmptyAfter) > 0)
					{
						opponentOpenThree = true;
					}
					if (opponentCount == 4 && (opponentEmptyBefore + opponentEmptyAfter) > 0)
					{
						opponentOpenFour = true;
					}
				}
				else
				{
					break;
				}
			}

			for (int i = 1; i <= 4; i++)
			{
				int nx = x - i * dir[d][0];
				int ny = y - i * dir[d][1];
				if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE)
				{
					break;
				}
				if (board[nx][ny] == opponent)
				{
					opponentCount++;
				}
				else if (board[nx][ny] == 0)
				{
					opponentEmptyBefore++;
					if (opponentCount == 3 && (opponentEmptyBefore + opponentEmptyAfter) > 0)
					{
						opponentOpenThree = true;
					}
					if (opponentCount == 4 && (opponentEmptyBefore + opponentEmptyAfter) > 0)
					{
						opponentOpenFour = true;
					}
				}
				else
				{
					break;
				}
			}

			if (opponentOpenFour)	//对手即将形成五连珠
			{
				score -= 9000;
			}
			else if (opponentOpenThree)
			{
				score -= 5000;
			}
		}
	}

	return score;
}

pair<int, int>ComputerMove(bool isFirstMove)
{
	int bestScore = -1;	//记录最佳得分
	pair<int, int> bestMove = { -1,-1 };	//记录最佳落子位置
	pair<int, int>bestBlockMove = { -1,-1 };
	int blockScoreThreshold = 800;

	//如果是电脑的第一步棋
	if (isFirstMove)
	{
		srand(time(0));	//初始化随机种子
		int x, y;
		//随机选择一个空位置落子
		do
		{
			x = rand() % GRID_SIZE;	//随机生成x坐标
			y = rand() % GRID_SIZE;	//随机生成y坐标
		} while (board[x][y] != 0);	//确保该位置上没有棋子
		bestMove = { x,y };
	}
	else
	{
		bool foundBlockMove = false;
		//优先检查是否有需要拦截的威胁
		for (int i = 0; i < GRID_SIZE; ++i)
		{
			for (int j = 0; j < GRID_SIZE; ++j)
			{
				//只考虑空位置
				if (board[i][j] == 0)
				{
					//评估玩家（黑棋）的得分
					int score = Evaluate(i, j, 1);
					if (score > blockScoreThreshold)	//若玩家得分超过阈值，认为存在威胁
					{
						bestBlockMove = { i,j };
						foundBlockMove = true;
						break;
					}
				}
			}
			if (foundBlockMove)
			{
				break;
			}
		}

		if (foundBlockMove)
		{
			//如果找到需要拦截的位置，则优先选择该位置
			bestMove = bestBlockMove;
		}
		else
		{
			//若没有需要拦截的位置，则按照正常逻辑，遍历整个棋盘寻找最佳落子位置
			for (int i = 0; i < GRID_SIZE; ++i)
			{
				for (int j = 0; j < GRID_SIZE; ++j)
				{
					//只考虑空位置
					if (board[i][j] == 0)
					{
						//评估该位置的得分（2表示电脑棋子）
						int score = Evaluate(i, j, 2);
						//如果找到更高得分点，更新最佳落子位置
						if (score > bestScore)
						{
							bestScore = score;
							bestMove = { i,j };
						}
					}
				}
			}
		}
	}
	return bestMove;
}

//游戏主体段

void PlayGame()
{
	bool isComputerFirstMove = true;	//标记是否为电脑落子的第一步

	ExMessage msg;
	//游戏主循环，条件：游戏未结束，游戏运行中，当前回合未超过最大回合数
	while (!gameOver && gameRunning && currentRound <= MAX_ROUND[gameMode - 1])
	{
		//如果是人机对战且轮到白方（电脑）下棋
		if (isPlayerVsComputer && !isBlackTurn)
		{
			//获取电脑的落子位置
			pair<int, int> move = ComputerMove(isComputerFirstMove);
			//如果是计算机的第一步，标记后续不再是第一步
			if (isComputerFirstMove)
			{
				isComputerFirstMove = false;
			}

			//在棋盘上落子
			board[move.first][move.second] = 2;
			DrawChess(move.first, move.second, false);
			//检查是否获胜
			if (CheckWin(move.first, move.second))
			{
				whiteWin++;	//白方胜利计数增加
				gameOver = true;	//本局结束
			}
			else
			{
				isBlackTurn = true;	//未胜利则黑方下棋
			}
			continue;	//跳过本次循环的剩余部分
		}

		//获取处理鼠标信息
		if (peekmessage(&msg, EM_MOUSE))
		{
			//如果是鼠标左键点击
			if (msg.message == WM_LBUTTONDOWN)
			{
				//计算点击的棋盘坐标
				int x = (msg.x - MARTIN_X + CELL_SIZE / 2) / CELL_SIZE;
				int y = (msg.y - MARTIN_Y + CELL_SIZE / 2) / CELL_SIZE;

				//检查坐标是否有效且该位置为空
				if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && board[x][y] == 0)
				{
					if (!board[x][y])
					{
						//根据当前回合落子对象来绘制棋子
						board[x][y] = isBlackTurn ? 1 : 2;
						DrawChess(x, y, isBlackTurn);

						//检查是否获胜
						if (CheckWin(x, y))
						{
							//如果是黑方
							if (isBlackTurn)
							{
								blackWin++;	//黑方胜利计数增加
							}
							else
							{
								whiteWin++;	//反之则白方计数增加
							}
							gameOver = true;	//本局游戏结束
						}
						else
						{
							isBlackTurn = !isBlackTurn;	//未出现胜利则继续交换回合
						}

					}
				}
			}
		}
	}

	//游戏结束处理
	if (gameOver && gameRunning)
	{
		bool isSingleGame = (gameMode == 1);
		//检查是否满足整场游戏结束的条件（单局或达到胜利局数）
		if (isSingleGame || 
			(gameMode == 2 && (blackWin == 2 || whiteWin == 2)) ||
			(gameMode == 3 && (blackWin == 3 || whiteWin == 3)) ||
			(gameMode == 4 && (blackWin == 4 || whiteWin == 4)))
		{
			gameOver = true;
			//显示最终胜利信息
			ShowWinText(blackWin, whiteWin, true);
		}
		else
		{
			currentRound++;	//当前回合数增加
			gameOver = false;	//重置游戏结束标志
			//显示当前局数结束信息
			ShowWinText(blackWin, whiteWin, false);
			isBlackTurn = true;	//重置为黑方先手
			ResetGame();	//重置当前一局游戏状态
		}
	}
}



//运行

//程序主入口
int main()
{
	//初始化图形窗口（640*640像素）
	initgraph(640, 640);
	setbkmode(WHITE);
	cleardevice();

	//绘制游戏菜单
	DrawMenu();

	//游戏主循环
	while (gameRunning)
	{
		DrawBoard();	//绘制棋盘
		PlayGame();		//开始一局游戏
	}

	closegraph();	//关闭图形窗口
	return 0;
}
