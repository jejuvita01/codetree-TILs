#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

typedef struct Mon_info {
    int x;
    int y;
    int dir;
} mon_info;

typedef struct Egg_info {
    int x;
    int y;
    int dir;
} egg_info;

typedef struct Pman {
    int x;
    int y;
} pman;

int m, t;
int turn;
pman p;
int map[4][4] = {0, }; // 시체가 있으면 그 시체가 사라질 턴을 저장하는 맵
deque<mon_info> mons;
deque<egg_info> eggs;
int mx[] = {-1, -1, 0, 1, 1, 1, 0, -1}; // 몬스터 이동
int my[] = {0, -1, -1, -1, 0, 1, 1, 1};
int px[] = {-1, 0, 1, 0}; // 팩맨 이동
int py[] = {0, -1, 0, 1}; // 상 좌 하 우

void print_map(int map[4][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
}

bool is_inside(int x, int y)
{
    if (0 <= x && x < 4 && 0 <= y && y < 4)
        return true;
    else
        return false;
}

int how_many_monster_here(int x, int y)
{
    int cnt = 0;
    
    for (int i = 0; i < mons.size(); i++)
        if (mons[i].x == x && mons[i].y == y)
            cnt++;
    
    return cnt;
}

void get_dir(int x, int y, int best_dir[3])
{
    int i_x, i_y, j_x, j_y, k_x, k_y;
    int mon_i, mon_j, mon_k;
    int max_mon = 0;
    
    bool is_updated = false;
    
    for (int i = 0; i < 4; i++) {
        i_x = p.x + px[i];
        i_y = p.y + py[i];
        if (is_inside(i_x, i_y)) {
            mon_i = how_many_monster_here(i_x, i_y);
            // cout << "i: " << i_x << ',' << i_y <<"에 몬스터 " << mon_i << "마리\n";
            for (int j = 0; j < 4; j++) {
                j_x = i_x +  px[j];
                j_y = i_y +  py[j];
                if (is_inside(j_x, j_y)) {
                    mon_j = how_many_monster_here(j_x, j_y);
                    // cout << "j: " << j_x << ',' << j_y <<"에 몬스터 " << mon_j << "마리\n";
                    for (int k = 0; k < 4; k++) {
                        k_x = j_x + px[k];
                        k_y = j_y + py[k];
                        if (is_inside(k_x, k_y)) {
                            mon_k = how_many_monster_here(k_x, k_y);
                            int total = 0;
                            total += mon_i; // i를 일단 더하고
                            if (!(i_x == j_x && i_y == j_y)) { // i j 좌표가 같지 않으면
                                total += mon_j; // j를 더하고
                                if (!(i_x == k_x && i_y == k_y) && !(j_x == k_x && j_y == k_y)) // k가 i j 좌표와 같지 않으면
                                    total += mon_k; // k도 더한다
                            }
                            else { // i j 좌표가 같으면
                                if (k_x != i_x && k_y != i_y) // k가 i와 같지 않으면
                                    total += mon_k; // k를 더한다
                            }
                            // cout << "k: " << k_x << ',' << k_y <<"에 몬스터 " << mon_k << "마리\n";
                            if (total > max_mon) {
                                max_mon = total;
                                // cout << "i, j, k: " << i << ',' << j <<',' << k <<',' <<"에서 몬스터 " << total << "마리로 업데이트 했다!\n";
                                best_dir[0] = i;
                                best_dir[1] = j;
                                best_dir[2] = k;
                                is_updated = true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (!is_updated) {
        for (int i = 0; i < 4; i++) {
            i_x = p.x + px[i];
            i_y = p.y + py[i];
            if (is_inside(i_x, i_y)) {
                for (int j = 0; j < 4; j++) {
                    j_x = i_x + px[j];
                    j_y = i_y + py[j];
                    if (is_inside(j_x, j_y)) {
                        for (int k = 0; k < 4; k++) {
                            k_x = j_x + px[k];
                            k_y = j_y + py[k];
                            if (is_inside(k_x, k_y)) {
                                best_dir[0] = i;
                                best_dir[1] = j;
                                best_dir[2] = k;
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void move_pman(void)
{
    int dir[3];
    
    // cout << "현재 팩맨 위치: " << p.x << ',' << p.y << '\n';
    // cout << "현재 몬스터 마리수: " << mons.size() << '\n';
    
    // 이동할 방향을 정한다
    get_dir(p.x, p.y, dir);
    
    // cout << dir[0] << ' ' << dir[1] << ' ' << dir[2] << '\n';
    
    for (int i = 0; i < 3; i++) { // 방향대로 이동하며
        p.x += px[dir[i]];
        p.y += py[dir[i]];
        int mon_cnt = how_many_monster_here(p.x, p.y);
        // cout << p.x << ',' << p.y << "에 몬스터 " << mon_cnt << "마리\n";
        if (mon_cnt > 0) {
            for (int mon = 0; mon < mons.size(); mon++) {
                mon_info mm = mons.front();
                mons.pop_front();
                if (mm.x == p.x && mm.y == p.y) { // 몬스터가 있으면
                    // cout << "몬스터 잡았다\n";
                    map[p.x][p.y] = turn + 2; // 사라질 턴 수를 맵에 기록하고
//                    mons.pop_front(); // 해당 몬스터 삭제
                    mon--;
                }
                else
                    mons.push_back(mm);
            }
        }
        // cout << "이제 몬스터 " << mons.size() << "마리" << '\n';
    }
}

void dup_mon(void)
{
    for (int i = 0; i < mons.size(); i++) {
        egg_info e;
        e.x = mons[i].x;
        e.y = mons[i].y;
        e.dir = mons[i].dir;
        eggs.push_back(e);
    }
}

void move_mon(int idx)
{
    int new_x, new_y;
    
    for (int i = 0; i < 8; i++) {
        new_x = mons[idx].x + mx[(mons[idx].dir + i) % 8];
        new_y = mons[idx].y + my[(mons[idx].dir + i) % 8];
        if (is_inside(new_x, new_y)) { // 격자 내에 있고
            if (map[new_x][new_y] == 0) { // 시체가 없으며
                if (!(new_x == p.x && new_y == p.y)) { // 팩맨이 없으면
                    // cout << "몬스터 " << mons[idx].x << ',' << mons[idx].y << "에서 " << i <<  "번 돌고 " <<  (mons[idx].dir + i) % 8 << "만큼 이동: " << new_x << ' ' << new_y << '\n';
                    mons[idx].x = new_x; // 이동
                    mons[idx].y = new_y;
                    mons[idx].dir = (mons[idx].dir + i) % 8;
                    return;
                }
            }
        }
    }
}

void remove_deadbody(void)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (map[i][j] != 0 && map[i][j] == turn) {
                // cout << i << ',' << j << "에 죽어있던 몬스터 제거\n";
                map[i][j] = 0;
            }
        }
    }
}

void make_mon(void)
{
    while (eggs.size() != 0) {
        mon_info mon;
        egg_info egg = eggs.front();
        eggs.pop_front();
        mon.x = egg.x;
        mon.y = egg.y;
        mon.dir = egg.dir;
        mons.push_back(mon);
    }
}

int main(void)
{
    cin >> m >> t;
    
    // pman 정보 입력
    cin >> p.x >> p.y;
    p.x -= 1;
    p.y -= 1;
    
    // monster 정보 입력
    for (int i = 0; i < m; i++) {
        mon_info mon;
        cin >> mon.x >> mon.y >> mon.dir;
        mon.x -= 1;
        mon.y -= 1;
        mon.dir -= 1;
        mons.push_back(mon);
    }
    
    vector<int> test;
    test.push_back(3);
    test.push_back(2);
    test.push_back(1);
    
    for (turn = 0; turn < t; turn++) {
        // cout << "\n\nTURN: " << turn << '\n';
        dup_mon();
        for (int i = 0; i < mons.size(); i++)
            move_mon(i);
        move_pman();
        remove_deadbody();
        make_mon();
        // cout << "TURN" << turn << " 끝날 때 최종 몬스터는: "<<  mons.size() << '\n';
    }
    
    cout << mons.size() << '\n';
    
    return 0;
}

/*
5 1
3 1
1 3 5
2 2 7
2 3 7
3 4 6
4 2 2
*/


/*
4 3
3 1
1 3 5
2 2 7
3 4 6
4 2 2
*/

/*
4 5
3 1
1 3 5
2 2 7
3 4 6
4 2 2
*/


/*
1 3
1 1
4 4 3
*/