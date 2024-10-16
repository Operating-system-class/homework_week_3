#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

// Define the shared_data structure
struct shared_data {
    int x, y, z;
    int ready;
};

int main() {
    // Create shared memory object
    int fd = shm_open("/my_process_shm", O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("Error opening shared memory");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory object
    if (ftruncate(fd, sizeof(struct shared_data)) == -1) {
        perror("Error setting size of shared memory");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object
    struct shared_data *data = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping shared memory");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Initialize shared data
    data->ready = 0;

    // Create child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Error forking process");
        munmap(data, sizeof(struct shared_data));
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        while (data->ready == 0) {
            usleep(100); // Avoid busy waiting
        }
        data->z = data->x + data->y;
        data->ready = 0;
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        printf("Enter value for x: ");
        scanf("%d", &data->x);
        printf("Enter value for y: ");
        scanf("%d", &data->y);

        data->ready = 1;

        while (data->ready == 1) {
            usleep(100); // Avoid busy waiting
        }

        printf("The value of z is: %d\n", data->z);

        wait(NULL);

        // Clean up
        munmap(data, sizeof(struct shared_data));
        close(fd);
        shm_unlink("/my_process_shm");
    }

    return 0;
}