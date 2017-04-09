#ifndef _BDDOBJ_
#define _BDDOBJ_

#include <stdio.h>

/************************************************************************
 * DO NOT CHANGE THIS #ifdef; enables testing with CUDD
 ************************************************************************/
#ifdef USE_CUDD

// This is for those who wish to test their BDD package with CUDD

#include "cuddObj.hh"
typedef Cudd bddMgr;

#else

#include <vector>
#include <string>
#include <iostream>

/* g_debug is used to toggle on and off any debug output you embed in your
 * code.  If it is nonzero, then output the debug information.  Otherwise
 * do not output anything.
 */

extern int g_debug;

/* g_verbose is used to toggle on and off any verbose output you embed in
 * your code.  This is not debug output, just more information than nothing.
 */

extern int g_verbose;

/* This is the default size of each bddMgr's unique and compute tables
 * respectively.
 */

static const int BDDMGR_UNIQUE_SLOTS = 1000;
static const int BDDMGR_CACHE_SLOTS = 2000;

/************************************************************************
 * CLASSES YOU NEED TO IMPLEMENT
 ***********************************************************************/

/* BDD
 *
 * Wrapper for pointers in to the bddMgr data structures.  Each
 * BDD needs a hook to know which bddMgr it belongs too.
 *
 * IMPORTANT: you do need to add checks for mismatched managers and
 * empty BDDs that are uninitialized or have no manager from the
 * default constructor.  Correct behavior is to output a error message
 * and exit.
 *
 * The destructor should _not_ delete memory managed by the bddMgr.
 * It only frees memory related to the BDD wrapper around the 
 * accessors into the bddMgr's data structures.
 */

class bddMgr;
class TableEntry;

class BDD {
  
  friend class bddMgr;

protected:
  
  bddMgr* ptrMgr;
  TableEntry* entryPtr;
  
public:

  BDD(bddMgr *mgr, TableEntry* entry);

  BDD(const BDD &from);

  ~BDD();

  int 
  operator==(const BDD& other) const;

  BDD& 
  operator=(const BDD& right);
  
  BDD 
  operator~();

  BDD 
  operator&(const BDD& other);

  BDD 
  operator|(const BDD& other);

  BDD 
  Ite(const BDD& g, const BDD& h, unsigned int limit = 0) const;

  BDD 
  Restrict(const BDD& c) const;

  BDD 
  GetLow() const;

  BDD
  GetHigh() const;

  void 
  PrintIDRow() const;

  unsigned int
  GetID() const;

  unsigned int 
  GetVar() const; 

  TableEntry*
  getEntryAddr() const;

  bddMgr* 
  getMgrAddr() const;

};

class TableEntry {
    friend class bddMgr;

public:
    unsigned int _variable;
    int _low;
    int _high;
    unsigned int _id;

    TableEntry(unsigned int variable, int low, int high, unsigned int id);
    ~TableEntry();

    unsigned int getvar();
    int getLow();
    int getHigh();
    unsigned int getID();
    void printRow();
};


/* bddMgr
 *
 * The bddMgr manages the unique and compute tables.  It keeps track
 * of all the bdd variables.  BDDs are just wrappers to protect
 * accessors into the bddMgr data structures (pointer, etc.).
 *
 */

class bddMgr {

  friend class BDD;

protected:
  std::vector<TableEntry*> entries;
  std::vector<TableEntry*> variables;
  int verbose;
  int debug;

public:

  bddMgr(unsigned int numVars = 0,
	 unsigned int numSlots = BDDMGR_UNIQUE_SLOTS,
	 unsigned int cacheSize = BDDMGR_CACHE_SLOTS);

  /* Makes a complete copy of a bddMgr.  BDDs for x should not
   * work in the copy (i.e., the copy has its own memory etc.)
   */  

  bddMgr(bddMgr& x);
  
  ~bddMgr();
  
  void 
  makeVerbose() {verbose = 1;}
  
  void 
  makeDebug() {debug = 1;};

  /* This should use the copy constructor to make a complete
   * independent memory copy of the bddMgr.
   */

  bddMgr& 
  operator=(const bddMgr& right);

  /* Creates a new variable in this manager.  Variables begin at 0
   * and increase montonically by 1 with each call to bddVar.
   */

  BDD 
  bddVar();

  /* Returns the BDD for variable index.  If the variable index does
   * not exist, then it creates the variable and returns the BDD.
   * 
   * If index < 0, then fail gracefully.
   */

  BDD 
  bddVar(int index);

  BDD 
  bddOne();

  BDD 
  bddZero();

/*
 * Gets or creates a BDD with the variable, High, and Low BDD's
 */
  BDD 
  getOrCreate(unsigned int var, int high, int low);

  int 
  getTopVar(const BDD& f, const BDD& g, const BDD& h);

  BDD 
  basicCofactor(int var, const BDD& x, unsigned int pos);
  
  void
  PrintTable();

  void 
  PrintVars();
};

/************************************************************************
 * END OF CLASSES YOU NEED TO IMPLELMENT
 ***********************************************************************/

#endif
#endif
