#include <iostream>
#include <deque>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

typedef struct Knight {
    int r, c, h, w, hp;
    int cur_hp;
    bool is_alive;
} knight;

int L, N, Q;
int m[50][50] = { 0, };
vector<knight> knights;
int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};


bool is_inside(int r, int c, int h, int w)
{
    return (0 <= r && r + h <= L && 0 <= c && c + w <= L);
}

bool color_map(int c_map[50][50], int n, int d)
{
//    cout << n << ' ' << knights[n].r << ' ' << knights[n].c << ' ' << knights[n].h << ' ' << knights[n].w << '\n';
    int start_x = knights[n].r + dx[d];
    int end_x = start_x + knights[n].h;
    int start_y = knights[n].c + dy[d];
    int end_y = start_y + knights[n].w;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            if (start_x <= i && i < end_x && start_y <= j && j < end_y) {
                if (m[i][j] == 2)
                    return false;
                c_map[i][j] = 1;
            }
            else
                c_map[i][j] = 0;
        }
    }
    
//    cout << n << "번 기사로 색칠한 맵: \n";
//    for (int i = 0; i < L; i++) {
//        for (int j = 0; j < L; j++) {
//            cout << c_map[i][j] << ' ';
//        }
//        cout << '\n';
//    }
    
    return true;
}

bool is_collide(int c_map[50][50], int cur, int n)
{
    int start_x = knights[n].r;
    int end_x = start_x + knights[n].h;
    int start_y = knights[n].c;
    int end_y = start_y + knights[n].w;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            if (start_x <= i && i < end_x && start_y <= j && j < end_y) {
                if (c_map[i][j] == 1)
                    return true;
            }
        }
    }
    return false;
}

bool is_pushable(int n, int d)
{
//    cout << "is_pushable 확인중\n";
    deque<int> to_push;
    to_push.push_back(n);
    while (to_push.empty() == false) {
        int cur = to_push.front();
        to_push.pop_front();
        if (!is_inside(knights[cur].r + dx[d], knights[cur].c + dy[d], knights[cur].h, knights[cur].w)) {
            // 만약 밀었을때 맵 밖으로 나가면 return false
//            cout << "맵 밖으로 나가서 불가능\n";
            return false;
        }
        int colored_map[50][50];
        if (color_map(colored_map, cur, d) == false) {
//            cout << "기사가 이동했을때 벽이 있어서 불가능\n";
            return false;
        }
        for (int i = 0; i < N; i++) { // 기사들을 차례로 돌면서
            if (i == cur)
                continue;
            if (knights[i].is_alive == false)
                continue;
            if (is_collide(colored_map, cur, i)) {
//                cout << cur << "과 " << i << "가 충돌해서 덱에 추가\n";
                to_push.push_back(i);
            }
        }
    }
    
    return true;
}

set<int> push_knight(int n, int d)
{
    set<int> pushed;
    deque<int> to_push;
    to_push.push_back(n);
    while (to_push.empty() == false) {
        int cur = to_push.front();
        to_push.pop_front();
        int colored_map[50][50];
        color_map(colored_map, cur, d);
        for (int i = 0; i < N; i++) { // 기사들을 차례로 돌면서
            if (i == cur)
                continue;
            if (knights[i].is_alive == false)
                continue;
            if (is_collide(colored_map, cur, i)) {
                to_push.push_back(i);
            }
        }
        knights[cur].r += dx[d];
        knights[cur].c += dy[d];
        pushed.insert(cur);
    }
    
    return pushed;
}

void cal_damage(set<int> pushed, int n)
{
    set<int>::iterator iter;
    for (iter = pushed.begin(); iter != pushed.end(); iter++) {
        int cur = *iter;
        int damage = 0;
        if (cur != n) {
            // 데미지 계산
            int start_x = knights[cur].r;
            int end_x = start_x + knights[cur].h;
            int start_y = knights[cur].c;
            int end_y = start_y + knights[cur].w;
            for (int i = 0; i < L; i++) {
                for (int j = 0; j < L; j++) {
                    if (start_x <= i && i < end_x && start_y <= j && j < end_y) {
                        if (m[i][j] == 1)
                            damage++;
                    }
                }
            }
            
            knights[cur].cur_hp -= damage;
            if (knights[cur].cur_hp <= 0)
                knights[cur].is_alive = false;
        }
    }
}

int main(void)
{
    cin >> L >> N >> Q;
    
    // get map info
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            cin >> m[i][j];
        }
    }
    
    // get knights info
    for (int i = 0; i < N; i++) {
        knight k;
        cin >> k.r >> k.c >> k.h >> k.w >> k.hp;
        k.r--;
        k.c--;
        k.cur_hp = k.hp;
        k.is_alive = true;
        knights.push_back(k);
    }
    
    // get commands
    for (int i = 0; i < Q; i++) {
//        cout << "\n" << i << "번 명령\n";
        int n, d;
        cin >> n >> d;
        n--;
        if (knights[n].is_alive == false) // 기사가 현재 맵에 없으면 다음 명령 받기
            continue;
        if (is_pushable(n, d)) { // 기사를 해당 방향으로 밀 수 있는지 확인
//            cout << i << "번 명령에서 밀기 가능\n";
            set<int> pushed;
            pushed = push_knight(n, d);// 가능하면 밀기
            cal_damage(pushed, n); // 밀고 난 후 데미지 계산하기
        }
    }
    
    int answer = 0;
    for (int i = 0; i < N; i++) {
        if (knights[i].is_alive) {
            answer += (knights[i].hp - knights[i].cur_hp);
        }
    }
    
    cout << answer << '\n';
    
    return 0;
}