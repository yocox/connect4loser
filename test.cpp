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
struct RepStr<C, 0> {
    using type = String<>;
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

using WWWW = WN<4, 0>::type;
using WWWN = WN<3, 1>::type;
using WWNN = WN<2, 2>::type;
using WNNN = WN<1, 3>::type;

using BBBB = InvString<WWWW>::type;
using BBBN = InvString<WWWN>::type;
using BBNN = InvString<WWNN>::type;
using BNNN = InvString<WNNN>::type;

/////////////////////////////////////////////////////////////////////////////
// matcher
/////////////////////////////////////////////////////////////////////////////

template <int X0, int Y0, int XI, int YI, typename StringType>
struct Matcher;

template <int X0, int Y0, int XI, int YI, Color Head, Color ... Tail>
struct Matcher<X0, Y0, XI, YI, String<Head, Tail...>> {
    enum { len = sizeof...(Tail) + 1 };
    enum { x0 = X0 };
    enum { y0 = Y0 };
    enum { xi = XI };
    enum { yi = YI };
    bool static check(const Table& t) {
        return t(X0, Y0) == Head && Matcher<X0 + XI, Y0 + YI, XI, YI, String<Tail...>>::check(t);
    }
    using string_type = String<Head, Tail...>;
};

template <int X0, int Y0, int XI, int YI>
struct Matcher<X0, Y0, XI, YI, String<>> {
    enum { len = 0 };
    enum { x0 = X0 };
    enum { y0 = Y0 };
    enum { xi = XI };
    enum { yi = YI };
    bool static check(const Table&) {
        return true;
    }
    using string_type = String<>;
};

template <int XI, int Len> struct XRanger { enum { begin = 0 }; enum { end = W - Len }; };
template <int Len> struct XRanger<0, Len> { enum { begin = 0 }; enum { end = W }; };
template <int Len> struct XRanger<-1, Len> { enum { begin = Len - 1 }; enum { end = W }; };
template <int YI, int Len> struct YRanger { enum { begin = 0 }; enum { end = H - Len }; };
template <int Len> struct YRanger<0, Len> { enum { begin = 0 }; enum { end = H }; };
template <int Len> struct YRanger<-1, Len> { enum { begin = Len - 1 }; enum { end = H }; };

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
    static int count(const Table&) {
        return 0;
    }
};

template <template<int, int> class MatcherType>
struct MatchCounter {
    enum { len_pat = MatcherType<0, 0>::len };
    enum { X0 = XRanger<MatcherType<0, 0>::xi, len_pat>::begin };
    enum { Y0 = YRanger<MatcherType<0, 0>::yi, len_pat>::begin };
    enum { X1 = XRanger<MatcherType<0, 0>::xi, len_pat>::end };
    enum { Y1 = YRanger<MatcherType<0, 0>::yi, len_pat>::end };
    static int count(const Table& t) {
        std::cout << typeid(typename MatcherType<0, 0>::string_type).name() << len_pat << "(" << X0 << ", " << Y0 << ") ~ (" << X1 << ", " << Y1 << ")" << std::endl;
        return MatchCounter_impl<MatcherType, X0, Y0, X0, Y0, X1, Y1>::count(t);
    }
};

template <typename StringType>
struct StringCounter {
    template <int X0, int Y0> using HoStr = Matcher<X0, Y0, 1, 0, StringType>;
    template <int X0, int Y0> using VeStr = Matcher<X0, Y0, 0, 1, StringType>;
    template <int X0, int Y0> using D1Str = Matcher<X0, Y0, 1, 1, StringType>;
    template <int X0, int Y0> using D2Str = Matcher<X0, Y0, 1,-1, StringType>;
    static int count(const Table& t) {
        return MatchCounter<HoStr>::count(t) +
               MatchCounter<VeStr>::count(t) +
               MatchCounter<D1Str>::count(t) +
               MatchCounter<D2Str>::count(t);
    }
};

/////////////////////////////////////////////////////////////////////////
// Score Weights
/////////////////////////////////////////////////////////////////////////

const int S_4C = 1000000;
const int S_3N =   10000;
const int S_3B =    2000;
const int S_2N =    1000;
const int S_2B =     200;
const int S_1N =     100;
const int S_1B =      20;

int eval(const Table&)
{
    int score = 0;
    ///////////////////////////////////////////////////////////
    // win
    ///////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////
    // not win, calculate score
    ///////////////////////////////////////////////////////////

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
using ma_hori = Matcher<X0, Y0, 1, 0, String<WHITE, WHITE, WHITE, WHITE>>;

int main()
{
    Table t;
    using WW = String<WHITE, WHITE, WHITE, WHITE>;
    t.put(0, BLACK);
    t.put(1, BLACK);
    t.put(2, BLACK);
    t.put(3, BLACK);
    t.put(0, WHITE);
    t.put(1, WHITE);
    t.put(2, WHITE);
    t.put(3, WHITE);
    t.put(0, WHITE);
    t.put(0, WHITE);
    t.put(0, WHITE);
    t.put(1, WHITE);
    t.put(2, WHITE);
    t.put(3, WHITE);
    std::cout << StringCounter<WW>::count(t) << std::endl;
    //std::cout << MatchCounter<HoriPat<BLACK, BLACK, BLACK, BLACK>::type>::count(t) << std::endl;

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
