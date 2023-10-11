#include "../include/pifunc.h"

#include <thread>
#include <future>
#include <vector>

using std::thread;
using std::future;
using std::launch;
using std::async;
using std::vector;

namespace pifunc
{
    struct pithread_t
    {
        fast64 start;
        fast32 amount;
        fast32 thread;
        fast32 threadct;
    };

    string pithread(const pithread_t p)
    {
        fast64 iteration = p.start + p.thread;
        string ans; ans.reserve(p.amount);
        for (fast32 i = 0; i < p.amount; ++i)
        {
            char c = pihex(iteration);
            ans.push_back(c);
            iteration += p.threadct;
        }
        return ans;
    }

    string pigroup(fast64 start, fast32 length, fast32 threads)
    {
        const fast32 amount = length / threads;
        if (length - (amount * threads) != 0)
            throw "length has to be divisible by threads";

        vector<future<string>> futures;
        futures.reserve(threads);
        for (fast32 i = 0; i < threads; ++i)
        {
            const pithread_t p {start, amount, i, threads};
            futures.push_back(async(launch::async, pithread, p));
        }

        vector<string> threadans;
        threadans.reserve(threads);
        for (future<string>& f : futures)
            threadans.push_back(f.get());

        string ans;
        ans.reserve(length);
        for (fast32 i = 0; i < amount; ++i)
            for (const string& s : threadans)
                ans.push_back(s[i]);

        return ans;
    }
}