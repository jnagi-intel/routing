//
//  Main source file for SSP project
//

#include "Ssp.h"

// Local Function Decalaration
BOOLEAN __parseGraphInputFile(struct _SSP_CONTEXT* pSspContext);
BOOLEAN __getShortestPath(struct _SSP_CONTEXT* pSspContext);
UINT    __getNextHopInShortestPath(struct _SSP_CONTEXT* pSspContext);
BOOLEAN __printPath(struct _SSP_CONTEXT* pSspContext);
BOOLEAN __createGraph(PSSP_CONTEXT pSspContext);
PVERTEX_ADJ_LIST_NODE __createAdjListNode(UINT DestNode, INT EdgeWeight);
BOOLEAN __addEdgeToGraph(PSSP_CONTEXT pSspContext, UINT FirstNode, UINT SecondNode, INT EdgeWeight);


// __createGraph()
// This procedure initializes the graph structure from the number of vertices 
// read from the  file
BOOLEAN __createGraph(PSSP_CONTEXT pSspContext)
{
    UINT uiCount = 0;

    // Initialize the memory for the graph first
    pSspContext->pGraph = (PGRAPH)malloc(sizeof(GRAPH));
    pSspContext->pGraph->NumVertices = pSspContext->NumVertices;

    // Initialize the memory for vertex nodes of adjacency list
    pSspContext->pGraph->pVertexList = (PVERTEX_LIST_NODE)malloc(sizeof(VERTEX_LIST_NODE)*pSspContext->pGraph->NumVertices);

    // Make all the head pointers NULL 
    for (uiCount = 0; uiCount < pSspContext->pGraph->NumVertices; uiCount++)
    {
        pSspContext->pGraph->pVertexList[uiCount].pVertexAdjListHeadNode = NULL;
    }

    return true;
}

// __createAdjListNode()
// This procedure creates an Adjcacency List Node by initializing the memory for the node 
PVERTEX_ADJ_LIST_NODE __createAdjListNode(UINT DestNode, INT EdgeWeight)
{
    PVERTEX_ADJ_LIST_NODE  pAdjListNode = NULL;

    // Allocate memory for the node, iniitalize fields 
    pAdjListNode = (PVERTEX_ADJ_LIST_NODE)malloc(sizeof(VERTEX_ADJ_LIST_NODE));
    pAdjListNode->DestNode = DestNode;
    pAdjListNode->EdgeWeight = EdgeWeight;
    pAdjListNode->next = NULL;

    return pAdjListNode;
}

// __addEdgeToGraph()
// This procedure adds an undirected edge between the 2 node, adding at the beginning of 
// Vertex List
BOOLEAN __addEdgeToGraph(PSSP_CONTEXT pSspContext, UINT FirstNode, UINT SecondNode, INT EdgeWeight)
{
    PVERTEX_ADJ_LIST_NODE  pAdjListNode = NULL;
    
    // Add an edge from first node to second node
    pAdjListNode = __createAdjListNode(SecondNode, EdgeWeight);
    pAdjListNode->next = pSspContext->pGraph->pVertexList[FirstNode].pVertexAdjListHeadNode;
    pSspContext->pGraph->pVertexList[FirstNode].pVertexAdjListHeadNode = pAdjListNode;

    // Add an edge from second node to first node 
    pAdjListNode = __createAdjListNode(FirstNode, EdgeWeight);
    pAdjListNode->next = pSspContext->pGraph->pVertexList[SecondNode].pVertexAdjListHeadNode;
    pSspContext->pGraph->pVertexList[SecondNode].pVertexAdjListHeadNode = pAdjListNode;

    return true;
}

// __parseGraphInputFile()
// This procedure parses the input file and gets the vertices, edges and weights, 
// fills up the adjcacency list graph structure
BOOLEAN __parseGraphInputFile(struct _SSP_CONTEXT* pSspContext)
{
    UINT    FirstEndNode = 0, SecondEndNode = 0, EdgeWeight = 0;

    pSspContext->pInputGraphFileHandle = fopen(pSspContext->SspArgs.InputGraphFilename, "r");
    if (pSspContext->pInputGraphFileHandle == NULL)
    {
        printf("Unable to open Input file\n");
        return false;
    }
    
    // Read the first line and get number of vertices and edges 
    fscanf(pSspContext->pInputGraphFileHandle, "%u %u", &pSspContext->NumVertices, &pSspContext->NumEdges);

    // Validate the Src Node and Dest Node
    if (pSspContext->SspArgs.SrcNode > pSspContext->NumVertices - 1 ||
        pSspContext->SspArgs.DestNode > pSspContext->NumVertices - 1)
    {
        return false;
    }

    // Initialize the graph structure
    __createGraph(pSspContext);
    
    // Read all the edges and weight information 
    // Check for End of File
    while (!feof(pSspContext->pInputGraphFileHandle))
    {
        // Read the line and get node index and edge weight
        fscanf(pSspContext->pInputGraphFileHandle, "%u %u %u", &FirstEndNode, &SecondEndNode, &EdgeWeight);

        // Add and edge between these nodes
        __addEdgeToGraph(pSspContext, FirstEndNode, SecondEndNode, EdgeWeight);
    }

    return true;
}

// __dijkstraSspAlgorithm()
// This procedure finds the shortest path and total weight from given source 
// and destination nodes 
BOOLEAN __dijkstraSspAlgorithm(struct _SSP_CONTEXT* pSspContext)
{
    PFIB_HEAP_CONTEXT       pFibHeapContext = pSspContext->pFibHeapContext;
    PFIB_HEAP_NODE          pMinWeightFibHeapNode = NULL;
    PFIB_HEAP_NODE          pAdjFibHeapNode = NULL;
    PVERTEX_ADJ_LIST_NODE   pAdjListNodeItr = NULL;
    UINT                    Count = 0;

    // Initialize the Fibonacci Heap Context 
    pFibHeapContext = createFibHeapContext(pSspContext->NumVertices);

    // Initailize the Fibonacci Heap, weights to all the vertices start with INT_MAX
    pSspContext->pMinWeightArray = (UINT*)malloc(sizeof(UINT)*pSspContext->NumVertices);
    pSspContext->pPrevNodeShortestPathArray = (UINT*)malloc(sizeof(UINT)*pSspContext->NumVertices);

    for (Count = 0; Count < pSspContext->NumVertices; Count++)
    {
        pSspContext->pMinWeightArray[Count] = INT_MAX;
        pSspContext->pPrevNodeShortestPathArray[Count] = -1;
        pFibHeapContext->FibHeapFnTbl.insertFibHeapNode(pFibHeapContext, Count, INT_MAX);
    }

    // Make the weight of src node as 0 
    pSspContext->pMinWeightArray[pSspContext->SrcNode] = 0;
    pFibHeapContext->FibHeapFnTbl.decreaseKeyFibHeap(pFibHeapContext, pSspContext->SrcNode, 0);

    // dijkstra algorithm loop to find the minimum weight from srcNode to destNode
    while (true)
    {
        // Extract the vertex with the minimum weight
        pMinWeightFibHeapNode = pFibHeapContext->FibHeapFnTbl.extractMinFibHeapNode(pFibHeapContext);

        // Exit if the last node is also extracted or if the Dest Node is extracted
        if (pFibHeapContext->pMinFibHeapNode == NULL || pMinWeightFibHeapNode->VertexIndex == pSspContext->DestNode)
        {
            break;
        }

        // Iterate through all the adjacent vertices of the min weight node, and 
        // update the weights
        pAdjListNodeItr = pSspContext->pGraph->pVertexList[pMinWeightFibHeapNode->VertexIndex].pVertexAdjListHeadNode;
        while (pAdjListNodeItr != NULL)
        {
            // if the weight of the path to adjacent node is less than its previously calculated path, update the weight 
            pAdjFibHeapNode = &pFibHeapContext->pFibHeapNodeList[pAdjListNodeItr->DestNode];
            if (pAdjFibHeapNode &&
                pMinWeightFibHeapNode->Key != INT_MAX &&
                (pMinWeightFibHeapNode->Key + pAdjListNodeItr->EdgeWeight) < pAdjFibHeapNode->Key)
            {
                pSspContext->pMinWeightArray[pAdjFibHeapNode->VertexIndex] = pMinWeightFibHeapNode->Key + pAdjListNodeItr->EdgeWeight;
                pFibHeapContext->FibHeapFnTbl.decreaseKeyFibHeap(pFibHeapContext, pAdjFibHeapNode->VertexIndex, pSspContext->pMinWeightArray[pAdjFibHeapNode->VertexIndex]);
                pSspContext->pPrevNodeShortestPathArray[pAdjFibHeapNode->VertexIndex] = pMinWeightFibHeapNode->VertexIndex;
            }

            // Iterate to next 
            pAdjListNodeItr = pAdjListNodeItr->next;
        }
    }

    // Destroy the Fibonacci Heap Context
    destroyFibHeapContext(&pFibHeapContext);

    return true;
}

// __getShortestPath()
// This procedure gets the shortest path by parsing the array for prev node
// in the path created by djikstra algorithm
BOOLEAN __getShortestPath(struct _SSP_CONTEXT* pSspContext)
{
    UINT    CurrVertexIndex = 0;
    UINT    CurrIndexPath = 0;

    // Run the Djikstra ALgorithm using Fibonacci Heaps
    __dijkstraSspAlgorithm(pSspContext);
    
    // get the Number of nodes including Src and Destination
    CurrVertexIndex = pSspContext->SspArgs.DestNode;
    do
    {
        pSspContext->NumNodesInPath++;
        CurrVertexIndex = pSspContext->pPrevNodeShortestPathArray[CurrVertexIndex];
        if (CurrVertexIndex == UINT_MAX)
        {
            printf("No Path to Destination Node\n");
            return false;
        }

    } while (CurrVertexIndex != pSspContext->SspArgs.SrcNode);
    pSspContext->NumNodesInPath++;

    // Initialize the path array
    pSspContext->pPath = (UINT*)malloc(sizeof(UINT)*pSspContext->NumNodesInPath);

    // Copy the path indexes in the array
    CurrIndexPath = pSspContext->NumNodesInPath - 1;
    CurrVertexIndex = pSspContext->SspArgs.DestNode;
    pSspContext->pPath[CurrIndexPath--] = pSspContext->SspArgs.DestNode;
    do
    {
        pSspContext->pPath[CurrIndexPath--] = pSspContext->pPrevNodeShortestPathArray[CurrVertexIndex];
        CurrVertexIndex = pSspContext->pPrevNodeShortestPathArray[CurrVertexIndex];
    } while (CurrVertexIndex != pSspContext->SspArgs.SrcNode);

    return true;
}

// __getNextHopInShortestPath()
// This procedure gets the next hop in the shortest path by parsing the array for prev node
// in the path created by djikstra algorithm and returns the vertex index
UINT __getNextHopInShortestPath(struct _SSP_CONTEXT* pSspContext)
{
    UINT    CurrVertexIndex = 0;
    UINT    CurrIndexPath = 0;
    UINT    NumNodesInPath = 0;
    UINT    *pPath = NULL;

    if (pSspContext->SrcNode == pSspContext->DestNode)
    {
        return pSspContext->SrcNode;
    }

    // Run the Djikstra Algorithm using Fibonacci Heaps
    __dijkstraSspAlgorithm(pSspContext);

    // get the Number of nodes including Src and Destination
    CurrVertexIndex = pSspContext->DestNode;
    do
    {
        NumNodesInPath++;
        CurrVertexIndex = pSspContext->pPrevNodeShortestPathArray[CurrVertexIndex];
    } while (CurrVertexIndex != pSspContext->SrcNode);
    NumNodesInPath++;

    // Initialize the path array
    pPath = (UINT*)malloc(sizeof(UINT)*NumNodesInPath);

    // Copy the path indexes in the array
    CurrIndexPath = NumNodesInPath - 1;
    CurrVertexIndex = pSspContext->DestNode;
    pPath[CurrIndexPath--] = pSspContext->DestNode;
    do
    {
        pPath[CurrIndexPath--] = pSspContext->pPrevNodeShortestPathArray[CurrVertexIndex];
        CurrVertexIndex = pSspContext->pPrevNodeShortestPathArray[CurrVertexIndex];
    } while (CurrVertexIndex != pSspContext->SrcNode);

    return pPath[1];
}

// __printPath()
// This procedure prints the shortest path to stdout
BOOLEAN __printPath(struct _SSP_CONTEXT* pSspContext)
{
    UINT    count = 0;  
    
    printf("%d\n", pSspContext->pMinWeightArray[pSspContext->SspArgs.DestNode]);
    for (count = 0; count < pSspContext->NumNodesInPath; count++)
    {
        printf("%d ", pSspContext->pPath[count]);
    }
    printf("\n");

    return true;
}

// createSspContext()
// This procedure allocates and initializes the SSP Context
PSSP_CONTEXT createSspContext()
{
    PSSP_CONTEXT    pSspContext = NULL;

    // Allocate the context
    pSspContext = (PSSP_CONTEXT)malloc(sizeof(SSP_CONTEXT));
    pSspContext->NumNodesInPath = 0;

    // Intialize the function pointers
    pSspContext->SspFnTbl.parseGraphInputFile        = __parseGraphInputFile;
    pSspContext->SspFnTbl.getShortestPath            = __getShortestPath;
    pSspContext->SspFnTbl.getNextHopInShortestPath   = __getNextHopInShortestPath;
    pSspContext->SspFnTbl.printPath                  = __printPath;

    return pSspContext;
}

// destroySspContext()
// This procedure deallocates the memory reserved by SSP Context
BOOLEAN destroySspContext(PSSP_CONTEXT *ppSspContext)
{
    if (*ppSspContext)
    {
        free(*ppSspContext);
        *ppSspContext = NULL;
    }

    return true;
}

// parseArguements()
// This procedure parses the command line arguements and fills the ARGS context 
BOOLEAN parseArguements(PSSP_CONTEXT pSspContext, char *argv[])
{
    UINT        FilenameLength = 0;
    BOOLEAN     bRetStatus = true;

    do
    {
        // get the filename 
        FilenameLength = strlen(argv[1]);
        if (FilenameLength > 0)
        {
            pSspContext->SspArgs.InputGraphFilename = (char*)malloc(sizeof(char)*FilenameLength);
            strcpy(pSspContext->SspArgs.InputGraphFilename, argv[1]);
        }
        else
        {
            printf("Illegal Filename!!\n");
            bRetStatus = false;
            break;
        }

        // Get the source node
        if (argv[2] != NULL)
        {
            pSspContext->SspArgs.SrcNode = atoi(argv[2]);
        }

        // Get the destination node 
        if (argv[3] != NULL)
        {
            pSspContext->SspArgs.DestNode = atoi(argv[3]);
        }
    } while (false);

    return bRetStatus;
}


