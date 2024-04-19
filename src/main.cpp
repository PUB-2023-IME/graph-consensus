// Warning!
// Tomar cuidado na hora de setar cor no algoritmo de simulação, eu devo lembrar que apenas objetos do tipo 'Node' tem o atributo set_color, ou seja, 
// quando eu for lidar com objetos do tipo CRGB, como é o caso dos objetos do array G.nodes_color (Para um Graph G), eu devo setar a cor manualmente, isto é,
// G.nodes_color[i] = CRGB(r,g,b)

/*              Bibliotecas que serão usadas no código              */
// Biblioteca responsável por fornecer o tipo 'uint8_t'.
#include <cstdint>
// Biblioteca responsável por fornecer o tipo 'vector'.
#include <vector>
// Bibliotecas responsáveis pela entrada e saída de dados.
#include <stdio.h>
#include <iostream>
// Biblioteca responsável por fornecer os métodos de manipulação de string que serão usados nesse código.
#include <string>
// Biblioteca responsável por fornecer os métodos necessários para lidar com números pseudo-aleatórios.
#include <random>
// Biblioteca responsável por fornecer os métodos necessários para controlar os leds.
#include <FastLED.h>


/*              Structs que serão usadas no código              */
// Struct responsável por armazenar algumas constantes que serão necessárias no código.
struct Configs{
    // Constante responsável por setar a quantidade de nós que um objeto do tipo 'Graph' terá.
    static const uint8_t NODES_NUMBER; // Perceba que, essa constante também setará, por consequência, o número de leds.
    // Constante responsável por setar o pino que enviará os dados para a placa.
    static const uint8_t LED_PIN;
    // Constante responsável por setar o valor de uma das cores que os leds podem assumir.
    static const CRGB COLOR_1;
    // Constante responsável por setar o valor de uma das cores que os leds podem assumir.
    static const CRGB COLOR_2;
    // Constante responsável por setar a "cor apagada" aos leds.
    static const CRGB OFF;
    // Constante responsável por setar a probabilidade de um led não mudar a sua cor em uma dada iteração da simulação do algoritmo de consenso.
    // OBS: Usaremos a escala de 0 a 100 para evitar de usar a comparação entre números do tipo 'float' ou 'double'. Ou seja,
    // SAME_COLOR_CHANCE = 55 representa uma probabilidade de 55% de um led não mudar a sua cor em uma dada iteração da simulação, por exemplo.
    static const int SAME_COLOR_CHANCE;
    // Constante responsável por setar a quantidade máxima de vértices que cada jogador poderá escolher.
    static const uint8_t NUMBER_OF_VERTICES_TO_CHOOSE;
};


/*              Constantes que serão utilizadas no código               */
// Inicializa os membros estáticos da struct Configs fora da definição da struct.
const uint8_t Configs::NODES_NUMBER = 100;
const uint8_t Configs::LED_PIN = 12;
const CRGB Configs::COLOR_1 = CRGB(255, 0, 0);
const CRGB Configs::COLOR_2 = CRGB(0, 255, 0);
const CRGB Configs::OFF = CRGB(0, 0, 0);
const int Configs::SAME_COLOR_CHANCE = 20;
const uint8_t Configs::NUMBER_OF_VERTICES_TO_CHOOSE = 3;


/*        Declaração da assinatura das funções auxiliares       */
void generateGraph();
void simulate();


/*              Classes que serão usadas no código              */
class Node{
    /*
        Descrição:
            Essa classe é responsável por implementar uma abstração específica de um nó que representa o vértice de um grafo.
        
        Atributos:
            public:
                led_number (uint8_t): Número inteiro que representa o número do led do nó em questão.
                current_color (CRGB): Struct do tipo 'CRGB' que representa a cor atual que o nó possui.
                next_color (CRGB): Struct do tipo 'CRGB' que representa a próxima cor que o nó possuirá.
                neighbors (vector<uint8_t>): Array de inteiros que indica quais são os vizinhos do nó, isto é, que indica quais nós estão 
                                             conectados ao nó em questão. Para tal, esse array armazena os números dos vértices que representam
                                             cada um dos vizinhos do nó em questão.
            private:
                neighbors_count (uint8_t): Valor inteiro que representa a quantidade de vizinhos do nó.

        Métodos:
            public:
                void set_current_color(CRGB color_to_set): Seta a cor 'color_to_set' no atributo 'current_color' do nó.
                void set_next_color(CRGB color_to_set): Seta a cor 'color_to_set' no atributo 'next_color' do nó.
                void set_neighbor(uint8_t neighbor_number): Adiciona um novo vizinho ao nó.
                boolean is_neighbor(uint8_t neighbor_number): Verifica se um dado nó é vizinho do nó em questão.
                std::ostream& operator<<(std::ostream& os, const Node& node): Cria uma visualização para objetos do tipo 'Node'.

                OBS: Não serão adicionados métodos get para os atributos, a ideia é que eles sejam acessados diretamente.
                
            private:
                std::string get_neighbors_as_string(): Retorna uma string que mostra os vizinhos do nó seguindo o formato '[vizinho_1,...,vizinho_N]'.
                                                       Esse método auxilia na construção do método que sobrecarrega o operador '<<'.
    */

    public:
        uint8_t led_number;
        CRGB current_color;
        CRGB next_color;
        std::vector<uint8_t> neighbors;

        // Cria um construtor personalizado. Repare que, caso não seja passado nenhum parâmetro para o construtor da classe 'Node',
        // o construtor padrão criará um objeto do tipo 'Node' que possui o atributo 'number' == 255.
        // Repare que os atributos públicos led_number, current_color e next_color, além do atributo privado 'neighbors_count' são inicializados 
        // no array de inicialização do construtor da classe 'Node', enquanto o atributo público neighbors é inicializado dentro 
        // do construtor em questão.
        Node(uint8_t number = 255) : led_number(number), current_color(Configs::OFF), next_color(Configs::OFF),neighbors_count(0){}

        // Método responsável por setar o atributo 'led_number'
        void set_led_number(uint8_t number){
            this->led_number = number;
        }

        // Método responsável por setar uma nova cor no atributo 'current_color' de um objeto do tipo 'Node'.
        void set_current_color(CRGB color_to_set){
            this->current_color = color_to_set;
        }

        // Método responsável por setar uma nova cor no atributo 'next_color' de um objeto do tipo 'Node'.
        void set_next_color(CRGB color_to_set){
            this->next_color = color_to_set;
        }

        // Método responsável por setar um novo vizinho a um objeto do tipo 'Node'.
        void set_neighbor(uint8_t neighbor_number){
            this->neighbors.push_back(neighbor_number);
            this->neighbors_count ++;
        }

        // Método responsável por checar se um dado nó é vizinho do nó em questão.
        bool is_neighbor(uint8_t neighbor_number){
            for(uint8_t i = 0; i < this->neighbors.size(); i++){
                if(this->neighbors[i] == neighbor_number){
                    return true;
                }
            }
            return false;
        }

        // Método responsável por gerar uma visualização de objetos do tipo 'Node'.
        friend std::ostream& operator<<(std::ostream& os, const Node& node) {
            // Imprime o número do nó.
            os << "Led Number: " << static_cast<int>(node.led_number) << std::endl; 
            // Imprime a cor RGB do nó.
            os << "Node Color (R,G,B): " << static_cast<int>(node.current_color.r) << "," 
                                        << static_cast<int>(node.current_color.g) << "," 
                                        << static_cast<int>(node.current_color.b) << std::endl; 
            // Imprime os vizinhos do nó.
            os << "Node neighbors: " << node.get_neighbors_as_string() << std::endl;

            return os;
        }
    private:
        uint8_t neighbors_count;

        // Repare que a função abaixo é setada como 'const', isso quer dizer que essa função não modificará o estado do objeto em que ela é chamada.
        // Método responsável por retornar uma string que contém uma representação que exibe os atributos de um objeto do tipo 'Node'.
        std::string get_neighbors_as_string() const {
            std::string neighbors_representation = "[";
            for(uint8_t i = 0; i < this->neighbors.size(); i++){
                neighbors_representation += std::to_string(this->neighbors[i]) + ", ";
            }

            // Remove o último espaço e a última vírgula.
            neighbors_representation.pop_back();
            neighbors_representation.pop_back();

            neighbors_representation += "]";
            
            return neighbors_representation;
        }
};

class Graph{
    /*
        Descrição:
            Essa classe é responsável por implementar a abstração de um grafo. Para tal, consideraremos um grafo como sendo um array
            fixo de nós, sendo que, a posição i do array citado representa o vértice de número i.
        Atributos:
            public:
                NODES_NUMBER (const uint8_t): Variável que guardará a quantidade de nós que o grafo terá, isto é, a quantidade de elementos
                do array que representa o grafo.
                nodes (Node[]): Array de objetos do tipo 'Node' que conterá todos os nós do grafo. 
                nodes_color (CRGB[]): Array de objetos do tipo 'CRGB' que conterá as cores que cada led assumirá no estado atual do grafo.
            private:
                current_nodes_number (uint8_t): Variável que guardará a quantidade atual de nós que o grafo possui, isto é, a quantidade de 
                elementos que o array que representa o grafo possui atualmente.
        Métodos:
            public:
                void set_node(Node node): Método responsável por adicionar um nó ao grafo, isto é, adicionar um objeto do tipo 'Node' ao array
                que representa o grafo.
                friend std::ostream& operator<<(std::ostream& os, const Graph& graph): Método responsável por gerar uma representação 
                para o grafo.

            private:
                N/A
    */          

    public:

        static const uint8_t NODES_NUMBER = Configs::NODES_NUMBER;
        Node nodes[NODES_NUMBER];
        CRGB nodes_color[NODES_NUMBER];

        // Cria um construtor para a classe 'Graph'.
        Graph() : current_nodes_number(0){}

        // Método responsável por adicionar um nó ao grafo e também por adicionar a cor atual desse nó ao array de objetos do tipo 'CRGB'
        // que guardará as cores que cada led assumirá no estado atual do grafo.
        void set_node(const Node& node){
            if(current_nodes_number < NODES_NUMBER){
                this->nodes[current_nodes_number] = node;
                nodes_color[current_nodes_number] = node.current_color;
                current_nodes_number++;
            }else{
                printf("Erro, o grafo já atingiu a sua quantidade máxima de nós.");
            }   
        }

        // Método responsável por adicionar uma visualização a objetos do tipo 'Graph'.
        // Perceba que o método abaixo utiliza a visualização gerada para objetos do tipo 'Node'.
        friend std::ostream& operator<<(std::ostream& os, const Graph& graph) {
            os << "\nGraph nodes: {\n" << std::endl;
            for (uint8_t i = 0; i < graph.current_nodes_number; i++) {
                os << "Vertex Number: " + std::to_string(i) << std::endl;
                os << graph.nodes[i] << std::endl;
            }
            os << "}" << std::endl;
            
            return os;
        }
    private:
        uint8_t current_nodes_number;
};


/*        Variáveis globais usadas no código        */
Graph G;


/*        Setup da placa        */
void setup() {
  // Com base na configuração abaixo, sempre que usarmos 'FastLED.show()' as cores que estiverem em 'G.nodes_color' serão as cores que irão ser exibidas
  // nos leds. Por conta disso, convencionaremos na hora de implementarmos a simulação que o array 'G.nodes' será o array que guardará o estado antigo dos
  // leds, enquanto o array 'G.nodes_color' será o array que guardará o estado novo dos leds. Dito isso, É IMPORTANTE RESSALTAR QUE 'G_nodes_color' e 'G.nodes'
  // são estruturas de dados diferentes, já que, 'G_nodes' é um array de objetos do tipo 'Node', enquanto 'G_nodes_color' é um array de objetos do tipo 'CRGB'.
  // Contudo, como todo 'G_nodes[i]' possui o atributo color que é do tipo 'CRGB', podemos operar com os dois arrays citados como sendo os vetores de estado.
  FastLED.addLeds<NEOPIXEL, Configs::LED_PIN>(G.nodes_color, Configs::NODES_NUMBER); // Inicializa a biblioteca FastLED
  
  generateGraph();
}

void loop() {
    /*
        Roda uma vez no loop {
            // Pedir os inputs do usuário. 
            // Modificar o grafo com base nos inputs (Estado inicial do grafo, isto é, os usuários já escolheram os vértices que eles irão colorir).
            // Mostrar o estado inicial do grafo (Leds) (incluindo as alterações na cor feita pelos usuários).
            // delay
        }
    */
    /*
        Loop {
            // Simulate();
            // Mostrar o estado atual do grafo.
            // Delay
        }
    */
}


/*        Funções auxiliares        */
void generateGraph(){
    
    // A estrutura de dados abaixo possui 'Configs::NODES_NUMBER' linhas pois cada linha i representa os vizinhos do
    // vértice 1, e, como a constante 'Configs::NODES_NUMBER' guarda o número de vértices, consequentemente, ela será
    // a responsável por setar o número de linhas na estrutura de dados abaixo. Além disso, a estrutura de dados abaixo
    // é setada como tendo 9 colunas pois esse é o tamanho da maior das linhas. 

    std::vector<std::vector<uint8_t>> ordered_pairs_connection;

    ordered_pairs_connection.push_back({4, 26}); //Vizinhos do vértice 0
    ordered_pairs_connection.push_back({2, 4}); //Vizinhos do vértice 1
    ordered_pairs_connection.push_back({1, 3}); //Vizinhos do vértice 2
    ordered_pairs_connection.push_back({2, 5, 6}); //Vizinhos do vértice 3
    ordered_pairs_connection.push_back({0, 1, 5, 22, 28}); //Vizinhos do vértice 4
    ordered_pairs_connection.push_back({3, 4, 6, 21, 23}); //Vizinhos do vértice 5
    ordered_pairs_connection.push_back({3, 5, 21}); //Vizinhos do vértice 6
    ordered_pairs_connection.push_back({8}); //Vizinhos do vértice 7
    ordered_pairs_connection.push_back({7, 9, 10, 11, 21}); //Vizinhos do vértice 8
    ordered_pairs_connection.push_back({8}); //Vizinhos do vértice 9
    ordered_pairs_connection.push_back({8, 13, 14}); //Vizinhos do vértice 10
    ordered_pairs_connection.push_back({8, 12, 16, 20, 22}); //Vizinhos do vértice 11
    ordered_pairs_connection.push_back({11, 14, 15, 16}); //Vizinhos do vértice 12
    ordered_pairs_connection.push_back({10, 99}); //Vizinhos do vértice 13
    ordered_pairs_connection.push_back({10, 12, 15, 99}); //Vizinhos do vértice 14
    ordered_pairs_connection.push_back({12, 14, 16, 17, 82}); //Vizinhos do vértice 15
    ordered_pairs_connection.push_back({11, 12, 15, 17, 18}); //Vizinhos do vértice 16
    ordered_pairs_connection.push_back({15, 16, 18, 81}); //Vizinhos do vértice 17
    ordered_pairs_connection.push_back({16, 17, 27}); //Vizinhos do vértice 18
    ordered_pairs_connection.push_back({20, 35}); //Vizinhos do vértice 19
    ordered_pairs_connection.push_back({11, 19, 21, 23}); //Vizinhos do vértice 20
    ordered_pairs_connection.push_back({5, 6, 8, 20, 22, 25}); //Vizinhos do vértice 21
    ordered_pairs_connection.push_back({4, 11, 21, 23, 24}); //Vizinhos do vértice 22
    ordered_pairs_connection.push_back({5, 20, 22, 29, 35}); //Vizinhos do vértice 23
    ordered_pairs_connection.push_back({22, 26}); //Vizinhos do vértice 24
    ordered_pairs_connection.push_back({21, 27}); //Vizinhos do vértice 25
    ordered_pairs_connection.push_back({0, 24}); //Vizinhos do vértice 26
    ordered_pairs_connection.push_back({18, 25}); //Vizinhos do vértice 27
    ordered_pairs_connection.push_back({4, 30}); //Vizinhos do vértice 28
    ordered_pairs_connection.push_back({23, 30, 31, 51}); //Vizinhos do vértice 29
    ordered_pairs_connection.push_back({28, 29, 52}); //Vizinhos do vértice 30
    ordered_pairs_connection.push_back({29, 32, 51}); //Vizinhos do vértice 31
    ordered_pairs_connection.push_back({31, 46}); //Vizinhos do vértice 32
    ordered_pairs_connection.push_back({35, 38, 52}); //Vizinhos do vértice 33
    ordered_pairs_connection.push_back({36, 50, 51}); //Vizinhos do vértice 34
    ordered_pairs_connection.push_back({19, 23, 33, 37, 47}); //Vizinhos do vértice 35
    ordered_pairs_connection.push_back({34, 40, 44, 50}); //Vizinhos do vértice 36
    ordered_pairs_connection.push_back({35, 38, 39}); //Vizinhos do vértice 37
    ordered_pairs_connection.push_back({33, 37, 39, 40}); //Vizinhos do vértice 38
    ordered_pairs_connection.push_back({37, 38, 42}); //Vizinhos do vértice 39
    ordered_pairs_connection.push_back({36, 38, 41, 42, 43}); //Vizinhos do vértice 40
    ordered_pairs_connection.push_back({40, 53, 73, 82}); //Vizinhos do vértice 41
    ordered_pairs_connection.push_back({39, 40, 44}); //Vizinhos do vértice 42
    ordered_pairs_connection.push_back({40, 44, 63}); //Vizinhos do vértice 43
    ordered_pairs_connection.push_back({36, 42, 43, 45, 46, 47}); //Vizinhos do vértice 44
    ordered_pairs_connection.push_back({44}); //Vizinhos do vértice 45
    ordered_pairs_connection.push_back({32, 44}); //Vizinhos do vértice 46
    ordered_pairs_connection.push_back({35, 44, 48}); //Vizinhos do vértice 47
    ordered_pairs_connection.push_back({47, 51}); //Vizinhos do vértice 48
    ordered_pairs_connection.push_back({50}); //Vizinhos do vértice 49
    ordered_pairs_connection.push_back({34, 36, 49}); //Vizinhos do vértice 50
    ordered_pairs_connection.push_back({29, 31, 34, 48}); //Vizinhos do vértice 51
    ordered_pairs_connection.push_back({30, 33}); //Vizinhos do vértice 52
    ordered_pairs_connection.push_back({41, 54, 56}); //Vizinhos do vértice 53
    ordered_pairs_connection.push_back({53, 55, 62, 63}); //Vizinhos do vértice 54
    ordered_pairs_connection.push_back({54, 58, 65}); //Vizinhos do vértice 55
    ordered_pairs_connection.push_back({53, 57, 58, 61}); //Vizinhos do vértice 56
    ordered_pairs_connection.push_back({56, 58, 59, 61, 66, 69}); //Vizinhos do vértice 57
    ordered_pairs_connection.push_back({55, 56, 57, 59, 60, 66}); //Vizinhos do vértice 58
    ordered_pairs_connection.push_back({57, 58, 69, 70}); //Vizinhos do vértice 59
    ordered_pairs_connection.push_back({58, 65, 66, 67, 87}); //Vizinhos do vértice 60
    ordered_pairs_connection.push_back({56, 57, 62, 67}); //Vizinhos do vértice 61
    ordered_pairs_connection.push_back({54, 61, 63, 64, 65, 72}); //Vizinhos do vértice 62
    ordered_pairs_connection.push_back({43, 54, 62, 64}); //Vizinhos do vértice 63
    ordered_pairs_connection.push_back({62, 63, 73, 81}); //Vizinhos do vértice 64
    ordered_pairs_connection.push_back({55, 60, 62, 67, 71, 72, 74}); //Vizinhos do vértice 65
    ordered_pairs_connection.push_back({57, 58, 60, 67, 70}); //Vizinhos do vértice 66
    ordered_pairs_connection.push_back({60, 61, 65, 66, 71, 74, 75}); //Vizinhos do vértice 67
    ordered_pairs_connection.push_back({69, 74}); //Vizinhos do vértice 68
    ordered_pairs_connection.push_back({57, 59, 68, 76, 77}); //Vizinhos do vértice 69
    ordered_pairs_connection.push_back({59, 66, 71, 75, 77}); //Vizinhos do vértice 70
    ordered_pairs_connection.push_back({65, 67, 70, 72, 78, 80, 86}); //Vizinhos do vértice 71
    ordered_pairs_connection.push_back({62, 65, 71, 73, 80}); //Vizinhos do vértice 72
    ordered_pairs_connection.push_back({41, 64, 72, 80, 81, 83}); //Vizinhos do vértice 73
    ordered_pairs_connection.push_back({65, 67, 68, 78, 80, 87}); //Vizinhos do vértice 74
    ordered_pairs_connection.push_back({67, 70, 76, 79}); //Vizinhos do vértice 75
    ordered_pairs_connection.push_back({69, 75, 77, 78, 87, 89}); //Vizinhos do vértice 76
    ordered_pairs_connection.push_back({69, 70, 76, 94}); //Vizinhos do vértice 77
    ordered_pairs_connection.push_back({71, 74, 76, 79, 84, 86, 90}); //Vizinhos do vértice 78
    ordered_pairs_connection.push_back({75, 78, 80, 85}); //Vizinhos do vértice 79
    ordered_pairs_connection.push_back({71, 72, 73, 74, 79, 84, 85}); //Vizinhos do vértice 80
    ordered_pairs_connection.push_back({17, 64, 73, 83}); //Vizinhos do vértice 81
    ordered_pairs_connection.push_back({15, 41, 83, 92}); //Vizinhos do vértice 82
    ordered_pairs_connection.push_back({73, 81, 82, 84, 92}); //Vizinhos do vértice 83
    ordered_pairs_connection.push_back({78, 80, 83, 91, 93}); //Vizinhos do vértice 84
    ordered_pairs_connection.push_back({79, 80, 90, 93, 96}); //Vizinhos do vértice 85
    ordered_pairs_connection.push_back({71, 78, 87, 88, 93}); //Vizinhos do vértice 86
    ordered_pairs_connection.push_back({60, 74, 76, 86, 88}); //Vizinhos do vértice 87
    ordered_pairs_connection.push_back({86, 87, 89, 90}); //Vizinhos do vértice 88
    ordered_pairs_connection.push_back({76, 88, 90}); //Vizinhos do vértice 89
    ordered_pairs_connection.push_back({78, 85, 88, 89, 91, 93, 94, 96}); //Vizinhos do vértice 90
    ordered_pairs_connection.push_back({84, 90, 92, 93}); //Vizinhos do vértice 91
    ordered_pairs_connection.push_back({82, 83, 91, 99}); //Vizinhos do vértice 92
    ordered_pairs_connection.push_back({84, 85, 86, 90, 91, 95, 97, 98, 99}); //Vizinhos do vértice 93
    ordered_pairs_connection.push_back({77, 90, 95}); //Vizinhos do vértice 94
    ordered_pairs_connection.push_back({93, 94, 96, 97}); //Vizinhos do vértice 95
    ordered_pairs_connection.push_back({85, 90, 95}); //Vizinhos do vértice 96
    ordered_pairs_connection.push_back({93, 95, 98}); //Vizinhos do vértice 97
    ordered_pairs_connection.push_back({93, 97, 99}); //Vizinhos do vértice 98
    ordered_pairs_connection.push_back({13, 14, 92, 93, 98}); //Vizinhos do vértice 99

    for (size_t i = 0; i < ordered_pairs_connection.size(); i++) {
        Node node;
        for (size_t j = 0; j < ordered_pairs_connection[i].size(); j++) {
            node.set_neighbor(ordered_pairs_connection[i][j]);
        }
        G.set_node(node);
    }

    // A matriz abaixo possui 100 linhas pois cada linha representa um par (led,vértice) e o grafo que iremos usar 
    // nesse código possui 100 vértices. Além disso, como cada linha representa um par (led, vértice), o número de
    // colunas será igual à 2.
    uint8_t led_vertex_relation[Configs::NODES_NUMBER][2] = {
        {1, 30}, {2, 28}, {3, 0}, {4, 1}, {5, 2}, {6, 4}, {7, 26}, {8, 27}, {9, 29}, {10, 31}, {11, 52}, 
        {12, 51}, {13, 33}, {14, 32}, {15, 24}, {16, 25}, {17, 5}, {18, 3}, {19, 6}, {20, 21}, {21, 22}, 
        {22, 23}, {23, 34}, {24, 50}, {25, 49}, {26, 48}, {27, 36}, {28, 35}, {29, 20}, {30, 8}, {31, 7}, 
        {32, 9}, {33, 11}, {34, 18}, {35, 19}, {36, 37}, {37, 38}, {38, 47}, {39, 46}, {40, 45}, {41, 44},
        {42, 40}, {43, 39}, {44, 17}, {45, 16}, {46, 12}, {47, 10}, {48, 13}, {49, 14}, {50, 15}, {51, 41}, 
        {52, 42}, {53, 43}, {54, 63}, {55, 81}, {56, 82}, {57, 99}, {58, 92}, {59, 83}, {60, 73}, {61, 64}, 
        {62, 54}, {63, 53}, {64, 55}, {65, 62}, {66, 72}, {67, 80}, {68, 84}, {69, 91}, {70, 98}, {71, 97}, 
        {72, 93}, {73, 85}, {74, 79}, {75, 71}, {76, 65}, {77, 61}, {78, 56}, {79, 60}, {80, 67}, {81, 74}, 
        {82, 78}, {83, 86}, {84, 95}, {85, 96}, {86, 90}, {87, 87}, {88, 75}, {89, 68}, {90, 66}, {91, 58}, 
        {92, 57}, {93, 59}, {94, 70}, {95, 76}, {96, 88}, {97, 94}, {98, 89}, {99, 77}, {100, 69}
    };

    // Itera sobre as sublistas da lista 'led_vertex_relation'.
    for(uint8_t i = 0; i < Configs::NODES_NUMBER; i++){
        // (IMPORTANTE!)
        // Repare que o LED começa a contar do 1 e vai até o 100.

        // Salvo em duas variáveis o par (número do led, número do vértice).
        uint8_t led_number = led_vertex_relation[i][0];
        uint8_t vertex_number = led_vertex_relation[i][1];

        // Atribuo ao vértice 'vertex_number' o número do seu respectivo led na placa, isto é 'led_number'.
        G.nodes[vertex_number].set_led_number(led_number);
    }

    std::cout << G << std::endl;
}

void simulate(){
    /*
        Description: 
        Essa função é responsável por executar o algoritmo de consenso no grafo G (declarado globalmente). Dito isso, vale relembrar 
        alguns pontos, primeiramente, é importante lembrar que cada posição 'i' de 'G.nodes' representa o vértice 'i' do grafo G. Por exemplo,
        o objeto do tipo 'Node' que está na posição 1, representa o vértice 1 do grafo G. Vale dizer também que, o número do led associado a um 
        dado 'G.nodes[i]' está guardado no atributo 'led_number' do objeto do tipo 'Node' em questão. Ou seja, para o objeto do tipo 'Node' 
        que ocupa a posição 1 de 'G.nodes', o led que representa esse vértice será o led 'G.nodes[1].led_color'.
    */    

    // Cria um gerador de números aleatórios usando uma semente gerada pelo sistema.
    std::mt19937 rng(std::random_device{}());

    for(Node node: G.nodes){
        // Cria uma distribuição uniforme que gera números inteiros entre 0 e 100. Tal distribuição será usada para gerar um número 
        // pseudo-aleatórios entre 0 e 100
        std::uniform_int_distribution<int> dist(0, 100);

        // Gera um número pseudo-aleatório entre 0 e 100 usando a semente e a distribuição setadas logo acima.
        int random_int = dist(rng);
        
        // Cria uma variável booleana que representa se a cor do nó em questão deverá ser alterada ou não, com base em SAME_COLOR_CHANCE.
        bool change_color = random_int > Configs::SAME_COLOR_CHANCE; 

        if(change_color){
            // Se o fluxo de execução chegar até aqui, a cor do nó em questão deverá ser alterada.
            if(node.neighbors.empty()){
                // Se o fluxo de execução chegar até aqui, significa que o nó em questão não possui vizinhos. Logo, a sua cor não será alterada.
                // Faz com que a próxima cor do nó em questão seja igual a sua cor atual.
                node.set_next_color(node.current_color);
            }else{
                // Se o fluxo de execução chegar até aqui, significa que o nó em questão possui vizinhos. Logo, a sua cor será alterada. 
                int neighbors_number = node.neighbors.size();
                // Cria uma distribuição uniforme que gera números inteiros entre 0 e 'neighbors_number-1'. Tal distribuição será usada para 
                // gerar um número pseudo-aleatórios entre 0 e 'neighbors_number-1'.
                // PS: O fator de correção '-1' é usado pois estamos sorteando um índice de um array (0 <= array_index <= array.size() - 1).
                std::uniform_int_distribution<int> dist(0, neighbors_number-1);
                // Gera um número pseudo-aleatório entre 0 e 'neighbors_number-1'. Tal número indicará qual índice do array de vizinhos foi 
                // sorteado, isto é, qual dos vizinhos foi sorteado.
                int choosen_neighbor_index = dist(rng);
                // Guarda em uma variável o número que representa o vértice do vizinho sorteado.
                uint8_t choosen_neighbor = node.neighbors[choosen_neighbor_index];
                // Faz com que a próxima cor do nó em questão seja igual a cor atual do seu vizinho sorteado.
                node.set_next_color(G.nodes[choosen_neighbor].current_color);
            }
        }else{
            // Se o fluxo de execução chegar até aqui, o nó em questão permanecerá com a sua cor inalterada.
            // Faz com que a próxima cor do nó em questão seja igual a sua cor atual.
            node.set_next_color(node.current_color);
        }
    }
}