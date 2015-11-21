<b>Universidade Federal da Fronteira Sul</b><br>
<b>Ciência da Computação - 6 fase</b><br>
<b>Redes de Computadores</b><br>

<i>Simulador de roteadores em uma rede. O programa obtém as informações de configuração de arquivos. Cada roteador
envia pacotes para outros roteadores através de sockets UDP.</i>

Cara roteador conhece apenas seus vizinhos. Na inicialização do roteador é realizada a leitura dos arquivos enlaces.config
e roteador.config e, apesar de os arquivos conterem os dados de toda a rede, o roteador lê apenas as configurações dos
roteadores adjacêntes a ele.

Ao ser instanciado, cada roteador envia seu vetor distância para seus vizinhos, assim recebe os vetores dos vizinhos e recomputa
o seu vetor. Toda vez que seu vetor sofre alguma alteração, imediatamente reenvia para seus vizinhos, mas mesmo se não sofrer nenhuma
alteração, reenvia periodicamente para seus vizinhos. Para detecção da queda de algum enlace, cada vizinho possui um contador, que é
decrementado a cada refresh e incrementado toda vez que recebe o vetor distância do vizinho.

Toda vez que um pacote de dados é enviado inicia-se um contador para timeout do pacote, se a confirmação não é recebida
o pacote é enviado novamente, o procedimento se repete por até 3 vezes, se depois das 3 vezes a confirmação não
for recebida assume-se que o enlace caiu e não se tanta mais reenviar o pacote.

Para compilar execute: gcc *.c -o roteador -Wall -pthread

A utilização do do roteador se dá por meio da execução do arquivo roteador com ./roteador, ao ser instanciado, o programa irá
solicitar o identificar correspondente ao roteador. Em seguida, caso houver vizinho ativos roteador irá apresentar sua tabela de
roteamento da seguinte forma:
DESTINO | NHOP | CUSTO
   1    |  1   |  5
   2    |  6   |  20
   3    |  5   |  13

onde DESTINO corresponde ao roteador destino, NHOP representa o próximo salto para chegar no destino e CUSTO indica o custo
do enlace.

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
