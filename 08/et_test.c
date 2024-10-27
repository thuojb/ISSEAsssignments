/*
 * et_test.c
 * 
 * Test cases for ExprTree
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>  // strlen
#include <ctype.h>   // isblank
#include <math.h>    // fabs

#include "expr_tree.h"

// Checks that value is true; if not, prints a failure message and
// returns 0 from this function
#define test_assert(value) {                                            \
    if (!(value)) {                                                     \
      printf("FAIL %s[%d]: %s\n", __FUNCTION__, __LINE__, #value);      \
      goto test_error;                                                  \
    }                                                                   \
  }

// For comparing floating point values
#define EPSILON 1e-6
#define double_eq(a, b) (fabs((a) - (b)) < EPSILON)

/*
 * Tests the ET_value, ET_node, and ET_free functions.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_node_free()
{
  ExprTree tree = NULL;

  tree = ET_value(23400000);
  test_assert( ET_depth(tree) == 1 );
  ET_free(tree);

  tree = ET_value(-1000);
  test_assert( ET_depth(tree) == 1 );
  ET_free(tree);

  tree = ET_node(OP_ADD, ET_value(1), ET_value(3));
  test_assert( ET_depth(tree) == 2 );
  ET_free(tree);

  return 1;

 test_error:
  ET_free(tree);
  return 0;
}


/*
 * Tests the ET_count function
 */
int test_count()
{
    ExprTree tree = NULL;
    
    // Test single node
    tree = ET_value(5.0);
    test_assert(ET_count(tree) == 1);
    ET_free(tree);

    // Test binary operation
    tree = ET_node(OP_ADD, ET_value(1), ET_value(2));
    test_assert(ET_count(tree) == 3);  // root + 2 leaves
    ET_free(tree);

    // Test unary operation
    tree = ET_node(UNARY_NEGATE, ET_value(1), NULL);
    test_assert(ET_count(tree) == 2);  // root + 1 leaf
    ET_free(tree);

    // Test complex tree
    tree = ET_node(OP_MUL,
                   ET_node(OP_ADD, ET_value(1), ET_value(2)),
                   ET_node(OP_SUB, ET_value(3), ET_value(4)));
    test_assert(ET_count(tree) == 7);  // root + 2 ops + 4 leaves
    ET_free(tree);

    return 1;

test_error:
    ET_free(tree);
    return 0;
}

/*
 * Tests the ET_depth function
 */
int test_depth()
{
    ExprTree tree = NULL;
    
    // Test single node
    tree = ET_value(5.0);
    test_assert(ET_depth(tree) == 1);
    ET_free(tree);

    // Test simple binary tree
    tree = ET_node(OP_ADD, ET_value(1), ET_value(2));
    test_assert(ET_depth(tree) == 2);
    ET_free(tree);

    // Test unary operation
    tree = ET_node(UNARY_NEGATE, ET_value(1), NULL);
    test_assert(ET_depth(tree) == 2);
    ET_free(tree);

    // Test unbalanced tree
    tree = ET_node(OP_ADD,
                   ET_node(OP_MUL,
                          ET_node(OP_SUB, ET_value(1), ET_value(2)),
                          ET_value(3)),
                   ET_value(4));
    test_assert(ET_depth(tree) == 4);
    ET_free(tree);

    return 1;

test_error:
    ET_free(tree);
    return 0;
}

/*
 * Tests the ET_evaluate function
 */
int test_evaluate()
{
    ExprTree tree = NULL;
    
    // Test single value
    tree = ET_value(5.0);
    test_assert(double_eq(ET_evaluate(tree), 5.0));
    ET_free(tree);

    // Test basic operations
    tree = ET_node(OP_ADD, ET_value(2), ET_value(3));
    test_assert(double_eq(ET_evaluate(tree), 5.0));
    ET_free(tree);

    tree = ET_node(OP_SUB, ET_value(5), ET_value(3));
    test_assert(double_eq(ET_evaluate(tree), 2.0));
    ET_free(tree);

    tree = ET_node(OP_MUL, ET_value(4), ET_value(3));
    test_assert(double_eq(ET_evaluate(tree), 12.0));
    ET_free(tree);

    tree = ET_node(OP_DIV, ET_value(10), ET_value(2));
    test_assert(double_eq(ET_evaluate(tree), 5.0));
    ET_free(tree);

    tree = ET_node(OP_POWER, ET_value(2), ET_value(3));
    test_assert(double_eq(ET_evaluate(tree), 8.0));
    ET_free(tree);

    // Test unary negation
    tree = ET_node(UNARY_NEGATE, ET_value(5), NULL);
    test_assert(double_eq(ET_evaluate(tree), -5.0));
    ET_free(tree);

    // Test complex expression: (2 + 3) * (4 - 1)
    tree = ET_node(OP_MUL,
                   ET_node(OP_ADD, ET_value(2), ET_value(3)),
                   ET_node(OP_SUB, ET_value(4), ET_value(1)));
    test_assert(double_eq(ET_evaluate(tree), 15.0));
    ET_free(tree);

    return 1;

test_error:
    ET_free(tree);
    return 0;
}

/*
 * Tests the ET_tree2string function
 */
int test_tree2string()
{
    ExprTree tree = NULL;
    char buf[128];
    size_t result;
    
    // Test single value
    tree = ET_value(5.0);
    result = ET_tree2string(tree, buf, sizeof(buf));
    test_assert(strcmp(buf, "5") == 0);
    test_assert(result == strlen(buf));
    ET_free(tree);

    // Test negative value
    tree = ET_value(-5.0);
    result = ET_tree2string(tree, buf, sizeof(buf));
    test_assert(strcmp(buf, "-5") == 0);
    test_assert(result == strlen(buf));
    ET_free(tree);

    // Test basic operations
    tree = ET_node(OP_ADD, ET_value(2), ET_value(3));
    result = ET_tree2string(tree, buf, sizeof(buf));
    test_assert(strcmp(buf, "(2 + 3)") == 0);
    test_assert(result == strlen(buf));
    ET_free(tree);

    // Test unary negation
    tree = ET_node(UNARY_NEGATE, ET_value(5), NULL);
    result = ET_tree2string(tree, buf, sizeof(buf));
    test_assert(strcmp(buf, "(-5)") == 0);
    test_assert(result == strlen(buf));
    ET_free(tree);

    // Test double unary negation
    tree = ET_node(UNARY_NEGATE,
                   ET_node(UNARY_NEGATE, ET_value(5), NULL),
                   NULL);
    result = ET_tree2string(tree, buf, sizeof(buf));
    test_assert(strcmp(buf, "(--5)") == 0);
    test_assert(result == strlen(buf));
    ET_free(tree);

    // Test complex expression: (2 + 3) * (4 - 1)
    tree = ET_node(OP_MUL,
                   ET_node(OP_ADD, ET_value(2), ET_value(3)),
                   ET_node(OP_SUB, ET_value(4), ET_value(1)));
    result = ET_tree2string(tree, buf, sizeof(buf));
    test_assert(strcmp(buf, "((2 + 3) * (4 - 1))") == 0);
    test_assert(result == strlen(buf));
    ET_free(tree);

    // Test buffer size limits
    tree = ET_node(OP_ADD, ET_value(1), ET_value(2));
    result = ET_tree2string(tree, buf, 5);  // Only room for "(1 $"
    test_assert(result == 4);
    test_assert(buf[3] == '$');
    test_assert(buf[4] == '\0');
    ET_free(tree);

    return 1;

test_error:
    ET_free(tree);
    return 0;
}

/*
 * Tests from the assignment document
 */
int test_assignment_examples()
{
    ExprTree tree = NULL;
    char buf[128];
    double result;

    // Test: 6.5 * (4 + 3)
    tree = ET_node(OP_ADD, ET_value(4.0), ET_value(3.0));
    tree = ET_node(OP_MUL, ET_value(6.5), tree);
    ET_tree2string(tree, buf, sizeof(buf));
    result = ET_evaluate(tree);
    test_assert(double_eq(result, 45.5));
    ET_free(tree);

    // Test: -0.125 (as a value)
    tree = ET_value(-0.125);
    ET_tree2string(tree, buf, sizeof(buf));
    result = ET_evaluate(tree);
    test_assert(double_eq(result, -0.125));
    ET_free(tree);

    // Test: -(-0.125)
    tree = ET_node(UNARY_NEGATE, ET_value(-0.125), NULL);
    ET_tree2string(tree, buf, sizeof(buf));
    result = ET_evaluate(tree);
    test_assert(double_eq(result, 0.125));
    ET_free(tree);

    // Test: -(--0.125)
    tree = ET_node(UNARY_NEGATE, 
                   ET_node(UNARY_NEGATE, ET_value(-0.125), NULL),
                   NULL);
    ET_tree2string(tree, buf, sizeof(buf));
    result = ET_evaluate(tree);
    test_assert(double_eq(result, -0.125));
    ET_free(tree);

    // Test: 2^3 / (1.3 + 2.7)
    tree = ET_node(OP_ADD, ET_value(1.3), ET_value(2.7));
    tree = ET_node(OP_DIV, 
                   ET_node(OP_POWER, ET_value(2), ET_value(3)),
                   tree);
    ET_tree2string(tree, buf, sizeof(buf));
    result = ET_evaluate(tree);
    test_assert(double_eq(result, 2.0));
    ET_free(tree);

    // Test: (20 - 10) - (30 - 25)
    tree = ET_node(OP_SUB, ET_value(20), ET_value(10));
    tree = ET_node(OP_SUB, tree,
                   ET_node(OP_SUB, ET_value(30), ET_value(25)));
    ET_tree2string(tree, buf, sizeof(buf));
    result = ET_evaluate(tree);
    test_assert(double_eq(result, 5.0));
    ET_free(tree);

    return 1;

test_error:
    ET_free(tree);
    return 0;
}

int main()
{
  int passed = 0;
  int num_tests = 0;

  num_tests++; passed += test_node_free(); 
  num_tests++; passed += test_count();
  num_tests++; passed += test_depth();
  num_tests++; passed += test_evaluate();
  num_tests++; passed += test_tree2string();
  num_tests++; passed += test_assignment_examples();

  printf("Passed %d/%d test cases\n", passed, num_tests);
  fflush(stdout);
  return 0;
}

