#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

typedef struct Knight_info {
    bool is_alive; // 살아있는지 표시하는 변수
    int idx; // 기사의 인덱스. 0부터 시작.
    int x;
    int y;
    int h;
    int w;
    int first_hp; // 초기 체력 저장. 변하지 않음
    int now_hp; // 현재 체력 저장. 변함
} knight_info;

int L, N, Q;
int map[40][40] = {0, }; // 0: 빈칸, 1: 함정, 2: 벽
vector<knight_info> knight; // 기사 정보 저장하는 벡터

int dx[] = {-1, 0, 1, 0}; // 상 우 하 좌
int dy[] = {0, 1, 0, -1};

bool is_inside(knight_info k)
{
    if (0 <= k.x && k.x + k.h <= L && 0 <= k.y && k.y + k.w <= L)
        return true;
    else
        return false;
}

bool is_collide(knight_info a, knight_info b)
{
    int visited[40][40] = {0, };
    
    // 일단 a 기사의 범위를 색칠한다
    for (int i = a.x; i < a.x + a.h; i++) {
        for (int j = a.y; j < a.y + a.w; j++) {
            visited[i][j] = 1;
        }
    }
    
    // b 기사의 범위를 확인하며 색칠되어있으면 true;
    for (int i = b.x; i < b.x + b.h; i++) {
        for (int j = b.y; j < b.y + b.w; j++) {
            if (visited[i][j] == 1)
                return true;
        }
    }
    
    return false; // 위에서 한번도 걸리지 않았다면 충돌하지 않았음(false)를 출력
}

bool is_wall_there(knight_info k)
{
    for (int i = k.x; i < k.x + k.h; i++) {
        for (int j = k.y; j < k.y + k.w; j++) {
            if (map[i][j] == 2)
                return true; // 벽이 있음
        }
    }
    
    return false; // 위에서 한번도 안걸렸으면 벽이 없음
}

bool is_pushable(int idx, int dir, deque<int>& pushed) // 기사의 idx, 밀 방향 dir, 밀린 기사를 저장할 pushed 덱
{
    bool pushable = true;
    
    knight_info tmp = knight[idx]; // 임시로 이동시킬 기사 tmp를 선언
    
    tmp.x += dx[dir]; // tmp를 dir로 이동시킴
    tmp.y += dy[dir];
    
    if (!is_inside(tmp)) { // 격자 밖으로 이동하면 false
//        cout << "민 " << idx << "기사가 격자 밖이라 안됨\n";
        return false;
    }
    
    if (is_wall_there(tmp)) { // 이동한 위치에 벽이 있으면 false // 이거 아래도 추가해줘야함
//        cout << "민 " << idx << "기사 위치에 벽이 있어서 안됨\n";
        return false;
    }
    
    // 밀었을 때 충돌하는 기사가 있는지 확인
    deque<knight_info> collide;
    for (int i = 0; i < N; i++) { // 기사 수 만큼
        if (knight[i].is_alive == true) { // 기사가 살아있고
            if (i != idx) { // i가 idx가 아니고
                if (is_collide(tmp, knight[i])) { // tmp와 그 기사가 충돌하면
                    collide.push_back(knight[i]); // dq에 넣기
                    pushed.push_back(i); // pushed에 push_back
                }
            }
        }
    }
    
    // 충돌하는 기사 연쇄작용 확인하고 연쇄 끝날때까지 반복
    while (collide.size() != 0) {
        knight_info check = collide.front(); // check할 knight_info 변수
        check.x += dx[dir]; // 이동 방향만큼 한 칸 밀어준다
        check.y += dy[dir];
        collide.pop_front();
        if (!is_inside(check)) { // 만약 이동한 check가 격자 밖이라면
//            cout << "밀린 " << check.idx << "번 기사가 격자 밖이라 안됨\n";
            return false; // return false
        }
        if (is_wall_there(check)) { // 만약 이동한 check에 벽이 있다면
//            cout << "밀린 " << check.idx << "번 기사에 벽이 있어서 안됨\n";
            return false; // return false
        }
        for (int i = 0; i < N; i++) {// 기사들을 다 돌면서
            if (knight[i].is_alive == true) {// 기사가 살아있고
                if (i != idx) { // i가 idx가 아니고
                    if (i != check.idx) { // i가 check의 인덱스가 아니고
                        if (is_collide(check, knight[i])) { // check와 i가 충돌했으면
                            collide.push_back(knight[i]); // collide에 push_back
                            pushed.push_back(i); // pushed에 push_back
                        }
                    }
                }
            }
        }
    }
    
    return pushable;
}

void push_knight(int idx, int dir, deque<int> pushed) // pushed 변경하면 안됨. damage에서 또 써야함
{
    // 일단 본인을 밀고
    knight[idx].x += dx[dir];
    knight[idx].y += dy[dir];
//    cout << "본인인 " << idx << "번 기사를 " << knight[idx].x << ',' << knight[idx].y << "로 밀었다.\n";

    // pushed에 있는 knight들을 모두 dir 방향으로 한 칸씩 밀어준다
    for (int i = 0; i < pushed.size(); i++) {
        knight[pushed[i]].x += dx[dir];
        knight[pushed[i]].y += dy[dir];
//        cout << "연쇄로 밀린 " << pushed[i] << "번 기사를 " << knight[pushed[i]].x << ',' << knight[pushed[i]].y << "로 밀었다\n";
    }
}

int how_many_trap(int idx)
{
    int trap = 0;
    
    for (int i = knight[idx].x; i < knight[idx].x + knight[idx].h; i++) {
        for (int j = knight[idx].y; j < knight[idx].y + knight[idx].w; j++)
            if (map[i][j] == 1)
                trap++;
    }
    
    return trap;
}

void damage(deque<int> pushed)
{
    for (int i = 0; i < pushed.size(); i++) { // 밀린 기사들 중
        int trap = how_many_trap(pushed[i]); // 밀린 기사의 영역에 몇개의 함정이 있는지 확인하고
        knight[pushed[i]].now_hp -= trap; // 그 기사의 현재 체력에서 함정 개수를 뺀다
        if (knight[pushed[i]].now_hp <= 0) // 현재 체력이 0 이하면
            knight[pushed[i]].is_alive = false; // 그 기사를 죽인다
    }
}

int main(void)
{
    cin >> L >> N >> Q;
    
    // 맵 정보 저장
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }
    
    // 기사들 정보 입력
    for (int i = 0; i < N; i++) {
        knight_info k;
        cin >> k.x >> k.y >> k.h >> k.w >> k.first_hp;
        k.x -= 1;
        k.y -= 1;
        k.idx = i;
        k.is_alive = true;
        k.now_hp = k.first_hp;
        knight.push_back(k);
    }
    
    // 명령 Q개 입력
    for (int i = 0; i < Q; i++) {
        int idx, dir;
        cin >> idx >> dir;
        idx -= 1;
        if (knight[idx].is_alive == true) { // 살아있는 기사이면
            deque<int> pushed;
            if (is_pushable(idx, dir, pushed)) { // 밀 수 있으면
                push_knight(idx, dir, pushed); // 밀기
                damage(pushed); // 밀린 기사들에게 데미지 주기
            }
        }
    }
    
    // 살아있는 기사들의 damage의 합 구하기
    int answer = 0;
    for (int i = 0; i < N; i++) {
        if (knight[i].is_alive == true) { // 현재 살아있는 기사라면
            answer += (knight[i].first_hp - knight[i].now_hp); // answer에 first_hp 에서 now_hp를 뺀 값을 더해주기
        }
    }
    
    cout << answer << '\n';
    
//    cout << "위에 무시 \n";
//    deque<int> pushed;
//    cout << "밀 수 있나? : " << (is_pushable(1, 1, pushed) == true ? "YES" : "NO") << " 밀린 기사는 몇명? : ";
//    cout << pushed.size() << '\n';
//    for (int i = 0; i < pushed.size(); i++)
//        cout << pushed[i] << '\n';
//    is_pushable(1, 1, pushed);
//    push_knight(1, 1, pushed);
    
    return 0;
}