/*
 * Project name:
   Simulador de um protocolo de roteamento entre roteadores de uma rede
   usando sockets UDP

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
   Vertex v;
   int ID;
   digraph_t *G;

   G = init();
   printf("ID: ");
   scanf("%d", &ID);
   printf("\n");
   if(ID > G->V || ID < 1) {
      printf("ID do roteador invalido\n");
      return 0;
   }

   //initRouter(ID);

   //for(v=0; v<G->V; v++)
   dijkstra(G, ID-1);

   //digraphShow(G);

   digraphExit(G);
   return 0;
}
