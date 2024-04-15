#include <iostream>

#include "BDDSampler.h"
#include "JsonParser.h"
#include "cuddObj.hh"

using namespace std;
int main(int argc, char* argv[]) {
  string inputFile = argv[1];
  string outputFile;
  Cudd mgr;
  vars_vector vars;
  ast_vector asts;

  JsonParser jp = JsonParser(asts, vars, inputFile, outputFile);
  jp.inputParse();

  BDDSampler bs = BDDSampler(mgr, vars, asts);
  bs.vars2BDD();

  bs.allConstraints2BDD();

  bs.write_dot_file("", bs.allConstraintsBDD);

  return 0;
}