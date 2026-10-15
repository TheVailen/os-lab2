#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    char filename[256];
    printf("Введите имя файла для сохранения результатов: ");

    if (scanf("%255s", filename) != 1) {
        fprintf(stderr, "Ошибка чтения имени файла\n");
        return -1;
    }

    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1) {
        perror("Ошибка при создании pipe1");
        return -1;
    }

    if (pipe(pipe2) == -1) {
        perror("Ошибка при создании pipe2");
        close(pipe1[0]);
        close(pipe1[1]);
        return -1;
    }

    printf("Каналы успешно созданы. Имя файла сохранено: %s\n", filename);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        return -1;
    }
    else if (pid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);

        if (dup2(pipe1[0], 0) == -1) {
            perror("Ошибка dup2 для stdin");
            return -1;
        }
        close(pipe1[0]);

        if (dup2(pipe2[1], 1) == -1) {
            perror("Ошибка dup2 для stdout");
            return -1;
        }
        close(pipe2[1]);

        execl("./child", "child", filename, NULL);

        perror("Ошибка запуска execl");
        return -1;
    }
    else {
        close(pipe1[0]);
        close(pipe2[1]);

        printf("Родительский процесс (PID: %d) создал дочерний (PID: %d)\n", getpid(), pid);

        int num;
        int child_response;

        while (1) {
            printf("Введите число: ");
            if (scanf("%d", &num) != 1) {
                break;
            }

            if (write(pipe1[1], &num, sizeof(num)) != sizeof(num)) {
                perror("Ошибка записи в pipe1");
                break;
            }

            ssize_t bytes_read = read(pipe2[0], &child_response, sizeof(child_response));
            
            if (bytes_read == 0) {
                break;
            } else if (bytes_read != sizeof(child_response)) {
                perror("Ошибка чтения из pipe2");
                break;
            }

            if (child_response == 0) {
                printf("Введено простое или отрицательное число. Завершаем работу\n");
                break;
            }
        }

        close(pipe1[1]);
        close(pipe2[0]);

        if (wait(NULL) == -1) {
            perror("Ошибка ожидания дочернего процесса");
        }
    }

    return 0;
}