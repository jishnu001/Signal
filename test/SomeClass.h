#pragma once

#include "../Signal.h"
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

class Timeout
{
public:
    // @param timeoutInterval in seconds
    // @param onTimeOut callback function
    // @param singleShot timer
    Timeout(int timeoutInterval, bool singleShot) : timeoutInterval(timeoutInterval), singleShot(singleShot) {}

    // Start the timer
    void start()
    {
        th = std::thread([&]()
                         { run(); });
        th.detach();
    }
    JSig::Signal timeout;
    JSig::Signal1<std::string> timeoutWithData;

private:
    void run()
    {
        while (true)
        {
            std::this_thread::sleep_for(timeoutInterval);
            timeout.emit();
            timeoutWithData.emit("Its timeout");
            if (singleShot)
                return;
        }
    }

    std::chrono::seconds timeoutInterval;
    bool singleShot;
    std::thread th;
};

class SomeClass
{
public:
    SomeClass(int id) : m_id(id) {}

    void doSomething()
    {
        std::cerr << "\nCalling the slot with id : " << m_id << "\n";
        s.emit();
    }
    void listen() { std::cerr << "\nLISTEN!!\n"; }
    void doSomethingMore(int i)
    {
        std::cerr << "\nCalling the slot with id : " << m_id << " and value " << i;
        s1.emit(i);
    }
    void listenMore(const std::string &data)
    {
        std::lock_guard lock(m);
        m_id += 5;
        std::cerr << "\nLISTEN: " << data << " " << m_id << "\n";
    }
    JSig::Signal s;
    JSig::Signal1<int> &getS1() { return s1; }

private:
    int m_id;
    JSig::Signal1<int> s1;
    std::mutex m;
};
