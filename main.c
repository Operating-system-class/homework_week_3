#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

// Struct chứa dữ liệu dùng chung giữa các process
struct shared_data {
    int x, y, z;
    int ready;
};

int main() {
    // Tạo khóa để sử dụng cho bộ nhớ dùng chung
    key_t key = ftok("shmfile", 65);
    
    // Tạo vùng nhớ dùng chung với kích thước của struct shared_data
    int shmid = shmget(key, sizeof(struct shared_data), 0666|IPC_CREAT);
    
    // Gắn vùng nhớ dùng chung vào không gian địa chỉ của process
    struct shared_data *data = (struct shared_data*) shmat(shmid, (void*)0, 0);
    
    // Khởi tạo giá trị ban đầu cho struct
    data->ready = 0;

    // Tạo process con
    pid_t pid = fork();
    
    if (pid < 0) {
        // Lỗi khi tạo process con
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Đây là process con
        while (data->ready == 0) {
            // Chờ đến khi mẹ gán ready = 1
            usleep(1000); // Dừng một chút để tránh busy-wait
        }

        // Khi ready = 1, tính z = x + y
        data->z = data->x + data->y;

        // Gán ready = 0 để báo cho mẹ biết đã tính xong
        data->ready = 0;

        // Tách vùng nhớ dùng chung
        shmdt(data);

        exit(0); // Kết thúc process con
    } else {
        // Đây là process mẹ
        // Nhập giá trị x và y từ bàn phím
        printf("Nhap gia tri x: ");
        scanf("%d", &data->x);
        printf("Nhap gia tri y: ");
        scanf("%d", &data->y);

        // Gán ready = 1 để báo cho process con biết đã có dữ liệu
        data->ready = 1;

        // Chờ đến khi process con gán ready = 0
        while (data->ready == 1) {
            usleep(1000); // Dừng một chút để tránh busy-wait
        }

        // In giá trị z
        printf("Gia tri z = x + y: %d\n", data->z);

        // Chờ process con kết thúc
        wait(NULL);

        // Tách và xóa vùng nhớ dùng chung
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);

        return 0; // Kết thúc process mẹ
    }
}
