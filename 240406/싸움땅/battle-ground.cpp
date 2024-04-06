#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef struct {
    int x; // x좌표
    int y; // y좌표
    int d; // 방향 0123이 상우하좌
    int power; // 초기 능력치
    int gun; // 갖고 있는 총의 능력치
    int score; // 획득한 점수
} user_info;

int n, m, k;
vector<int> map[20][20];
vector<user_info> user;
int dx[4] = {-1, 0, 1, 0}; // d에 2를 더하면 반대 방향이 됨
int dy[4] = {0, 1, 0, -1};

bool comp(int a, int b) // 벡터를 오름차순으로 정렬할때 쓸 함수
{
    return a > b;
}

void print_score(void)
{
    // cout << "\nSCORE\n";
    
    for (int u = 0; u < m; u++) {
//        cout << "U" << u << ":" << user[u].score << ' '; // 디버깅용
        cout << user[u].score << ' ';
    }
    
    // cout << '\n';
}

void print_user(void)
{
    cout << "\nUSER MAP\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int is_user = -1;
            for (int u = 0; u < user.size(); u++) {
                if (user[u].x == i && user[u].y == j) {
                    is_user = u;
                    break;
                }
            }
            if (is_user == -1)
                cout << "0  ";
            else
                cout << "U" << is_user << ' ';
        }
        cout <<'\n';
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
    // 1. 유저를 이동시킨다
    if (!is_inside(user[u].x + dx[user[u].d], user[u].y + dy[user[u].d])) // 유저가 격자 밖으로 벗어나면
        user[u].d = (user[u].d + 2) % 4; // 방향을 반대로 바꿔준다
    
    user[u].x += dx[user[u].d];
    user[u].y += dy[user[u].d];
    
    int ux = user[u].x;
    int uy = user[u].y;
//    cout << ux << ' ' << uy << '\n';
    
    
    // 이동한 칸에 유저가 있는지 확인한다
    int fighter = -1; // 싸울 사람이 있으면 그 사람의 user 인덱스로 설정한다
    for (int i = 0; i < user.size(); i++) {
        if (i != u) {
            if (ux == user[i].x && uy == user[i].y) {
                fighter = i;
                break;
            }
        }
    }
    
    // 2. 이동한 칸에 플레이어가 없으면
    if (fighter == -1) {
        if (map[ux][uy].size() > 0) { // 총이 있으면
            // 플레이어가 가진 총과 자리의 총들 중 가장 큰 것을 가진다
            sort(map[ux][uy].begin(), map[ux][uy].end(), comp); // 자리의 총 내림차순 정렬
            if (user[u].gun < map[ux][uy][0]) { // 유저의 gun보다 자리의 최대 gun이 클 때
                if (user[u].gun == 0) { // 유저가 총이 없을 경우
//                    cout << "유저" << u << " 건 획득:" << map[ux][uy][0] << '\n';
                    user[u].gun = map[ux][uy][0];
                    map[ux][uy].erase(map[ux][uy].begin(), map[ux][uy].begin() + 1);
                }
                else { // 유저가 총이 있을 경우
                    int user_gun = user[u].gun;
                    user[u].gun = map[ux][uy][0];
                    map[ux][uy][0] = user_gun;
                }
            }
        }
    }
    
    // 3. 이동한 칸에 플레이어가 있으면
    else { // fighter vs u (user의 인덱스들)
        // 싸운다
        // cout << "싸운다" << '\n';
        int loser, winner;
        if (user[fighter].power + user[fighter].gun > user[u].power + user[u].gun) { // fighter가 세면
            loser = u;
            winner = fighter;
        }
        else if (user[u].power + user[u].gun > user[fighter].power + user[fighter].gun) { // u가 세면
            loser = fighter;
            winner = u;
        }
        else { // 힘이 같으면
            if (user[fighter].power > user[u].power) { // 초기 능력치가 fighter가 세면
                loser = u;
                winner = fighter;
            }
            else { // 초기 능력치가 u가 세면
                loser = fighter;
                winner = u;
            }
        }
        
        // cout << "LOSER:" << loser << ", WINNER:" << winner << '\n';
        // winner의 점수를 올려준다
        user[winner].score += ((user[winner].power + user[winner].gun) - (user[loser].power + user[loser].gun));
        
        // 진 사람은 총을 내려놓는다
        map[user[loser].x][user[loser].y].push_back(user[loser].gun); // 자리에 총을 추가하고
        user[loser].gun = 0; // 총을 잃는다
        
        // 이긴 사람은 자리의 총들과 자신의 총 중 큰것을 가진다
        sort(map[user[winner].x][user[winner].y].begin(), map[user[winner].x][user[winner].y].end(), comp); // 자리의 총들을 내림차순 정렬하고
        if (user[winner].gun < map[user[winner].x][user[winner].y][0]) { // winner의 총보다 자리의 총이 크면
            // 각 총을 바꾼다
            int winner_gun = user[winner].gun;
            user[winner].gun = map[user[winner].x][user[winner].y][0];
            map[user[winner].x][user[winner].y][0] = winner_gun;
        }
        
        for (int i = 0; i < 4; i++) { // 진 사람은 원래 방향으로 이동하는데
            int new_d = (user[loser].d + i) % 4;
            int new_x = user[loser].x + dx[new_d];
            int new_y = user[loser].y + dy[new_d];
            bool is_user = false;
            // 이동하려는 곳에 user가 있거나
            for (int k = 0; k < user.size(); k++) {
                if (k != loser && k != winner) {
                    if (is_inside(new_x, new_y)) {
                        if (user[k].x == new_x && user[k].y == new_y) {
                            is_user = true;
                            k = (int)user.size() + 1;
                        }
                    }
                }
            }
            if (is_user)
                continue;
            // 격자 밖이면
            if (!is_inside(new_x, new_y))
                continue;
            else { // 진사람.d를 1씩 증가시키며 빈칸(사람이 없는곳)이 보이면 바로 가고
                user[loser].d = new_d; // loser의 d를 수정해준다
                user[loser].x = new_x;
                user[loser].y = new_y;
                break;
            }
        }
        
        // 진 사람은 이동한 자리에 총이 있으면 가장 큰 것을 가진다
        int lx = user[loser].x;
        int ly = user[loser].y;
        if (map[lx][ly].size() > 0) { // 총이 있으면
            // 플레이어가 가진 총과 자리의 총들 중 가장 큰 것을 가진다
            sort(map[lx][ly].begin(), map[lx][ly].end(), comp); // 자리의 총 내림차순 정렬
            if (user[loser].gun < map[lx][ly][0]) { // 유저의 gun보다 자리의 최대 gun이 클 때
                user[loser].gun = map[lx][ly][0];
                map[lx][ly].erase(map[lx][ly].begin(), map[lx][ly].begin() + 1);
            }
        }
        
        // cout << "LOSER: " << lx << ' ' << ly << '\n';
        // cout << "WINNER: " << user[winner].x << ' ' << user[winner].y << '\n';
    }
}

int main(void)
{
    cin >> n >> m >> k;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            if (x > 0) { // 총이 있으면 map 백터에 push_back
                map[i][j].push_back(x);
            }
            else // 없으면 해당 map[i][j].size() == 0 으로 남아있음
                continue;
        }
    }
    
    for (int i = 0; i < m; i++) {
        user_info u;
        cin >> u.x >> u.y >> u.d >> u.power;
        u.x -= 1;
        u.y -= 1;
        u.gun = u.score = 0;
        user.push_back(u);
    }
    
    for (int turn = 0; turn < k; turn++) {
        for (int u = 0; u < m; u++) {
            move_user(u);
        }
        // print_user();
        // print_score();
    }
    
    print_score();
    
    
    /*
    vector<int> n;
    n.push_back(3);
    n.push_back(5);
    n.push_back(1);
    sort(n.begin(), n.end(), comp);
    
    for (int i = 0; i < n.size(); i++)
        cout << n[i] << '\n';
    */
    
    return 0;
}