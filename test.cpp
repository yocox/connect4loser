#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <cassert>
#include <typeinfo>
#include <bitset>

const int W = 7;
const int H = 6;

////////////////////////////////////////////////////////////////////////
// ½L­±
////////////////////////////////////////////////////////////////////////

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
        assert(0 && "can not put in this column!");
        std::cerr << "can not put in this column!" << std::endl;
    }

    void pop() {
        const auto& pos = steps.back();
        v[pos.first][pos.second] = NONE;
        steps.pop_back();
    }

    void print() const
    {
        for (int y = H - 1; y >= 0; --y) {
            std::cout << H - y << "|";
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
// Compile Time Unroll Pattern Matcher
/////////////////////////////////////////////////////////////////////////////

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
// ColorString
/////////////////////////////////////////////////////////////////////////////

template <Color C> struct InvColor { const static Color color = NONE ; };
template <> struct InvColor<WHITE> { const static Color color = BLACK; };
template <> struct InvColor<BLACK> { const static Color color = WHITE; };

template <Color ... Colors>
struct String {
    using type = String<Colors ...>;
};

// Append<"WW", N, N> = "WWNN"
template <typename StringType, Color ... Colors>
struct Append;

template <Color Head, Color ... StrColors, Color ... Tail>
struct Append<String<StrColors...>, Head, Tail...> {
    using type = typename Append<String<StrColors..., Head>, Tail...>::type;
};

template <Color ... StrColors>
struct Append<String<StrColors...>> {
    using type = String<StrColors...>;
};

// InvString<"WW"> = "BB"
template <typename StringType>
struct InvString;

template <Color ... Colors>
struct InvString<String<Colors...>> {
    using type = String<InvColor<Colors>::color ...>;
};

// RevString<"WWWN"> = "NWWW"
template <typename StringType>
struct RevString;

template <Color Head, Color ...Tail>
struct RevString<String<Head, Tail...>> {
    using type = typename Append<typename RevString<String<Tail...>>::type, Head>::type;
};

template <Color Head>
struct RevString<String<Head>> {
    using type = typename String<Head>::type;
};

// Concate<"WWW", "BBB"> = "WWWBBB"
template <typename S1, typename S2>
struct Concate;

template <typename S1, Color ... S2Colors>
struct Concate<S1, String<S2Colors...>> {
    using type = typename Append<S1, S2Colors...>::type;
};

template <typename S1>
struct Concate<S1, String<>> {
    using type = S1;
};

// RepStr<W, 5> = "WWWWW"
template <Color C, int N>
struct RepStr {
    using type = typename Append<typename RepStr<C, N - 1>::type, C>::type;
};

template <Color C>
struct RepStr<C, 1> {
    using type = String<C>;
};

// WN<4, 2> = "WWWWNN"
template <int WHITE_LEN, int NONE_LEN>
struct WN {
    using type = typename Concate<
                    typename RepStr<WHITE, WHITE_LEN>::type,
                    typename RepStr<NONE, NONE_LEN>::type
                 >::type;
};

/////////////////////////////////////////////////////////////////////////////
// Meta Pattern
/////////////////////////////////////////////////////////////////////////////

using WWWW = WN<4, 0>;
using WWWN = WN<3, 1>;
using WWNN = WN<2, 2>;
using WNNN = WN<1, 3>;



/////////////////////////////////////////////////////////////////////////////
// matcher
/////////////////////////////////////////////////////////////////////////////

template <int X0, int Y0, int XI, int YI, Color ... COLORS>
struct Matcher;

template <int X0, int Y0, int XI, int YI, Color Head, Color ... Tail>
struct Matcher<X0, Y0, XI, YI, Head, Tail...> {
    enum { len = sizeof...(Tail) + 1 };
    enum { X0 = X0 };
    enum { Y0 = Y0 };
    enum { XI = XI };
    enum { YI = YI };
    bool static check(const Table& t) {
        return t(X0, Y0) == Head && Matcher<X0 + XI, Y0 + YI, XI, YI, Tail...>::check(t);
    }
};

template <int X0, int Y0, int XI, int YI>
struct Matcher<X0, Y0, XI, YI> {
    enum { len = 0 };
    enum { X0 = X0 };
    enum { Y0 = Y0 };
    enum { XI = XI };
    enum { YI = YI };
    bool static check(const Table& t) {
        return true;
    }
    using string_type = String<>;
};

template <template<int,int> class MatcherType>
struct MatchCounter {
    enum { len_pat = MatcherType::len };
    enum { X0 = 0 };
    enum { Y0 = 0 };
    enum { X1 = 3 };
    enum { Y1 = 3 };
    static int count(const Table& t) {
        return MatchCounter_impl<MatcherType, X0, Y0, X0, Y0, X1, Y1>::count(t);
    }
};

template <template<int, int> class MatcherType, int X, int Y, int X0, int Y0, int X1, int Y1>
struct MatchCounter_impl {
    static int count(const Table& t) {
        return MatcherType<X, Y>::check(t) + MatchCounter_impl<MatcherType, X + 1, Y, X0, Y0, X1, Y1>::count(t);
    }
};

template <template<int, int> class MatcherType, int Y, int X0, int Y0, int X1, int Y1>
struct MatchCounter_impl<MatcherType, X1, Y, X0, Y0, X1, Y1> {
    static int count(const Table& t) {
        return 0 + MatchCounter_impl<MatcherType, X0, Y + 1, X0, Y0, X1, Y1>::count(t);
    }
};

template <template<int, int> class MatcherType, int X, int X0, int Y0, int X1, int Y1>
struct MatchCounter_impl<MatcherType, X, Y1, X0, Y0, X1, Y1> {
    static int count(const Table& t) {
        return 0 + MatchCounter_impl<MatcherType, X + 1, Y1, X0, Y0, X1, Y1>::count(t);
    }
};

template <template<int, int> class MatcherType, int X0, int Y0, int X1, int Y1>
struct MatchCounter_impl<MatcherType, X1, Y1, X0, Y0, X1, Y1> {
    static int count(const Table& t) {
        return 0;
    }
};


/////////////////////////////////////////////////////////////////////////////
// horizontal
/////////////////////////////////////////////////////////////////////////////

using H_WWWW = HoriPattern<WHITE, WHITE, WHITE, WHITE>;
using H_BBBB = HoriPattern<BLACK, BLACK, BLACK, BLACK>;
using H_NWWW = HoriPattern< NONE, WHITE, WHITE, WHITE>;
using H_NBBB = HoriPattern< NONE, BLACK, BLACK, BLACK>;
using H_BWWW = HoriPattern<BLACK, WHITE, WHITE, WHITE>;
using H_WBBB = HoriPattern<WHITE, BLACK, BLACK, BLACK>;
using H_NWW = HoriPattern< NONE, WHITE, WHITE>;
using H_NBB = HoriPattern< NONE, BLACK, BLACK>;
using H_BWW = HoriPattern<BLACK, WHITE, WHITE>;
using H_WBB = HoriPattern<WHITE, BLACK, BLACK>;
using H_NW = HoriPattern< NONE, WHITE>;
using H_NB = HoriPattern< NONE, BLACK>;
using H_BW = HoriPattern<BLACK, WHITE>;

/////////////////////////////////////////////////////////////////////////////
// vertical
/////////////////////////////////////////////////////////////////////////////

using V_WWWW = VertPattern<WHITE, WHITE, WHITE, WHITE>;
using V_BBBB = VertPattern<BLACK, BLACK, BLACK, BLACK>;
using V_NWWW = VertPattern< NONE, WHITE, WHITE, WHITE>;
using V_NBBB = VertPattern< NONE, BLACK, BLACK, BLACK>;
using V_BWWW = VertPattern<BLACK, WHITE, WHITE, WHITE>;
using V_WBBB = VertPattern<WHITE, BLACK, BLACK, BLACK>;
using V_NWW = VertPattern< NONE, WHITE, WHITE>;
using V_NBB = VertPattern< NONE, BLACK, BLACK>;
using V_BWW = VertPattern<BLACK, WHITE, WHITE>;
using V_WBB = VertPattern<WHITE, BLACK, BLACK>;
using V_NW = VertPattern< NONE, WHITE>;
using V_NB = VertPattern< NONE, BLACK>;
using V_BW = VertPattern<BLACK, WHITE>;

/////////////////////////////////////////////////////////////////////////////
// Dia1
/////////////////////////////////////////////////////////////////////////////

using D1_WWWW = Dia1Pattern<WHITE, WHITE, WHITE, WHITE>;
using D1_BBBB = Dia1Pattern<BLACK, BLACK, BLACK, BLACK>;
using D1_NWWW = Dia1Pattern< NONE, WHITE, WHITE, WHITE>;
using D1_NBBB = Dia1Pattern< NONE, BLACK, BLACK, BLACK>;
using D1_BWWW = Dia1Pattern<BLACK, WHITE, WHITE, WHITE>;
using D1_WBBB = Dia1Pattern<WHITE, BLACK, BLACK, BLACK>;
using D1_NWW = Dia1Pattern< NONE, WHITE, WHITE>;
using D1_NBB = Dia1Pattern< NONE, BLACK, BLACK>;
using D1_BWW = Dia1Pattern<BLACK, WHITE, WHITE>;
using D1_WBB = Dia1Pattern<WHITE, BLACK, BLACK>;
using D1_NW = Dia1Pattern< NONE, WHITE>;
using D1_NB = Dia1Pattern< NONE, BLACK>;
using D1_BW = Dia1Pattern<BLACK, WHITE>;

/////////////////////////////////////////////////////////////////////////////
// Dai2
/////////////////////////////////////////////////////////////////////////////

using D2_WWWW = Dia2Pattern<WHITE, WHITE, WHITE, WHITE>;
using D2_BBBB = Dia2Pattern<BLACK, BLACK, BLACK, BLACK>;
using D2_NWWW = Dia2Pattern< NONE, WHITE, WHITE, WHITE>;
using D2_NBBB = Dia2Pattern< NONE, BLACK, BLACK, BLACK>;
using D2_BWWW = Dia2Pattern<BLACK, WHITE, WHITE, WHITE>;
using D2_WBBB = Dia2Pattern<WHITE, BLACK, BLACK, BLACK>;
using D2_NWW = Dia2Pattern< NONE, WHITE, WHITE>;
using D2_NBB = Dia2Pattern< NONE, BLACK, BLACK>;
using D2_BWW = Dia2Pattern<BLACK, WHITE, WHITE>;
using D2_WBB = Dia2Pattern<WHITE, BLACK, BLACK>;
using D2_NW = Dia2Pattern< NONE, WHITE>;
using D2_NB = Dia2Pattern< NONE, BLACK>;
using D2_BW = Dia2Pattern<BLACK, WHITE>;

/////////////////////////////////////////////////////////////////////////
// Score Weights
/////////////////////////////////////////////////////////////////////////

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
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if ( H_BBBB::check(t, x, y)) { return -S_4C; } if ( H_WWWW::check(t, x, y)) { return  S_4C; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BBBB::check(t, x, y)) { return -S_4C; } if ( V_WWWW::check(t, x, y)) { return  S_4C; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W - 3; ++x) { if (D1_BBBB::check(t, x, y)) { return -S_4C; } if (D1_WWWW::check(t, x, y)) { return  S_4C; } } }
    for (int y = 3; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if (D2_BBBB::check(t, x, y)) { return -S_4C; } if (D2_WWWW::check(t, x, y)) { return  S_4C; } } }

    // WWWN
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if ( H_NBBB::check(t, x, y)) { score -= S_3N; } if ( H_NWWW::check(t, x, y)) { score += S_3N; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_NBBB::check(t, x, y)) { score -= S_3N; } if ( V_NWWW::check(t, x, y)) { score += S_3N; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W - 3; ++x) { if (D1_NBBB::check(t, x, y)) { score -= S_3N; } if (D1_NWWW::check(t, x, y)) { score += S_3N; } } }
    for (int y = 3; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if (D2_NBBB::check(t, x, y)) { score -= S_3N; } if (D2_NWWW::check(t, x, y)) { score += S_3N; } } }

    // WWWB
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if ( H_WBBB::check(t, x, y)) { score -= S_3B; } if ( H_BWWW::check(t, x, y)) { score += S_3B; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_WBBB::check(t, x, y)) { score -= S_3B; } if ( V_BWWW::check(t, x, y)) { score += S_3B; } } }
    for (int y = 0; y < H - 3; ++y) { for (int x = 0; x < W - 3; ++x) { if (D1_WBBB::check(t, x, y)) { score -= S_3B; } if (D1_BWWW::check(t, x, y)) { score += S_3B; } } }
    for (int y = 3; y < H    ; ++y) { for (int x = 0; x < W - 3; ++x) { if (D2_WBBB::check(t, x, y)) { score -= S_3B; } if (D2_BWWW::check(t, x, y)) { score += S_3B; } } }

    // WWN
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if ( H_NBB::check(t, x, y)) { score -= S_2N; } if ( H_NWW::check(t, x, y)) { score += S_2N; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_NBB::check(t, x, y)) { score -= S_2N; } if ( V_NWW::check(t, x, y)) { score += S_2N; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W - 2; ++x) { if (D1_NBB::check(t, x, y)) { score -= S_2N; } if (D1_NWW::check(t, x, y)) { score += S_2N; } } }
    for (int y = 2; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if (D2_NBB::check(t, x, y)) { score -= S_2N; } if (D2_NWW::check(t, x, y)) { score += S_2N; } } }

    // WWB
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if ( H_WBB::check(t, x, y)) { score -= S_2B; } if ( H_BWW::check(t, x, y)) { score += S_2B; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_WBB::check(t, x, y)) { score -= S_2B; } if ( V_BWW::check(t, x, y)) { score += S_2B; } } }
    for (int y = 0; y < H - 2; ++y) { for (int x = 0; x < W - 2; ++x) { if (D1_WBB::check(t, x, y)) { score -= S_2B; } if (D1_BWW::check(t, x, y)) { score += S_2B; } } }
    for (int y = 2; y < H    ; ++y) { for (int x = 0; x < W - 2; ++x) { if (D2_WBB::check(t, x, y)) { score -= S_2B; } if (D2_BWW::check(t, x, y)) { score += S_2B; } } }

    // WN
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if ( H_NB::check(t, x, y)) { score -= S_1N; } if ( H_NW::check(t, x, y)) { score += S_1N; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_NB::check(t, x, y)) { score -= S_1N; } if ( V_NW::check(t, x, y)) { score += S_1N; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W - 1; ++x) { if (D1_NB::check(t, x, y)) { score -= S_1N; } if (D1_NW::check(t, x, y)) { score += S_1N; } } }
    for (int y = 1; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if (D2_NB::check(t, x, y)) { score -= S_1N; } if (D2_NW::check(t, x, y)) { score += S_1N; } } }

    // WB
    for (int y = 0; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if ( H_BW::check(t, x, y)) { score -= S_1B; } if ( H_BW::check(t, x, y)) { score += S_1B; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W    ; ++x) { if ( V_BW::check(t, x, y)) { score -= S_1B; } if ( V_BW::check(t, x, y)) { score += S_1B; } } }
    for (int y = 0; y < H - 1; ++y) { for (int x = 0; x < W - 1; ++x) { if (D1_BW::check(t, x, y)) { score -= S_1B; } if (D1_BW::check(t, x, y)) { score += S_1B; } } }
    for (int y = 1; y < H    ; ++y) { for (int x = 0; x < W - 1; ++x) { if (D2_BW::check(t, x, y)) { score -= S_1B; } if (D2_BW::check(t, x, y)) { score += S_1B; } } }

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

template <int X0, int Y0>
using ma_hori = Matcher<X0, Y0, 1, 0, WHITE, WHITE, WHITE, WHITE>;

int main()
{
    Table t;
    using WW = String<WHITE, WHITE, WHITE, WHITE>;
    t.put(0, WHITE);
    t.put(1, WHITE);
    t.put(2, WHITE);
    t.put(3, WHITE);
    using maty = ma_hori<0, 0>;
    bool ee = maty::check(t);
    int cnt = MatchCounter<ma_hori>::count(t);

    return 0;

   



    int level = 1;
    std::cout << "level:[0~3] ";
    std::cin >> level;
    level = level * 2 + 1;

    srand((unsigned)std::time(NULL));
    if (rand() % 2 == 1)
        //t.put(3, WHITE)
        ;

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
