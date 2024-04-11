#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <cmath>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

using namespace std;

typedef struct Player_info {
    int x;
    int y;
    int power;
    int gun;
    int dir; // 상 우 하 좌
    int score;
} player_info;

int n, m, k;
int turn;
deque<int> gun[20][20];
vector<player_info> player;
int dx[] = {-1, 0, 1, 0}; // 상 우 하 좌
int dy[] = {0, 1, 0, -1};

void print_map(void)
{
    cout << "\nMAP\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int idx = -1;
            for (int p = 0; p < m; p++) {
                if (player[p].x == i && player[p].y == j) {
                    idx = p;
                    break;
                }
            }
            
            if (idx != -1) {
                cout << "U" << idx << ' ';
            }
            else
                cout << "0  ";
        }
        cout << '\n';
    }
}

bool comp(int a, int b)
{
    return a > b;
}

void get_gun(int p, int x, int y)
{
    if (gun[x][y].size() == 0)
        return;
    
    // 그 자리에 있는 gun들을 내림차순으로 정렬한다
    sort(gun[x][y].begin(), gun[x][y].end(), comp);
    
    if (player[p].gun == 0) { // 사용자가 gun이 없으면
        player[p].gun = gun[x][y].front(); // 제일 큰 gun을 가진다
        gun[x][y].pop_front();
    }
    else { // 사용자가 gun이 있으면
        // 자신의 gun과 자리의 gun중 가장 큰 것을 비교한다
        // 자기 총이 크면 가만히 있는다
        if (gun[x][y].size() != 0) {
            if (player[p].gun < gun[x][y].front()) { // 자리의 총이 크면 swap
                int tmp = player[p].gun;
                player[p].gun = gun[x][y].front();
                gun[x][y][0] = tmp;
            }
        }
    }
}

bool is_inside(int x, int y)
{
    if (0 <= x && x < n && 0 <= y && y < n)
        return true;
    else
        return false;
}

void move_loser(int idx)
{
    int next_x, next_y;
    
    for (int i = 0; i < 4; i++) {
        next_x = player[idx].x + dx[(player[idx].dir + i) % 4];
        next_y = player[idx].y + dy[(player[idx].dir + i) % 4];
        // cout << "LOSER 이동 후보" << i + 1 << ": " << next_x << ',' << next_y << '\n';
        if (is_inside(next_x, next_y)) { // 격자 내에 있고
            bool is_player = false; // 다른 플레이어 있는지 확인
            for (int p = 0; p < m; p++) {
                if (p != idx) { // 본인이 아니고
                    if (player[p].x == next_x && player[p].y == next_y) { // 이동할 곳과 좌표가 같으면
                        // cout << "좌표 같아서 안됨\n";
                        is_player = true; // 불리안 업데이트
                    }
                }
            }
            if (is_player == false) {// 다른 플레이어 없으면
                // 바로 이동
                player[idx].x = next_x; // idx의 좌표값을 업데이트 하고
                player[idx].y = next_y;
                // cout << "LOSER의 위치를 " << player[idx].x << ',' << player[idx].y << "로 옮겼다\n";
                player[idx].dir = (player[idx].dir + i) % 4; // dir 업데이트 하고
                return; // 리턴
            }
        }
    }
    
}

void fight(int a, int b)
{
    int winner, loser;
    
    winner = loser = -1;
    
    // 총 공격력을 비교한다
    int a_total = player[a].power + player[a].gun;
    int b_total = player[b].power + player[b].gun;
    int diff = abs(a_total - b_total);
//    cout << "DIFF: " << diff << '\n';
    if (diff > 0) {// 총 공격력이 차이나면
        if (a_total > b_total) {// 더 큰 사람을 winner, 작은 사람을 loser로 업데이트
            winner = a;
            loser = b;
        }
        else {
            winner = b;
            loser = a;
        }
    }
    else {// 총 공격력이 같으면
        // power를 비교한다
        // 더 큰 사람을 winner, 작은 사람을 loser로 업데이트
        if (player[a].power > player[b].power) {
            winner = a;
            loser = b;
        }
        else {
            winner = b;
            loser = a;
        }
    }
    
    player[winner].score += diff; // 이긴 사람의 점수를 차이만큼 올려준다
    // cout << "WINNER: " << winner  << ", LOSER: " << loser << ", SCORE: " << player[winner].score << '\n';
    gun[player[loser].x][player[loser].y].push_back(player[loser].gun); // 진 사람의 총을 내려놓는다
    player[loser].gun = 0;
    
    get_gun(winner, player[winner].x, player[winner].y); // 이긴 사람은 get_gun
    
    move_loser(loser); // 진 사람은 이동한다
    get_gun(loser, player[loser].x, player[loser].y); // 진 사람도 get_gun
}

void move_user(int p)
{
    // 일단 플레이어를 이동시킨다
    player[p].x += dx[player[p].dir];
    player[p].y += dy[player[p].dir];
    
    if (!is_inside(player[p].x, player[p].y)) {// 격자 밖으로 벗어나면
        player[p].dir = (player[p].dir + 2) % 4; // 플레이어의 방향을 바꾸고
        player[p].x += 2 * (dx[player[p].dir]); // 반대로 이동시킨다
        player[p].y += 2 * (dy[player[p].dir]);
    }
    
    // 이동시킨곳에 플레이어가 있는지 확인
    int is_player = -1;
    for (int i = 0; i < m; i++) {
        if (i != p) { // 본인이 아니고
            if (player[i].x == player[p].x && player[i].y == player[p].y) {
                is_player = i;
                break;
            }
        }
    }
    
    if (is_player != -1) { // 플레이어가 있으면
//        cout << p << "와 " << is_player <<  "가 싸운다\n";
        fight(p, is_player); // 싸운다
    }
    
    else // 없으면
         get_gun(p, player[p].x, player[p].y); // 총이 있다면 비교해서 가지거나 가만히 있는다
}

int main(void)
{
    cin >> n >> m >> k;
    
    // 맵 정보 입력 받으며 각 위치별로 총 push_back 해주기
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            if (x > 0) { // 총이 있으면 push_bak
                gun[i][j].push_back(x);
            }
        }
    }
    
    // player 정보 입력
    for (int i = 0; i < m; i++) {
        player_info p;
        cin >> p.x >> p.y >> p.dir >> p.power;
        p.x -= 1;
        p.y -= 1;
        p.gun = 0;
        p.score = 0;
        player.push_back(p);
    }
    
    // K 턴 반복
    for (turn = 0; turn < k; turn++) {
        // cout << "\n\nTURN: " << turn + 1 << '\n';
        for (int p = 0; p < m; p++) {// 유저를 모두 이동시킨다
            move_user(p);
        }
        // print_map();
        
        // cout << "SCORE\n";
        // for (int i = 0; i < m; i++)
        //     cout << player[i].score << ' ';
    }
    
    for (int i = 0; i < m; i++)
        cout << player[i].score << ' ';
    
    return 0;
}