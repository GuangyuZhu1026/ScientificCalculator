//
//  main.cpp
//  calculator
//
//  Created by Guangyu Zhu on 05/03/2017.
//  Copyright © 2017 SDL_tutorial. All rights reserved.
//

#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <cmath>
#include <unordered_map>
using namespace std;

unordered_map<string, double> mMap;

bool invalid = false;
bool foundVarable = false;
bool hasVariable = false;
bool error1 = false;//for unclearify variablr
bool error2 = false;//for division by zero

int getPrecedence (string sign){
    int precedence;
    if (sign=="+") precedence = 1;
    else if (sign=="-") precedence = 1;
    else if (sign=="*") precedence = 2;
    else if (sign=="/") precedence = 2;
    else if (sign=="^") precedence = 3;
    return precedence;
}

//check if a number is variable, if it is a variable, it contains no number
bool isVariable( string number){
    bool hasNum = false;
    for (int i = 0; i<number.size(); i++) {
        char curr = number[i];
        if ('0' <= curr && curr <= '9') {
            hasNum = true;
            break;
        }
    }
    if (hasNum) {
        return false;
    }
    else {
        hasVariable = true;
        return true;
    }
}

/*function used in makequeue function to find the real number and push it into the queue
 sometimes the num will contain sin**** cos***, do the calculation, result in realNum
 if it is a variabe, find variable in map, if not find such variable, undeclear variable error occur
 Do: convert Pi, e; do sin/cos/log calculation*/
string findRealNum(string number){
    string realNum;
    long numLength = number.size();
    string firstThreeChar;
    string numInFunc;
    double num;
    
    //if it's not a variable, either sin/cos/log or number itself
    if (!isVariable(number)) {
        if (numLength <3) {
            realNum = number;
        }
        else if (numLength >=3){
            //string function is either sin/cos/log
            for (int i=0; i<3; i++) {
                firstThreeChar += number[i];
            }
            if (firstThreeChar == "sin" || firstThreeChar=="cos" || firstThreeChar =="log") {
                for (int i=3; i<number.size(); i++){
                    numInFunc +=number[i];
                }
                if (firstThreeChar == "sin"){
                    num = sin(stod(numInFunc));
                    realNum = to_string(num);
                }
                else if (firstThreeChar == "cos"){
                    num = cos(stod(numInFunc));
                    realNum = to_string(num);
                }
                else if (firstThreeChar == "log"){
                    num = log(stod(numInFunc));
                    realNum = to_string(num);
                }
            }
            else realNum = number;
        }
    }
    
    //if it is a variable
    else if (isVariable(number)){
        if (mMap.find(number) == mMap.end()){
            foundVarable = false;
        }
        else{
            foundVarable = true;
            realNum = to_string(mMap[number]);
        }
    }
    return realNum;
}

/*for making the intial queue contain tokens with numbers and operations, return type is queue
 here, number is splited by + - * /, anything between is a number*/
queue <string> makeQueue(queue<string>& input){
    queue<string> q;
    string number;
    string sign;
    while (!input.empty()) {
        string a = input.front();
        if (a!="+" && a!="-" && a!="*" && a!="/" && a!= "^" && a!=" "){
            number += a;
        }
        else if (a=="+" || a=="-" || a=="*" || a=="/" || a=="^"){
            number = findRealNum(number);
            q.push(number);
            number="";
            sign = a;
            q.push(sign);
            sign = "";
        }
        input.pop();
        //when input is empty, there might be a number inside num, we need to push it to queue q
        if (input.empty() && !number.empty()) {
            number = findRealNum(number);
            q.push(number);
        }
    }
    return q;
}


//function to make the postfix string
queue <string> postFixQueue(queue <string>& stringToOrganize){
    stack<string> op;//store operation
    queue<string> postfix;//store final result
    string front;
    
    while(!stringToOrganize.empty()){
        front = stringToOrganize.front();
        stringToOrganize.pop();
        
        if (front!="+" && front!="-" && front!="*" && front!="/" && front!= "^"){
            postfix.push(front);
        }
        
        else if (front=="+" || front=="-" || front=="*" || front=="/" || front== "^"){
            if (op.empty()){
                op.push(front);
            }
            else if (!op.empty()){
                string top = op.top();
                //when there are to consecutive ^ sign, precedure is different
                if(getPrecedence(top) == 3 && getPrecedence(front) == 3){
                    op.push(front);
                }
                //when there's no consecutive ^ sign
                else{
                    if (getPrecedence(top)<getPrecedence(front))
                        op.push(front);
                    else if (getPrecedence(top)>=getPrecedence(front)){
                        while (getPrecedence(top)>=getPrecedence(front)){
                            string temp = op.top();
                            postfix.push(temp);
                            op.pop();
                            if (!op.empty())  top = op.top();
                            else break;
                        }
                        op.push(front);
                    }
                }
            }
        }
        
        //ckeck after last operand is put in string if stack is empty
        //if not, put the left opereations into postfix string
        if (stringToOrganize.empty()){
            while (!op.empty()){
                string temp = op.top();
                postfix.push(temp);
                op.pop();
            }
        }
    }
    return  postfix;
}

//do calculation from postfix string
double doMath(queue<string>& postfix){
    //final result stored in result
    double result;
    double tempResult;
    double first;
    double second;
    stack<double> oprand;
    
    while (!postfix.empty()){
        string curr = postfix.front();
        postfix.pop();
        if (curr!="+" && curr!="-" && curr!="*" && curr!="/" && curr!= "^"){
            double num = stod(curr);
            oprand.push(num);
        }
        else{
            char operation = curr.at(0);
            switch(operation){
                case '+':
                    first = oprand.top();
                    oprand.pop();
                    second = oprand.top();
                    oprand.pop();
                    tempResult = second + first;
                    oprand.push(tempResult);
                    break;
                    
                case '-':
                    first = oprand.top();
                    oprand.pop();
                    second = oprand.top();
                    oprand.pop();
                    tempResult = (double)second - first;
                    oprand.push(tempResult);
                    break;
                    
                case '*':
                    first = oprand.top();
                    oprand.pop();
                    second = oprand.top();
                    oprand.pop();
                    tempResult = (double)second * first;
                    oprand.push(tempResult);
                    break;
                    
                case '/':
                    first = oprand.top();
                    if (first ==0){
                        invalid = true;
                        break;
                    }
                    oprand.pop();
                    second = oprand.top();
                    oprand.pop();
                    tempResult = (double)second / first;
                    oprand.push(tempResult);
                    break;
                    
                case '^':
                    first = oprand.top();
                    oprand.pop();
                    second = oprand.top();
                    oprand.pop();
                    tempResult = (double)pow(second, first);
                    oprand.push(tempResult);
                    break;
                    
            }
        }
    }
    
    result = oprand.top();
    return result;
}

int main(){
    string quit = "quit";
    mMap["Pi"] = 3.14169;
    mMap["e"] = 2.718;
    
    //store user input into a string called input
    string input;
    getline(cin, input);
    int i, j;
    
    while (input.compare(quit) != 0){
        //--------------check if it's let statement ---------------------------------------
        string firstFourChar = "";
        string varName = "";
        string assignExpression = "";
        //check the first four char of input if it's "let "
        for (i =0; i<4; i++) {
            firstFourChar += input[i];
        }
        
        //-----------------if it is let statement---------------------------------------------
        if (firstFourChar == "let "){
            for (i = 4; i<input.size(); i++) {
                char k = input [i];
                if ( k== '='){
                    break;
                }
                else if(k != ' '){
                    varName += input[i];
                }
            }
            for (j=i+1; j<input.size(); j++){
                if (input[j] != ' ') {
                    assignExpression += input[j];
                }
            }
            //---------do the assignexpression first and map the result into the map-----------------------------
            queue<string> firststore;
            string sign;
            stack<string>  S;
            string substring;
            stack<string> temp;
            queue<string> tempExp;
            for(int i=0;i<assignExpression.length();i++){
                
                if (assignExpression[i]!='+' && assignExpression[i]!='-' && assignExpression[i]!='*' && assignExpression[i]!='/' && assignExpression[i]!=' ' && assignExpression[i]!='^' && assignExpression[i] !='(' && assignExpression[i]!=')') {
                    substring+=assignExpression[i];
                }
                else if(assignExpression[i]!='+' || assignExpression[i]!='-' || assignExpression[i]!='*' || assignExpression[i]!='/' || assignExpression[i]!='^' || assignExpression[i] !='(' || assignExpression[i]!=')'){
                    if (substring!="") {
                        firststore.push(substring);
                    }
                    substring="";
                    sign += assignExpression[i];
                    if (sign!="") {
                        firststore.push(sign);
                    }
                    sign = "";
                }
                if (i ==assignExpression.length()-1 && !substring.empty()) {
                    firststore.push(substring);
                }
            }
            
            while (!firststore.empty()) {
                if (firststore.front() != ")") {
                    S.push(firststore.front());
                    firststore.pop();
                }
                else if (firststore.front() == ")"){
                    firststore.pop();
                    while (S.top() != "(") {
                        temp.push(S.top());//括号中的式子 in reverse order
                        S.pop();
                    }
                    S.pop(); //pop out the '(' in stack
                    while (!temp.empty()) {//把式子倒过来
                        tempExp.push(temp.top());
                        temp.pop();
                    }
                    //do the math in parentheses
                    queue<string> stringToOrganize = makeQueue(tempExp);
                    
                    while (!tempExp.empty()) {
                        tempExp.pop();
                    }  //reset tempExp to nothing
                    
                    if (hasVariable && !foundVarable) {
                        break;
                    }
                    else{
                        queue <string> postfix = postFixQueue(stringToOrganize);
                        double result = doMath(postfix);
                        if (invalid) {
                            break;
                        }
                        else{
                            string pushResult = to_string(result);
                            S.push(pushResult);
                        }
                    }
                }
            }
            if (hasVariable && !foundVarable) {
                error1 = true;
                hasVariable = false;
                foundVarable = false;
            }
            
            if (invalid) {
                error2 = true;
                invalid = false;
            }
            
            // if above calculation does not contain division by zero or undeclear variablr error, do final calculation
            else{
                queue<string> finalString;
                while (!S.empty()) {
                    temp.push(S.top());
                    S.pop();
                }
                while (!temp.empty()) {
                    finalString.push(temp.top());
                    temp.pop();
                }
                
                //first split inout into numbers and operation signs and store into queue
                queue<string> stringToOrganize = makeQueue(finalString);
                
                if (hasVariable && !foundVarable) {
                    error1 = true;
                    hasVariable = false;
                    foundVarable = false;
                }
                
                else{
                    //take the infix queue and organize it into postfix queue
                    queue <string> postfix = postFixQueue(stringToOrganize);
                    //evaluate the postfix queue and do the math operation
                    double result = doMath(postfix);
                    //error handling1: division by zero
                    if (invalid) {
                        error2 = true;
                        invalid = false;
                    }
                    else{
                        mMap [varName] = result;
                    }
                }
            }
        }
        //---if it's not "let" statement, just do calculation----------------------------------
        else{
            //do parentheses first
            queue<string> firststore;
            string sign;
            stack<string>  S;
            string substring;
            stack<string> temp;
            queue<string> tempExp;
            
            for(int i=0;i<input.length();i++){
                if (input[i]!='+' && input[i]!='-' && input[i]!='*' && input[i]!='/' && input[i]!='^' && input[i]!=' ' && input[i] !='(' && input[i]!=')') {
                    substring+=input[i];
                }
                else if(input[i]!='+' || input[i]!='-' || input[i]!='*' || input[i]!='/' || input[i]!='^'|| input[i] !='(' || input[i]!=')'){
                    if (substring!="") {
                        firststore.push(substring);
                    }
                    substring="";
                    sign += input[i];
                    if (sign!="") {
                        firststore.push(sign);
                    }
                    sign = "";
                }
                if (i ==input.length()-1 && !substring.empty()) {
                    firststore.push(substring);
                }
            }
            
            //do parentheses first
            while (!firststore.empty()) {
                if (firststore.front() != ")") {
                    S.push(firststore.front());
                    firststore.pop();
                }
                else if (firststore.front() == ")"){
                    firststore.pop();
                    while (S.top() != "(") {
                        temp.push(S.top());//括号中的式子 in reverse order
                        S.pop();
                    }
                    S.pop(); //pop out the '(' in stack
                    while (!temp.empty()) {//把式子倒过来
                        tempExp.push(temp.top());
                        temp.pop();
                    }
                    //do the math in parentheses
                    queue<string> stringToOrganize = makeQueue(tempExp);
                    
                    while (!tempExp.empty()) {
                        tempExp.pop();
                    }  //reset tempExp to nothing
                    
                    if (hasVariable && !foundVarable) {
                        break;
                    }
                    else{
                        queue <string> postfix = postFixQueue(stringToOrganize);
                        double result = doMath(postfix);
                        if (invalid) {
                            break;
                        }
                        else{
                            string pushResult = to_string(result);
                            S.push(pushResult);
                        }
                    }
                }
            }
            if (hasVariable && !foundVarable) {
                cout << "Undecleared-Variable"<<endl;
                hasVariable = false;
                foundVarable = false;
            }
            
            if (invalid) {
                cout << "Division-By-Zero"<<endl;
                invalid = false;
            }
            
            // if above calculation does not contain division by zero or undeclear variablr error, do final calculation
            else{
                queue<string> finalString;
                while (!S.empty()) {
                    temp.push(S.top());
                    S.pop();
                }
                while (!temp.empty()) {
                    finalString.push(temp.top());
                    temp.pop();
                }
                
                //first split inout into numbers and operation signs and store into queue
                queue<string> stringToOrganize = makeQueue(finalString);
                
                if (hasVariable && !foundVarable) {
                    cout << "Undecleared-Variable"<<endl;
                    hasVariable = false;
                    foundVarable = false;
                }
                
                else{
                    //take the infix queue and organize it into postfix queue
                    queue <string> postfix = postFixQueue(stringToOrganize);
                    //evaluate the postfix queue and do the math operation
                    double result = doMath(postfix);
                    //error handling1: division by zero
                    if (invalid) {
                        cout << "Division-By-Zero"<<endl;
                        invalid = false;
                    }
                    else{
                        cout<<result<<endl;
                    }
                }
            }
        }
        getline(cin, input);
    }
    return 0;
}
