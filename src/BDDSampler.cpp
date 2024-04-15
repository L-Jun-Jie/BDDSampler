#include "BDDSampler.h"

using myEnums::Operation;

void BDDSampler::vars2BDD() {
  for (auto& var : vars) {
    std::vector<BDD> tmp;
    tmp.reserve(var.bit_width);
    for (int i = 0; i < var.bit_width; ++i) {
      tmp.push_back(mgr.bddVar());
    }
    bdd_vector.push_back(tmp);
  }
}

BitVec BDDSampler::getBitVec(jsons::AST* t) {
  switch (t->op) {
    case Operation::VAR:
      return bdd_vector[t->id];
    case Operation::CONST: {
      std::string tmp_width;
      std::string tmp_value;
      bool is_value = false;

      for (auto c : t->value) {
        if (c == '\'' || c == 'h') {
          is_value = true;
          continue;
        }
        if (!is_value)
          tmp_width += c;
        else
          tmp_value += c;
      }

      int bit_width = stoi(tmp_width);
      int bit_value = std::stoi(tmp_value, 0, 16);
      std::cout << "常量的十进制值：" << bit_value << std::endl;
      if (bit_value < 0) {
        std::cout << "can not use getBitVec for negative values !" << std::endl;
        exit(0);
      }

      BitVec var;
      // 获得常量对应的位向量
      for (size_t i = 0U; i < bit_width; ++i) {
        if (bit_value & 1U) {
          var.push_back(mgr.bddOne());
        } else {
          var.push_back(mgr.bddZero());
        }
        bit_value >>= 1U;
      }
    }
  }
}

void BDDSampler::calculate_constraints(jsons::AST* t, BitVec& res) {
  if (t == nullptr)
    return;
  else if (t->op == Operation::VAR || t->op == Operation::CONST) {
    res = getBitVec(t);
  } else {
    BitVec lhs, rhs;
    calculate_constraints(t->lhs, lhs);
    calculate_constraints(t->rhs, rhs);
    switch (t->op) {
      case Operation::LOG_AND:
        logic_and(lhs, rhs, res);
        break;
      case Operation::LOG_OR:
        logic_or(lhs, rhs, res);
        break;
      case Operation::LOG_NEG:
        logic_neg(lhs, res);
        break;
      default:
        std::cout << "This operation type is not supported！" << std::endl;
        exit(0);
    }
  }
}

/* Constructing constraints as a BDD */
BDD BDDSampler::singleConstraint2BDD(jsons::AST& t) {
  BDD singleConstraintResult;
  BitVec tmp;
  calculate_constraints(&t, tmp);
  singleConstraintResult = mgr.bddZero();
  for (const auto& bdd : tmp) {
    singleConstraintResult = singleConstraintResult | bdd;
  }
  singleConstraintBDDs.push_back(singleConstraintResult);
  return singleConstraintResult;
}

BDD BDDSampler::allConstraints2BDD() {
  BDD allConstraintsResult = mgr.bddOne();
  for (auto& t : asts) {
    BDD singleConstraintResult = singleConstraint2BDD(t);
    allConstraintsResult *= singleConstraintResult;
  }
  allConstraintsBDD = allConstraintsResult;
  return allConstraintsResult;
}

/* Write to dot file */
void BDDSampler::write_dot_file(std::string dotFileDir, BDD& bdd) {
  std::string filePath = dotFileDir + "OutPutFiles/OutPutFile" +
                         std::to_string(dotFileIndex++) + ".dot";
  FILE* file = fopen(filePath.c_str(), "wb");  // writes to binary file
  if (file == nullptr) {
    printf("Error opening file: %s\n", strerror(errno));
  }

  auto** ddNodeArray = static_cast<DdNode**>(malloc(sizeof(DdNode*)));
  ddNodeArray[0] = bdd.getNode();
  Cudd_DumpDot(mgr.getManager(), 1, ddNodeArray, nullptr, nullptr, file);
  free(ddNodeArray);

  fclose(file);
}

/* Logical Operation */

void BDDSampler::logic_and(BitVec& lhs, BitVec& rhs, BitVec& res) {
  auto l_exp = lhs;
  auto r_exp = rhs;

  BitVec tmp1;
  BitVec tmp2;

  // 检测 lhs 是否为真
  for (size_t i = 0; i < l_exp.size(); ++i) {
    if (i != 0)
      tmp1.push_back(tmp1[i - 1] | l_exp[i]);
    else
      tmp1.push_back(l_exp[i]);
  }

  // 检测 rhs 是否为真
  for (size_t i = 0; i < r_exp.size(); ++i) {
    if (i != 0)
      tmp2.push_back(tmp2[i - 1] | r_exp[i]);
    else
      tmp2.push_back(r_exp[i]);
  }

  res.push_back(tmp1[l_exp.size() - 1] * tmp2[r_exp.size() - 1]);
}

void BDDSampler::logic_or(BitVec& lhs, BitVec& rhs, BitVec& res) {
  auto l_exp = lhs;
  auto r_exp = rhs;

  BitVec tmp1;
  BitVec tmp2;

  // 检测 lhs 是否为真
  for (size_t i = 0; i < l_exp.size(); ++i) {
    if (i != 0)
      tmp1.push_back(tmp1[i - 1] | l_exp[i]);
    else
      tmp1.push_back(l_exp[i]);
  }

  // 检测 rhs 是否为真
  for (size_t i = 0; i < r_exp.size(); ++i) {
    if (i != 0)
      tmp2.push_back(tmp2[i - 1] | r_exp[i]);
    else
      tmp2.push_back(r_exp[i]);
  }

  res.push_back(tmp1[l_exp.size() - 1] | tmp2[r_exp.size() - 1]);
}

void BDDSampler::logic_neg(BitVec& lhs, BitVec& res) {
  BitVec l_exp;

  BDD tmp = mgr.bddZero();
  for (auto& bdd : l_exp) {
    tmp = tmp | bdd;
  }
  res.push_back(~tmp);
}