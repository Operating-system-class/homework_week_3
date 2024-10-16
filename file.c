#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>

// Định nghĩa cấu trúc shared_data
struct shared_data {
    int x, y, z;
    int ready;
};

int main() {
    // Tạo vùng nhớ chia sẻ cho cấu trúc shared_data
    struct shared_data *data = mmap(NULL, sizeof(struct shared_data), 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Khởi tạo giá trị ban đầu cho vùng nhớ chia sẻ
    data->ready = 0;

    // Tạo tiến trình con
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Code của tiến trình con
        while (data->ready == 0) {
            // Chờ đến khi ready = 1
            usleep(100); // Tránh busy waiting
        }

        // Tính toán z = x + y
        data->z = data->x + data->y;

        // Gán ready = 0 để báo cho mẹ biết đã tính xong
        data->ready = 0;

        // Kết thúc tiến trình con
        exit(0);
    } else {
        // Code của tiến trình mẹ
        // Nhập giá trị x và y từ bàn phím
        printf("Nhập x: ");
        scanf("%d", &data->x);
        printf("Nhập y: ");
        scanf("%d", &data->y);

        // Gán ready = 1 để báo cho con biết có thể tính toán
        data->ready = 1;

        // Chờ đến khi con hoàn thành tính toán (ready == 0)
        while (data->ready == 1) {
            usleep(100); // Tránh busy waiting
        }

        // In kết quả z
        printf("Kết quả z = %d\n", data->z);

        // Đợi con kết thúc
        wait(NULL);

        // Giải phóng vùng nhớ chia sẻ
        munmap(data, sizeof(struct shared_data));

        // Kết thúc tiến trình mẹ
        return 0;
    }
}



