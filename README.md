# ulong
 C++ unsigned long integer


time test log
первые 4 числа, это время относительно u64
дальше для каждого типа 5-10 идет пара чисел (время относительно u64, сколько было потрачено для одного u64 блока в ulong)

1) u64
2) ulong<u64, u32, 1> = u64
3) ulong<u32, u16, 2> = u64
4) ulong<u16, u8,  4> = u64

5) ulong<u32, u16, 4> = u128
6) ulong<u64, u32, 2> = u128

7) ulong<u32, u16, 8> = u256
8) ulong<u64, u32, 4> = u256

9) ulong<u32, u16, 32> = u1024
10) ulong<u64, u32, 16> = u1024

<ITERATION TEST..
N = 1000000
iteration inc
1 5 4 5
5 3
6 3
7 2
8 2
18 1
18 1

iteration dec
1 7 7 6
7 4
8 4
8 2
8 2
23 1
27 2

BIT SHIFT TEST..
1 7 6 6
9 5
8 4
24 6
21 5
72 5
49 3

ADITION TEST..
1 6 6 10
11 5
9 5
23 6
21 5
75 5
65 4

SUBTRACTION TEST..
1 7 8 12
11 6
10 5
23 6
21 5
79 5
70 4

MULTIPLICATION TEST..
N = 100000
1 12 25 77
78 39
31 15
268 67
114 28
4138 259
1444 90

DIVISION TEST.. (unsinged(-1) / 1)
N = 1000
1 1130 1172 1674
3677 1839
3035 1517
14192 3548
12722 3181
162814 10176
138397 8650>
