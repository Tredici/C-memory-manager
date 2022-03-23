#include <iostream>
#include "../memory_manager.h"
using namespace std;

char buffer[10000000];

int main(int argc, char const *argv[])
{
    cout << "Test: " << __FILE__ << endl;
    if (mm_init_memory_manager(buffer, sizeof(buffer)))
    {
        cerr << "Disastro mm_init_memory_manager" << endl;
        return 1;
    }
    constexpr int total = 10;
    int *num[total] = {};

    for (int i=0; i != total; ++i)
    {
        int *p = (int *)mm_malloc(sizeof(int));
        if (!p)
        {
            cout << "Disastro p!" << endl;
            return 1;
        }
        *(num[i] = p) = i;
        cout << "Allocato " << i << " " << (long)p << endl;
    }
    int sum = 0;
    for (int i=0; i != total; ++i)
    {
        sum += *num[i];
    }
    cout << "Sum = " << sum << endl;

    cout << "+++++LIBERA+++++" << endl;
    for (int i=0; i != total/2; ++i)
    {
        auto p = num[i];
        cout << "Liberato " << i << " " << (long)p << endl;
        mm_free(p);
        num[i] = nullptr;
    }
    cout << "+++++RICREA+++++" << endl;
    for (int i=0; i != total/2+1; ++i)
    {
        auto p = mm_malloc(sizeof(long));
        cout << "Allocato " << i << " " << (long)p << endl;
    }
    cout << "+++++FATTO+++++" << endl;
    return 0;
}


