//
//  Source file for Fibonacci Heap Implementation
//

#include "FibHeap.h"

// Local Function Definitions
BOOLEAN         __insertFibHeapNode(struct _FIB_HEAP_CONTEXT *pFibHeapContext, UINT VertexIndex, INT Key);
PFIB_HEAP_NODE  __extractMinFibHeapNode(struct _FIB_HEAP_CONTEXT *pFibHeapContext);
BOOLEAN         __decreaseKeyFibHeap(struct _FIB_HEAP_CONTEXT *pFibHeapContext, UINT VertexIndex, INT NewKey);
PFIB_HEAP_NODE  __deleteKeyFibHeap(struct _FIB_HEAP_CONTEXT *pFibHeapContext, INT Key);
BOOLEAN         __linkFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext, PFIB_HEAP_NODE pLargerKeyFibHeapNode, PFIB_HEAP_NODE pSmallerKeyFibHeapNode);
BOOLEAN         __consolidateFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext);
BOOLEAN         __cutFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext, PFIB_HEAP_NODE pFibHeapNode, PFIB_HEAP_NODE pParentFibHeapNode);
BOOLEAN         __cascadingCutFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext, PFIB_HEAP_NODE pFibHeapNode);

// createFibHeapContext()
// This procedure allocates and returns the Fibonacci Heap Context
// Also initializes all the function pointers
PFIB_HEAP_CONTEXT createFibHeapContext(UINT NumNodesSspGraph)
{
    PFIB_HEAP_CONTEXT pFibHeapContext = NULL;

    // Create and Allocate Fibonacci Heap Context
    pFibHeapContext = (PFIB_HEAP_CONTEXT)malloc(sizeof(FIB_HEAP_CONTEXT));
    pFibHeapContext->NumNodesFibHeap = 0;
    pFibHeapContext->NumNodesRootList = 0;
    pFibHeapContext->pMinFibHeapNode = NULL;
    pFibHeapContext->NumNodesSspGraph = NumNodesSspGraph;

    // Allocate memory for Nodes List 
    pFibHeapContext->pFibHeapNodeList = (PFIB_HEAP_NODE)malloc(sizeof(FIB_HEAP_NODE)* pFibHeapContext->NumNodesSspGraph);

    // Initialize the fucntion table 
    pFibHeapContext->FibHeapFnTbl.insertFibHeapNode      = __insertFibHeapNode;
    pFibHeapContext->FibHeapFnTbl.extractMinFibHeapNode  = __extractMinFibHeapNode;
    pFibHeapContext->FibHeapFnTbl.decreaseKeyFibHeap     = __decreaseKeyFibHeap;
    pFibHeapContext->FibHeapFnTbl.deleteKeyFibHeap       = __deleteKeyFibHeap;

    return pFibHeapContext;
}

// destroyFibHeapContext()
// This procedure destroys the Context, freeing up all the allocated memory 
BOOLEAN destroyFibHeapContext(PFIB_HEAP_CONTEXT *ppFibHeapContext)
{
    if ((*ppFibHeapContext)->pFibHeapNodeList)
    {
        free((*ppFibHeapContext)->pFibHeapNodeList);
        (*ppFibHeapContext)->pFibHeapNodeList = NULL;
    }

    if (*ppFibHeapContext)
    {
        free(*ppFibHeapContext);
        *ppFibHeapContext = NULL;
    }

    return true;
}

// __insertFibHeap()
// This procedure inserts a node into a Fibonacci Heap
BOOLEAN __insertFibHeapNode(struct _FIB_HEAP_CONTEXT *pFibHeapContext, UINT VertexIndex, INT Key)
{
    PFIB_HEAP_NODE  pTempFibHeapNode = NULL;

    // Use the Vertex Index as Index into Fib Heap List and Add key 
    pFibHeapContext->pFibHeapNodeList[VertexIndex].Key          = Key;
    pFibHeapContext->pFibHeapNodeList[VertexIndex].Degree       = 0;
    pFibHeapContext->pFibHeapNodeList[VertexIndex].pParent      = NULL;
    pFibHeapContext->pFibHeapNodeList[VertexIndex].pChild       = NULL;
    pFibHeapContext->pFibHeapNodeList[VertexIndex].pLeftSib     = &pFibHeapContext->pFibHeapNodeList[VertexIndex];
    pFibHeapContext->pFibHeapNodeList[VertexIndex].pRightSib    = &pFibHeapContext->pFibHeapNodeList[VertexIndex];;
    pFibHeapContext->pFibHeapNodeList[VertexIndex].bChildCut    = false;
    pFibHeapContext->pFibHeapNodeList[VertexIndex].VertexIndex  = VertexIndex;

    // concatenate the root list containing the node with root list of the Heap
    if (pFibHeapContext->pMinFibHeapNode != NULL)
    {
        // Make the new node the left sibling of the min heap pointer node
        pTempFibHeapNode = pFibHeapContext->pMinFibHeapNode->pLeftSib;
        pFibHeapContext->pMinFibHeapNode->pLeftSib = &pFibHeapContext->pFibHeapNodeList[VertexIndex];
        pFibHeapContext->pFibHeapNodeList[VertexIndex].pRightSib = pFibHeapContext->pMinFibHeapNode;
        pFibHeapContext->pFibHeapNodeList[VertexIndex].pLeftSib = pTempFibHeapNode;
        pTempFibHeapNode->pRightSib = &pFibHeapContext->pFibHeapNodeList[VertexIndex];

        // update the min pointer if required
        if (pFibHeapContext->pFibHeapNodeList[VertexIndex].Key < pFibHeapContext->pMinFibHeapNode->Key)
        {
            pFibHeapContext->pMinFibHeapNode = &pFibHeapContext->pFibHeapNodeList[VertexIndex];
        }
    }
    else
    {
        // first tree/node in the root list
        pFibHeapContext->pMinFibHeapNode = &pFibHeapContext->pFibHeapNodeList[VertexIndex];
    }

    pFibHeapContext->NumNodesFibHeap++;
    pFibHeapContext->NumNodesRootList++;

    return true;
}

// __linkFibHeap()
// This procedure links the trees with the same degree making the larger key node child of the smaller key node
BOOLEAN __linkFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext, PFIB_HEAP_NODE pLargerKeyFibHeapNode, PFIB_HEAP_NODE pSmallerKeyFibHeapNode)
{
    // remove larger key node from the root list 
    pLargerKeyFibHeapNode->pLeftSib->pRightSib = pLargerKeyFibHeapNode->pRightSib;
    pLargerKeyFibHeapNode->pRightSib->pLeftSib = pLargerKeyFibHeapNode->pLeftSib;

    // Make larger key node child of smaller key node 
    // Check if there is another child of smaller node 
    if (pSmallerKeyFibHeapNode->pChild != NULL)
    {
        pLargerKeyFibHeapNode->pRightSib = pSmallerKeyFibHeapNode->pChild;
        pLargerKeyFibHeapNode->pLeftSib = pSmallerKeyFibHeapNode->pChild->pLeftSib;
        pSmallerKeyFibHeapNode->pChild->pLeftSib->pRightSib = pLargerKeyFibHeapNode;
        pSmallerKeyFibHeapNode->pChild->pLeftSib = pLargerKeyFibHeapNode;

        // Update the child pointer according to the Keys
        if (pLargerKeyFibHeapNode->Key < pSmallerKeyFibHeapNode->pChild->Key)
        {
            pSmallerKeyFibHeapNode->pChild = pLargerKeyFibHeapNode;
        }
    }
    else
    {
        // Only node
        pSmallerKeyFibHeapNode->pChild = pLargerKeyFibHeapNode;
        pLargerKeyFibHeapNode->pRightSib = pLargerKeyFibHeapNode;
        pLargerKeyFibHeapNode->pLeftSib = pLargerKeyFibHeapNode;
    }

    pLargerKeyFibHeapNode->pParent = pSmallerKeyFibHeapNode;
    pSmallerKeyFibHeapNode->Degree++;

    return true;
}

// __consolidateFibHeap()
// This procedure conslidates the root list so that every root in the 
// root list has a distinct degree value
BOOLEAN __consolidateFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext)
{
    PFIB_HEAP_NODE  *DegreeArray = NULL;
    PFIB_HEAP_NODE  pRootFibHeapNodeItr = NULL;
    PFIB_HEAP_NODE  pTempFibHeapNodeItr = NULL;
    PFIB_HEAP_NODE  pDegreeArrayFibHeapNode = NULL;
    PFIB_HEAP_NODE  pNextFibHeapNode = NULL;
    UINT            uiMaxDegree = 0;
    UINT            uiCount = 0;
    UINT            uiDegree = 0;
    UINT            RootListCount = 0;

    // Initialize the Degree Array by making all the pointers NULL
    uiMaxDegree = (UINT)(log(pFibHeapContext->NumNodesFibHeap) / log(2));
    DegreeArray = (PFIB_HEAP_NODE*)malloc((uiMaxDegree+2)*sizeof(FIB_HEAP_NODE));
    for (uiCount = 0; uiCount <= uiMaxDegree+1; uiCount++)
    {
        DegreeArray[uiCount] = NULL;
    }

    // Process each root w in the root list. 
    // After processing each root w, it ends up in a tree rooted at some node x, which may or may not be identical to w
    // Of the processed roots, no other will have the same degree as x, so we will set array entry DegreeArray[Degree(x)] to point to x 
    // When this loop terminates, at most one root of each degree will remain, and degreeArray will point to each remaining root
    pNextFibHeapNode = pRootFibHeapNodeItr = pFibHeapContext->pMinFibHeapNode;
    
    for (RootListCount = 0; RootListCount < pFibHeapContext->NumNodesRootList; RootListCount++)
    {
        pNextFibHeapNode = pRootFibHeapNodeItr->pRightSib;
        uiDegree = pRootFibHeapNodeItr->Degree;

        while (DegreeArray[uiDegree] != NULL)
        {
            // DegreeArray not NULL, another root exists with the same degree, link! 
            pDegreeArrayFibHeapNode = DegreeArray[uiDegree];

            // exchange if key is < current root
            if (pRootFibHeapNodeItr->Key > pDegreeArrayFibHeapNode->Key)
            {
                pTempFibHeapNodeItr = pRootFibHeapNodeItr;
                pRootFibHeapNodeItr = pDegreeArrayFibHeapNode;
                pDegreeArrayFibHeapNode = pTempFibHeapNodeItr;
            }

            if (pDegreeArrayFibHeapNode == pFibHeapContext->pMinFibHeapNode)
            {
                pFibHeapContext->pMinFibHeapNode = pRootFibHeapNodeItr;
            }

            // Link the 2 root trees
            __linkFibHeap(pFibHeapContext, pDegreeArrayFibHeapNode, pRootFibHeapNodeItr);

            if (pRootFibHeapNodeItr->pRightSib == pRootFibHeapNodeItr)
            {
                pFibHeapContext->pMinFibHeapNode = pRootFibHeapNodeItr;
            }

            DegreeArray[uiDegree] = NULL;
            uiDegree++;
        }

        // Update the DegreeArray and move to next root
        DegreeArray[uiDegree] = pRootFibHeapNodeItr;
        pRootFibHeapNodeItr = pNextFibHeapNode;

    }
    
    pFibHeapContext->pMinFibHeapNode = NULL;
    pFibHeapContext->NumNodesRootList = 0;

    // Reconstruct the Heap from Degree Array
    for (uiCount = 0; uiCount <= uiMaxDegree+1; uiCount++)
    {
        if (DegreeArray[uiCount] != NULL)
        {
            DegreeArray[uiCount]->pParent = NULL;

            // Add this to the root list of Fib Heap 
            if (pFibHeapContext->pMinFibHeapNode != NULL)
            {
                // Add the node to the root list
                pFibHeapContext->pMinFibHeapNode->pLeftSib->pRightSib = DegreeArray[uiCount];
                DegreeArray[uiCount]->pLeftSib = pFibHeapContext->pMinFibHeapNode->pLeftSib;
                DegreeArray[uiCount]->pRightSib = pFibHeapContext->pMinFibHeapNode;
                pFibHeapContext->pMinFibHeapNode->pLeftSib = DegreeArray[uiCount];
                
                // Update key if required 
                if (DegreeArray[uiCount]->Key < pFibHeapContext->pMinFibHeapNode->Key)
                {
                    pFibHeapContext->pMinFibHeapNode = DegreeArray[uiCount];
                }
            }
            else
            {
                // Empty root list
                DegreeArray[uiCount]->pLeftSib = DegreeArray[uiCount];
                DegreeArray[uiCount]->pRightSib = DegreeArray[uiCount];
                pFibHeapContext->pMinFibHeapNode = DegreeArray[uiCount];
            }

            pFibHeapContext->NumNodesRootList++;
        }
    }

    return true;
}

// __extractMinFibHeapNode()
// This procedure extracts the minimum node from the Fib Heap 
PFIB_HEAP_NODE __extractMinFibHeapNode(PFIB_HEAP_CONTEXT pFibHeapContext)
{
    PFIB_HEAP_NODE  pMinFibHeapNode         = NULL;     // Node to be removed! 
    PFIB_HEAP_NODE  pChildFibHeapNode       = NULL;
    PFIB_HEAP_NODE  pChildFibHeapNodeItr    = NULL;
    PFIB_HEAP_NODE  pTempFibHeapNode        = NULL;

    // save the pointer to the minimum node 
    pMinFibHeapNode = pFibHeapContext->pMinFibHeapNode;

    if (pMinFibHeapNode != NULL)
    {
        // Remove this node and make all its children roots of Fib Heap
        // Get the child node
        pChildFibHeapNode = pMinFibHeapNode->pChild;

        if (pChildFibHeapNode != NULL)
        {
            pChildFibHeapNodeItr = pChildFibHeapNode;

            // Iterate through all the children of the Min Node
            do
            {
                // add this node to the root list
                pTempFibHeapNode = pChildFibHeapNodeItr->pRightSib;
                (pMinFibHeapNode->pLeftSib)->pRightSib = pChildFibHeapNodeItr;
                pChildFibHeapNodeItr->pRightSib = pMinFibHeapNode;
                pChildFibHeapNodeItr->pLeftSib = pMinFibHeapNode->pLeftSib;
                pMinFibHeapNode->pLeftSib = pChildFibHeapNodeItr;
                pChildFibHeapNodeItr->pParent = NULL;

                // update the min pointer if required
                if (pChildFibHeapNodeItr->Key < pFibHeapContext->pMinFibHeapNode->Key)
                {
                    pFibHeapContext->pMinFibHeapNode = pChildFibHeapNodeItr;
                }

                // Move to the next right sibling
                pChildFibHeapNodeItr = pTempFibHeapNode;

                pFibHeapContext->NumNodesRootList++;

            } while (pChildFibHeapNodeItr != pChildFibHeapNode);
        }

        // Remove Min node from the root list of Fib Heap
        (pMinFibHeapNode->pLeftSib)->pRightSib = pMinFibHeapNode->pRightSib;
        (pMinFibHeapNode->pRightSib)->pLeftSib = pMinFibHeapNode->pLeftSib;
        pFibHeapContext->NumNodesRootList--;

        // if Min Node was the only node in the Fib Heap without any children, then point the new Min Pointer to NULL 
        if (pMinFibHeapNode == pMinFibHeapNode->pRightSib)
        {
            pFibHeapContext->pMinFibHeapNode = NULL;
        }
        else
        {
            // Point it to any other node to the right and consolidate the root list of Fib Heap
            pFibHeapContext->pMinFibHeapNode = pMinFibHeapNode->pRightSib;
            __consolidateFibHeap(pFibHeapContext);
        }

        // Reduce the count of nodes in Fib Heap 
        pFibHeapContext->NumNodesFibHeap--;
    }

    return pMinFibHeapNode;
}

// __cutFibHeap()
// This procedure removes the node and adds it to the root list fo the parent node
BOOLEAN __cutFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext, PFIB_HEAP_NODE pFibHeapNode, PFIB_HEAP_NODE pParentFibHeapNode)
{
    // Remove Fib Heap Node frm the child list of parent, decrementing the degree of parent
    if (pParentFibHeapNode->pChild == pFibHeapNode)
    {
        // Check if there are any more children of parent
        if (pFibHeapNode->pRightSib == pFibHeapNode)
        {
            pParentFibHeapNode->pChild = NULL;
        }
        else
        {
            // if Fib Heap Node is the child, then point it to some else node 
            pParentFibHeapNode->pChild = pFibHeapNode->pRightSib;
        }
    }
    pParentFibHeapNode->Degree--;

    // Add Fib Heap Node to the root list of Heap
    pFibHeapNode->pLeftSib->pRightSib = pFibHeapNode->pRightSib;
    pFibHeapNode->pRightSib->pLeftSib = pFibHeapNode->pLeftSib;
    pFibHeapContext->pMinFibHeapNode->pLeftSib->pRightSib = pFibHeapNode;
    pFibHeapNode->pRightSib = pFibHeapContext->pMinFibHeapNode;
    pFibHeapNode->pLeftSib = pFibHeapContext->pMinFibHeapNode->pLeftSib;
    pFibHeapContext->pMinFibHeapNode->pLeftSib = pFibHeapNode;
    pFibHeapNode->pParent = NULL;
    pFibHeapNode->bChildCut = false;
    pFibHeapContext->NumNodesRootList++;

    // Check if the min pointer has to be changed
    if (pFibHeapNode->Key < pFibHeapContext->pMinFibHeapNode->Key)
    {
        pFibHeapContext->pMinFibHeapNode = pFibHeapNode;
    }

    return true;
}

// __cascadingCutFibHeap()
// This procedure recurses its way up the tree untill either a root or an unmarked node is found
BOOLEAN __cascadingCutFibHeap(PFIB_HEAP_CONTEXT pFibHeapContext, PFIB_HEAP_NODE pFibHeapNode)
{
    PFIB_HEAP_NODE pParentFibHeapNode = pFibHeapNode->pParent;

    // Check if the node is the root
    if (pParentFibHeapNode != NULL)
    {
        // if its unmarked, mark it and return
        if (!pFibHeapNode->bChildCut)
        {
            pFibHeapNode->bChildCut = true;
        }
        else
        {
            // it has just lost its second child, cut it and recursively call cascadingCut
            __cutFibHeap(pFibHeapContext, pFibHeapNode, pParentFibHeapNode);
            __cascadingCutFibHeap(pFibHeapContext, pParentFibHeapNode);
        }
    }

    return true;
}

// __decreaseKeyFibHeap()
// This procedure decreases the value of the Key in the particular node in the Fibonacci Heap
BOOLEAN __decreaseKeyFibHeap(struct _FIB_HEAP_CONTEXT *pFibHeapContext, UINT VertexIndex, INT NewKey)
{
    PFIB_HEAP_NODE  pFibHeapNode = NULL;
    PFIB_HEAP_NODE  pParentFibHeapNode = NULL;

    if (pFibHeapContext->pMinFibHeapNode != NULL)
    {
        // Make sure Fibonacci Heap is not empty
        // Search for the node with the given VertexIndex
        //pFibHeapNode = __findNodeFibHeap(pFibHeapContext->pMinFibHeapNode, VertexIndex);
        pFibHeapNode = &pFibHeapContext->pFibHeapNodeList[VertexIndex];

        if (pFibHeapNode != NULL)
        {
            // There exists a node with the original Key! 
            if (pFibHeapNode->Key > NewKey)
            {
                // New key is smaller than the Orig Key 
                pFibHeapNode->Key = NewKey;
                pParentFibHeapNode = pFibHeapNode->pParent;

                if (pParentFibHeapNode != NULL && pParentFibHeapNode->Key > pFibHeapNode->Key)
                {
                    __cutFibHeap(pFibHeapContext, pFibHeapNode, pParentFibHeapNode);
                    __cascadingCutFibHeap(pFibHeapContext, pParentFibHeapNode);
                }

                // Check if the min pointer has to be changed
                if (pFibHeapNode->Key < pFibHeapContext->pMinFibHeapNode->Key)
                {
                    pFibHeapContext->pMinFibHeapNode = pFibHeapNode;
                }
            }
        }
        else
        {
            // node not found! 
            return false;
        }
    }
    else
    {
        // Heap is empty 
        return false;
    }

    return true;
}

// __deleteKeyFibHeap()
// This procedure deletes the node with the VertexIndex mentioned
PFIB_HEAP_NODE __deleteKeyFibHeap(struct _FIB_HEAP_CONTEXT *pFibHeapContext, INT VertexIndex)
{
    // Make the node become the minimum key node in the heap
    // As the key is unsigned int, smallest would be -1
    if (__decreaseKeyFibHeap(pFibHeapContext, VertexIndex, -1))
    {
        // Remove from the heap using extract min 
        return __extractMinFibHeapNode(pFibHeapContext);
    }

    return NULL;
}

