#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>

typedef struct
{
    int _headX, _headY;
    int _shieldNum;
    int _nowDrct;
}Snake;

int MapX = 6;
int MapY = 6;
char* headMark1 = "9290";
char* headMark2 = "-9290";
char* neckMark = "1922290";
char* bodyMark = "1922290";
int MaxDepth = 12;
int INF = 1000000;
int shieldValue = 3;
int longerValue = 5;
int walk[8][2] = {{0, -1}, {-1, 0},
                          {0, 1}, {1, 0},
                          {-1, -1}, {1, 1},
                          {-1, 1}, {1, -1}};

int map[30][40];
int flag[30][40];
Snake mySnake;

void int_to_string(int num, char* p)//整数转字符串（itoa不能用！WAW）
{
    int num_list[12];
    int k = 0;
    if (num<0)
    {
        p[0] = '-';
        k++;
    }
    num = abs(num);
    int n = 0;
    while (num>0)
    {
        num_list[n++] = num%10;
        num = num/10;
    }
    for (int i=n-1; i>=0; i--)
        p[k++] = num_list[i] + '0';
    p[k] = '\0';
}

int max(int a, int b) {return a>b?a:b;}

bool if_out(int x, int y)//判断是否出界
{
    if (x<0 || x>=MapX || y<0 || y>=MapY) return true;
    else return false;
}

bool drct_conflict(int drct1, int drct2)//判断方向是不是冲突
{
    return (abs(drct1 - drct2) == 2 && drct1 != 4 && drct2 != 4)?true:false;
}

Snake snake_move(Snake s, int k)//向k方向移动，生成下一个状态
{
    s._headX += walk[k][0];
    s._headY += walk[k][1];
    s._nowDrct = k;
    return s;
}

void findMySnake()
{
    for (int i=0; i<MapX; i++)
        for (int j=0; j<MapY; j++)
        {
            char crnt_num[12];
            int_to_string(map[i][j], crnt_num);
            if (strstr(crnt_num, headMark1) != NULL && crnt_num[0] != '-')//找到头，没开盾的头
            {
                mySnake._headX = i;
                mySnake._headY = j;
                mySnake._nowDrct = crnt_num[strlen(crnt_num)-1] - '0';
                map[i][j] = 0;
            }
            else if (strstr(crnt_num, headMark2) != NULL)//开了盾的头，这里也有坑
            {
                mySnake._headX = i;
                mySnake._headY = j;
                mySnake._nowDrct = crnt_num[strlen(crnt_num)-2] - '0';
                map[i][j] = 0;
            }
            else if (strstr(crnt_num, neckMark) != NULL && strlen(crnt_num) > 7)
            {
                int num = 0;
                int len = strlen(crnt_num);
                for (int k = 7; k<len; k++)
                    num = num * 10 + (crnt_num[k] - '0');
                mySnake._shieldNum = num;
                map[i][j] = 0;
            }
            else if (strstr(crnt_num, bodyMark) != NULL) map[i][j] = 0;
        }
}


bool if_other_snake(int x, int y)//判断这个位置是不是别的蛇
{
    char crnt_num[12];
    int_to_string(map[x][y], crnt_num);
    if (strlen(crnt_num) >= 5 && strcmp(crnt_num, "-10000") != 0) return true;
    return false;
}


bool check_ard(Snake snk)//检查周围有没有蛇头
{
    int x = snk._headX, y = snk._headY;
    for (int i=0; i<8; i++)
    {
        int xx = x + walk[i][0], yy = y + walk[i][1];
        if (if_out(xx, yy)) continue;
        char s[12];
        int_to_string(map[xx][yy], s);
        if (strlen(s) == 5) return true;
        if (strlen(s) == 7 && s[0] == '-') return true;
    }
    return false;
}


bool check_edge(Snake snk)
{
    int dx = abs(snk._headX - MapX/2),
         dy = abs(snk._headY - MapY/2);
    if (snk._shieldNum < 2)
        if (dx>18 || dy > 13) return true;
    return false;
}


int evaluator(Snake snk, int mapPoint, int depth)//评估函数
{
    int plus_value, minus_value = 0;

    if (mapPoint == -10000) plus_value = shieldValue;
    else if (mapPoint == -100) plus_value = longerValue;
    else plus_value = abs(mapPoint);
    plus_value *= (MaxDepth - depth);

    if (check_ard(snk)) minus_value = -100;//周围有别的蛇头很容易出事
    if (check_edge(snk)) minus_value = minus_value - 10;//走到地图边缘也很容易出事
    return plus_value + minus_value;
}


double count_enemy_density(Snake snk)
{
    double enemy_num = 0, square = 0;
    for (int i=0; i<MapX; i++)
        for (int j=0; j<MapY; j++)
        {
            int dx = snk._headX - i, dy = snk._headY - j;
            int dist = dx*dx + dy*dy;
            if (dist <= 6)
            {
                square = square + 1;
                if (if_other_snake(i, j)) enemy_num = enemy_num + 1;
            }
        }
    return enemy_num/square;
}


int dfs(Snake snk, int value, int depth)
{
    if (depth > MaxDepth) return value;
    int maxValue = -INF;
    for (int i=0; i<4; i++)
        if (!drct_conflict(i, snk._nowDrct))
        {
            Snake next = snake_move(snk, i);
            if (if_out(next._headX, next._headY)) continue;
            if (if_other_snake(next._headX, next._headY)) continue;
            if (flag[next._headX][next._headY] == -1) continue;

            int k = map[next._headX][next._headY];
            map[next._headX][next._headY] = 0;
            flag[next._headX][next._headY] = -1;

            maxValue = max(maxValue, dfs(next, value + evaluator(next, k, depth), depth+1));
            map[next._headX][next._headY] = k;
            flag[next._headX][next._headY] = 0;
        }
    return maxValue;
}



int Search()//第一层搜索要根据后续搜索做决策，所以单独拿出来
{
    if (mySnake._shieldNum > 0)//有盾
    {
        double enemy_density = count_enemy_density(mySnake);
        if (check_ard(mySnake)) return 4;//八个方向检查到蛇头
        if (enemy_density >= 0.5) return 4;//周围敌人的密度大于0.5
    }
    memset(flag, 0, sizeof (flag));
    flag[mySnake._headX][mySnake._headY] = -1;
    int maxValue = -INF;
    int choice = mySnake._nowDrct;

    for (int i=0; i<4; i++)
        if (!drct_conflict(i, mySnake._nowDrct))
        {
            Snake next = snake_move(mySnake, i);
            if (if_out(next._headX, next._headY)) continue;
            if (if_other_snake(next._headX, next._headY)) continue;

            int k = map[next._headX][next._headY];
            map[next._headX][next._headY] = 0;
            flag[next._headX][next._headY] = -1;
            int thisValue = dfs(next, evaluator(next, k, 0), 1);
            flag[next._headX][next._headY] = 0;
            map[next._headX][next._headY] = k;

            if (thisValue > maxValue)
            {
                maxValue = thisValue;
                choice = i;
            }
        }
    if (maxValue < 0 && mySnake._shieldNum > 0) return 4;
    return choice;
}


int main()
{
    for (int i=0; i<MapX; i++)
        for (int j=0; j<MapY; j++)
            scanf("%d", &map[i][j]);
    findMySnake();
    int choice = Search();
    printf("%d", choice);
    return 0;
}
