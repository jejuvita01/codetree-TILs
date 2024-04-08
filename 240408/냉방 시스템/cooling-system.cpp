#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3

typedef struct Wall_info {
    int x;
    int y;
    int dir; // 0: 해당 칸 위쪽, 1: 해당 칸 왼쪽
} wall_info;

typedef struct Wind_info {
    int x;
    int y;
    int dir; // 0: 왼쪽, 1: 위쪽, 2: 오른쪽, 3: 아래쪽
} wind_info;


int n, m, k; // n: 격자, m: 벽 개수, k: 원하는 시원함
int map[20][20] = {0, }; // 초기 맵 정보 저장, 바꾸지 않을 맵
int cool[20][20] = {0, }; // 시원함 정보 저장, 바꿀 맵
int visited[20][20] = {0, }; // spread_cool 할때마다 초기화해서 쓸 맵
vector<wind_info> winds; // 에어컨 저장할 벡터
vector<wall_info> walls; // 벽 저장할 벡터
int total_time = 0;
int dx[4] = {0, -1, 0, 1}; // 왼쪽, 위쪽, 오른쪽, 아래쪽
int dy[4] = {-1, 0, 1, 0};

void init_visited(void)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            visited[i][j] = 0;
}

bool is_cool_enough(void)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (map[i][j] == 1) { // 사무실이면
                if (cool[i][j] < k) // 시원함이 k 미만이라면 return false
                    return false;
            }
        }
    }
    
    return true; // 사무실이 모두 k 이상으로 시원하면 return true
}

void print_cool(void)
{
    cout << "\nCOOL_MAP\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << cool[i][j] << ' ';
        }
        cout << '\n';
    }
    
    cout << '\n';
}

bool is_inside(int x, int y)
{
    if (0 <= x && x < n && 0 <= y && y < n)
        return true;
    else
        return false;
}

bool is_wall_there(int dir, int x, int y, int next_x, int next_y)
{
    for (int i = 0; i < walls.size(); i++) {
        if (dir == RIGHT) {
            if (walls[i].x == next_x && walls[i].y == next_y)
                if (walls[i].dir == 1)
                    return true;
        }
        else if (dir == UP) {
            if (walls[i].x == x && walls[i].y == y)
                if (walls[i].dir == 0)
                    return true;
        }
        else if (dir == LEFT) {
            if (walls[i].x == x && walls[i].y == y)
                if (walls[i].dir == 1)
                    return true;
        }
        else if (dir == DOWN) {
            if (walls[i].x == next_x && walls[i].y == next_y)
                if (walls[i].dir == 0)
                    return true;
        }
    }
    
    return false;
}

void spread_wind(int w, int x, int y, int c)
// winds의 index 값인 w
// 현재 바꿔야하는 칸인 x, y(맨 처음엔 에어컨 다음 칸으로 주어짐)
// 현재 시원함인 c (5에서 시작해 1까지. 0이되면 끝남)
{
    if (visited[x][y])
        return;
    
    visited[x][y] = 1; // 방문 처리를 하고
    cool[x][y] += c; // 일단 현재 x, y를 c로 바꿔주고
    
    if (c == 1) // 현재 c가 1이면 리턴
        return;
    
    // 현재 c가 1이 아니라면
    int next_x, next_y; // 원하는 지점을 저장할 변수
    
    // 직진
    next_x = x + dx[winds[w].dir];
    next_y = y + dy[winds[w].dir];
    if (is_inside(next_x, next_y)) { // 격자 내에 있고
        if (!is_wall_there(winds[w].dir, x, y, next_x, next_y)) // 벽이 없고
            if (!visited[next_x][next_y]) // 퍼뜨리지 않았다면
                spread_wind(w, next_x, next_y, c - 1); // spread_wind
    }
    
    int tmp_x, tmp_y; // 원하는 지점이 아닌, 중간 지점을 저장할 변수
    // 왼쪽 대각선 (tmp는 (w.dir - 1) % 4)
    tmp_x = x + dx[(winds[w].dir - 1 + 4) % 4];
    tmp_y = y + dy[(winds[w].dir - 1 + 4) % 4];
    next_x = tmp_x + dx[winds[w].dir];
    next_y = tmp_y + dy[winds[w].dir];
    if (is_inside(tmp_x, tmp_y)) { // tmp가 격자 내에 있고
        if (!is_wall_there((winds[w].dir - 1) % 4, x, y, tmp_x, tmp_y)) // tmp로 이동이 가능하고
            if (is_inside(next_x, next_y)) // next가 격자 내에 있고
                if (!is_wall_there(winds[w].dir, tmp_x, tmp_y, next_x, next_y)) // tmp와 next간에 벽이 없고
                    if (!visited[next_x][next_y]) // 퍼뜨리지 않았다면
                        spread_wind(w, next_x, next_y, c - 1); // spread_wind
    }
    
    // 오른쪽 대각선 (tmp는 (w.dir + 1) % 4)
    tmp_x = x + dx[(winds[w].dir + 1) % 4];
    tmp_y = y + dy[(winds[w].dir + 1) % 4];
    next_x = tmp_x + dx[winds[w].dir];
    next_y = tmp_y + dy[winds[w].dir];
    if (is_inside(tmp_x, tmp_y)) { // tmp가 격자 내에 있고
        if (!is_wall_there((winds[w].dir + 1) % 4, x, y, tmp_x, tmp_y)) // tmp로 이동이 가능하고
            if (is_inside(next_x, next_y)) // next가 격자 내에 있고
                if (!is_wall_there(winds[w].dir, tmp_x, tmp_y, next_x, next_y)) // tmp와 next간에 벽이 없고
                    if (!visited[next_x][next_y]) // 퍼뜨리지 않았다면
                        spread_wind(w, next_x, next_y, c - 1); // spread_wind
    }
}

void mix_cool(void)
{
    int static_cool[20][20] = {0, };
    
    // 원래의 시원함을 저장할 맵 (변경하지 않음)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            static_cool[i][j] = cool[i][j];
    
    // cool 맵 전체를 보며
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
//            cout << "현재 좌표: " << i << "," << j << ", 온도: " << static_cool[i][j] <<  '\n';
            for (int k = 0; k < 4; k++) { // 네 방향을 고려해서
                int next_i = i + dx[k];
                int next_j = j + dy[k];
                if (is_inside(next_i, next_j)) { // next가 맵 안인지 보고
                    if (!is_wall_there(k, i, j, next_i, next_j)) { // 벽이 없으면
                        if (visited[next_i][next_j] == 0) {
                            int diff = abs(static_cool[i][j] - static_cool[next_i][next_j]);
                            if (diff >= 4) { // 원래 시원함이 4 이상 차이나면
                                // diff = abs(static_cool[i][j] - cool[next_i][next_j]);
                                if (static_cool[i][j] > static_cool[next_i][next_j]) { // i,j 에서 next_i, j로 이동
//                                    cout << "1: DIFF " << i << "," << j << " 에서" << diff / 4 << " 빼준다\n";
                                    cool[i][j] -= (int)(diff / 4);
                                    cool[next_i][next_j] += (int)(diff / 4); ///////////// 여기 주의하기
                                }
                                else { // next_i, j에서 i, j로 이동
//                                    cout << "2: DIFF " << next_i << "," << next_j << " 에서" << diff / 4 << " 빼준다\n";
                                    cool[i][j] += (int)(diff / 4);
                                    cool[next_i][next_j] -= (int)(diff / 4);  ///////////////////////
                                }
                            }
                        }
                    }
                }
            }
            visited[i][j] = 1;
        }
    }
}

void reduce_outwall(void)
{
    // 첫 줄 감소
    for (int j = 0; j < n; j++) {
        if (cool[0][j] > 0)
            cool[0][j] -= 1;
    }
    
    // 마지막 줄 감소
    for (int j = 0; j < n; j++) {
        if (cool[n - 1][j] > 0)
            cool[n - 1][j] -= 1;
    }
    
    // 왼쪽 줄 감소
    for (int i = 0; i < n; i++) {
        if (i != 0 && i != n - 1)
            if (cool[i][0] > 0)
                cool[i][0] -= 1;
    }
    
    // 오른쪽 줄 감소
    for (int i = 0; i < n; i++) {
        if (i != 0 && i != n - 1)
            if (cool[i][n - 1] > 0)
                cool[i][n - 1] -= 1;
    }
}

int main(void)
{
    cin >> n >> m >> k;
    
    for (int i = 0; i < n; i++) { // map 정보 입력
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
            if (2 <= map[i][j] && map[i][j] <= 5) { // 만약 에어컨이면
                wind_info w;
                w.x = i;
                w.y = j;
                w.dir = map[i][j] - 2;
                winds.push_back(w);
            }
        }
    }
    
    for (int i = 0; i < m; i++) { // 벽 정보 입력
        wall_info w;
        cin >> w.x >> w.y >> w.dir;
        w.x -= 1;
        w.y -= 1;
        walls.push_back(w);
    }


    while (!is_cool_enough() && total_time <= 100) {
        // 모든 에어컨에서 시원함을 퍼뜨린다
        for (int w = 0; w < winds.size(); w++) {
            init_visited();
            spread_wind(w, winds[w].x + dx[winds[w].dir], winds[w].y + dy[winds[w].dir], 5);
//            print_cool();
        }
//        cout << "SPREAD";
//        print_cool();
        
        // 공기가 섞인다 (cool_map을 바꿔야함)
//        cout << "MIX";
        init_visited();
        mix_cool();
//        print_cool();
        
        // 외벽쪽을 감소시킨다
//        cout << "REDUCE";
        reduce_outwall();
//        print_cool();
        
        total_time++;
//        cout << total_time << '\n';
    }
    
    if (total_time > 100)
        total_time = -1;
    
    cout << total_time << '\n';
    /*
    cout << "얘도 되는지 봐\n";
    cool[0][0] = 8;
    cool[0][1] = 4;
    cool[0][2] = 1;
    cool[1][0] = 2;
    cool[1][1] = 9;
    cool[1][2] = 3;
    cool[2][0] = 2;
    cool[2][1] = 1;
    cool[2][2] = 4;
    
    mix_cool();
    print_cool();
     */
    
    return 0;
}

/*
4 1 4
0 0 0 0
4 0 1 1
1 1 1 1
0 0 0 0
3 2 0
*/

/*
3 2 10
0 0 0
0 0 0
0 0 1
2 3 1
3 2 0
 
이거 넣고
cool을 문제에 있는 3x3 맵처럼 값 넣어보기
*/


/*
5 2 3
0 0 0 0 0
2 0 1 1 1
0 0 1 1 0
0 0 0 0 0
0 3 0 0 0
3 2 0
3 2 1
*/