/**
 * Ricky Palaguachi
 * CS280
 * Program 3 
 * main.cpp
*/
#include <iostream>
#include <fstream>
#include "parse.h"
#include "lex.h"
using std::ifstream;

using namespace std;


void traverse(ParseTree* node) {
    //null node
    if (node == 0)
        return;
    //traverse down the tree
    if (node->getLeft() != 0) {
        traverse(node->getLeft());
    }
    if (node->getRight() != 0) {
        traverse(node->getRight());
    }
}


ParseTree* Parse(istream& in, int startLine){
    int & line = startLine;
    ParseTree* prog = Prog(in, line);
    return prog;
}

int main(int argc, char *argv[]){
    istream *in = &cin;
    ifstream file;

    //Validate command line args
    if (argc > 2) {
        cout << "TOO MANY FILENAMES" << endl;
        return 0;
    }
    else if (argc == 2){
        string fileName = argv[1];
        file.open(fileName);

        if( file.is_open() == false )  {
            cerr << "COULD NOT OPEN " << fileName << endl;
            return 0; 
        }

        in = &file;
    }

    //begin parsing file
    ParseTree* tree = Parse(*in, 0);

    if (tree == 0)
    {
        return 0;
    }


    return 0;
}//main()


