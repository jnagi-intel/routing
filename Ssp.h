//
//  Header file for SSP Project
//

#ifndef _SSP_H_
#define _SSP_H_

#include "Types.h"
#include "FibHeap.h"

// Adjacency List Representation of the graph 
// Adjacency List Nodes of every vertex
typedef struct _VERTEX_ADJ_LIST_NODE
{
    UINT    DestNode;
    INT     EdgeWeight;
    struct _VERTEX_ADJ_LIST_NODE *next;
}VERTEX_ADJ_LIST_NODE, *PVERTEX_ADJ_LIST_NODE;

// Vertex Nodes
typedef struct _VERTEX_LIST_NODE
{
    PVERTEX_ADJ_LIST_NODE pVertexAdjListHeadNode;
}VERTEX_LIST_NODE, *PVERTEX_LIST_NODE;

// Graph will be array of Vertex Node pointers 
typedef struct _GRAPH
{
    UINT                NumVertices;
    PVERTEX_LIST_NODE   pVertexList;
}GRAPH, *PGRAPH;

// Args for Dijkstra's SSP Algorithm
typedef struct _SSP_ARGS
{
    char*   InputGraphFilename;
    UINT    SrcNode;
    UINT    DestNode;
}SSP_ARGS, *PSSP_ARGS;

// Context Definition for Dijkstra's SSP Algorithm 
typedef struct _SSP_CONTEXT
{
    SSP_ARGS        SspArgs;
    FILE            *pInputGraphFileHandle;
    UINT            NumVertices;
    UINT            NumEdges;
    PGRAPH          pGraph;
    UINT            *pMinWeightArray;
    UINT            *pPrevNodeShortestPathArray;
    UINT            *pPath;
    UINT            NumNodesInPath;
    UINT            SrcNode;
    UINT            DestNode;
    struct _SSP_FN_TBL
    {
        BOOLEAN(*parseGraphInputFile)(struct _SSP_CONTEXT* pSspContext);
        BOOLEAN(*getShortestPath)(struct _SSP_CONTEXT* pSspContext);
        UINT(*getNextHopInShortestPath)(struct _SSP_CONTEXT* pSspContext);
        BOOLEAN(*printPath)(struct _SSP_CONTEXT* pSspContext);
    }SspFnTbl;
    PFIB_HEAP_CONTEXT   pFibHeapContext;
}SSP_CONTEXT, *PSSP_CONTEXT;

// Function prototypes
PSSP_CONTEXT createSspContext();
BOOLEAN destroySspContext(PSSP_CONTEXT *ppSspContext);
BOOLEAN parseArguements(PSSP_CONTEXT pSspContext, char *argv[]);

#endif
