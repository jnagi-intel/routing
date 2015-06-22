
//
//  Header File for Fibonacci Heap handling
//

#ifndef _FIBHEAP_H_
#define _FIBHEAP_H_

#include "Types.h"

// Fibonacci Heap Node Definition
typedef struct _FIB_HEAP_NODE
{
    UINT                    VertexIndex;
    INT                     Key;
    UINT                    Degree;
    struct _FIB_HEAP_NODE   *pParent;
    struct _FIB_HEAP_NODE   *pLeftSib;
    struct _FIB_HEAP_NODE   *pRightSib;
    struct _FIB_HEAP_NODE   *pChild;
    BOOLEAN                 bChildCut;
}FIB_HEAP_NODE, *PFIB_HEAP_NODE;

// Fibonacci Heap Context definition
typedef struct _FIB_HEAP_CONTEXT
{
    UINT                NumNodesFibHeap;
    UINT                NumNodesRootList;
    UINT                NumNodesSspGraph;
    FIB_HEAP_NODE       *pMinFibHeapNode;
    FIB_HEAP_NODE       *pFibHeapNodeList;
    struct _FIB_HEAP_FN_TBL
    {
        BOOLEAN(*insertFibHeapNode)(struct _FIB_HEAP_CONTEXT *pFibHeapContext, UINT VertexIndex, INT Key);
        PFIB_HEAP_NODE(*extractMinFibHeapNode)(struct _FIB_HEAP_CONTEXT *pFibHeapContext);
        BOOLEAN(*decreaseKeyFibHeap)(struct _FIB_HEAP_CONTEXT *pFibHeapContext, UINT VertexIndex, INT NewKey);
        PFIB_HEAP_NODE(*deleteKeyFibHeap)(struct _FIB_HEAP_CONTEXT *pFibHeapContext, INT Key);
    }FibHeapFnTbl;
}FIB_HEAP_CONTEXT, *PFIB_HEAP_CONTEXT;

// Function Prototype
PFIB_HEAP_CONTEXT createFibHeapContext(UINT NumNodesSspGraph);
BOOLEAN           destroyFibHeapContext(PFIB_HEAP_CONTEXT *ppFibHeapContext);

#endif
