#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <array>
#include <utility>
#include <ctime>

const int W = 7;
const int H = 6;

enum Color { NONE = 0, WHITE = 1, BLACK = 2 };

struct Table
{
    Table() {
        for (int x = 0; x < W; ++x) {
            for (int y = 0; y < H; ++y) {
                v[x][y] = NONE;
            }
        }
    }
    Color v[W][H];
    Color operator()(const int x, const int y) const {
        return v[x][y];
    }
    bool putable(const int x) const {
        return (v[x][H - 1]) == NONE;
    }
    void put(const int x, Color c) {
        for (int y = 0; y < H; ++y) {
            if (v[x][y] == NONE) {
                v[x][y] = c;
                steps.emplace_back(x, y);
                return;
            }
        }
    }

    void pop() {
        const auto& pos = steps.back();
        v[pos.first][pos.second] = NONE;
        steps.pop_back();
    }

    void print() const
    {
        for (int y = H - 1; y >= 0; --y) {
            for (int x = 0; x < W; ++x) {
                if (v[x][y] == WHITE) {
                    std::cout << "x";
                }
                else if (v[x][y] == BLACK) {
                    std::cout << "o";
                }
                else {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << "==========" << std::endl;
        for (int x = 0; x < W; ++x) {
            std::cout << x;
        }
        std::cout << std::endl;
    }
    
    std::vector<std::pair<int, int>> steps;


};

template <int XI, int YI, int D, Color ... COLORS>
struct Pattern;

template <int XI, int YI, int D, Color Head, Color ... Tail>
struct Pattern<XI, YI, D, Head, Tail...> {
    bool static check(const Table& t, const int x, const int y) {
        return t(x + D * XI, y + D * YI) == Head && Pattern<XI, YI, D + 1, Tail...>::check(t, x, y);
    }
};

template <int XI, int YI, int D>
struct Pattern<XI, YI, D> {
    bool static check(const Table& t, const int x, const int y) {
        return true;
    }
};

template <int XI, int YI, int D, Color ... COLORS>
struct SymPattern {
    bool static check(const Table& t, const int x, const int y) {
        return Pattern<XI, YI,  D, COLORS...>::check(t, x, y) ||
               Pattern<-XI, -YI, D, COLORS...>::check(t, x + XI * (static_cast<int>(sizeof...(COLORS)-1)), y + YI * (static_cast<int>(sizeof...(COLORS)) - 1));
    }
};

template <Color ... Colors> using HoriPattern = SymPattern<1, 0, 0, Colors...>;
template <Color ... Colors> using VertPattern = SymPattern<0, 1, 0, Colors...>;
template <Color ... Colors> using Dia1Pattern = SymPattern<1, 1, 0, Colors...>;
template <Color ... Colors> using Dia2Pattern = SymPattern<1,-1, 0, Colors...>;

/////////////////////////////////////////////////////////////////////////////
// horizontal
/////////////////////////////////////////////////////////////////////////////

// 100000
using H_WWWW = HoriPattern<WHITE, WHITE, WHITE, WHITE>;
using H_BBBB = HoriPattern<BLACK, BLACK, BLACK, BLACK>;

// 10000
using H_NWWW = HoriPattern< NONE, WHITE, WHITE, WHITE>;
using H_NBBB = HoriPattern< NONE, BLACK, BLACK, BLACK>;
using H_WWWN = HoriPattern<WHITE, WHITE, WHITE, NONE>;
using H_BBBN = HoriPattern<BLACK, BLACK, BLACK, NONE>;

// 2000
using H_BWWW = HoriPattern<BLACK, WHITE, WHITE, WHITE>;
using H_WBBB = HoriPattern<WHITE, BLACK, BLACK, BLACK>;
using H_WWWB = HoriPattern<WHITE, WHITE, WHITE, BLACK>;
using H_BBBW = HoriPattern<BLACK, BLACK, BLACK, WHITE>;

// 1000
using H_NWW = HoriPattern< NONE, WHITE, WHITE>;
using H_NBB = HoriPattern< NONE, BLACK, BLACK>;
using H_WWN = HoriPattern<WHITE, WHITE, NONE>;
using H_BBN = HoriPattern<BLACK, BLACK, NONE>;

// 200
using H_BWW = HoriPattern<BLACK, WHITE, WHITE>;
using H_WBB = HoriPattern<WHITE, BLACK, BLACK>;
using H_WWB = HoriPattern<WHITE, WHITE, BLACK>;
using H_BBW = HoriPattern<BLACK, BLACK, WHITE>;

// 100
using H_NW = HoriPattern< NONE, WHITE>;
using H_NB = HoriPattern< NONE, BLACK>;
using H_WN = HoriPattern<WHITE, NONE>;
using H_BN = HoriPattern<BLACK, NONE>;

// 20
using H_BW = HoriPattern<BLACK, WHITE>;
using H_WB = HoriPattern<WHITE, BLACK>;
using H_WB = HoriPattern<WHITE, BLACK>;
using H_BW = HoriPattern<BLACK, WHITE>;

/////////////////////////////////////////////////////////////////////////////
// vertical
/////////////////////////////////////////////////////////////////////////////

// 100000
using V_WWWW = VertPattern<WHITE, WHITE, WHITE, WHITE>;
using V_BBBB = VertPattern<BLACK, BLACK, BLACK, BLACK>;

// 10000
using V_NWWW = VertPattern< NONE, WHITE, WHITE, WHITE>;
using V_NBBB = VertPattern< NONE, BLACK, BLACK, BLACK>;
using V_WWWN = VertPattern<WHITE, WHITE, WHITE, NONE>;
using V_BBBN = VertPattern<BLACK, BLACK, BLACK, NONE>;

// 2000
using V_BWWW = VertPattern<BLACK, WHITE, WHITE, WHITE>;
using V_WBBB = VertPattern<WHITE, BLACK, BLACK, BLACK>;
using V_WWWB = VertPattern<WHITE, WHITE, WHITE, BLACK>;
using V_BBBW = VertPattern<BLACK, BLACK, BLACK, WHITE>;

// 1000
using V_NWW = VertPattern< NONE, WHITE, WHITE>;
using V_NBB = VertPattern< NONE, BLACK, BLACK>;
using V_WWN = VertPattern<WHITE, WHITE, NONE>;
using V_BBN = VertPattern<BLACK, BLACK, NONE>;

// 200
using V_BWW = VertPattern<BLACK, WHITE, WHITE>;
using V_WBB = VertPattern<WHITE, BLACK, BLACK>;
using V_WWB = VertPattern<WHITE, WHITE, BLACK>;
using V_BBW = VertPattern<BLACK, BLACK, WHITE>;

// 100
using V_NW = VertPattern< NONE, WHITE>;
using V_NB = VertPattern< NONE, BLACK>;
using V_WN = VertPattern<WHITE, NONE>;
using V_BN = VertPattern<BLACK, NONE>;

// 20
using V_BW = VertPattern<BLACK, WHITE>;
using V_WB = VertPattern<WHITE, BLACK>;
using V_WB = VertPattern<WHITE, BLACK>;
using V_BW = VertPattern<BLACK, WHITE>;

/////////////////////////////////////////////////////////////////////////////
// Dia1
/////////////////////////////////////////////////////////////////////////////

// 100000
using D1_WWWW = Dia1Pattern<WHITE, WHITE, WHITE, WHITE>;
using D1_BBBB = Dia1Pattern<BLACK, BLACK, BLACK, BLACK>;

// 10000
using D1_NWWW = Dia1Pattern< NONE, WHITE, WHITE, WHITE>;
using D1_NBBB = Dia1Pattern< NONE, BLACK, BLACK, BLACK>;
using D1_WWWN = Dia1Pattern<WHITE, WHITE, WHITE, NONE>;
using D1_BBBN = Dia1Pattern<BLACK, BLACK, BLACK, NONE>;

// 2000
using D1_BWWW = Dia1Pattern<BLACK, WHITE, WHITE, WHITE>;
using D1_WBBB = Dia1Pattern<WHITE, BLACK, BLACK, BLACK>;
using D1_WWWB = Dia1Pattern<WHITE, WHITE, WHITE, BLACK>;
using D1_BBBW = Dia1Pattern<BLACK, BLACK, BLACK, WHITE>;

// 1000
using D1_NWW = Dia1Pattern< NONE, WHITE, WHITE>;
using D1_NBB = Dia1Pattern< NONE, BLACK, BLACK>;
using D1_WWN = Dia1Pattern<WHITE, WHITE, NONE>;
using D1_BBN = Dia1Pattern<BLACK, BLACK, NONE>;

// 200
using D1_BWW = Dia1Pattern<BLACK, WHITE, WHITE>;
using D1_WBB = Dia1Pattern<WHITE, BLACK, BLACK>;
using D1_WWB = Dia1Pattern<WHITE, WHITE, BLACK>;
using D1_BBW = Dia1Pattern<BLACK, BLACK, WHITE>;

// 100
using D1_NW = Dia1Pattern< NONE, WHITE>;
using D1_NB = Dia1Pattern< NONE, BLACK>;
using D1_WN = Dia1Pattern<WHITE, NONE>;
using D1_BN = Dia1Pattern<BLACK, NONE>;

// 20
using D1_BW = Dia1Pattern<BLACK, WHITE>;
using D1_WB = Dia1Pattern<WHITE, BLACK>;
using D1_WB = Dia1Pattern<WHITE, BLACK>;
using D1_BW = Dia1Pattern<BLACK, WHITE>;

/////////////////////////////////////////////////////////////////////////////
// Dai2
/////////////////////////////////////////////////////////////////////////////

// 100000
using D2_WWWW = Dia2Pattern<WHITE, WHITE, WHITE, WHITE>;
using D2_BBBB = Dia2Pattern<BLACK, BLACK, BLACK, BLACK>;

// 10000
using D2_NWWW = Dia2Pattern< NONE, WHITE, WHITE, WHITE>;
using D2_NBBB = Dia2Pattern< NONE, BLACK, BLACK, BLACK>;
using D2_WWWN = Dia2Pattern<WHITE, WHITE, WHITE, NONE>;
using D2_BBBN = Dia2Pattern<BLACK, BLACK, BLACK, NONE>;

// 2000
using D2_BWWW = Dia2Pattern<BLACK, WHITE, WHITE, WHITE>;
using D2_WBBB = Dia2Pattern<WHITE, BLACK, BLACK, BLACK>;
using D2_WWWB = Dia2Pattern<WHITE, WHITE, WHITE, BLACK>;
using D2_BBBW = Dia2Pattern<BLACK, BLACK, BLACK, WHITE>;

// 1000
using D2_NWW = Dia2Pattern< NONE, WHITE, WHITE>;
using D2_NBB = Dia2Pattern< NONE, BLACK, BLACK>;
using D2_WWN = Dia2Pattern<WHITE, WHITE, NONE>;
using D2_BBN = Dia2Pattern<BLACK, BLACK, NONE>;

// 200
using D2_BWW = Dia2Pattern<BLACK, WHITE, WHITE>;
using D2_WBB = Dia2Pattern<WHITE, BLACK, BLACK>;
using D2_WWB = Dia2Pattern<WHITE, WHITE, BLACK>;
using D2_BBW = Dia2Pattern<BLACK, BLACK, WHITE>;

// 100
using D2_NW = Dia2Pattern< NONE, WHITE>;
using D2_NB = Dia2Pattern< NONE, BLACK>;
using D2_WN = Dia2Pattern<WHITE, NONE>;
using D2_BN = Dia2Pattern<BLACK, NONE>;

// 20
using D2_BW = Dia2Pattern<BLACK, WHITE>;
using D2_WB = Dia2Pattern<WHITE, BLACK>;
using D2_WB = Dia2Pattern<WHITE, BLACK>;
using D2_BW = Dia2Pattern<BLACK, WHITE>;


//=========================

template <int COLOR>
bool horizon4(const Table& t, const int x, const int y) {
    if (t(x + 0, y) == COLOR &&
        t(x + 1, y) == COLOR &&
        t(x + 2, y) == COLOR &&
        t(x + 3, y) == COLOR)
    {
        return true;
    }
    return false;
}

template <int COLOR>
bool vertical4(const Table& t, const int x, const int y) {
    if (t(x, y + 0) == COLOR &&
        t(x, y + 1) == COLOR &&
        t(x, y + 2) == COLOR &&
        t(x, y + 3) == COLOR)
    {
        return true;
    }
    return false;
}

template <int COLOR>
bool left_bottom4(const Table& t, const int x, const int y) {
    if (t(x + 0, y + 0) == COLOR &&
        t(x + 1, y + 1) == COLOR &&
        t(x + 2, y + 2) == COLOR &&
        t(x + 3, y + 3) == COLOR)
    {
        return true;
    }
    return false;
}

template <int COLOR>
bool left_top4(const Table& t, const int x, const int y) {
    if (t(x + 0, y - 0) == COLOR &&
        t(x + 1, y - 1) == COLOR &&
        t(x + 2, y - 2) == COLOR &&
        t(x + 3, y - 3) == COLOR)
    {
        return true;
    }
    return false;
}

const int S_4C = 1000000;
const int S_3N =  10000;
const int S_3B =   2000;
const int S_2N =   1000;
const int S_2B =    200;
const int S_1N =    100;
const int S_1B =     20;

int eval(const Table& t)
{
    int score = 0;
    ///////////////////////////////////////////////////////////
    // win
    ///////////////////////////////////////////////////////////
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BBBB::check(t, x, y)) { return -S_4C; } if ( V_WWWW::check(t, x, y)) { return  S_4C; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W - 3; ++x) { if (D1_BBBB::check(t, x, y)) { return -S_4C; } if (D1_WWWW::check(t, x, y)) { return  S_4C; } } }
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if ( H_BBBB::check(t, x, y)) { return -S_4C; } if ( H_WWWW::check(t, x, y)) { return  S_4C; } } }
    for (int y = 3; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if (D2_BBBB::check(t, x, y)) { return -S_4C; } if (D2_WWWW::check(t, x, y)) { return  S_4C; } } }

    // WWWN
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if ( H_BBBN::check(t, x, y)) { score -= S_3N; } if ( H_WWWN::check(t, x, y)) { score += S_3N; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BBBN::check(t, x, y)) { score -= S_3N; } if ( V_WWWN::check(t, x, y)) { score += S_3N; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W - 3; ++x) { if (D1_BBBN::check(t, x, y)) { score -= S_3N; } if (D1_WWWN::check(t, x, y)) { score += S_3N; } } }
    for (int y = 3; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if (D2_BBBN::check(t, x, y)) { score -= S_3N; } if (D2_WWWN::check(t, x, y)) { score += S_3N; } } }

    // WWWB
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if ( H_BBBW::check(t, x, y)) { score -= S_3B; } if ( H_WWWB::check(t, x, y)) { score += S_3B; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BBBW::check(t, x, y)) { score -= S_3B; } if ( V_WWWB::check(t, x, y)) { score += S_3B; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W - 3; ++x) { if (D1_BBBW::check(t, x, y)) { score -= S_3B; } if (D1_WWWB::check(t, x, y)) { score += S_3B; } } }
    for (int y = 3; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if (D2_BBBW::check(t, x, y)) { score -= S_3B; } if (D2_WWWB::check(t, x, y)) { score += S_3B; } } }

    // WWN
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if ( H_BBN::check(t, x, y)) { score -= S_2N; } if ( H_WWN::check(t, x, y)) { score += S_2N; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BBN::check(t, x, y)) { score -= S_2N; } if ( V_WWN::check(t, x, y)) { score += S_2N; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W - 2; ++x) { if (D1_BBN::check(t, x, y)) { score -= S_2N; } if (D1_WWN::check(t, x, y)) { score += S_2N; } } }
    for (int y = 2; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if (D2_BBN::check(t, x, y)) { score -= S_2N; } if (D2_WWN::check(t, x, y)) { score += S_2N; } } }

    // WWB
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if ( H_BBW::check(t, x, y)) { score -= S_2B; } if ( H_WWB::check(t, x, y)) { score += S_2B; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BBW::check(t, x, y)) { score -= S_2B; } if ( V_WWB::check(t, x, y)) { score += S_2B; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W - 2; ++x) { if (D1_BBW::check(t, x, y)) { score -= S_2B; } if (D1_WWB::check(t, x, y)) { score += S_2B; } } }
    for (int y = 2; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if (D2_BBW::check(t, x, y)) { score -= S_2B; } if (D2_WWB::check(t, x, y)) { score += S_2B; } } }

    // WN
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if ( H_BN::check(t, x, y)) { score -= S_1N; } if ( H_WN::check(t, x, y)) { score += S_1N; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BN::check(t, x, y)) { score -= S_1N; } if ( V_WN::check(t, x, y)) { score += S_1N; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W - 1; ++x) { if (D1_BN::check(t, x, y)) { score -= S_1N; } if (D1_WN::check(t, x, y)) { score += S_1N; } } }
    for (int y = 1; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if (D2_BN::check(t, x, y)) { score -= S_1N; } if (D2_WN::check(t, x, y)) { score += S_1N; } } }

    // WB
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if ( H_BW::check(t, x, y)) { score -= S_1B; } if ( H_WB::check(t, x, y)) { score += S_1B; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BW::check(t, x, y)) { score -= S_1B; } if ( V_WB::check(t, x, y)) { score += S_1B; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W - 1; ++x) { if (D1_BW::check(t, x, y)) { score -= S_1B; } if (D1_WB::check(t, x, y)) { score += S_1B; } } }
    for (int y = 1; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if (D2_BW::check(t, x, y)) { score -= S_1B; } if (D2_WB::check(t, x, y)) { score += S_1B; } } }

    return score;
}

struct Trace {
    int x;
    int score;
    int future;
};

std::pair<int, int> minmax(Table& t, const int deep)
{
    bool want_max = deep % 2 == 1;

    if (want_max) {
        for (int x = 0; x < 6; ++x) {
            if (t.putable(x)) {
                t.put(x, WHITE);
                const int s = eval(t);
                t.pop();
                if (s == S_4C) {
                    return { x, S_4C };
                }
            }
        }
    }
    else {
        for (int x = 0; x < 6; ++x) {
            if (t.putable(x)) {
                t.put(x, BLACK);
                const int s = eval(t);
                t.pop();
                if (s == -S_4C) {
                    return{ x, -S_4C };
                }
            }
        }
    }

    int s[6];
    for (int i = 0; i < 6; ++i)
    {
        if (t.putable(i))
        {
            t.put(i, want_max ? WHITE : BLACK);
            if (deep == 1) {
                s[i] = eval(t);
            }
            else {
                auto tr = minmax(t, deep - 1);
                s[i] = tr.second;
            }
            t.pop();
        }
        else
        {
            if (want_max)
                s[i] = std::numeric_limits<int>::min();
            else
                s[i] = std::numeric_limits<int>::max();
        }
    }

    if (want_max) {
        auto max_i = std::max_element(std::begin(s), std::end(s));
        return{ max_i - std::begin(s), *max_i };
    }
    else {
        auto min_i = std::min_element(std::begin(s), std::end(s));
        return{ min_i - std::begin(s), *min_i };
    }
}

int choose_a_move(Table& t, int level)
{
    std::cout << "thinking..." << std::endl;
    auto i_score = minmax(t, level);
    return i_score.first;
}

int main()
{
    int level = 1;
    std::cout << "level:[0~3] ";
    std::cin >> level;
    level = level * 2 + 1;

    Table t;
    srand((unsigned)std::time(NULL));
    if (rand() % 2 == 1);
        t.put(3, WHITE);

    t.print();

    while (true) {
        // player
        std::cout << "choose : ";
        int i;
        std::cin >> i;
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
        std::cout << "CPU put " << m << std::endl;
        t.put(m, WHITE);
        t.print();

        if (eval(t) == S_4C) {
            std::cout << "you lose" << std::endl;
            return 0;
        }

    }
}
