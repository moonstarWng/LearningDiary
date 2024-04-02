#include <iostream>
#include <vector>
using namespace std;

class NQueen
{
private:
    vector<vector<string>> _res;
    int _count;
public:
    NQueen(int n)
    : _count(0)
    {
        _res.clear();
        vector<string> chessboard(n, string(n, '.'));
        backtracing(n, 0, chessboard);
        printChessboard();
    }

    bool isValid(int row, int col, vector<string> &chessboard, int n) {
        for (int i = 0; i < row; i++) {
            if (chessboard[i][col] == 'Q') {
                return false;
            }
        }
        for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
            if (chessboard[i][j] == 'Q') {
                return false;
            }
        }
        for (int i = row - 1, j = col + 1; i >= 0 && j < n; i--, j++) {
            if (chessboard[i][j] == 'Q') {
                return false;
            }
        }
        return true;
    }

    void backtracing(int n, int row, vector<string> &chessboard)
    {
        if (row == n) {
            _count++;
            _res.emplace_back(chessboard);
            return;
        }
        for (int col = 0; col < n; col++) {
            if (isValid(row, col, chessboard, n)) {
                chessboard[row][col] = 'Q';
                backtracing(n, row + 1, chessboard);
                chessboard[row][col] = '.';
            }
        }
    }
    
    void printChessboard() {
        for (auto i : _res) {
            for (auto j : i) {
                cout << j << endl;
            }
            cout << "---------------" << endl;
        }
        cout << "一共有 " << _count << " 个解" << endl;
        cout << "---------------" << endl;
    }
};

int main(int argc, char *argv[])
{
    int n;
    while(1){
        cout << "请输入一个整数(输入 -1 即可退出)：" ;
        cin >> n;
        if(n == -1) {
            break;
        }
        NQueen q(n);
    } 
    cout << "程序退出！" << endl;
    return 0;
}

