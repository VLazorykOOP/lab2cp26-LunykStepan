#include <iostream>
#include <thread>
#include <mutex>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <atomic>

using namespace std;

mutex mtx;
atomic<bool> running(true);

void workerBee(int id, double x, double y, double V)
{
    double homeX = x, homeY = y;

    while (running)
    {
        while (running && (abs(x) > 0.1 || abs(y) > 0.1))
        {
            x -= x * V;
            y -= y * V;

            mtx.lock();
            cout << "Worker " << id << " at (" << x << ", " << y << ")\n";
            mtx.unlock();

            this_thread::sleep_for(chrono::milliseconds(200));
        }

        while (running && (abs(x - homeX) > 0.1 || abs(y - homeY) > 0.1))
        {
            x += (homeX - x) * V;
            y += (homeY - y) * V;

            mtx.lock();
            cout << "Worker " << id << " returning (" << x << ", " << y << ")\n";
            mtx.unlock();

            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }
}

void droneBee(int id, double x, double y, double V, int N)
{
    srand(time(0) + id);

    while (running)
    {
        double angle = (rand() % 360) * 3.1415 / 180.0;
        double dx = cos(angle) * V;
        double dy = sin(angle) * V;

        auto start = chrono::steady_clock::now();

        while (running && chrono::duration_cast<chrono::seconds>(
            chrono::steady_clock::now() - start).count() < N)
        {
            x += dx;
            y += dy;

            mtx.lock();
            cout << "Drone " << id << " at (" << x << ", " << y << ")\n";
            mtx.unlock();

            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }
}

int main()
{
    double V = 0.2;
    int N = 2;

    thread w1(workerBee, 1, 5, 5, V);
    thread w2(workerBee, 2, 7, 6, V);

    thread d1(droneBee, 1, 0, 0, V, N);
    thread d2(droneBee, 2, 2, 2, V, N);

    this_thread::sleep_for(chrono::seconds(5));
    running = false;

    w1.join();
    w2.join();
    d1.join();
    d2.join();

    cout << "End\n";
}
