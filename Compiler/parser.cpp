/*
 Ari Chan
 Program Description: Builds trees of commands from tokens
*/


#include "parser.h"
#include <cstring>
#include <stack>
#include <vector>
#include <algorithm>

Parser::Parser(Lexer& lexerx, ostream& outx): lexer(lexerx), out(outx), lindex(1), tindex(1) {
  token = lexer.nextToken();
}

Parser::~Parser() {
}

const string Parser::ops[] = {"ADD", "SUB", "MULT", "DIV",

                              "ISEQ", "ISNE", "ISLT", "ISLE", "ISGT", "ISGE",

                              "AND", "OR",

                              "PUSHL", "PUSHV", "STORE",

                              "JUMP", "JUMPF", "JUMPT", "CALL", "RET", "FUNC",

                              "PRINTF",

                              "LABEL", "SEQ"};

void Parser::error(string message) {
  cerr << message << "Found " << token.lexeme() << " at line " << token.line() << " position " << token.pos() << endl;
  exit(1);
}

void Parser::check(int tokenType, string message) {
  if (token.type() != tokenType)
    error(message);
}

// Given code
void emit(string txt) {
  cout << txt << endl;
}

// Given code
int jindex = 0;
string jlabel() { string tmp = "J"; stringstream ss; ss << ++jindex; string res = ss.str(); tmp = tmp + res; return tmp;}

// Given code
void cond(string op) {
  string j1 = jlabel();
  string j2 = jlabel();
  emit("  pop rbx");
  emit("  pop rax");
  emit("  cmp rax,rbx");
  emit("  " + op + " " + j1);
  emit("  mov rax,0");
  emit("  jmp " + j2);
  emit(j1 + ":");
  emit("  mov rax,1");
  emit(j2 + ":");
  emit("  push rax");
}

// Given Code
string currentFunc;
int nfmts = 0;
string fmts[100];
void Parser::geninst(TreeNode *node) {
  string fmt = "";
  int nparams = 0;
  if (node != NULL) {
    geninst(node->leftChild);
    geninst(node->rightChild);
    switch (node->op) {
    case SEQ:
      break;
    case PUSHV:
      emit("  push qword[" + node->val + "]");
      break;
    case PUSHL:
      emit("  mov rax," + node->val);
      emit("  push rax");
      break;
    case ADD:
      emit("  pop rbx");
      emit("  pop rax");
      emit("  add rax,rbx");
      emit("  push rax");
      break;
    case SUB:
      emit("  pop rbx");
      emit("  pop rax");
      emit("  sub rax,rbx");
      emit("  push rax");
      break;
    case MULT:
      emit("  pop rbx");
      emit("  pop rax");
      emit("  imul rbx");
      emit("  push rax");
      break;
    case DIV:
      emit("  mov rdx,0");
      emit("  pop rbx");
      emit("  pop rax");
      emit("  idiv rbx");
      emit("  push rax");
      break;
    case STORE:
      emit("  pop qword[" + node->val + "]");
      break;
    case AND:
      emit("  pop rbx");
      emit("  pop rax");
      emit("  and rax,rbx");
      emit("  push rax");
      break;
    case OR:
      emit("  pop rbx");
      emit("  pop rax");
      emit("  or  rax,rbx");
      emit("  push rax");
      break;
    case LABEL:
      emit(node->val);      
      break;
    case ISEQ:
      cond("je");
      break;
    case ISNE:
      cond("jne");
      break;
    case ISLT:
      cond("jl");
      break;
    case ISLE:
      cond("jle");
      break;
    case ISGT:
      cond("jg");
      break;
    case ISGE:
      cond("jge");
      break;
    case JUMP:
      emit("  jmp " + node->val);
      break;
    case JUMPF:
      emit("  pop rax");
      emit("  cmp rax,0");
      emit("  je " + node->val);
      break;
    case JUMPT:
      emit("  pop rax");
      emit("  cmp rax,0");
      emit("  jne " + node->val);
      break;
    case CALL:
      emit("  call " + node->val);
      emit("  push rax");      
      break;
    case FUNC:
      currentFunc = node->val;
      emit(currentFunc);
      if (currentFunc != "main:")
        emit("  pop r15");      
      break;
    case RET:
      emit("  pop rax");
      if (currentFunc != "main:")
         emit("  push r15");            
      emit("  ret");
      break;
    case PRINTF: 
      fmt = node->val;
      nparams = fmt.at(0) - '0';
      fmt = "`" + fmt.substr(1) + "`";
      fmts[nfmts++] = fmt;
      emit("  mov    rdi,fmt" + itos(nfmts));
      if (nparams == 5) {
	emit("  pop   r9");
	--nparams;
      }
      if (nparams == 4) {
	emit("  pop   r8");
	--nparams;
      }
      if (nparams == 3) {
	emit("  pop   rcx");
	--nparams;
      }
      if (nparams == 2) {
	emit("  pop   rdx");
	--nparams;
      }
      if (nparams == 1) {
	emit("  pop   rsi");
      }
      emit("  mov    rax,0");
      emit("  push   rbp");
      emit("  call   printf");
      emit("  pop    rbp");       
      break;
    default:
      cerr << "In geninst: Unknown operation " << ops[node->op] << endl;
      exit(1);
      break;      
    }
  }
}

// Traverse the tree and collect all variable names from any
// PUSHV or STORE instructions
vector<string> varNames;
void Parser::vardefs(TreeNode* node) {
  if (node != NULL) {
    vardefs(node->leftChild);
    vardefs(node->rightChild);
  // Adds variable names to an array list if an only if it is part of a PUSHV
  // or STORE instruction and is not already in the array  
  if ((node->op == PUSHV || node->op == STORE) && !(find(varNames.begin(), varNames.end(), node->val) != varNames.end())) {
      // Add variable name
      varNames.push_back(node->val);
    }
  }
}


// Given code
void Parser::genasm(TreeNode* node) {
  cout << "  global main" << endl;
  cout << "  extern printf" << endl << endl;
  cout << "  segment .bss" << endl;
  vardefs(node);
  for (int i=0; i < (signed)varNames.size(); ++i) {
    cout << "  " << varNames[i] << " resq 1" << endl;
  }
  cout << endl << "  section .text" << endl;
  geninst(node);
  cout << endl << "  section .data" << endl;
  for (int i=0; i < nfmts; ++i) {
    cout << "  fmt" << i+1 << ": db " << fmts[i] << ", 0" << endl;
  }
}

// <funcall> ::= IDENT LPAREN [<expression> { COMMA <expression> }] RPAREN
Parser::TreeNode* Parser::funcall(string functionName) {
  TreeNode* callNode = NULL;
  TreeNode* nameNode = NULL;

  // Gets the args 
  if (token.type() != Token::RPAREN) {
    callNode = expression();
    while (token.type() == Token::COMMA) {
      token = lexer.nextToken(); // Skips over the comma
      callNode = new TreeNode(SEQ, callNode, expression());
    }
  }
  else {
    callNode = new TreeNode(SEQ); // If there are no args
  }
  
  // Builds the tree
  nameNode = new TreeNode(CALL, functionName);
  callNode = new TreeNode(SEQ, callNode, nameNode);
  
  return callNode;
}

// <factor> ::= LPAREN <expression> RPAREN | INTLIT | IDENT | <funcall>
Parser::TreeNode* Parser::factor() {
  TreeNode* node = NULL; 

  // Parses a factor
  if (token.type() == Token::LPAREN) {
    token = lexer.nextToken(); // Skips '('
    node = expression();
    check(Token::RPAREN, "Error, expected ')'. ");
    token = lexer.nextToken(); // Skips ')' and loads next token
  }
  else if (token.type() == Token::INTLIT) {
    node = new TreeNode(PUSHL, token.lexeme());
    token = lexer.nextToken(); // Loads next token
  }
  else if (token.type() == Token::IDENT) {
    Token tmpToken(token.type(), token.lexeme(), token.line(), token.pos());
      // Stores the IDENT
    token = lexer.nextToken();
    // Checks if the IDENT is part of a funcall() or just an IDENT
    if (token.type() == Token::LPAREN) {
      token = lexer.nextToken(); // Skips '('
      node = funcall(tmpToken.lexeme());
      check(Token::RPAREN, "Error, expected ')'. ");
      token = lexer.nextToken(); // Skips ')' and loads next token
    }
    else { // Case where factor is a variable
      node = new TreeNode(PUSHV, tmpToken.lexeme());
    }
  }

  return node;
}

// <term> ::=  <factor> { TIMES <factor> | DIVIDE <factor> }
Parser::TreeNode* Parser::term() {
  TreeNode* termNode = factor();
  TreeNode* factNode = NULL;
  Token tmpOp;
  
  // Parses repetitions of terms
  while (token.type() == Token::TIMES || token.type() == Token::DIVIDE) {
    tmpOp = Token(token.type(), token.lexeme(), token.line(), token.pos());
      // Stores the operation
    token = lexer.nextToken(); // Go to the next term or operation
    if (tmpOp.type() == Token::TIMES) {
      factNode = factor();
      termNode = new TreeNode(MULT, tmpOp.lexeme(), termNode, factNode);
    }
    else if (tmpOp.type() == Token::DIVIDE) {
      factNode = factor();
      termNode = new TreeNode(DIV, tmpOp.lexeme(), termNode, factNode);
    }
  }

  return termNode;
}

// <expression> ::= <term> { PLUS <term> | MINUS <term> }
Parser::TreeNode* Parser::expression() {
  TreeNode* expNode = term();
  TreeNode* termNode = NULL;
  Token tmpOp;

  // Parses repetitions of expressions
  while (token.type() == Token::PLUS || token.type() == Token::MINUS) {
    tmpOp = Token(token.type(), token.lexeme(), token.line(), token.pos());
      // Stores the operation
    token = lexer.nextToken(); // Go to the next expression or operation
    if (tmpOp.type() == Token::PLUS) {
      termNode = term();
      expNode = new TreeNode(ADD, tmpOp.lexeme(), expNode, termNode);
    }
    else if (tmpOp.type() == Token::MINUS) {
      termNode = term();
      expNode = new TreeNode(SUB, tmpOp.lexeme(), expNode, termNode);
    }
  }

  return expNode;
}

// <relationalexpression> ::= <expression> [ EQ <expression> | LT <expression> | LE <expression> | GT <expression> | GE <expression> | NE <expression> ]
Parser::TreeNode* Parser::relationalExpression() {
  TreeNode* relNode = expression();
  TreeNode* expNode = NULL;
  
  // Parses an expression if and only if it has a relational operator.
  // Otherwise, return a single expression.
  if (token.type() >= Token::EQ && token.type() <= Token::GE) {
    Token tmpOp(token.type(), token.lexeme(), token.line(), token.pos()); 
      // Stores the operation
    token = lexer.nextToken(); // Load the next token for expression()
    if (tmpOp.type() == Token::EQ) {
      expNode = expression();
      relNode = new TreeNode(ISEQ, tmpOp.lexeme(), relNode, expNode);
    }
    else if (tmpOp.type() == Token::NE) {
      expNode = expression();
      relNode = new TreeNode(ISNE, tmpOp.lexeme(), relNode, expNode);
    }
    else if (tmpOp.type() == Token::LT) {
      expNode = expression();
      relNode = new TreeNode(ISLT, tmpOp.lexeme(), relNode, expNode);
    }
    else if (tmpOp.type() == Token::LE) {
      expNode = expression();
      relNode = new TreeNode(ISLE, tmpOp.lexeme(), relNode, expNode);
    }
    else if (tmpOp.type() == Token::GT) {
      expNode = expression();
      relNode = new TreeNode(ISGT, tmpOp.lexeme(), relNode, expNode);
    }
    else if (tmpOp.type() == Token::GE) {
      expNode = expression();
      relNode = new TreeNode(ISGE, tmpOp.lexeme(), relNode, expNode);
    }
  }

  return relNode;
}

// <logicalexpression> ::= <relationalexpression> { AND <relationalexpression> | OR <relationalexpression> }
Parser::TreeNode* Parser::logicalExpression() {
  TreeNode* logNode = relationalExpression();
  TreeNode* relNode = NULL;
  Token tmpOp;
 
  // Parses repetitions of logical expressions 
  while (token.type() == Token::AND || token.type() == Token::OR) {
    tmpOp = Token(token.type(), token.lexeme(), token.line(), token.pos());
      // Stores the operation
    token = lexer.nextToken();
      // Loads the beginning of a relational expression
    if (tmpOp.type() == Token::AND) {
      relNode = relationalExpression();
      logNode = new TreeNode(AND, tmpOp.lexeme(), logNode, relNode);
    }
    else if (tmpOp.type() == Token::OR) {
      relNode = relationalExpression();
      logNode = new TreeNode(OR, tmpOp.lexeme(), logNode, relNode);
    }
  }

  return logNode;
}

// <returnstatement> ::= RETURN <logicalExpression> SEMICOLON
Parser::TreeNode* Parser::returnStatement() {
  TreeNode* retNode = NULL;
  TreeNode* logNode = NULL;
  
  // Checks for a return statement and skips over it
  check(Token::RETURN, "Error, undefined statement. ");
  token = lexer.nextToken();
  
  logNode = logicalExpression();
  
  // Checks for a semicolon and skips over it
  check(Token::SEMICOLON, "Error, expected ';'. ");
  token = lexer.nextToken();

  // Build return tree
  retNode = new TreeNode(RET);
  retNode = new TreeNode(SEQ, logNode, retNode);
 
  return retNode;
}

// Code is given
Parser::TreeNode* Parser::printfStatement() {
  TreeNode* paramList = NULL;
  int nparams = 0;
  check(Token::PRINTF, "Expecting printf");
  token = lexer.nextToken();
  check(Token::LPAREN, "Expecting (");
  token = lexer.nextToken();
  check(Token::STRINGLIT, "Expecting string literal");
  string formatString = token.lexeme();
  token = lexer.nextToken();
  if (token.type() == Token::COMMA) {
    token = lexer.nextToken();
    paramList = expression();
    ++nparams;
    while (token.type() == Token::COMMA) {
      token = lexer.nextToken();
      paramList = new TreeNode(SEQ, paramList, expression());
      ++nparams;
    }
  }
  check(Token::RPAREN, "Expecting )");
  token = lexer.nextToken();
  check(Token::SEMICOLON, "Expecting ;");
  token = lexer.nextToken();
  TreeNode* printStatement = new TreeNode(SEQ, paramList, new TreeNode(PRINTF, itos(nparams) + formatString));
  return printStatement;
}

// <whilestatement> ::= WHILE LPAREN <logicalexpression> RPAREN <block>
Parser::TreeNode* Parser::whileStatement() {
  TreeNode* whileNode = NULL;
  TreeNode* logNode = NULL;
  TreeNode* blockNode = NULL;
  TreeNode* labelNode1 = NULL;
  TreeNode* jumpNode1 = NULL;
  TreeNode* labelNode2 = NULL;
  TreeNode* jumpNode2 =  NULL;

  // Checks for "while" statement
  check(Token::WHILE, "Error, undefined statement. ");
  token = lexer.nextToken();

  // Checks for '(' and skips over it
  check(Token::LPAREN, "Error, expected '('. ");
  token = lexer.nextToken();

  logNode = logicalExpression();
  
  // Checks for ')' and skips over it
  check(Token::RPAREN, "Error, expected ')'. ");
  token = lexer.nextToken();

  // Checks for a open curly bracket and skips over it
  check(Token::LBRACE, "Error, expected '{'. ");
  token = lexer.nextToken();

  blockNode = block();

  // Checks for a closed curly bracket and skips over it
  check(Token::RBRACE, "Error, expected '}'. ");
  token = lexer.nextToken();

  string label1 = makeLabel();
  string label2 = makeLabel();
  labelNode1 = new TreeNode(LABEL, label1 + ":");
  jumpNode1 = new TreeNode(JUMPF, label2);
  labelNode2 = new TreeNode(LABEL, label2 + ":");
  jumpNode2 = new TreeNode(JUMP, label1);

  // Builds the "while" tree
  whileNode = new TreeNode(SEQ, labelNode1, logNode);
  jumpNode1 = new TreeNode(SEQ, jumpNode1, blockNode);
  whileNode = new TreeNode(SEQ, whileNode, jumpNode1);
  jumpNode2 = new TreeNode(SEQ, jumpNode2, labelNode2);
  whileNode = new TreeNode(SEQ, whileNode, jumpNode2);

  return whileNode;
}

// <ifstatement> ::= IF LPAREN <logicalexpression> RPAREN <block> [ELSE <block>]
Parser::TreeNode* Parser::ifStatement() {
  TreeNode* ifNode = NULL;
  TreeNode* elseNode = NULL;
  TreeNode* labelNode1 = NULL;
  TreeNode* labelNode2 = NULL;
  TreeNode* jumpNode1 = NULL;
  TreeNode* jumpNode2 = NULL;
  TreeNode* logNode = NULL;
  TreeNode* blockNode = NULL;
  
  // Checks for an "if" and skips over it
  check(Token::IF, "Error, undefined statement. ");
  token = lexer.nextToken();

  // Checks for a open parenthesis and skips over it
  check(Token::LPAREN, "Error, expected '('. ");
  token = lexer.nextToken();

  logNode = logicalExpression();
 
  // Checks for a closed parenthesis and skips over it
  check(Token::RPAREN, "Error, expected ')'. ");
  token = lexer.nextToken();
  
  // Checks for a open curly bracket and skips over it
  check(Token::LBRACE, "Error, expected '{'. ");
  token = lexer.nextToken();
  
  blockNode = block();
  
  // Checks for a closed curly bracket and skips over it
  check(Token::RBRACE, "Error, expected '}'. ");
  token = lexer.nextToken();
  
  // Checks for an "else" statement
  // Builds a tree for either an "if-else" or just an "if" statement.
  if (token.type() == Token::ELSE) {
    token = lexer.nextToken(); // Skips the "else" token

    // Create labels
    string label1 = makeLabel();
    string label2 = makeLabel();
    labelNode1 = new TreeNode(LABEL, label1 + ":");
    jumpNode1 = new TreeNode(JUMPF, label2);
    labelNode2 = new TreeNode(LABEL, label2 + ":");
    jumpNode2 = new TreeNode(JUMP, label1);

    // Skips over curly bracket
    check(Token::LBRACE, "Error, expected '{'. ");
    token = lexer.nextToken();

    elseNode = block();

    // Skips over curly bracket
    check(Token::RBRACE, "Error, expected '}'. ");
    token = lexer.nextToken();
    
    // Builds an "if-else" tree
    ifNode = new TreeNode(SEQ, logNode, jumpNode1);
    blockNode = new TreeNode(SEQ, blockNode, jumpNode2);
    ifNode = new TreeNode(SEQ, ifNode, blockNode);
    elseNode = new TreeNode(SEQ, labelNode2, elseNode);
    ifNode = new TreeNode(SEQ, ifNode, elseNode);
  }
  else { // Builds an "if" tree
    string label1 = makeLabel();
    labelNode1 = new TreeNode(LABEL, label1 + ":");
    jumpNode1 = new TreeNode(JUMPF, label1);

    // Builds an "if" tree
    ifNode = new TreeNode(SEQ, logNode, jumpNode1);
    ifNode = new TreeNode(SEQ, ifNode, blockNode);
  }

  ifNode = new TreeNode(SEQ, ifNode, labelNode1);
 
  return ifNode;
}

// <assignstatement> ::= IDENT ASSIGN <logicalexpression> SEMICOLON
Parser::TreeNode* Parser::assignStatement() {
  TreeNode* identNode = NULL;
  TreeNode* assignNode = NULL;
  
  // Checks for an ident, stores it, and loads next token
  check(Token::IDENT, "Error, expected IDENT. ");
  identNode = new TreeNode(STORE, token.lexeme());
  token = lexer.nextToken();
  
  // Checks for an assignment operator and skips over it
  check(Token::ASSIGN, "Error, expected '='. ");
  token = lexer.nextToken();
  
  assignNode = logicalExpression();
  
  // Checks for semicolon and loads next token
  check(Token::SEMICOLON, "Error, expected ';'. ");
  token = lexer.nextToken();

  assignNode = new TreeNode(SEQ, assignNode, identNode);

  return assignNode;
}

// <statement> ::= <assignmentstatement> | <whilestatement> | <ifstatement> | <returnstatement>
Parser::TreeNode* Parser::statement() {
  TreeNode* statement = NULL;
  
  // Parses the individual statement types
  if (token.type() == Token::IDENT) {
    statement = assignStatement();
  }
  else if (token.type() == Token::WHILE) {
    statement = whileStatement();
  }
  else if (token.type() == Token::IF) {
    statement = ifStatement();
  }
  else if (token.type() == Token::RETURN) {
    statement = returnStatement();
  }
  else if (token.type() == Token::PRINTF) {
    statement = printfStatement();
  }
  else {
    error("Invalid start of statement. ");
  }
 
  return statement;
}

// <block> ::= LBRACE { <statement> } RBRACE 
Parser::TreeNode* Parser::block() {
  TreeNode* blockNode = NULL;

  // Ensures the the block is not empty
  if (token.type() != Token::RBRACE) {
    blockNode = statement();
    // Loop and parse each statement while it is not the end of the block
    while (token.type() != Token::RBRACE) {
      blockNode = new TreeNode(SEQ, blockNode, statement());
    }
  }
  else if (blockNode == NULL) { // Returns sequence node if block is empty
    blockNode = new TreeNode(SEQ);
  }

  return blockNode;
}

// <function> ::= FUNCTION IDENT LPAREN <parameterdefs> RPAREN <block>
Parser::TreeNode* Parser::function() {
  TreeNode* funcNode = NULL;
  TreeNode* blockNode = NULL;
  TreeNode* paramNode = NULL;
  stack<string> funParams;

  // Checks for a function and loads next token
  check(Token::FUNCTION, "Error, not a valid function. ");
  token = lexer.nextToken();

  // Checks for function name, stores it, and loads next token
  check(Token::IDENT, "Error, not a valid function. ");
  string funcName = token.lexeme();
  token = lexer.nextToken();

  funcNode = new TreeNode(FUNC, funcName + ":");
  
  // Checks for '(' and skips it
  check(Token::LPAREN, "Error, expected '('. ");
  token = lexer.nextToken();
  
  // Retrieve the list of parameters
  if (token.type() != Token::RPAREN) {
    funParams.push(token.lexeme());
    token = lexer.nextToken();
    while (token.type() == Token::COMMA) {
      token = lexer.nextToken(); // Skips over the comma
      funParams.push(token.lexeme());
      token = lexer.nextToken();
    }
  }

  // Checks for ')' and skips it
  check(Token::RPAREN, "Error, expected ')'. ");
  token = lexer.nextToken();

  // Store the parameter names
  string paramName;
  while (!funParams.empty()) {
    paramName = funParams.top();
    funParams.pop();
    paramNode = new TreeNode(STORE, paramName);
    funcNode = new TreeNode(SEQ, funcNode, paramNode);
  }

  // Checks for a open curly bracket and skips over it
  check(Token::LBRACE, "Error, expected '{'. ");
  token = lexer.nextToken();

  blockNode = block();
  
  // Checks for a closed curly bracket and skips over it
  check(Token::RBRACE, "Error, expected '}'. ");
  token = lexer.nextToken();

  // Build function tree
  funcNode = new TreeNode(SEQ, funcNode, blockNode);

  return funcNode;
}

// <compilationunit> ::= { <function> }
Parser::TreeNode* Parser::compilationunit() {
  TreeNode* compileNode = NULL;

  // Checks for a function
  check(Token::FUNCTION, "Error, no functions found.");

  compileNode = function();
  // Builds a tree while there are functions
  while (token.type() == Token::FUNCTION) {
    compileNode = new TreeNode(SEQ, compileNode, function());
  }

  return compileNode;
}

