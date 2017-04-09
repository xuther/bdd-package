#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sys/wait.h>

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
    mgr.PrintTable();
    
    std::cout << std::endl << std::endl << std::endl;

    BDD r = (~a & c);
    mgr.PrintTable();

    std::cout << std::endl << std::endl << std::endl;

    BDD s = (b & (~c & d));
    mgr.PrintTable();
    std::cout << std::endl << std::endl << std::endl;

    BDD t = r | s;
    mgr.PrintTable();
    std::cout << std::endl << std::endl << std::endl;

    BDD u = q | t;
    mgr.PrintTable();
    std::cout << std::endl << std::endl << std::endl;
    
/*	bddMgr mgr (0, 0);
	BDD d = mgr.bddVar();
	BDD c = mgr.bddVar();
	BDD b = mgr.bddVar();
	BDD a = mgr.bddVar();
	
	BDD r = (a & b) | (~a & c) | (b & ~c & d);

	std::vector<BDD> nodes;
    nodes.push_back(r);
	
	const char *inames[] = {"d", "c", "b", "a"};
	const char *onames[] = { "s"};  
	FILE* fp = fopen("test_BDD.dot", "w");
	DumpDot(mgr, nodes, inames, onames, fp);
	fclose(fp);
	makePDF("test_BDD");
    */
}


int 
main(int argc, char* argv[]) {
	test_BDD();	
	return 0;
}
