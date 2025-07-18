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

//����������

void DrawMenu();	//�������˵�����
void DrawModeMenu();	//������Ϸģʽѡ�����
void DrawBoard();	//��������
void DrawChess(int x, int y, bool isBlack);	//��ָ��λ�û�������
bool CheckWin(int x, int y);	//��鵱ǰ�����Ƿ���ʤ��
void ResetGame();	//���õ�ǰ���
int Evaluate(int x, int y, int player);	//����������ĳ��λ�õĵ÷֣����ڵ��ԣ�
pair<int, int>ComputerMove(bool isFirstMove);	//���Լ����������λ��
void PlayGame();	//ִ����Ϸ�߼�
void ShowWinText(int blackScore, int whiteScore, bool isGameOver);	//��ʾʤ����Ϣ
void ResetGameForNewSession();	//����������Ϸ�Ի��������ȷֵȣ�

//ȫ�ֱ���������

#define GRID_SIZE 15	//���̴�С
#define CELL_SIZE 30	//ÿ�����ӵĴ�С
#define MARTIN_X 50	    //�������Ͻ�x����
#define MARTIN_Y 50	    //�������Ͻ�y����

int board[GRID_SIZE][GRID_SIZE] = { 0 };	//����״̬��0��ʾ�գ�1��ʾ���壬2��ʾ����
bool isBlackTurn = true;	//��ǰ�Ƿ�Ϊ����غ�
bool gameOver = false;	   //��Ϸ�Ƿ����
bool gameRunning = true;	//��Ϸ�Ƿ���������
bool isPlayerVsComputer = false;	//�Ƿ�Ϊ�˻���սģʽ


//����UI��

int gameMode = 1;	//��ǰѡ�����Ϸģʽ��1��/3��/5��/7�֣�
int blackWin = 0, whiteWin = 0;	//����Ͱ����ʤ������
int currentRound = 1;	//��ǰ����
const int MAX_ROUND[] = { 1,3,5,7 };	//��ͬ��Ϸģʽ��������

void DrawMenu()
{
	cleardevice();
	setbkcolor(RGB(255,228,225));	//���ñ���
	cleardevice();

	//���Ʊ���
	settextcolor(BLACK);
	settextstyle(40, 0, _T("����"));
	int titleX = (640 - textwidth(_T("��������Ϸ"))) / 2;
	outtextxy(titleX, 50, _T("��������Ϸ"));

	//���ư�ť
	int buttonWidth = 300;
	int buttonHeight = 50;
	int buttonSpacing = 30;
	int startY = 300;

	int button1X = (640 - buttonWidth) / 2;
	setfillcolor(RGB(200, 200, 200));
	solidrectangle(button1X, startY, button1X + buttonWidth, startY + buttonHeight);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(20, 0, _T("����"));
	int text1X = button1X + (buttonWidth - textwidth(_T("���˶�ս"))) / 2;
	int text1Y = startY + (buttonHeight - textheight(_T("���˶�ս"))) / 2;
	outtextxy(text1X, text1Y, _T("���˶�ս"));

	int button2X = button1X;
	int button2Y = startY + buttonHeight + buttonSpacing;
	setfillcolor(RGB(200,200,200));
	solidrectangle(button2X,button2Y,button2X+buttonWidth,button2Y+buttonHeight);
	settextcolor(BLACK);
	settextstyle(20, 0, _T("����"));
	int text2X = button2X + (buttonWidth - textwidth(_T("�˻���ս"))) / 2;
	int text2Y = button2Y + (buttonHeight - textheight(_T("�˻���ս"))) / 2;
	outtextxy(text2X, text2Y, _T("�˻���ս"));

	//����ȡ�����״̬
	ExMessage msg;
	bool modeSelected = false;
	while (!modeSelected)
	{
		if (peekmessage(&msg, EM_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)	//����������Ƿ���
			{
				if (msg.x >= button1X && msg.x <= button1X + buttonWidth && msg.y >= startY && msg.y <= startY + buttonHeight)	//�����������˶�ս����ť
				{
					isPlayerVsComputer = false;
					modeSelected = true;
					DrawModeMenu(); //�����˻���ս��ȷ��ģʽѡ�񣬽�����һ��ʤģʽѡ��ģ��
				}
				else if (msg.x >= button2X && msg.x <= button2X + buttonWidth && msg.y >= button2Y && msg.y <= button2Y + buttonHeight)	//���������˻���ս����ť
				{
					isPlayerVsComputer = true;
					modeSelected = true;
					DrawModeMenu();	//���˻���ս��ͬ�Ͻ�����ת
				}
			}
		}
	}
}

void DrawModeMenu()
{
	//�����Ļ�������µĽ���
	cleardevice();
	setbkcolor(RGB(255, 228, 225));
	cleardevice();

	settextcolor(BLACK);
	settextstyle(25, 0, _T("����"));
	int titleY = (640 - textwidth(_T("��ѡ����Ϸģʽ��"))) / 2;
	outtextxy(titleY, 50, _T("��ѡ����Ϸģʽ��"));

	//ͬDrawMenu���裬�԰�ť���л���
	int buttonWidth = 300;
	int buttonHeight = 50;
	int buttonSpacing = 30;
	int modeButtonStartY = 150;

	const TCHAR* modeLabels[] = { _T("һ�ֶ�ʤ��"),_T("������ʤ"),_T("�����ʤ"),_T("�߾���ʤ") };
	for (int i = 0; i < 4; ++i)	//Ϊ�򻯴��룬ֱ������ѭ������һ���Ի����ĸ���ť
	{
		int buttonX = (640 - buttonWidth) / 2;
		int buttonY = modeButtonStartY + i * (buttonHeight + buttonSpacing);
		setfillcolor(RGB(200, 200, 200));
		solidrectangle(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
		settextcolor(BLACK);
		settextstyle(20, 0, _T("����"));
		int textX = buttonX + (buttonWidth - textwidth(modeLabels[i])) / 2;
		int textY = buttonY + (buttonHeight - textheight(modeLabels[i])) / 2;
		outtextxy(textX, textY, modeLabels[i]);
	}

	//���ɼ���ȡ�����״̬
	ExMessage msg;
	bool modeSelected = false;
	while (!modeSelected)
	{
		if (peekmessage(&msg, EM_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)	//ͬ
			{
				for (int i = 0; i < 4; ++i)
				{
					int buttonX = (640 - buttonWidth) / 2;
					int buttonY = modeButtonStartY + i * (buttonHeight + buttonSpacing);
					if (msg.x >= buttonX && msg.x <= buttonX + buttonWidth && msg.y >= buttonY && msg.y <= buttonY + buttonHeight)	//��ⰴ�����ĸ���ť
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
	//�������̱�����С
	setfillcolor(RGB(255, 211, 155));
	solidrectangle(10, 40, 480, 510);

	//������
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

	//�������λ
	setfillcolor(BLACK);
	solidcircle(140, 140, 4);
	solidcircle(380, 140, 4);
	solidcircle(140, 380, 4);
	solidcircle(380, 380, 4);
	solidcircle(260, 260, 4);

	//��������
	setbkmode(TRANSPARENT);
	TCHAR num[5];
	int x = 50;
	int y = 50;
	for (int i = 15; i >= 1; i--)
	{
		_stprintf_s(num, _T("%d"), i);
		settextcolor(BLACK);
		settextstyle(15, 10, _T("����"));
		int h = textheight(num);
		int w = textwidth(num);
		outtextxy(30 - w / 2, y - h / 2, num);
		y += 30;
	}
	//������ĸ
	TCHAR ch[2];
	for (int i = 0; i <15; i++)
	{
		ch[0] = _T('A') + i;
		ch[1] = _T('\0');
		settextcolor(BLACK);
		settextstyle(15, 10, _T("����"));
		int h = textheight(ch);
		int w = textwidth(ch);
		outtextxy(x - w / 2, 490 - h / 2, ch);
		x += 30;
	}
}

void DrawChess(int x, int y, bool isBlack)
{
	//���ݵ�ǰ�Ǻ��廹�ǰ�����������
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
	//�����Ļ����ת��
	cleardevice();
	setbkcolor(RGB(255, 228, 225));
	cleardevice();
	settextcolor(BLACK);
	settextstyle(30, 0, _T("����"));
	TCHAR winText[100];
	if (isGameOver)	//���������Ϸ����
	{
		//��ʾ���ջ�ʤ��Ϣ
		_stprintf_s(winText, _T("��Ϸ������%s����MVP��"), blackScore > whiteScore ? _T("����") : _T("����"));
	}
	else //�����û����
	{
		//����ʾ���ֽ������ʤ����˫���ĵ÷�
		_stprintf_s(winText, _T("���ֽ�������ǰ�ȷ֣�����%d - ����%d"), blackScore, whiteScore);
	}
	//����Ļ��ʾ���м�
	int textX = (640 - textwidth(winText)) / 2;
	int textY = 300;
	outtextxy(textX, textY, winText);

	//����ȡ��������״̬
	bool returnToMenu = false;
	while (!returnToMenu)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)	//����س���������
		{
			if (isGameOver)	//���������Ϸ������
			{
				gameRunning = false;	//ֱ���˳�
			}
			else
			{
				ResetGame();	//δ���������������һ��
				break;
			}
		}
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)	//�������Esc��
		{
			gameRunning = false;
			returnToMenu = true;	//���������沢�˳�
			break;
		}
		else if (GetAsyncKeyState('R') & 0x8000)	//�������R��
		{
			returnToMenu = true;	//��������������ѡ��
		}
		Sleep(100);
	}

	//���������沢����������Ϸ
	if (returnToMenu && gameRunning)
	{
		ResetGameForNewSession();
		DrawMenu();
	}
	
}

//��Ϸ���ܶ�

bool CheckWin(int x, int y)
{
	//����ĸ�����
	int dir[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
	int count;

	//������
	for (int d = 0; d < 4; d++)
	{
		count = 1;

		//ÿ��һ�����������Χ
		for (int i = 1; i <= 4; i++)
		{
			int nx = x + i * dir[d][0];
			int ny = y + i * dir[d][1];
			if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE || board[nx][ny] != board[x][y])	//�����Χû��������ֱ�ߵ�����
			{
				break;	//ֱ���˳�ѭ��
			}
			count++;	//�оͼ�һ
			if (count >= 5)	//�������ֱ�ߵ����Ӵﵽ���
			{
				gameOver = true;
				return true;	//��Ϸ����
			}
		}

		//�����⣬ע��ͬ��
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
	//�������̵�ÿ��λ��
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			board[i][j] = 0;	//��ÿ��λ�õ����Ӷ�����
		}
	}

	//������Ϸ״̬
	gameOver = false;	
	cleardevice();
	DrawBoard();
}

void ResetGameForNewSession()
{
	//������������
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			board[i][j] = 0;
		}
	}

	//ֱ�ӽ�������Ϸȫ������Ϊ��ʼ״̬
	gameOver = false;
	blackWin = 0;
	whiteWin = 0;
	currentRound = 1;
	isBlackTurn = true;
}

//�˻���ƶ�

int Evaluate(int x, int y, int player)
{
	//���÷��򡢵÷֡����ּ��
	int dir[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
	int score = 0;
	int opponent = (player == 1) ? 2 : 1;

	//ʹ����CheckWin�������Ƶ�������������
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

		//��������
		if (count >= 5)
		{
			score += 10000;	//������
		}
		else if (isOpenFour)
		{
			score += 1000;	//����
		}
		else if (count == 4 && (emptyBefore + emptyAfter) > 0)
		{
			score += 500;	//����
		}
		else if (isOpenThree)
		{
			score += 100;	//����
		}
		else if (count == 3 && (emptyBefore + emptyAfter) > 1)
		{
			score += 50;	//����
		}
		else if (count == 2 && (emptyBefore + emptyAfter) > 2)
		{
			score += 10;	//���
		}

		//���Ƕ�����в
		if (blockBefore == 0 && blockAfter == 0 && count >= 4)
		{
			//������ֿ����γ������飬����ܸߵĸ���
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

			if (opponentOpenFour)	//���ּ����γ�������
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
	int bestScore = -1;	//��¼��ѵ÷�
	pair<int, int> bestMove = { -1,-1 };	//��¼�������λ��
	pair<int, int>bestBlockMove = { -1,-1 };
	int blockScoreThreshold = 800;

	//����ǵ��Եĵ�һ����
	if (isFirstMove)
	{
		srand(time(0));	//��ʼ���������
		int x, y;
		//���ѡ��һ����λ������
		do
		{
			x = rand() % GRID_SIZE;	//�������x����
			y = rand() % GRID_SIZE;	//�������y����
		} while (board[x][y] != 0);	//ȷ����λ����û������
		bestMove = { x,y };
	}
	else
	{
		bool foundBlockMove = false;
		//���ȼ���Ƿ�����Ҫ���ص���в
		for (int i = 0; i < GRID_SIZE; ++i)
		{
			for (int j = 0; j < GRID_SIZE; ++j)
			{
				//ֻ���ǿ�λ��
				if (board[i][j] == 0)
				{
					//������ң����壩�ĵ÷�
					int score = Evaluate(i, j, 1);
					if (score > blockScoreThreshold)	//����ҵ÷ֳ�����ֵ����Ϊ������в
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
			//����ҵ���Ҫ���ص�λ�ã�������ѡ���λ��
			bestMove = bestBlockMove;
		}
		else
		{
			//��û����Ҫ���ص�λ�ã����������߼���������������Ѱ���������λ��
			for (int i = 0; i < GRID_SIZE; ++i)
			{
				for (int j = 0; j < GRID_SIZE; ++j)
				{
					//ֻ���ǿ�λ��
					if (board[i][j] == 0)
					{
						//������λ�õĵ÷֣�2��ʾ�������ӣ�
						int score = Evaluate(i, j, 2);
						//����ҵ����ߵ÷ֵ㣬�����������λ��
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

//��Ϸ�����

void PlayGame()
{
	bool isComputerFirstMove = true;	//����Ƿ�Ϊ�������ӵĵ�һ��

	ExMessage msg;
	//��Ϸ��ѭ������������Ϸδ��������Ϸ�����У���ǰ�غ�δ�������غ���
	while (!gameOver && gameRunning && currentRound <= MAX_ROUND[gameMode - 1])
	{
		//������˻���ս���ֵ��׷������ԣ�����
		if (isPlayerVsComputer && !isBlackTurn)
		{
			//��ȡ���Ե�����λ��
			pair<int, int> move = ComputerMove(isComputerFirstMove);
			//����Ǽ�����ĵ�һ������Ǻ��������ǵ�һ��
			if (isComputerFirstMove)
			{
				isComputerFirstMove = false;
			}

			//������������
			board[move.first][move.second] = 2;
			DrawChess(move.first, move.second, false);
			//����Ƿ��ʤ
			if (CheckWin(move.first, move.second))
			{
				whiteWin++;	//�׷�ʤ����������
				gameOver = true;	//���ֽ���
			}
			else
			{
				isBlackTurn = true;	//δʤ����ڷ�����
			}
			continue;	//��������ѭ����ʣ�ಿ��
		}

		//��ȡ���������Ϣ
		if (peekmessage(&msg, EM_MOUSE))
		{
			//��������������
			if (msg.message == WM_LBUTTONDOWN)
			{
				//����������������
				int x = (msg.x - MARTIN_X + CELL_SIZE / 2) / CELL_SIZE;
				int y = (msg.y - MARTIN_Y + CELL_SIZE / 2) / CELL_SIZE;

				//��������Ƿ���Ч�Ҹ�λ��Ϊ��
				if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && board[x][y] == 0)
				{
					if (!board[x][y])
					{
						//���ݵ�ǰ�غ����Ӷ�������������
						board[x][y] = isBlackTurn ? 1 : 2;
						DrawChess(x, y, isBlackTurn);

						//����Ƿ��ʤ
						if (CheckWin(x, y))
						{
							//����Ǻڷ�
							if (isBlackTurn)
							{
								blackWin++;	//�ڷ�ʤ����������
							}
							else
							{
								whiteWin++;	//��֮��׷���������
							}
							gameOver = true;	//������Ϸ����
						}
						else
						{
							isBlackTurn = !isBlackTurn;	//δ����ʤ������������غ�
						}

					}
				}
			}
		}
	}

	//��Ϸ��������
	if (gameOver && gameRunning)
	{
		bool isSingleGame = (gameMode == 1);
		//����Ƿ�����������Ϸ���������������ֻ�ﵽʤ��������
		if (isSingleGame || 
			(gameMode == 2 && (blackWin == 2 || whiteWin == 2)) ||
			(gameMode == 3 && (blackWin == 3 || whiteWin == 3)) ||
			(gameMode == 4 && (blackWin == 4 || whiteWin == 4)))
		{
			gameOver = true;
			//��ʾ����ʤ����Ϣ
			ShowWinText(blackWin, whiteWin, true);
		}
		else
		{
			currentRound++;	//��ǰ�غ�������
			gameOver = false;	//������Ϸ������־
			//��ʾ��ǰ����������Ϣ
			ShowWinText(blackWin, whiteWin, false);
			isBlackTurn = true;	//����Ϊ�ڷ�����
			ResetGame();	//���õ�ǰһ����Ϸ״̬
		}
	}
}



//����

//���������
int main()
{
	//��ʼ��ͼ�δ��ڣ�640*640���أ�
	initgraph(640, 640);
	setbkmode(WHITE);
	cleardevice();

	//������Ϸ�˵�
	DrawMenu();

	//��Ϸ��ѭ��
	while (gameRunning)
	{
		DrawBoard();	//��������
		PlayGame();		//��ʼһ����Ϸ
	}

	closegraph();	//�ر�ͼ�δ���
	return 0;
}
