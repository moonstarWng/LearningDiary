#include <iostream>
#include <list>
#include <unordered_map>
using namespace std;

class Cache
{
private:
    int _capacity;
    list<pair<int, int>> _list;
    unordered_map<int, list<pair<int, int>>::iterator> _map;
public:
    Cache (int capacity) : _capacity(capacity) {}
    /* ------------------------------------------------ */
    int get(int key){
        if(_map.count(key) == 0)
        {
            return -1;
        }
        _list.splice(_list.begin(), _list, _map[key]);
        return _list.front().second;
    }
    /* ------------------------------------------------ */
    void put(int key, int val){
        if(_map.count(key)){
            _list.splice(_list.begin(), _list, _map[key]);
            _list.front().second = val;
        } else {
            _list.emplace_front(key, val);
            _map[key] = _list.begin();
        }
        if(_list.size() > _capacity){
            _map.erase(_list.back().first);
            _list.pop_back();
        }
    }
    /* ------------------------------------------------ */
    void print(){
        for(auto &i : _list){
            cout << "Key = " << i.first << "; Val = " << i.second << endl;
        }
        cout << "---------------------" << endl;
    }
};

void test(){
    Cache c(3);
    c.put(1, 1);
    c.print();
    /* ------------------------------------------------ */
    c.put(2, 2);
    c.print();
    /* ------------------------------------------------ */
    c.put(3, 3);
    c.print();
    /* ------------------------------------------------ */
    c.put(4, 4);
    c.print();
    /* ------------------------------------------------ */
    c.put(2, 2);
    c.print();
    /* ------------------------------------------------ */
    c.put(3, 5);
    c.print();
    /* ------------------------------------------------ */
    cout << "Val = " << c.get(4) << " when key = 4" << endl;
    c.print();
}

int main(int argc, char *argv[])
{
    test();
    return 0;
}

