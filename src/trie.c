#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct key {
    char a;
}key_t;

typedef struct val {
    void *val;
}val_t;

typedef struct map {
    key_t key;
    val_t val;
}map_t;

typedef struct trie_node {
    map_t *child;
    bool is_leaf;
}trn_t;

#define NUM_KEYS 26

void setup_keys(trn_t *trn)
{
    for (int i = 0; i < NUM_KEYS; i++) {
        trn->child[i].key.a = 'a'+i;
    }

    return;
}

void setup_vals(trn_t *trn)
{
    for (int i = 0; i < NUM_KEYS; i++) {
        trn->child[i].val.val = NULL;
    }

    return;
}

trn_t *createTrieNode()
{
    trn_t *trn = malloc(sizeof(trn_t));
    
    trn->child = malloc(NUM_KEYS * sizeof(map_t));

    trn->is_leaf = false;

    setup_keys(trn);
    setup_vals(trn);

    return trn;
}

void insertTrie(trn_t *node, char *key)
{
    int len = strlen(key);

    trn_t *temp = node;

    for (int i = 0 ; i < len; i++)
    {
        if (temp->child[key[i] - 'a'].val.val == NULL) {
            temp->child[key[i] - 'a'].val.val = createTrieNode();
        }
        temp = (trn_t *)temp->child[key[i] - 'a'].val.val;
    }

    temp->is_leaf = true;

    return;
}

bool searchTrie(trn_t *root, char *key)
{
    int len = strlen(key);

    trn_t *temp = root;

    for (int i = 0; i < len; i++) {
        if (temp->child[key[i]-'a'].val.val == NULL)
            return false;
        temp = (trn_t *)temp->child[key[i]-'a'].val.val;
    }

    return (temp != NULL && temp->is_leaf);
}

bool isFreeNode(trn_t *trn)
{
    for(int i = 0; i < NUM_KEYS; i++) {
        if (trn->child[i].val.val)
            return false;
    }

    return true;
}

bool deleteTrie(trn_t *root, char *key, int idx, int key_len)
{
    if (root) {
        if (idx == key_len)
        {
            if (root->is_leaf) {
                root->is_leaf = false;
                if (isFreeNode(root))
                    return true;
                return false;
            }
        }
        else {
            if (deleteTrie(root->child[key[idx]-'a'].val.val, key, idx+1, key_len)) {
                trn_t *temp = root->child[key[idx]-'a'].val.val;
                free(temp->child);
                free(temp);
                root->child[key[idx]-'a'].val.val = NULL;

                return !root->is_leaf && isFreeNode(root);
            }
        }
    }

    return false;
}

int main()
{
    char keys[][8] = {"the", "a", "there", "answer", "any",
                        "by", "bye", "their"};

    char output[][32] = {"Not present in trie", "Present in trie"};

    trn_t *root = createTrieNode();

    for (int i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
        insertTrie(root, keys[i]);
    }

    printf("%s --- %s\n", "the", output[searchTrie(root, "the")] );
    printf("%s --- %s\n", "these", output[searchTrie(root, "these")] );
    printf("%s --- %s\n", "their", output[searchTrie(root, "their")] );
    printf("%s --- %s\n", "thaw", output[searchTrie(root, "thaw")] );

    deleteTrie(root, "their", 0, strlen("their"));

    printf("%s --- %s\n", "their", output[searchTrie(root, "their")] );

    return 0;
}
