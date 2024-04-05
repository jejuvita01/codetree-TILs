#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>

using namespace std;

typedef struct {
    int x;
    int y;
} point;

int n, m;
vector<int> map[15];
int visited[15][15] = {0, };
vector<point> basecamp; // basecamp의 위치를 저장
vector<point> store; // 각 유저가 가야할 편의점의 위치를 저장
vector<point> user; // 유저의 위치를 저장
vector<int> close_base; // 각 유저가 가야할 basecamp의 번호 저장 [0, ]
int answer_time = 0;
int dx[4] = {-1, 0, 0, 1};
int dy[4] = {0, -1, 1, 0};

void init_visited(void)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            visited[i][j] = 0;
}

bool is_inside(point p)
{
    if (0 <= p.x && p.x < n && 0 <= p.y && p.y < n)
        return true;
    else
        return false;
}

void print_map(void)
{
    cout << '\n';
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            point pos = {i, j};
            int base_flag = -1;
            int store_flag = -1;
            int user_flag = -1;
            for (int b = 0; b < basecamp.size(); b++) {
                if (pos.x == basecamp[b].x && pos.y == basecamp[b].y) {
                    base_flag = b;
                    b = (int)basecamp.size() + 1;
                }
            }
            for (int s = 0; s < store.size(); s++) {
                if (pos.x == store[s].x && pos.y == store[s].y) {
                    store_flag = s;
                    s = (int)store.size() + 1;
                }
            }
            for (int u = 0; u < user.size(); u++) {
                if (pos.x == user[u].x && pos.y == user[u].y) {
                    user_flag = u;
                    u = (int)user.size() + 1;
                }
            }
            if (user_flag != -1) {
                cout << "u" << user_flag << ' ';
            }
            else if (store_flag != -1) {
                cout << "s" << store_flag << ' ';
            }
            else if (base_flag != -1) {
                cout << "b" << base_flag << ' ';
            }
            else {
                cout << "0  ";
            }
        }
        cout << '\n';
    }
    
    cout << '\n';
}

void bfs(point p)
{
    deque<point> dq;
    
    visited[p.x][p.y] = 1;
    dq.push_back(p);
    
    while (!dq.empty()) {
        point cur;
        cur.x = dq.front().x;
        cur.y = dq.front().y;
        dq.pop_front();
        for (int i = 0; i < 4; i++) {
            point next;
            next.x = cur.x + dx[i];
            next.y = cur.y + dy[i];
            if (!is_inside(next))
                continue;
            if (visited[next.x][next.y] == 0) {
                visited[next.x][next.y] = visited[cur.x][cur.y] + 1;
                dq.push_back(next);
            }
        }
    }
    
    /*
    cout << '\n';
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << visited[i][j] << ' ';
        }
        cout << '\n';
    }
    */
}

int dist(point a, point b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

void find_basecamp(int u_idx) // vector<int> close_base에 각 유저가 가야할 basecamp의 번호 저장
{
    bfs(store[u_idx]); // 모든 점을 방문할 때의 거리 도출
    
    int min_move = 300;
    int min_base_idx = -1;
    for (int b = 0; b < basecamp.size(); b++) {
        bool is_taken = false;
        for (int u = 0; u < close_base.size(); u++) {
            if (b == close_base[u]) {
                is_taken = true;
                u = (int)close_base.size() + 1;
            }
        }
        if (!is_taken) {
            if (visited[basecamp[b].x][basecamp[b].y] < min_move) {
                min_move = visited[basecamp[b].x][basecamp[b].y];
                min_base_idx = b;
            }
        }
    }
    
    close_base.push_back(min_base_idx);
}

/*
void dfs_user_to_store(int u)
{
    deque<point> dq;
    
    visited[p.x][p.y] = 1;
    dq.push_back(p);
    
    while (!dq.empty()) {
        point cur;
        cur.x = dq.front().x;
        cur.y = dq.front().y;
        dq.pop_front();
        for (int i = 0; i < 4; i++) {
            point next;
            next.x = cur.x + dx[i];
            next.y = cur.y + dy[i];
            if (!is_inside(next))
                continue;
            if (visited[next.x][next.y] == 0) {
                visited[next.x][next.y] = visited[cur.x][cur.y] + 1;
                dq.push_back(next);
            }
        }
    }
}
*/

void move_user(int u)
{
    // 일단 dfs로 최단경로 안구하고 상좌우하 순으로 가까워지면(제곱거리 짧으면) 가는걸로 했는데,
    // 틀리면 여기 의심해보기
    point pos = user[u];
    int min_dist = dist(user[u], store[u]);
    
    for (int i = 0; i < 4; i++) {
        point next;
        next.x = pos.x + dx[i];
        next.y = pos.y + dy[i];
        if (!is_inside(next))
            continue;
        if (map[next.x][next.y] == -1)
            continue;
        if (dist(next, store[u]) < min_dist) {
            user[u].x = next.x;
            user[u].y = next.y;
            break;
        }
    }
}

int moving_user(void)
{
    int answer = m;
    
    for (int i = 0; i < user.size(); i++) {
        if (user[i].x == store[i].x && user[i].y == store[i].y)
            answer--;
    }
    
    return answer;
}

int main(void)
{
    cin >> n >> m;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i].push_back(x);
            if (x == 1) {
                point p = {i, j};
                basecamp.push_back(p);
            }
        }
    }
    
    for (int i = 0; i < m; i++) { // 인원 수만큼 가고싶은 편의점의 위치를 저장
        point p;
        cin >> p.x >> p.y;
        p.x -= 1;
        p.y -= 1;
        store.push_back(p);
        init_visited();
        find_basecamp(i); // 각 인원이 가야할 베이스캠프 저장
    }
    
//    print_map();
//    for (int i = 0; i < m; i++) {
//        cout << "USER" << i << " BASE: " << close_base[i] << '\n';
//    }
    
    for (int time = 0; time < m; time++) { // m명을 1분마다 차례로 격자에 넣기
        answer_time++;
//        cout << '\n' << "TIME: " << answer_time << '\n';
        if (time == 0) { // 0번 유저 들어가기
            point p = basecamp[close_base[time]];
            user.push_back(p);
            map[p.x][p.y] = -1; // 베이스캠프에 들어가면 그 좌표 지나가기 불가
        }
        else {
            // 유저들 이동시키고
            for (int u = 0; u < user.size(); u++) {
                if (!(user[u].x == store[u].x && user[u].y == store[u].y))
                    move_user(u);
            }
            // 새로운 유저 베이스캠프에 넣어주기
            point p = basecamp[close_base[time]];
            user.push_back(p);
            map[p.x][p.y] = -1; // 베이스캠프에 들어가면 그 좌표 지나가기 불가
            for (int u = 0; u < user.size(); u++) {
                if (user[u].x == store[u].x && user[u].y == store[u].y)
                    map[user[u].x][user[u].y] = -1; // 유저가 편의점에 들어가면 그 좌표 지나가기 불가
            }
        }
//        print_map();
    }
    
    while (moving_user() != 0) {
        answer_time++;
//        cout << '\n' << "TIME: " << answer_time << '\n';
        for (int u = 0; u < user.size(); u++) {
            if (!(user[u].x == store[u].x && user[u].y == store[u].y))
                move_user(u);
        }
        for (int u = 0; u < user.size(); u++) {
            if (user[u].x == store[u].x && user[u].y == store[u].y)
                map[user[u].x][user[u].y] = -1; // 유저가 편의점에 들어가면 그 좌표 지나가기 불가
        }
//        print_map();
        if (answer_time > 10)
            break;
    }
    
    cout << answer_time << '\n';
    
    return 0;
}