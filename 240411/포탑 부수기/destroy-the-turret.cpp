#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

typedef struct Point {
    int x;
    int y;
} point;

int N, M, K;
int turn;
int map[10][10] = {0, };
int visited[10][10] = {0, };
int last_attack[10][10] = {0, };
point a, d; // 각각 공격자, 방어자 의 좌표를 저장하는 변수
int dx[] = {0, 1, 0, -1}; // 우 하 좌 상
int dy[] = {1, 0, -1, 0};


void print_map(void)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
}

void init_visited(void)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            visited[i][j] = 0;
        }
    }
}

void select_attacker(void)
{
    int min_power = 6000;
    a.x = a.y = -1;
    
    for (int i = 0; i < N; i++) { // 전체 맵을 돌며
        for (int j = 0; j < M; j++) {
            if (map[i][j] != 0) { // 부서진 포탑이 아닐 때
                if (map[i][j] < min_power || (a.x == -1 && a.y == -1)) {// 공격력이 작으면 업데이트
                    min_power = map[i][j];
                    a.x = i;
                    a.y = j;
                }
                else if (map[i][j] == min_power) {// 공격력이 같으면
                    if (last_attack[i][j] > last_attack[a.x][a.y]) {// 더 최근에 공격했으면 업데이트
                        min_power = map[i][j];
                        a.x = i;
                        a.y = j;
                    }
                    else if (last_attack[i][j] == last_attack[a.x][a.y]) {// 공격 시기가 같으면
                        if (i + j > a.x + a.y) {// 행 + 열 값이 더 크면 업데이트
                            min_power = map[i][j];
                            a.x = i;
                            a.y = j;
                        }
                        else if (i + j == a.x + a.y) {// 행 + 열 값이 같으면
                            if (j > a.y) {// 열 값이 더 크면 업데이트
                                min_power = map[i][j];
                                a.x = i;
                                a.y = j;
                            }
                        }
                    }
                }
            }
        }
    }
    
    last_attack[a.x][a.y] = turn;
}

void select_defenser(void)
{
    int max_power = 0;
    d.x = d.y = -1;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (map[i][j] != 0) { // 부서진 포탑이 아니고
                if (!(i == a.x && j == a.y)) { // 공격자가 아닐 때
                    if (max_power < map[i][j] || (d.x == -1 && d.y == -1)) { // 공격력이 더 크면 업데이트
                        // cout << "0여기서 " << i << ',' << j << " 로 업데이트\n";
                        max_power = map[i][j];
                        d.x = i;
                        d.y = j;
                    }
                    else if (max_power == map[i][j]) {// 공격력이 같으면
                        if (last_attack[i][j] < last_attack[d.x][d.y]) {// 공격한지 더 오래됐으면 업데이트
                            // cout << "1여기서 " << i << ',' << j << " 로 업데이트\n";
                            max_power = map[i][j];
                            d.x = i;
                            d.y = j;
                        }
                        else if (last_attack[i][j] == last_attack[d.x][d.y]) {// 공격한 시기가 같으면
                            if (i + j < d.x + d.y) {// 행 + 열이 작으면 업데이트
                                // cout << "2여기서 " << i << ',' << j << " 로 업데이트\n";
                                max_power = map[i][j];
                                d.x = i;
                                d.y = j;
                            }
                            else if (i + j == d.x + d.y) {// 행 + 열이 같으면
                                if (j < d.y) {// 열값이 작으면 업데이트
                                    // cout << "3여기서 " << i << ',' << j << " 로 업데이트\n";
                                    max_power = map[i][j];
                                    d.x = i;
                                    d.y = j;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void bfs(deque<int> dir[10][10])
{
    deque<point> dq;
    
    visited[a.x][a.y] = 1;
    dq.push_back(a);
    
    while(dq.size() != 0) {
        point now = dq.front();
        dq.pop_front();
        for (int i = 0; i < 4; i++) {
            point next;
            next.x = (now.x + dx[i] + N) % N;
            next.y = (now.y + dy[i] + M) % M;
            if (map[next.x][next.y] != 0) {// 부서지지 않았고,
                if (visited[next.x][next.y] == 0) {// 방문하지 않았으면
                    visited[next.x][next.y] = visited[now.x][now.y] + 1; // 방문하기
                    dir[next.x][next.y].push_back(i);
                    dq.push_back(next);
                    if (next.x == d.x && next.y == d.y) // 만약 공격자에 도달했으면 return
                        return;
                }
            }
        }
    }
}

void laser(deque<int> dir[10][10], deque<point>& attacked)
{
    point now, next;
    
    // 방어자의 위치에서 시작한다
    now.x = next.x = d.x;
    now.y = next.y = d.y;
    // 방어자의 공격력을 낮춘다
    map[d.x][d.y] -= map[a.x][a.y];
    if (map[d.x][d.y] < 0)
        map[d.x][d.y] = 0;
    
    next.x = (now.x - dx[dir[now.x][now.y][0]] + N) % N;
    next.y = (now.y - dy[dir[now.x][now.y][0]] + M) % M;
//    cout << "NEXT 시작: " << next.x << ',' << next.y << '\n';
    
    while (!(next.x == a.x && next.y == a.y)) {
        attacked.push_back(next); // 공격받은 좌표에 next를 추가한다
        map[next.x][next.y] -= (map[a.x][a.y] / 2); // next의 공격력을 감소시킨다
        if (map[next.x][next.y] < 0)
            map[next.x][next.y] = 0;
        // 다음 좌표를 구한다
        now.x = next.x;
        now.y = next.y;
        next.x = (now.x - dx[dir[now.x][now.y][0]] + N) % N;
        next.y = (now.y - dy[dir[now.x][now.y][0]] + M) % M;
//        cout << "NEXT는: " << next.x << ',' << next.y << '\n';
        if (next.x == a.x && next.y == a.y) // next가 a이면 종료한다
            return;
    }
}

void throw_power(deque<point>& attacked)
{
    // 방어자의 공격력을 낮춘다
    map[d.x][d.y] -= map[a.x][a.y];
    if (map[d.x][d.y] < 0)
        map[d.x][d.y] = 0;
    
    int xx[] = {0, 0, 1, 1, 1, -1, -1, -1}; // 8방향
    int yy[] = {1, -1, 0, 1, -1, 0, 1, -1};
    
    for (int i = 0; i < 8; i++) {// 8 방향의 포탑을 순회한다
        point next;
        next.x = (d.x + xx[i] + N) % N;
        next.y = (d.y + yy[i] + M) % M;
        if (map[next.x][next.y] != 0) {// 0 이 아니고
            if (!(next.x == a.x && next.y == a.y)) {// 공격자가 아니면
                map[next.x][next.y] -= (map[a.x][a.y] / 2); // 공격하고 공격력을 낮추고 attacked에 추가시킨다.
                if (map[next.x][next.y] < 0)
                    map[next.x][next.y] = 0;
                attacked.push_back(next);
            }
        }
    }
}

void plus_power(deque<point> attacked)
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (!(i == a.x && j == a.y)) { // 공격자가 아니고
                if (!(i == d.x && j == d.y)) { // 방어자가 아니며
                    if (map[i][j] != 0) { // 부서지지 않았고
                        bool is_attacked = false;
                        for (int k = 0; k < attacked.size(); k++) {
                            if (i == attacked[k].x && j == attacked[k].y)
                                is_attacked = true;
                        }
                        if (!is_attacked) {// 공격받지 않았으면
                            map[i][j]++; // 공격력 + 1
                        }
                    }
                }
            }
        }
    }
}

int left_tower(void)
{
    int cnt = 0;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (map[i][j] != 0)
                cnt++;
            if (cnt >= 2)
                return cnt;
        }
    }
    
    return cnt;
}

int main(void)
{
    cin >> N >> M >> K;
    
    // 맵 정보 입력
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            int x;
            cin >> x;
            map[i][j] = x; // 맵에 포탑의 공격력을 저장
            last_attack[i][j] = 0; // 마지막 공격 턴을 0으로 초기화
        }
    }
    
    // turn 을 돌며 동작 수행
    for (turn = 1; turn <= K; turn++) {
        // cout << "\n\n\nTURN: " << turn + 1 << '\n';
        if (left_tower() == 1) // 남은 포탑 개수 확인
            break;
        select_attacker(); // 공격자를 선정한다
        // cout << "ATTACKER: " << a.x << ',' << a.y << '\n';
        map[a.x][a.y] += (N + M);// 공격자의 공격력을 올려준다
        // cout << "공격자 힘: " << map[a.x][a.y] << '\n';
        select_defenser(); // 가장 강한 포탑을 설정한다
        // cout << "DEFENSER: " << d.x << ',' << d.y << '\n';
        init_visited();
        deque<int> dir[10][10];
        bfs(dir); // bfs로 방문한다 (방문하며 경로로 도달하는 방향들도 저장)
        // cout << "방어자까지의 경로는 총 : " << visited[d.x][d.y] << '\n';
        deque<point> attacked; // 직간접적으로 공격받은 것들을 저장할 변수
        if (visited[d.x][d.y] != 0) {// 대상자의 visited[][]가 0이 아니라면
            // cout << "LASER\n";
            laser(dir, attacked); // 레이저
            // cout << "공격받은 사람은: ";
            // for (int i = 0; i < attacked.size(); i++) {
            //     cout << attacked[i].x << ',' << attacked[i].y << "와 ";
            // }
            // cout << '\n';
        }
        else { // 대상자를 레이저 할 수 없으면
            // cout << "THROW\n";
            throw_power(attacked); // 포탄 공격
        }
        if (left_tower() == 1) // 남아있는 포탑 개수 확인
            break;
        plus_power(attacked); // 공격받지 않은 포탑들을의 공격력을 + 1 해준다
        // print_map();
    }
    
    int answer = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (map[i][j] > answer)
                answer = map[i][j];
        }
    }
    
    cout << answer;
    
    return 0;
}