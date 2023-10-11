#include "../include/pifunc.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::string;
using std::getline;
using std::thread;

template <typename T>
bool retrieve(const string& file_name, T& res);

template <typename T>
bool store(const string& file_name, const T& val);

void prompt_change_batch_len(fast32& batch_length, fast32 threads);

void run_calc(const string batch_count_fn,
    const fast32 batch_len, fast64& batch_count, const fast32 threads,
    const bool& run, const bool& stop, bool& running, bool& stopped);

void prompt_take_cmd(bool& run, bool& stop, const bool& running, const bool& stopped);

int main()
{
    const fast32 threads = thread::hardware_concurrency();
    fast32 batch_length = threads * 1000;
    fast64 batch_count = 0;

    const string batch_length_fn = "batchlength.cpppi";
    const string batch_count_fn = "batchcount.cpppi";

    cout
        << "-----------------------\n"
        << "CppPi7\n"
        << "Version 1.0\n"
        << "System Threads: " << threads << "\n"
        << "-----------------------\n";

    cout << "\n";

    if (!retrieve(batch_count_fn, batch_count))
    {
        cout << "Warning: batchcount.cpppi not found or can't be read.\n";
    }
    if (!retrieve(batch_length_fn, batch_length))
    {
        cout << "Warning: batchlength.cpppi not found or can't be read.\n";
        cout << "\n";
        cout << "Default batch length: " << batch_length << "\n";
        prompt_change_batch_len(batch_length, threads);
        store(batch_length_fn, batch_length);
    }

    cout << "\n";

    cout
        << "Commands:\n"
        << "1 = start/pause\n"
        << "2 = exit\n";

    cout << "\n";

    bool stop = false;
    bool run = false;
    bool stopped = false;
    bool running = false;

    thread run_calc_thread(run_calc, batch_count_fn,
        batch_length, std::ref(batch_count), threads,
        std::ref(run), std::ref(stop), std::ref(running), std::ref(stopped));
    run_calc_thread.detach();

    prompt_take_cmd(run, stop, running, stopped);

}

template <typename T>
bool retrieve(const string& file_name, T& res)
{
    ifstream strm(file_name);
    if (!strm.is_open())
        return false;
    strm >> res;
    strm.close();
    return true;
}

template <typename T>
bool store(const string& file_name, const T& val)
{
    ofstream strm(file_name);
    if (!strm.is_open())
        return false;
    strm << val;
    strm.close();
    return true;
}



void prompt_change_batch_len(fast32& batch_len, fast32 threads)
{
    string user_in;
    cout << "Change default batch length? y / n: ";
    while (true)
    {
        getline(cin, user_in);
        if (user_in == "n") return;
        else if (user_in == "y") break;
        else
        {
            cout << "Invalid input, try again: ";
            continue;
        }
    }

    cout << "\n";

    cout << "Batch length must be divisible by system threads.\n";
    cout << "Enter a value: ";
    while (true)
    {
        getline(cin, user_in);
        try
        {
            fast32 temp_batch_len = stoi(user_in);
            if (temp_batch_len % threads != 0)
            {
                cout << "Invalid input, try again: ";
                continue;
            }
            batch_len = temp_batch_len;
            return;
        }
        catch(const std::exception& e)
        {
            cout << "Invalid input, try again: ";
            continue;
        }
    }
}

void run_calc(const string batch_count_fn,
    const fast32 batch_len, fast64& batch_count, const fast32 threads,
    const bool& run, const bool& stop, bool& running, bool& stopped)
{
    while (!stop)
    {
        stopped = false;
        while (run)
        {
            running = true;
            string ans = pifunc::pigroup(batch_count * batch_len, batch_len, threads);
            store(batch_count_fn, batch_count);
            string batch_file_fn = "batch" + std::to_string(batch_count) + ".txt";
            store(batch_file_fn, ans);
            ++batch_count;
        }
        running = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    stopped = true;
}

void prompt_take_cmd(bool& run, bool& stop, const bool& running, const bool& stopped)
{
    string user_in;
    while (!stop)
    {
        cout << "> ";
        getline(cin, user_in);

        if (user_in == "1")
        {
            switch (run)
            {
                case false:
                run = true;
                cout << "Starting...\n";
                while (!running)
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                cout << "Started.\n";
                break;

                case true:
                run = false;
                cout << "Pausing...\n";
                while (running)
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                cout << "Paused.\n";
                break;
            }
        }
        else if (user_in == "2")
        {
            run = false;
            stop = true;
            cout << "Stopping...\n";
            while (!stopped)
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            cout << "Stopped.\n";
        }
        else
        {
            cout << "Invalid command, try again.\n";
        }

        cout << "\n";
    }
}