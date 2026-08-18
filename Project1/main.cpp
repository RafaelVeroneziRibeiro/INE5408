#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <stack>
#include <queue>
#include <vector>

using namespace std;

class Cenario
{
public:
    Cenario(string &texto, size_t indice_inicial)
    {
        size_t pos = indice_inicial;
        nome = proxima_tag_conteudo(texto, pos, "nome");
        altura = static_cast<size_t>(stoi(proxima_tag_conteudo(texto, pos, "altura")));
        largura = static_cast<size_t>(stoi(proxima_tag_conteudo(texto, pos, "largura")));
        x = static_cast<size_t>(stoi(proxima_tag_conteudo(texto, pos, "x")));
        y = static_cast<size_t>(stoi(proxima_tag_conteudo(texto, pos, "y")));
        matriz = matriz_remove_espacos(proxima_tag_conteudo(texto, pos, "matriz"));
        indice_final = pos;
    }
    ~Cenario() {};
    string nome;
    size_t altura;
    size_t largura;
    size_t x;
    size_t y;
    string matriz;
    size_t indice_final;

private:
    string proxima_tag(string &texto, size_t &pos)
    {
        string tag = "";
        for (; pos < texto.length(); pos++)
        {
            if (texto[pos] == '<')
            {
                pos++;
                while (texto[pos] != '>')
                {
                    tag += texto[pos];
                    pos++;
                }
                pos++;
                return tag;
            }
        }
        return tag;
    }
    string proximo_conteudo(string &texto, size_t &pos)
    {
        string txt = "";
        while (texto[pos] != '<')
        {
            txt += texto[pos];
            pos++;
        }
        while (texto[pos] != '>')
        {
            pos++;
        }
        pos++;
        return txt;
    }
    string proxima_tag_conteudo(string &texto, size_t &pos, string nome_tag)
    {
        string tag = "";
        while (tag != nome_tag)
        {
            tag = proxima_tag(texto, pos);
        }
        return proximo_conteudo(texto, pos);
    }
    string matriz_remove_espacos(string texto)
    {
        string saida;
        for (size_t i = 0; i < texto.length(); i++)
        {
            if (texto[i] == '0' || texto[i] == '1')
            {
                saida += texto[i];
            }
        }
        return saida;
    }
};

int main()
{

    string filename;
    std::cin >> filename;

    ifstream filexml(filename);
    if (!filexml.is_open())
    {
        cerr << "Erro ao abrir o arquivo " << filename << endl;
        throw runtime_error("Erro no arquivo XML");
    }

    string texto;
    char character;
    while (filexml.get(character))
    {
        texto += character;
    }

    // PROBLEMA 1: Validação de arquivo XML usando Pilha

    stack<string> pilha_tags;

    for (size_t i = 0; i < texto.length(); i++)
    {
        if (texto[i] == '<')
        {
            size_t j = i + 1;
            string tag_atual = "";

            while (j < texto.length() && texto[j] != '>')
            {
                tag_atual += texto[j];
                j++;
            }
            i = j; // Avança o índice principal

            if (!tag_atual.empty())
            {
                // Se for tag de fechamento (começa com '/')
                if (tag_atual[0] == '/')
                {
                    string nome_tag = tag_atual.substr(1); // Remove a '/'

                    if (pilha_tags.empty() || pilha_tags.top() != nome_tag)
                    {
                        cout << "erro\n";
                        return 0; // Encerra o programa em caso de erro
                    }
                    pilha_tags.pop();
                }
                // Se for tag de abertura
                else
                {
                    pilha_tags.push(tag_atual);
                }
            }
        }
    }

    // Se ao final a pilha não estiver vazia, faltou fechar tags
    if (!pilha_tags.empty())
    {
        cout << "erro\n";
        return 0;
    }


    // PROBLEMA 2: Determinação de área usando Fila (BFS)

    size_t pos = 0;

    // Procura por blocos de <cenario>
    while (true)
    {
        size_t next_cenario = texto.find("<cenario>", pos);
        if (next_cenario == string::npos)
        {
            break; // Sai do loop se não houver mais cenários
        }

        Cenario c(texto, next_cenario);
        pos = c.indice_final; // Atualiza a posição de busca no texto para o próximo cenário

        // Inicializa a Fila e a Matriz de Visitados (R)
        queue<pair<int, int>> fila;
        vector<vector<int>> R(c.altura, vector<int>(c.largura, 0));
        int area = 0;

        // O índice correspondente na string "matriz" unidimensional (fórmula 2D -> 1D)
        int start_idx = c.x * c.largura + c.y;

        // Só inicia a contagem se a posição de largada for válida e for espaço limpável ('1')
        if (c.matriz[start_idx] == '1')
        {
            fila.push({c.x, c.y});
            R[c.x][c.y] = 1;

            // Variáveis de deslocamento para a vizinhança-4 (Cima, Baixo, Esquerda, Direita)
            int dx[] = {-1, 1, 0, 0};
            int dy[] = {0, 0, -1, 1};

            while (!fila.empty())
            {
                // Remove da fila
                int cx = fila.front().first;
                int cy = fila.front().second;
                fila.pop();
                area++; // Adiciona a área do componente conexo

                // Avalia os 4 vizinhos
                for (int i = 0; i < 4; i++)
                {
                    int nx = cx + dx[i];
                    int ny = cy + dy[i];

                    // 1. Verifica limites de borda do cenário
                    if (nx >= 0 && nx < c.altura && ny >= 0 && ny < c.largura)
                    {
                        int n_idx = nx * c.largura + ny;

                        // 2. Verifica se a intensidade é 1 e se NÃO foi visitado
                        if (c.matriz[n_idx] == '1' && R[nx][ny] == 0)
                        {
                            fila.push({nx, ny});
                            R[nx][ny] = 1; // Marca como visitado imediatamente
                        }
                    }
                }
            }
        }

        // Impressão do resultado conforme especificado (Nome e Área)
        cout << c.nome << " " << area << "\n";
    }

    return 0;
}