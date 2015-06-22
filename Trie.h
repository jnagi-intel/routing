
//
//  Header File for Trie Imlpementation
//

#ifndef _TRIE_H_
#define _TRIE_H_

#include "Types.h"

#define NUM_BITS_IP_ADDR 32

#define __UINT_TO_IP_ADDR(Uint, IpAddr) \
    (IpAddr.IpAddr32Bit = Uint)

// Structure used to store IP Addresses
typedef struct _IP_ADDR
{
    union
    {
        UINT IpAddr32Bit;
        struct
        {
            UCHAR   IpAddr0 : 8;
            UCHAR   IpAddr1 : 8;
            UCHAR   IpAddr2 : 8;
            UCHAR   IpAddr3 : 8;
        };
    };
}IP_ADDR;

// Structure of a Trie Node
typedef struct _TRIE_NODE
{
    IP_ADDR IpAddr;
    char    LongestPrefix[33];
    UINT    NextHop;
    struct _TRIE_NODE *pLeftChild;
    struct _TRIE_NODE *pRightChild;
}TRIE_NODE, *PTRIE_NODE;


// Trie Context, includes the function pointers
typedef struct _TRIE_CONTEXT
{
    PTRIE_NODE   pTrieHeadNode;
    struct _TRIE_FN_TBL
    {
        PTRIE_NODE(*createTrieNode)(IP_ADDR Prefix, UINT NextHop);
        BOOLEAN(*insertTrieNode)(struct _TRIE_CONTEXT *pTrieContext, PTRIE_NODE pTrieNode);
        BOOLEAN(*mergeSubTries)(PTRIE_NODE pTrieNode);
        PTRIE_NODE(*findTrieNode)(struct _TRIE_CONTEXT* pTrieContext, IP_ADDR IpAddr);
        BOOLEAN(*createNewTrieHeadNode)(struct _TRIE_CONTEXT* pTrieContext);
    }TrieFnTbl;
}TRIE_CONTEXT, *PTRIE_CONTEXT;

// Function Prototype
PTRIE_CONTEXT createTrieContext();
BOOLEAN destroyTrieContext(PTRIE_CONTEXT *ppTrieContext);
#endif
