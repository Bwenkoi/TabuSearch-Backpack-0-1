#include <fstream>
#include <iostream>
#include <dirent.h> //ler diretorio de arquivos
#include <ctime>
#include <iomanip>
#include <string.h>

using namespace std;

// estrutura para armazenar as informações dos itens
struct tipoItem
{
    float peso;  // peso do item
    float valor; // valor do item
};

struct tipoMochila
{
    int n;            // quantidade de itens
    float capacidade; // capacidade da mochila
    float otimo;      // melhor solução conhecida
};

// estrutura para armazenar soluções da mochila
struct tipoSolucao
{
    float peso;  // peso total da solução
    float valor; // valor total dos itens
    int nitem;   // numero de itens na mochila
    int *item;   // vetor binário que registra quais itens estão na mochila (1- está na mochila)
};

//Definições da Lista
#define tamLista 7
struct listaTabu
{
    float Elementos[tamLista]; // Lista de Itens Proibidos
    int posicao;               // Posição em que a Lista se Encontra
};

//Utilizando a Lista de maneira Global
listaTabu Lista;

//Função para inicializar os Valores
void inicializaLista(){
    Lista.posicao = 0;
    for(int i = 0 ; i < tamLista ; i++){
        Lista.Elementos[i] = 0;
    }
}
//Função para Imprimir a Lista
void imprimeLista(){
    cout << "\n";
    cout << "Posicao: [" << Lista.posicao << "]";
    cout << " Valor na Lista: [" << Lista.Elementos[Lista.posicao] << "]";
}

// função lê o diretório com as instâncias e abre um menu de seleção e retorna a string contendo o nome do arquivo da instância
// e do nome do arquivo com a resposta.
void selecionaInstancia(char nomeArq1[], char nomeArq2[], char nome[])
{
    DIR *d;
    int op;
    struct dirent *dir;

    cout << "\nTipo de instancia\n================\n[1] Large Scale\n[2] Low Dimensional\n\nDigite uma opcao: ";
    cin >> op;

    switch (op)
    {
        case 1:
            strcpy(nomeArq1,"C:\\Codigos\\instances_01_KP\\large_scale");
            strcpy(nomeArq2,"C:\\Codigos\\instances_01_KP\\large_scale-optimum");
            break;
        case 2:
            strcpy(nomeArq1,"C:\\Codigos\\instances_01_KP\\low-dimensional");
            strcpy(nomeArq2,"C:\\Codigos\\instances_01_KP\\low-dimensional-optimum");
            break;
        default:
            cout<<"Opcao invalida";
            exit(0);
    }

    d = opendir(nomeArq1);
    if (d)
    {
        cout << "\nInstancias\n=========\n";
        int c = 0;
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                c++;
                cout << "[" << setw(2) << c << "] " << dir->d_name << endl;
            }
        }
        cout << "\nDigite uma opcao: ";
        cin >> op;
        if (op <= 0 || op > c)
        {
            cout << "Opcao invalida";
            closedir(d);
            exit(0);
        }
        rewinddir(d);

        c = 0;
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                c++;
                if (c == op)
                {
                    strcat(nomeArq1, "\\");
                    strcat(nomeArq1, dir->d_name);
                    strcat(nomeArq2, "\\");
                    strcat(nomeArq2, dir->d_name);
                    strcpy(nome, dir->d_name);
                    break;
                }
            }
        }

        closedir(d);
    }
}

// Lê do arquivo os dados das instância
void carregaDados(tipoMochila &mochila, tipoItem **itens, char nome[])
{
    char nomeArq1[300], nomeArq2[300];
    ifstream fin;

    selecionaInstancia(nomeArq1, nomeArq2, nome);

    cout << "\nArquivo da instancia escolhida: " << nomeArq1 << endl;
    cout << "\nArquivo contendo a solucao otima da instancia: " << nomeArq2 << endl;

    fin.open(nomeArq2);
    if (!fin.is_open())
    {
        cout << "Erro ao abrir o arquivo \"" << nomeArq2 << "\"" << endl;
        exit(0);
    }
    fin >> mochila.otimo;
    // cout<<"\nValor Otimo: "<<otimo<<endl;
    fin.close();

    fin.open(nomeArq1);
    if (!fin.is_open())
    {
        cout << "Erro ao abrir o arquivo \"" << nomeArq1 << "\"" << endl;
        exit(0);
    }

    fin >> mochila.n;
    fin >> mochila.capacidade;
    // cout<<"numero de itens: "<<n<<" capacidade: "<<capacidade<<endl;
    *itens = new tipoItem[mochila.n];   // aloca a quantidade de itens
    for (int i = 0; i < mochila.n; i++) // lendo os itens
    {
        fin >> (*itens)[i].valor >> (*itens)[i].peso;
        // cout<<"item "<<i+1<<" -- valor: "<<(*itens)[i].valor<<" peso: "<<(*itens)[i].peso<< endl;
    }
    fin.close();
}

// zera as variáveis da solução e aloca de forma dinâmica o vetor binário de solução
void criaSolucao(tipoSolucao &solucao, int n)
{
    // criando uma solução com nenhum item na mochila
    solucao.peso = 0;
    solucao.valor = 0;
    solucao.nitem = 0;
    solucao.item = new int[n];
    for (int i = 0; i < n; i++)
        solucao.item[i] = 0;
}

// imprime o vetor com os itens
void imprimeItens(tipoItem itens[], int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << "Item " << setw(4) << i + 1;
        cout << "   |  Peso: " << setw(5) << itens[i].peso;
        cout << "   |  Valor: " << setw(5) << itens[i].valor << endl;
    }
    cout << endl;
}

// imprime os dados de uma solução
void imprimeSolucao(tipoSolucao solucao, tipoMochila mochila, tipoItem itens[])
{
    int n = mochila.n;
    cout << "Solucao\n";
    cout << "=======================================\n";
    cout << "Valor Otimo:" << mochila.otimo << endl;
    cout << "Capacidade Maxima: " << mochila.capacidade << endl;
    cout << "Total de itens: " << mochila.n << endl;
    cout << "=======================================\n";
    cout << "Valor Atual: " << solucao.valor << endl;
    cout << "Peso Atual: " << solucao.peso << endl;
    cout << "Numero de itens selecionados: " << solucao.nitem << endl;
    //<< endl;

    // cout<<"Escolhidos:\n";
    /*
     for(int i=0;i<n;i++)
     {
         if(solucao.item[i]==1) //item esta na mochila
         {

             cout<<"Item "<<setw(4)<<i+1;
             cout<<"   |  Peso: "<<setw(5) <<itens[i].peso;
             cout<<"   |  Valor: "<<setw(5) <<itens[i].valor<<endl;
         }
     }
     */

    //cout << endl;
}

// cria uma solução aleatória + gulosa
void solucaoInicialAleatoria(tipoSolucao &solucao, tipoMochila mochila, tipoItem itens[])
{
    int n = mochila.n;
    int *marca = new int[n];
    int i, cont = 0;
    for (i = 0; i < n; i++)
    {
        marca[i] = 0;
    }

    while (solucao.peso < mochila.capacidade && cont < n)
    {
        i = rand() % n;
        while (marca[i] == 1)
            i = (i + 1) % n;

        if (solucao.peso + itens[i].peso <= mochila.capacidade)
        {
            solucao.item[i] = 1;
            solucao.peso = solucao.peso + itens[i].peso;
            solucao.valor = solucao.valor + itens[i].valor;
            solucao.nitem++;
        }
        marca[i] = 1;
        cont++;
    }

    delete marca;
}

// cria uma solução onde tenta adicionar 10% dos itens
void solucaoInicial(tipoSolucao &solucao, tipoMochila mochila, tipoItem itens[])
{
    int n = mochila.n;
    int *marca = new int[n];
    int i, cont = 0;
    int m = n * 0.1;
    for (i = 0; i < n; i++)
    {
        marca[i] = 0;
    }

    while (solucao.peso < mochila.capacidade && cont < m)
    {
        i = rand() % n;
        while (marca[i] == 1)
            i = (i + 1) % n;

        if (solucao.peso + itens[i].peso <= mochila.capacidade)
        {
            solucao.item[i] = 1;
            solucao.peso = solucao.peso + itens[i].peso;
            solucao.valor = solucao.valor + itens[i].valor;
            solucao.nitem++;
        }
        marca[i] = 1;
        cont++;
    }

    delete marca;
}

// avalia a vizinhança de uma solução e retorna true se encontrou um vizinho melhor.
bool avaliaVizinhanca(tipoSolucao &solucao, tipoMochila mochila, tipoItem itens[], bool tabu)
{
    int i, j, pos1 = -1;
    float mValor = solucao.valor, mPeso = solucao.peso;
    float peso_aux, valor_aux;
    float mValorTabu = 0;
    bool naLista = false;

    int n = mochila.n;
    int capacidade = mochila.capacidade;

    switch (tabu) {
        case false:
            for (i = 0; i < n; i++){ // procura a melhor solução na vizinhança
                if (solucao.item[i] == 0){ // insere o item
                    peso_aux = solucao.peso + itens[i].peso;
                    valor_aux = solucao.valor + itens[i].valor;
                }
                else { // remove o item
                    peso_aux = solucao.peso - itens[i].peso;
                    valor_aux = solucao.valor - itens[i].valor;
                }
                if (peso_aux <= capacidade && valor_aux > mValor) {
                    pos1 = i;
                    mValor = valor_aux;
                    mPeso = peso_aux;
                }
            }

            if (pos1 != -1){ // verifica se encontrou alguma solução
                if (solucao.item[pos1] == 0){
                    solucao.item[pos1] = 1;
                    solucao.nitem++;
                } else {
                    solucao.item[pos1] = 0;
                    solucao.nitem--;
                }
                solucao.valor = mValor;
                solucao.peso = mPeso;

                return true;
            }
            return false;
            break;
        case true:
            for (i = 0; i < n; i++){ // procura a melhor solução na vizinhança
                naLista = false;
                if (solucao.item[i] == 0){ // insere o item
                    peso_aux = solucao.peso + itens[i].peso;
                    valor_aux = solucao.valor + itens[i].valor;
                }
                else { // remove o item
                    peso_aux = solucao.peso - itens[i].peso;
                    valor_aux = solucao.valor - itens[i].valor;
                }
                for(j = 0 ; j < tamLista ; j++){
                    if(Lista.Elementos[j] == valor_aux){
                        naLista = true;
                    }
                }
                if (peso_aux <= capacidade && valor_aux > mValorTabu && !naLista) {
                    pos1 = i;
                    mValorTabu = valor_aux;
                    mValor = valor_aux;
                    mPeso = peso_aux;
                }
            }

            if (pos1 != -1){ // verifica se encontrou alguma solução
                if (solucao.item[pos1] == 0){
                    solucao.item[pos1] = 1;
                    solucao.nitem++;
                } else {
                    solucao.item[pos1] = 0;
                    solucao.nitem--;
                }
                solucao.valor = mValor;
                solucao.peso = mPeso;

                return true;
            }
            return false;
            break;
    }

}

// avalia a vizinhança de uma solução e retorna true se encontrou um vizinho melhor.
bool avaliaVizinhancaTamanhoDois(tipoSolucao &solucao, tipoMochila mochila, tipoItem itens[], bool tabu)
{
    int i, j, y, pos1 = -1, pos2 = -1, posd1 = -1;
    float mValor = solucao.valor, mPeso = solucao.peso;
    float peso_aux, valor_aux, peso_aux2, valor_aux2;
    bool melhorou = false, naLista = false;
    float mValorTabu = 0;

    int n = mochila.n;
    int capacidade = mochila.capacidade;

    switch (tabu) {
        case false:
            // d=1
            for (i = 0; i < n; i++)
            {
                if (solucao.item[i] == 0)
                {
                    peso_aux = solucao.peso + itens[i].peso;
                    valor_aux = solucao.valor + itens[i].valor;
                }
                else
                {
                    peso_aux = solucao.peso - itens[i].peso;
                    valor_aux = solucao.valor - itens[i].valor;
                }
                if (peso_aux <= capacidade && valor_aux > mValor)
                {
                    posd1 = i;
                    mValor = valor_aux;
                    mPeso = peso_aux;
                }
            }
            if (posd1 != -1)
            {
                melhorou = true;
            }

            // d=2
            for (i = 0; i < n - 1; i++)
            {
                if (solucao.item[i] == 0)
                {
                    peso_aux = solucao.peso + itens[i].peso;
                    valor_aux = solucao.valor + itens[i].valor;
                }
                else
                {
                    peso_aux = solucao.peso - itens[i].peso;
                    valor_aux = solucao.valor - itens[i].valor;
                }

                for (j = i + 1; j < n; j++)
                {

                    if (solucao.item[j] == 0)
                    {
                        peso_aux2 = peso_aux + itens[j].peso;
                        valor_aux2 = valor_aux + itens[j].valor;
                    }
                    else
                    {
                        peso_aux2 = peso_aux - itens[j].peso;
                        valor_aux2 = valor_aux - itens[j].valor;
                    }

                    if (peso_aux2 <= capacidade && valor_aux2 > mValor)
                    {
                        pos1 = i;
                        pos2 = j;
                        mValor = valor_aux2;
                        mPeso = peso_aux2;
                    }
                }
            }
            if (pos1 != -1)
            {
                if (solucao.item[pos1] == 0)
                {
                    solucao.item[pos1] = 1;
                    solucao.nitem++;
                }
                else
                {
                    solucao.item[pos1] = 0;
                    solucao.nitem--;
                }

                if (solucao.item[pos2] == 0)
                {
                    solucao.item[pos2] = 1;
                    solucao.nitem++;
                }
                else
                {
                    solucao.item[pos2] = 0;
                    solucao.nitem--;
                }

                solucao.valor = mValor;
                solucao.peso = mPeso;
                return true;
            }
            if (melhorou)
            {
                if (solucao.item[posd1] == 0)
                {
                    solucao.item[posd1] = 1;
                    solucao.nitem++;
                }
                else
                {
                    solucao.item[posd1] = 0;
                    solucao.nitem--;
                }
                solucao.valor = mValor;
                solucao.peso = mPeso;
                return true;
            }
            return false;
            break;
        case true:
            // d=1
            for (i = 0; i < n; i++)
            {
                naLista = false;
                if (solucao.item[i] == 0)
                {
                    peso_aux = solucao.peso + itens[i].peso;
                    valor_aux = solucao.valor + itens[i].valor;
                }
                else
                {
                    peso_aux = solucao.peso - itens[i].peso;
                    valor_aux = solucao.valor - itens[i].valor;
                }
                for(j = 0 ; j < tamLista ; j++){
                    if(Lista.Elementos[j] == valor_aux){
                        naLista = true;
                    }
                }
                if (peso_aux <= capacidade && valor_aux > mValorTabu && !naLista)
                {
                    posd1 = i;
                    mValorTabu = valor_aux;
                    mValor = valor_aux;
                    mPeso = peso_aux;
                }
            }
            if (posd1 != -1)
            {
                melhorou = true;
            }

            // d=2
            for (i = 0; i < n - 1; i++)
            {
                if (solucao.item[i] == 0)
                {
                    peso_aux = solucao.peso + itens[i].peso;
                    valor_aux = solucao.valor + itens[i].valor;
                }
                else
                {
                    peso_aux = solucao.peso - itens[i].peso;
                    valor_aux = solucao.valor - itens[i].valor;
                }

                for (j = i + 1; j < n; j++)
                {
                    naLista = false;
                    if (solucao.item[j] == 0)
                    {
                        peso_aux2 = peso_aux + itens[j].peso;
                        valor_aux2 = valor_aux + itens[j].valor;
                    }
                    else
                    {
                        peso_aux2 = peso_aux - itens[j].peso;
                        valor_aux2 = valor_aux - itens[j].valor;
                    }

                    for(y = 0 ; y < tamLista ; y++){
                        if(Lista.Elementos[y] == valor_aux){
                            naLista = true;
                        }
                    }

                    if (peso_aux2 <= capacidade && valor_aux2 > mValorTabu && !naLista)
                    {
                        pos1 = i;
                        pos2 = j;
                        mValorTabu = valor_aux2;
                        mValor = valor_aux2;
                        mPeso = peso_aux2;
                    }
                }
            }
            if (pos1 != -1)
            {
                if (solucao.item[pos1] == 0)
                {
                    solucao.item[pos1] = 1;
                    solucao.nitem++;
                }
                else
                {
                    solucao.item[pos1] = 0;
                    solucao.nitem--;
                }

                if (solucao.item[pos2] == 0)
                {
                    solucao.item[pos2] = 1;
                    solucao.nitem++;
                }
                else
                {
                    solucao.item[pos2] = 0;
                    solucao.nitem--;
                }

                solucao.valor = mValor;
                solucao.peso = mPeso;
                return true;
            }
            if (melhorou)
            {
                if (solucao.item[posd1] == 0)
                {
                    solucao.item[posd1] = 1;
                    solucao.nitem++;
                }
                else
                {
                    solucao.item[posd1] = 0;
                    solucao.nitem--;
                }
                solucao.valor = mValor;
                solucao.peso = mPeso;
                return true;
            }
            return false;
            break;
    }

}

void buscaTabu(tipoSolucao &solucao, tipoMochila mochila, tipoItem itens[], ofstream &arq)
{
    bool resultado;
    int i = 0;
    //Critério de parada: 10 iterações sem melhoras na solução global
    int criterioParada = 10;
    int falhas = 0;

    inicializaLista();
    solucaoInicial(solucao, mochila, itens);
    //solucaoInicialAleatoria(solucao, mochila, itens);
    Lista.Elementos[Lista.posicao] = solucao.valor;
    //imprimeSolucao(solucao, mochila, itens);
    //imprimeLista();

    arq << i << ";" << solucao.valor << "\n"; // gravando a solução inicial

    while (falhas <= criterioParada)
    {
        //Distância = 1
        //resultado = avaliaVizinhanca(solucao, mochila, itens, false);
        //Distância = 2
        resultado = avaliaVizinhancaTamanhoDois(solucao, mochila, itens, false);
        if (Lista.posicao == tamLista - 1 || Lista.posicao == -1) {
            Lista.posicao = 0;
            Lista.Elementos[Lista.posicao] = solucao.valor;
        } else {
            Lista.posicao++;
            Lista.Elementos[Lista.posicao] = solucao.valor;
        }
        if(!resultado){
            if(solucao.valor == mochila.otimo) break;

            falhas ++;
            if(falhas <= criterioParada){
                //Distância = 1
                //avaliaVizinhanca(solucao, mochila, itens, true);
                //Distância = 2
                avaliaVizinhancaTamanhoDois(solucao, mochila, itens, true);
            } else {
                break;
            }
        }
        //imprimeLista();
        //imprimeSolucao(solucao, mochila, itens);
        i++;
        arq << i << ";" << solucao.valor << "\n"; // gravando a solução atual
    }
    arq << -2 << ";" << falhas << "\n"; // gravando as excuções
}

void criaArquivo(ofstream &arq, tipoMochila mochila, char nome[])
{

    char nomearq[100];

    /*-------------------Usado para criar o arquivo de saída--------------------------*/
    strcpy(nomearq, nome);
    strcat(nomearq, "_saida.txt");
    arq.open(nomearq);
    if (!arq.is_open())
    {
        cout << "Erro ao criar o arquivo saida.txt" << endl;
        exit(0);
    }
    arq << "Iteração" << ";" << "Valor" <<"\n";
    arq << -1 << ";" << mochila.otimo << "\n";
    /*--------------------------------------------------------------------------------*/
}

int main()
{
    tipoItem *itens; // itens
    tipoMochila mochila;
    tipoSolucao solucao;
    char nome[100];
    ofstream arq;

    // criando a semente dos números aleatório
    unsigned seed = time(NULL);
    srand(seed);

    // escolhe e carrega os dados das instâncias
    carregaDados(mochila, &itens, nome);

    // imprimeItens(itens,n);
    criaArquivo(arq, mochila, nome);

    cout << "Executando "<<endl;
    cout << "=======================================";
    // cria a solução (zera e aloca o vetor binário)
    criaSolucao(solucao, mochila.n);

    // função de otimização
    buscaTabu(solucao, mochila, itens, arq);

    // melhor solução
    cout << "\nMelhor Solucao"<<endl;
    imprimeSolucao(solucao, mochila, itens);

    //marca final da execução no arquivo
    //arq<<"-1 -1"<<"\n";

    cout << "=======================================\n";
    cout << "FIM Execucao "<<endl;

    cout << "\nArquivo de saida criado: " << nome << "_saida.txt";

    arq.close(); // fechar o arquivo de saída

    // apaga o vetor binário criado de forma dinâmica
    delete[] solucao.item;

    // apaga o vetor de itens;
    delete[] itens;

    return 0;
}