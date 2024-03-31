#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

typedef struct {
    int x;
    int y;
} point;

int n, m, k;
int map[10][10] = {0, }; // -1: 출구, 0: 빈칸, 1~9: 벽
vector<point> user_pos;
int total_move = 0;
point exit_pos;
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

// square 변수들
point sq_start;
point sq_end;
int sq_length;

void print_map(void)
{
    cout << '\n';
    cout << "MAP" << '\n';
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if ((1 <= map[i][j] && map[i][j] <= 9) || map[i][j] == -1)
                cout << map[i][j] << ' ';
            else {
                bool user_flag = false;
                int u;
                for (u = 0; u < user_pos.size(); u++) {
                    if (user_pos[u].x == i && user_pos[u].y == j) {
                        user_flag = true;
                        break;
                    }
                }
                if (user_flag)
                    cout << u + 50 << ' ';
                else
                    cout << 0 << ' ';
            }
        }
        cout << '\n';
    }
    cout << '\n';
}

int dist(point a, point b)
{
    return (abs(a.x - b.x) + abs(a.y - b.y));
}

void move_users(void)
{
    for (int user = 0; user < user_pos.size(); user++) {
        point pos = {user_pos[user].x, user_pos[user].y};
        point new_pos;
        for (int i = 0; i < 4; i++) {
            new_pos.x = pos.x + dx[i];
            new_pos.y = pos.y + dy[i];
            if (0 <= new_pos.x && new_pos.x < n && 0 <= new_pos.y && new_pos.y < n) {
                if (new_pos.x == exit_pos.x && new_pos.y == exit_pos.y) {
                    // cout << "지움" <<'\n';
                    user_pos.erase(user_pos.begin() + user, user_pos.begin() + user + 1); // 여기서 틀릴 수 있음
                    user--; // 여기서 틀릴 수 있음
                    total_move++;
                    break;
                }
                if (map[new_pos.x][new_pos.y] == 0) {
                    if (dist(new_pos, exit_pos) < dist(pos, exit_pos)) {
                        // cout << "USER" << user << ": " <<  new_pos.x << ' ' << new_pos.y << '\n';
                        user_pos[user].x = new_pos.x;
                        user_pos[user].y = new_pos.y;
                        total_move++;
                        break;
                    }
                }
            }
        }
    }
}

void find_square(void)
{
    int min_length = 10; // 변의 길이
    point min_pos = {10, 10}; // 최소 정사각형을 만드는 참가자의 위치

    // 포함해야 하는 참가자의 위치 구하기
    for (int u = 0; u < user_pos.size(); u++) {
        int length;
        length = abs(user_pos[u].x - exit_pos.x);
        if (length < abs(user_pos[u].y - exit_pos.y))
            length = abs(user_pos[u].y - exit_pos.y);
        length += 1;
        if (length < min_length) {
            min_length = length;
            min_pos.x = user_pos[u].x;
            min_pos.y = user_pos[u].y;
        }
        else if (length == min_length) {
            if (min_pos.x > user_pos[u].x) {
                min_pos.x = user_pos[u].x;
                min_pos.y = user_pos[u].y;
            }
            else if (min_pos.x == user_pos[u].x) {
                if (min_pos.y > user_pos[u].y) {
                    min_pos.x = user_pos[u].x;
                    min_pos.y = user_pos[u].y;
                }
            }
        }
    }
    
    // 정사각형의 시작점, 끝점, 변의 길이 구하기
    /*
     point sq_start;
     point sq_end;
     int sq_length;
    */
    sq_length = min_length;
    if (abs(min_pos.x - exit_pos.x) == abs(min_pos.y - exit_pos.y)) { // 사용자와 출구를 끝 점으로 써도 될 때
        if (min_pos.x < exit_pos.x) { // 사용자가 좌상단에 있을 때
            sq_start.x = min_pos.x;
            sq_start.y = min_pos.y;
            sq_end.x = exit_pos.x;
            sq_end.y = exit_pos.y;
        }
        else { // 출구가 좌상단에 있을 때
            sq_start.x = exit_pos.x;
            sq_start.y = exit_pos.y;
            sq_end.x = min_pos.x;
            sq_end.y = min_pos.y;
        }
    }
    else { // 사용자와 출구로 정사각형이 만들어지지 않을 때
        if (min_pos.x < exit_pos.x || min_pos.y < exit_pos.y) { // 사용자가 좌/상단에 있을 때
            if ((0 <= exit_pos.x - sq_length + 1) && (0 <= exit_pos.y - sq_length + 1)) { // 출구를 끝점으로 쓸 때
                sq_start.x = exit_pos.x - sq_length + 1;
                sq_start.y = exit_pos.y - sq_length + 1;
                sq_end.x = exit_pos.x;
                sq_end.y = exit_pos.y;
            }
            else { // 출구가 끝점이 아닐 때
                if (exit_pos.x - min_pos.x + 1 == sq_length) {
                    sq_start.x = min_pos.x;
                    sq_start.y = 0;
                    sq_end.x = exit_pos.x;
                    sq_end.y = sq_length - 1;
                }
                else { // exit_pos.y - min_pos.y + 1 == sq_length
                    sq_start.x = 0;
                    sq_start.y = min_pos.y;
                    sq_end.x = sq_length - 1;
                    sq_end.y = exit_pos.y;
                }
            }
        }
        else { // 출구가 좌/상단에 있을 때
            if ((0 <= min_pos.x - sq_length + 1) && (0 <= min_pos.y - sq_length + 1)) { // 사용자를 끝점으로 쓸 때
                sq_start.x = min_pos.x - sq_length + 1;
                sq_start.y = min_pos.y - sq_length + 1;
                sq_end.x = min_pos.x;
                sq_end.y = min_pos.y;
            }
            else { // 사용자가 끝점이 아닐 때
                if (min_pos.x - exit_pos.x + 1 == sq_length) {
                    sq_start.x = exit_pos.x;
                    sq_start.y = 0;
                    sq_end.x = min_pos.x;
                    sq_end.y = sq_length - 1;
                }
                else { // min_pos.y - exit_pos.y + 1 == sq_length
                    sq_start.x = 0;
                    sq_start.y = exit_pos.y;
                    sq_end.x = sq_length - 1;
                    sq_end.y = min_pos.y;
                }
            }
        }
    }
}

void reduce_wall_power(void)
{
    for (int i = sq_start.x; i <= sq_end.x; i++) {
        for (int j = sq_start.y; j <= sq_end.y; j++) {
            if (1 <= map[i][j] && map[i][j] <= 9)
                map[i][j]--;
        }
    }
    
    // print_map();
}

void rotate_square(void)
{
    int new_sq[sq_length][sq_length];
    int user_move_flag[10] = {0, };
    
    for (int j = sq_length - 1; j >= 0; j--) {
        for (int i = 0; i < sq_length; i++) {
            new_sq[i][j] = map[sq_end.x - j][sq_start.y + i];
            for (int u = 0; u < user_pos.size(); u++) {
                if (user_pos[u].x == sq_end.x - j && user_pos[u].y == sq_start.y + i) {
                    if (user_move_flag[u] == 0) {
                        user_move_flag[u] = 1;
                        user_pos[u].x = sq_start.x + i;
                        user_pos[u].y = sq_start.y + j;
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < sq_length; i++) {
        for (int j = 0; j < sq_length; j++) {
            map[i + sq_start.x][j + sq_start.y] = new_sq[i][j];
            if (map[i + sq_start.x][j + sq_start.y] == -1) {
                exit_pos.x = i + sq_start.x;
                exit_pos.y = j + sq_start.y;
            }
        }
    }
}

int main(void)
{
    cin >> n >> m >> k; // NxN, M명의 user, K초
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
    for (int i = 0; i < m; i++) {
        point pos;
        cin >> pos.x >> pos.y;
        pos.x--;
        pos.y--;
        user_pos.push_back(pos);
    }
    
    cin >> exit_pos.x >> exit_pos.y;
    exit_pos.x--;
    exit_pos.y--;
    
    map[exit_pos.x][exit_pos.y] = -1;
    
    // print_map();
    
    for (int i = 0; i < k; i++) {
        // cout << "K: " << i + 1 << '\n';
        if (user_pos.size() == 0)
            break;
        move_users();
        find_square();
        /*
        cout << "SQUARE" << '\n';
        cout << "SQ_START: " << sq_start.x << ' ' << sq_start.y << '\n';
        cout << "SQ_END: " << sq_end.x << ' ' << sq_end.y << '\n';
        cout << "SQ_LENGTH: " << sq_length << '\n';
        */
        reduce_wall_power();
        rotate_square();
        /*
        print_map();
        cout << "USER" << '\n';
        for (int u = 0; u < user_pos.size(); u++)
            cout << u << ": " << user_pos[u].x << ' ' << user_pos[u].y << '\n';
        cout << "\n\n\n";
        */
    }

    /*
    vector<int> test;
    
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    test.push_back(5);
    
    test.erase(test.begin() + 2, test.begin() + 3); // 내가 3을 지워볼게~(2번째 인덱스)
    // 벡터.erase(지우려는 본인 위치, 그 다음 위치)
    
    for (int i = 0; i < test.size(); i++)
        cout << test[i];
    */
        
    cout << total_move << '\n' << exit_pos.x + 1 << ' ' << exit_pos.y + 1 << '\n';
    
    return 0;
}