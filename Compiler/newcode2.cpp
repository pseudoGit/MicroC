
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


void Parser::genasm(TreeNode *node) {
  n = 1;
  cout << "  global main" << endl;
  cout << "  extern printf" << endl << endl;
  cout << "  segment .bss" << endl;
  vardefs(node);
  for (int i=1; i < n; ++i) {
    cout << "  " << vars[i] << " resq 1" << endl;
  }
  cout << endl << "  section .text" << endl;
  geninst(node);
  cout << endl << "  section .data" << endl;
  for (int i=0; i < nfmts; ++i) {
    cout << "  fmt" << i+1 << ": db " << fmts[i] << ", 0" << endl;
  }

}



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
  TreeNode* printStatement = new TreeNode(SEQ, paramList,
					  new TreeNode(PRINTF, itos(nparams) + formatString));
  return printStatement;
}

