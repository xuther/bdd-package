#include "bddObj.h"

/* Global variables to turn on and off different output you embed in your 
 * code.
 */

int g_debug = 0;
int g_verbose = 0;

/************************************************************************
 * MEMBER FUNCTIONS YOU NEED TO IMPLELMENT
 ***********************************************************************/

BDD::BDD() {
}

BDD::BDD(const BDD &from) {
}
 
BDD::~BDD() {
}
 
int 
BDD::operator==(const BDD& other) const {
  return 0;
}

BDD& 
BDD::operator=(const BDD& right) {
  return *this;
}

BDD 
BDD::operator~() {
  return *this;
}

BDD 
BDD::operator&(const BDD& other) {
  return *this;
}

BDD 
BDD::operator|(const BDD& other) {
  return *this;
}

BDD 
BDD::Restrict(const BDD& c) const{
  return *this;
}

bddMgr::bddMgr(unsigned int numVars,
	       unsigned int numSlots,
	       unsigned int cacheSize) {
}
  
bddMgr::bddMgr(bddMgr& x) {
}
  
bddMgr::~bddMgr() {
}
  
bddMgr& 
bddMgr::operator=(const bddMgr& right) {
  return *this;
}

BDD 
bddMgr::bddVar() {
  return BDD();
}

BDD 
bddMgr::bddVar(int index) {
  return BDD();
}

BDD 
bddMgr::bddOne() {
  return BDD();
}

BDD 
bddMgr::bddZero() {
  return BDD();
}

/************************************************************************
 * END OF MEMBER FUNCTIONS YOU NEED TO IMPLELMENT
 ***********************************************************************/

