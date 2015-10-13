<b>Universidade Federal da Fronteira Sul</b><br>
<b>Ciência da Computação - 6 fase</b><br>
<b>Redes de Computadores</b><br>

<i>Simulador de roteadores em uma rede. O programa obtém as informações de configuração de arquivos. Cada roteador
conversa com outros roteadores através de sockets UDP.</i>

Os enlaces da rede são representados por um grafo em lista de adjacências, como os arquivos de configuração não
possuem a quantidade de nós na rede, primeiro o programa vare todo o arquivo enlaces.config, para encontar o 
número de roteadores, depois, é inicializado um grafo que contenha a quantidade de vértices igual ao número de
roteadores. Em seguida executa-se a função nextHop(G) para calcular o vetor distancia do roteador, que contém
o próximo salto para todos os destinos alcançáveis com o menor custo.

Toda vez que um pacote é enviado inicia-se um contadot para timeout do envio, se a confirmação não é recebida
o pacote é enviado novamente, o procedimento se repete por até 3 vezes, se depois das 3 vezes a confirmação não
for recebida assume-se que o enlace caiu e não se tanta mais reenviar o pacote.

Para compilar execute: gcc *.c -o roteador -Wall -pthread

UTILIZAÇÃO:
O arquivo enlaces.config contém os enlaces da rede, sua organização é, por exemplo:
1 2 4
2 3 5

onde a primeiro coluna representa o roteador origem, a segunda coluna representa o roteador destino e a terceira
coluna representa o custo do enlace.

O arquivo roteador.config contém a configuração de cada roteador, sua organização é, por exemplo:
1 25002 127.0.0.1
2 25005 127.0.0.2
3 25007 127.0.0.3

onde a primeira coluna representa o id do roteador, a segunda coluna contém a porta do roteador indicado na primeira
coluna e a terceira coluna contém o endereço IP do roteador.

Para executar o programa os arquivos enlaces.config e roteador.config devem estar em um diretório nomeado "config"
dentro do diretório onde está o executável. Ao ser executado, uma mensagem irá solicitar o id destino e a mensagem
a ser enviada, basta então inserir o id destino e e mensagem limitada a 100 caracteres seguida de <enter>.
