#include <stdio.h>
#include <assert.h>
#include <map>
#include <set>
#include <list>

#ifdef USE_CUDD
#include "cuddObj.hh"
typedef Cudd bddMgr;
#else
#include "bddObj.h"
#endif

/**
 * Output: dot graph
 * Input: 
 *   - mgr: BDD manager
 *   - nodes: each BDD in the list is output
 *   - inames: an array of strings for variable names
 *             The array should match the BDD variable order.
 *   - onames: an array of strings for each BDD in the list
 */
void DumpDot(bddMgr& mgr,
				const std::vector<BDD>& nodes,
				char const * const * inames = 0, 
				char const * const * onames = 0, 
				FILE* fp = stdout);

/*******************************************************************************
 * SUPPORT FUNCTIONS
 *******************************************************************************/
void dumpHeader(FILE* fp) {
	assert(fp != NULL);
	fprintf(fp, "digraph \"DD\" {\n");
	fprintf(fp, "size = \"7.5,10\"\n");
	fprintf(fp, "center = true;\n");
	fprintf(fp, "edge [dir = none];\n");	
}

void dumpOnames(char const * const * onames, unsigned size, FILE* fp) {
	assert(fp != NULL);
	if (onames == NULL) {
		return;
	}

	// All the function names are on the same level
	fprintf(fp, "{ rank = same; node [shape = box]; edge [style = invis];\n");

	unsigned i = 0;
	fprintf(fp, "\" %s \"", onames[i++]);
	while (i < size) {
		fprintf(fp, " -> \" %s \"", onames[i++]);
	}

	fprintf(fp, "; }\n");
}

void dumpConstNodes(FILE* fp) {
	assert(fp != NULL);	
	fprintf(fp, "{ rank = same; \"CONST NODES\" [style = invis];\n");
	fprintf(fp, "{ node [shape = box]; \"0x0\";\n");
	fprintf(fp, "  node [shape = box]; \"0x1\";\n");
    fprintf(fp, "}\n");
    fprintf(fp, "}\n");
}

// Generates a unique ID for matching shared structures in the BDD
// ID 0 and 1 are reserved for terminals.
static unsigned ID = 1;
unsigned getFreshID() {
	return ++ID;
}

void dumpNode(unsigned x, FILE* fp) {
	assert(fp != NULL);
	if (x == 0) {
		fprintf(fp, "\"0x0\"");
	} else {
		fprintf(fp, "\"%#x\"", x);
	}
}

void dumpEdge(unsigned x, unsigned y, bool high, FILE* fp) {
	assert(fp != NULL);
	dumpNode(x, fp);
	fprintf(fp, " -> ");
	dumpNode(y, fp);
	if (!high) {
		fprintf(fp, "[style = dashed]");
	}
	fprintf(fp, "\n");
}

// Finds the first variable on which f depends. 
unsigned findVar(bddMgr& mgr, BDD f, unsigned var) {
	assert( f != mgr.bddOne() && f != mgr.bddZero());
	BDD x = mgr.bddVar(var);
	while (f.Restrict(x) == f) {
		x = mgr.bddVar(++var);
	}
	return var;
}

unsigned dumpDot(bddMgr& mgr,
				 BDD f,
				 unsigned var,
				 std::list<std::pair<BDD,unsigned> >& bddToNodeID,
				 std::map<unsigned,unsigned>& idToInameIndex,
	             FILE* fp) {
		
	// Check for terminals
	if (f == mgr.bddOne()) {
		return 1;
	}
	if (f == mgr.bddZero()) {
		return 0;
	}

	// Check the cache
	for (auto i : bddToNodeID) {
		if (i.first == f) {
			return i.second;
		}
	}

	// Get the low/high nodes on the next variable on which f depends
	var = findVar(mgr, f, var);
	BDD x = mgr.bddVar(var);
	unsigned high = dumpDot(mgr, f.Restrict(x), var+1, bddToNodeID, idToInameIndex, fp);
	unsigned low = dumpDot(mgr, f.Restrict(~x), var+1, bddToNodeID, idToInameIndex, fp);
	// Not sure if this duplicate check is needed...
	if (low == high) {
		return low;
	}

	// Create the node and update the maps
	unsigned idForf = getFreshID();
	bddToNodeID.push_back(std::make_pair(f, idForf));
	idToInameIndex.insert(std::make_pair(idForf, var));
	dumpEdge(idForf, high, true, fp);
	dumpEdge(idForf, low, false, fp);
	
	return idForf;
}

void dumpFooter(FILE* fp) {
	assert(fp != NULL);
	fprintf(fp, "\"0x0\" [label = \"0\"];\n");
	fprintf(fp, "\"0x1\" [label = \"1\"];\n");
	fprintf(fp, "}\n");
}

void dumpInames(FILE* fp, const std::map<unsigned, unsigned>& idToInameIndex, char const * const * inames) {
   	if (inames == NULL) {
		return;
	}
	assert(fp != NULL);
	
	std::set<unsigned> sortedInameIndexes;
	std::multimap<unsigned,unsigned> sameLists;

	// Replace the numeric node IDs with the corresponding inames
	// Build iname order and lists of nodes that have the same variable
	for (auto i : idToInameIndex) {
		dumpNode(i.first, fp);
		fprintf(fp, " [label = \"%s\"];\n", inames[i.second]);
		sortedInameIndexes.insert(i.second);
		sameLists.insert(std::make_pair(i.second, i.first));
	}

	// Create graph for the variable ordering (only variables that affect the output functions appear)
	fprintf(fp, "{ node [shape = plaintext];\n");
	fprintf(fp, "  edge [style = invis];\n");
	for (auto i : sortedInameIndexes) {
		fprintf(fp, "\" %s \" -> ", inames[i]);
	}
	fprintf(fp, "\"CONST NODES\";\n");
	fprintf(fp, "}\n");

	// Align the BDD nodes with the variable names that appear inside the nodes.
	// In other words, nodes with same variables appear at the same level.
	if (sameLists.size() == 0) {
		return;
	}
	unsigned currentKey = (*(sameLists.begin())).first;
	fprintf(fp, "{ rank = same; \" %s \";\n", inames[currentKey]);
	for (auto i : sameLists) {
		if (currentKey != i.first) {
			currentKey = i.first;
			fprintf(fp, "}\n");
			fprintf(fp, "{ rank = same; \" %s \";\n", inames[currentKey]);
		}
		dumpNode(i.second, fp);
		fprintf(fp, ";\n");
	}
	fprintf(fp, "}\n");
}

void DumpDot(bddMgr& mgr,
				const std::vector<BDD>& nodes,
				char const * const * inames, 
				char const * const * onames, 
				FILE* fp) {
	assert(nodes.size() && fp != NULL);
	std::list<std::pair<BDD,unsigned> > *bddToNodeID = new std::list<std::pair<BDD,unsigned> >();
	std::map<unsigned,unsigned> *idToInameIndex = new std::map<unsigned,unsigned>();
	
	dumpHeader(fp);
	dumpOnames(onames, nodes.size(), fp);
	dumpConstNodes(fp);

	unsigned o = 0;
	for(auto i : nodes) {
		unsigned id = dumpDot(mgr, i, 0, *bddToNodeID, *idToInameIndex, fp);
		if (onames != NULL ) {
			fprintf(fp, " \" %s \" -> ", onames[o++]);
			dumpNode(id, fp);
			fprintf(fp, "\n");
		}
	}
	
	dumpInames(fp, *idToInameIndex, inames);
	dumpFooter(fp);
	
	delete bddToNodeID;
	delete idToInameIndex;
}

