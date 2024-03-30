#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>

using namespace std;

int n, m, k, turn;
int power[10][10];
int bfs_power[10][10];
int visited[10][10];
int recent_attack[10][10] = {0, };
int dx[] = {0, 1, 0, -1};
int dy[] = {1, 0, 0, -1};


typedef struct {
    int x;
    int y;
} point;

point attacker, defenser;

void init_visited(void)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            visited[i][j] = 0;
}

point select_attacker(void)
{
    int min_power = 5001;
    int min_turn = 0;
    point min_pos = {0, 0};
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (power[i][j] != 0) {
                if (power[i][j] < min_power) { // 공격력이 제일 약한 경우
                    min_power = power[i][j];
                    min_turn = recent_attack[i][j];
                    min_pos.x = i;
                    min_pos.y = j;
                }
                else if (power[i][j] == min_power) {
                    if (recent_attack[i][j] > min_turn) { //제일 최근에 공격한 경우
                        min_power = power[i][j];
                        min_turn = recent_attack[i][j];
                        min_pos.x = i;
                        min_pos.y = j;
                    }
                    else if (recent_attack[i][j] == min_turn) {
                        if (min_pos.x + min_pos.y < i + j) { // 행 + 열 값이 가장 큰 경우
                            min_power = power[i][j];
                            min_turn = recent_attack[i][j];
                            min_pos.x = i;
                            min_pos.y = j;
                        }
                        else if (min_pos.x + min_pos.y == i + j) {
                            if (min_pos.y < j) { // 열 값이 가장 큰 경우
                                min_power = power[i][j];
                                min_turn = recent_attack[i][j];
                                min_pos.x = i;
                                min_pos.y = j;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return min_pos;
}

point select_defenser(void)
{
    int max_power = 0;
    int max_turn = 1001;
    point max_pos = {10, 10};
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (power[i][j] > max_power) { // 공격력이 제일 강한 경우
                max_power = power[i][j];
                max_turn = recent_attack[i][j];
                max_pos.x = i;
                max_pos.y = j;
            }
            else if (power[i][j] == max_power) {
                if (recent_attack[i][j] < max_turn) { // 제일 오래 전에 공격한 경우
                    max_power = power[i][j];
                    max_turn = recent_attack[i][j];
                    max_pos.x = i;
                    max_pos.y = j;
                }
                else if (recent_attack[i][j] == max_turn) {
                    if (max_pos.x + max_pos.y > i + j) { // 행 + 열 값이 가장 작은 경우
                        max_power = power[i][j];
                        max_turn = recent_attack[i][j];
                        max_pos.x = i;
                        max_pos.y = j;
                    }
                    else if (max_pos.x + max_pos.y == i + j) {
                        if (max_pos.y > j) { // 열 값이 가장 작은 경우
                            max_power = power[i][j];
                            max_turn = recent_attack[i][j];
                            max_pos.x = i;
                            max_pos.y = j;
                        }
                    }
                }
            }
        }
    }
    
    return max_pos;
}

void bfs(void)
{
    deque<point> dq;
    
    visited[attacker.x][attacker.y] = 1;
    dq.push_back(attacker);
    
    while (dq.size() != 0) {
        int ori_x = dq.front().x;
        int ori_y = dq.front().y;
        dq.pop_front();
        int new_x, new_y;
        for (int i = 0; i < 4; i++) {
            new_x = ori_x + dx[i];
            new_y = ori_y + dy[i];
            new_x = (new_x + n) % n;
            new_y = (new_y + m) % m;
            if (new_x == defenser.x && new_y == defenser.y) {
                visited[new_x][new_y] = 1;
                bfs_power[defenser.x][defenser.y] -= bfs_power[attacker.x][attacker.y];
                if (bfs_power[defenser.x][defenser.y] < 0)
                    bfs_power[defenser.x][defenser.y] = 0;
                return;
            }
            if (bfs_power[new_x][new_y] != 0 && !visited[new_x][new_y]) {
                visited[new_x][new_y] = 1;
                bfs_power[new_x][new_y] -= (bfs_power[attacker.x][attacker.y] / 2);
                if (bfs_power[new_x][new_y] < 0)
                    bfs_power[new_x][new_y] = 0;
                point next = {new_x, new_y};
                dq.push_back(next);
            }
        }
    }
}

void bomb_attack(void)
{
    int x_move[] = {0, 0, 1, 1, 1, -1, -1, -1};
    int y_move[] = {1, -1, 0, 1, -1, 0, 1, -1};
    
    power[defenser.x][defenser.y] -= power[attacker.x][attacker.y];
    if (power[defenser.x][defenser.y] < 0)
        power[defenser.x][defenser.y] = 0;
    
    for (int i = 0; i < 8; i++) {
        int new_x = defenser.x + x_move[i];
        int new_y = defenser.y + y_move[i];
        new_x = (new_x + n) % n;
        new_y = (new_y + m) % m;
        if (new_x != attacker.x && new_y != attacker.y) {
            if (power[new_x][new_y] >= (power[attacker.x][attacker.y] / 2))
                power[new_x][new_y] -= (power[attacker.x][attacker.y] / 2);
            else
                power[new_x][new_y] = 0;
        }
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (i != attacker.x && j != attacker.y) {
                if (i != defenser.x && j != defenser.y) {
                    for (int k = 0; k < 8; k++) {
                        int new_x = defenser.x + x_move[k];
                        int new_y = defenser.y + y_move[k];
                        new_x = (new_x + n) % n;
                        new_y = (new_y + m) % m;
                        if (i != new_x && j != new_y) {
                            if (power[i][j] != 0)
                                power[i][j]++;
                        }
                    }
                }
            }
        }
    }
}

void copy_power(int dest[10][10], int copy[10][10])
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            dest[i][j] = copy[i][j];
        }
    }
}

int get_max_power(void)
{
    int max = 0;
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (power[i][j] > max)
                max = power[i][j];
    
    return max;
}

void print_power(void)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cout << power[i][j] << ' ';
        }
        cout << '\n';
    }
}

int main() {
    cin >> n >> m >> k;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int x;
            cin >> x;
            power[i][j] = x;
        }
    }

    for (turn = 0; turn < k; turn++) {
        attacker = select_attacker(); // attacker 구하기
        power[attacker.x][attacker.y] += (n + m); // attacker 공격력 올려주기
        defenser = select_defenser(); // defenser 구하기
        
        // 레이저 가능하면 레이저 하고 아니면 포탄 해야함
        init_visited();
        copy_power(bfs_power, power);
        bfs();
        if (visited[defenser.x][defenser.y] == 0) {
            bomb_attack();
            // 여기서 + 1 해줘야함
        }
        else {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    if (power[i][j] != bfs_power[i][j] && bfs_power[i][j] != 0 && power[i][j] != 0) {
                        if (i != attacker.x && j != attacker.y) {
                            if (i != defenser.x && j != defenser.y) {
                                bfs_power[i][j]++;
                            }
                        }
                    }
                }
            }
            copy_power(power, bfs_power); // power에 laser_power 복사하기
        }
    }
    
    // print_power();
    
    cout << get_max_power() << '\n';

    return 0;
}