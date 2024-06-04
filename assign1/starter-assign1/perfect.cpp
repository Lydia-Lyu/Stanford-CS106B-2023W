/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "console.h"
#include <iostream>
#include "SimpleTest.h"
#include "perfect.h"
using namespace std;

/* The divisorSum function takes one argument `n` and calculates the
 * sum of proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division using the modulus operator %
 *
 * Note: the C++ long type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* The isPerfect function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* The findPerfects function takes one argument `stop` and performs
 * an exhaustive search for perfect numbers over the range 1 to `stop`.
 * Each perfect number found is printed to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* 寻找因子的范围缩小到n的平方根
 * header comment.
 */
long smarterSum(long n) {
    long total = 0;
    if (n == 1) {
        return total;
    }
    for (long divisor = 1; divisor * divisor <= n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
            if (divisor != 1 && divisor * divisor != n) {
                total += n / divisor;
            }
        }
    }
    return total;
}

/*
 * header comment.
 */
bool isPerfectSmarter(long n) {
    return (n != 0) && (n == smarterSum(n));
}

/*
 * header comment.
 */
void findPerfectsSmarter(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        // Progress bar
        if (num % 10000 == 0) {
            cout << "." << flush;
        }
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* Euclid
 * header comment.
 */
long findNthPerfectEuclid(long n) {
    long cnt = 0;
    long k = 0;
    while (cnt < n) {
        k++;
        // calculate m
        long m = pow(2, k) - 1;
        // check m as prime or composite
        if (smarterSum(m) == 1) {
            cnt++;
        }
    }
    return pow(2, k-1) * (pow(2, k) - 1);
}


/* * * * * * Test Cases * * * * * */

// TODO: add your STUDENT_TEST test cases here!

/*
 * Here is sample test demonstrating how to use a loop to set the input sizes
 * for a sequence of time trials.
 */
//STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {
//    int smallest = 1000, largest = 8000;
//    for (int size = smallest; size <= largest; size *= 2) {
//        TIME_OPERATION(size, findPerfects(size));
//    }
//}


/* Please not add/modify/remove the PROVIDED_TEST entries below.
 * Place your student tests cases above the provided tests.
 */

PROVIDED_TEST("Euclid nth perfect number") {
    EXPECT(isPerfect(findNthPerfectEuclid(1)));
    EXPECT(isPerfect(findNthPerfectEuclid(2)));
    EXPECT(isPerfect(findNthPerfectEuclid(3)));
}

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(smarterSum(1), 0);
    EXPECT_EQUAL(smarterSum(6), 6);
    EXPECT_EQUAL(smarterSum(12), 16);
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfectSmarter(6));
    EXPECT(isPerfectSmarter(28));
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfectSmarter(12));
    EXPECT(!isPerfectSmarter(98765));
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfectSmarter(0));
    EXPECT(!isPerfectSmarter(1));
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
    EXPECT(isPerfectSmarter(33550336));
}

PROVIDED_TEST("Time trial of findPerfects on input size 1000") {
    TIME_OPERATION(1000, findPerfects(1000));
}

PROVIDED_TEST("Time trial of findPerfects on input size 10000") {
    TIME_OPERATION(10000, findPerfects(10000));
}
