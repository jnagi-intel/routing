
//
//  Main header file for the routing project
//

#ifndef _ROUTING_H_
#define _ROUTING_H_

#include "Types.h"
#include "Ssp.h"
#include "Trie.h"

// Args for Routing, rest of args will be stored in SSP
typedef struct _ROUTING_ARGS
{
    char*   InputIpFileName;
}ROUTING_ARGS, *PROUTING_ARGS;

// Context Definition for Routing
typedef struct _ROUTING_CONTEXT
{
    ROUTING_ARGS    RoutingArgs;
    FILE            *pInputIpFileHandle;
    IP_ADDR         *pIpAddrList;
    CHAR*           PrefixArray;
    UINT            PrefixArrayIndex;
    UINT            MinWeight;
    struct _ROUTING_FN_TBL
    {
        BOOLEAN(*parseInputFiles)(struct _ROUTING_CONTEXT* pRoutingContext);
        BOOLEAN(*getPrefixesOnShortestPath)(struct _ROUTING_CONTEXT* pRoutingContext);
        BOOLEAN (*printPrefixes)(struct _ROUTING_CONTEXT* pRoutingContext);
    }RoutingFnTbl;
    PSSP_CONTEXT    pSspContext;
    PTRIE_CONTEXT   pTrieContext;
}ROUTING_CONTEXT, *PROUTING_CONTEXT;

// Function Prototypes 
PROUTING_CONTEXT createRoutingContext();
BOOLEAN destroyRoutingContext(PROUTING_CONTEXT *ppRoutingContext);
BOOLEAN parseArguementsRouting(PROUTING_CONTEXT pRoutingContext, char *argv[]);

#endif
