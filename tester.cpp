#include "ulong.h"
#include "time.h"

#include <iostream>
#include <random>

#include <iomanip>

using namespace std;

int verify_test() {

#define check(good, may_bad) \
if (may_bad != good) {\
    cout << a << " " << b << " " << good << "\n";\
    return 0;\
}

    cout << "BRUTE FORCE TESTING..\n";
    {
        using elong = ulong<u8, u8, 4>;
        using uvar = u32;

        cout << "bit operations test..\n";
        {
            using elong = ulong<u16, u8, 1>;
            using uvar = u16;

            for (u32 a = 1; a <= 1e5; a++) {
                for (int b = -4 * (int)sizeof(uvar); b <= 4 * (int)sizeof(uvar); b++) {

                    uvar good1 = uvar(a) >> b;

                    elong may_bad1 = elong(a) >> b;

                    check(good1, may_bad1);

                    ////////////////////////////

                    uvar good2 = uvar(a) << b;

                    elong may_bad2 = elong(a) << b;

                    check(good2, may_bad2);
                }
            }
        }

        cout << "addition test..\n";
        {
            for (u32 a = UINT_MAX - 1e4; a <= UINT_MAX; a++) {
                for (u32 b = UINT_MAX - 1e4; b <= UINT_MAX; b++) {

                    uvar sum = a + b;

                    elong my_sum = elong(a) + b;

                    check(sum, my_sum);

                    if (b == UINT_MAX) {
                        break;
                    }
                }

                if (a == UINT_MAX) {
                    break;
                }
            }

            for (u32 a = 0; a <= 0xff; a++) {
                for (u32 b = 0; b <= 0xffff; b++) {

                    uvar good = uvar(a) + uvar(b);

                    elong may_bad = elong(uvar(a)) + uvar(b);

                    check(good, may_bad);
                }
            }
        }

        cout << "subtraction test..\n";
        {
            for (u64 a = UINT_MAX - 1e4; a <= UINT_MAX; a++) {
                for (u64 b = UINT_MAX - 1e4; b <= UINT_MAX; b++) {

                    uvar sum = a - b;

                    elong my_sum = elong(a) - b;

                    check(sum, my_sum);

                    if (b == UINT_MAX) {
                        break;
                    }
                }
                if (a == UINT_MAX) {
                    break;
                }
            }

            for (u32 a = 0; a <= 0xff; a++) {
                for (u32 b = 0; b <= 0xffff; b++) {

                    uvar good = uvar(a) - uvar(b);

                    elong may_bad = elong(uvar(a)) - uvar(b);

                    check(good, may_bad);
                }
            }
        }

        cout << "multiplication test..\n";
        {
            using elong = ulong<u16, u8, 4>;
            using uvar = u64;

            for (uvar a = 1e5; a <= 1e5 + 1e3; a++) {
                for (uvar b = 1e5; b <= 1e5 + 1e3; b++) {

                    uvar sum = a * b;

                    elong my_sum = elong(a) * b;

                    check(sum, my_sum);
                }
            }

            for (uvar a = 1e9; a <= 1e9 + 1e3; a++) {
                for (uvar b = 1e9; b <= 1e9 + 1e3; b++) {

                    uvar sum = a * b;

                    elong my_sum = elong(a) * b;

                    check(sum, my_sum);
                }
            }
        }

        cout << "division test..\n";
        {
            using elong = ulong<u8, u8, 4>;
            using uvar = u32;

            for (uvar a = 1e5; a <= 1e5 + 1e3; a++) {
                for (uvar b = 1e5; b <= 1e5 + 1e3; b++) {

                    uvar sum = a / b;

                    elong my_sum = elong(a) / b;

                    check(sum, my_sum);
                }
            }

            cout << "1 successfully\n";

            for (uvar a = 1e9; a <= 1e9 + 1e3; a++) {
                for (uvar b = 1e9; b <= 1e9 + 1e3; b++) {

                    uvar sum = a / b;

                    elong my_sum = elong(a) / b;

                    check(sum, my_sum);
                }
            }

            cout << "2 successfully\n";

            for (uvar a = 0; a <= 0xfff; a++) {
                for (uvar b = 0; b <= 0xfff; b++) {

                    uvar sum = 0;
                    if (b != 0) {
                        sum = a / b;
                    }

                    elong my_sum = elong(a) / b;

                    check(sum, my_sum);
                }
            }

            cout << "3 successfully\n";
        }

        cout << "remainder of the division test..\n";
        {
            using elong = ulong<u8, u8, 4>;
            using uvar = u32;

            for (uvar a = 1e5; a <= 1e5 + 1e3; a++) {
                for (uvar b = 1e5; b <= 1e5 + 1e3; b++) {

                    uvar sum = a % b;

                    elong my_sum = elong(a) % b;

                    check(sum, my_sum);
                }
            }

            cout << "1 successfully\n";

            for (uvar a = 1e9; a <= 1e9 + 1e3; a++) {
                for (uvar b = 1e9; b <= 1e9 + 1e3; b++) {

                    uvar sum = a % b;

                    elong my_sum = elong(a) % b;

                    check(sum, my_sum);
                }
            }

            cout << "2 successfully\n";

            for (uvar a = 0; a <= 0xfff; a++) {
                for (uvar b = 0; b <= 0xfff; b++) {

                    uvar sum = a;
                    if (b != 0) {
                        sum = a % b;
                    }

                    elong my_sum = elong(a) % b;

                    check(sum, my_sum);
                }
            }

            cout << "3 successfully\n";
        }
    }
    
    int rnd_test = 1e6;
    int rnd_case = 30;

    cout << "RANDOM TESTS..\n";
    {
        mt19937_64 rnd(time(NULL));

        using elong = ulong<u16, u8, 4>;
        using uvar = u64;

        cout << "addition test..\n";
        {
            for (int test_case = 0; test_case < rnd_case; test_case++) {
                for (int i = 0; i < rnd_test; i++) {
                    uvar a = rnd();
                    uvar b = rnd();

                    uvar x = a + b;

                    elong y = elong(a) + b;

                    check(x, y);
                }
                cout << "test_case: " << test_case << "\n";

                rnd = mt19937_64(time(NULL));
            }
        }

        cout << "subtraction test..\n";
        {
            for (int test_case = 0; test_case < rnd_case; test_case++) {
                for (int i = 0; i < rnd_test; i++) {
                    uvar a = rnd();
                    uvar b = rnd();

                    uvar x = a - b;

                    elong y = elong(a) - b;

                    check(x, y);
                }
                cout << "test_case: " << test_case << "\n";

                rnd = mt19937_64(time(NULL));
            }
        }

        cout << "multiplication test..\n";
        {
            for (int test_case = 0; test_case < rnd_case; test_case++) {
                for (int i = 0; i < rnd_test; i++) {
                    uvar a = rnd();
                    uvar b = rnd();

                    uvar x = a * b;

                    elong y = elong(a) * b;

                    check(x, y);
                }
                cout << "test_case: " << test_case << "\n";

                rnd = mt19937_64(time(NULL));
            }
        }

        cout << "division test..\n";
        {
            for (int test_case = 0; test_case < rnd_case; test_case++) {
                for (int i = 0; i < rnd_test; i++) {
                    uvar a = rnd();
                    uvar b = rnd();

                    uvar x = 0;
                    if (b != 0) {
                        x = a / b;
                    }

                    elong y = elong(a) / b;

                    check(x, y);
                }
                cout << "test_case: " << test_case << "\n";

                rnd = mt19937_64(time(NULL));
            }
        }
        cout << "remainder of the division test..\n";
        {
            for (int test_case = 0; test_case < rnd_case; test_case++) {
                for (int i = 0; i < rnd_test; i++) {
                    uvar a = rnd();
                    uvar b = rnd();

                    uvar x = a;
                    if (b != 0) {
                        x = a % b;
                    }

                    elong y = elong(a) % b;

                    check(x, y);
                }
                cout << "test_case: " << test_case << "\n";

                rnd = mt19937_64(time(NULL));
            }
        }
    }

    cout << "everything checked successfully :)\n";
}


#pragma optimize ("", off)

template<typename T>
double iteration_inc(u64 n) {
    Timer t;

    for (T i = 0; i < n; ++i) {}

    return t.time();
}

template<typename T>
double iteration_dec(u64 n) {
    Timer t;

    for (T i = n; i > 0; --i) {}

    return t.time();
}

template<typename T>
double bit_shift(u64 n) {
    Timer t;

    for (u32 i = 0; i < n; i++) {
        static_cast<T>(i) << n;
    }

    return t.time();
}

template<typename T>
double addition(u64 sqrtn) {
    Timer t;
    for (u32 i = 0; i < sqrtn; i++) {
        for (u64 j = 0; j < sqrtn; j++) {
            T(i) + T(j);
        }
    }
    return t.time();
}

template<typename T>
double subtraction(u64 sqrtn) {
    Timer t;
    for (u32 i = 0; i < sqrtn; i++) {
        for (u64 j = 0; j < sqrtn; j++) {
            T(i) - T(j);
        }
    }
    return t.time();
}

template<typename T>
double multiply(u64 sqrtn) {
    Timer t;
    for (u32 i = 0; i < sqrtn; i++) {
        for (u64 j = 0; j < sqrtn; j++) {
            T(i) * T(j);
        }
    }
    return t.time();
}

template<typename T>
double division(u64 sqrtn) {
    Timer t;
    for (u32 i = 0; i < sqrtn; i++) {
        for (u64 j = 0; j < sqrtn; j++) {
            T(-1) / T(1);
        }
    }
    return t.time();
}

void time_test() {
    u64 n = 1'000'000;
    cout << "TIME TEST. N = " << n << "\n";

    cout << "\
1) u64\n\
2) ulong<u64, u32, 1> = u64\n\
3) ulong<u32, u16, 2> = u64\n\
4) ulong<u16, u8,  4> = u64\n\
\n\
5) ulong<u32, u16, 4> = u128\n\
6) ulong<u64, u32, 2> = u128\n\
\n\
7) ulong<u32, u16, 8> = u256\n\
8) ulong<u64, u32, 4> = u256\n\
\n\
9) ulong<u32, u16, 32> = u1024\n\
10) ulong<u64, u32, 16> = u1024\n\
";

    cout << fixed << setprecision(0);

    using f0 = u64;
    using f1 = ulong<u64, u32, 1>;
    using f2 = ulong<u32, u16, 2>;
    using f3 = ulong<u16, u8, 4>;

    using f4 = ulong<u32, u16, 4>;
    using f5 = ulong<u64, u32, 2>;

    using f6 = ulong<u32, u16, 8>;
    using f7 = ulong<u64, u32, 4>;

    using f8 = ulong<u32, u16, 32>;
    using f9 = ulong<u64, u32, 16>;

#define pp(p, tp) p / p0 << " " << (p / p0) / (sizeof(tp) / sizeof(p0)) << "\n"

#define foo(func)\
double p0, p1, p2, p3, p4, p5, p6, p7, p8, p9;\
p0 = p1 = p2 = p3 = p4 = p5 = p6 = p7 = p8 = p9 = 0;\
for(int i = 0; i < 3; ++i){\
    p0 += func<f0>(n);\
    p1 += func<f1>(n);\
    p2 += func<f2>(n);\
    p3 += func<f3>(n);\
\
    p4 += func<f4>(n);\
    p5 += func<f5>(n);\
\
    p6 += func<f6>(n);\
    p7 += func<f7>(n);\
\
    p8 += func<f8>(n);\
    p9 += func<f9>(n);\
}\
cout << 1 << " " << p1 / p0 << " " << p2 / p0 << " " << p3 / p0 << "\n" <<\
pp(p4, f4) << pp(p5, f5) << pp(p6, f6) << pp(p7, f7) << pp(p8, f8) << pp(p9, f9) << "\n";

    cout << "ITERATION TEST..\n";
    {
        {
            cout << "iteration inc\n";

            foo(iteration_inc);
        }
        {
            cout << "iteration dec\n";

            foo(iteration_dec);
        }
    }

    cout << "BIT SHIFT TEST..\n";
    {
        foo(bit_shift);
    }

    cout << "ADITION TEST..\n";
    {
        n = sqrt(n);
        foo(addition);
        n *= n;
    }

    cout << "SUBTRACTION TEST..\n";
    {
        n = sqrt(n);
        foo(subtraction);
        n *= n;
    }

    n /= 10;
    cout << "MULTIPLICATION TEST..\nN = " << n << "\n";
    {
        n = sqrt(n);
        foo(multiply);
        n *= n;
    }

    n = 1000;
    cout << "DIVISION TEST..\nN = " << n << "\n";
    {
        n = sqrt(n);
        foo(division);
        n *= n;
    }
}

int main() {

    verify_test();

    time_test();


    return 0;
}