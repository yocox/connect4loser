#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <cassert>

const int W = 7;
const int H = 6;

////////////////////////////////////////////////////////////////////////
// pre-calculated scores
////////////////////////////////////////////////////////////////////////

int S[65536] = { 0 };

const int S_4C = 1000000;
const int S_L3 = 100000;
const int S_D3 = 2000;
const int S_J3 = 1800;
const int S_L2 = 200;
const int S_J2 = 180;
const int S_1 = 20;


////////////////////////////////////////////////////////////////////////
// 盤面
////////////////////////////////////////////////////////////////////////

enum Color { NONE = 0, WHITE = 1, BLACK = 2, WALL = 3 };

struct Table
{
    Table() {
        for (int y = 0; y < H; ++y) {
            v[y] = 0;
        }
    }
    int v[H];
    void clear(const int x, const int y) {
        v[y] &= (~(0x3 << (x * 2)));
    }
    void set(const int x, const int y, Color c) {
        clear(x, y);
        v[y] |= (c << (x * 2));}
    Color get(const int x, const int y) const {
        return static_cast<Color>((v[y] >> (x * 2)) & 0x3);
    }
    Color operator()(const int x, const int y) const {
        return get(x, y);
    }
    bool putable(const int x) const {
        return (get(x, H - 1)) == NONE;
    }
    void put(const int x, Color c) {
        for (int y = 0; y < H; ++y) {
            if (get(x, y) == NONE) {
                set(x, y, c);
                steps.emplace_back(x, y);
                return;
            }
        }
        assert(0 && "can not put in this column!");
        std::cerr << "can not put in this column!" << std::endl;
    }

    void pop() {
        const auto& pos = steps.back();
        clear(pos.first, pos.second);
        steps.pop_back();
    }

    void print() const
    {
        for (int y = H - 1; y >= 0; --y) {
            std::cout << H - y << "|";
            for (int x = 0; x < W; ++x) {
                if (get(x, y) == WHITE) {
                    std::cout << "x";
                }
                else if (get(x, y) == BLACK) {
                    std::cout << "o";
                }
                else {
                    std::cout << " ";
                }
            }
            std::cout << "|" << std::endl;
        }
        std::cout << " +-------+" << std::endl;
        std::cout << "  ";
        for (int x = 0; x < W; ++x) {
            std::cout << x + 1;
        }
        std::cout << std::endl;
    }
    
    std::vector<std::pair<int, int>> steps;


};

/////////////////////////////////////////////////////////////////////////////
// Basic Patterns
// 四子一線 oooo
// 活三 _ooo_ o_oo_o
// 死三 ooo_ _ooo
// 跳三 o_oo oo_o
// 活二 oo__ _oo_ __oo
// 跳二 o_o_ _o_o o__o
// 活一 o___ _o__ __o_ ___o
/////////////////////////////////////////////////////////////////////////////

int has_4(const std::vector<Color>& v)
{
    for (std::vector<Color>::size_type i = 0; i < v.size() - 3; ++i) {
        if (v[i] == WHITE && v[i + 1] == WHITE && v[i + 2] == WHITE && v[i + 3] == WHITE) return 1;
        if (v[i] == BLACK && v[i + 1] == BLACK && v[i + 2] == BLACK && v[i + 3] == BLACK) return -1;
    }
    return 0;
}

int has_live3(const std::vector<Color>& v)
{
    for (std::vector<Color>::size_type i = 0; i < v.size() - 4; ++i) {
        if (v[i] == NONE && v[i + 1] == WHITE && v[i + 2] == WHITE && v[i + 3] == WHITE && v[i + 4] == NONE) return 1;
        if (v[i] == NONE && v[i + 1] == BLACK && v[i + 2] == BLACK && v[i + 3] == BLACK && v[i + 4] == NONE) return -1;
    }
    for (std::vector<Color>::size_type i = 0; i < v.size() - 5; ++i) {
        if (v[i] == WHITE && v[i + 1] == NONE && v[i + 2] == WHITE && v[i + 3] == WHITE && v[i + 4] == NONE && v[i + 5] == WHITE) return 1;
        if (v[i] == BLACK && v[i + 1] == NONE && v[i + 2] == BLACK && v[i + 3] == BLACK && v[i + 4] == NONE && v[i + 5] == BLACK) return -1;
    }
    return 0;
}

int has_dead3(const std::vector<Color>& v)
{
    for (std::vector<Color>::size_type i = 0; i < v.size() - 3; ++i) {
        if (v[i] == WHITE && v[i + 1] == WHITE && v[i + 2] == WHITE && v[i + 3] == NONE) return 1;
        if (v[i] == BLACK && v[i + 1] == BLACK && v[i + 2] == BLACK && v[i + 3] == NONE) return -1;
        if (v[i] == NONE && v[i + 1] == WHITE && v[i + 2] == WHITE && v[i + 3] == WHITE) return 1;
        if (v[i] == NONE && v[i + 1] == BLACK && v[i + 2] == BLACK && v[i + 3] == BLACK) return -1;
    }
    return 0;
}

int has_jump3(const std::vector<Color>& v)
{
    for (std::vector<Color>::size_type i = 0; i < v.size() - 3; ++i) {
        if (v[i] == WHITE && v[i + 1] == WHITE && v[i + 2] == NONE && v[i + 3] == WHITE) return 1;
        if (v[i] == BLACK && v[i + 1] == BLACK && v[i + 2] == NONE && v[i + 3] == BLACK) return -1;
        if (v[i] == WHITE && v[i + 1] == NONE && v[i + 2] == WHITE && v[i + 3] == WHITE) return 1;
        if (v[i] == BLACK && v[i + 1] == NONE && v[i + 2] == BLACK && v[i + 3] == BLACK) return -1;
    }
    return 0;
}

int has_live2(const std::vector<Color>& v)
{
    for (std::vector<Color>::size_type i = 0; i < v.size() - 3; ++i) {
        if (v[i] == NONE && v[i + 1] == NONE && v[i + 2] == WHITE && v[i + 3] == WHITE) return 1;
        if (v[i] == NONE && v[i + 1] == NONE && v[i + 2] == BLACK && v[i + 3] == BLACK) return -1;
        if (v[i] == NONE && v[i + 1] == WHITE && v[i + 2] == WHITE && v[i + 3] == NONE) return 1;
        if (v[i] == NONE && v[i + 1] == BLACK && v[i + 2] == BLACK && v[i + 3] == NONE) return -1;
        if (v[i] == WHITE && v[i + 1] == WHITE && v[i + 2] == NONE && v[i + 3] == NONE) return 1;
        if (v[i] == BLACK && v[i + 1] == BLACK && v[i + 2] == NONE && v[i + 3] == NONE) return -1;
    }
    return 0;
}

int has_jump2(const std::vector<Color>& v)
{
    for (std::vector<Color>::size_type i = 0; i < v.size() - 3; ++i) {
        if (v[i] == WHITE && v[i + 1] == NONE && v[i + 2] == WHITE && v[i + 3] == NONE) return 1;
        if (v[i] == BLACK && v[i + 1] == NONE && v[i + 2] == BLACK && v[i + 3] == NONE) return -1;
        if (v[i] == NONE && v[i + 1] == WHITE && v[i + 2] == NONE && v[i + 3] == WHITE) return 1;
        if (v[i] == NONE && v[i + 1] == BLACK && v[i + 2] == NONE && v[i + 3] == BLACK) return -1;
        if (v[i] == WHITE && v[i + 1] == NONE && v[i + 2] == NONE && v[i + 3] == WHITE) return 1;
        if (v[i] == BLACK && v[i + 1] == NONE && v[i + 2] == NONE && v[i + 3] == BLACK) return -1;
    }
    return 0;
}

int has_1(const std::vector<Color>& v)
{
    for (std::vector<Color>::size_type i = 0; i < v.size() - 3; ++i) {
        if (v[i] == WHITE && v[i + 1] == NONE && v[i + 2] == NONE && v[i + 3] == NONE) return 1;
        if (v[i] == BLACK && v[i + 1] == NONE && v[i + 2] == NONE && v[i + 3] == NONE) return -1;
        if (v[i] == NONE && v[i + 1] == WHITE && v[i + 2] == NONE && v[i + 3] == NONE) return 1;
        if (v[i] == NONE && v[i + 1] == BLACK && v[i + 2] == NONE && v[i + 3] == NONE) return -1;
        if (v[i] == NONE && v[i + 1] == NONE && v[i + 2] == WHITE && v[i + 3] == NONE) return 1;
        if (v[i] == NONE && v[i + 1] == NONE && v[i + 2] == BLACK && v[i + 3] == NONE) return -1;
        if (v[i] == NONE && v[i + 1] == NONE && v[i + 2] == NONE && v[i + 3] == WHITE) return 1;
        if (v[i] == NONE && v[i + 1] == NONE && v[i + 2] == NONE && v[i + 3] == BLACK) return -1;
    }
    return 0;
}

std::vector<Color> int2ColorVec(int n)
{
    std::vector<Color> result;
    for (int i = 0; i < 7; ++i) {
        result.push_back(static_cast<Color>(n & 0x3));
        n >>= 2;
    }
    return std::move(result);
}

int colors2int(Color c)
{
    return c;
}

template<typename... Rest>
int colors2int(Color c, Rest... rest)
{
    return c | (colors2int(rest...) << 2);
}

void construct_pre_table()
{
    for (int i = 0; i < 65536; ++i) {
        const std::vector<Color> colors = int2ColorVec(i);
        int s = 0;
        s = has_4    (colors); if (s == 1) { S[i] = S_4C; continue; } else if (s == -1) { S[i] = -S_4C; continue; }
        s = has_live3(colors); if (s == 1) { S[i] = S_L3; continue; } else if (s == -1) { S[i] = -S_L3; continue; }
        s = has_dead3(colors); if (s == 1) { S[i] = S_D3; continue; } else if (s == -1) { S[i] = -S_D3; continue; }
        s = has_jump3(colors); if (s == 1) { S[i] = S_J3; continue; } else if (s == -1) { S[i] = -S_J3; continue; }
        s = has_live2(colors); if (s == 1) { S[i] = S_L2; continue; } else if (s == -1) { S[i] = -S_L2; continue; }
        s = has_jump2(colors); if (s == 1) { S[i] = S_J2; continue; } else if (s == -1) { S[i] = -S_J2; continue; }
        s = has_1    (colors); if (s == 1) { S[i] = S_1 ; continue; } else if (s == -1) { S[i] = -S_1 ; continue; }
    }    
}

/////////////////////////////////////////////////////////////////////////
// Score Weights
/////////////////////////////////////////////////////////////////////////

template<typename... Colors>
int eval_line(Colors... args)
{
    int i = colors2int(args...);
    return S[i];
}

int eval(const Table& t)
{
    int score = 0;
    int ls = 0;

    // hori
    ls = eval_line(t(0, 0), t(1, 0), t(2, 0), t(3, 0), t(4, 0), t(5, 0), t(6, 0)); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 1), t(1, 1), t(2, 1), t(3, 1), t(4, 1), t(5, 1), t(6, 1)); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 2), t(1, 2), t(2, 2), t(3, 2), t(4, 2), t(5, 2), t(6, 2)); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 3), t(1, 3), t(2, 3), t(3, 3), t(4, 3), t(5, 3), t(6, 3)); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 4), t(1, 4), t(2, 4), t(3, 4), t(4, 4), t(5, 4), t(6, 4)); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 5), t(1, 5), t(2, 5), t(3, 5), t(4, 5), t(5, 5), t(6, 5)); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;

    // vert
    ls = eval_line(t(0, 0), t(0, 1), t(0, 2), t(0, 3), t(0, 4), t(0, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(1, 0), t(1, 1), t(1, 2), t(1, 3), t(1, 4), t(1, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(2, 0), t(2, 1), t(2, 2), t(2, 3), t(2, 4), t(2, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(3, 0), t(3, 1), t(3, 2), t(3, 3), t(3, 4), t(3, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(4, 0), t(4, 1), t(4, 2), t(4, 3), t(4, 4), t(4, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(5, 0), t(5, 1), t(5, 2), t(5, 3), t(5, 4), t(5, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(6, 0), t(6, 1), t(6, 2), t(6, 3), t(6, 4), t(6, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;

    // D1
    ls = eval_line(t(0, 2), t(1, 3), t(2, 4), t(3, 5), WALL                  ); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 1), t(1, 2), t(2, 3), t(3, 4), t(4, 5), WALL         ); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 0), t(1, 1), t(2, 2), t(3, 3), t(4, 4), t(5, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(1, 0), t(2, 1), t(3, 2), t(4, 3), t(5, 4), t(6, 5), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(2, 0), t(3, 1), t(4, 2), t(5, 3), t(6, 4), WALL         ); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(3, 0), t(4, 1), t(5, 2), t(6, 3), WALL                  ); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;

    // D2
    ls = eval_line(t(0, 3), t(1, 2), t(2, 1), t(3, 0), WALL                  ); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 4), t(1, 3), t(2, 2), t(3, 1), t(4, 0), WALL         ); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(0, 5), t(1, 4), t(2, 3), t(3, 2), t(4, 1), t(5, 0), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(1, 5), t(2, 4), t(3, 3), t(4, 2), t(5, 1), t(6, 0), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(2, 5), t(3, 4), t(4, 3), t(5, 2), t(6, 1), WALL); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;
    ls = eval_line(t(3, 5), t(4, 4), t(5, 3), t(6, 2), WALL                  ); if (ls == S_4C) return ls; if (ls == -S_4C) return ls; score += ls;

    return score;
}

std::pair<int, int> minmax(Table& t, const int deep, const bool want_max, const int known_limit)
{
    int s[W];
    int current_sub_tree_limit = want_max ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (int i = 0; i < W; ++i)
    {
        if (!t.putable(i)) {
            s[i] = want_max ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
            continue;
        }

        t.put(i, want_max ? WHITE : BLACK);
        int score = eval(t);
        if (want_max) {
            if (score == S_4C) { t.pop();  return{ i, S_4C }; }
        } else {
            if (score == -S_4C) { t.pop();  return{ i, -S_4C }; }
        }

        if (deep == 1) {
            s[i] = score;
        } else {
            auto tr = minmax(t, deep - 1, !want_max, current_sub_tree_limit);
            s[i] = tr.second;
        }
        t.pop();

        // alpha-beta
        if (want_max) {
            current_sub_tree_limit = std::max(current_sub_tree_limit, s[i]);
            if (current_sub_tree_limit > known_limit) {
                //std::cout << "alpha-beta proning " << current_sub_tree_limit << " > " << known_limit << std::endl;
                return { i, current_sub_tree_limit };
            }
        }
        else {
            current_sub_tree_limit = std::min(current_sub_tree_limit, s[i]);
            if (current_sub_tree_limit < known_limit) {
                //std::cout << "alpha-beta proning " << current_sub_tree_limit << " < " << known_limit << std::endl;
                return { i, current_sub_tree_limit };
            }
        }
    }

    if (want_max) {
        auto max_i = std::max_element(std::begin(s), std::end(s));
        return { max_i - std::begin(s), *max_i };
    }
    else {
        auto min_i = std::min_element(std::begin(s), std::end(s));
        return { min_i - std::begin(s), *min_i };
    }
}

int choose_a_move(Table& t, int level)
{
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    using want_max = bool;

    std::cout << "thinking... ";
    auto start = high_resolution_clock::now();
    auto i_score = minmax(t, level, want_max(true), std::numeric_limits<int>::max());
    auto finish = high_resolution_clock::now();

    milliseconds total_ms = std::chrono::duration_cast<milliseconds>(finish - start);
    std::cout << total_ms.count() << " ms" << std::endl;
    return i_score.first;
}

int main()
{
    construct_pre_table();

    int level = 1;
    std::cout << "level:[0~10] ";
    std::cin >> level;

    Table t;
    srand((unsigned)std::time(NULL));
    t.print();

    while (true) {
        // player
        std::cout << "choose : ";
        int i;
        std::cin >> i;
        --i;
        if (i < 0 || i > 6)
            break;

        t.put(i, BLACK);
        t.print();

        if (eval(t) == -S_4C) {
            std::cout << "you win" << std::endl;
            return 0;
        }

        // cpu
        int m = choose_a_move(t, level);
        std::cout << "CPU put " << m + 1 << std::endl;
        t.put(m, WHITE);
        t.print();

        if (eval(t) == S_4C) {
            std::cout << "you lose" << std::endl;
            return 0;
        }

    }
}
