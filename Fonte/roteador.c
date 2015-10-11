/*
 * Project name:
   Simulador de roteadores usando sockets UDP

 * Autor:
   Igor Beilner

 * Revision History:
   Versão 1.0
   10/10/2015

 * Description:


* Test configuration:

 * NOTES:
*/

#include "roteador.h"

int main() {
   digraph_t *G;

   G = init();

   digraphShow(G);

   digraphExit(G);
   return 0;
}
