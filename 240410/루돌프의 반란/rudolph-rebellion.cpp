#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

#define SANTA_TO_DEER 0
#define DEER_TO_SANTA 1

typedef struct Deer_info {
    int x;
    int y;
} deer_info;

typedef struct Santa_info {
    int idx; // 0 ~ P-1
    int status; // 0: 탈락, 1: 생존, 2: 기절
    int x;
    int y;
    int back_turn; // 기절에서 깨어날 턴 수
    int score;
} santa_info;

int N, M, P, C, D; // 격자, 턴 수, 산타 수, 루돌프 힘, 산타 힘
int turn; // 현재 턴을 나타내는 변수 [0, M-1]
int alive_santa; // 살아있는 산타의 수를 저장할 변수
deer_info deer; // 루돌프의 위치를 표시할 변수
vector<santa_info> santa; // 산타들을 저장할 벡터
int dx[] = {-1, 0, 1, 0}; // 상 우 하 좌
int dy[] = {0, 1, 0, -1};

bool comp(santa_info a, santa_info b)
{
    return a.idx < b.idx;
}

int dist(deer_info d, santa_info s) // 루돌프와 산타간의 거리를 리턴
{
    return ((d.x - s.x) * (d.x - s.x) + (d.y - s.y) * (d.y - s.y));
}

bool is_inside(int x, int y)
{
    if (0 <= x && x < N && 0 <= y && y < N)
        return true;
    else
        return false;
}

int get_min_dist_santa(void) // 루돌프에서 제일 거리가 가까운 산타의 idx를 리턴
{
    int min_dist = 5000;
    int min_idx = -1;
    
    for (int i = 0; i < P; i++) {
        if (santa[i].status != 0) {// 탈락한 산타가 아니면
            int i_dist = dist(deer, santa[i]);
            if (i_dist < min_dist || min_idx == -1) { // 거리가 더 가까우면 업데이트
                min_dist = i_dist;
                min_idx = i;
            }
            else if (i_dist == min_dist) { // 거리가 같으면
                if (santa[i].x > santa[min_idx].x) { // r이 더 크면 업데이트
                    min_dist = i_dist;
                    min_idx = i;
                }
                else if (santa[i].x == santa[min_idx].x) { // r이 같으면
                    if (santa[i].y > santa[min_idx].y) { // c가 더 크면 업데이트
                        min_dist = i_dist;
                        min_idx = i;
                    }
                }
            }
        }
    }
    
    return min_idx;
}

void interaction(int idx, int x_dir, int y_dir) // idx의 산타를 x_dir, y_dir 만큼 한 칸 이동시켜준다
{
    // 산타를 한 칸 이동시키고
    santa[idx].x += x_dir;
    santa[idx].y += y_dir;
    
    if (!is_inside(santa[idx].x, santa[idx].y)) {// 격자 밖이면
        santa[idx].status = 0; // 탈락시키고
        alive_santa--; // alive_santa 하나 감소시키기
        return; // 바로 함수 종료
    }
    
    for (int i = 0; i < P; i++) { // 그 자리에 산타가 있는지 확인
        if (i != idx) {// 자기 자신이 아니고
            if (santa[i].status != 0) {// 현재 생존했거나 기절한 (status가 0이 아닌) 산타가
                if (santa[i].x == santa[idx].x && santa[i].y == santa[idx].y) {//idx의 위치와 같다면
                    interaction(i, x_dir, y_dir); // 재귀 호출
                }
            }
        }
    }
}

void collide(int mover, int santa_idx, int x_dir, int y_dir)
// mover: 0이면 산타가 이동, 1이면 루돌프가 이동
// x_dir, y_dir은 dx, dy값
{
    if (mover == 0) { // 산타가 부딪힌거면
        x_dir *= (-1); // 방향을 반대로 바꿔주고
        y_dir *= (-1);
        // cout << "바뀐 방향은 : " << x_dir << ',' << y_dir << '\n';
        santa[santa_idx].x += D * x_dir; // 산타 이동
        santa[santa_idx].y += D * y_dir;
        // cout << santa_idx + 1 << "은 " << santa[santa_idx].x << ',' << santa[santa_idx].y << " 로 이동했다\n";
        santa[santa_idx].score += D; // 점수 증가
    }
    else { // 루돌프가 부딪힌거면
        // 방향을 바꾸지 않고
        santa[santa_idx].x += C * x_dir; // 산타 이동
        santa[santa_idx].y += C * y_dir;
        santa[santa_idx].score += C; // 점수 증가
    }
    
    if (!is_inside(santa[santa_idx].x, santa[santa_idx].y)) { // 그 자리가 격자 밖이라면
        // cout << "충돌한 " << santa_idx + 1 << "번 산타가 격자 밖이라 탈락했다\n";
        santa[santa_idx].status = 0; // 탈락시키기
        alive_santa--; // alive_santa의 수를 감소시키고
        return; // 바로 함수 종료
    }
    
    // 산타가 격자 내에 있으면
    santa[santa_idx].status = 2; // 산타를 기절시키고
    santa[santa_idx].back_turn = turn + 2; // 돌아올 턴 수를 (현재 턴 + 2)로 저장해주기
    
    for (int i = 0; i < P; i++) { // 상호작용할 산타가 있는지 확인
        if (i != santa_idx) { // 자기 자신이 아니고
            if (santa[i].status != 0) { // 탈락하지 않은 산타라면 (생존했거나 기절)
                if (santa[i].x == santa[santa_idx].x && santa[i].y == santa[santa_idx].y) {
                    // cout << i + 1 << "번 산타가 그 자리에 있어서 상호작용이 일어났다\n";
                    interaction(i, x_dir, y_dir); // 상호작용 시켜주고
                    return; // 함수 종료 (연쇄작용은 interaction에서 구현할 것)
                }
            }
        }
    }
}

void move_deer(void)
{
    int santa_idx = get_min_dist_santa(); // 제일 가까운 산타의 인덱스를 구한다
    // cout << "루돌프에게 가장 가까운 산타는 " << santa_idx + 1 << "번 산타이고, " << santa[santa_idx].x << ',' << santa[santa_idx].y << " 에 있다\n";
    int dx, dy;
    
    // 이동할 dx, dy를 구한다
    if (santa[santa_idx].x > deer.x)
        dx = 1;
    else if (santa[santa_idx].x < deer.x)
        dx = -1;
    else
        dx = 0;
    
    if (santa[santa_idx].y > deer.y)
        dy = 1;
    else if (santa[santa_idx].y < deer.y)
        dy = -1;
    else
        dy = 0;
    
    // 루돌프를 이동한다
    deer.x += dx;
    deer.y += dy;
    // cout << "루돌프는 " << deer.x << ',' << deer.y << " 로 이동했다\n";
    
    // 이동한 곳에 산타가 있다면 충돌처리를 한다
    if (deer.x == santa[santa_idx].x && deer.y == santa[santa_idx].y) {
        // cout << "루돌프와 " << santa_idx + 1 << "번 산타가 충돌했다\n";
        collide(DEER_TO_SANTA, santa_idx, dx, dy);
    }
}

void move_santa(int s)
{
    int min_dist = dist(deer, santa[s]);
    int min_idx = -1;
    
    // cout << s + 1 << "번 산타를 움직이려 한다\n";
    int new_x, new_y;
    for (int i = 0; i < 4; i++) { // 네 방향을 탐색하며
        new_x = santa[s].x + dx[i]; // new_x, new_y 좌표 설정
        new_y = santa[s].y + dy[i];
        if (is_inside(new_x, new_y)) { // 격자를 벗어나지 않고
            bool is_santa = false;
            for (int j = 0; j < P; j++) { // 그 자리에 다른 산타가 없는지 확인
                if (j != s) { // 본인이 아니고
                    if (santa[j].x == new_x && santa[j].y == new_y) { // 이동한 자리에 산타가 있으면
                        is_santa = true; // true로 만들고
                        j = P + 1; // 반복문 종료
                    }
                }
            }
            if (is_santa == false) { // 다른 산타가 없으며
                santa_info new_santa;
                new_santa.x = new_x;
                new_santa.y = new_y;
                if (dist(deer, new_santa) < min_dist) { // 루돌프에게 가까워지면
                    min_dist = dist(deer, new_santa); // min_dist, min_idx 업데이트
                    min_idx = i;
                }
            }
        }
    }
    
    
    if (min_idx != -1) { // min_idx가 -1이 아니면
        santa[s].x += dx[min_idx]; // 산타 이동
        santa[s].y += dy[min_idx];
        // cout << "산타는 " << santa[s].x << ',' << santa[s].y << " 로 이동했다\n";
        if (santa[s].x == deer.x && santa[s].y == deer.y) { // 이동한 곳이 루돌프의 위치면
            // cout << "그곳에 루돌프가 있어서 충돌했다\n";
            // cout << "dx: " << dx[min_idx] << ", dy: " << dy[min_idx] << '\n';
            collide(SANTA_TO_DEER, s, dx[min_idx], dy[min_idx]); // collide
        }
    }
    
    // else
    //     cout << "산타를 이동시키지 못했다\n";
}

void plus_score(void)
{
    for (int i = 0; i < P; i++)
        if (santa[i].status != 0)
            santa[i].score++;
}

int main(void)
{
    cin >> N >> M >> P >> C >> D;
    
    // 루돌프 정보 입력
    cin >> deer.x >> deer.y;
    deer.x -= 1;
    deer.y -= 1;
    
    // 산타 정보 입력
    for (int i = 0; i < P; i++) {
        santa_info s;
        cin >> s.idx >> s.x >> s.y;
        s.idx -= 1;
        s.x -= 1;
        s.y -= 1;
        s.status = 1; // 초기 상태는 모두 1(생존)
        s.back_turn = 0;
        s.score = 0;
        santa.push_back(s);
    }
    
    // 산타 벡터를 번호순 정렬
    sort(santa.begin(), santa.end(), comp);
    
//    for (int i = 0; i < P; i++)
//        cout << santa[i].idx << ' ' ;
    
    alive_santa = P; // 현재 살아있는 산타는 P 명
    
    // m 턴 동안
    for (turn = 0; turn < M; turn++) {
        // cout << "\n\nTURN: " << turn + 1 << '\n';
        if (alive_santa == 0) // 산타가 모두 탈락했으면 종료
            break;
        for (int s = 0; s < P; s++) {// 기절한(status 2) 산타가 현재 턴에 살아 돌아오면 status 바꾸기
            if (santa[s].status == 2)
                if (santa[s].back_turn == turn)
                    santa[s].status = 1;
        }
        move_deer(); // deer 이동시키기
        if (alive_santa == 0) // 산타가 모두 탈락했으면 종료
            break;
        for (int s = 0; s < P; s++) {// 모든 산타를 돌며
            if (santa[s].status == 1) { // status == 1 이면
                move_santa(s); // 산타 이동시키기
            }
        }
        plus_score(); // 살아남은 산타들에게 점수 주기
    }
    
    // 산타들이 얻은 점수 출력
    for (int s = 0; s < P; s++) {
        cout << santa[s].score << ' ';
    }
    
    return 0;
}