We test the ITE implementation with a few tests (that basically run it through the homework problems, as well as a few edge cases). This is found in the `test_BDD()` function.

The exhaustive test is performed by encoding a relatively complex set (8 bit encoding with eight values) and then testing for membership and non-membership. This is in the `test_BDD_set()` function.

Both will run when the the program is compiled and run.

To see debugging output set `DEBUG` to `true` in `bddObj.h`
