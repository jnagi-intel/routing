//
// Main source file for the Routing project
//

#include "Routing.h"

// Local Function Declarations
BOOLEAN __parseInputFiles(struct _ROUTING_CONTEXT* pRoutingContext);
BOOLEAN __getPrefixesOnShortestPath(struct _ROUTING_CONTEXT* pRoutingContext);
BOOLEAN __printPrefixes(struct _ROUTING_CONTEXT* pRoutingContext);

// createRoutingContext()
// This procedure allocates and initializes the Routing Context
PROUTING_CONTEXT createRoutingContext()
{
    PROUTING_CONTEXT    pRoutingContext = NULL;

    // Allocate the context
    pRoutingContext = (PROUTING_CONTEXT)malloc(sizeof(ROUTING_CONTEXT));
    pRoutingContext->PrefixArrayIndex = 0;
    pRoutingContext->pSspContext = createSspContext();
    pRoutingContext->pTrieContext = createTrieContext();

    // Intialize the function pointers
    pRoutingContext->RoutingFnTbl.parseInputFiles            = __parseInputFiles;
    pRoutingContext->RoutingFnTbl.getPrefixesOnShortestPath  = __getPrefixesOnShortestPath;
    pRoutingContext->RoutingFnTbl.printPrefixes              = __printPrefixes;

    return pRoutingContext;
}

// destroyRoutingContext()
// This procedure deallocates the memory for Routing Context
BOOLEAN destroyRoutingContext(PROUTING_CONTEXT *ppRoutingContext)
{
    // Destroy SSP and Trie Context too first
    destroySspContext(&(*ppRoutingContext)->pSspContext);
    destroyTrieContext(&(*ppRoutingContext)->pTrieContext);

    // Now free Routing Context
    if (*ppRoutingContext)
    {
        free(*ppRoutingContext);
        *ppRoutingContext = NULL;
    }

    return true;
}

// parseArguementsRouting()
// This procedure parses the command line arguements and fills the ARGS context 
BOOLEAN parseArguementsRouting(PROUTING_CONTEXT pRoutingContext, char *argv[])
{
    UINT        FilenameLength = 0;
    BOOLEAN     bRetStatus = true;

    do
    {
        // get the filename 
        FilenameLength = strlen(argv[1]);
        if (FilenameLength > 0)
        {
            pRoutingContext->pSspContext->SspArgs.InputGraphFilename = (char*)malloc(sizeof(char)*FilenameLength);
            strcpy(pRoutingContext->pSspContext->SspArgs.InputGraphFilename, argv[1]);
        }
        else
        {
            printf("Illegal Filename!!\n");
            bRetStatus = false;
            break;
        }

        // Get the second filename

        FilenameLength = strlen(argv[2]);
        if (FilenameLength > 0)
        {
            pRoutingContext->RoutingArgs.InputIpFileName = (char*)malloc(sizeof(char)*FilenameLength);
            strcpy(pRoutingContext->RoutingArgs.InputIpFileName, argv[2]);
        }
        else
        {
            printf("Illegal Filename!!\n");
            bRetStatus = false;
            break;
        }

        // Get the source node
        if (argv[3] != NULL)
        {
            pRoutingContext->pSspContext->SspArgs.SrcNode = atoi(argv[3]);
        }

        // Get the destination node 
        if (argv[4] != NULL)
        {
            pRoutingContext->pSspContext->SspArgs.DestNode = atoi(argv[4]);
        }
    } while (false);

    return bRetStatus;
}

// __parseInputFiles()
// This procedure parses both IP addresses file and the graph file for 
// routing
BOOLEAN __parseInputFiles(struct _ROUTING_CONTEXT* pRoutingContext)
{
    PSSP_CONTEXT pSspContext = pRoutingContext->pSspContext;
    char         IpAddrString[20], IpAddrSubString[5];
    UINT         StringCount = 0, SubStringCount = 0, IpAddrByte = 3, IpAddrListIndex = 0;
    
    // Parse the graph input file
    if (!pSspContext->SspFnTbl.parseGraphInputFile(pSspContext))
    {
        printf("Error in parsing Graph input!\n");
        return false;
    }

    // Parse the IP file 
    pRoutingContext->pInputIpFileHandle = fopen(pRoutingContext->RoutingArgs.InputIpFileName, "r");
    if (pRoutingContext->pInputIpFileHandle == NULL)
    {
        printf("Unable to open Input file\n");
        return false;
    }

    // Allocate space for the IP Array
    pRoutingContext->pIpAddrList = (IP_ADDR*)malloc(sizeof(IP_ADDR)*pSspContext->NumVertices);

    // Read all the IP Addresses
    // Check for End of File
    while (!feof(pRoutingContext->pInputIpFileHandle))
    {
        // Read the line and get node index and edge weight
        fscanf(pRoutingContext->pInputIpFileHandle, "%s", &IpAddrString[0]);

        // Parse the string and fill up the IP_ADDR
        for (SubStringCount = 0; SubStringCount < 5; SubStringCount++)
        {
            IpAddrSubString[SubStringCount] = '\0';
        }
        SubStringCount = 0;
        
        while (IpAddrString[StringCount] != '\0')
        {
            // Get string of every byte of the IP addresses
            while (IpAddrString[StringCount] != '.' )
            {
                if (IpAddrString[StringCount] == '\0')
                {
                    break;
                }
                IpAddrSubString[SubStringCount++] = IpAddrString[StringCount++];
            }

            // Conver to UCHAR and save it in the Context
            if (IpAddrString[StringCount] == '.' || IpAddrString[StringCount] == '\0')
            {
                switch (IpAddrByte)
                {
                case 3:
                    pRoutingContext->pIpAddrList[IpAddrListIndex].IpAddr3 = (UCHAR)atoi(&IpAddrSubString[0]);
                    break;

                case 2:
                    pRoutingContext->pIpAddrList[IpAddrListIndex].IpAddr2 = (UCHAR)atoi(&IpAddrSubString[0]);
                    break;

                case 1:
                    pRoutingContext->pIpAddrList[IpAddrListIndex].IpAddr1 = (UCHAR)atoi(&IpAddrSubString[0]);
                    break;

                case 0:
                    pRoutingContext->pIpAddrList[IpAddrListIndex].IpAddr0 = (UCHAR)atoi(&IpAddrSubString[0]);
                    break;

                default:
                    break;
                }

                // Clear out SubString
                for (SubStringCount = 0; SubStringCount < 5; SubStringCount++)
                {
                    IpAddrSubString[SubStringCount] = '\0';
                }

                SubStringCount = 0;
                StringCount++;

                if (IpAddrByte-- == 0)
                {
                    // Move to the next IP Address
                    IpAddrByte = 3;
                    break;
                }
            }
        }

        // Reset the string count and increment the list index for the next IP Address entry
        IpAddrListIndex++;
        StringCount = 0;
    }

    return true;
}

// __getPrefixesOnShortestPath()
// This procedure gets the list of longest prefix match for the shortest path between 
// source and destination nodes
BOOLEAN __getPrefixesOnShortestPath(struct _ROUTING_CONTEXT* pRoutingContext)
{
    PSSP_CONTEXT    pSspContext = pRoutingContext->pSspContext;
    PTRIE_CONTEXT   pTrieContext = pRoutingContext->pTrieContext;
    UINT            PathNodeIndex = 0;
    UINT            VertexItr = 0;
    
    // First get the shortest path using the dijsktra algorithm
    pSspContext->SrcNode = pSspContext->SspArgs.SrcNode;
    pSspContext->DestNode = pSspContext->SspArgs.DestNode;
    
    if(!pSspContext->SspFnTbl.getShortestPath(pSspContext))
    {
        // No Path to Destination!
        return false;
    }

    pRoutingContext->MinWeight = pSspContext->pMinWeightArray[pSspContext->DestNode];

    // Allocate the prefix array 
    pRoutingContext->PrefixArray = (char*)malloc(sizeof(char)* 33 * (pSspContext->NumNodesInPath - 1));

    // Path Array in the SSP context has the nodes on the path
    // For every node on the path, build the routing table, build tries for all the nodes and get the longest prefix match
    for (PathNodeIndex = 0; PathNodeIndex < pSspContext->NumNodesInPath  - 1; PathNodeIndex++)
    {
        // Build New Trie!
        pTrieContext->TrieFnTbl.createNewTrieHeadNode(pTrieContext);

        // Build the routing table by adding all the (Dest IP address, NextHop) pairs to the trie
        for (VertexItr = 0; VertexItr < pSspContext->NumVertices; VertexItr++)
        {
            // Update the src and dest nodes for djikstra
            pSspContext->SrcNode = pSspContext->pPath[PathNodeIndex];
            pSspContext->DestNode = VertexItr;

            // Ignore the pair where src and destination are same
            if (pSspContext->SrcNode == pSspContext->DestNode)
            {
                continue;
            }

            // Run Djikstra to get the next hop and add it to the Trie
            pTrieContext->TrieFnTbl.insertTrieNode(pTrieContext, 
                pTrieContext->TrieFnTbl.createTrieNode(pRoutingContext->pIpAddrList[VertexItr], pSspContext->SspFnTbl.getNextHopInShortestPath(pSspContext)));

        }

        // Merge the Sub Tries! 
        pTrieContext->TrieFnTbl.mergeSubTries(pTrieContext->pTrieHeadNode);

        // Get the Prefix in the array 
        pRoutingContext->PrefixArrayIndex += sprintf(&pRoutingContext->PrefixArray[pRoutingContext->PrefixArrayIndex], "%s ",
            pTrieContext->TrieFnTbl.findTrieNode(pTrieContext, pRoutingContext->pIpAddrList[pSspContext->SspArgs.DestNode])->LongestPrefix);
    }

    return true;
}

// __printPrefixes()
// This procedure prints the prefixes in the shortest path to stdout
BOOLEAN __printPrefixes(struct _ROUTING_CONTEXT* pRoutingContext)
{
    // Print the weight and the string
    printf("%d\n", pRoutingContext->MinWeight);
    printf("%s", pRoutingContext->PrefixArray);
    printf("\n");

    return true;
}





