//
//  Main source file for SSP
//

#include "Ssp.h"

int main(int argc, char *argv[])
{
    PSSP_CONTEXT        pSspContext = NULL;
    INT                 RetStatus = 1;

    do
    {
        // Check if the number of arguements entered by user are correct 
        if (argc != 4)
        {
            printf("Illegal arguements for ssp\n");
            RetStatus = -1;
            break;
        }

        // Reserve Memory for Context
        pSspContext = createSspContext();
        
        // Parse the arguements and fill the args 
        if (!parseArguements(pSspContext, argv))
        {
            RetStatus = -1;
            break;
        }

        // Parse Input File and fill up the Adjacency List structure to represent the graph 
        if (!pSspContext->SspFnTbl.parseGraphInputFile(pSspContext))
        {
            RetStatus = -1;
            break;
        }

        // Get Path, array of nodes from src to destination
        // Src and Dest Nodes taken from the args
        pSspContext->SrcNode = pSspContext->SspArgs.SrcNode;
	    pSspContext->DestNode = pSspContext->SspArgs.DestNode; 
        
        if (!pSspContext->SspFnTbl.getShortestPath(pSspContext))
        {
            RetStatus = -1;
            break;
        }

        // Print to std out 
        pSspContext->SspFnTbl.printPath(pSspContext);

        // deallocate the memory reserved 
        destroySspContext(&pSspContext); 

    } while (false);

    return RetStatus;
}

