#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <ctime>

std::vector<std::string> queue;
std::vector<std::string> ready;
std::mutex orderMtx;
std::mutex prepareMtx;

void order_acceptance(const std::vector<std::string> &dishes)
{
//    for (int i=0;i < amountOrders;++i)
    while (true)
    {
        orderMtx.lock();
        std::string order = dishes[rand()%dishes.size()];
        queue.push_back(order);
        std::cout << "Received an order: " << order << std::endl;
        orderMtx.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(rand()%6+5));
    }
}

void cooking()
{
    while (true)
    {
        if (!queue.empty())
        {
            orderMtx.lock();
            std::string order = queue.back();
            std::cout << "Order '" << order << "' to prepare..." << std::endl;
            queue.pop_back();
            orderMtx.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(rand() % 11 + 5));
            prepareMtx.lock();
            std::cout << "Order '" << order << "' is ready." << std::endl;
            ready.push_back(order);
            prepareMtx.unlock();
        }
    }
}

void delivery(const int amountOrders)
{
    for (int i=0; i < amountOrders;)
    {
        if (!ready.empty())
        {
            prepareMtx.lock();
            std::string order = ready.back();
            std::cout << "~Courier took the order '" << order << "'~" << std::endl;
            ready.pop_back();
            prepareMtx.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(30));
            ++i;
        }
    }
}




int main() {

    int amountOrders = 10;
    std::srand(std::time(nullptr));

    const std::vector<std::string> dishes {"pizza", "soup", "steak", "salad", "sushi"};

    std::thread orderThr (order_acceptance,std::ref(dishes));
    std::thread prepareThr (cooking);
    std::thread deliveryThr (delivery, amountOrders);

    orderThr.detach();
    prepareThr.detach();
    deliveryThr.join();

    std::cout << std::endl;
    std::cout << "/--Delivery is over for today--/" << std::endl;

    return 0;
}
