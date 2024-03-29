#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

class txt
{
public:
    txt(ifstream &map_file, ifstream &input)
    {
        buildMap(map_file);
        string text;
        while (getline(input, text))
        {
            istringstream stream(text);
            string word;
            bool firstword = true;
            while (stream >> word)
            {
                if (firstword)
                {
                    firstword = false;
                }
                else
                {
                    cout << " ";
                }
                cout << transform(word);
            }
            cout << endl;
        }
    }

    void buildMap(ifstream &map_file){
        string key, val;
        while (map_file >> key && getline(map_file, val))
        {
            if(val.size() > 1)
            {
                _dir[key] = val.substr(1);
            } 
            else
            {
                throw runtime_error("no rule for " + key);
            }
        }
    }

    const string & transform(const string &s)
    {
        auto m = _dir;
        auto map_it = m.find(s);
        if (map_it != m.cend())
        {
            return map_it->second;
        }
        else
        {
            return s;
        }
    }
private:
    unordered_map<string, string> _dir;
};

int main(int argc, char *argv[])
{
    ifstream map_file("map.txt");
    ifstream input("file.txt");

    txt(map_file, input);
    return 0;
}

