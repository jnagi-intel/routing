//
//  Main source file for Routing
//

#include "Routing.h"

int main(int argc, char *argv[])
{
    PROUTING_CONTEXT    pRoutingContext = NULL;
    INT                 RetStatus = 1;

    do
    {
        // Check if the number of arguements entered by user are correct 
	if (argc != 5)
        {
            printf("Illegal arguements for routing\n");
            RetStatus = -1;
	    break;
        }

        // Reserve Memory for Context
        pRoutingContext = createRoutingContext();

        // Parse the arguements and fill the args 
        if (!parseArguementsRouting(pRoutingContext, argv))
        {
            RetStatus = -1;
            break;
        }

        // Parse both the Input files
        if (!pRoutingContext->RoutingFnTbl.parseInputFiles(pRoutingContext))
        {
            RetStatus = -1;
            break;
        }

        // Get the nodes in the shortest path
        if (!pRoutingContext->RoutingFnTbl.getPrefixesOnShortestPath(pRoutingContext))
        {
            RetStatus = -1;
            break;
        }

        // Print to std out 
        pRoutingContext->RoutingFnTbl.printPrefixes(pRoutingContext);

        // Deallocate the memory allocated
        destroyRoutingContext(&pRoutingContext);

    } while (false);

    return RetStatus;
}
