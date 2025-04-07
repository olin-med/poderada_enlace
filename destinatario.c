/* destinatario.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER "01111110"
#define TERMINATOR "01111110"

// Função que verifica se um número é potência de 2.
int is_power_of_two(int x)
{
    return (x & (x - 1)) == 0;
}

// Função que decodifica o código de Hamming: corrige erro se houver e extrai os dados.
char *hamming_decode(const char *codeword)
{
    int n = strlen(codeword);
    // Cria uma cópia do código para manipulação.
    char *code = (char *)malloc((n + 1) * sizeof(char));
    strcpy(code, codeword);

    // Calcula a síndrome para identificar erro.
    int error_position = 0;
    for (int p = 1; p <= n; p *= 2)
    {
        int parity = 0;
        for (int i = p; i <= n; i++)
        {
            if (i & p)
            {
                if (code[i - 1] == '1')
                    parity ^= 1;
            }
        }
        if (parity != 0)
        {
            error_position += p;
        }
    }
    // Se a posição do erro for válida, corrige o bit.
    if (error_position != 0 && error_position <= n)
    {
        code[error_position - 1] = (code[error_position - 1] == '0') ? '1' : '0';
    }
    // Extrai os bits de dados (desprezando os bits de paridade).
    int parity_count = 0;
    for (int i = 1; i <= n; i++)
    {
        if (is_power_of_two(i))
            parity_count++;
    }
    int data_length = n - parity_count;
    char *data = (char *)malloc((data_length + 1) * sizeof(char));
    int index = 0;
    for (int i = 1; i <= n; i++)
    {
        if (!is_power_of_two(i))
        {
            data[index++] = code[i - 1];
        }
    }
    data[data_length] = '\0';
    free(code);
    return data;
}

int main()
{
    char frame[2048];
    // Lê o frame completo da entrada padrão.
    if (fgets(frame, sizeof(frame), stdin) == NULL)
    {
        fprintf(stderr, "Erro ao ler frame.\n");
        exit(1);
    }
    frame[strcspn(frame, "\n")] = '\0';

    // Procura pelo cabeçalho.
    char *start = strstr(frame, HEADER);
    if (start == NULL)
    {
        fprintf(stderr, "Cabeçalho não encontrado.\n");
        exit(1);
    }
    start += strlen(HEADER); // pula o cabeçalho
    // Procura pelo terminador a partir do payload.
    char *end = strstr(start, TERMINATOR);
    if (end == NULL)
    {
        fprintf(stderr, "Terminador não encontrado.\n");
        exit(1);
    }
    // Extrai o payload codificado.
    int encoded_length = end - start;
    char *encoded_payload = (char *)malloc((encoded_length + 1) * sizeof(char));
    strncpy(encoded_payload, start, encoded_length);
    encoded_payload[encoded_length] = '\0';

    // Decodifica o payload utilizando o código de Hamming.
    char *decoded_payload = hamming_decode(encoded_payload);
    // Imprime a mensagem decodificada.
    printf("%s", decoded_payload);

    free(encoded_payload);
    free(decoded_payload);
    return 0;
}
