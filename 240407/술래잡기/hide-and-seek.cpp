#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

typedef struct Chaser_info {
    int dir; // 술래가 바라보는 방향 0123 - 상우하좌
    int x;
    int y;
    int score;
    int spin; // 0: 시계(바깥으로), 1: 반시계(안으로)
    int length; // 그리는 사각형의 변의 길이 저장, 3, 5, 7,,,
    int move_cnt; // 변의 길이가 변하지 않았을 때 이동한 횟수 저장
} chaser_info;

typedef struct Tree_info {
    int x;
    int y;
} tree_info;

typedef struct Runner_info {
    int index;
    bool alive;
    int type; // 1: left_right, 2: up_down
    int dir;
    int x;
    int y;
} runner_info;

int n, m, h, k; // n: 격자 크기, m: 도망자 수, h: 나무 수, k: 턴 수
chaser_info chaser;
vector<tree_info> trees;
vector<runner_info> runners;
int tree_map[99][99] = {0, }; // 나무가 있으면 1, 없으면 0
int dx[] = {-1, 0, 1, 0}; // 상 우 하 좌
int dy[] = {0, 1, 0, -1};

void print_map(void)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (chaser.x == i && chaser.y == j)
                cout << "C" << chaser.dir << ' ';
            else
                cout << "0  ";
        }
        cout << '\n';
    }
    
    cout << '\n';
}

void move_chaser(void)
{
    if (chaser.x == n / 2 && chaser.y == n / 2) { // 시작점일때
        chaser.x += dx[chaser.dir];
        chaser.y += dy[chaser.dir];
        chaser.spin = 0;
        chaser.dir = RIGHT;
        chaser.length = 3;
        chaser.move_cnt = 1;
        return;
    }
    
    if (chaser.spin == 0) { // 시계방향 스핀이면
        if (chaser.move_cnt < chaser.length * 4 - 4) { // 아직 같은 변의 길이에서 움직이고 있을 때
            if (abs(chaser.x + dx[chaser.dir] - n / 2) > chaser.length / 2 || abs(chaser.y + dy[chaser.dir] - n / 2) > chaser.length / 2) { // 방향 바꿔줘야 할 때
                // cout << "방향 바꾸기\n";
                chaser.dir = (chaser.dir + 1) % 4;
            }
            chaser.x += dx[chaser.dir];
            chaser.y += dy[chaser.dir];
            chaser.move_cnt++;
        }
        if (chaser.move_cnt == chaser.length * 4 - 4) { // 변의 길이를 바꿔줘야 할 때
            if (chaser.x == 0 && chaser.y == 0) {
//                cout << "반시계\n";
                chaser.spin = 1; // 반시계로 바꿔야함
                chaser.dir = DOWN;
                chaser.move_cnt = 1;
                return;
            }
            else {
                chaser.length += 2;
                chaser.move_cnt = 0;
            }
        }
        if (abs(chaser.x + dx[chaser.dir] - n / 2) > chaser.length / 2 || abs(chaser.y + dy[chaser.dir] - n / 2) > chaser.length / 2) { // 방향 바꿔줘야 할 때
//            cout << "방향 바꾸기\n";
            chaser.dir = (chaser.dir + 1) % 4;
        }
    }
    
    else { // 반시계방향 스핀이면
        if (chaser.move_cnt < chaser.length * 4 - 4) { // 아직 같은 변의 길이에서 움직이고 있을 때
            if (abs(chaser.x + dx[chaser.dir] - n / 2) > chaser.length / 2 || abs(chaser.y + dy[chaser.dir] - n / 2) > chaser.length / 2) { // 방향 바꿔줘야 할 때
                // cout << "방향 바꾸기\n";
                chaser.dir = (chaser.dir - 1 + 4) % 4;
            }
            chaser.x += dx[chaser.dir];
            chaser.y += dy[chaser.dir];
            chaser.move_cnt++;
            if (chaser.x == (n / 2) - 1 && chaser.y == n / 2) {
//                chaser.x = n / 2;
//                chaser.y = n / 2;
                chaser.dir = 2;
                // return;
            }
        }
        if (chaser.move_cnt == chaser.length * 4 - 4) { // 변의 길이를 바꿔줘야 할 때
            if (chaser.x == n / 2 && chaser.y == n / 2) {
                chaser.dir = 0;
                return;
                // chaser.spin = 0; // 시계로 바꿔야함
            }
            else if (chaser.x == n / 2 - 1 && chaser.y == n / 2) {
                chaser.length = 3;
                chaser.move_cnt = 7;
            }
            else {
                chaser.length -= 2;
                if (chaser.length <= 0)
                    chaser.length = 1;
                chaser.move_cnt = 0;
            }
        }
        if (abs(chaser.x + dx[chaser.dir] - n / 2) > chaser.length / 2 || abs(chaser.y + dy[chaser.dir] - n / 2) > chaser.length / 2) { // 방향 바꿔줘야 할 때
            chaser.dir = (chaser.dir - 1 + 4) % 4;
        }
    }
}

int dist(runner_info r)
{
    return (abs(r.x - chaser.x) + abs(r.y - chaser.y));
}

bool is_inside(int x, int y) {
    if (0 <= x && x < n && 0 <= y && y < n)
        return true;
    else
        return false;
}

void move_runner(runner_info& r)
{
    if (r.type == 1) { // 좌우 이동이면
        if (!is_inside(r.x + dx[r.dir], r.y + dy[r.dir])) {
            if (r.dir == RIGHT)
                r.dir = LEFT;
            else
                r.dir = RIGHT;
        }
    }
    
    else { // 상하 이동이면
        if (!is_inside(r.x + dx[r.dir], r.y + dy[r.dir])) {
            if (r.dir == UP)
                r.dir = DOWN;
            else
                r.dir = UP;
        }
    }
    
    if (!(r.x + dx[r.dir] == chaser.x && r.y + dy[r.dir] == chaser.y)) {
//        cout << "움직임\n";
        r.x += dx[r.dir];
        r.y += dy[r.dir];
    }
}

void seek(int turn)
{
    int kill = 0;
    
    int x_start = chaser.x;
    int x_end = chaser.x + 2 * dx[chaser.dir];
    int y_start = chaser.y;
    int y_end = chaser.y + 2 * dy[chaser.dir];
    
    if (x_start > x_end) {
        int tmp = x_start;
        x_start = x_end;
        x_end = tmp;
    }
    if (y_start > y_end) {
        int tmp = y_start;
        y_start = y_end;
        y_end = tmp;
    }
    
    if (x_start < 0)
        x_start = 0;
    if (x_end > n)
        x_end = n - 1;
    if (y_start < 0)
        y_start = 0;
    if (y_end > n)
        y_end = n - 1;
    
    for (int r = 0; r < m; r++) {
        if (runners[r].alive == false)
            continue;
        if ((x_start <= runners[r].x && runners[r].x <= x_end) && (y_start <= runners[r].y && runners[r].y <= y_end)) {// 만약 술래의 시야에 있으면
//            cout << r << "걸렸다\n";
            if (tree_map[runners[r].x][runners[r].y] == 0) { // 그 자리에 나무가 없으면
                runners[r].alive = false; // 죽는다
                kill++; // 죽인 개수를 올려준다
            }
        }
    }
            
    chaser.score += turn * kill;
}

int main(void)
{
    cin >> n >> m >> h >> k;
    
    // 술래 초기 정보 설정
    chaser.dir = UP;
    chaser.x = chaser.y = n / 2; // n이 반드시 홀수니까
    chaser.score = 0;
    chaser.spin = 0; // 시계방향으로 바깥으로 나가는걸로 시작
    chaser.length = 3;
    chaser.move_cnt = 0;
    
    for (int i = 0; i < m; i++) {
        runner_info r;
        r.index = i;
        cin >> r.x >> r.y >> r.type;
        r.x -= 1;
        r.y -= 1;
        if (r.type == 1) // 좌우
            r.dir = RIGHT;
        else // 상하
            r.dir = DOWN;
        r.alive = true;
        runners.push_back(r);
    }
    
    for (int i = 0; i < h; i++) {
        int x, y;
        cin >> x >> y;
        tree_map[x - 1][y - 1] = 1;
    }
    
    for (int i = 0; i < k; i++) {
//        cout << "TURN:" << i << ", SCORE: " << chaser.score << '\n';
//        cout << chaser.length << ' ' << chaser.move_cnt << '\n';
//        print_map();
        for (int r = 0; r < m; r++) {
            if (runners[r].alive == true && dist(runners[r]) <= 3)
                move_runner(runners[r]);
        }
        move_chaser();
        seek(i + 1);
    }
    
    cout << chaser.score << '\n';
    
    return 0;
}