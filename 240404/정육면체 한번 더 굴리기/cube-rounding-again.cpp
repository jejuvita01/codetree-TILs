#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3

int n, m;
vector<int> map[20];
int pos_x, pos_y;
int new_x, new_y;
int direction; // 0: 오, 1: 왼, 2: 상, 3: 하
int dx[] = {0, 0, -1, 1}; // 오, 왼, 상, 하
int dy[] = {1, -1, 0, 0}; // 오, 왼, 상, 하
int cur_dice_top; // 현재 주사위의 가장 윗면
vector<int> dice; // 주사위의 각각 오, 왼, 상, 하에 무엇이 있는지를 저장한 벡터
int visited[20][20] = {0, };
int answer = 0;

void init_visited(void)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            visited[i][j] = 0;
}

bool is_inside(int new_x, int new_y)
{
    if (0<= new_x && new_x < n && 0 <= new_y && new_y < n)
        return true;
    else
        return false;
}

void move_dice()
{
    new_x = pos_x + dx[direction];
    new_y = pos_y + dy[direction];
    
    if (!is_inside(new_x, new_y)) { // 맵을 벗어날때
        switch (direction) {
            case RIGHT: direction = LEFT; break;
            case LEFT: direction = RIGHT; break;
            case UP: direction = DOWN; break;
            case DOWN: direction = UP; break;
            default: break;
        };
        new_x = pos_x + dx[direction];
        new_y = pos_y + dy[direction];
    }
    
    pos_x = new_x;
    pos_y = new_y;
    
    cur_dice_top = dice[(cur_dice_top - 1 + direction) % 6];
}

void dfs(int n, int x, int y)
{
    if (visited[x][y]) return;
    
    visited[x][y] = 1;
    
    for (int i = 0; i < 4; i++) {
        int xx, yy;
        xx = x + dx[i];
        yy = y + dy[i];
        if (is_inside(xx, yy)) {
            if (map[xx][yy] == n)
                dfs(n, xx, yy);
        }
    }
}

int cnt_visited()
{
    int cnt = 0;
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (visited[i][j] == 1)
                cnt++;
    
    return cnt;
}

int get_score()
{
    int score = 0;
    int num = map[pos_x][pos_y];

    dfs(num, pos_x, pos_y);
    score = num * cnt_visited();
    
    return score;
}

int get_direction(void)
{
    int cur_dice_bottom = 7 - cur_dice_top;
    int cur_map_num = map[pos_x][pos_y];
    
    if (cur_dice_bottom > cur_map_num) {
        switch (direction) { // 시계방향 90도
            case RIGHT: direction = DOWN; break;
            case LEFT: direction = UP; break;
            case UP: direction = RIGHT; break;
            case DOWN: direction = LEFT; break;
            default: break;
        };
    }
    
    else if (cur_dice_bottom < cur_map_num) {
        switch (direction) { // 반시계방향 90도
            case RIGHT: direction = UP; break;
            case LEFT: direction = DOWN; break;
            case UP: direction = LEFT; break;
            case DOWN: direction = RIGHT; break;
            default: break;
        };
    }
    
    else
        direction = direction;
    
    return direction;
        
}

int main(void)
{
    // 주사위 정보 저장 (윗면, 오, 왼, 상, 하, 바닥 에 무엇이 있는지)
    dice.push_back(1); dice.push_back(3); dice.push_back(4); dice.push_back(5); dice.push_back(2); dice.push_back(6);
    cur_dice_top = 1;
    
    cin >> n >> m;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i].push_back(x);
        }
    }
    
    pos_x = pos_y = 0;
    direction = RIGHT;
    
    for (int i = 0; i < m; i++) {
        move_dice();
        init_visited();
        answer += get_score();
        direction = get_direction();
    }
    
    cout << answer << '\n';
    
    return 0;
}