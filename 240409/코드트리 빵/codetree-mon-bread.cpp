#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

typedef struct Base_info {
    int index;
    int x;
    int y;
    bool selected;
} base_info;

typedef struct Store_info {
    int index;
    int x;
    int y;
    bool visited;
} store_info;

typedef struct User_info {
    int base_idx;
    int x;
    int y;
    bool is_moving;
} user_info;

typedef struct Point {
    int x;
    int y;
} point;

int n, m, base_cnt, in_store_cnt;
int total_time;
int visited[15][15] = {0, };
vector<base_info> bases;
vector<store_info> stores;
vector<user_info> users;
int map[15][15] = {0, };
int dx[] = {-1, 0, 0, 1}; // 상 좌 우 하
int dy[] = {0, -1, 1, 0};

void init_visited(void)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            visited[i][j] = 0;
}

bool is_inside(int x, int y)
{
    if (0 <= x && x < n && 0 <= y && y < n)
        return true;
    else
        return false;
}

void store_to_base_bfs(int idx) // user의 idx(즉, stores의 idx)
// store에서 base로의 최단경로를 찾는 bfs
{
    deque<point> dq;
    point now, next;
    now.x = stores[idx].x;
    now.y = stores[idx].y;
    visited[now.x][now.y] = 1;
    dq.push_back(now);
    
    while (dq.size() != 0) {
        now.x = dq.front().x;
        now.y = dq.front().y;
        dq.pop_front();
        for (int i = 0; i < 4; i++) {
            next.x = now.x + dx[i];
            next.y = now.y + dy[i];
            if (is_inside(next.x, next.y)) {
                if (visited[next.x][next.y] == 0) {
                    if (map[next.x][next.y] != -1) {
                        visited[next.x][next.y] = visited[now.x][now.y] + 1;
                        dq.push_back(next);
                    }
                }
            }
        }
    }
}

void select_basecamp(int user_idx)
{
    store_to_base_bfs(user_idx); // store에서 base로 bfs 하고
    
    int min_dist = 15 * 15 + 1;
    int min_idx = -1;
    for (int i = 0; i < base_cnt; i++) { // base를 전체를 돌며
        if (bases[i].selected == false) {
            if (visited[bases[i].x][bases[i].y] < min_dist || min_idx == -1) {// 거리가 min_dist보다 작으면
                min_dist = visited[bases[i].x][bases[i].y]; // min_dist 업데이트
                min_idx = i; // min_idx 업데이트
            }
            else if (visited[bases[i].x][bases[i].y] == min_dist) { // 거리가 같으면
                if (bases[i].x < bases[min_idx].x) {// 만약 i의 행이 min_idx의 행보다 작으면
                    min_dist = visited[bases[i].x][bases[i].y]; // 업데이트
                    min_idx = i;
                }
                else if (bases[i].x == bases[min_idx].x) {// 만약 i의 행이 min_idx의 행과 같으면
                    if (bases[i].y < bases[min_idx].y) {// 만약 i의 열이 min_idx의 열보다 작으면
                        min_dist = visited[bases[i].x][bases[i].y]; // 업데이트
                        min_idx = i;
                    }
                }
            }
        }
    }
    
    users[user_idx].base_idx = min_idx;
}

void user_to_store_bfs(int idx, deque<int> direction[15][15]) // user의 idx == store의 idx를 전달받음
// user에서 store로 가는 bfs를 구함
// direction[i][j]에는 전에 어떤 방향으로 이동해서 이곳에 왔는지 방향을 저장
{
    deque<point> dq;
    point now;
    point next;
    now.x = users[idx].x;
    now.y = users[idx].y;
    visited[now.x][now.y] = 1;
    // direction[now.x][now.y].push_back(-1);
    dq.push_back(now);
    
    while (dq.size() != 0) {
        now.x = dq.front().x;
        now.y = dq.front().y;
        dq.pop_front();
        for (int i = 0; i < 4; i++) { // 네 방향 탐색
            next.x = now.x + dx[i]; // 다음 위치 선정
            next.y = now.y + dy[i];
            if (is_inside(next.x, next.y)) { // 만약 next가 맵 내부에 있고
                if (visited[next.x][next.y] == 0) { // 아직 방문하지 않았고
                    if (map[next.x][next.y] != -1) { // 맵에서 방문할 수 없는 곳이 아니면
                        direction[next.x][next.y].push_back(i); // 다음 위치에 어떤 방향에서 왔는지를 저장해주고
                        dq.push_back(next); // dq에 넣어주고
                        visited[next.x][next.y] = 1; // visited를 1로 처리한다
//                        cout << "NEXT: " << next.x << ' ' << next.y << ", dir: " << direction[next.x][next.y].front() << '\n';
                        if (next.x == stores[idx].x && next.y == stores[idx].y) // 만약 next가 store라면 리턴
                            return;
                    }
                }
            }
        }
    }
}

int select_route(int idx, deque<int> direction[15][15]) // 다음 이동할 direction을 리턴
// store에서 출발해서 현재 유저 위치에 도달하면 멈춤
// 현재 유저 위치에서 가야할 dir을 리턴함
{
    point now;
    point next;
    int now_dir = -1; // 현재 칸에서 이동한 dir
    int change_dir = -1; // 반대로 추적하려면 이동해야할 dir
    
//    cout  << "USER" << idx << " 의 경로를 역추적한다" << '\n';
    
    now.x = next.x = stores[idx].x;
    now.y = next.y = stores[idx].y;
//    cout << "USER" << idx << "의 현재 위치: " << now.x << ' ' << now.y << '\n';
    while (!(now.x == users[idx].x && now.y == users[idx].y)) {
//        cout << "여기\n";
        now_dir = direction[now.x][now.y].front();
        switch (now_dir) {
            case UP: change_dir = DOWN; break;
            case LEFT: change_dir = RIGHT; break;
            case RIGHT: change_dir = LEFT; break;
            case DOWN: change_dir = UP; break;
            default: break;
        };
//        cout << "원래 방향: " << now_dir << ", 변경된 방향: " << change_dir << '\n';
        next.x = now.x + dx[change_dir];
        next.y = now.y + dy[change_dir];
        now.x = next.x;
        now.y = next.y;
    }
    
    return now_dir;
}

void move_user(int idx) // user의 idx를 전달 받음
{
//    cout << idx << "번 유저 이동시키기\n";
    deque<int> direction[15][15];
    int next_dir = -1;
    init_visited();
    user_to_store_bfs(idx, direction); // user부터 store까지의 bfs를 구하고
    next_dir = select_route(idx, direction); // 다음 이동할 direction을 구하고
    if (next_dir != -1) {// 그 위치로 이동
        users[idx].x += dx[next_dir];
        users[idx].y += dy[next_dir];
    }
    
//    cout << idx << "번 유저 " << users[idx].x << ' '<< users[idx].y << " 로 이동\n";
}

int main(void)
{
    cin >> n >> m;
    base_cnt = 0; // base의 수를 셀 변수
    in_store_cnt = 0; // store에 도착한 유저 명수를 셀 변수
    
    for (int i = 0; i < n; i++) { // map 정보 입력 받으며 bases 벡터 생성
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
            if (map[i][j] == 1) {
                base_info b;
                b.index = base_cnt;
                b.x = i;
                b.y = j;
                b.selected = false;
                bases.push_back(b);
                base_cnt++;
            }
        }
    }
    
    // 편의점 정보 입력 받으며 stores 벡터 생성
    for (int i = 0; i < m; i++) {
        store_info s;
        cin >> s.x >> s.y;
        s.x -= 1;
        s.y -= 1;
        s.index = i;
        s.visited = false;
        stores.push_back(s);
    }
    
    // user_info 초기화해서(-1, -1, -1) users 벡터 생성
    for (int i = 0; i < m; i++) {
        user_info u;
        u.x = -1;
        u.y = -1;
        u.base_idx = -1;
        u.is_moving = false;
        users.push_back(u);
    }
    
    // total_time 0부터 m - 1까지 유저를 넣고 이동시키며 users 벡터 생성
    for (int i = 0; i < m; i++) {
        if (total_time == 0) { // 초기상태일때
//            cout << "첫번째 유저 차례\n";
            init_visited();
            select_basecamp(i); // user0의 베이스 캠프를 정하고,
            users[i].x = bases[users[i].base_idx].x; // user0을 거기로 이동시키고
            users[i].y = bases[users[i].base_idx].y;
            map[users[i].x][users[i].y] = -1; // user0의 베이스캠프를 map 에서 -1로 만들기
            bases[users[i].base_idx].selected = true; // user0의 베이스캠프를 selected로 만든다
            users[i].is_moving = true; // user0의 is_moving을 true로 만든다
//            cout << "첫번째 유저의 베이스 캠프: " << bases[users[i].base_idx].x << ' ' << bases[users[i].base_idx].y << '\n';
        }
        else {
            for (int j = 0; j < i; j++) { // user들을 확인하며
                if (users[j].is_moving == true) {// 현재 맵에 있는 유저라면
                    move_user(j); // 이동시키기
                }
            }
            for (int j = 0; j < i; j++) { // user들을 확인하며
                if (users[j].is_moving == true) { // 현재 맵에 있는 유저라면
                    if (users[j].x == stores[j].x && users[j].y == stores[j].y) {// 편의점에 도착한 사람이 있으면
                        users[j].is_moving = false; // user 이동을 멈추고
                        stores[j].visited = true; // 그 칸을 visited로 만들고
                        map[stores[j].x][stores[j].y] = -1; // map에서 -1로 만들고
                        in_store_cnt++; // store에 들어간 user 수를 증가시켜준다
//                        cout << "USER" << j << " 는 이제 움직이지 않는다\n";
                    }
                }
            }
//            cout << "USER" << i << " 의 basecamp: ";
            init_visited();
            select_basecamp(i); // user(i)의 베이스 캠프를 정해서
            users[i].x = bases[users[i].base_idx].x; // user(i)을 거기로 이동시키고
            users[i].y = bases[users[i].base_idx].y;
            map[users[i].x][users[i].y] = -1; // user(i)의 베이스캠프를 map에서 -1로 만들고
            bases[users[i].base_idx].selected = true; // user(i)의 베이스캠프를 selected로 만든다
            users[i].is_moving = true; // user(i)의 활동 상태를 true로 만든다
//            cout << bases[users[i].base_idx].x << ' ' << bases[users[i].base_idx].y << '\n';
        }
        total_time++;
//        cout << "TIME: " << total_time << '\n';
    }
    
    while (in_store_cnt < m) { // store에 모든 유저가 도착할때까지 반복
        for (int i = 0; i < m; i++) {// user들을 확인하며
            if (users[i].is_moving == true) // 현재 맵에 있는 유저라면
                move_user(i); // 이동시키기
        }
        for (int i = 0; i < m; i++) { // user들을 확인하며
            if (users[i].is_moving == true) {// 현재 맵에 있는 유저라면
                if (users[i].x == stores[i].x && users[i].y == stores[i].y) {// 편의점에 도착했다면
                    users[i].is_moving = false; // user 이동을 멈추고
                    stores[i].visited = true; // 그 칸을 visited로 만들고
                    map[stores[i].x][stores[i].y] = -1; // map에서 -1로 만들고
                    in_store_cnt++; // store에 들어간 user 수를 증가시켜준다
//                    cout << "USER" << i << " 는 이제 움직이지 않는다\n";
                }
            }
        }
        total_time++;
//        cout << "TIME: " << total_time << '\n';
    }
    
    cout << total_time;
    
    return 0;
}