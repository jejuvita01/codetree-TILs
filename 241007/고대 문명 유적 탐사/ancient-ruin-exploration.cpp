#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>

using namespace std;

int K, M;
deque<int> m;
int map[5][5];
int visited[5][5];
int turn_score = 0;
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};

typedef struct Point {
    int x, y;
} point;

void copy_map(int new_map[5][5])
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            new_map[i][j] = map[i][j];
}

bool is_inside(int x, int y)
{
    return (0 <= x && x < 5 && 0 <= y && y < 5);
}

void init_visited(void)
{
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            visited[i][j] = 0;
        }
    }
}

void print_map(void)
{
    cout << "\n\nMAP\n\n";
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
    cout << "\n\n";
}

void rotate_map(int m[5][5], int x, int y) // x, y는 정사각형의 좌상단 점
{
    int rot[3][3];
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rot[i][j] = m[3 - j + x - 1][y + i];
        }
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m[i + x][j + y] = rot[i][j];
        }
    }
}

void dfs(int m[5][5], int x, int y, int& cnt, vector<point>& points)
{
    cnt++;
    point p = {x, y};
    points.push_back(p);
    visited[x][y] = 1;
    
    for (int d = 0; d < 4; d++) {
        int new_x, new_y;
        new_x = x + dx[d];
        new_y = y + dy[d];
        if (is_inside(new_x, new_y) && m[x][y] == m[new_x][new_y] && visited[new_x][new_y] == 0)
            dfs(m, new_x, new_y, cnt, points);
    }
}

void delete_points(vector<point>& points)
{
    for (int i = 0; i < points.size(); i++) {
        map[points[i].x][points[i].y] = 0;
    }
}

int cnt_deletable(int m[5][5], bool delete_flag)
{
    init_visited();
    int total = 0;
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (m[i][j] != 0 && visited[i][j] == 0) {
                int cnt = 0;
                vector<point> points;
                dfs(m, i, j, cnt, points);
                if (cnt >= 3) {
                    if (delete_flag) {
                        delete_points(points); // points들 맵에서 0으로 만들고
                        turn_score += cnt; // cnt만큼 해당 턴의 점수 올려주기
                    }
                    total += cnt;
                }
            }
        }
    }
    
    return total;
}

void delete_map(int x, int y, int rot) // rot * 90 = 회전 각
{
    init_visited();
    for (int i = 0; i < rot; i++) {
        rotate_map(map, x, y);
//        print_map();
    }
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (map[i][j] != 0 && visited[i][j] == 0) {
                int cnt = 0;
                vector<point> points;
                dfs(map, i, j, cnt, points);
                if (cnt >= 3) {
                    delete_points(points); // points들 맵에서 0으로 만들고
                    turn_score += cnt; // cnt만큼 해당 턴의 점수 올려주기
                }
            }
        }
    }
}

void rotate_whole_map(int& x, int& y, int& rot)
{
    int max_d, max_x, max_y, max_r;
    
    max_d = 0;
    max_x = 4;
    max_y = 4;
    max_r = 3;
    for (int i = 0; i <= 2; i++) {
        for (int j = 0; j <= 2; j++) {
            int new_map[5][5];
            copy_map(new_map);
            for (int r = 1; r <= 3; r++) {
                rotate_map(new_map, i, j); // 맵을 90, 180, 270번 회전하면서
                int d = cnt_deletable(new_map, false); // 지울 수 있는 개수 확인
                if (d > max_d) {
                    max_d = d;
                    max_x = i;
                    max_y = j;
                    max_r = r;
                }
                else if (d == max_d) {// 우선순위 확인해서 회전 위치, 각 정하기
                    if (r < max_r) { // 회전각이 작으면
                        max_d = d;
                        max_x = i;
                        max_y = j;
                        max_r = r;
                    }
                    else if (r == max_r) {
                        if (j < max_y) {// 열이 작으면
                            max_d = d;
                            max_x = i;
                            max_y = j;
                            max_r = r;
                        }
                        else if (j == max_y) {
                            if (i < max_x) { // 행이 작으면
                                max_d = d;
                                max_x = i;
                                max_y = j;
                                max_r = r;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (max_d == 0)
        x = y = rot = -1;
    else {
        x = max_x;
        y = max_y;
        rot = max_r;
    }
}

void fill_the_blank(void)
{
    for (int j = 0; j < 5; j++) {
        for (int i = 4; i >= 0; i--) {
            if (map[i][j] == 0) {
                map[i][j] = m.front();
                m.pop_front();
            }
        }
    }
}

int main(void)
{
    cin >> K >> M;
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            cin >> map[i][j];
        }
    }
    
    for (int i = 0; i < M; i++) {
        int x;
        cin >> x;
        m.push_back(x);
    }
    
    init_visited();
    
    // turn 시작
    for (int i = 0; i < K; i++) {
        turn_score = 0;
        int x, y, rot;
        rotate_whole_map(x, y, rot); // (0, 0)에서 (2, 2)까지 (90, 180, 270) 돌려보면서 돌릴 곳을 찾기
        if (x == -1 && y == -1 && rot == -1) // 만약 모든 경우가 0이면 탐색 끝내기(출력X)
            break;
        // 아니라면
//        cout << "이번 턴은: " << x << ' ' << y << ' ' << rot << '\n';
        delete_map(x, y, rot);  // 정해진 우선순위 회전위치,각으로 회전하고 삭제하기
//        print_map();
        fill_the_blank(); // 채워넣기
//        cout << "\n\n지우고 처음 채운 맵";
//        print_map();
        while (cnt_deletable(map, true)) { // 채워넣었을때 삭제 가능한지 보고
//            cout << "지운 맵\n";
//            print_map();
            fill_the_blank(); // 채워넣기 반복
//            cout << "채운 맵\n";
//            print_map();
        }
        cout << turn_score << ' ';
//        print_map();
//        printf("__________________________________________\n\n");
    }
    cout << '\n';
    
    return 0;
}


/*
7 5 4 2 7
5 6 1 7 5
1 3 4 5 1
7 2 1 6 7
5 4 3 2 7
*/