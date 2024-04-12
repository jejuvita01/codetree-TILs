#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>

using namespace std;

#define TIME 10
#define REV 11
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

typedef struct Point {
    int x;
    int y;
} point;

typedef struct Team_info {
    int idx; // team의 인덱스
    deque<point> people; // 사람들 좌표 덱 (머리사람~꼬리사람 순)
} team_info;

int N, M, K;
int turn; // 현재 턴 수를 나타내는 변수
int map[20][20] = {0, };
int visited[20][20] = {0, };
vector<team_info> team;
long long total_score = 0;

/*
int time_dx[] = {-1, 0, 1, 0}; // 상 우 하 좌
int time_dy[] = {0, 1, 0, -1};
int rev_dx[] = {1, 0, -1, 0}; // 하 우 상 좌
int rev_dy[] = {0, 1, 0, -1};
 */

int dx[] = {-1, 0, 1, 0}; // 상 우 하 좌
int dy[] = {0, 1, 0, -1};


bool is_inside(int x, int y)
{
    if (0 <= x && x < N && 0 <= y && y < N)
        return true;
    else
        return false;
}

void init_visited(void)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            visited[i][j] = 0;
}

void print_map(void)
{
    cout << "\nMAP\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
}

void dfs(int i, int j, int idx, point& tail_point)
{
    visited[i][j] = 1;
    point p = {i, j};
    
    team[idx].people.push_back(p);
    
    if (i == tail_point.x && j == tail_point.y)
        return;
    
    if (map[i][j] == 3) { // 꼬리 사람이면 리턴
//        cout << i << ',' << j << "가 꼬리 사람이라 리턴한다\n";
        return;
    }
    
    bool is_anything_near = false;
    for (int d = 0; d < 4; d++) {
        int next_x = i + dx[d];
        int next_y = j + dy[d];
        if (is_inside(next_x ,next_y)) { // 격자 내이고
            if (visited[next_x][next_y] == 0) { // 방문하지 않았고
                if (map[next_x][next_y] == 2 || map[next_x][next_y] == 3) { // 중간 사람이거나 꼬리 사람이면
                    if (map[next_x][next_y] == 3) { // 꼬리 사람이면 좌표 저장해놓기
                        tail_point = {next_x, next_y};
                    }
                    if (map[next_x][next_y] == 2) { // 중간 사람이면 바로 dfs
                        dfs(next_x, next_y, idx, tail_point);
                        is_anything_near = true;
                    }
                }
            }
        }
    }
    
    if (is_anything_near == false)
        dfs(tail_point.x, tail_point.y, idx, tail_point);
}

void move_people(int idx) // team의 idx
{
    point from, to;
    int tail = 0;
    
    // 일단 머리를 이동시킨다
    from = team[idx].people.front();
    
    // 일단 머리 사람을 한 칸 이동시킨다
    for (int i = 0; i < 4; i++) {
        to.x = from.x + dx[i];
        to.y = from.y + dy[i];
        if (is_inside(to.x, to.y)) {
            if (map[to.x][to.y] == 4 || map[to.x][to.y] == 3) { // 다음 칸이 이동선이거나 꼬리 사람이면
                team[idx].people[0].x = to.x; // 머리 사람을 to로 이동시킨다
                team[idx].people[0].y = to.y;
                tail = map[to.x][to.y];
                map[to.x][to.y] = 1; // 머리 사람이 이동했음을 map에 표시한다
                to.x = team[idx].people[0].x + dx[(i + 2) % 4]; // 이후 이동을 위해 to를 원래 머리 사람의 위치로 이동시킨다
                to.y = team[idx].people[0].y + dy[(i + 2) % 4];
                break; // 바로 반복문을 종료한다
            }
        }
    }

    // to는 현재 머리 사람의 이전 좌표이다
    for (int i = 1; i < team[idx].people.size(); i++) { // 2번~3번 사람을 그 전사람의 좌표로 이동시킨다
        from.x = team[idx].people[i].x; // team[idx].people[i]의 좌표를 from에 복사하고
        from.y = team[idx].people[i].y;
        team[idx].people[i].x = to.x; // team[idx].people[i]에 to의 좌표를 복사한다
        team[idx].people[i].y = to.y;
//        cout << map[from.x][from.y] << " 번째 사람 옮기기\n";
        map[to.x][to.y] = map[from.x][from.y]; // map을 변경해준다
//        print_map();
        to.x = from.x;
        to.y = from.y;
        
//        print_map();
    }
    
    if (tail == 4)
        map[from.x][from.y] = 4; // 꼬리사람이 이동한 흔적을 map에서 4로 지워준다
    else
        map[from.x][from.y] = 3;
}

point throw_ball(void)
{
    int div = turn / N; // 몫
    int left = turn % N; // 나머지
    point p = {-1, -1}; // 제일 처음 부딪힌 사람의 포인트를 리턴
    
    // cout << "현재 div와 left는 : " << div << ',' << left << '\n';
    
    if (div % 4 == 0) { // 좌 -> 우
        // cout << "공은 i = " << left << "에서 j : " << 0 << "~" << N - 1 << "까지 순회한다\n";
        for (int j = 0; j < N; j++) { // 공을 던진다
            if (!(map[left][j] == 4 || map[left][j] == 0)) { // 사람이 있는지 확인한다
//                cout << left << ',' << j << "에 사람이 있어서 " << map[left][j] << "의 제곱만큼 점수 증가\n";
                p.x = left;
                p.y = j;
                break;
            }
        }
    }
    
    else if (div % 4 == 1) { // 하 -> 상
        // cout << "공은 j = " << left << "에서 i : " << N - 1 << "~" << 0 << "까지 순회한다\n";
        for (int i = N - 1; i >= 0; i--) { // 공을 던진다
            if (!(map[i][left] == 4 || map[i][left] == 0)) { // 사람이 있는지 확인한다
//                cout << i << ',' << left << "에 사람이 있어서 " << map[i][left] << "의 제곱만큼 점수 증가\n";
                p.x = i;
                p.y = left;
                break;
            }
        }
    }
    
    else if (div % 4 == 2) { // 우 -> 좌
        // cout << "공은 i = " << N - left - 1 << "에서 j : " << N - 1 << "~" << 0 << "까지 순회한다\n";
        for (int j = N - 1; j >= 0; j--) { // 공을 던진다
            if (!(map[N - left - 1][j] == 4 || map[N - left - 1][j] == 0)) { // 사람이 있는지 확인한다
//                cout << N - left - 1 << ',' << j << "에 사람이 있어서 " << map[N - left - 1][j] << "의 제곱만큼 점수 증가\n";
                p.x = N - left - 1;
                p.y = j;
                break;
            }
        }
    }
    
    else { // 상 -> 하
        // cout << "공은 j = " << N - left - 1 << "에서 i : " << 0 << "~" << N - 1 << "까지 순회한다\n";
        for (int i = 0; i < N; i++) { // 공을 던진다
            if (!(map[i][N - left - 1] == 4 || map[i][N - left - 1] == 0)) { //사람이 있는지 확인한다
//                cout << i << ',' << N - left - 1 << "에 사람이 있어서 " << map[i][N - left - 1] << "의 제곱만큼 점수 증가\n";
                p.x = i;
                p.y = N - left - 1;
                break;
            }
        }
    }
    
    
    return p;
}

int find_team(point p)
{
    int t = -1;
    
    for (t = 0; t < M; t++) {
        for (int i = 0; i < team[t].people.size(); i++) {
            if (p.x == team[t].people[i].x && p.y == team[t].people[i].y) {
                total_score += (i + 1) * (i + 1); // 몇번째 사람인지 확인했으니 점수를 올려준다
                return t;
            }
        }
    }
        
    return t;
}

void reverse_team(int t)
{
    map[team[t].people.front().x][team[t].people.front().y] = 3;
    map[team[t].people.back().x][team[t].people.back().y] = 1;
    reverse(team[t].people.begin(), team[t].people.end());
}

int main(void)
{
    cin >> N >> M >> K;
    
    int team_idx = 0;
    
    // 맵 정보 입력
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // 머리 사람이 나오면
            if (map[i][j] == 1) {
                // cout << "TEAM: " << team_idx << '\n';
                team_info t;
                t.idx = team_idx;
                team.push_back(t);
                point p = {-1, -1};
                dfs(i, j, team_idx, p);
                team_idx++;
            }
        }
    }
    
    // for (int i = 0; i < M; i++) {
    //     cout << i + 1 << "번째 팀은 " << team[i].idx << "의 인덱스를 가지고, \n";
    //     for (int j = 0; j < team[i].people.size(); j++) {
    //         cout << '(' << team[i].people[j].x << ',' << team[i].people[j].y << ')' << ',';
    //     }
    //     cout << "순서대로 사람이 있다.\n";
    // }
    

    // 턴을 돈다
    for (turn = 0; turn < K; turn++) {
        //  cout << "TURN: " << turn << '\n';
        for (int i = 0; i < M; i++) { // 팀을 순회하며
            move_people(i); // 팀의 사람들을 이동시킨다
            // print_map();
        }
        point p = throw_ball(); // 공을 던진다
        if (!(p.x == -1 && p.y == -1)) {
            int t = find_team(p); // p가 -1, -1 이 아니면 p가 속한 팀을 찾는다
//            cout << "\n\np: " << p.x << ',' << p.y << "는 " << t << "번 팀에 속해있다\n";
            if (t != -1) {
                reverse_team(t); // p가 속한 팀의 방향을 바꾼다(머리, 꼬리 맵 값도 바꿔야함)
//                print_map();
//                for (int i = 0; i < M; i++) {
//                    cout << i + 1 << "번째 팀은 " << team[i].idx << "의 인덱스를 가지고, \n";
//                    for (int j = 0; j < team[i].people.size(); j++) {
//                        cout << '(' << team[i].people[j].x << ',' << team[i].people[j].y << ')' << ',';
//                    }
//                    cout << "순서대로 사람이 있다.\n";
//                }
            }
        }
        
        //  cout << "현재 점수는: " << total_score << '\n';
    }
    
    cout << total_score << '\n';
    
    return 0;
}


/*
7 2 49
3 2 1 0 0 0 0
4 0 4 0 2 1 4
4 4 4 0 2 0 4
0 0 0 0 3 0 4
0 0 4 4 4 0 4
0 0 4 0 0 0 4
0 0 4 4 4 4 4
*/