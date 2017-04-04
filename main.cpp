#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>

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
	bddMgr mgr (0, 0);
	BDD a = mgr.bddVar();
	BDD b = mgr.bddVar();
	BDD ap = mgr.bddVar();
	BDD bp = mgr.bddVar();
	
	BDD r = (~a & ~b & ap & bp) | (a & b & ~ap & ~bp) | (a & b & ap & ~bp) |
		(a & ~b & ap & ~bp);
	
	BDD l = (~ap & ~bp) | (ap & ~bp);
	
	std::vector<BDD> nodes;
	nodes.push_back(r);
	nodes.push_back(l);
	
	const char *inames[] = {"a", "b", "ap", "bp"};
	const char *onames[] = {"r", "l"};  
	FILE* fp = fopen("test_BDD.dot", "w");
	DumpDot(mgr, nodes, inames, onames, fp);
	fclose(fp);
	makePDF("test_BDD");
}


int 
main(int argc, char* argv[]) {
	test_BDD();	
	return 0;
}
