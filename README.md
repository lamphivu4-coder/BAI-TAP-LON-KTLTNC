# Đồ án Lập trình Hướng đối tượng C++ (Kỹ thuật lập trình nâng cao)
**Đề tài:** Quản lý dịch vụ thương mại hàng không  
**Trường:** Đại học Giao thông Vận tải - Phân hiệu tại TP.HCM (UTC2)

##  Danh sách thành viên nhóm và Phân công nhiệm vụ

| STT | Họ và tên | Phần code đảm nhiệm | Mô tả chi tiết công việc |
| :---: | :--- | :--- | :--- |
| **1** | **Mai Đoan Khánh Thạch** | **Class 1, 2, 3, 4**<br>*(Xử lý Dữ liệu & Tiện ích)* | Xây dựng các hàm kiểm tra tính hợp lệ của dữ liệu đầu vào. Lập trình thuật toán phân tích chuỗi CCCD để trích xuất thông tin. Xây dựng các lớp quản lý dữ liệu Sân bay và Hãng hàng không đọc từ file. |
| **2** | **Bùi Quang Thoại** | **Class 5, 6, 7, 8**<br>*(Kế thừa & Đối tượng cơ sở)* | Thiết kế cấu trúc Kế thừa và Đa hình cho các Hạng vé máy bay. Khai báo các hàm ảo xử lý hệ số giá và định mức hành lý. Xây dựng lớp cơ sở Nguoi và kế thừa lên lớp HanhKhach. |
| **3** | **Lương Ngọc Tuấn** | **Class 9, 10, 11, nửa đầu 12**<br>*(Nghiệp vụ & Cấu trúc dữ liệu)* | Viết thuật toán tính giá vé máy bay và giá cước ký gửi hàng hóa. Thiết kế đối tượng trung tâm VeMayBay tích hợp nạp chồng toán tử. Khởi tạo Template Class QuanLyDanhSach để quản lý mảng tổng quát. |
| **4** | **Lâm Phi Vũ** | **Nửa sau Class 12, MainForm, main() & Dữ liệu**<br>*(Giao diện, Xử lý Luồng & Dữ liệu)* | Tìm kiếm thông tin thực tế và chuẩn bị các file dữ liệu đầu vào định dạng txt. Đóng góp ý tưởng và logic thuật toán tạo mã đặt chỗ PNR. Áp dụng thuật toán chuẩn STL xử lý mảng. Lập trình toàn bộ giao diện GUI bằng Windows Forms và xử lý sự kiện tương tác. Lập trình xuất báo cáo csv và kiểm soát ngoại lệ. |

##  Các tính năng chính của chương trình
- Áp dụng đầy đủ các tính chất của Lập trình Hướng đối tượng (OOP): Đóng gói, Kế thừa, Đa hình.
- Ứng dụng Template Class và STL Algorithms trong việc quản lý và thao tác với danh sách đối tượng.
- Tìm kiếm, đặt vé máy bay (Một chiều / Khứ hồi) tự động tính toán giá vé theo hạng ghế và hành lý.
- Quản lý ký gửi hàng hóa dựa trên khoảng cách địa lý giữa các sân bay.
- Giao diện trực quan được xây dựng bằng Windows Forms (C++/CLI).
- Lưu trữ và thống kê dữ liệu doanh thu, xuất báo cáo ra file `.csv`.
- Đọc dữ liệu đầu vào từ các file `.txt`.

##  Hướng dẫn cài đặt và chạy chương trình
1. Clone repository này về máy tính cá nhân.
2. Đảm bảo các file dữ liệu `.txt` (`tinhthanh.txt`, `sanbay.txt`, `hangbay.txt`) và hình ảnh logo nằm đúng thư mục theo cấu hình mã nguồn.
3. Mở project bằng Visual Studio (đã cài đặt hỗ trợ C++/CLI và Windows Forms).
4. Biên dịch (Build) và chạy chương trình từ file `main`.
