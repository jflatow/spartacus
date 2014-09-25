#ifndef TREE_H
#define TREE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define keycmp strcmp
#define keycpy(k) strcpy(malloc(strlen(k) + 1), k)
#define valdot(v, a, b) v += a * b

#define ZERO 0

#define RELATION_NONE 0
#define RELATION_EQ   1
#define RELATION_GT   2
#define RELATION_GTEQ 3
#define RELATION_LT   4
#define RELATION_LTEQ 5
#define RELATION_LTGT 6
#define RELATION_ANY  7

typedef unsigned char Relation;

typedef char * Key;
typedef double Val;

typedef struct _Tree {
  Key key;
  Val val;
  struct _Tree *left;
  struct _Tree *right;
} Tree;

Tree *tree_new();
Tree *tree_copy(Tree *tree);
Tree *tree_free(Tree *tree);
Val tree_fetch(Tree *tree, Key key);
Val tree_store(Tree *tree, Key key, Val val);
Val tree_dot(Tree *a, Tree *b);

typedef struct _Path {
  struct _Path *parent;
  Tree *node;
  Tree *lower;
  Tree *upper;
} Path;

Path *path_new(Tree *tree);
Path *path_copy(Path *path);
Path *path_free(Path *path);
Path *path_push(Path *path, Tree *node, Tree *lower, Tree *upper);
Path *path_pop(Path *path);
Path *path_min(Path *path);
Path *path_max(Path *path);
Path *path_left(Path *path);
Path *path_right(Path *path);
Path *path_upper(Path *path);
Path *path_lower(Path *path);
Path *path_find(Path *path, Key key, Relation rel);
Path *path_prev(Path *path);
Path *path_next(Path *path);
bool path_contains(Path *path, Key key);

#endif /* TREE_H */