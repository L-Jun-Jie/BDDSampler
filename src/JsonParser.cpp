#include "JsonParser.h"

#include <fstream>

using myEnums::Operation;

namespace jsons {
// 转换变量列表
inline void from_json(const json& j, Var& var) {
    j.at("id").get_to(var.id);
    j.at("name").get_to(var.name);
    j.at("signed").get_to(var.is_sign);
    j.at("bit_width").get_to(var.bit_width);
}

inline Operation string_to_Operation(const std::string op_str) {
    if (op_str == "VAR") {
        return Operation::VAR;
    } else if (op_str == "CONST") {
        return Operation::CONST;
    } else if (op_str == "LOG_NEG") {
        return Operation::LOG_NEG;
    } else if (op_str == "BIT_NEG") {
        return Operation::BIT_NEG;
    } else if (op_str == "MINUS") {
        return Operation::MINUS;
    } else if (op_str == "ADD") {
        return Operation::ADD;
    } else if (op_str == "SUB") {
        return Operation::SUB;
    } else if (op_str == "MUL") {
        return Operation::MUL;
    } else if (op_str == "DIV") {
        return Operation::DIV;
    } else if (op_str == "LOG_AND") {
        return Operation::LOG_AND;
    } else if (op_str == "LOG_OR") {
        return Operation::LOG_OR;
    } else if (op_str == "EQ") {
        return Operation::EQ;
    } else if (op_str == "NEQ") {
        return Operation::NEQ;
    } else if (op_str == "LT") {
        return Operation::LT;
    } else if (op_str == "LTE") {
        return Operation::LTE;
    } else if (op_str == "GT") {
        return Operation::GT;
    } else if (op_str == "GTE") {
        return Operation::GTE;
    } else if (op_str == "BIT_AND") {
        return Operation::BIT_AND;
    } else if (op_str == "BIT_OR") {
        return Operation::BIT_OR;
    } else if (op_str == "BIT_XOR") {
        return Operation::BIT_XOR;
    } else if (op_str == "RSHIFT") {
        return Operation::RSHIFT;
    } else if (op_str == "LSHIFT") {
        return Operation::LSHIFT;
    } else if (op_str == "IMPLY") {
        return Operation::IMPLY;
    } else {
        return Operation::NOT_SUP;
    }
}
// 先根转换约束列表
inline void from_json(const json& j, AST& ast) {
    j.at("op").get_to(ast.op_str);
    ast.op = string_to_Operation(ast.op_str);

    switch (ast.op) {
        case Operation::VAR:
            j.at("id").get_to(ast.id);
            ast.value = "";
            ast.lhs = nullptr;
            ast.rhs = nullptr;
            break;
        case Operation::CONST:
            j.at("value").get_to(ast.value);
            ast.id = -1;
            ast.lhs = nullptr;
            ast.rhs = nullptr;
            break;
        case Operation::LOG_NEG:
        case Operation::BIT_NEG:
        case Operation::MINUS: {
            ast.id = -1;
            ast.value = "";
            ast.rhs = nullptr;
            auto left_ast = new AST();
            from_json(j["lhs_expression"], *left_ast);
            ast.lhs = left_ast;
            break;
        }
        case Operation::ADD:
        case Operation::SUB:
        case Operation::MUL:
        case Operation::DIV:
        case Operation::LOG_AND:
        case Operation::LOG_OR:
        case Operation::EQ:
        case Operation::NEQ:
        case Operation::LT:
        case Operation::LTE:
        case Operation::GT:
        case Operation::GTE:
        case Operation::BIT_AND:
        case Operation::BIT_OR:
        case Operation::BIT_XOR:
        case Operation::RSHIFT:
        case Operation::LSHIFT:
        case Operation::IMPLY: {
            ast.id = -1;
            ast.value = "";
            auto left_ast = new AST();
            auto right_ast = new AST();
            from_json(j["lhs_expression"], *left_ast);
            ast.lhs = left_ast;
            from_json(j["rhs_expression"], *right_ast);
            ast.rhs = right_ast;
            break;
        }
        default:
            std::cout << "Formulas containing illegal data operations: "
                      << ast.op_str << std::endl;
            exit(0);
    }
}
}  // namespace jsons

void JsonParser::bottomUp(jsons::AST* t, const vars_vector& vars) {
    switch (t->op) {
        case Operation::ADD:
        case Operation::SUB:
        case Operation::MUL:
        case Operation::DIV:
        case Operation::BIT_AND:
        case Operation::BIT_OR:
        case Operation::BIT_XOR:
            bottomUp(t->lhs, vars);  // 计算左表达式位宽
            bottomUp(t->rhs, vars);  // 计算右表达式位宽
            if (t->lhs->bit_width >= t->rhs->bit_width)
                t->bit_width = t->lhs->bit_width;
            else
                t->bit_width = t->rhs->bit_width;
            break;
        case Operation::EQ:
        case Operation::NEQ:
        case Operation::LT:
        case Operation::LTE:
        case Operation::GT:
        case Operation::GTE:
        case Operation::LOG_AND:
        case Operation::LOG_OR:
        case Operation::IMPLY:
            bottomUp(t->lhs, vars);
            bottomUp(t->rhs, vars);
            t->bit_width = 1;
            break;
        case Operation::LSHIFT:
        case Operation::RSHIFT:
            bottomUp(t->lhs, vars);
            bottomUp(t->rhs, vars);
            t->bit_width = t->lhs->bit_width;
            break;
        case Operation::LOG_NEG:
            bottomUp(t->lhs, vars);
            t->bit_width = 1;
            break;
        case Operation::BIT_NEG:
        case Operation::MINUS:
            bottomUp(t->lhs, vars);
            t->bit_width = t->lhs->bit_width;
            break;
        case Operation::VAR:
            t->bit_width = vars[t->id].bit_width;
            break;
        case Operation::CONST: {
            std::string tmp_width;
            for (auto c : t->value) {
                if (c == '\'' || c == 'h') {
                    break;
                }
                tmp_width += c;
            }
            // 获得常量位宽
            t->bit_width = stoi(tmp_width);
            break;
        }
        default:
            std::cout << "The OP is not yet supported !" << std::endl;
            exit(0);
            break;
    }
}

void JsonParser::topDown(jsons::AST* t, const vars_vector& vars) {
    switch (t->op) {
        case Operation::VAR:
        case Operation::CONST:
            return;
            break;
        case Operation::ADD:
        case Operation::SUB:
        case Operation::MUL:
        case Operation::DIV:
        case Operation::BIT_AND:
        case Operation::BIT_OR:
        case Operation::BIT_XOR:
            if (t->lhs->bit_width != t->bit_width)
                t->lhs->bit_width = t->bit_width;
            topDown(t->lhs, vars);  // 统一左侧子表达式位宽
            if (t->rhs->bit_width != t->bit_width)
                t->rhs->bit_width = t->bit_width;
            topDown(t->rhs, vars);  // 统一右侧子表达式位宽
            break;
        case Operation::EQ:
        case Operation::NEQ:
        case Operation::LT:
        case Operation::LTE:
        case Operation::GT:
        case Operation::GTE:
            if (t->lhs->bit_width < t->rhs->bit_width)
                t->lhs->bit_width = t->rhs->bit_width;
            if (t->lhs->bit_width > t->rhs->bit_width)
                t->rhs->bit_width = t->lhs->bit_width;
            topDown(t->lhs, vars);
            topDown(t->rhs, vars);
            break;
        case Operation::LOG_AND:
        case Operation::LOG_OR:
        case Operation::IMPLY:
            topDown(t->lhs, vars);
            topDown(t->rhs, vars);
            break;
        case Operation::LOG_NEG:
            topDown(t->lhs, vars);
            break;
        case Operation::LSHIFT:
        case Operation::RSHIFT:
            t->lhs->bit_width = t->bit_width;
            topDown(t->lhs, vars);
            topDown(t->rhs, vars);
            break;
        case Operation::BIT_NEG:
        case Operation::MINUS:
            t->lhs->bit_width = t->bit_width;
            topDown(t->lhs, vars);
            break;
        default:
            std::cout << "The OP is not yet supported !" << std::endl;
    }
}

void JsonParser::printAST(jsons::AST* t, const vars_vector& vars) {
    if (t != nullptr) {
        printAST(t->lhs, vars);
        switch (t->op) {
            case Operation::VAR:
                printf("%d'(var_%d)", t->bit_width, t->id);
                break;
            case Operation::CONST:
                printf("%d'(%s)", t->bit_width, t->value.c_str());
                break;
            case Operation::LOG_NEG:
            case Operation::BIT_NEG:
            case Operation::MINUS:
                printf(" %d'(%s)", t->bit_width, t->op_str.c_str());
                break;
            case Operation::ADD:
            case Operation::SUB:
            case Operation::MUL:
            case Operation::DIV:
            case Operation::LOG_AND:
            case Operation::LOG_OR:
            case Operation::EQ:
            case Operation::NEQ:
            case Operation::LT:
            case Operation::LTE:
            case Operation::GT:
            case Operation::GTE:
            case Operation::BIT_AND:
            case Operation::BIT_OR:
            case Operation::BIT_XOR:
            case Operation::RSHIFT:
            case Operation::LSHIFT:
            case Operation::IMPLY:
                printf(" %d'(%s) ", t->bit_width, t->op_str.c_str());
                break;
            default:
                std::cout << "Formulas containing illegal data operations: "
                          << t->op_str << std::endl;
                exit(0);
        }
        printAST(t->rhs, vars);
    }
}

// 解析并输出json文件
void JsonParser::inputParse() {
    json j;
    std::fstream jfile(inputFile);
    std::cout << inputFile << std::endl;
    if (!jfile.is_open()) {
        std::cout << "Fail to open file!" << std::endl;
        return;
    }
    jfile >> j;

    // 读取变量列表
    std::cout << "variable_list: " << std::endl;
    size_t vars_size = j["variable_list"].size();
    std::cout << "vars_list_size = " << vars_size << std::endl;
    for (int i = 0; i < vars_size; i++) {
        vars.push_back(j["variable_list"][i]);
        printf("variable%d：id=%d, name=%s, signed=%d, bit_width=%d\n",
               i, vars[i].id, vars[i].name.c_str(), vars[i].is_sign, vars[i].bit_width);
    }
    std::cout << std::endl;

    // 读取约束列表
    size_t constrains_size = j["constraint_list"].size();
    for (int i = 0; i < constrains_size; i++) {
        asts.push_back(j["constraint_list"][i]);
    }

    // 统一表达式位宽
    for (auto& t : asts) {
        bottomUp(&t, vars);
        topDown(&t, vars);
    }

    // 输出约束
    std::cout << "constraint_list: " << std::endl;
    std::cout << "constraint_list_size = " << constrains_size << std::endl;

    for (int i = 0; i < constrains_size; i++) {
        std::cout << "constraint" << i << ": ";
        printAST(&asts[i], vars);
        std::cout << std::endl;
    }
    std::cout << std::endl;
}