#include <stdlib.h>

#include "argtable2.h"
#include "bddObj.h"

/* NOTE: the last 2 paramters are optional
 */

extern void
bddToDot(bddMgr& mgr, 
	 BDD f, 
	 int num_vars, 
	 FILE* fp_dot,
	 int debug = 0, 
	 int verbose = 0);

/* The gVerbose and gDebug globals are used to toggle verbose and debug
 * output.  This standard is followed in all projects for cs486.
 */

int gVerbose = 0, gDebug = 0;

/* This is a simple test function.  Please feel free to change it to
 * meet your needs.
 */

void 
test_BDD(FILE *fp_dot) {
  bddMgr mgr (0, 0);
  BDD a = mgr.bddVar();
  BDD b = mgr.bddVar();
  BDD ap = mgr.bddVar();
  BDD bp = mgr.bddVar();

  BDD r = (~a & ~b & ap & bp) | (a & b & ~ap & ~bp) | (a & b & ap & ~bp) |
    (a & ~b & ap & ~bp);

  BDD l = (~ap & ~bp) | (ap & ~bp);
  BDD rest = r.Restrict(l);

  if (fp_dot) {
    bddToDot(mgr, rest, 4, fp_dot, gDebug, gVerbose);
  }
}

/* WARNING:
 *
 * This will not build until you have set ARCINCPATH and ARGLIBPATH
 * in the Makefile.
 *
 * See: 
 *
 * http://students.cs.byu.edu/~cs486ta/handouts/f04/bdd-package.html 
 *
 * For detailed instructions.
 */

int 
main(int argc, char* argv[]) {
  struct arg_lit *help, *ver, *debug, *verbose;
  struct arg_str *dump_dot;
  struct arg_end *end;

  int exitcode = 0, nerrors = 0;

  FILE *fp_dot = NULL;

  /* Build the argument syntax table. (see argtable.sourcefource.net)
   */

  void * argtable[] = {
        help = arg_lit0("h", "help",       
		       "display help and exit"),
         ver = arg_lit0(NULL, "version", 
		       "display the version information and exit"),
       debug = arg_lit0("g", "debug",      
		       "enables debug output (default off)"),
     verbose = arg_lit0("v", "verbose",    
		       "enables verbose output (default off)"),
    dump_dot = arg_str0("d", "dot", "<filename>",
		        "Writes the final bdd to a dot file for display"),
         end = arg_end(20)
  };

  /* Make sure each entry of the argument table was successfully
   * allocated.
   */

  if (arg_nullcheck(argtable) != 0) {
    printf("ERROR: failed to construct argtable; insufficient memory.\n");
    exitcode = 1;
    goto exit;
  }

  /* Parse the command line and record the number of errors.
   */

  nerrors = arg_parse(argc, argv, argtable);
  
  /* Each arg_lit structure has a field named count.  If count is not 0, then
   * it means the option is specified on the command line.
   * We give the help option precedence over the other options.  If it
   * is ever specified, then we output help and exit.
   */

  if (help->count > 0) {
    printf("Usage: %s ", argv[0]);
    arg_print_syntax(stdout, argtable, "\n");
    printf("Runs a test harness on the BDD package and optionally "
	   "outputs a dot file to visualize it.\n\n");
    arg_print_glossary(stdout, argtable, " %-25s %s\n");
    exitcode = 0;
    goto exit;
  }

  if (ver->count > 0) {
    printf("'%s' is a simple BDD package test harness.\n", argv[0]);
    printf("CS 686 (c) Fall 2004\n");
    exitcode = 0;
    goto exit;
  }

  /* nerrors indicates that the command line was not correctly parsed.  If
   * the user did not specify help or version, then we output the errors
   * and exit.
   */

  if (nerrors > 0) {
    arg_print_errors(stdout, end, argv[0]);
    exitcode = 1;
    goto exit;
  }
  
  /* Set all of the options relating to the program configuration
   */

  if (verbose->count) {
    gVerbose = 1;
    printf("Enabling verbose output\n");
  }

  if (debug->count) {
    gDebug = 1;
    if (gVerbose) {
      printf("Enabling debug output\n");
    }
  }

  if (dump_dot->count) {
    if (gVerbose) {
      printf("Enabling dump dot\n");
    }
    fp_dot = fopen(dump_dot->sval[0], "w");
    if (!fp_dot) {
      printf("ERROR: failed to open %s for writing\n", dump_dot->sval[0]);
    }
  }

  test_BDD(fp_dot);

 exit:

  if (fp_dot)
    fclose(fp_dot);

  //   deallocate each non-null entry in argtable[]
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
  return exitcode;

}
