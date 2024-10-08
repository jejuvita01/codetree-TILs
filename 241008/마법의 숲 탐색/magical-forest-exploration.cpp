#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

int R, C, K;
int map[70][70];
int visited[70][70];
int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};
int answer = 0;

void init_map(void)
{
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            map[i][j] = 0;
}

void init_visited(void)
{
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            visited[i][j] = 0;
}

void print_map(void)
{
    cout << "\n\nMAP\n";
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
}

bool is_inside(int x, int y)
{
    return (0 <= x && x < R && 0 <= y && y < C);
}

bool can_go_down(int r, int c)
{
    if (r + 2 >= R) // 일단 한칸 아래가 맵 안인지 보고
        return false;
    if (r == -2 && map[0][c] == 0) // 제일 첫 시작일때는 바로 아래 맵 한 칸만 확인하기
        return true;
    if (map[r+1][c-1] == 0 && map[r+2][c] == 0 && map[r+1][c+1] == 0) // 세 칸이 비어있으면 이동 가능
        return true;
    else // 아니면 불가능
        return false;
}

bool can_go_left(int r, int c)
{
    if (c - 2 < 0) // 일단 한칸 왼쪽이 맵 안인지 보고
        return false;
    if (map[r][c-2] == 0 && map[r-1][c-1] == 0 && map[r+1][c-1] == 0) // 세 칸이 비어있으면 이동 가능
        return true;
    else // 아니면 불가능
        return false;
}

bool can_go_right(int r, int c)
{
//    cout << "RIGHT CHECK (r, c): " << r << ' ' << c << '\n';
//    cout << map[r][c + 2] << ' ' << map[r-1][c+1] << ' ' << map[r+1][c+1] << '\n';
//    print_map();
    if (c + 2 >= C) // 일단 한칸 오른쪽이 맵 안인지 보고
        return false;
    if (map[r][c+2] == 0 && map[r-1][c+1] == 0 && map[r+1][c+1] == 0) // 세 칸이 비어있으면 이동 가능
        return true;
    else // 아니면 불가능
        return false;
}

void move_golem(int& r, int& c, int& d) // r: 골렘의 중앙 행 값, c: 골렘의 중앙 열 값, d: 출구의 방향
{
    bool down, left, right;
    down = left = right = false;
    while (1) {
        if (can_go_down(r, c)) {
            r++;
//            cout << "DOWN\n";
            down = true;
        }
        else if (right == false && can_go_left(r, c)) { // 왼쪽으로 회전
            if (is_inside(r + 2, c - 1) && map[r+2][c-1] == 0) {
                d = (d - 1 + 4) % 4; // d 반시계로 바꿔주기
                c--;
//                cout << "LEFT\n";
                left = true;
            }
            else
                return;
        }
        else if (left == false && can_go_right(r, c)) { // 오른쪽으로 회전
            if (is_inside(r + 2, c + 1) && map[r+2][c+1] == 0) {
                d = (d + 1) % 4; // d 시계로 바꿔주기
                c++;
//                cout << "RIGHT\n";
                right = true;
            }
            else
                return;
        }
        else
            break;
//        // 더이상 움직일 수 없을때 종료를 해줘야함.
//        if (left == false && right == false && r + 2 == R)
//            break;
//        if (left == false && right == false && !can_go_down(r, c) && !can_go_left(r, c) && !can_go_right(r, c))
//            break;
//        if (left == true && !can_go_left(r, c) && !can_go_down(r, c))
//            break;
//        if (right == true && !can_go_right(r, c) && !can_go_down(r, c))
//            break;
    }
}

void paint_map(int r, int c)
{
    map[r][c] = 1;
    map[r][c-1] = 1;
    map[r][c+1] = 1;
    map[r-1][c] = 1;
    map[r+1][c] = 1;
}

bool is_near_golem(int x, int y)
{
//    cout << "x, y: " << x << ", " << y << '\n';
//    print_map();
    
    bool flag = false;
    for (int i = 0; i < 4; i++) {
        int new_x, new_y;
        new_x = x + dx[i];
        new_y = y + dy[i];
        if (is_inside(new_x, new_y) && map[new_x][new_y] == 1)
            flag = true;
    }
    return flag;
}

void dfs(int x, int y)
{
    visited[x][y] = 1;
    for (int i = 0; i < 4; i++) {
        int new_x, new_y;
        new_x = x + dx[i];
        new_y = y + dy[i];
        if (is_inside(new_x, new_y) && visited[new_x][new_y] == 0 && map[new_x][new_y] == 1)
            dfs(new_x, new_y);
    }
}

int find_biggest_row(void)
{
    for (int i = R - 1; i >= 0; i--) {
        for (int j = 0; j < C; j++) {
            if (map[i][j] == 1)
                return i;
        }
    }
    return 0;
}

void move_ghost(int r, int c, int d) {
    int x, y;
    x = r + dx[d];
    y = c + dy[d];
    
    if (is_near_golem(x, y)) { // x, y 주변에 다른 골렘이 위치하고 있으면?
        init_visited();
        paint_map(r, c);
        dfs(x, y); // 전체 dfs
        answer += find_biggest_row() + 1; // visited 조사해서 방문한 가장 큰 행 구하기
        
        
//        cout << "VISITED MAP\n";
//        for (int i = 0; i < R; i++) {
//            for (int j = 0; j < C; j++) {
//                cout << visited[i][j] << ' ';
//            }
//            cout << '\n';
//        }
    }
    else { // 아니면 골렘 내에서 가장 남쪽행 값 더해주기
        answer += (r + 1 + 1);
    }
}

int main(void)
{
    cin >> R >> C >> K;
    
    init_map();
    
    for (int turn = 0; turn < K; turn++) {
        int c, d;
        cin >> c >> d;
        c -= 1;
//        cout << "\n\n********TURN: " << turn << '\n';
        int r = -2;
        move_golem(r, c, d);
        if (r <= 0) { // 골렘의 몸이 맵을 벗어났으면
            init_map();
        }
        else { // 골렘의 몸이 맵을 벗어나지 않았으면
//            cout << "r, c, d: " << r << ' ' << c << ' ' << d << '\n';
            move_ghost(r, c, d); // 정령을 최대한 남쪽으로 옮기고 행만큼 점수 더해주기
            paint_map(r, c); // 골렘의 최종 위치 색칠해주기
        }
//        print_map();
//        cout << "TURN" << turn << " answer: " << answer << '\n';
    }
    
    cout << answer << '\n';
    
    return 0;
}