#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int is_prime(int n) {
    if (n < 2) {
        return 0;
    }
    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) {
            return 0; 
        }
    }
    return 1; 
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Ошибка: не передано имя файла\n");
        return -1;
    }

    FILE *file = fopen(argv[1], "w");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return -1;
    }

    int num;
    int response;

    while (read(0, &num, sizeof(num)) == sizeof(num)) {
        
        if (num < 0 || is_prime(num) == 1) {
            response = 0;
            if (write(1, &response, sizeof(response)) != sizeof(response)) {
                perror("Ошибка записи стоп-сигнала");
            }
            break;
        }

        if (num > 3) {
            fprintf(file, "%d\n", num);
            fflush(file); 
        }

        response = 1;
        if (write(STDOUT_FILENO, &response, sizeof(response)) != sizeof(response)) {
            perror("Ошибка ответа родителю");
            break;
        }
    }

    fclose(file);
    return 0;
}