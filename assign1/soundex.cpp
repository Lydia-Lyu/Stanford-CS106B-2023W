/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <cctype>
#include <fstream>
#include <string>
#include <map>
#include "console.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include "vector.h"
#include "SimpleTest.h"
#include "soundex.h"
using namespace std;

/* This function is intended to return a string which
 * includes only the letter characters from the original
 * (all non-letter characters are excluded)
 *
 * WARNING: The provided code is buggy!
 *
 * Add student test cases to identify which inputs to this function
 * are incorrectly handled. Then, remove this comment and
 * replace it with a description of the bug you fixed.
 */


/* Extracts only letters from a string
 */
string lettersOnly(string s) {
    string result = charToString(s[0]);
    for (int i = 1; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}


/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
string soundex(string s) {
    map<char, char> code_dict = {{'A', '0'}, {'E', '0'}, {'I', '0'}, {'O', '0'}, {'U', '0'},
                                 {'H', '0'}, {'W', '0'}, {'Y', '0'}, {'B', '1'}, {'F', '1'},
                                 {'P', '1'}, {'V', '1'}, {'C', '2'}, {'G', '2'}, {'J', '2'},
                                 {'K', '2'}, {'Q', '2'}, {'S', '2'}, {'X', '2'}, {'Z', '2'},
                                 {'D', '3'}, {'T', '3'}, {'L', '4'}, {'M', '5'}, {'N', '5'},
                                 {'R', '6'}};

    Vector<char> code;

    // Extract all letters (only) in upper case
    string sl = toUpperCase(lettersOnly(s));

    // Extract the first letter in upper case
    char fst = sl[0];

    // Initialize the first element of the code (in this step, '0' is not discard)
    code.add(code_dict[fst]);


    // Change all the other letters into number
    for (int i = 1; i < sl.size(); i++) {
        // Escape the duplicate letters: the next letter is same with current letter
        if (i < sl.size() && code_dict[sl[i]] == code_dict[sl[i-1]]) {
            continue;
        }
        // Escape zero code
        char tmp = code_dict[sl[i]];
        if (tmp == '0') {
            continue;
        }
        code.add(tmp);
        // The maximum length of the code is 4
        if (code.size() == 4) {
            break;
        }
    }

    // Fulfill the code with '0' if its length < 4
    int diff = 4 - code.size();
    for (int i = 0; i < diff; i++) {
        code.add('0');
    }

    // Replace the first element with first letter
    code[0] = fst;

    // Return the code (currently as `Vector`) in the string format
    string res;
    for (char c : code) {
        res += c;
    }
    return res;
}


/* Read in string from the specific file path
 */
void soundexSearch(string filepath) {
    // This provided code opens the specified file
    // and reads the lines into a vector of strings
    ifstream in;
    Vector<string> allNames;

    if (openFile(in, filepath)) {
        allNames = readLines(in);
    }
    cout << "Read file " << filepath << ", "
         << allNames.size() << " names found." << endl;

    // The names read from file are now stored in Vector allNames

    string surname;
    Vector<string> result;

    while (true) {
        surname = getLine("Enter a surname (RETURN to quit): ");
        if (surname == "") {
            cout << "Quit." << endl;
            return;
        }

        string surname_code = soundex(surname);
        cout << "Soundex code for " << surname << " is: " << surname_code <<endl;

        for (string name : allNames) {
            if (surname_code == soundex(name)) {
                result.add(name);
            }
        }

        result.sort();
        cout << "Matched names are: " << result << endl;
        result.clear();
    }
}



/* * * * * * Test Cases * * * * * */

// TODO: add your STUDENT_TEST test cases here!


/* Please not add/modify/remove the PROVIDED_TEST entries below.
 * Place your student tests cases above the provided tests.
 */

PROVIDED_TEST("Test exclude of punctuation, digits, and spaces") {
    string s = "O'Hara";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "tldr");
}

PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}
