#include <iostream>
#include <algorithm>
#include <memory>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <fstream>
using namespace std;


//----------------------------------------------------------
class QueryResult;

//----------------------------------------------------------
class TextQuery{
    /* 该类用于读取文件 */
    /* 并对文件进行处理 */
    /* 将文件保存为数据结构 */
public:
    using line_no = size_t;
    TextQuery(ifstream &);
    QueryResult query(const string &)const;
private:
    // 输入文件
    shared_ptr<vector<string>> file;
    // 每个单词到他所在行号的映射
    map<string, shared_ptr<set<line_no>>> wm;
};
//----------------------------------------------------------

TextQuery::TextQuery(ifstream &is): file(new vector<string>){ 
    /* 该函数用于读取文件 */
    /* 并进行处理 */
    string text;
    while(getline(is, text)){
        // 读一行，保存一行
        file->push_back(text);
        // n 为当前行号
        line_no n = file->size() - 1;
        // 将行分解为单词
        istringstream iss(text);
        string word;
        while(iss >> word){
            // 统计单词和行号
            auto &lines = wm[word];
            if(!lines){
                lines.reset(new set<line_no>);
            }
            lines->insert(n);
        }
    }
}
//----------------------------------------------------------
class QueryResult{
    /* 该类用于表示 string 出现的那些行 */
    using line_no = TextQuery::line_no;
    friend ostream& print(ostream &, const QueryResult &);
public:
    QueryResult(string s,
                shared_ptr<set<line_no>> p,
                shared_ptr<vector<string>> f)
    : _sought(s), _lines(p), _file(f) {}
    set<line_no>::iterator begin() {return _lines->begin();}
    set<line_no>::iterator end() {return _lines->end();}
    shared_ptr<vector<string>> get_file() {return _file;}
private:
    // 查询单词
    string _sought;
    // 出现的行号
    shared_ptr<set<line_no>> _lines;
    // 输入文件
    shared_ptr<vector<string>> _file;
};
//----------------------------------------------------------
QueryResult TextQuery::query(const string &sought) const{
    static shared_ptr<set<line_no>> nodata(new set<line_no>);
    auto loc = wm.find(sought);
    if (loc == wm.end()){
        return QueryResult(sought, nodata, file);
    } else {
        return QueryResult(sought, loc->second, file);
    }
}
//----------------------------------------------------------

ostream &print (ostream & os, const QueryResult &qr){
    /* 如果找到了单词，打印出现次数和所有出现的位置 */
    os << qr._sought << " ocurrs " << qr._lines->size()
        << " time" << (qr._lines->size() > 1 ? "s" : "") << endl;
    for (auto num : *qr._lines){
        os << "\t(line" << num + 1 << ") "
            << *(qr._file->begin() + num) << endl;
    }
    return os;
}
//----------------------------------------------------------

class Query_base{
    /* 抽象基类，具体的查讯类型从中派生 */
    friend class Query;
protected:
    // 起别名
    using line_no = TextQuery::line_no;
    // 析构函数作为虚函数
    // 是因为当用基类指针指向派生类对象的时候
    // 如果不声明虚函数删除指针只会调用基类的析构函数，派生类对象无法得到释放
    virtual ~Query_base() = default;
private:
    // 纯虚函数的声明
    // eval
    virtual QueryResult eval(const TextQuery&) const = 0;
    // rep
    virtual string rep() const = 0;
};
//----------------------------------------------------------

class Query {
    friend Query operator~(const Query &);
    friend Query operator|(const Query &, const Query &);
    friend Query operator&(const Query &, const Query &);
public:
    Query(const string&);
    // 接口函数：调用对应的 Query_base 操作
    QueryResult eval(const TextQuery &t) const{
        return q->eval(t);
    }
    string rep() const{
        return q->rep();
    }
private:
    Query(shared_ptr<Query_base> query) : q(query) {}
    shared_ptr<Query_base> q;
};
//----------------------------------------------------------

ostream & operator<<(ostream &os, const Query &query){
    return os << query.rep();
}

class WordQuery: public Query_base {
    friend class Query;
    WordQuery(const string &s): query_word(s){}
    QueryResult eval(const TextQuery &t) const{
        return t.query(query_word);
    } 
    string rep() const{
        return query_word;
    }
    string query_word;
};
//----------------------------------------------------------

inline Query::Query(const string &s) : q(new WordQuery(s)) {}

class NotQuery
: public Query_base{
    friend Query operator~(const Query &);
    NotQuery(const Query &q): query(q) {}
    string rep() const{
        return "~(" + query.rep() + ")";
    }
    QueryResult eval(const TextQuery&) const;
    Query query;
};
//----------------------------------------------------------

inline Query operator~(const Query &operand){
    return shared_ptr<Query_base>(new NotQuery(operand));
}
//----------------------------------------------------------

class BinaryQuery: public Query_base{
protected:
    BinaryQuery(const Query &l, const Query &r, string s)
    : lhs(l), rhs(r), opSym(s) {}
    string rep() const{
        return "(" + lhs.rep() + " " + opSym + " " + rhs.rep() + ")";
    }
    Query lhs, rhs;
    string opSym;
};
//----------------------------------------------------------

class AndQuery: public BinaryQuery{
    friend Query operator&(const Query &, const Query&);
    AndQuery(const Query &left, const Query &right)
    :BinaryQuery(left, right, "&") {}
    QueryResult eval(const TextQuery&) const;
};
//----------------------------------------------------------

inline Query operator&(const Query &lhs, const Query &rhs){
    return shared_ptr<Query_base>(new AndQuery(lhs, rhs));
}
//----------------------------------------------------------

class OrQuery: public BinaryQuery{
    friend Query operator|(const Query &, const Query&);
    OrQuery(const Query &left, const Query &right)
    :BinaryQuery(left, right, "|") {}
    QueryResult eval(const TextQuery&) const;
};
//----------------------------------------------------------

inline Query operator|(const Query &lhs, const Query &rhs){
    return shared_ptr<Query_base>(new OrQuery(lhs, rhs));
}
//----------------------------------------------------------

QueryResult OrQuery::eval(const TextQuery &text) const
{
    auto right = rhs.eval(text), left = lhs.eval(text);
    auto ret_lines = make_shared<set<line_no>>(left.begin(), left.end());
    ret_lines->insert(right.begin(), right.end());
    return QueryResult(rep(), ret_lines, left.get_file());
}
//----------------------------------------------------------

QueryResult AndQuery::eval(const TextQuery &text) const
{
    auto left = lhs.eval(text), right = rhs.eval(text);
    auto ret_lines = make_shared<set<line_no>>(left.begin(), left.end());
    set_intersection(left.begin(), left.end(),
                     right.begin(), right.end(),
                     inserter(*ret_lines, ret_lines->begin()));
    return QueryResult(rep(), ret_lines, left.get_file());
}
//----------------------------------------------------------

QueryResult NotQuery::eval(const TextQuery &text) const
{
    auto result = query.eval(text);
    auto ret_lines = make_shared<set<line_no>>();
    auto beg = result.begin(), end = result.end();
    auto sz = result.get_file()->size();
    for(size_t n = 0; n != sz; n++){
        if(beg == end || *beg != n){
            ret_lines->insert(n);
        } else if (beg != end){
            beg++;
        }
    }
    return QueryResult(rep(), ret_lines, result.get_file());
}
//----------------------------------------------------------

void test(){
    ifstream inputFile("The_Holy_Bible.txt");
    TextQuery t(inputFile);
    Query q = Query("little") & Query("shall");
    QueryResult res = q.eval(t);
    print(cout, res);
}
//----------------------------------------------------------

int main(int argc, char *argv[])
{
    test();
    return 0;
}

