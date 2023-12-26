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
            //�߳�ͬ��
            std::unique_lock<std::mutex> lock(mtx_);
            //������֪ͨ�˿�ȡ
            while (current_ == max_) {
                not_full_.wait(lock);
            }
            current_++;
            std::cout << "�߳�ID��" << std::this_thread::get_id() << "   �����ݹ񣬵�ǰ��ݹ��У� " << current_ << std::endl;
            //˯��
            std::this_thread::sleep_for(std::chrono::microseconds(1500));
            not_empty_.notify_all();
        }
    }

    void consumer() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx_);
            //û�п����֪ͨ���Ա����Ͷ��
            while (current_ == 0) {
                not_empty_.wait(lock);
            }
            current_--;
            std::cout << "�߳�ID��" << std::this_thread::get_id() << "   �˿�ȡ�߿�ݣ���ǰ��ݹ��У�" << current_ << std::endl;
            //˯��1000΢��
            std::this_thread::sleep_for(std::chrono::microseconds(3000));
            //δ��֪ͨ ֪ͨ���Ա����Ͷ��
            not_full_.notify_all();
        }
    }
private:
    const int max_ = MAX_CABINET_COUNT;
    int current_ = 0;
    //�̻߳���
    std::mutex mtx_;
    //�߳�ͨ��
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