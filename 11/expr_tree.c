/*
 * expr_tree.c
 * 
 * A dynamically allocated tree to handle arbitrary arithmetic
 * expressions
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "expr_tree.h"

#define LEFT 0
#define RIGHT 1

struct _expr_tree_node {
  ExprNodeType type;
  union {
    struct _expr_tree_node *child[2];
    double value;
  } n;
};


/*
 * Convert an ExprNodeType into a printable character
 *
 * Parameters:
 *   ent    The ExprNodeType to convert
 * 
 * Returns: A single character representing the ent
 */
static char ExprNodeType_to_char(ExprNodeType ent)
{
    switch (ent) {
        case VALUE: return 'V';
        case UNARY_NEGATE: return '-';
        case OP_ADD: return '+';
        case OP_SUB: return '-';
        case OP_MUL: return '*';
        case OP_DIV: return '/';
        case OP_POWER: return '^';
        default: return '?';
    }
}


// Documented in .h file
ExprTree ET_value(double value)
{
  ExprTree new = (ExprTree) malloc(sizeof(struct _expr_tree_node));
  assert(new);
  
  new->type = VALUE;
  new->n.value = value;

  return new;
}


// Documented in .h file
ExprTree ET_node(ExprNodeType op, ExprTree left, ExprTree right)
{
  ExprTree new = (ExprTree) malloc(sizeof(struct _expr_tree_node));
  assert(new);
  
  new->type = op;
  new->n.child[LEFT] = left;
  new->n.child[RIGHT] = right;

  return new;
}


// Documented in .h file
void ET_free(ExprTree tree)
{

  if (tree == NULL)
    return;
  
  if (tree->type != VALUE) {
    ET_free(tree->n.child[LEFT]);
    ET_free(tree->n.child[RIGHT]);
  }
    
  free(tree);
  // return;
}


// Documented in .h file
int ET_count(ExprTree tree)
{
  if (tree == NULL)
    return 0;

  if (tree->type == VALUE)
    return 1;

  return 1 + ET_count(tree->n.child[LEFT]) + ET_count(tree->n.child[RIGHT]);
}


// Documented in .h file
int ET_depth(ExprTree tree)
{
  assert(tree);
  
  if (tree->type == VALUE)
    return 1;
    
  int left_depth = ET_depth(tree->n.child[LEFT]);
  int right_depth = ET_depth(tree->n.child[RIGHT]);
    
    return 1 + (left_depth > right_depth ? left_depth : right_depth);

  //return 0;
}


// Documented in .h file
double ET_evaluate(ExprTree tree)
{
  assert(tree);
  
  if (tree->type == VALUE)
    return tree->n.value;
    
  double left_val = ET_evaluate(tree->n.child[LEFT]);
  double right_val = tree->n.child[RIGHT] ? ET_evaluate(tree->n.child[RIGHT]) : 0.0;
    
  switch (tree->type) {
    case UNARY_NEGATE:
      return -left_val;
    case OP_ADD:
      return left_val + right_val;
    case OP_SUB:
      return left_val - right_val;
    case OP_MUL:
      return left_val * right_val;
    case OP_DIV:
      return left_val / right_val;
    case OP_POWER:
      return pow(left_val, right_val);
    default:
      return 0.0;
    }
}


// Documented in .h file
size_t ET_tree2string(ExprTree tree, char *buf, size_t buf_sz)
{
  assert(tree);
  
  if (buf_sz < 1) {
    return 0;
  }
    
  size_t written = 0;
    
  // Handle leaf nodes (values)
  if (tree->type == VALUE) {
    written = snprintf(buf, buf_sz, "%.2f", tree->n.value);
    if (written >= buf_sz) {
      buf[buf_sz - 1] = '$';
      return buf_sz - 1;
    }
    return written;
  }
    
    // Handle interior nodes (operators)
  if (buf_sz < 2) {
    buf[0] = '$';
    return 1;
  }
    
  // Add opening parenthesis
  buf[written++] = '(';
    
  // Add left subtree
  size_t left_written = ET_tree2string(tree->n.child[LEFT],
                                      buf + written, 
                                      buf_sz - written);
  written += left_written;
    
  // Add operator
  if (written + 1 < buf_sz) {
    buf[written++] = ExprNodeType_to_char(tree->type);
  } else {
      buf[buf_sz - 1] = '$';
      return buf_sz - 1;
  }
    
  // Add right subtree for binary operators
  if (tree->type != UNARY_NEGATE) {
    size_t right_written = ET_tree2string(tree->n.child[RIGHT], 
                                        buf + written, 
                                        buf_sz - written);
    written += right_written;
  }
    
  // Add closing parenthesis
  if (written + 1 < buf_sz) {
      buf[written++] = ')';
      buf[written] = '\0';
  } else {
      buf[buf_sz - 1] = '$';
      return buf_sz - 1;
  }
    
  return written;
}


