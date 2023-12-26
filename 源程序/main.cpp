#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include<chrono>
#include<functional>
#define MAX_CABINET_COUNT 5


class MyClass
{
public:
    MyClass() {}
    ~MyClass() {}
    void producer() {
        while (true) {
            //线程同步
            std::unique_lock<std::mutex> lock(mtx_);
            //满了则通知顾客取
            while (current_ == max_) {
                not_full_.wait(lock);
            }
            current_++;
            std::cout << "线程ID：" << std::this_thread::get_id() << "   放入快递柜，当前快递柜共有： " << current_ << std::endl;
            //睡眠
            std::this_thread::sleep_for(std::chrono::microseconds(1500));
            not_empty_.notify_all();
        }
    }

    void consumer() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx_);
            //没有快递了通知快递员继续投递
            while (current_ == 0) {
                not_empty_.wait(lock);
            }
            current_--;
            std::cout << "线程ID：" << std::this_thread::get_id() << "   顾客取走快递，当前快递柜共有：" << current_ << std::endl;
            //睡眠1000微妙
            std::this_thread::sleep_for(std::chrono::microseconds(3000));
            //未满通知 通知快递员继续投递
            not_full_.notify_all();
        }
    }
private:
    const int max_ = MAX_CABINET_COUNT;
    int current_ = 0;
    //线程互斥
    std::mutex mtx_;
    //线程通信
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
};




int main() {
    MyClass A;
    std::thread t1(std::bind(&MyClass::producer, &A));
    std::thread t2(std::bind(&MyClass::consumer, &A));
    std::thread t3(std::bind(&MyClass::producer, &A));
    std::thread t4(std::bind(&MyClass::consumer, &A));


    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 0;
}