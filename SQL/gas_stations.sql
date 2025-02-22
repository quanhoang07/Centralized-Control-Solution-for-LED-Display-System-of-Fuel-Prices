create database gas_stations
use gas_stations

create table Stations (
	id INT AUTO_INCREMENT PRIMARY KEY,
    name varchar(45) NOT NULL,
    location varchar(255) NOT NULL,
    status varchar(45) NOT NULL
);

create table GasPrice (
	id INT AUTO_INCREMENT PRIMARY KEY,
    station_id INT NOT NULL,
    gas_type varchar(50) NOT NULL,
    price DECIMAL(10, 3) NOT NULL,
    updated_at DATETIME,
    FOREIGN KEY (station_id) REFERENCES Stations(id)
);
    
create table LEDDisplays (
	id INT AUTO_INCREMENT PRIMARY KEY,
    station_id INT NOT NULL,
    display_status varchar(45) NOT NULL,
    last_updated DATETIME,
    FOREIGN KEY (station_id) REFERENCES Stations(id)
);

create table ErrorLogs (
	id INT AUTO_INCREMENT PRIMARY KEY,
    station_id INT NOT NULL,
    error_type varchar(45),
    description varchar(255),
    reported_at DATETIME,
    FOREIGN KEY (station_id) REFERENCES Stations(id)
);

CREATE TABLE Users (
    id INT PRIMARY KEY AUTO_INCREMENT,         -- Mã định danh duy nhất cho mỗi người dùng
    username VARCHAR(50) UNIQUE NOT NULL,     -- Tên đăng nhập, có độ dài tối đa 50 ký tự, không được trùng lặp
    password_hash VARCHAR(255) NOT NULL,      -- Mật khẩu đã được băm để đảm bảo an toàn, độ dài 255 ký tự để lưu trữ giá trị băm
    email VARCHAR(100) UNIQUE,                -- Địa chỉ email, tùy chọn và phải là duy nhất nếu có
    role ENUM('admin', 'staff', 'user') DEFAULT 'user',  -- Quyền hạn của người dùng, với giá trị mặc định là 'user'
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,  -- Thời gian tạo tài khoản, mặc định là thời điểm hiện tại
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP  -- Thời gian cập nhật lần cuối, tự động cập nhật khi có thay đổi
);




ALTER TABLE ErrorLogs
    DROP COLUMN error_type,  -- Xóa cột error_type
    ADD COLUMN led_id INT NOT NULL;  -- Thêm cột led_id
