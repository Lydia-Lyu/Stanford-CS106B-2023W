#include "Huffman.h"
#include "priorityqueue.h"
#include "map.h"
#include "set.h"
#include "stack.h"
using namespace std;
int weightofinterval(EncodingTreeNode* interval, Map<char ,int>& weight);
/**
 * Deallocates all nodes in a Huffman tree. We've provided this helper function
 * to you since we also use it in our test driver and figured you might want
 * to make use of it.
 */
void deleteTree(EncodingTreeNode* tree) {
    if (tree != nullptr) {
        deleteTree(tree->zero);
        deleteTree(tree->one);
        delete tree;
    }
}

/**
 * Constructs a Huffman coding tree for the given string, using the algorithm
 * described in lecture.
 *
 * If the input string does not contain at least two different characters,
 * this function should report an error.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 */
int weightOfInterval(EncodingTreeNode* interval, Map<char, int> weights) {
    // Recursion: calcualate the sum of weight of all corresponding leaf nodes (only leaf nodes are valid)
    if (interval->zero == nullptr && interval->one == nullptr) {
        return weights[interval->ch];
    }
    return weightOfInterval(interval->zero, weights) + weightOfInterval(interval->one, weights);
}

EncodingTreeNode* huffmanTreeFor(const string& str) {
    // Construct a huffman tree
    // Store the coded tree node in a PriorityQueue (minimum): an intermediate step for constructing
    PriorityQueue<EncodingTreeNode*> pq;

    // Calculate the weight of characters of the string: frequency of occurrence
    Map<char, int> weights;
    for (auto s : str) {
        weights[s]++;
    }

    // Invalid input with only 1 characters
    if (weights.size() < 2) {
        error("Invalid input string!");
        return nullptr;
    }

    // Main process of constructing
    for (auto s : weights) {
        EncodingTreeNode* node = new EncodingTreeNode;
        node->ch = s;
        node->zero = nullptr;
        node->one = nullptr;
        pq.enqueue(node, weights[s]);
    }

    while (pq.size() > 1) {
        EncodingTreeNode* interval = new EncodingTreeNode;
        interval->ch = ' ';
        interval->zero = pq.dequeue();
        interval->one = pq.dequeue();
        pq.enqueue(interval, weightOfInterval(interval, weights));
    }

    // Output: return the root node of the huffman tree
    // The root node is the maximum-weight node - the last node
    return pq.peek();
}

/**
 * Given a Queue<Bit> containing a compressed message and a tree that was used
 * to encode those bits, decodes the bits back to the original message.
 *
 * You can assume the input tree is not null and is not a single character;
 * those are edge cases you don't need to handle.
 *
 * You can assume the input Queue<Bit> is well-formed in that every character
 * was encoded correctly, there are no stray bits in the Queue, etc.
 */

string decodeText(Queue<Bit>& bits, EncodingTreeNode* tree) {
    // Decode the queue of bits according to the Huffman encoding tree
    // Every character is identified when the path of bits reaching the leaf node along the tree
    string res = "";
    // Record the decoding progress along the bits
    int index = 0;
    // The pointer of decoding
    EncodingTreeNode* cur = tree;
    // The length of the initial queue
    int len = bits.size();

    while (index < len) {
        Bit direction = bits.dequeue();
        if (direction == 0 && cur->zero != nullptr) {
            cur = cur->zero;
        } else if (direction == 1 && cur->one != nullptr) {
            cur = cur->one;
        } else {
            error("Invalid bits input!");
            return "";
        }

        // Iteration
        index++;

        // Leaf node: end the path
        if (cur->zero == nullptr && cur->one == nullptr) {
            res += cur->ch;
            // Initialize the pointer
            cur = tree;
        }
    }

    return res;
}

/**
 * Given a string and a Huffman encoding tree, encodes that text using the tree
 * and outputs a Queue<Bit> corresponding to the encoded representation.
 *
 * The input tree will not be null and will not consist of a single node; these
 * are edge cases you don't have to handle. The input tree will contain all
 * characters that make up the input string.
 */
// For the encoding tree, generate a Map to recording codes for all characters in the tree (leaf nodes)
void getencodebook(EncodingTreeNode* tree, Map<char, string>& encodebook, string code) {
    // Recursion
    // Base case: leaf node
    if (tree->zero == nullptr && tree->one == nullptr) {
        encodebook[tree->ch] += code;
    }

    // Recursive Backtracking: similar, find all leaf nodes of the encoding tree
    // Check the next level validity to avoid dereferencing nullptr
    if (tree->zero) {
        getencodebook(tree->zero, encodebook, code + "0");
    }
    if (tree->one) {
        getencodebook(tree->one, encodebook, code + "1");
    }
}


Queue<Bit> encodeText(const string& str, EncodingTreeNode* tree) {
    Queue<Bit> bits;

    // Generate the encodebook
    Map<char, string> encodebook;
    getencodebook(tree, encodebook, "");

    // Encode using getencodebook
    for (auto s : str) {
        for (auto b : encodebook[s]) {
            if (b == '1') {
                bits.enqueue(1);
            }
            if (b == '0') {
                bits.enqueue(0);
            }
        }
    }

    return bits;
}

/**
 * Decodes the given Queue<Bit> and Queue<char> into a Huffman coding tree.
 *
 * You can assume that the input Queues are structured properly in that they
 * represent a legal encoding of a tree, that there aren't stray characters
 * or bits in them, etc.
 */
EncodingTreeNode* decodeTree(Queue<Bit>& bits, Queue<char>& leaves) {
    // Base case
    if (bits.isEmpty() || leaves.isEmpty()) {
        return nullptr;
    }

    // Pre-order traversals
    EncodingTreeNode* root = new EncodingTreeNode;
    // Leaf node
    if (bits.dequeue() == 0) {
        root->ch = leaves.dequeue();
        root->zero = nullptr;
        root->one = nullptr;
    } else {
        root->ch = ' ';
        // First build left subtree
        root->zero = decodeTree(bits, leaves);
        // (After extracting codes of left subtree) Build right subtree
        root->one = decodeTree(bits, leaves);
    }

    return root;
}

/**
 * Encodes the given Huffman tree as a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment handout.
 *
 * You can assume the input Queues are empty on entry to this function.
 *
 * You can assume that the Huffman tree provided to you is properly structured,
 * in that each internal node will have two children and only the characters in
 * the leaves matter, etc.
 */
void encodeTree(EncodingTreeNode* tree, Queue<Bit>& bits, Queue<char>& leaves) {
    // Stack: LIFO
    Stack<EncodingTreeNode*> q;
    q.push(tree);

    // Pre-order traversals
    while (!q.isEmpty()) {
        EncodingTreeNode* curr = q.pop();
        // Leaf node
        if (curr->zero == nullptr && curr->one == nullptr) {
            leaves.enqueue(curr->ch);
            bits.enqueue(0);
            continue;
        }

        bits.enqueue(1);
        if (curr->zero && curr->one) {
            // First right subtree
            q.push(curr->one);
            q.push(curr->zero);
        } else if (curr->zero) {
            q.push(curr->zero);
        } else if (curr->one) {
            q.push(curr->one);
        }
    }
}

/**
 * Compresses the given text string using Huffman coding, producing as output
 * a HuffmanResult containing the encoded tree and message.
 *
 * Your implementation of this function should report an error if there are
 * fewer than two distinct characters in the input string.
 */
HuffmanResult compress(const string& text) {
    HuffmanResult res;
    EncodingTreeNode* tree = huffmanTreeFor(text);
    res.messageBits = encodeText(text, tree);
    encodeTree(tree, res.treeBits, res.treeLeaves);
    deleteTree(tree);
    return res;
}

/**
 * Decompresses the given HuffmanResult and returns the string it represents.
 *
 * Your implementation may change the file parameter however it sees fit. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 * You can assume the input file is well-formed and was created by a correct
 * implementation of compress.
 */
string decompress(HuffmanResult& file) {
    EncodingTreeNode* tree = decodeTree(file.treeBits, file.treeLeaves);
    string res = decodeText(file.messageBits, tree);
    deleteTree(tree);
    return res;
}

/* * * * * * Test Cases Below This Point * * * * * */
#include "GUI/SimpleTest.h"

/* TODO: Add your own custom tests here! */













/* * * * * Provided Tests Below This Point * * * * */
#include <limits>
#include "set.h"

/* Utility function to test if a purported Huffman tree is indeed a Huffman tree.
 * Specifically, this checks that each internal node has either zero or two
 * children. There are other ways you could produce an invalid Huffman tree - for
 * example, by having uninitialized pointers or by linking in a cycle - but we
 * don't test for that here.
 */
bool isEncodingTree(EncodingTreeNode* tree) {
    /* The empty tree is not a Huffman tree. */
    if (tree == nullptr) return false;

    /* If we have one missing child, we should have two missing children. */
    if ((tree->zero == nullptr) != (tree->one == nullptr)) return false;

    /* If we have children at all, they need to be Huffman trees. */
    return tree->zero == nullptr || (isEncodingTree(tree->zero) && isEncodingTree(tree->one));
}

/* Utility function to test if two trees are equal. This is adapted from Section
 * Handout 8 and particularized to Huffman trees.
 */
bool areEqual(EncodingTreeNode* lhs, EncodingTreeNode* rhs) {
    /* Base case: If either is a leaf, both should be. */
    bool lhsLeaf = lhs->zero == nullptr && lhs->one == nullptr;
    bool rhsLeaf = rhs->zero == nullptr && rhs->one == nullptr;
    if (lhsLeaf || rhsLeaf) {
        return lhs->ch == rhs->ch && lhsLeaf == rhsLeaf;
    }

    /* Otherwise, they're both internal nodes. Check that they match. */
    return areEqual(lhs->zero, rhs->zero) && areEqual(lhs->one, rhs->one);
}

/* Utility function to return a string of all possible characters. */
string pangrammaticString() {
    string result;

    char ch = numeric_limits<char>::min();
    result += ch;
    do {
        ch++;
        result += ch;
    } while (ch != numeric_limits<char>::max());

    return result;
}

/* Utility function that makes an inefficient but still valid encoding tree
 * for the given characters.
 */
EncodingTreeNode* strandTreeRec(const string& text, size_t index) {
    if (index == text.size()) error("No characters provided to strandTreeFor.");

    /* We always get a leaf node. */
    EncodingTreeNode* leaf = new EncodingTreeNode {
        text[index], nullptr, nullptr
    };

    /* Last character? If so, that's all. */
    if (index + 1 == text.size()) return leaf;

    /* Otherwise, build a larger tree. */
    else return new EncodingTreeNode {
        ' ', leaf, strandTreeRec(text, index + 1)
    };
}

/* Given a string, returns a coding tree for the letters of that string. The
 * coding tree has the following shape, hence the name:
 *
 *        *
 *       / \
 *    ch1   *
 *         / \
 *       ch2  *
 *            ...
 *             *
 *            / \
 *        chN-1 chN
 */
EncodingTreeNode* strandTreeFor(const string& text) {
    /* Deduplicate characters in the string. */
    Set<char> used;
    for (char ch: text) {
        used += ch;
    }

    return strandTreeRec(string(used.begin(), used.end()), 0);
}

PROVIDED_TEST("huffmanTreeFor reports errors on invalid inputs.") {
    EXPECT_ERROR(huffmanTreeFor(""));    // No characters
    EXPECT_ERROR(huffmanTreeFor("a"));   // One character
    EXPECT_ERROR(huffmanTreeFor("aaa")); // One character
}

PROVIDED_TEST("huffmanTreeFor builds tree for two characters.") {
    EncodingTreeNode* reference = new EncodingTreeNode {
        ' ', new EncodingTreeNode {'a', nullptr, nullptr}, new EncodingTreeNode {'b', nullptr, nullptr}
    };

    EncodingTreeNode* tree = huffmanTreeFor("aaabbbb");
    EXPECT(isEncodingTree(tree));
    EXPECT(areEqual(tree, reference));

    deleteTree(reference);
    deleteTree(tree);
}

PROVIDED_TEST("huffmanTreeFor works on the full range of characters.") {
    /* Get a string of all possible characters, then pair them off and see what we find. */
    string allChars = pangrammaticString();
    for (size_t i = 0; i < allChars.size(); i += 2) {
        string toEncode;
        toEncode += allChars[i];
        toEncode += allChars[i + 1];
        toEncode += allChars[i + 1];

        EncodingTreeNode* reference = new EncodingTreeNode {
            ' ',
            new EncodingTreeNode {allChars[i], nullptr, nullptr},
            new EncodingTreeNode {allChars[i + 1], nullptr, nullptr}
        };

        EncodingTreeNode* tree = huffmanTreeFor(toEncode);
        EXPECT(isEncodingTree(tree));
        EXPECT(areEqual(tree, reference));

        deleteTree(reference);
        deleteTree(tree);
    }
}

PROVIDED_TEST("huffmanTreeFor uses cumulative weights (v1).") {
    /* This tree:
     *                 *
     *                / \
     *               *   D
     *              / \
     *             C   *
     *                / \
     *               A   B
     */
    EncodingTreeNode* reference = new EncodingTreeNode {
        '*',
            new EncodingTreeNode { '!',
                new EncodingTreeNode { 'C', nullptr, nullptr },
                new EncodingTreeNode { '?',
                    new EncodingTreeNode { 'A', nullptr, nullptr },
                    new EncodingTreeNode { 'B', nullptr, nullptr }
                }
            },
            new EncodingTreeNode { 'D', nullptr, nullptr }
    };

    /* Ax2, Bx3, Cx4, Dx10 */
    EncodingTreeNode* tree = huffmanTreeFor("AABBBCCCCDDDDDDDDDD");
    EXPECT(isEncodingTree(tree));
    EXPECT(areEqual(tree, reference));

    deleteTree(reference);
    deleteTree(tree);
}

PROVIDED_TEST("huffmanTreeFor uses cumulative weights (v2).") {
    /*
     *          *
     *       /     \
     *      *       *
     *     / \     / \
     *    D   E   F   *
     *               / \
     *              C   *
     *                 / \
     *                A   B
     */
    EncodingTreeNode* reference =new EncodingTreeNode {
        ' ',
        new EncodingTreeNode {
            ' ',
            new EncodingTreeNode{ 'D', nullptr, nullptr },
            new EncodingTreeNode{ 'E', nullptr, nullptr }
        },
        new EncodingTreeNode {
            ' ',
            new EncodingTreeNode{ 'F', nullptr, nullptr },
            new EncodingTreeNode {
                ' ',
                new EncodingTreeNode{ 'C', nullptr, nullptr },
                new EncodingTreeNode{
                    ' ',
                    new EncodingTreeNode{ 'A', nullptr, nullptr },
                    new EncodingTreeNode{ 'B', nullptr, nullptr },
                }
            }
        }
    };

    /* Ax2, Bx3, Cx4, Dx6, Ex7, Fx8 */
    EncodingTreeNode* tree = huffmanTreeFor("AABBBCCCCDDDDDDEEEEEEEFFFFFFFF");
    EXPECT(isEncodingTree(tree));

    EXPECT(areEqual(tree, reference));

    deleteTree(reference);
    deleteTree(tree);
}

PROVIDED_TEST("decodeText works on small sample.") {
    /* This tree:
     *                 *
     *                / \
     *               O   *
     *                  / \
     *                 *   N
     *                / \
     *               M   S
     */
    EncodingTreeNode* tree = new EncodingTreeNode {
        '*',
            new EncodingTreeNode { 'O', nullptr, nullptr },
            new EncodingTreeNode { '*',
                new EncodingTreeNode{ '*',
                    new EncodingTreeNode { 'M', nullptr, nullptr },
                    new EncodingTreeNode { 'S', nullptr, nullptr }
                },
                new EncodingTreeNode{ 'N', nullptr, nullptr }
            }
    };

    /* What you get if you encode MONSOON with this tree. */
    Queue<Bit> bits = { 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1 };

    EXPECT_EQUAL(decodeText(bits, tree), "MONSOON");

    deleteTree(tree);
}

PROVIDED_TEST("Can decode all char values.") {
    /* All possible characters. */
    string allChars = pangrammaticString();

    /* Try decoding all pairs of adjacent characters. */
    for (size_t i = 0; i < allChars.size(); i += 2) {
        string expected;
        expected += allChars[i];
        expected += allChars[i + 1];
        expected += allChars[i + 1];

        EncodingTreeNode* tree = new EncodingTreeNode {
            ' ',
            new EncodingTreeNode {allChars[i], nullptr, nullptr},
            new EncodingTreeNode {allChars[i + 1], nullptr, nullptr}
        };

        /* Decode the bitstream 011, which should map back to the expected
         * string.
         */
        Queue<Bit> bits = { 0, 1, 1 };
        EXPECT_EQUAL(decodeText(bits, tree), expected);

        deleteTree(tree);
    }
}

PROVIDED_TEST("encodeText works on small sample.") {
    /* This tree:
     *                 *
     *                / \
     *               O   *
     *                  / \
     *                 *   N
     *                / \
     *               M   S
     */
    EncodingTreeNode* tree = new EncodingTreeNode {
           '*',
           new EncodingTreeNode { 'O', nullptr, nullptr },
               new EncodingTreeNode { '*',
               new EncodingTreeNode{ '*',
               new EncodingTreeNode { 'M', nullptr, nullptr },
               new EncodingTreeNode { 'S', nullptr, nullptr }
            },
            new EncodingTreeNode{ 'N', nullptr, nullptr }
        }
    };

    /* What you get if you encode MONSOON with this tree. */
    Queue<Bit> expected = { 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1 };

    EXPECT_EQUAL(encodeText("MONSOON", tree), expected);

    deleteTree(tree);
}

PROVIDED_TEST("Can encode all char values.") {
    /* All possible characters. */
    string allChars = pangrammaticString();

    /* Try encoding all pairs of adjacent characters. */
    for (size_t i = 0; i < allChars.size(); i += 2) {
        string toEncode;
        toEncode += allChars[i];
        toEncode += allChars[i + 1];
        toEncode += allChars[i + 1];

        EncodingTreeNode* tree = new EncodingTreeNode {
                ' ',
                new EncodingTreeNode {allChars[i], nullptr, nullptr},
                new EncodingTreeNode {allChars[i + 1], nullptr, nullptr}
        };

        /* See what bits we get back. We should get 011, since the first
         * character has code 0 and the second has code 1.
         */
        Queue<Bit> bits = encodeText(toEncode, tree);
        Queue<Bit> expected = { 0, 1, 1 };

        EXPECT_EQUAL(bits, expected);

        deleteTree(tree);
    }
}

PROVIDED_TEST("decodeText undoes encodeText on range of sample strings.") {
    Vector<string> testCases = {
        "THAT THAT IS IS THAT THAT IS NOT IS NOT IS THAT IT IT IS",
        "AABAAABBABAAABAAAA",
        ":-) :-D XD <(^_^)>",
        pangrammaticString(),
    };

    for (string test: testCases) {
        /* Use a silly encoding scheme, but one that works regardless of what
         * characters are provided.
         */
        EncodingTreeNode* tree = strandTreeFor(test);
        EXPECT(isEncodingTree(tree));

        Queue<Bit> bits = encodeText(test, tree);
        string result = decodeText(bits, tree);

        EXPECT_EQUAL(test.size(), result.size());
        EXPECT_EQUAL(test, result);

        deleteTree(tree);
    }
}

PROVIDED_TEST("Can decode an example tree.") {
    /* This encodes this tree:
     *
     *                 *
     *                / \
     *               *   C
     *              / \
     *             A   B
     */
    Queue<Bit>  bits   = { 1, 1, 0, 0, 0 };
    Queue<char> leaves = { 'A', 'B', 'C' };

    EncodingTreeNode* tree = decodeTree(bits, leaves);
    EXPECT(isEncodingTree(tree));

    /* Confirm this is the right tree. */
    EncodingTreeNode* expected = new EncodingTreeNode {
        '*',
            new EncodingTreeNode {
                '*',
                new EncodingTreeNode { 'A', nullptr, nullptr },
                new EncodingTreeNode { 'B', nullptr, nullptr },
            },
            new EncodingTreeNode { 'C', nullptr, nullptr }
    };

    EXPECT(areEqual(tree, expected));

    deleteTree(tree);
    deleteTree(expected);
}

PROVIDED_TEST("Can decode trees using all possible char values.") {
    /* All possible characters. */
    string allChars = pangrammaticString();

    /* Try encoding all pairs of adjacent characters. */
    for (size_t i = 0; i < allChars.size(); i += 2) {
        EncodingTreeNode* expected = new EncodingTreeNode {
            ' ',
            new EncodingTreeNode {allChars[i], nullptr, nullptr},
            new EncodingTreeNode {allChars[i + 1], nullptr, nullptr}
        };
        Queue<Bit>  treeBits   = { 1, 0, 0 };
        Queue<char> treeLeaves = { allChars[i], allChars[i + 1] };

        EncodingTreeNode* tree = decodeTree(treeBits, treeLeaves);
        EXPECT(isEncodingTree(tree));
        EXPECT(areEqual(tree, expected));

        deleteTree(tree);
        deleteTree(expected);
    }
}

PROVIDED_TEST("Can encode an example tree.") {
    /* Build an encoding tree for "ABBCCCC." It should look like this:
     *
     *                 *
     *                / \
     *               *   C
     *              / \
     *             A   B
     *
     * This will compress down to
     *
     *        11000
     *        ABC
     */
    EncodingTreeNode* tree = huffmanTreeFor("ABBCCCC");

    Queue<Bit>  bits;
    Queue<char> leaves;

    encodeTree(tree, bits, leaves);

    Queue<Bit>  expectedBits   = { 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'A', 'B', 'C' };

    EXPECT_EQUAL(bits,   expectedBits);
    EXPECT_EQUAL(leaves, expectedLeaves);

    deleteTree(tree);
}

PROVIDED_TEST("Can encode trees using all possible char values.") {
    /* All possible characters. */
    string allChars = pangrammaticString();

    /* Try encoding all pairs of adjacent characters. */
    for (size_t i = 0; i < allChars.size(); i += 2) {
        EncodingTreeNode* tree = new EncodingTreeNode {
            ' ',
            new EncodingTreeNode {allChars[i], nullptr, nullptr},
            new EncodingTreeNode {allChars[i + 1], nullptr, nullptr}
        };

        /* See what we get back. It should be the bitstring 100 (root with
         * two children) and the two leaves, in order.
         */
        Queue<Bit>  treeBits;
        Queue<char> treeLeaves;

        Queue<Bit>  expectedBits = { 1, 0, 0 };
        Queue<char> expectedLeaves = { allChars[i], allChars[i + 1] };

        encodeTree(tree, treeBits, treeLeaves);
        EXPECT_EQUAL(treeBits, expectedBits);
        EXPECT_EQUAL(treeLeaves, expectedLeaves);

        deleteTree(tree);
    }
}

PROVIDED_TEST("decodeTree undoes encodeTree on sample strings.") {
    /* Make a Huffman tree for the string of all characters. */
    EncodingTreeNode* sourceTree = huffmanTreeFor(pangrammaticString());
    EXPECT(isEncodingTree(sourceTree));

    /* Encode, then decode it. */
    Queue<Bit>  bits;
    Queue<char> leaves;
    encodeTree(sourceTree, bits, leaves);

    EncodingTreeNode* resultTree = decodeTree(bits, leaves);
    EXPECT(isEncodingTree(resultTree));
    EXPECT(areEqual(sourceTree, resultTree));

    deleteTree(sourceTree);
    deleteTree(resultTree);
}

PROVIDED_TEST("Can decompress a small sample file.") {
    HuffmanResult file = {
        { 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0 },
        { 'u', 'k', 'p', 'n', 'a', 'm', 'h' },
        { 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1,
          0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0,
          0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0 }
    };

    EXPECT_EQUAL(decompress(file), "humuhumunukunukuapuaa");
}

PROVIDED_TEST("Compress reports errors on bad inputs.") {
    EXPECT_ERROR(compress(""));
    EXPECT_ERROR(compress("A"));
    EXPECT_ERROR(compress("AAAA"));
}

PROVIDED_TEST("Can compress a small sample file.") {
    HuffmanResult file = compress("ABANANAABANDANA");
    Queue<Bit>  treeBits    = { 1, 1, 1, 0, 0, 0, 0 };
    Queue<char> treeChars   = { 'D', 'B', 'N', 'A' };
    Queue<Bit>  messageBits = { 1, 0, 0, 1, 1, 0, 1, 1, 0,
                                1, 1, 1, 0, 0, 1, 1, 0, 1,
                                0, 0, 0, 1, 0, 1, 1 };

    EXPECT_EQUAL(file.treeBits, treeBits);
    EXPECT_EQUAL(file.treeLeaves, treeChars);
    EXPECT_EQUAL(file.messageBits, messageBits);
}

PROVIDED_TEST("Compress undoes decompress on a range of strings.") {
    Vector<string> testCases = {
        "THAT THAT IS IS THAT THAT IS NOT IS NOT IS THAT IT IT IS",
        "AABAAABBABAAABAAAA",
        ":-) :-D XD <(^_^)>",
        pangrammaticString(),
    };

    for (string test: testCases) {
        HuffmanResult file = compress(test);
        string result = decompress(file);

        EXPECT_EQUAL(result.size(), test.size());
        EXPECT_EQUAL(test, result);
    }
}
