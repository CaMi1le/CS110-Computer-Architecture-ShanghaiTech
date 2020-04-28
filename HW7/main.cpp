#include <iostream>
#include <string>
#include <math.h>
#include <random>
#include "doubll.hpp"

#define UNUSED(X) (void)X

template<typename _Tp>
void print_list(const doubll<_Tp>& list)
{
    std::cout << "---------------------" << std::endl;
    typename doubll<_Tp>::const_iterator it;
    for(it = list.cbegin(); it != list.cend(); it++){
        std::cout << (*it) << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    UNUSED(argc); UNUSED(argv);
    doubll<double> list(1, 3.5);
    
    for(int i = 0; i < 100; i++){
        list.insert(list.cend(), (double)i);
    }
    print_list(list);
    doubll<double>::iterator it = list.begin();
    std::default_random_engine e(2);
    while(it != list.end()){
        int r = e() % 100;
        if(r < 70){
            // (*it) *= (*it);
            ++it;
        }else{
            doubll<double>::iterator p = it++;
            list.erase(p);
            std::cout << "Erase: " << (*p) << std::endl;
        }
    }
    doubll<double>::const_iterator cit = list.begin();
    for(cit = --list.cend(); cit != list.cbegin(); cit--)
    {
        std::cout << (*cit) << " ";
    }
    // print_list(list);
    list.reverse();
    print_list(list);

    doubll<std::string> list1(100, "hello world");
    list1.begin()->at(0) = 'H';
    std::cout << *list1.begin() << std::endl;

    return 0;
}