#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>

using namespace std;

typedef struct User_info {
    int is_moving; // 맵에 있는지(움직이는지) 표현하는 변수
    int x;
    int y;
    int move;
} user_info;

int N, M, K; // N: 격자 크기, M: 사람 수, K: 턴 수
int total_move = 0; // 총 움직인 횟수
int moving_user; // 현재 맵에 남아있는 유저 수
int map[10][10] = {0, }; // 0: 빈칸, 1~9: 벽, -1: 출구
int exit_x, exit_y; // 출구의 좌표를 저장할 변수
vector<user_info> user;
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, 1, -1};

void print_map(void)
{
    cout << "\n\nMAP\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
}

void print_user(void)
{
    for (int i = 0; i < M; i++) {
        cout << "USER" << i << "는 " << (user[i].is_moving ? "맵에있고, " : "탈출했고, ") << user[i].x << ',' << user[i].y << " 에 있다." << '\n';
    }
}

void spin_map(int x_start, int y_start, int length)
{
    int tmp_map[10][10] = {0, };
    int changed_flag[10] = {0, };
    
    // tmp_map에 회전한 결과를 저장하고
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            tmp_map[i][j] = map[x_start + length - j - 1][y_start + i];
        }
    }
    
    // tmp_map을 map에 복사하기
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            map[x_start + i][y_start + j] = tmp_map[i][j];
            if (map[x_start + i][y_start + j] == -1) { // 출구 발견하면
                exit_x = x_start + i; // 출구 정보 업데이트
                exit_y = y_start + j;
            }
            for (int u = 0; u < M; u++) {
                if (user[u].is_moving) {
                    if (changed_flag[u] == 0) {
                        if (user[u].x == (x_start + length - j - 1) && user[u].y == (y_start + i)) {
                            user[u].x = i + x_start;
                            user[u].y = j + y_start;
                            // cout << "USER" << u << "를 " << i + x_start << ',' << j + y_start << "로 이동시켰다\n";
                            changed_flag[u] = 1;
                        }
                    }
                }
            }
        }
    }
    
    /*
    // user_map을 만든다
    int user_map[10][10] = {-1, };
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            user_map[i][j] = -1;
    
    for (int u = 0; u < M; u++) {
        if (user[u].is_moving == true) {
            user_map[user[u].x][user[u].y] = u;
        }
    }
    
    cout << "\nUSER MAP\n";
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << user_map[i][j] << ' ';
        }
        cout << '\n';
    }
    
    // tmp_map을 초기화한다
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            tmp_map[i][j] = -1;
    
    // tmp_map에 회전한 결과를 저장한다
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            tmp_map[i][j] = user_map[x_start + length - j - 1][y_start + i];
        }
    }
    
    for (int i = x_start; i < x_start + length; i++) {
        for (int j = y_start; j < y_start + length; j++) {
            user_map[i][j] = tmp_map[i - x_start][j - y_start];
            if (user_map[i][j] != -1) { // 유저 발견하면
                
                cout << "USER" << user_map[i][j] << "를 " << i << ',' << j << "로 이동시켰다\n";
                user[user_map[i][j]].x = i;
                user[user_map[i][j]].y = j;
            }
        }
    }
    */
}

int get_dist(int x, int y) // u와 출구와의 거리를 리턴
{
    return (abs(x - exit_x) + abs(y - exit_y));
}

bool is_inside(int x, int y)
{
    if (0 <= x && x < N && 0 <= y && y < N)
        return true;
    else
        return false;
}

void move_user(int idx) // user의 idx를 전달받음
{
    int min_dist = get_dist(user[idx].x, user[idx].y);
    int next_x, next_y;
    
    for (int i = 0; i < 4; i++) { // 모든 방향을 순회하며
        next_x = user[idx].x + dx[i];
        next_y = user[idx].y + dy[i];
        if (is_inside(next_x, next_y)) { // 이동한 곳이 격자 내에 있고
            if (map[next_x][next_y] == 0 || map[next_x][next_y] == -1) {// 벽이 아니고
                if (get_dist(next_x, next_y) < min_dist) {// min_dist보다 가까워지면
                    // cout << "USER" << idx << "는 " << user[idx].x << ',' << user[idx].y << " 에서 " << next_x <<',' << next_y << " 로 이동했다\n";
                    total_move++;
                    user[idx].x = next_x; // 이동하기
                    user[idx].y = next_y;
                    if (user[idx].x == exit_x && user[idx].y == exit_y) { // user가 출구에 도달하면
                        user[idx].is_moving = false; // 해당 유저의 is_moving 을 false로 바꾸고
                        moving_user--; // moving_user--
                    }
                    return;
                }
            }
        }
    }
}

int get_closest_user(void)
{
    int min_dist = 100;
    int min_idx = -1;
    
    for (int u = 0; u < M; u++) {
        if (user[u].is_moving == true) { // user가 맵에 있고
            int dist = get_dist(user[u].x, user[u].y);
            if (dist < min_dist || min_idx == -1) {// 거리가 가까우면
                min_dist = dist; // 업데이트
                min_idx = u;
            }
            else if (dist == min_dist) {// 거리가 같으면
                if (user[u].x < user[min_idx].x) {// r이 작으면 업데이트
                    min_dist = dist;
                    min_idx = u;
                }
                else if (user[u].x == user[min_idx].x) {// r이 같으면
                    if (user[u].y < user[min_idx].y) {// c가 작으면 업데이트
                        min_dist = dist;
                        min_idx = u;
                    }
                }
            }
        }
    }
    
    return min_idx;
}

void find_range(int& x_start, int& y_start, int& length)
{
    int idx = get_closest_user(); // exit과 가장 가까이 있는 user의 idx를 구한다
    // cout << "제일 가까운 유저는 " << user[idx].x << ',' << user[idx].y << " 에 있다\n";
    
    // 변 길이를 구한다
    length = max(abs(user[idx].x - exit_x) + 1, abs(user[idx].y - exit_y) + 1);
    
    // 제일 오른쪽 끝에 있는 x, y값을 구한다
    int biggest_x = (user[idx].x > exit_x ? user[idx].x : exit_x);
    int biggest_y = (user[idx].y > exit_y ? user[idx].y : exit_y);
    
    x_start = biggest_x - (length - 1);
    y_start = biggest_y - (length - 1);
    
    if (x_start < 0)
        x_start = 0;
    if (y_start < 0)
        y_start = 0;
    
    // cout << "최소 사각형의 시작점은 " << x_start << ',' << y_start << " 이고 길이는 " << length << '\n';
}

void reduce_power(int x_start, int y_start, int length)
{
    for (int i = x_start; i < x_start + length; i++) { // 정사각형 범위를 순회하며
        for (int j = y_start; j < y_start + length; j++) {
            if (map[i][j] != 0 && map[i][j] != -1) // 해당 map이 벽이면
                map[i][j]--; // 내구성 1만큼 감소시키기
        }
    }
}

int main(void)
{
    cin >> N >> M >> K;
    
    // 맵 정보 받기
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
    // M명의 user 정보 받기
    for (int i = 0; i < M; i++) {
        user_info u;
        cin >> u.x >> u.y;
        u.x -= 1;
        u.y -= 1;
        u.move = 0;
        u.is_moving = true;
        user.push_back(u);
    }
    moving_user = M;
    
    // 출구 정보 받기
    cin >> exit_x >> exit_y;
    exit_x -= 1;
    exit_y -= 1;
    map[exit_x][exit_y] = -1;
    
    
    // print_user();
    // K번의 턴
    for (int turn = 0; turn < K; turn++) {
        // cout << "\n\nTURN: " << turn + 1 << '\n';
        if (moving_user == 0)
            break;
        for (int u = 0; u < M; u++) { // 참가자들을 순회하며
            if (user[u].is_moving == true) { // 현재 이동중인 참가자는
                move_user(u); // 이동시키기
            }
        }
        
        if (moving_user == 0)
            break;
        
        int x_start, y_start, length;
        find_range(x_start, y_start, length); // 최소 사각형의 범위를 찾고
        spin_map(x_start, y_start, length); // 회전시키고
        reduce_power(x_start, y_start, length); // 범위 내의 벽의 내구성을 감소시키기
        // print_map();
        // print_user();
        // cout << "TOTAL_MOVE: " << total_move << '\n';
        
    }
    
//    print_map();
//
//    spin_map(1, 1, 3);
//    print_map();
    
    cout << total_move << '\n' << exit_x + 1 << ' ' << exit_y + 1;
    
    
    return 0;
}