#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
using namespace std;

class Dictionatry{
public:
    string dealWord(string & word){
        for(size_t idx = 0; idx != word.size(); idx++){
            if(!isalpha(word[idx])){
                return string(); // 空串代替非字母
            } else if (isupper(word[idx])){
                word[idx] = tolower(word[idx]); // 大写转小写
            }
        }
        return word;
    }

    void read(const string & filename){
        ifstream ifs(filename);
        if(!ifs){
            cerr << "ifs open file fail!" << endl;
            return;
        }
        string line;
        while(getline(ifs, line)){
            istringstream iss(line);
            string word;
            while(iss >> word){
                string newWord = dealWord(word);
                if(newWord != string()){
                    _map[newWord]++;
                }
            }
        }
        ifs.close();
    }

    void store(const string & filename){
        ofstream ofs(filename);
        if (!ofs){
            cerr << "ofs open file fail" << endl;
            return;
        }
        unordered_map<string, int>::iterator it = _map.begin();
        for(; it != _map.end(); it++){
            ofs << it->first << " " << it->second << endl;
        }
        ofs.close();
    }
    
private:
    unordered_map<string, int> _map;
};

void test(){
    Dictionatry dict;
    cout << "befor reading..." << endl;
    time_t beg = time(NULL);
    dict.read("The_Holy_Bible.txt");
    time_t end = time(NULL);
    cout << "time: " << (end - beg) << " s" << endl;
    cout << "after reading..." << endl;
    dict.store("dictunordered_map.dat");
}

int main(int argc, char *argv[])
{
    test();
    return 0;
}

