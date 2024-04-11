#include <iostream>
#include "cuddObj.hh"
#include "JsonParser.h"

using namespace std;
int main(int argc, char* argv[]) {
    string inputFile = argv[1];
    string outputFile;
    Cudd mgr;
    vars_vector vars;
    ast_vector asts;

    JsonParser jp = JsonParser(asts, vars, inputFile, outputFile);
    jp.inputParse();
    return 0;
}