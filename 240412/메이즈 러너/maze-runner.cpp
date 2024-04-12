#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

typedef struct Point {
    int x;
    int y;
} point;

typedef struct User_info {
    point p;
    bool is_moving;
} user_info;

int n, m, k;
int turn;
int moving_user;
int total_move = 0;
int map[10][10];
point out; // 출구의 좌표 저장
vector<user_info> user;
int dx[] = {-1, 1, 0, 0}; // 상 하 좌 우
int dy[] = {0, 0, -1, 1};

int get_dist(int x, int y)
{
    return (abs(x - out.x) + abs(y - out.y));
}

void print_map(void)
{
    cout << "MAP\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
}

bool is_inside(int x, int y)
{
    if (0 <= x && x < n && 0 <= y && y < n)
        return true;
    else
        return false;
}

void move_user(int u)
{
    int min = get_dist(user[u].p.x, user[u].p.y);
    
    for (int i = 0; i < 4; i++) {
        int x = user[u].p.x + dx[i];
        int y = user[u].p.y + dy[i];
        if (is_inside(x, y)) { // 격자 내에 있고
            if (map[x][y] == 0 || map[x][y] == -1) { // 벽이 아니고
                if (get_dist(x, y) < min) {// 출구와의 거리가 가까워지면
                    user[u].p.x = x; // 이동하고 바로 리턴
                    user[u].p.y = y;
                    // cout << "USER" << u << "가 " << x << ',' << y << "로 이동\n";
                    total_move++;
                    if (user[u].p.x == out.x && user[u].p.y == out.y) {
                        user[u].is_moving = false;
                        moving_user--;
                    }
                    return;
                }
            }
        }
    }
}

void find_square(int& x, int& y, int& length)
{
    for (int sz = 2; sz <= n; sz++) { // 정사각형 변 길이를 2부터 N 까지 돌며
        for (int i = 0; i < n; i++) { // 가장 좌상단부터 정사각형 탐색 시작
            for (int j = 0; j < n; j++) {
                // cout << "i,j: " << i << ',' << j << ", length: " << sz << '\n';
                bool is_out = false;
                bool is_user = false;
                
                // 사각형 내에 출구가 있는지 확인
                if (i <= out.x && out.x < i + sz && j <= out.y && out.y < j + sz) {
                    // cout << "사각형 안에 출구가 있다\n";
                    is_out = true;
                }
                
                // 사각형 내에 유저가 있는지 확인
                for (int u = 0; u < m; u++) {
                    if (user[u].is_moving == true) { // 유저가 탈출하지 않았으면
                        if (i <= user[u].p.x && user[u].p.x < i + sz && j <= user[u].p.y && user[u].p.y < j + sz) {
                            // cout << "사각형 안에 " << "USER" << u << "가 " << user[u].p.x << ',' << user[u].p.y << " 에 있다\n";
                            is_user = true;
                            u = m;
                        }
                    }
                }
                
                // 둘다 있으면 업데이트 하고 종료
                if (is_out && is_user) {
                    x = i;
                    y = j;
                    length = sz;
                    return;
                }
            }
        }
        
    }
}

void turn_map(int x, int y, int sz)
{
    int new_map[sz][sz]; // 맵 회전할 때 쓸 배열
    int changed_flag[10] = {0, }; // 유저 회전했는지 체크하는 배열
    
    for (int i = 0; i < 10; i++)
        changed_flag[i] = 0;
    
    // 맵 회전
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            new_map[i][j] = map[x + sz - j - 1][y + i];
            for (int u = 0; u < m; u++) {
                if (user[u].is_moving == true) { // 유저가 맵에 있고
                    if (changed_flag[u] == 0) { // 아직 회전하지 않았고
                        if (user[u].p.x == x + sz - j - 1 && user[u].p.y == y + i) {// 격자 값이 같으면
                            changed_flag[u] = 1;
                            user[u].p.x = x + i;
                            user[u].p.y = y + j;
                            // cout << "USER" << u << "를 " << x + i << ',' << y + j << "로 이동\n";
                        }
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            map[x + i][y + j] = new_map[i][j];
            // 벽 있으면 내구성 1 감소시키기
            if (map[x + i][y + j] > 0)
                map[x + i][y + j]--;
            if (map[x + i][y + j] == -1) {
                out.x = x + i; // 출구 회전
                out.y = y + j;
            }
        }
    }
}

int main(void)
{
    cin >> n >> m >> k;
    
    // 맵 정보 입력
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
    // 사람 위치 입력
    for (int i = 0; i < m; i++) {
        user_info u;
        cin >> u.p.x >> u.p.y;
        u.is_moving = true;
        u.p.x -= 1;
        u.p.y -= 1;
        user.push_back(u);
    }
    
    moving_user = m;
    
    // 출구 좌표 입력
    cin >> out.x >> out.y;
    out.x -= 1;
    out.y -= 1;
    map[out.x][out.y] = -1;
    
    // 턴 반복
    for (turn = 1; turn <= k; turn++) {
        // cout << "\n\nTURN: " << turn << '\n';
        if (moving_user == 0)
            break;
        for (int u = 0; u < m; u++) {// 유저 순회하며
            if (user[u].is_moving == true) // 현재 이동중인 유저 이동
                move_user(u);
        }
        // print_map();
        // for (int u = 0; u < m; u++) {
        //     cout << "USER" << u << "는 " << (user[u].is_moving ? "맵에 있고 " : "탈출했고 ") << user[u].p.x << ',' << user[u].p.y << "에 있다\n";
        // }
        if (moving_user == 0)
            break;
        int x, y, length;
        x = y = length = -1;
        find_square(x, y, length); // 정사각형 찾기
        if (!(x == -1 && y == -1 && length == -1)) {
            // cout << "X, Y, LENGTH: " << x << ',' << y << ',' << length << '\n';
            turn_map(x, y, length); // 정사각형, 유저, 출구 회전
        }
        // cout << "회전시킨 맵\n";
        // print_map();
    }
    
    cout << total_move << '\n' << out.x + 1 << ' ' << out.y + 1 << '\n';
    
    return 0;
}