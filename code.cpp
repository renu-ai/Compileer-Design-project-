#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>

using namespace std;

     // recursive tree logic of my checkCalculation function
     // for ex : B + A + C * D
     // tree logic of this ↙
     //          +
     //        /   \  
     //      B       +
     //            /  \
     //          A     *
     //              /   \
     //             C     D
     //

string checkCalculation(string str, map<string,string>&vars);
string checkIfElse(string str, map<string,string>&vars);

// remove all white spaces
string trimString(string s) {
    int lefti = 0,righti = s.size()-1;
    while (s[lefti] == ' ') lefti++;
    while (s[righti] == ' ') righti--;
    return s.substr(lefti, righti-lefti+1);
}

bool includesOperator(string s) {
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == '+' || s[i] == '/' || s[i] == '-' || s[i] == '*') return true;
    }
    return false;
}

bool isIfElse(string str) {
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == '>' || str[i] == '<' || str[i] == '=' || str[i] == '!') return true;
    } return false;
}

string checkLoop(string str, map<string,string> &vars, string key) {
    //check if the key exists in our variables map
    if (vars.find(key) == vars.end()) vars[key] = "0";
    int index = str.find("TIMES");
    string left = trimString(str.substr(0,index));
    int count = stoi(trimString(str.substr(index+5)));
    for (int i = 0; i < count; ++i) {
        vars[key] = str.find("IF") != string::npos ? checkIfElse(left, vars) : checkCalculation(left, vars);
    } return "";
}


string checkIfElse(string str, map<string,string> &vars) {
    int index = str.find("IF");
    int index2 = str.find("THEN",index+1);
    int index3 = str.find("ELSE",index2+1);

    string condition = trimString(str.substr(index+2,index2 - index - 2));
    string ifRes = trimString(str.substr(index2+4,index3 - index2 - index-4));
    string elseRes = trimString(str.substr(index3+4));
    string left,right,op = ">=";

    ifRes = includesOperator(ifRes) ? checkCalculation(ifRes, vars) : vars[ifRes];
    elseRes = includesOperator(elseRes) ? checkCalculation(elseRes, vars) : vars[elseRes];

    // find the operator
    index = condition.find(">=");
    if (index == string::npos) {index = condition.find(">");op = ">";}
    if (index == string::npos) {index = condition.find("<=");op = "<=";}
    if (index == string::npos) {index = condition.find("<");op = "<";}
    if (index == string::npos) {index = condition.find("==");op = "==";}
    if (index == string::npos) {index = condition.find("!=");op = "!=";}

    if (index != string::npos) {
        string left_side = trimString(condition.substr(0,index)); 
        string right_side = trimString(condition.substr(index+op.size()));

        if (vars.find(left_side) != vars.end()) left = vars[left_side];
        else if (includesOperator(left_side)) left = checkCalculation(left_side, vars);
        else left = left_side;
        if (vars.find(right_side) != vars.end()) right = vars[right_side];
        else if (includesOperator(right_side)) right = checkCalculation(right_side, vars);
        else right = right_side;

        if (op == ">") return (stod(left) > stod(right) ? ifRes : elseRes);
        else if (op == "<") return stod(left) < stod(right) ? ifRes : elseRes;
        else if (op == "<=") return stod(left) <= stod(right) ? ifRes : elseRes;
        else if (op == ">=") return stod(left) >= stod(right) ? ifRes : elseRes;
        else if (op == "==") return stod(left) == stod(right) ? ifRes : elseRes;
        else if (op == "!=") return stod(left) != stod(right) ? ifRes : elseRes;
    } return "";
}


string checkCalculation(string str, map<string,string>&vars)
{
    bool intCalc = false;
    string left,right;
    // decide the operator and calculation class with priority
    // paranthese priority
    int index = str.find("(");
    while (index != string::npos) {
        int index2 = str.rfind(")");
        str.replace(index, index2-index+1, checkCalculation(str.substr(index+1,index2-index-1), vars));
        index = str.find(str, index+1);
    }
    // continue calculation without parantheses
    char op = '+';
    index = str.find("+");
    if (index == string::npos) {index = str.find("-");op='-';}
    if (index == string::npos) {index = str.find("/");op='/';}
    if (index == string::npos) {index = str.find("*");op='*';}
    // check if there is a math calc
    if (index != string::npos)
    {
        string left_side = trimString(str.substr(0,index));
        string right_side = trimString(str.substr(index + 1));

        // check left side
        if (vars.find(left_side) != vars.end()) left = vars[left_side];     // if it exists in my map
        else if (includesOperator(left_side)) left = checkCalculation(left_side, vars);  // recursive - tree logic
        else left = left_side;
        // check right side
        if (vars.find(right_side) != vars.end()) right = vars[right_side];      // if it exists in my map
        else if (includesOperator(right_side)) right = checkCalculation(right_side, vars);  
        else right = right_side;
        
        if (left.find(".") == string::npos && right.find(".") == string::npos) intCalc = true ;     // check if the calc is between integers or doubles

        switch (op) {
            case '+':
            return to_string(intCalc ? stoi(left) + stoi(right) : stod(left) + stod(right)); break;
            case '-':
            return to_string(intCalc ? stoi(left) - stoi(right) : stod(left) - stod(right)); break;
            case '*':
            return to_string(intCalc ? stoi(left) * stoi(right) : stod(left) * stod(right)); break;
            case '/':
            return to_string(intCalc ? stoi(left) / stoi(right) : stod(left) / stod(right)); break;
        }
    } 
    return trimString(str);
};


void checkVariable(string str, map<string,string>&vars) {
    int index = str.find("=");
    if (index != string::npos) {
        // it's an assignment
        int loopind = str.find("LOOP");  // if there is a loop
        string key = loopind != string::npos ? trimString(str.substr(loopind+4,index-loopind-4)) : trimString(str.substr(0,index));
        if (loopind != string::npos) checkLoop(str.substr(index + 1), vars, key);
        else vars[key] = str.find("IF") != string::npos ? checkIfElse(str.substr(index + 1), vars) :  checkCalculation(str.substr(index + 1),vars);  // check if it's a calculation assignment and store the val
    }
};


// check if we are done
bool checkOutput(string str) {return str.find("OUT", 0) != string::npos ? true : false;}


int main() {
    map<string,string> vars;
    string str;

    //read and process the input
    ifstream input;
    input.open("<file name>.inp");
    while (getline(input,str)) {
        if (checkOutput(str)) break;
        checkVariable(str, vars);
        // checkIfElse(str, vars);
    }
    input.close();

    // checking if i get the vars correctly
    // for (auto a : vars) {
    //     cout << a.first << " ==== " << a.second << endl; 
    // }

    // print the output
    auto output = vars[trimString(str.substr(str.find("OUT")+3))];
    cout << "output : " << output << endl; // log the output
    ofstream ofile;
    ofile.open("output.out");
    ofile << output;
    ofile.close();

    // system("Pause");

    return 0;
}
