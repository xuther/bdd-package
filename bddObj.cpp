#include "bddObj.h"


/* Global variables to turn on and off different output you embed in your 
 * code.
 */

int g_debug = 0;
int g_verbose = 0;

/************************************************************************
 * MEMBER FUNCTIONS YOU NEED TO IMPLELMENT
 ***********************************************************************/

BDD::BDD(bddMgr *mgr, TableEntry* entry) {
    ptrMgr = mgr;
    entryPtr = entry;
}

TableEntry*
BDD::getEntryAddr() const {
    return entryPtr;
}

bddMgr*
BDD::getMgrAddr() const {
    return ptrMgr;
}

BDD::BDD(const BDD &from) {
    ptrMgr = from.getMgrAddr();
    entryPtr = from.getEntryAddr();
}
 
BDD::~BDD() {
}
 
int 
BDD::operator==(const BDD& other) const {
    if (entryPtr -> getID() == other.GetID()) {
        return 1;
    }
    else {
        return 0; 
    }  
}

BDD& 
BDD::operator=(const BDD& right) {
    ptrMgr = right.getMgrAddr();
    entryPtr = right.getEntryAddr();

    return *this;
}

unsigned int 
BDD::GetID() const {
    return entryPtr -> getID();
}  

BDD 
BDD::operator~() {
  return Ite(ptrMgr -> bddZero(), ptrMgr -> bddOne());
}

BDD 
BDD::operator&(const BDD& other) {
    return Ite(other, ptrMgr -> bddZero());
}

BDD 
BDD::operator|(const BDD& other) {
  return Ite(ptrMgr -> bddOne(), other);
}

BDD 
BDD::Restrict(const BDD& c) const{
    int high = c.GetHigh().GetID();
    int low = c.GetLow().GetID();

    if (DEBUG == true)
        std::cout << "High; " << high << " Low: " << low << std::endl;

    //check to make sure it's either an identity or a ~identity
    if ((high != 0 && high != 1) || (low != 0 && low != 1)) {
        std::cout << "Invalid restriction bdd" << std::endl;
        return *this;
    }
    if (DEBUG == true)
        std::cout << " Beginning cofactor for restrict..." << std::endl; 
    return ptrMgr -> basicCofactor(c.GetVar(), *this, high);
}

BDD
BDD::GetLow() const {
    if (DEBUG == true)
      std::cout << "getting low" << std::endl;
  int val = entryPtr -> getLow();

  if(val == 0) {
      return ptrMgr -> bddZero();
  }
  if(val == 1) {
      return ptrMgr -> bddOne();
  }

    if (DEBUG == true)
        std::cout << val << std::endl;
  return ptrMgr -> getRow(val-2);
}

BDD
BDD::GetHigh() const {
    if (DEBUG == true)
      std::cout << "getting high" << std::endl;
  int val = entryPtr -> getHigh();

  if(val == 0) {
       if (DEBUG == true)
          std::cout << val << std::endl;
      return ptrMgr -> bddZero();
  }
  if(val == 1) {
       if (DEBUG == true)
          std::cout << val << std::endl;
      return ptrMgr -> bddOne();
  }

    if (DEBUG == true)
        std::cout << val << std::endl;
  return ptrMgr -> getRow(val-2);
}

unsigned int 
BDD::GetVar() const {
    return entryPtr -> getvar();
}

BDD
BDD::Ite(const BDD& g, const BDD& h, unsigned int limit) const {
    if (DEBUG == true)
        std::cout << "starting ITE on: " << GetID() << ", " << g.GetID() << ", " << h.GetID() << std::endl;
    //Recursive Call
    //Base Case
    if (*this == ptrMgr -> bddOne()){
        if (DEBUG == true)
            std::cout << "Found one" << std::endl;
        return BDD(g.getMgrAddr(), g.getEntryAddr());
    }
    if (*this == ptrMgr -> bddZero()){
        if (DEBUG == true)
            std::cout << "Found zero" << std::endl;
        return BDD(h.getMgrAddr(), h.getEntryAddr());
    }
    if (g == h) {
        if (DEBUG == true)
            std::cout << "Homogeonous options" << std::endl;
        return BDD(g.getMgrAddr(), g.getEntryAddr());
    }  

    int v = ptrMgr -> getTopVar(*this, g, h);
    if (DEBUG == true) {
        std::cout << "Variable for factoring " << v << std::endl;
        std::cout << "f: " << std::endl;
        PrintIDRow();
        std::cout << "g: " << std::endl;
        g.PrintIDRow();
        std::cout << "h: " << std::endl;
        h.PrintIDRow();
    }

    //recursive calls to get the left and right side
    BDD t = ptrMgr -> basicCofactor(v,*this,0).Ite(ptrMgr -> basicCofactor(v,g,0),ptrMgr -> basicCofactor(v,h,0));
    if (DEBUG == true)
        std::cout << "Found t:" <<std::endl;
    if (DEBUG == true)
        t.PrintIDRow();

    //recurseive calls tog et the left and right side
    BDD e = ptrMgr -> basicCofactor(v,*this,1).Ite(ptrMgr -> basicCofactor(v,g,1),ptrMgr -> basicCofactor(v,h,1));
    if (DEBUG == true)
        std::cout << "Found e:" <<std::endl;
    if (DEBUG == true)
        e.PrintIDRow();

    if (t == e) {
        return e;
    }

    return ptrMgr -> getOrCreate(v, t.GetID(), e.GetID());
}

void 
BDD::PrintIDRow() const {
    entryPtr->printRow();
}

BDD
bddMgr::getOrCreate(unsigned int var, int low, int high) {

    for (int i = 0; i < entries.size(); i++) {
        if (var == entries[i] -> getvar() &&
                high == entries[i] -> getHigh() &&
                low == entries[i] -> getLow()) {
            return BDD(this, entries[i]);
        }
    }
    TableEntry *v = new TableEntry(var, low, high, entries.size());

    entries.push_back(v);
    return BDD(this, v);
}

BDD
bddMgr::basicCofactor(int var, const BDD& x, unsigned int pos) {
    if (DEBUG == true) {
        std::cout << "starting cofactor..." << std::endl;
        std::cout << "Cofactor: ";
        x.PrintIDRow();
        }

    if (bddZero() == x || bddOne() == x)  {
    if (DEBUG == true) {
        std::cout << "Cofactor called on true/false BDD" << std::endl;
        x.PrintIDRow(); }
        return x;
    }

    if (x.GetVar() != var) {
    if (DEBUG == true) {
        std::cout << "Variable not contained" << std::endl;
        x.PrintIDRow(); }
        return x; 
    }
    if (pos ==1) {
        if (DEBUG == true)
            std::cout << "Variable conainted. High Cofactor specified. " << std::endl;
        BDD val = x.GetHigh();
        if (DEBUG == true)
            val.PrintIDRow();
        return val;
    }
    else  {
        if (DEBUG == true)
            std::cout << "Variable contained. Low cofactor specified." << std::endl;
            BDD val = x.GetLow();
        if (DEBUG == true)
            val.PrintIDRow();
            return val; 
    }
}

bddMgr::bddMgr(unsigned int numVars,
	       unsigned int numSlots,
	       unsigned int cacheSize) {

      TableEntry *zero = new TableEntry(0, -1, -1, 0);
      TableEntry *one = new TableEntry(1, -1,-1,1);
      entries.push_back(zero);
      entries.push_back(one);
      variables.push_back(zero);
      variables.push_back(one);

}
  
bddMgr::bddMgr(bddMgr& x) {
}

//Entries should handle the deletion of all the entries
bddMgr::~bddMgr() {
    for (int i = 0; i < entries.size(); i++) {
        delete(entries[i]);
    }
}
  
bddMgr& 
bddMgr::operator=(const bddMgr& right) {
  return *this;
}

BDD 
bddMgr::bddVar() {
  TableEntry *toAdd = new TableEntry(entries.size(), 0, 1, variables.size());
  entries.push_back(toAdd);
  variables.push_back(toAdd);
  
  return BDD(this, toAdd);
}

BDD 
bddMgr::bddVar(int index) {
    if (DEBUG == true)
        std::cout << "Getting Identity BDD at index: " << index +2 << std::endl;
    if (DEBUG == true)
        variables[index + 2] -> printRow();
    return BDD(this, variables[index + 2]);
}

BDD 
bddMgr::getRow(int index) {
    if (DEBUG == true) 
        std::cout << "Getting BDD at index: " << index +2 << std::endl;
    if (DEBUG == true) 
        entries[index + 2] -> printRow();
    return BDD(this, entries[index + 2]);
}

BDD 
bddMgr::bddOne() {
  return BDD(this, entries[1]);
}

BDD 
bddMgr::bddZero() {
  return BDD(this, entries[0]);
}

void bddMgr::PrintTable() {
    std::cout << "Var\tLow\tHigh\tID" << std::endl;
    for (int i = 0; i < entries.size(); i++) {
        entries[i] -> printRow();
    }
}

void bddMgr::PrintVars() {
    std::cout << "Var\tLow\tHigh\tID" << std::endl;
    for (int i = 0; i< variables.size(); i++) {
        variables[i] -> printRow();
    }
}

int bddMgr::getTopVar(const BDD& f, const BDD& g, const BDD& h) {
    unsigned int fID = f.GetVar();
    unsigned int gID = g.GetVar();
    unsigned int hID = h.GetVar();

    if (DEBUG == true) 
       std::cout << "Getting top var out of " << fID << " , " << gID << " , " << hID << " , " << std::endl;

   unsigned int minValue = 0;
   if (fID != 0 == fID != 1) {
       minValue = fID;
   }
   if (gID != 0 && gID != 1) {
       if (gID < minValue)
           minValue = gID;
   }
   if (hID != 0 && hID != 1) {
       if (hID < minValue)
           minValue = hID;
   }

    if (DEBUG == true) 
       std::cout << "Top var was " << minValue << std::endl;

   return minValue;
}


/***********************************************************************
 * Table entry
 **********************************************************************/

TableEntry::TableEntry(unsigned int variable, int low, int high, unsigned int id) {
   _variable = variable;
   _low = low;
   _high = high;
   _id = id;
}
TableEntry::~TableEntry() {
}

unsigned int
TableEntry::getvar() {
    return _variable;
}

int TableEntry::getLow() {
    return _low;
}

int TableEntry::getHigh() {
    return _high;
}

unsigned int TableEntry::getID() {
    return _id;
}

void TableEntry::printRow() {
    std::cout << _variable << "\t" << _low << "\t" << _high << "\t" << _id << std::endl;
}


/************************************************************************
 * END OF MEMBER FUNCTIONS YOU NEED TO IMPLELMENT
 ***********************************************************************/
