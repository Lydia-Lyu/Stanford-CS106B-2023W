#include "HumanPyramids.h"
#include "error.h"
#include <map.h>
#include <gridlocation.h>
using namespace std;

/* TODO: Refer to HumanPyramids.h for more information about what this function should do.
 * Then, delete this comment.
 */

// Version 1
// double weightOnBackOf(int row, int col) {
//     if (row < 0 || col < 0 || col > row) {
//         error("Index out of range!");
//     }

//     if (row == 0 && col == 0) {
//         return 0;
//     } else if (col == 0) {
//         return (weightOnBackOf(row - 1, col) + 160)/2;
//     } else if (col == row) {
//         return (weightOnBackOf(row -1, col - 1) + 160)/2;
//     } else {
//         return (weightOnBackOf(row -1, col - 1) + 160)/2 + (weightOnBackOf(row - 1, col) + 160)/2;
//     }
// }


// Version 2
double weightOnBackOf(int row, int col, Map<GridLocation, double>& table) {
    GridLocation loc(row, col);

    if (row < 0 || col < 0 || col > row) {
        error("Index out of range!");
    }

    if (row == 0 && col == 0) {
        return 0;
    } else if (table.containsKey(loc)) {
        return table[loc];
    } else {
        double weight = 0;
        if (col == 0) {
            weight = (weightOnBackOf(row - 1, col, table) + 160)/2;
        } else if (col == row) {
            weight = (weightOnBackOf(row -1, col - 1, table) + 160)/2;
        } else {
            weight = (weightOnBackOf(row -1, col - 1, table) + 160)/2 + (weightOnBackOf(row - 1, col, table) + 160)/2;
        }
        table[loc] = weight;
        return weight;
    }
}

double weightOnBackOf(int row, int col) {
    Map<GridLocation, double> table;
    return weightOnBackOf(row, col, table);
}





/* * * * * * Test Cases * * * * * */
#include "GUI/SimpleTest.h"

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */




///* * * * * * Test cases from the starter files below this point. * * * * * */

PROVIDED_TEST("Milestone 1: Check Person E from the handout.") {
   /* Person E is located at row 2, column 1. */
   EXPECT_EQUAL(weightOnBackOf(2, 1), 240);
}

PROVIDED_TEST("Milestone 1: Function reports errors in invalid cases.") {
   EXPECT_ERROR(weightOnBackOf(-1, 0));
   EXPECT_ERROR(weightOnBackOf(10, 11));
   EXPECT_ERROR(weightOnBackOf(-1, 10));
}

PROVIDED_TEST("Milestone 2: Memoization is implemented (should take under a second)") {
   /* TODO: Yes, we are asking you to make a change to this test case! Delete the
    * line immediately after this one - the one that starts with SHOW_ERROR - once
    * you have implemented memoization to test whether it works correctly.
    */
   // SHOW_ERROR("This test is configured to always fail until you delete this line from\n         HumanPyramids.cpp. Once you have implemented memoization and want\n         to check whether it works correctly, remove the indicated line.");

   /* Do not delete anything below this point. :-) */

   /* This will take a LONG time to complete if memoization isn't implemented.
    * We're talking "heat death of the universe" amounts of time. :-)
    *
    * If you did implement memoization but this test case is still hanging, make
    * sure that in your recursive function (not the wrapper) that your recursive
    * calls are to your new recursive function and not back to the wrapper. If you
    * call the wrapper again, you'll get a fresh new memoization table rather than
    * preserving the one you're building up in your recursive exploration, and the
    * effect will be as if you hadn't implemented memoization at all.
    */
   EXPECT(weightOnBackOf(100, 50) >= 10000);
}
