#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef struct {
    int x;
    int y;
} point;

int n, m, p, c, d;
point deer;
vector<point> santa_pos(30);
vector<int> santa_score;
vector<int> santa_status; // 0: 정상, 1: 기절, 2: 탈락
vector<int> santa_cooltime; // 기절한 산타가 일어나는 턴을 저장
int santa_cnt;
int d_x[8] = {0, 0, 1, 1, 1, -1, -1, -1};
int d_y[8] = {1, -1, 0, 1, -1, 0, 1, -1};
int s_x[4] = {-1, 0, 1, 0};
int s_y[4] = {0, 1, 0, -1};

void print_map(void)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            bool flag = false;
            if (deer.x == i && deer.y == j) {
                cout << 'd' << ' ';
                flag = true;
            }
            else {
                for (int s = 0; s < p; s++) {
                    if (santa_pos[s].x == i && santa_pos[s].y == j) {
                        cout << s + 1 << ' ';
                        flag = true;
                    }
                }
            }
            if (!flag)
                cout << 0 << ' ';
        }
        cout << '\n';
    }
}

void refresh_santa(int turn)
{
    for (int i = 0; i < p; i++) {
        if (santa_cooltime[i] == turn && santa_status[i] == 1)
            santa_status[i] = 0;
    }
}

int dist(point a, point b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

bool is_inside(point a)
{
    if (0 <= a.x && a.x < n && 0 <= a.y && a.y < n)
        return true;
    else
        return false;
}

void interaction(int idx, int who, int direction)
// who: 1이면 deer이 이동, 2이면 santa가 이동
// direction: 이동한 사람이 온 방향의 index (d_x, s_x의 index 값)
// who가: 1이면 direcion 그대로, 2이면 direction 반대로
{
    if (who == 1) { // deer이 이동했으면
        santa_pos[idx].x += d_x[direction];
        santa_pos[idx].y += d_y[direction];
    }
    
    else { // santa가 이동했으면
        santa_pos[idx].x -= s_x[direction];
        santa_pos[idx].y -= s_y[direction];
    }
    
    if (!is_inside(santa_pos[idx])) {
        santa_status[idx] = 2;
        santa_cnt--;
    }
    
    for (int i = 0; i < p; i++) {
        if (santa_pos[i].x == santa_pos[idx].x && santa_pos[i].y == santa_pos[idx].y && i != idx)
            interaction(i, who, direction);
    }
}

void collision(int idx, int who, int direction, int turn)
// who: 1이면 deer이 이동, 2이면 santa가 이동
// direction: 이동한 사람이 온 방향의 index (d_x, s_x의 index 값)
{
    if (who == 1) { // deer이 이동했을 때
        // 산타 점수 += c
        // cout << "DEER_MOVE_SO_COLLIDE" << '\n';
        // cout << "SANTA:" << idx + 1 << '\n';
        santa_score[idx] += c;
        // deer가 이동한 방향만큼 c 칸 밀려남
        santa_pos[idx].x += d_x[direction] * c;
        santa_pos[idx].y += d_y[direction] * c;
        // cout << santa_pos[idx].x << ' ' << santa_pos[idx].y << '\n';
    }
    
    else { // santa가 이동했을 때
        // 산타 점수 += d
        // cout << "SANTA_MOVE_SO_COLLIDE" << '\n';
        // cout << "SANTA" << idx + 1 << ": " << santa_pos[idx].x << ' ' << santa_pos[idx].y << '\n';
        santa_score[idx] += d;
        // santa가 이동한 방향의 반대로 d칸 밀려남
        santa_pos[idx].x -= s_x[direction] * d;
        santa_pos[idx].y -= s_y[direction] * d;
        // cout << santa_pos[idx].x << ' ' << santa_pos[idx].y << '\n';
    }
    
    // map 바깥이면 탈락
    if (!is_inside(santa_pos[idx])) {
        santa_status[idx] = 2;
        santa_cnt--;
    }
    else {
        // cout << "기절" << '\n';
        santa_status[idx] = 1;
        santa_cooltime[idx] = turn + 2;
    }
    // 해당 자리에 산타가 있으면 밀기
    for (int i = 0; i < p; i++) {
        if (santa_pos[i].x == santa_pos[idx].x && santa_pos[i].y == santa_pos[idx].y && i != idx) {
            // cout << "INTERACTION WITH " << i + 1 << " AND " << idx + 1 << '\n';
            interaction(i, who, direction); // 밀릴사람, 이동한 것, 이동해 온 방향
        }
    }
}

int select_santa(void)
{
    point min_pos = {-1, -1};
    int santa_num = 0;
    
    for (int i = 0; i < santa_status.size(); i++) {
        if (santa_status[i] == 2)
            continue;
        if (min_pos.x == -1 && min_pos.y == -1) {
            min_pos = santa_pos[i];
            santa_num = i;
        }
        if (dist(deer, santa_pos[i]) < dist(deer, min_pos)) {
            min_pos = santa_pos[i];
            santa_num = i;
        }
        else if (dist(deer, santa_pos[i]) == dist(deer, min_pos)) {
            if (min_pos.x < santa_pos[i].x) {
                min_pos = santa_pos[i];
                santa_num = i;
            }
            else if (min_pos.x == santa_pos[i].x) {
                if (min_pos.y < santa_pos[i].y) {
                    min_pos = santa_pos[i];
                    santa_num = i;
                }
            }
        }
    }
    
    return santa_num;
}

int select_direction(point target)
{
    int min = 0;
    point min_pos = deer;
    point next;
    
    for (int i = 0; i < 8; i++) {
        next.x = deer.x + d_x[i];
        next.y = deer.y + d_y[i];
        if (is_inside(next)) {
            if (dist(target, next) < dist(target, min_pos)) {
                min = i;
                min_pos = next;
            }
        }
    }
    
    return min;
}

void move_deer(int turn)
{
    int santa_idx = select_santa();
    point target_santa = santa_pos[santa_idx];
    int dir = select_direction(target_santa);
    
    deer.x += d_x[dir];
    deer.y += d_y[dir];
    
    if (target_santa.x == deer.x && target_santa.y == deer.y) {
        santa_status[santa_idx] = 1;
        collision(santa_idx, 1, dir, turn);
    }
    
    return;
}

void move_santa(int idx, int turn)
{
    point next = {santa_pos[idx].x, santa_pos[idx].y};
    point min = {santa_pos[idx].x, santa_pos[idx].y};
    int min_i = 0;
    
    for (int i = 0; i < 4; i++) {
        bool flag = false;
        next.x = santa_pos[idx].x + s_x[i];
        next.y = santa_pos[idx].y + s_y[i];
        if (!is_inside(next))
            continue;
        for (int s = 0; s < p; s++) {
            if (s != idx) {
                if (next.x == santa_pos[s].x && next.y == santa_pos[s].y) {
                    flag = true;
                }
            }
        }
        if (flag == true)
            continue;
        if (dist(deer, next) < dist(deer, min)) { // 거리가 제일 작을때 옮기기
            // 제일 작은 거리가 되는 i를 구해서 그때 옮기기
            min.x = santa_pos[idx].x + s_x[i];
            min.y = santa_pos[idx].y + s_y[i];
            min_i = i;
        }
    }
    
    if (!(min.x == santa_pos[idx].x && min.y == santa_pos[idx].y)) {
        santa_pos[idx] = min;
        // cout << santa_pos[idx].x << ' '  << santa_pos[idx].y << '\n';
        if (santa_pos[idx].x == deer.x && santa_pos[idx].y == deer.y)
            collision(idx, 2, min_i, turn);
    }
}

void santa_reward(void)
{
    for (int i = 0; i < p; i++)
        if (santa_status[i] != 2)
            santa_score[i]++;
}

int main(void)
{
    cin >> n >> m >> p >> c >> d;
    
    cin >> deer.x >> deer.y;
    deer.x -= 1;
    deer.y -= 1;
    
    santa_cnt = p;
    
    for (int i = 0; i < p; i++) {
        int s;
        point p;
        cin >> s >> p.x >> p.y;
        s -= 1;
        p.x -= 1;
        p.y -= 1;
        santa_pos[s] = p;
        santa_score.push_back(0);
        santa_status.push_back(0);
        santa_cooltime.push_back(0);
    }
    
    // print_map();
    
    for (int i = 0; i < m; i++) {
        // cout << "**TURN:" << i + 1 << '\n';
        refresh_santa(i);
        if (santa_cnt == 0)
            break;
        move_deer(i);
        if (santa_cnt == 0)
            break;
        for (int s = 0; s < santa_status.size(); s++) {
            if (santa_status[s] == 0)
                move_santa(s, i);
        }
        if (santa_cnt == 0)
            break;
        santa_reward();
        // print_map();
        // cout << "SCORE" << '\n';
    }
    
    for (int i = 0; i < p; i++) {
        cout << santa_score[i] << ' ';
    }
    cout << '\n';
    
    return 0;
}