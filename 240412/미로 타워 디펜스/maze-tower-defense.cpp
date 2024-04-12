#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

typedef struct Point {
    int x;
    int y;
} point;

int n, m;
int max_deque_size;
point player;
deque<int> monster;
int map[25][25];
int dx[] = {0, 1, 0, -1}; // 우 하 좌 상
int dy[] = {1, 0, -1, 0};
int score = 0;


bool is_inside(int x, int y)
{
    if (0 <= x && x < n && 0 <= y && y < n)
        return true;
    else
        return false;
}

void print_deque(void)
{
    cout << "\nDEQUE SIZE: " << monster.size() << '\n';
    for (int i = 0; i < monster.size(); i++)
        cout << monster[i] << ' ';
    cout << '\n';
}

void print_map(void)
{
    cout << "\nMAP\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
}

void map_to_deque(void)
{
    monster.clear();
    
    int total_move = 0;
    int length = 1;
    int dir = LEFT;
    int x, y;
    
    x = y = n / 2;
    while (!(x == 0 && y == 0)) {
        while (total_move < length) {
            x = x + dx[dir];
            y = y + dy[dir];
            monster.push_back(map[x][y]);
            total_move++;
            //            cout << "x:" << x << ", y:" << y << ", dir:" << dir << '\n';
            if (x == 0 && y == 0)
                break;
        }
        dir = (dir -1 + 4) % 4;
        total_move = 0;
        if (dir == LEFT || dir == RIGHT)
            length++;
    }
    
}

void deque_to_map(void)
{
    int idx = 0;
    int total_move = 0;
    int length = 1;
    int dir = LEFT;
    int x, y;
    
    x = y = n / 2;
    while (!(x == 0 && y == 0)) {
        while (total_move < length) {
            x = x + dx[dir];
            y = y + dy[dir];
            map[x][y] = monster[idx++];
            total_move++;
            // cout << "x:" << x << ", y:" << y << ", dir:" << dir << '\n';
            if (x == 0 && y == 0)
                break;
        }
        dir = (dir -1 + 4) % 4;
        total_move = 0;
        if (dir == LEFT || dir == RIGHT)
            length++;
    }
}

void attack(int d, int p) {
    for (int i = 1; i <= p; i++) { // p 칸만큼
        int x = n / 2 + i * dx[d];
        int y = n / 2 + i * dy[d];
        score += map[x][y];
        map[x][y] = 0;
    }
}

void move_deque(void)
{
    int non_zero_cnt = 0;
    
    for (int i = 0; i < max_deque_size; i++) {
        int tmp = monster.front();
        monster.pop_front(); // 일단 제일 앞에껄 꺼내서
        if (tmp != 0) { // 0이 아니면 push_back 해준다
            monster.push_back(tmp);
            non_zero_cnt++;
        }
    }
    
    for (int i = 0; i < max_deque_size - non_zero_cnt; i++)
        monster.push_back(0);
}

void delete_dup(void)
{
    bool is_deleted = false;
    int cur_num = 0;
    int cnt = 0;
    int to_put_zero = 0;
    
    for (int i = 0; i < max_deque_size; i++) {
        int tmp = monster.front();
        monster.pop_front();
        if (tmp != 0 && tmp == cur_num) { // 만약 전의 것과 같으면
            cnt++; // cnt를 올려준다
        }
        else if (tmp != cur_num) {// 만약 전의 것과 다르면
            if (cnt >= 4) {// cnt가 4 이상이면
                to_put_zero += cnt; // to_put_zero를 cnt만큼 올려준다
                score += cur_num * cnt;
                is_deleted = true;
            }
            else {// cnt가 4 이하면
                for (int c= 0; c < cnt; c++) // cnt만큼 다시 monster에 push_back 해준다
                    monster.push_back(cur_num);
            }
            // 두 경우 모두
            cur_num = tmp;
            cnt = 1;
        }
        else if (tmp == 0)
            to_put_zero++;
    }
    
    while (monster.size() < max_deque_size)
        monster.push_back(0);
    
    if (is_deleted)
        delete_dup();
}

void design_deque(void)
{
    int cur_num = 0;
    int cnt = 0;
    
    for (int i = 0; i < max_deque_size; i++) {
        int tmp = monster.front();
        monster.pop_front();
        if (tmp == 0) {
            monster.push_back(cnt);
            monster.push_back(cur_num);
            break;
        }
        else if (tmp != cur_num) {// 나온게 현재 숫자랑 다르면
            monster.push_back(cnt); // 덱에 cnt, cur_num 순서대로 넣어주기
            monster.push_back(cur_num);
            cur_num = tmp;
            cnt = 1;
        }
        else if (tmp == cur_num) // tmp가 현재 숫자와 같으면
            cnt++;
    }
    
    while (monster.front() == 0) {
        monster.pop_front();
        monster.push_back(0);
    }
    
    while (monster.size() > max_deque_size)
        monster.pop_back();
}

int main(void)
{
    cin >> n >> m;
    
    // 플레이어의 위치는 정중앙
    player.x = n / 2;
    player.y = n / 2;
    
    // 덱의 최대 크기
    max_deque_size = n * n - 1; // 플레이어를 제외한 나머지 격자들의 개수
    
    // 맵 정보 입력받기
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
    // 맵의 정보를 덱에 입력하기
    map_to_deque();
//    print_map();
//    print_deque();
    
    // 라운드만큼 공격하기
    for (int i = 0; i < m; i++) {
        int d, p;
        cin >> d >> p; // 공격 방향과 칸 수
        attack(d, p); // 공격
        map_to_deque(); // 공격받은 맵의 결과를 덱에 넣는다
        move_deque(); // 공격받은 몬스터를 없애고 덱을 앞으로 이동시켜준다
        deque_to_map(); // 맵에도 바뀐 덱을 적용해준다
        delete_dup(); // 4개 이상 인접한 몬스터들을 지워준다
        deque_to_map(); // 맵에도 바뀐 덱을 적용해준다
        design_deque(); // 덱을 규칙에 맞게 바꿔준다
        deque_to_map(); // 맵에도 바뀐 덱을 적용해준다
    }
    
    cout << score << '\n';
    
    return 0;
}