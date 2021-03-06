// Shell.

#include "types.h"
#include "user.h"
#include "fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10
#define bufSize 100
#define strSize 50
#define intSize 3
#define testNum 10
#define offset 48
#define decimal 10
#define charRange 128
#define intRange 100

static unsigned long next = 1;

struct cmd {
  int type;
};

struct execcmd {
  int type;
  char *argv[MAXARGS];
  char *eargv[MAXARGS];
};

struct redircmd {
  int type;
  struct cmd *cmd;
  char *file;
  char *efile;
  int mode;
  int fd;
};

struct pipecmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct listcmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct backcmd {
  int type;
  struct cmd *cmd;
};

int fork1(void);  // Fork but panics on failure.
void panic(char*);
struct cmd *parsecmd(char*);
void testcmd(char*, char*, int);
void ranStr(int, char*);
void ranInt(char*);
void testFuzzBasic(char*);
void testFuzzStr(char*, char*);
void testFuzzInt(char*, char*);
void testFuzz(char*);

int 
rand()
{
  next = next * 1103515245 + 12345;
  return((uint)(next/65536) % 32768);
}

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2];
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit();

  switch(cmd->type){
  default:
    panic("runcmd");

  case EXEC:
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit();
    close(1);
    exec(ecmd->argv[0], ecmd->argv);
    printf(2, "exec %s failed\n", ecmd->argv[0]);
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if(open(rcmd->file, rcmd->mode) < 0){
      printf(2, "open %s failed\n", rcmd->file);
      exit();
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    if(fork1() == 0)
      runcmd(lcmd->left);
    wait();
    runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
      panic("pipe");
    if(fork1() == 0){
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork1() == 0){
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait();
    wait();
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork1() == 0)
      runcmd(bcmd->cmd);
    break;
  }
  exit();
}

int
main(int argc, char** argv)
{
  static char buf[bufSize];
 
  testFuzz(buf);

  exit();
}

void
testcmd(char *buf, char *fuzz, int flag)
{
  if(flag){
    strcpy(buf, fuzz);
  }
  printf(1, "%s\n", buf);
    
  if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
    // Chdir must be called by the parent, not the child.
    buf[strlen(buf)-1] = 0;  // chop \n
    if(chdir(buf+3) < 0)
      printf(2, "cannot cd %s\n", buf+3);
    return;
  }
  if(fork1() == 0)
    runcmd(parsecmd(buf));
  wait();
  printf(1, "\n");

  return;
}

void
ranStr(int len, char* strbuf)
{
  for(int i = 0; i < len - 1; i++){
    strbuf[i] = (char)(rand() % charRange);
  }

  strbuf[len] = 0;

  return;
}

void 
testFuzzStr(char *buf, char *testCmd)
{
  char strbuf[strSize];
  ranStr(rand() % strSize, strbuf);
  memset(buf, 0, bufSize);
  strcpy(buf, testCmd);
  strcpy(buf + strlen(testCmd), strbuf);
  testcmd(buf, "", 0);

  return;
}

void 
ranInt(char* intBuf)
{
  memset(intBuf, 0, intSize);
  int num = rand() % intRange;

  // one digit
  if(num % decimal == num){
    intBuf[0] = (char)(num + offset);
  }
  else{// two digits
    intBuf[1] = (char)(num % decimal + offset);
    intBuf[0] = (char)(num / decimal + offset);
  }
}

void 
testFuzzInt(char *buf, char *testCmd)
{
  char intBuf[intSize];
  ranInt(intBuf);
  testcmd(buf, 0, bufSize);
  strcpy(buf, testCmd);
  strcpy(buf + strlen(testCmd), intBuf);
  testcmd(buf, "", 0);
    
  return;
}

void 
testFuzz(char* buf)
{
  testFuzzBasic(buf);

  printf(1, "==============Testing echo===============\n"); 
  for(int i = 0; i < testNum; i++){  
      testFuzzStr(buf, "echo ");
  }
  
  printf(1, "==============Testing mkdir===============\n"); 
  for(int i = 0; i < testNum; i++){
      testFuzzStr(buf, "mkdir ");
  }


  printf(1, "==============Testing kill===============\n"); 
  for(int i = 0; i < testNum; i++){
      testFuzzInt(buf, "kill ");
  }
 

}

void 
testFuzzBasic(char *buf)
{
  // testing wc
  printf(1, "==============Testing wc===============\n"); 
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 32 -o t0 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -p -o t1 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -o t2 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -l -o t3 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -o t4 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 30 -e \"btvxrn\" -o t5 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 20 -0 -l -o t6 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 255 -o t7 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 255 -0 -o t8 | wc", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 25 -l -s 96 -o t9 | wc", 1);

  // testing cat
  printf(1, "==============Testing cat===============\n"); 
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 32 -o t10 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -p -o t11 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -o t12 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -l -o t13 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -o t14 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 30 -e \"btvxrn\" -o t15 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 20 -0 -l -o t16 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 255 -o t17 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 255 -0 -o t18 | cat", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 25 -l -s 96 -o t19 | cat", 1);

  // testing grep
  printf(1, "==============Testing grep===============\n"); 
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 32 -o t20 | grep e", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -p -o t21 | grep a", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -o t22 | grep st", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -l -o t23 | grep i", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 100 -o t24 | grep a", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 30 -e \"btvxrn\" -o t25 | grep e", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 20 -0 -l -o t26 | grep st", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 255 -o t27 | grep i", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 255 -0 -o t28 | grep s", 1);
  memset(buf, 0, bufSize);
  testcmd(buf, "fuzz 25 -l -s 96 -o t29 | grep t", 1);

  return;
}


void
panic(char *s)
{
  printf(2, "%s\n", s);
  exit();
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

// Constructors

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = EXEC;
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
listcmd(struct cmd *left, struct cmd *right)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
backcmd(struct cmd *subcmd)
{
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  return (struct cmd*)cmd;
}
// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '(':
  case ')':
  case ';':
  case '&':
  case '<':
    s++;
    break;
  case '>':
    s++;
    if(*s == '>'){
      ret = '+';
      s++;
    }
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;

  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);
struct cmd *nulterminate(struct cmd*);

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    printf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
  while(peek(ps, es, "&")){
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es));
  }
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    switch(tok){
    case '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    case '+':  // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    }
  }
  return cmd;
}

struct cmd*
parseblock(char **ps, char *es)
{
  struct cmd *cmd;

  if(!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  if(!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if(peek(ps, es, "("))
    return parseblock(ps, es);

  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|)&;")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    if(argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  return ret;
}

// NUL-terminate all the counted strings.
struct cmd*
nulterminate(struct cmd *cmd)
{
  int i;
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    return 0;

  switch(cmd->type){
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    for(i=0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    nulterminate(pcmd->left);
    nulterminate(pcmd->right);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    nulterminate(lcmd->left);
    nulterminate(lcmd->right);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
