#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <assert.h>
#include <sys/wait.h>
#include <bitset>

#include "bddObj.h"

/* NOTE: the last 3 paramters are optional
 */
extern void
DumpDot(bddMgr& mgr,
		const std::vector<BDD>& nodes,
		char const * const * inames = 0, 
		char const * const * onames = 0, 
		FILE* fp = stdout);

void makePDF(std::string name) {
  pid_t pid = fork();
  if (pid == 0) {
  	  char arg0[] = "dot";
  	  char arg1[] = "-Tpdf";
	  std::string oname = "-o" + name + ".pdf";
	  std::string iname = name + ".dot";
  	  char* argv[] = {arg0, arg1, (char *)oname.c_str(), (char *)iname.c_str(), 0};
  	  execve("/usr/local/bin/dot", argv, NULL);
  } else {
  	  int stat;
  	  waitpid(pid, &stat, 0);
  }
}

void 
test_BDD() {
    bddMgr mgr (0);
    mgr.bddVar();
    mgr.bddVar();
    mgr.bddVar();
    mgr.bddVar();
    BDD a = mgr.bddVar(0);
    BDD b = mgr.bddVar(1);
    BDD c = mgr.bddVar(2);
    BDD d = mgr.bddVar(3);
    
    BDD q = (a & b);
    assert(q.GetID() == 6);
    mgr.PrintTable();
    
    std::cout << std::endl << std::endl << std::endl;

    BDD r = (~a & c);
    assert(r.GetID() == 8);
    mgr.PrintTable();

    std::cout << std::endl << std::endl << std::endl;

    BDD s = (b & (~c & d));
    mgr.PrintTable();
    assert(s.GetID() == 11);
    std::cout << std::endl << std::endl << std::endl;

    BDD t = r | s;
    mgr.PrintTable();
    assert(t.GetID() == 14);
    std::cout << std::endl << std::endl << std::endl;

    BDD u = q | t;
    mgr.PrintTable();
    assert(u.GetID() == 15);
    std::cout << std::endl << std::endl << std::endl;
    
}


void 
test_BDD_set() {

/*
 * Basically a test to check and see if we can  use the BDD to determine set membership in a reasonably complex set. We'll use a 256 member set, represented by 8 variables.
 * Set will contain values 
 *    Val | Variable | Binary 
 *    ------------------------
 *    159 |  one     | 10011111 
 *    54  |  two     | 00110110
 *    246 |  three   | 11110110
 *    222 |  four    | 11011110
 *    83  |  five    | 01010011
 *    41  |  six     | 00101001
 *    140 |  seven   | 10001100
 *    106 |  eight   | 01101010
 *
 */
   std::vector<unsigned char> values = {159,54,246,222,83,41,140,106};
   std::vector<std::bitset<8>> bitValues;

   for(int i =0; i < values.size(); i++) {
       std::bitset<8> x(values[i]);
       std::cout << x << std::endl;
       bitValues.push_back(x);
   }

   bddMgr mgr (0);
   BDD a = mgr.bddVar();
   BDD b = mgr.bddVar();
   BDD c = mgr.bddVar();
   BDD d = mgr.bddVar();
   BDD e = mgr.bddVar();
   BDD f = mgr.bddVar();
   BDD g = mgr.bddVar();
   BDD h = mgr.bddVar();

   std::vector<BDD> bdds = {a,b,c,d,e,f,g,h};

//Encode the set values
    
   BDD one = (a & (~b) & (~c) & d & e & f & g & h);
   BDD two = ((~a) & (~b) & c & d & (~e) & f & g & (~h));
   BDD three = (a & b & c & d & (~e) & f & g & (~h));
   BDD four = ( a & b & (~c) & d & e & f & g & (~h));
   BDD five = ((~a) & b & (~c) & d & (~e) & (~f) & g & h);
   BDD six = ((~a) & (~b) & c & (~d) & e & (~f) & (~g) & h);
   BDD seven = (a & (~b) & (~c) & (~d) & e & f & (~g) & (~h));
   BDD eight = ((~a) & b & c & (~d) & e & (~f) & g & (~h));

   mgr.PrintTable();

   BDD set = one | two | three | four | five | six | seven | eight;
   mgr.PrintTable();


   for (int i = 0 ; i < values.size(); i++) {
       BDD cur = set;
       std::cout << bitValues[i] << std::endl;
       for (int j = 0; j < bdds.size(); j++) {
           if (bitValues[i].test(7 - j)) {
               std::cout << "Restricting on " << j << std::endl;
               bdds[j].PrintIDRow();
               std::cout << "Pre restrict " << std::endl;
               cur.PrintIDRow();
               BDD next = cur.Restrict(bdds[j]);
               std::cout << "Post restrict " << std::endl;
               next.PrintIDRow();
               cur = next;
           }
           else {
               std::cout << "Restricting on ~" << j << std::endl;
               std::cout << "Pre restrict " << std::endl;
               cur = cur.Restrict(~bdds[j]);
               std::cout << "Post restrict " << std::endl;
               cur.PrintIDRow();
               
           }
       }
       cur.PrintIDRow();
       assert(cur.GetID() == 1);
   }
}

int 
main(int argc, char* argv[]) {
//	test_BDD();	
    test_BDD_set();
	return 0;
}
