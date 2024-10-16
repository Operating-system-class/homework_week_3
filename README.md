Viết chương trình C với yêu cầu sau:
- Process mẹ tạo vùng nhớ dùng chung cho một struct{int x, y, z; int ready;}, khởi tạo ready = 0, rồi tạo process con.
- Process mẹ đọc giá trị x, y từ bàn phím xong và gán ready = 1, sau đó chờ đến khi ready == 0 thì in giá trị z và kết thúc.
- Process con chờ đến khi ready == 1 thì tính z = x + y, rồi gán ready = 0 và kết thúc.
