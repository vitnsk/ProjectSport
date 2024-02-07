#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>
#include<locale>
#include <iomanip>

using namespace std;

class Sportman
{
private:
    int position = 0;
    int id = 0;
    double starttime = 0;
    double finishtime = 0;
    double resulttime = 0;
    string name;
    string surname;
    string resultmin;
    string ressecint;
    string ressecfr;

public:
    int get_position() { return position; }
    int get_id() { return id; }
    double get_starttime() { return starttime; }
    double get_finishtime() { return finishtime; }
    double get_resulttime() { return resulttime; }
    string get_name() { return name; }
    string get_surname() { return surname; }
    string get_resultmin() { return resultmin; }
    string get_ressecint() { return ressecint; }
    string get_ressecfr() { return ressecfr; }

    void set_position(int value) { position = value; }
    void set_id(int value) { id = value; }
    void set_starttime(double value) { starttime = value; }
    void set_finishtime(double value) { finishtime = value; }
    void set_resulttime(double value) { resulttime = value; }
    void set_name(string value) { name = value; }
    void set_surname(string value) { surname = value; }
    void set_resultmin(string value) { resultmin = value; }
    void set_ressecint(string value) { ressecint = value; }
    void set_ressecfr(string value) { ressecfr = value; }
};

class Sportres
{
private:
    int numer = 0;
    double resultat = 0;
    string flag;

public:
    int get_numer() { return numer; }
    double get_resultat() { return resultat; }
    string get_flag() { return flag; }
    void set_numer(int value) { numer = value; }
    void set_resultat(double value) { resultat = value; }
    void set_flag(string value) { flag = value; }
};

//Размечаеи JSON файл
vector<string> parseJson(string& str)
{
    vector<string> ret;

    string strdef = "ID:";
    for (int ib = 1, ie = 1; str.at(ie) != '}'; ib = ie + 1)
    {
        stack<char> stk;
        stk.push(str[ib]);

        for (ie = ib + 1; ie < str.size() && !stk.empty(); ++ie)
        {
            if (str[ie] == '}')
            {
                stk.pop();
            }
        }
        ret.push_back({ &str[ib], (unsigned)ie - ib });
    }
    return ret;
}

//Удаляем лишние пробелы в JSON файле
string removespace(string& s)
{
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == ' ' || s[i] == '\n')
        {
            s.erase(s.begin() + i);
            i--;
        }
    }
    return s;
}

// анализируем строку Json
void analisStr(vector<string>& vct, vector<char>& symb, vector<char>& symbput, const char& symbst, const char& symbend, int& ib, int& i)
{

    for (int k = 0, m = 0; k < vct[ib].size(); k++)
    {
        if (symb[k] == symbst)
        {
            for (int m = k + i; m < vct[ib].size(); m++)
            {
                if (symb[m + 1] == symbend)
                {
                    break;
                }
                symbput.push_back(symb[m]);
            }
        }
    }
}

//Анализируем JSON и записываем данные в vector
void analisJson(string& str, vector<Sportman>& sportmansLs)
{
    vector<char> symid;
    vector<char> symname;
    vector<char> symsurname;
    auto vct = parseJson(str);

    for (int ib = 0, ie = 1; ib < vct.size(); ib++)
    {
        Sportman sprtm;
        Sportman* spr = &sprtm;
        vector<char>  symb(vct[ib].begin(), vct[ib].end());
        string st;
        int i;
        //определяем нагудный номер
        for (int k = 0; k < vct[ib].size(); k++)
        {
            if (isdigit(symb[k]))
            {
                symid.push_back(symb[k]);
            }
        }
        // определяем name
        i = 7;
        analisStr(vct, symb, symname, 'N', ',', ib, i);
        //определяем surname
        i = 10;
        analisStr(vct, symb, symsurname, 'S', '}', ib, i);

        string idjson(symid.begin(), symid.end());
        string namejson(symname.begin(), symname.end());
        string surnamejson(symsurname.begin(), symsurname.end());

        spr->set_id(stoi(idjson));
        spr->set_name(namejson);
        spr->set_surname(surnamejson);

        symid.clear();
        symname.clear();
        symsurname.clear();

        sportmansLs.push_back(*spr);
    }
    vct.clear();
}

//проверяем цифры в строке
int digital(string& num)
{
    int resultnum;
    size_t begin = num.find_first_of("0123456789");
    size_t end = num.find_last_of("0123456789");
    string number = num.substr(begin, end - begin + 1);
    resultnum = atoi(number.c_str());
    return resultnum;
}

//анализ текстового файла
void analisTxt(ifstream& in, vector<Sportres>& sportresLs)
{
    string line;
    while (getline(in, line))
    {
        Sportres sprtres;
        Sportres* spres = &sprtres;

        string startend, restime;
        string num;
        istringstream iss(line);

        iss >> num >> startend >> restime;
        istringstream my_stream(restime);

        string token;
        string  hoursp, minutesp, secsp;

        double timesport = 0;
        size_t pos = -1;
        //заменяем скобки пробелами
        while (my_stream >> token) {
            while ((pos = token.rfind(':'))
                != string::npos) {
                token.replace(pos, 1, " ");
            }
            istringstream streamTime(token);
            streamTime >> hoursp >> minutesp >> secsp;
            timesport = stod(hoursp) + stod(minutesp) / 60 + stod(secsp) / 36000;
        }
        spres->set_numer(digital(num));
        spres->set_flag(startend);
        spres->set_resultat(timesport);
        sportresLs.push_back(*spres);
    }
}

// Условие сортировки
bool lessresult(Sportman& left, Sportman& right)
{
    return left.get_resulttime() < right.get_resulttime();
}

// преобразование и сортировка
void  vectorTransf(vector<Sportman>& sportmansLs, vector<Sportres>& sportresLs)
{
    for (int ib = 0; ib < sportresLs.size(); ib++)
    {
        for (int im = 0; im < sportmansLs.size(); im++)
        {

            if (sportmansLs[im].get_id() == sportresLs[ib].get_numer())
                if (sportresLs[ib].get_flag() == "start")
                {
                    sportmansLs[im].set_starttime(sportresLs[ib].get_resultat());
                }
                else if (sportresLs[ib].get_flag() == "finish")
                {
                    sportmansLs[im].set_finishtime(sportresLs[ib].get_resultat());
                }
        }

    }
    // получаем время участников
    for (int im = 0; im < sportmansLs.size(); im++)
    {
        sportmansLs[im].set_resulttime(sportmansLs[im].get_finishtime() - sportmansLs[im].get_starttime());
    }
    //сортируем по возрастанию
    sort(sportmansLs.begin(), sportmansLs.end(), lessresult);

    //сдвигаем в конец списка с нулевым временем
    Sportman nullres;
    for (int k = 0; k < sportmansLs.size(); k++)
    {
        if (sportmansLs[k].get_resulttime() == 0)
        {
            nullres = sportmansLs[k];
            sportmansLs.erase(sportmansLs.begin() + k);
            sportmansLs.push_back(nullres);
        }
    }

    // выставляем места
    for (int k = 0; k < sportmansLs.size(); k++)
    {
        sportmansLs[k].set_position(k + 1);
    }
}
// устанавливаем формат мм:сс
string timeDigFormat(int dig)
{
    char digchar[3];
    sprintf_s(digchar, "%02d", dig); ;
    return digchar;
}
//формат времени
void timeFormat(vector<Sportman>& sportmansLs, vector<Sportres>& sportresLs)
{
    for (int im = 0; im < sportmansLs.size(); im++)
    {
        int timesec;
        int sphour;
        int spminute;
        int secint;
        double stime;
        double  timesecfr;
        double  timesecint;
        double spsecund;
        double rsecund;
        string stsecint, stsecfr, stminint;

        stime = sportmansLs[im].get_resulttime() * 3600;
        timesecfr = modf(stime, &timesecint);
        timesec = (int)timesecint;

        sphour = timesec / 3600;
        spminute = (timesec % 3600) / 60;
        secint = timesec % 60;
        spsecund = (double)(timesec % 60) + timesecfr;
        rsecund = round(spsecund * 100) / 100;
       
        stsecint = timeDigFormat(secint);
        stsecfr = timeDigFormat((int)round(timesecfr * 100));
        stminint = timeDigFormat(spminute);

        sportmansLs[im].set_ressecint(stsecint);
        sportmansLs[im].set_ressecfr(stsecfr);
        sportmansLs[im].set_resultmin(stminint);
    }
}

//вывод на консоль
void consolePut(vector<Sportman>& sportmansLs)
{
    cout << '|' << "Занятое место" << '|' << "Нагрудный номер" << '|' << "Имя" << setw(5) << '|' << "Фамилия" << setw(8) << '|' << "Результат" << setw(5) << '|' << endl;
    cout << '|' << "-----------" << '|' << "--------------" << '|' << "-------" << '|' << "----------" << '|' << "-------" << '|' << endl;

    for (int im = 0; im < sportmansLs.size(); im++)
    {
        cout << '|' << setw(5) << sportmansLs[im].get_position() << setw(8) << '|';
        cout << setw(5) << sportmansLs[im].get_id() << setw(5) << '|';
        cout << setw(5) << sportmansLs[im].get_surname() << setw(10) << '|';
        cout << setw(5) << sportmansLs[im].get_name() << setw(8) << '|';
        cout << setw(5) << sportmansLs[im].get_resultmin() << ':' << sportmansLs[im].get_ressecint() << ',' << sportmansLs[im].get_ressecfr();
        cout << setw(5) << '|' << endl;
    }
}

//вывод в файл
void filePut(vector<Sportman>& sportmansLs)
{
    ofstream out("..\\list.json");

    if (out.is_open()) {
        out  << "{" << endl;
        for (int ib = 0; ib < sportmansLs.size(); ib++) {           
            out << setw(2) << '\"' << sportmansLs[ib].get_position() << '\"';
            out << ":{" << endl;
            out << "       \"Нагрудный знак\":" << sportmansLs[ib].get_id() << '\"' << ',' << endl;
            out << "       \"Имя\":" << '\"' << sportmansLs[ib].get_surname() << '\"' << ',' << endl;
            out << "       \"Фамилия\":" << '\"' << sportmansLs[ib].get_name() << '\"' << ',' << endl;
            out << "       \"Результат\":" << '\"' << sportmansLs[ib].get_resultmin() << ':' << sportmansLs[ib].get_ressecint() << ',' << sportmansLs[ib].get_ressecfr() << '\"' << endl;
            out << setw(4) << "}";
            if (ib != sportmansLs.size() - 1) {
                out << ",";
            }
            out<< endl;
        }
        out << "}" << endl;
        out.close();
    }
    else {
        cerr << "Unable to open out file list.json \n";
    }
}

int main() {
    setlocale(LC_ALL, "Russian.utf8");
    vector<Sportman> sportmansLs;
    vector<Sportres> sportresLs;
    string idjson, namejson, surnamejson;

    ifstream ifs("..\\competitors2.json"); // окрываем json файл для чтения
    if (ifs.is_open())
    {
        string str{ istreambuf_iterator<char>{ ifs.rdbuf()}, {} };
        ifs.close();
        str = removespace(str);
        analisJson(str, sportmansLs);
    }
    else {
        std::cerr << "Unable to open file competitors2.json \n";
    }
    ifstream in("..\\results_RUN.txt"); // окрываем results_RUN.txt файл для чтения
    if (in.is_open())
    {
        analisTxt(in, sportresLs);
        in.close();
    }
    else {
        cerr << "Unable to open file results_RUN.txt \n";
    }
    vectorTransf(sportmansLs, sportresLs);
    timeFormat(sportmansLs, sportresLs);
    consolePut(sportmansLs);
    filePut(sportmansLs);
}

