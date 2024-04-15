#ifndef MYENUMS_H
#define MYENUMS_H

#pragma once
namespace myEnums {
enum Operation {
  /* 叶子节点 */
  VAR,
  CONST,
  /* 单目运算 */
  LOG_NEG,
  BIT_NEG,
  MINUS,
  /* 双目运算 */
  ADD,
  SUB,
  MUL,
  DIV,
  LOG_AND,
  LOG_OR,
  EQ,
  NEQ,
  LT,
  LTE,
  GT,
  GTE,
  BIT_AND,
  BIT_OR,
  BIT_XOR,
  RSHIFT,
  LSHIFT,
  IMPLY,
  /* 非法类型 */
  NOT_SUP
};
}

#endif