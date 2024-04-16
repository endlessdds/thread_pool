#include <bits/stdc++.h>
#include <gtest/gtest.h>

#include "../include/thread_pool.h"

using ll = long long;
std::vector<ll> mem;

ll fib(int k) {
    if (mem[k] != -1) {
        return mem[k];
    }
    if (k == 0 || k == 1) {
        return 1;
    }
    return mem[k] = fib(k - 1) + fib(k - 2);
}

TEST(thread_pool_test, thread_pool) {
    int fib_max = 10000;
    std::vector<ll> fib_v(fib_max, 1);
    mem.assign(fib_max, -1);
    for (int i = 2; i < fib_max; i++) {
        fib_v[i] = fib_v[i - 1] + fib_v[i - 2];
    }
    thread_pool tp{};
    tp.start(16);

    std::vector<ll> answer;
    std::vector<std::future<ll>> fts;
    int iter = 10000;
    for (int i = 0; i < iter; i++) {
        int k = rand() % fib_max;
        fts.push_back(tp.submit(fib, k));
        answer.push_back(fib_v[k]);
    }
    tp.close();
    for (int i = 0; i < iter; i++) {
        EXPECT_EQ(answer[i], fts[i].get());
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
