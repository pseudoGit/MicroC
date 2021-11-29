#include "token.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <fstream>

using namespace std;

void processFile(istream& in) {
  Lexer lexer(in);
  Token token;
  Parser parser(lexer, cout);
  
  // Parser::TreeNode* program = parser.factor();
  // Parser::TreeNode* program = parser.term();
  // Parser::TreeNode* program = parser.expression();
  // Parser::TreeNode* program = parser.relationalExpression();
  // Parser::TreeNode* program = parser.logicalExpression();
  // Parser::TreeNode* program = parser.assignStatement();
  // Parser::TreeNode* program = parser.ifStatement();
  // Parser::TreeNode* program = parser.whileStatement();
  Parser::TreeNode* program = parser.compilationunit();
  
  // cout << Parser::TreeNode::toString(program) << endl;
  parser.genasm(program);
}

int main(int argc, char **argv) {
  ifstream in;
  
  if (argc > 1) {
      in.open(argv[1]);
      processFile(in);
      in.close();
  } else {
    processFile(cin);
  }
  return 0; 
}

