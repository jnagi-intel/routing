//
//  Source File for Trie Implementation
//

#include "Trie.h"

// Local Function Definition
PTRIE_NODE  __createTrieNode(IP_ADDR IpAddr, UINT NextHop);
BOOLEAN     __insertTrieNode(struct _TRIE_CONTEXT *pTrieContext, PTRIE_NODE pTrieNode);
BOOLEAN     __mergeSubTries(PTRIE_NODE pTrieNode);
BOOLEAN     __getBitValueAtBitIndex(UINT Number, UINT BitIndex);
IP_ADDR     __createIpAddrFromUint(UINT Value);
PTRIE_NODE  __findTrieNode(struct _TRIE_CONTEXT* pTrieContext, IP_ADDR IpAddr);
BOOLEAN     __createNewTrieHeadNode(struct _TRIE_CONTEXT* pTrieContext);
BOOLEAN     __deleteTrieNodes(PTRIE_NODE pTrieNode);
BOOLEAN     __convertDecimalToBinaryString(UINT DecimalValue, CHAR* BinaryString);


// createTrieContext()
// This procedure allocates and returns the Trie Context
// Also initializes all the function pointers
PTRIE_CONTEXT createTrieContext()
{
    PTRIE_CONTEXT   pTrieContext = NULL;

    // Create and Allocate Trie Context 
    pTrieContext = (PTRIE_CONTEXT)malloc(sizeof(TRIE_CONTEXT));
    pTrieContext->pTrieHeadNode = __createTrieNode(__createIpAddrFromUint(0), INT_MAX);

    // Intialize the function pointers
    pTrieContext->TrieFnTbl.insertTrieNode        = __insertTrieNode;
    pTrieContext->TrieFnTbl.createTrieNode        = __createTrieNode;
    pTrieContext->TrieFnTbl.mergeSubTries         = __mergeSubTries;
    pTrieContext->TrieFnTbl.findTrieNode          = __findTrieNode;
    pTrieContext->TrieFnTbl.createNewTrieHeadNode = __createNewTrieHeadNode;

    return pTrieContext;
}

// destroyTrieContext()
// This procedure deallocates the memory reserved by Trie Context
BOOLEAN destroyTrieContext(PTRIE_CONTEXT *ppTrieContext)
{
    if (*ppTrieContext)
    {
        free(*ppTrieContext);
        *ppTrieContext = NULL;
    }

    return true;
}

// ___createTrieNode()
// This procedure allocates the Trie Node and fills the Vertex Index 
PTRIE_NODE __createTrieNode(IP_ADDR IpAddr, UINT NextHop)
{
    PTRIE_NODE  pTrieNode = NULL;
    UINT        Count = 0;

    // Allocate and intialize the fields
    pTrieNode = (PTRIE_NODE)malloc(sizeof(TRIE_NODE));
    pTrieNode->IpAddr.IpAddr32Bit = IpAddr.IpAddr32Bit;
    pTrieNode->NextHop = NextHop;
    pTrieNode->pLeftChild = NULL;
    pTrieNode->pRightChild = NULL;

    for (Count = 0; Count < 33; Count++)
    {
        pTrieNode->LongestPrefix[Count] = '\0';
    }

    return pTrieNode;
}

// __insertTrieNode()
// This procedure inserts a Trie Node in the binary Trie, creating all the nodes
// on the way,it will also re allocate the memory for the node node
BOOLEAN __insertTrieNode(struct _TRIE_CONTEXT *pTrieContext, PTRIE_NODE pTrieNode)
{
    PTRIE_NODE  pCurrTrieNode = pTrieContext->pTrieHeadNode;
    INT         BitIndex = 0;
    UINT        CurrentPrefix = 0;

    // For this project all the inserts will be 32 bit IP addresses
    // Start parsing the Prefix from left most bit and add the Next Hop node in the 32nd node
    // Post Order Traversal merge will take care of the sub tries 
    for (BitIndex = NUM_BITS_IP_ADDR-1; BitIndex >= 0; BitIndex--)
    {
        if (__getBitValueAtBitIndex(pTrieNode->IpAddr.IpAddr32Bit, BitIndex))
        {
            CurrentPrefix = (CurrentPrefix << 1) | 0x1;

            // create a node if this is NULL
            if (pCurrTrieNode->pRightChild == NULL)
            {
                pCurrTrieNode->pRightChild = __createTrieNode(__createIpAddrFromUint(0), INT_MAX);
                __convertDecimalToBinaryString(CurrentPrefix, &pCurrTrieNode->pRightChild->LongestPrefix[0]);
            }

            pCurrTrieNode = pCurrTrieNode->pRightChild;
        }
        else
        {
            CurrentPrefix <<= 1;

            // create a node if this is NULL
            if (pCurrTrieNode->pLeftChild == NULL)
            {
                pCurrTrieNode->pLeftChild = __createTrieNode(__createIpAddrFromUint(0), INT_MAX);
                __convertDecimalToBinaryString(CurrentPrefix, &pCurrTrieNode->pLeftChild->LongestPrefix[0]);
            }

            pCurrTrieNode = pCurrTrieNode->pLeftChild;
        }
    }

    // Store the Next Hop in the last node 
    pCurrTrieNode->IpAddr.IpAddr32Bit = pTrieNode->IpAddr.IpAddr32Bit;
    pCurrTrieNode->NextHop = pTrieNode->NextHop;

    return true;
}

// __getBitValueAtBitIndex()
// This procedure returns true if the bit at the bit index in the number is a 1, else false
BOOLEAN __getBitValueAtBitIndex(UINT Number, UINT BitIndex)
{
    UINT BitMask = 1 << BitIndex;
    return ((Number & BitMask) == 0x0) ? false : true;
}

// __createIpAddFromUint()
// This procedure creates an IpAddr variable from Uint
IP_ADDR __createIpAddrFromUint(UINT Value)
{
    IP_ADDR IpAddr;
    IpAddr.IpAddr32Bit = Value;
    return IpAddr;
}

// __mergeSubTries()
// This procedure merges the sub tries with the same Next Hop
// by doing a post order traversal
BOOLEAN __mergeSubTries(PTRIE_NODE pTrieNode)
{
    PTRIE_NODE  pCurrTrieNode = pTrieNode;

    // Do a post order traversal
    if (pCurrTrieNode != NULL)
    {
        __mergeSubTries(pCurrTrieNode->pLeftChild);
        __mergeSubTries(pCurrTrieNode->pRightChild);
        if (pCurrTrieNode->pLeftChild != NULL && pCurrTrieNode->pRightChild != NULL)
        {
            // Check if both the left and right children have the same Next Hop
            if (pCurrTrieNode->pLeftChild->NextHop == pCurrTrieNode->pRightChild->NextHop)
            {
                if (pCurrTrieNode->pLeftChild->NextHop != INT_MAX)
                {
                    // Merge both the sub tries and delete the children from the trie
                    pCurrTrieNode->NextHop = pCurrTrieNode->pLeftChild->NextHop;

                    free(pCurrTrieNode->pLeftChild);
                    free(pCurrTrieNode->pRightChild);
                    pCurrTrieNode->pLeftChild = NULL;
                    pCurrTrieNode->pRightChild = NULL;
                }
            }
        }

        // Check if only left Child has the Next Hop filled and right child is NULL
        if (pCurrTrieNode->pLeftChild != NULL && pCurrTrieNode->pRightChild == NULL)
        {
            if (pCurrTrieNode->pLeftChild->NextHop != INT_MAX)
            {
                // Merge and delete the left child
                pCurrTrieNode->NextHop = pCurrTrieNode->pLeftChild->NextHop;

                free(pCurrTrieNode->pLeftChild);
                pCurrTrieNode->pLeftChild = NULL;
            }
        }

        // Check if only right Child has the Next Hop filled and left child is NULL
        if (pCurrTrieNode->pLeftChild == NULL && pCurrTrieNode->pRightChild != NULL)
        {
            if (pCurrTrieNode->pRightChild->NextHop != INT_MAX)
            {
                // Merge and delete the left child
                pCurrTrieNode->NextHop = pCurrTrieNode->pRightChild->NextHop;

                free(pCurrTrieNode->pRightChild);
                pCurrTrieNode->pRightChild = NULL;
            }
        }
    }

    return true;
}

// __findTrieNode()
// This procedure returns the trie node with the given Ip Address
PTRIE_NODE __findTrieNode(struct _TRIE_CONTEXT* pTrieContext, IP_ADDR IpAddr)
{
    PTRIE_NODE  pCurrTrieNode = pTrieContext->pTrieHeadNode;
    INT         BitIndex = 0;

    // For this project all the inserts will be 32 bit IP addresses
    // Start parsing the Prefix from left most bit and go searching 
    // downn the trie untill you fall off the trie!
    for (BitIndex = NUM_BITS_IP_ADDR - 1; BitIndex >= 0; BitIndex--)
    {
        if (__getBitValueAtBitIndex(IpAddr.IpAddr32Bit, BitIndex))
        {
            if (pCurrTrieNode->pRightChild != NULL)
            {
                pCurrTrieNode = pCurrTrieNode->pRightChild;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (pCurrTrieNode->pLeftChild != NULL)
            {
                pCurrTrieNode = pCurrTrieNode->pLeftChild;
            }
            else
            {
                break;
            }
        }
    }

    return pCurrTrieNode;
}

// __createnewTrieHeader()
// This procedure deallocates all the trie nodes and creates a new trie head node
BOOLEAN __createNewTrieHeadNode(struct _TRIE_CONTEXT* pTrieContext)
{
    // Do a post order traversal and delete all the nodes! 
    __deleteTrieNodes(pTrieContext->pTrieHeadNode);

    // Initialize head node again
    pTrieContext->pTrieHeadNode = __createTrieNode(__createIpAddrFromUint(0), INT_MAX);

    return true;
}


// __deleteTrieNodes()
// This procedure deletes the trie nodes by doing a post order traversal
BOOLEAN __deleteTrieNodes(PTRIE_NODE pTrieNode)
{
    PTRIE_NODE pCurrTrieNode = pTrieNode;

    if (pCurrTrieNode != NULL)
    {
        __deleteTrieNodes(pTrieNode->pLeftChild);
        __deleteTrieNodes(pTrieNode->pRightChild);

        free(pTrieNode);
        pTrieNode = NULL;
    }

    return true;
}

// __convertDecimalToBinaryString()
// This procedure takes in UINT as an input and outputs a binary string equivalent 
// of the number
BOOLEAN __convertDecimalToBinaryString(UINT DecimalValue, CHAR* BinaryString)
{
    UINT    Index = 0;
    UINT    Remainder = 0;
    UINT    Start = 0, End = 0;
    CHAR    Temp;

    // handle the case for 0 separately
    if (DecimalValue == 0)
    {
        BinaryString[Index++] = '0';
        BinaryString[Index] = '\0';
    }

    // Covert to Binary String
    while (DecimalValue != 0)
    {
        Remainder = DecimalValue % 2;
        BinaryString[Index++] = (Remainder == 0) ? '0' : '1';
        DecimalValue = DecimalValue / 2;
    }

    // Append /0 in the end
    BinaryString[Index] = '\0';

    // Reverse the String
    Start = 0;
    End = Index - 1;
    while (Start < End)
    {
        Temp = BinaryString[Start];
        BinaryString[Start] = BinaryString[End];
        BinaryString[End] = Temp;
        Start++;
        End--;
    }

    return true;
}
