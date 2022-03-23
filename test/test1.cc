#include <iostream>
#include "../memory_manager.h"
using namespace std;

struct item
{
    int val;
    struct item* next;
    void clear()
    {
        if (next)
        {
            next->clear();
            mm_free(next);
        }
    }
};
struct list
{
    item *first = nullptr;

    int sum() const
    {
        int ans{};
        for (auto p = first; p; p = p->next)
            ans += p->val;
        return ans;
    }

    void create(item *&p, int val)
    {
        item *tmp = (item*)mm_malloc(sizeof(*tmp));
        if (!tmp)
        {
            cerr << "create()" << endl;
            throw 1;
        }
        *tmp = (item){}; // zero everything
        tmp->val = val;
        p = tmp;
    }

    list *push(int v)
    {
        if (!first)
        {
            create(first, v);
        }
        else
        {
            item *last = first;
            while (last->next)
            {
                cout << "+";
                last = last->next;
            }
            cout << endl;
            create(last->next, v);
        }
    }

    void clear()
    {
        if (first)
        {
            first->clear();
            mm_free(first);
        }
    }
};

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

    list *l = (list *)mm_malloc(sizeof(*l));
    int test = 0;
    if (!l)
        throw 10;
    for (int i=0; i != total; ++i)
    {
        l->push(i);
        cout << "Add " << i << endl;
        test+=i;
    }
    if (l->sum() != test)
    {
        cerr << "NOOOOO, sum = " << l->sum() << endl;
    }
    else
    {
        cerr << "OK, sum = " << l->sum() << endl;
    }


    /* code */
    return 0;
}


