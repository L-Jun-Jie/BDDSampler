#ifndef BDDSAMPLER_H
#define BDDSAMPLER_H
#include <string>

#include "JsonParser.h"
#include "cuddObj.hh"

typedef std::vector<BDD> BitVec;

class BDDSampler {
 private:
  Cudd& mgr;
  vars_vector vars;
  ast_vector asts;
  std::vector<BitVec> bdd_vector;
  std::vector<BDD> singleConstraintBDDs;

  int dotFileIndex = 0;

 public:
  BDD allConstraintsBDD;
  BDDSampler(Cudd& _mgr, vars_vector& _vars, ast_vector& _asts)
      : mgr(_mgr), vars(_vars), asts(_asts) {}

  void vars2BDD();

  void calculate_constraints(jsons::AST* t, BitVec& res);

  BitVec getBitVec(jsons::AST* t);

  /* Constructing a single constraint as a BDD */
  BDD singleConstraint2BDD(jsons::AST& t);
  BDD allConstraints2BDD();

  /* Write to dot file */
  void write_dot_file(std::string dotFileDir, BDD& bdd);

  /* Logical Operation */
  void logic_and(BitVec& lhs, BitVec& rhs, BitVec& res);
  void logic_or(BitVec& lhs, BitVec& rhs, BitVec& res);
  void logic_neg(BitVec& lhs, BitVec& res);
};

#endif