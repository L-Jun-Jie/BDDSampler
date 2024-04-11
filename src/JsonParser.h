#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <iostream>
#include <string>

#include "BitVec.h"
#include "MyEnums.h"
#include "cuddObj.hh"
#include "json.hpp"
using nlohmann::json;
namespace jsons {
class Var {
   public:
    int id;
    std::string name;
    int is_sign;  // 是否是有符号变量
    int bit_width;
};

class AST {
   public:
    Operation op;
    std::string op_str;
    int id;             // 变量节点的 ID
    std::string value;  // 常数节点的值
    AST* lhs;
    AST* rhs;
    int bit_width;
};

}  // namespace jsons
typedef std::vector<jsons::Var> vars_vector;
typedef std::vector<jsons::AST> ast_vector;

class JsonParser {
    ast_vector& asts;
    std::string inputFile;
    std::string outputFile;
    vars_vector& vars;

   public:
    JsonParser(ast_vector& _ast_vector, vars_vector& _vars, std::string in, std::string out)
        : asts(_ast_vector), vars(_vars), inputFile(in), outputFile(out) {}

    // 自底向上计算各个子表达式的位宽
    void bottomUp(jsons::AST* ast, const vars_vector& vars);

    // 自顶向下统一位宽
    void topDown(jsons::AST* ast, const vars_vector& vars);

    // 中序输出抽象语法树并计算当前约束中的最大位宽限制
    void printAST(jsons::AST* ast, const vars_vector& vars);

    // 解析并输出json文件
    void inputParse();
};

#endif