/* remetente.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define HEADER "01111110"
#define TERMINATOR "01111110"

// Função que verifica se um número é potência de 2.
int is_power_of_two(int x)
{
    return (x & (x - 1)) == 0;
}

// Calcula o número de bits de paridade necessários para um payload de tamanho k.
int calculate_parity_bits(int k)
{
    int r = 0;
    while ((1 << r) < (k + r + 1))
    {
        r++;
    }
    return r;
}

// Função que realiza a codificação Hamming para o payload recebido.
char *hamming_encode(const char *payload)
{
    int k = strlen(payload);
    int r = calculate_parity_bits(k);
    int n = k + r; // tamanho total do código
    // Aloca um vetor para o código (índices de 1 a n; usaremos índice 0 como dummy).
    char *code = (char *)malloc((n + 1) * sizeof(char));
    if (!code)
    {
        perror("malloc");
        exit(1);
    }
    for (int i = 0; i <= n; i++)
    {
        code[i] = '0';
    }
    int data_index = 0;
    // Insere os bits de dados nas posições que não são potências de 2.
    for (int i = 1; i <= n; i++)
    {
        if (is_power_of_two(i))
        {
            // posição de bit de paridade: permanece '0' por enquanto.
        }
        else
        {
            if (data_index < k)
            {
                code[i] = payload[data_index];
                data_index++;
            }
        }
    }
    // Calcula os bits de paridade.
    for (int p = 1; p <= n; p *= 2)
    { // p: posição do bit de paridade
        int parity = 0;
        for (int i = p; i <= n; i++)
        {
            if (i & p)
            {
                if (code[i] == '1')
                    parity ^= 1;
            }
        }
        // Ajusta o bit de paridade para paridade par.
        code[p] = (parity % 2 == 0) ? '0' : '1';
    }
    // Cria a string resultado ignorando o índice 0.
    char *result = (char *)malloc((n + 1) * sizeof(char));
    if (!result)
    {
        perror("malloc");
        exit(1);
    }
    for (int i = 1; i <= n; i++)
    {
        result[i - 1] = code[i];
    }
    result[n] = '\0';
    free(code);
    return result;
}

int main(int argc, char *argv[])
{
    char payload[1024];
    if (argc > 1)
    {
        strcpy(payload, argv[1]);
    }
    else
    {
        if (fgets(payload, sizeof(payload), stdin) == NULL)
        {
            fprintf(stderr, "Erro ao ler entrada.\n");
            exit(1);
        }
        payload[strcspn(payload, "\n")] = '\0'; // remove o newline
    }
    // Validação: garantir que o payload contém apenas 0s e 1s.
    for (int i = 0; i < strlen(payload); i++)
    {
        if (payload[i] != '0' && payload[i] != '1')
        {
            fprintf(stderr, "Payload inválido. Use apenas 0s e 1s.\n");
            exit(1);
        }
    }
    // Codifica o payload usando o código de Hamming.
    char *encoded_payload = hamming_encode(payload);
    // Constrói o frame: cabeçalho + payload codificado + terminador.
    char frame[2048];
    snprintf(frame, sizeof(frame), "%s%s%s", HEADER, encoded_payload, TERMINATOR);
    // Envia o frame pela saída padrão.
    printf("%s", frame);
    free(encoded_payload);
    return 0;
}
