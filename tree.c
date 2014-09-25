#include "tree.h"

Tree *tree_new() {
  return calloc(sizeof(Tree), 1);
}

Tree *tree_copy(Tree *tree) {
  Tree *copy = tree_new();
  copy->left = tree->left ? tree_copy(tree->left) : NULL;
  copy->right = tree->right ? tree_copy(tree->right) : NULL;
  copy->key = tree->key ? keycpy(tree->key) : NULL;
  copy->val = tree->val;
  return copy;
}

Tree *tree_free(Tree *tree) {
  if (tree->left)
    tree_free(tree->left);
  if (tree->right)
    tree_free(tree->right);
  if (tree->key)
    free(tree->key);
  free(tree);
  return NULL;
}

Val tree_fetch(Tree *tree, Key key) {
  if (tree->key) {
    int cmp = keycmp(key, tree->key);
    if (cmp < 0)
      return tree->left ? tree_fetch(tree->left, key) : ZERO;
    if (cmp > 0)
      return tree->right ? tree_fetch(tree->right, key) : ZERO;
    return tree->val;
  }
  return ZERO;
}

Val tree_store(Tree *tree, Key key, Val val) {
  if (tree->key) {
    int cmp = keycmp(key, tree->key);
    if (cmp < 0)
      return tree_store(tree->left ? tree->left : (tree->left = tree_new()), key, val);
    if (cmp > 0)
      return tree_store(tree->right ? tree->right : (tree->right = tree_new()), key, val);
    return tree->val = val;
  }
  tree->key = keycpy(key);
  return tree->val = val;
}

Val tree_dot(Tree *a, Tree *b) {
  Path *pa = path_min(path_new(a)), *pb = path_new(b);
  Val prod = ZERO;
  if (a->key && b->key)
    while (pa && (a = pa->node) && (pb = path_find(pb, a->key, RELATION_GTEQ)) && (b = pb->node)) {
      if (keycmp(a->key, b->key) == 0) {
        valdot(prod, a->val, b->val);
        pa = path_next(pa);
      } else {
        pa = path_find(pa, b->key, RELATION_GTEQ);
      }
    }
  if (pa)
    path_free(pa);
  if (pb)
    path_free(pb);
  return prod;
}

Path *path_new(Tree *tree) {
  Path *path = calloc(sizeof(Path), 1);
  path->node = tree;
  return path;
}

Path *path_copy(Path *path) {
  Path *copy = path_new(path->node);
  copy->parent = path->parent ? path_copy(path->parent) : NULL;
  copy->lower = path->lower;
  copy->upper = path->upper;
  return copy;
}

Path *path_free(Path *path) {
  if (path->parent)
    path_free(path->parent);
  free(path);
  return NULL;
}

Path *path_push(Path *parent, Tree *tree, Tree *lower, Tree *upper) {
  Path *path = path_new(tree);
  path->parent = parent;
  path->lower = lower;
  path->upper = upper;
  return path;
}

Path *path_pop(Path *path) {
  Path *parent = path->parent;
  free(path);
  return parent;
}


Path *path_min(Path *path) {
  while (path->node && path->node->left)
    path = path_left(path);
  return path;
}

Path *path_max(Path *path) {
  while (path->node && path->node->right)
    path = path_right(path);
  return path;
}

Path *path_left(Path *path) {
  return path_push(path, path->node->left, path->lower, path->node);
}

Path *path_right(Path *path) {
  return path_push(path, path->node->right, path->node, path->upper);
}

Path *path_lower(Path *path) {
  Tree *lower = path->lower;
  while (path && path->node != lower)
    path = path_pop(path);
  return path;
}

Path *path_upper(Path *path) {
  Tree *upper = path->upper;
  while (path && path->node != upper)
    path = path_pop(path);
  return path;
}

Path *path_find(Path *path, Key key, Relation rel) {
  Tree *node;

  while (!path_contains(path, key))
    path = path_pop(path);

  while ((node = path->node) && node->key) {
    int cmp = keycmp(key, node->key);
    if (cmp == 0) {
      if (rel & RELATION_EQ)
        return path;
      if (rel & RELATION_LT)
        return node->left ? path_max(path_left(path)) : path_lower(path);
      if (rel & RELATION_GT)
        return node->right ? path_min(path_right(path)) : path_upper(path);
    } else if (cmp < 0) {
      path = path_left(path);
    } else if (cmp > 0) {
      path = path_right(path);
    }
  }

  if (rel & RELATION_LT)
    return path_lower(path);
  if (rel & RELATION_GT)
    return path_upper(path);

  return path;
}

Path *path_prev(Path *path) {
  Relation rel = path->node ? RELATION_LT : RELATION_LTEQ;
  while (!path->node)
    path = path_pop(path);
  return path_find(path, path->node->key, RELATION_LT);
}

Path *path_next(Path *path) {
  Relation rel = path->node ? RELATION_GT : RELATION_GTEQ;
  while (!path->node)
    path = path_pop(path);
  return path_find(path, path->node->key, rel);
}

bool path_contains(Path *path, Key key) {
  Tree *lower = path->lower, *upper = path->upper;
  return ((!lower || keycmp(lower->key, key) < 0) &&
          (!upper || keycmp(upper->key, key) > 0));
}
