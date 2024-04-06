#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

typedef struct Knight_info {
    int index;
    bool alive = true;
    int x;
    int y;
    int h;
    int w;
    int original_hp; // 초기 체력
    int now_hp; // 현재 체력
} knight_info;

int l, n, q;
int map[40][40]; // 0: 빈칸, 1: 함정, 2: 벽
vector<knight_info> knight;
int dx[4] = {-1, 0, 1, 0}; // 상 우 하 좌
int dy[4] = {0, 1, 0, -1};

void print_map(void)
{
    cout << "MAP\n";
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < l; j++) {
            cout << map[i][j] << ' ';
        }
        cout << '\n';
    }
    
    return;
}

bool is_inside(knight_info k)
{
    if (0 <= k.x && k.x + k.h <= l && 0 <= k.y && k.y + k.w <= l)
        return true;
    else
        return false;
}

bool is_wall_exist(knight_info k)
{
    for (int i = k.x; i < k.x + k.h; i++) {
        for (int j = k.y; j < k.y + k.w; j++) {
            if (map[i][j] == 2) {
//                cout << i << ", " << j << " 에 벽이 있음\n";
                return true;
            }
        }
    }
    
    return false;
}

bool is_collide(knight_info a, knight_info b)
{
    // 여기 이상할수도 있음
    if (b.x < a.x + a.h && a.x + a.h <= b.x + b.h && b.y < a.y + a.w && a.y + a.w <= b.y + b.w) {
//        cout << a.index + 1 << "와 " << b.index + 1 << " 충돌\n";
        return true;
    }
    if (a.x < b.x + b.h && b.x + b.h <= a.x + a.h && a.y < b.y + b.w && b.y + b.w <= a.y + a.w)
        return true;
    
    return false;

    /*
    int visited[40][40] = {0, };
    
    for (int i = a.x; i < a.x + a.h; i++) {
        for (int j = a.y; j < a.y + a.w; j++) {
            visited[i][j] = 1;
        }
    }
    
    for (int i = b.x; i < b.x + b.h; i++) {
        for (int j = b.y; j < b.y + b.w; j++) {
            if (visited[i][j] == 1)
                return true;
        }
    }
    
    return false;
     */
}

bool is_pushable(int k, int dir)
{
    bool answer = true;
    
    knight_info next = knight[k];
    next.x += dx[dir];
    next.y += dy[dir];
    
    if (!is_inside(next)) { // 기사가 맵을 벗어나려고 하면
//        cout << "맵 벗어나서 안됨\n";
        return false;
    }
    
    if (is_wall_exist(next)) { // 기사가 이동하려는 곳에 벽이 있으면
//        cout << "벽 있어서 안됨\n";
        return false;
    }
    
    // 기사가 이동하려는 곳에 기사가 있는지 확인
    deque<knight_info> dq;
    
    for (int i = 0; i < knight.size(); i++) {
        if (i == k) // 본인이면 continue
            continue;
        if (knight[i].alive == false) // 탈락한 기사면 continue
            continue;
        if (is_collide(next, knight[i])) // 겹치면 넣기
            dq.push_back(knight[i]);
    }
    
//    cout << "덱에 들어간 기사들?\n";
//    for (int i = 0; i < dq.size(); i++)
//        cout << dq[i].index << ' ';
//    cout << '\n';

    // 기사가 있다면
    if (dq.size() != 0) {
        while (dq.size() != 0) { // dq가 빌 때까지
            next = dq.front();
            dq.pop_front();
            next.x += dx[dir];
            next.y += dy[dir];
//            if (!is_inside(next)) // 맵을 벗어나거나
//                return false;
            if (is_wall_exist(next)) // 벽이 존재하면 false를 return
                return false;
//            if (dq.front().index == k)
//                continue; 여기 뭔가 수상함;;;;;;;;;;
            for (int i = 0; i < knight.size(); i++) { // 이동 가능하면 dq에 push
                if (i == next.index)
                    continue;
                if (knight[i].alive == false)
                    continue;
                if (!is_collide(next, knight[i]))
                    continue;
                dq.push_back(knight[i]);
            }
        }
    }
    
    // 기사가 없다면
    else
        answer = true;
    
    return answer;
}

void push_knights(int k, int dir, vector<int>& pushed)
{
    // 기사 k의 x, y값을 dir로 한 칸 업데이트
    knight[k].x += dx[dir];
    knight[k].y += dy[dir];
    
    // 만약 다른 기사와 충돌하면
    deque<knight_info> dq;
    for (int i = 0; i < knight.size(); i++) {
        if (i == k)
            continue;
        if (knight[i].alive == false)
            continue;
        if (is_collide(knight[k], knight[i])) // 밀 기사를 덱에 푸시
            dq.push_back(knight[i]);
    }
    
//    cout << "덱에 들어간 기사들?\n";
//    for (int i = 0; i < dq.size(); i++)
//        cout << dq[i].index + 1 << ' ';
//    cout << '\n';
    
    while (dq.size() != 0) {// 밀 기사가 있으면 밀기
        int index = dq.front().index;
        dq.pop_front();
        if (index != k)
            pushed.push_back(index);
        knight[index].x += dx[dir];
        knight[index].y += dy[dir];
        if (!is_inside(knight[index])) {
//            cout << index + 1 << " 기사 밀려 죽음\n";
            knight[index].alive = false;
        }
        for (int i = 0; i < knight.size(); i++) { // 이동 가능하면 dq에 push
            if (i == index)
                continue;
            if (i == k)
                continue;
            if (knight[i].alive == false)
                continue;
            if (!is_collide(knight[index], knight[i]))
                continue;
            dq.push_back(knight[i]);
        }
    } // 없으면 끝
}

int how_many_trap(int k)
{
    int answer = 0;
    
    for (int i = knight[k].x; i < knight[k].x + knight[k].h; i++) {
        for (int j = knight[k].y; j < knight[k].y + knight[k].w; j++) {
            if (map[i][j] == 1)
                answer++;
        }
    }
    
    return answer;
}

void calculate_damage(vector<int> pushed)
{
//    cout << "DAMAGE\n";
    for (int i = 0; i < pushed.size(); i++) {// 밀려진 기사들 중
        int damage = how_many_trap(pushed[i]); // 함정이 몇개 포함됐는지 세기
//        cout << pushed[i] + 1 << "번 기사, damage " << damage << '\n';
        knight[pushed[i]].now_hp -= damage;
//        cout << knight[pushed[i]].now_hp << '\n';
        if (knight[pushed[i]].now_hp <= 0) { // 만약 (now_hp - 함정 개수) <= 0이면
//            cout << i + 1 << "번 기사 함정에 죽음\n";
            knight[pushed[i]].alive = false; // 비활성화하기
        }
    }
}

int main(void)
{
    cin >> l >> n >> q;
    
    // map 정보 저장
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < l; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
//    print_map();
    
    // 기사 정보 저장
    // (x,y,h,w,k) 순, x, y는 1씩 빼서 맵 좌상단이 (0, 0)이 되게 함
    for (int i = 0; i < n; i++) {
        knight_info k;
        cin >> k.x >> k.y >> k.h >> k.w >> k.original_hp;
        k.index = i;
        k.x -= 1;
        k.y -= 1;
        k.alive = true;
        k.now_hp = k.original_hp;
        knight.push_back(k);
    }
    
    // 명령 수행
    for (int i = 0; i < q; i++) {
//        cout << i + 1 << "번 명령\n";
        int k_num, dir;
        cin >> k_num >> dir;
        k_num -= 1;
        if (!knight[k_num].alive)
            continue;
        if (is_pushable(k_num, dir)) {// 미는게 가능하면
//            cout << "밀었다\n";
            vector<int> pushed; // 밀린 기사의 인덱스를 저장하는 벡터
            push_knights(k_num, dir, pushed); // 밀기
            calculate_damage(pushed); // 함정이 있는 것은 데미지 입히기
        }
//        cout << "기사 상태\n";
//        for (int u = 0; u < n; u++) {
//            cout << u + 1 << "번 기사: ";
//            if (knight[u].alive) {
//                cout << "살아있고, 체력은 " << knight[u].now_hp << '\n';
//            }
//            else
//                cout << "죽었음\n";
//        }
    }
    
    // 살아남은 기사가 받은 데미지 더하기
    int answer = 0;
    for (int i = 0; i < n; i++) {
        if (knight[i].alive) {
//            cout << "살아남은 " << i + 1 << "기사, " << (knight[i].original_hp - knight[i].now_hp) << '\n';
            answer += (knight[i].original_hp - knight[i].now_hp);
        }
    }
    
    // 살아남은 기사가 받은 데미지의 합 출력
    cout << answer << '\n';
    
    return 0;
}