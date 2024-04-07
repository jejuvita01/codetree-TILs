#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

typedef struct Section_info {
    int index; // [1, ] 섹션의 인덱스
    int cnt; // 칸 수
    int color; // 섹션의 색
} section_info;

int n;
int map[29][29];
int visited[29][29] = {0, };
long long answer = 0;
vector<section_info> sections;
int dx[] = {0, 0, 1, -1};
int dy[] = {1, -1, 0, 0};

void print_map(int m[29][29])
{
    cout << '\n';
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << m[i][j] << "  ";
            if (m[i][j] < 10)
                cout << ' ';
        }
        cout << '\n';
    }
}

void cross(void)
{
    int new_map[29][29];
    
    for (int i = 0; i < n / 2; i++) {
        new_map[n / 2][i] = map[i][n / 2];
        new_map[n / 2][n - i - 1] = map[n - i - 1][n / 2];
        new_map[i][n / 2] = map[n / 2][n - i - 1];
        new_map[n - i - 1][n / 2] = map[n / 2][i];
    }
    
    for (int i = 0; i < n / 2; i++) {
        map[n / 2][i] = new_map[n / 2][i];
        map[n / 2][n - i - 1] = new_map[n / 2][n - i - 1];
        map[i][n / 2] = new_map[i][n / 2];
        map[n - i - 1][n / 2] = new_map[n - i - 1][n / 2];
    }
}

void square(int x_start, int y_start)
{
    int new_map[29][29] = {0, };
    
    /*
    for (int j = y_start; j < y_end; j++)
        for (int i = x_start; i < x_end; i++)
            new_map[i][j] = map[y_end - j - 1][i];
     */
    for (int j = 0; j < n / 2; j++) {
        for (int i = 0; i < n / 2; i++) {
            new_map[i][(n / 2) - 1 - j] = map[x_start + j][y_start + i];
        }
    }
    
//    cout << "NEW_MAP" << '\n';
//    for (int i = 0; i < n; i++) {
//        for (int j = 0; j < n; j++) {
//            cout << new_map[i][j] << "  ";
//            if (new_map[i][j] < 10)
//                cout << ' ';
//        }
//        cout << '\n';
//    }
    
    for (int i = 0; i < n / 2; i++) {
        for (int j = 0; j < n / 2; j++)
            map[x_start + i][y_start + j] = new_map[i][j];
    }
}

void four_square(void)
{
    // 1 // x: [0, n/2), y: [0, n/2)
    // 2 // x: [0, n/2), y: [n/2 + 1, n)
    // 3 // x: [n/2 + 1, n), y: [0, n/2)
    // 4 // x: [n/2 + 1, n), y: [n/2 + 1, n)
    
    square(0, 0);
    square(0, n/2 + 1);
    square(n/2 + 1, 0);
    square(n/2 + 1, n/2 + 1);
}

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

void dfs(int x, int y, int color, int section, int& cnt) {
    if (visited[x][y] != 0) return;
    
    visited[x][y] = section;
    
    for (int i = 0; i < 4; i++) {
        int new_x = x + dx[i];
        int new_y = y + dy[i];
        if (is_inside(new_x, new_y)) {
            if (map[new_x][new_y] == color && visited[new_x][new_y] == 0) {
                cnt += 1;
                dfs(new_x, new_y, color, section, cnt);
            }
        }
    }
}

int is_near(section_info& a, section_info& b) // 두 섹션이 인접했는지, 인접했으면 인접한 변 수를 리턴
// 인접하지 않았으면 -1 리턴
{
    // cout << "달라짐" << '\n';
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (visited[i][j] == a.index) {
                // cout << "여기\n";
                for (int k = 0; k < 4; k++) {
                    int next_x = i + dx[k];
                    int next_y = j + dy[k];
                    if (is_inside(next_x, next_y)) {
                        // cout << "저기\n";
                        if (visited[next_x][next_y] == b.index) {
                            cnt++;
                        }
                    }
                }
            }
        }
    }
    
    if (cnt == 0)
        return -1;
    else
        return cnt;
}

long long get_two_score(section_info a, section_info b, int near_line) // 두 섹션간의 점수를 리턴
{
    long long answer = 1;
    
    // 각자의 색, 각자의 칸 수를 일단 곱한다
    answer *= (a.color * b.color) * (a.cnt + b.cnt);
    
    // 맞닿은 변의 개수를 곱한다
    answer *= near_line;
    
    return answer;
}

long long get_score(void)
{
    long long score = 0;
    int section = 0;
    
    // dfs로 visited에 section 저장하기
    init_visited();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (visited[i][j] == 0) {
                section_info s;
                int cnt = 1;
                section++;
                dfs(i, j, map[i][j], section, cnt);
                s.index = section;
                s.color = map[i][j];
                s.cnt = cnt;
                sections.push_back(s);
            }
        }
    }
    
//     print_map(visited);
    
    for (int i = 0; i < section; i++) {
        for (int j = i + 1; j < section; j++) {
            int near_line = is_near(sections[i], sections[j]);
            if (near_line != -1) {
//                cout << "SECTION " << i << "와 SECTION " << j << "가 겹침\n";
//                cout << "변이 몇개냐면: " << near_line << '\n';
                score += get_two_score(sections[i], sections[j], near_line);
            }
        }
    }

    return score;
}

int main(void)
{
    cin >> n;
    
    
    // 맵 정보 입력 받기
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            cin >> x;
            map[i][j] = x;
        }
    }

    answer += get_score();
    for (int i = 1; i <= 3; i++) {
        sections.clear();
        cross();
        four_square();
        answer += get_score();
    }

    cout << answer << '\n';
    
    return 0;
}