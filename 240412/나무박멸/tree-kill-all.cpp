#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

int n, m, k, c; // n: 격자, m: 년수, k: 범위, c: 유지년수
int year; // 현재 년도를 저장한 변수
int total_kill = 0; // 총 사라진 나무의 수를 저장할 변수
int map[20][20];
int killer[20][20]; // 제초제가 남아있는 년 수를 저장한 배열
int tx[] = {0, 0, 1, -1}; // 상하좌우 이동 변수
int ty[] = {1, -1, 0, 0};
int kx[] = {1, 1, -1, -1}; // 대각선 이동 변수
int ky[] = {-1, 1, -1, 1};

void print_map(void)
{
    cout << "\n\nMAP\n";
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

void grow_tree(void)
{
    // 맵을 전체 돌며 나무 수 증가
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (map[i][j] > 0) {
                int near_tree = 0; // 인접한 나무의 개수를 셀 변수
                for (int d = 0; d < 4; d++) { // 인접한 네 곳을 탐색하며
                    int x = i + tx[d];
                    int y = j + ty[d];
                    if (is_inside(x, y)) { // 격자 내에 있고
                        if (map[x][y] > 0) { // 나무라면
                            near_tree++; // 나무 카운트++
                        }
                    }
                }
                map[i][j] += near_tree;
            }
        }
    }
}

void init_map(int arr[20][20])
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            arr[i][j] = 0;
}

void spread_tree(void)
{
    int plus_tree[20][20]; // 각 칸에 더할 나무의 수를 저장하는 배열
    init_map(plus_tree);
    
    for (int i = 0; i < n; i++) {// 맵을 돌며
        for (int j = 0; j < n; j++) {
            if (map[i][j] > 0) { // i, j에서 나무를 발견하면
                int blank = 0; // 주변에 몇 칸 퍼뜨릴 수 있는지 저장하는 변수
                for (int d = 0; d < 4; d++) {
                    int x = i + tx[d];
                    int y = j + ty[d];
                    if (is_inside(x, y)) { // x, y가 격자 내에 있고
                        if (killer[x][y] == 0) { // 제초제가 없으며
                            if (map[x][y] == 0) { // 빈칸이면
                                blank++; // 주변 몇 칸에 퍼뜨릴 수 있는지 카운트
                            }
                        }
                    }
                }
                if (blank > 0) { // 퍼뜨릴 칸이 0 초과면
                    int spread = map[i][j] / blank; // i, j에서 퍼뜨릴 나무의 수를 (본인 / 카운트) 로 계산하고
                    for (int d = 0; d < 4; d++) {// 다시 네 방향을 돌며
                        int x = i + tx[d];
                        int y = j + ty[d];
                        if (is_inside(x, y)) { // x, y가 격자 내에 있고
                            if (killer[x][y] == 0) { // 제초제가 없으며
                                if (map[x][y] == 0) { // 빈칸이면
                                    // 퍼뜨릴 수 있는 칸이면 plus_tree에 더해주기
                                    plus_tree[x][y] += spread;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            map[i][j] += plus_tree[i][j];
}

void find_biggest(int& max_x, int& max_y)
{
    int max = 0;
    
    for (int i = 0; i < n; i++) { // 맵 돌기
        for (int j = 0; j < n; j++) {
            if (map[i][j] > 0) { // i, j가 나무면
                int cnt = map[i][j]; // 일단 본인의 크기만큼 사라짐
                for (int d = 0; d < 4; d++) { // i, j 나무에서 대각선 방향만큼 돌면서
                    int x = i + kx[d];
                    int y = j + ky[d];
                    if (is_inside(x, y)) { // x, y가 격자 내에 있다면
                        if (map[x][y] > 0) { // 나무가 있다면
                            cnt += map[x][y];
                            for (int r = 1; r < k; r++) {// x, y 쪽 대각선으로 k 만큼 뿌리기
                                int new_x = x + r * kx[d];
                                int new_y = y + r * ky[d];
                                if (is_inside(new_x, new_y)) { // new_x, new_y가 격자 내에 있고
                                    if (map[new_x][new_y] > 0) { // 나무가 있다면
                                        cnt += map[new_x][new_y]; // 그만큼 또 없애기
                                    }
                                    else
                                        r = k;
                                }
                            }
                        }
                    }
                    // 그 칸에 벽이 있거나 나무가 없다면
                    // 그 칸까지만 뿌리고
                    // 제초제 스탑
                }
//                cout << i << ',' << j << ": " << cnt << '\n';
                if (cnt > max) {
                    max = cnt;
                    max_x = i;
                    max_y = j;
//                    cout << i <<',' << j << " 에서 " << max << "로 업데이트 되었다.\n";
                }
                else if (cnt == max) {
                    if (i < max_x) {
                        max = cnt;
                        max_x = i;
                        max_y = j;
                    }
                    else if (i == max_x) {
                        if (j < max_y) {
                            max = cnt;
                            max_x = i;
                            max_y = j;
                        }
                    }
                }
            }
        }
    }
}

void remove_killer(void)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (killer[i][j] == year)
                killer[i][j] = 0;
        }
    }
}

void kill(int x, int y)
{
    total_kill += map[x][y];
    map[x][y] = 0;
    killer[x][y] = year + c + 1;
    
    int i, j, new_i, new_j;
    for (int d = 0; d < 4; d++) { // 대각선 네 방향 탐색
        i = x + kx[d];
        j = y + ky[d];
        if (is_inside(i, j)) { // 격자 내에 있고
            if (map[i][j] > 0) { // 그 칸이 나무이면
                total_kill += map[i][j];
                map[i][j] = 0;
                killer[i][j] = year + c + 1;
                for (int r = 1; r < k; r++) { // r 범위만큼 더 확산
                    new_i = i + r * kx[d];
                    new_j = j + r * ky[d];
                    if (is_inside(new_i, new_j)) { // 격자 내에 있고
                        if (map[new_i][new_j] > 0) { // 그 칸이 나무이면
                            total_kill += map[new_i][new_j];
                            map[new_i][new_j] = 0;
                            killer[new_i][new_j] = year + c + 1;
                        }
                        else { // 그 칸이 나무가 아니면
                            killer[new_i][new_j] = year + c + 1; // 여기까지만 제초제 뿌리고
                            r = k; // 반복문 종료
                        }
                    }
                }
            }
            else { // 나무가 아니면 그 칸까지만 제초제 뿌리기
                killer[i][j] = year + c + 1;
            }
        }
    }
}

void print_killer_map(void)
{
    cout << "\n\nKILLER\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << killer[i][j] << ' ';
        }
        cout << '\n';
    }
}

int main(void)
{
    cin >> n >> m >> k >> c;
    
    // 맵 정보 저장
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
    init_map(killer); // 제초제가 사라질 년수를 모두 0으로 만들어준다
    
    // m년동안 반복
    for (year = 1; year <= m; year++) {
        remove_killer(); // 제초제 없애고
        grow_tree(); // 나무 성장
        spread_tree(); // 나무 번식
        int x, y;
        x = y = -1;
        find_biggest(x, y); // 제초제의 영향이 가장 큰 칸 찾기
        if (!(x == -1 && y == -1))
            kill(x, y); // 제초제 뿌리기
    }
    
    cout << total_kill << '\n';
    
    return 0;
}