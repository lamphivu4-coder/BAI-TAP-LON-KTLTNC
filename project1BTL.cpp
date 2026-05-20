#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.dll>

#pragma warning(disable: 4996)

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm> 
#include <stdexcept> 
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

// ========================== CÁC HÀM TIỆN ÍCH ==========================
String^ StrToSys(const std::string& str) {
    array<Byte>^ bytes = gcnew array<Byte>(str.length());
    for (size_t i = 0; i < str.length(); ++i) {
        bytes[i] = (Byte)str[i];
    }
    return System::Text::Encoding::UTF8->GetString(bytes);
}

std::string SysToStr(String^ str) {
    array<Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(str);
    std::string result = "";
    for (int i = 0; i < bytes->Length; ++i) {
        result += (char)bytes[i];
    }
    return result;
}

double tuyetDoi(double x) {
    return x < 0 ? -x : x;
}

double tinhCanBacHai(double n) {
    if (n <= 0) return 0;
    double x = n;
    double y = 1;
    double e = 0.000001;
    while (x - y > e) {
        x = (x + y) / 2;
        y = n / x;
    }
    return x;
}
// Class 1
class KiemTra {
public:
    static bool hopLeCCCD(const std::string& cccd) {
        if (cccd.length() != 12) return false;
        for (size_t i = 0; i < cccd.length(); i++) {
            if (cccd[i] < '0' || cccd[i] > '9') return false;
        }
        return true;
    }
    static bool hopLeSDT(const std::string& phone) {
        if (phone.length() < 10) return false;
        if (phone[0] != '+' && phone[0] != '0') return false;
        for (size_t i = 1; i < phone.length(); i++) {
            if (phone[i] < '0' || phone[i] > '9') return false;
        }
        return true;
    }
};

struct TinhThanh {
    std::string ma;
    std::string ten;
};

struct ThongTinCCCD {
    std::string soCCCD;
    std::string maTinh;
    std::string tenTinh;
    std::string gioiTinh;
    int namSinh;
    int tuoi;
    bool hopLe;
};
//Class 2
class PhanTichCCCD {
private:
    static std::vector<TinhThanh> danhSachTinh;
    static void khoiTaoDuLieu() {
        if (!danhSachTinh.empty()) return;
        std::ifstream file("tinhthanh.txt");
        if (!file.is_open()) {
            danhSachTinh.push_back({ "079", "TP.Ho Chi Minh" });
            danhSachTinh.push_back({ "001", "Ha Noi" });
            danhSachTinh.push_back({ "048", "Da Nang" });
            return;
        }
        std::string dong;
        while (std::getline(file, dong)) {
            if (!dong.empty() && dong.back() == '\r') dong.pop_back();
            size_t pos = dong.find(',');
            if (pos != std::string::npos) {
                std::string ma = dong.substr(0, pos);
                std::string ten = dong.substr(pos + 1);
                danhSachTinh.push_back({ ma, ten });
            }
        }
        file.close();
    }
public:
    static ThongTinCCCD phanTich(const std::string& cccd) {
        khoiTaoDuLieu();
        ThongTinCCCD tt;
        tt.soCCCD = cccd;
        tt.hopLe = false;
        if (!KiemTra::hopLeCCCD(cccd)) return tt;

        tt.maTinh = cccd.substr(0, 3);
        tt.tenTinh = "Khong xac dinh";
        for (size_t i = 0; i < danhSachTinh.size(); i++) {
            if (danhSachTinh[i].ma == tt.maTinh) {
                tt.tenTinh = danhSachTinh[i].ten;
                break;
            }
        }

        int maGT = cccd[3] - '0';
        int maNam;
        std::stringstream ss(cccd.substr(4, 2));
        ss >> maNam;

        int theKy;
        if (maGT <= 1) { theKy = 1900; tt.gioiTinh = (maGT == 0) ? "Nam" : "Nu"; }
        else if (maGT <= 3) { theKy = 2000; tt.gioiTinh = (maGT == 2) ? "Nam" : "Nu"; }
        else if (maGT <= 5) { theKy = 2100; tt.gioiTinh = (maGT == 4) ? "Nam" : "Nu"; }
        else if (maGT <= 7) { theKy = 2200; tt.gioiTinh = (maGT == 6) ? "Nam" : "Nu"; }
        else { theKy = 2300; tt.gioiTinh = (maGT == 8) ? "Nam" : "Nu"; }

        tt.namSinh = theKy + maNam;
        time_t t = time(0);
        struct tm* now = localtime(&t);
        tt.tuoi = (now->tm_year + 1900) - tt.namSinh;
        tt.hopLe = true;
        return tt;
    }
};

std::vector<TinhThanh> PhanTichCCCD::danhSachTinh;

struct SanBay {
    std::string iata;
    std::string ten;
    std::string thanhPho;
    std::string quocGia;
    int khuVuc;
    double viDo;
    double kinhDo;
};
//Class 3 vu
class DuLieuSanBay {
private:
    std::vector<SanBay> danhSach;
    static DuLieuSanBay* theHien;
    DuLieuSanBay() {
        std::ifstream file("sanbay.txt");
        if (!file.is_open()) {
            danhSach.push_back({ "HAN", "Noi Bai", "Ha Noi", "Viet Nam", 1, 21.2212, 105.8072 });
            danhSach.push_back({ "SGN", "Tan Son Nhat", "TP.HCM", "Viet Nam", 1, 10.8189, 106.6519 });
            danhSach.push_back({ "DAD", "Da Nang", "Da Nang", "Viet Nam", 1, 16.0439, 108.1993 });
            danhSach.push_back({ "JFK", "John F. Kennedy", "New York", "My", 2, 40.6413, -73.7781 });
            danhSach.push_back({ "LHR", "Heathrow", "London", "Anh", 2, 51.4700, -0.4543 });
            danhSach.push_back({ "CDG", "Charles de Gaulle", "Paris", "Phap", 2, 49.0097, 2.5479 });
            danhSach.push_back({ "NRT", "Narita", "Tokyo", "Nhat Ban", 2, 35.7647, 140.3863 });
            danhSach.push_back({ "DXB", "Dubai Intl", "Dubai", "UAE", 2, 25.2532, 55.3657 });
            danhSach.push_back({ "DOH", "Hamad Intl", "Doha", "Qatar", 2, 25.2730, 51.6080 });
            danhSach.push_back({ "SIN", "Changi", "Singapore", "Singapore", 2, 1.3644, 103.9915 });
            return;
        }
        std::string dong;
        while (std::getline(file, dong)) {
            if (!dong.empty() && dong.back() == '\r') dong.pop_back();
            std::stringstream ss(dong);
            std::string ma, ten, tp, qg, kv, vi, kinh;
            std::getline(ss, ma, ',');
            std::getline(ss, ten, ',');
            std::getline(ss, tp, ',');
            std::getline(ss, qg, ',');
            std::getline(ss, kv, ',');
            std::getline(ss, vi, ',');
            std::getline(ss, kinh, ',');
            try {
                if (!kinh.empty()) {
                    danhSach.push_back({ ma, ten, tp, qg, std::stoi(kv), std::stod(vi), std::stod(kinh) });
                }
            }
            catch (...) {}
        }
        file.close();
    }
public:
    static DuLieuSanBay* layTheHien() {
        if (!theHien) theHien = new DuLieuSanBay();
        return theHien;
    }
    std::vector<SanBay> layDanhSach() { return danhSach; }
    SanBay timKiem(const std::string& iata) {
        for (size_t i = 0; i < danhSach.size(); i++) {
            if (danhSach[i].iata == iata) return danhSach[i];
        }
        if (!danhSach.empty()) return danhSach[0];
        return { "", "", "", "", 0, 0.0, 0.0 };
    }
    double tinhKhoangCach(const SanBay& sb1, const SanBay& sb2) {
        double dLat = tuyetDoi(sb1.viDo - sb2.viDo) * 111.0;
        double dLon = tuyetDoi(sb1.kinhDo - sb2.kinhDo) * 111.0;
        return tinhCanBacHai(dLat * dLat + dLon * dLon);
    }
};
DuLieuSanBay* DuLieuSanBay::theHien = nullptr;
struct HangHangKhong {
    std::string maHang;
    std::string tenHang;
    int soSao;
};
struct ChuyenBay {
    std::string maCB;
    HangHangKhong hangBay;
    SanBay sbDi;
    SanBay sbDen;
    std::string dongMayBay;
    std::string thoiGianDi;
    std::string thoiGianDen;
    double giaNiemYet;
};

//Class 4
//Class 4
class DuLieuHangKhong {
private:
    std::vector<HangHangKhong> danhSach;
    static DuLieuHangKhong* theHien;
    DuLieuHangKhong() {
        std::ifstream file("hangbay.txt");
        if (!file.is_open()) {
            // Dữ liệu dự phòng nếu không có file hangbay.txt
            danhSach.push_back({ "VN", "Vietnam Airlines", 4 });
            danhSach.push_back({ "VJ", "Vietjet Air", 3 });
            danhSach.push_back({ "QH", "Bamboo Airways", 4 });
            danhSach.push_back({ "EK", "Emirates", 5 });
            danhSach.push_back({ "QR", "Qatar Airways", 5 });
            danhSach.push_back({ "SQ", "Singapore Airlines", 5 });
            return;
        }

        std::string dong;
        while (std::getline(file, dong)) {
            if (!dong.empty() && dong.back() == '\r') dong.pop_back();
            std::stringstream ss(dong);
            std::string ma, ten, sao;

            std::getline(ss, ma, ',');
            std::getline(ss, ten, ',');
            std::getline(ss, sao, ',');

            try {
                if (!sao.empty()) {
                    danhSach.push_back({ ma, ten, std::stoi(sao) });
                }
            }
            catch (...) {} // Bỏ qua dòng lỗi nếu có
        }
        file.close();
    }
public:
    static DuLieuHangKhong* layTheHien() {
        if (!theHien) theHien = new DuLieuHangKhong();
        return theHien;
    }
    std::vector<HangHangKhong> layDanhSach() { return danhSach; }
    HangHangKhong timKiem(const std::string& ten) {
        for (size_t i = 0; i < danhSach.size(); i++) {
            if (danhSach[i].tenHang == ten) return danhSach[i];
        }
        return danhSach[0];
    }
};
DuLieuHangKhong* DuLieuHangKhong::theHien = nullptr;

class HangVe {
public:
    virtual std::string layTenHang() const = 0;
    virtual std::string layMaHang() const = 0;
    virtual double layHeSoGia() const = 0;
    virtual int layHanhLyKyGui() const = 0;
    virtual int layHanhLyXachTay() const = 0;
    virtual HangVe* taoBanSao() const = 0;
    virtual ~HangVe() {}
};
//Class 5
class PhoThong : public HangVe {
public:
    std::string layTenHang() const override { return "Economy"; }
    std::string layMaHang() const override { return "Y"; }
    double layHeSoGia() const override { return 1.0; }
    int layHanhLyKyGui() const override { return 23; }
    int layHanhLyXachTay() const override { return 7; }
    HangVe* taoBanSao() const override { return new PhoThong(*this); }
};
//Class 6
class ThuongGia : public HangVe {
public:
    std::string layTenHang() const override { return "Business"; }
    std::string layMaHang() const override { return "C"; }
    double layHeSoGia() const override { return 4.0; }
    int layHanhLyKyGui() const override { return 40; }
    int layHanhLyXachTay() const override { return 14; }
    HangVe* taoBanSao() const override { return new ThuongGia(*this); }
};

class HangNhat : public HangVe {
public:
    std::string layTenHang() const override { return "First Class"; }
    std::string layMaHang() const override { return "F"; }
    double layHeSoGia() const override { return 10.0; }
    int layHanhLyKyGui() const override { return 50; }
    int layHanhLyXachTay() const override { return 20; }
    HangVe* taoBanSao() const override { return new HangNhat(*this); }
}; 
//Class 7 thoai
class Nguoi {
protected:
    std::string hoTen;
    ThongTinCCCD thongTinCccd;
    std::string sdt;
public:
    Nguoi() {}
    void thietLap(std::string ten, ThongTinCCCD tt, std::string phone) {
        hoTen = ten;
        thongTinCccd = tt;
        sdt = phone;
    }
    std::string layHoTen() const { return hoTen; }
    ThongTinCCCD layThongTinCCCD() const { return thongTinCccd; }
    virtual ~Nguoi() {}
};
// Class 8
class HanhKhach : public Nguoi {
private:
    std::string quocTich;
public:
    HanhKhach() {}
    void thietLapHK(std::string ten, ThongTinCCCD tt, std::string phone, std::string qt) {
        thietLap(ten, tt, phone);
        quocTich = qt;
    }
};
//Class 9
class TinhGiaVe {
public:
    static double tinhToan(double giaCoBan, HangVe* hv, double kyGui, double xachTay, double& tienPhat) {
        double tong = giaCoBan * hv->layHeSoGia();
        tienPhat = 0;
        if (kyGui > hv->layHanhLyKyGui()) {
            tienPhat += (kyGui - hv->layHanhLyKyGui()) * 200000;
        }
        if (xachTay > hv->layHanhLyXachTay()) {
            tienPhat += (xachTay - hv->layHanhLyXachTay()) * 300000;
        }
        return (tong * 1.2) + tienPhat;
    }
};
//Class 10
class VeMayBay {
private:
    std::string pnr;
    std::string soHieuCB;
    HanhKhach hanhKhach;
    SanBay sbDi;
    SanBay sbDen;
    HangHangKhong hangBay;
    HangVe* hangVe;
    std::string ngayGioBay;

    // Thuộc tính mới cho vé Khứ Hồi
    bool laHaiChieu;
    std::string ngayVe;

    double hanhLyKG;
    double hanhLyXT;
    double tienPhatHanhLy;
    double tongTien;
    std::string ngayDat;
    std::string trangThai;
public:
    VeMayBay() : hangVe(nullptr), laHaiChieu(false) {}
    VeMayBay(std::string ma, HanhKhach hk, ChuyenBay cb, HangVe* hv, double kg, double xt, bool khamHoi, std::string ngayV) {
        pnr = ma;
        soHieuCB = cb.maCB;
        hanhKhach = hk;
        sbDi = cb.sbDi;
        sbDen = cb.sbDen;
        hangBay = cb.hangBay;
        hangVe = hv->taoBanSao();
        hanhLyKG = kg;
        hanhLyXT = xt;
        ngayGioBay = cb.thoiGianDi;

        laHaiChieu = khamHoi;
        ngayVe = ngayV;

        trangThai = "Da Dat";
        tongTien = TinhGiaVe::tinhToan(cb.giaNiemYet, hv, kg, xt, tienPhatHanhLy);

        if (laHaiChieu) {
            tongTien *= 2; // Nhân đôi tổng tiền cho 2 chiều
            tienPhatHanhLy *= 2;
        }

        time_t now = time(0);
        char buf[80];
        strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", localtime(&now));
        ngayDat = std::string(buf);
    }
    VeMayBay(const VeMayBay& khac) {
        pnr = khac.pnr;
        soHieuCB = khac.soHieuCB;
        hanhKhach = khac.hanhKhach;
        sbDi = khac.sbDi;
        sbDen = khac.sbDen;
        hangBay = khac.hangBay;
        hanhLyKG = khac.hanhLyKG;
        hanhLyXT = khac.hanhLyXT;
        ngayGioBay = khac.ngayGioBay;
        laHaiChieu = khac.laHaiChieu;
        ngayVe = khac.ngayVe;
        tienPhatHanhLy = khac.tienPhatHanhLy;
        tongTien = khac.tongTien;
        ngayDat = khac.ngayDat;
        trangThai = khac.trangThai;
        hangVe = khac.hangVe ? khac.hangVe->taoBanSao() : nullptr;
    }
    VeMayBay& operator=(const VeMayBay& khac) {
        if (this != &khac) {
            delete hangVe;
            pnr = khac.pnr;
            soHieuCB = khac.soHieuCB;
            hanhKhach = khac.hanhKhach;
            sbDi = khac.sbDi;
            sbDen = khac.sbDen;
            hangBay = khac.hangBay;
            hanhLyKG = khac.hanhLyKG;
            hanhLyXT = khac.hanhLyXT;
            ngayGioBay = khac.ngayGioBay;
            laHaiChieu = khac.laHaiChieu;
            ngayVe = khac.ngayVe;
            tienPhatHanhLy = khac.tienPhatHanhLy;
            tongTien = khac.tongTien;
            ngayDat = khac.ngayDat;
            trangThai = khac.trangThai;
            hangVe = khac.hangVe ? khac.hangVe->taoBanSao() : nullptr;
        }
        return *this;
    }
    ~VeMayBay() { delete hangVe; }

    void huyVe() { trangThai = "Da Huy"; }
    std::string layTrangThai() const { return trangThai; }
    std::string layPNR() const { return pnr; }
    std::string laySoHieu() const { return soHieuCB; }
    std::string layTenKhach() const { return hanhKhach.layHoTen(); }
    ThongTinCCCD layThongTinCCCD() const { return hanhKhach.layThongTinCCCD(); }
    std::string layHanhTrinh() const { return sbDi.iata + " - " + sbDen.iata; }
    std::string layNgayGioBay() const { return ngayGioBay; }
    bool layLaHaiChieu() const { return laHaiChieu; }
    std::string layNgayVe() const { return ngayVe; }
    std::string layHangBay() const { return hangBay.tenHang; }
    std::string layTenHangVe() const { return hangVe ? hangVe->layTenHang() : ""; }
    double layTienPhat() const { return tienPhatHanhLy; }
    double layTongTien() const { return tongTien; }
    std::string layNgayDat() const { return ngayDat; }
    std::string layChuoiHanhLy() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << hanhLyKG << "kg KG / " << hanhLyXT << "kg XT";
        return ss.str();
    }
};
//Class 11
class TinhGiaHangHoa {
public:
    static double tinhToan(double khoangCach, double khoiLuong, int loaiKhach, int loaiHang) {
        double donGiaCoBan = 25.0;
        double heSoKhach = 1.0;
        if (loaiKhach == 0) heSoKhach = 1.2;
        else if (loaiKhach == 2) heSoKhach = 0.85;

        double heSoHang = 1.0;
        if (loaiHang == 1) heSoHang = 1.5;
        else if (loaiHang == 2) heSoHang = 1.3;

        double tongTien = khoangCach * khoiLuong * donGiaCoBan * heSoKhach * heSoHang;
        if (tongTien < 150000) tongTien = 150000;
        return tongTien;
    }
};

struct PhieuHangHoa {
    std::string maPhieu;
    std::string tenNguoiGui;
    SanBay sbDi;
    SanBay sbDen;
    std::string loaiKhach;
    std::string loaiHang;
    double khoiLuong;
    double khoangCach;
    double tongTien;
    std::string ngayTao;
};

// ========================== CÁC LỚP BỔ SUNG ĐỂ ĐÁP ỨNG YÊU CẦU ========================== A tuan

// 1. Template Class kết hợp STL Algorithms (Thay thế std::vector nguyên thủy)
template <typename T>
//Class 12
class QuanLyDanhSach {
private:
    std::vector<T> danhSach;
public:
    void them(const T& item) {
        danhSach.push_back(item);
    }

    size_t kichThuoc() const {
        return danhSach.size();
    }

    T& operator[](size_t index) {
        if (index >= danhSach.size()) throw std::out_of_range("Chi so vuot qua gioi han");
        return danhSach[index];
    }

    const T& operator[](size_t index) const {
        if (index >= danhSach.size()) throw std::out_of_range("Chi so vuot qua gioi han");
        return danhSach[index];
    }

    void xoaTatCa() {
        danhSach.clear();
    }

    // Áp dụng STL Algorithm: std::remove_if
    template<typename Predicate>
    void xoaTheoDieuKien(Predicate dk) {
        danhSach.erase(std::remove_if(danhSach.begin(), danhSach.end(), dk), danhSach.end());
    }
};

// Sử dụng Template Class vừa tạo
QuanLyDanhSach<VeMayBay> heThongVe;
QuanLyDanhSach<PhieuHangHoa> heThongHangHoa;

// 2. Hàm kiểm tra độc lập (Khắc phục lỗi E2093 của C++/CLI)
bool KiemTraVeDaHuy(const VeMayBay& v) {
    return v.layTrangThai() == "Da Huy";
}

// ============================================================================================== các hàm kiểm tra tạo cột tạo bảng

public ref class MainForm : public Form {
private:
    TabControl^ tabCtrl;
    TabPage^ tabDatVe;
    TabPage^ tabDanhSach;
    TabPage^ tabHangHoa;
    TabPage^ tabThongKe;

    ComboBox^ cbSanBayDi;
    ComboBox^ cbSanBayDen;

    // Thêm các biến cho chức năng 1 chiều / Khứ hồi
    RadioButton^ rbMotChieu;
    RadioButton^ rbHaiChieu;
    DateTimePicker^ dtpNgayBay;
    Label^ lblNgayVe;
    DateTimePicker^ dtpNgayVe;

    Button^ btnTimChuyenBay;
    DataGridView^ gridChuyenBay;
    TextBox^ txtHoTen;
    TextBox^ txtCCCD;
    Label^ lblThongTinCCCD;
    TextBox^ txtSDT;
    TextBox^ txtQuocTich;
    ComboBox^ cbHangVe;
    TextBox^ txtKyGui;
    TextBox^ txtXachTay;
    Label^ lblHMDinhMuc;
    Button^ btnDatVe;

    // Khai báo biến hiển thị Logo
    PictureBox^ picLogo;

    DataGridView^ gridVe;
    TextBox^ txtTimKiem;
    Button^ btnTimKiem;
    Button^ btnHuyVe;
    Button^ btnXoaVeHuy; // Nút ứng dụng STL Algorithm

    TextBox^ txtNguoiGuiHH;
    ComboBox^ cbSBDiHH;
    ComboBox^ cbSBDenHH;
    ComboBox^ cbLoaiKhachHH;
    ComboBox^ cbLoaiHangHH;
    TextBox^ txtKhoiLuongHH;
    Button^ btnTaoPhieuHH;
    DataGridView^ gridHangHoa;

    DataGridView^ gridThongKe;
    Label^ lblTongDoanhThu;
    Label^ lblDoanhThuHangHoa;
    Button^ btnLuuDuLieu; // Nút ứng dụng File I/O & Exception Handling

    std::vector<ChuyenBay>* dsChuyenBayHienTai;

    Label^ TaoLabel(String^ text, int x, int y, bool isBold) {
        Label^ lbl = gcnew Label();
        lbl->Text = text;
        lbl->Location = Point(x, y);
        lbl->AutoSize = true;
        if (isBold) lbl->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
        return lbl;
    }

    TextBox^ TaoTextBox(int x, int y, int w) {
        TextBox^ txt = gcnew TextBox();
        txt->Location = Point(x, y);
        txt->Width = w;
        return txt;
    }

    ComboBox^ TaoComboBox(int x, int y, int w) {
        ComboBox^ cb = gcnew ComboBox();
        cb->Location = Point(x, y);
        cb->Width = w;
        cb->DropDownStyle = ComboBoxStyle::DropDownList;
        return cb;
    }

    void KhoiTaoUI() {
        this->Text = L"Hệ Thống Quản Lý Thương Mại Hàng Không Pro";
        this->Size = Drawing::Size(1350, 850);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->Font = gcnew Drawing::Font("Segoe UI", 10);
        dsChuyenBayHienTai = new std::vector<ChuyenBay>();

        tabCtrl = gcnew TabControl();
        tabCtrl->Dock = DockStyle::Fill;
        tabDatVe = gcnew TabPage(L"Đặt Vé Mới");
        tabDanhSach = gcnew TabPage(L"Lịch Sử & Quản Lý Vé");
        tabHangHoa = gcnew TabPage(L"Ký Gửi Hàng Hóa");
        tabThongKe = gcnew TabPage(L"Báo Cáo Thống Kê");

        // ======================= TAB ĐẶT VÉ =======================
        int topY = 20, stepY = 35;

        // --- THÊM LOGO ---
       // --- THÊM LOGO ---
        picLogo = gcnew PictureBox();
        // Dời tọa độ X sang 750 và Y xuống 380 (ngang hàng với ô nhập Họ Tên)
        picLogo->Location = Point(700, 336);
        // Phóng to kích thước logo ra một chút để lấp đầy khoảng trắng cho đẹp
        picLogo->Size = Drawing::Size(500, 430);   // THẠCH
        picLogo->SizeMode = PictureBoxSizeMode::Zoom;
        try {
            picLogo->Image = Image::FromFile("D:\\logo.png");
        }
        catch (...) {
            picLogo->BackColor = Color::LightGray;
        }
        tabDatVe->Controls->Add(picLogo);


        tabDatVe->Controls->Add(TaoLabel(L"1. TÌM KIẾM CHUYẾN BAY", 20, topY, true));

        tabDatVe->Controls->Add(TaoLabel(L"Sân Bay Đi:", 40, topY + stepY, false));
        cbSanBayDi = TaoComboBox(150, topY + stepY, 200);

        tabDatVe->Controls->Add(TaoLabel(L"Sân Bay Đến:", 380, topY + stepY, false));
        cbSanBayDen = TaoComboBox(490, topY + stepY, 200);

        // --- TÙY CHỌN 1 CHIỀU / KHỨ HỒI ---
        rbMotChieu = gcnew RadioButton();
        rbMotChieu->Text = L"Một chiều";
        rbMotChieu->Location = Point(720, topY);
        rbMotChieu->Checked = true;
        rbMotChieu->AutoSize = true;

        rbHaiChieu = gcnew RadioButton();
        rbHaiChieu->Text = L"Khứ hồi (2 chiều)";
        rbHaiChieu->Location = Point(840, topY);
        rbHaiChieu->AutoSize = true;
        rbHaiChieu->CheckedChanged += gcnew EventHandler(this, &MainForm::XuLyChonLoaiVe);

        tabDatVe->Controls->Add(TaoLabel(L"Ngày Đi:", 720, topY + stepY, false));
        dtpNgayBay = gcnew DateTimePicker();
        dtpNgayBay->Location = Point(840, topY + stepY);
        dtpNgayBay->Width = 150;
        dtpNgayBay->Format = DateTimePickerFormat::Custom;
        dtpNgayBay->CustomFormat = L"dd/MM/yyyy";

        lblNgayVe = TaoLabel(L"Ngày Về:", 720, topY + stepY * 2, false);
        lblNgayVe->Visible = false; // Mặc định ẩn

        dtpNgayVe = gcnew DateTimePicker();
        dtpNgayVe->Location = Point(840, topY + stepY * 2);
        dtpNgayVe->Width = 150;
        dtpNgayVe->Format = DateTimePickerFormat::Custom;
        dtpNgayVe->CustomFormat = L"dd/MM/yyyy";
        dtpNgayVe->Visible = false; // Mặc định ẩn

        btnTimChuyenBay = gcnew Button();
        btnTimChuyenBay->Text = L"TÌM CHUYẾN BAY";
        btnTimChuyenBay->Location = Point(1020, topY + stepY - 2);
        btnTimChuyenBay->Size = Drawing::Size(150, 30);
        btnTimChuyenBay->BackColor = Color::DarkOrange;
        btnTimChuyenBay->ForeColor = Color::White;
        btnTimChuyenBay->FlatStyle = FlatStyle::Flat;
        btnTimChuyenBay->Click += gcnew EventHandler(this, &MainForm::XuLyTimChuyenBay);

        gridChuyenBay = gcnew DataGridView();
        gridChuyenBay->Location = Point(40, topY + stepY * 3 + 10);
        gridChuyenBay->Size = Drawing::Size(1130, 180);
        gridChuyenBay->AllowUserToAddRows = false;
        gridChuyenBay->ReadOnly = true;
        gridChuyenBay->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
        gridChuyenBay->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
        gridChuyenBay->ColumnCount = 6;
        gridChuyenBay->Columns[0]->Name = L"Hãng Khai Thác";
        gridChuyenBay->Columns[1]->Name = L"Số Hiệu";
        gridChuyenBay->Columns[2]->Name = L"Dòng Máy Bay";
        gridChuyenBay->Columns[3]->Name = L"Khởi Hành";
        gridChuyenBay->Columns[4]->Name = L"Hạ Cánh Dự Kiến";
        gridChuyenBay->Columns[5]->Name = L"Giá Cơ Bản (VND)";

        int midY = topY + stepY * 3 + 210;
        tabDatVe->Controls->Add(TaoLabel(L"2. THÔNG TIN HÀNH KHÁCH & ĐẶT VÉ", 20, midY, true));

        int labelX = 40, inputX = 180, pY = midY + 35, pStepY = 40;

        tabDatVe->Controls->Add(TaoLabel(L"Họ và Tên (*):", labelX, pY, false));
        txtHoTen = TaoTextBox(inputX, pY, 250);

        tabDatVe->Controls->Add(TaoLabel(L"Số CCCD (*):", labelX, pY + pStepY, false));
        txtCCCD = TaoTextBox(inputX, pY + pStepY, 250);
        txtCCCD->MaxLength = 12;
        txtCCCD->TextChanged += gcnew EventHandler(this, &MainForm::XuLyTruyXuatCCCD);

        lblThongTinCCCD = TaoLabel(L"", inputX, pY + pStepY + 25, false);
        lblThongTinCCCD->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Italic);

        tabDatVe->Controls->Add(TaoLabel(L"Số Điện Thoại:", labelX, pY + pStepY * 2, false));
        txtSDT = TaoTextBox(inputX, pY + pStepY * 2, 250);

        tabDatVe->Controls->Add(TaoLabel(L"Quốc Tịch:", labelX, pY + pStepY * 3, false));
        txtQuocTich = TaoTextBox(inputX, pY + pStepY * 3, 250);
        txtQuocTich->Text = L"Viet Nam"; 

        tabDatVe->Controls->Add(TaoLabel(L"Hạng Vé:", labelX, pY + pStepY * 4, false));
        cbHangVe = TaoComboBox(inputX, pY + pStepY * 4, 250);
        cbHangVe->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::CapNhatHinhMucHanhLy);

        lblHMDinhMuc = TaoLabel(L"Định mức: 0kg KG / 0kg XT", inputX + 260, pY + pStepY * 4, false);
        lblHMDinhMuc->ForeColor = Color::Blue;

        tabDatVe->Controls->Add(TaoLabel(L"Ký Gửi (Kg):", labelX, pY + pStepY * 5, false));
        txtKyGui = TaoTextBox(inputX, pY + pStepY * 5, 250);
        txtKyGui->Text = L"23";

        tabDatVe->Controls->Add(TaoLabel(L"Xách Tay (Kg):", labelX, pY + pStepY * 6, false));
        txtXachTay = TaoTextBox(inputX, pY + pStepY * 6, 250);
        txtXachTay->Text = L"7";

        btnDatVe = gcnew Button();
        btnDatVe->Text = L"XÁC NHẬN ĐẶT VÉ";
        btnDatVe->Location = Point(inputX, pY + pStepY * 7);
        btnDatVe->Size = Drawing::Size(250, 45);
        btnDatVe->BackColor = Color::SteelBlue;
        btnDatVe->ForeColor = Color::White;
        btnDatVe->FlatStyle = FlatStyle::Flat;
        btnDatVe->Font = gcnew Drawing::Font("Segoe UI", 12, FontStyle::Bold);
        btnDatVe->Click += gcnew EventHandler(this, &MainForm::XuLyDatVe);

        tabDatVe->Controls->Add(rbMotChieu); tabDatVe->Controls->Add(rbHaiChieu);
        tabDatVe->Controls->Add(lblNgayVe); tabDatVe->Controls->Add(dtpNgayVe);
        tabDatVe->Controls->Add(cbSanBayDi); tabDatVe->Controls->Add(cbSanBayDen);
        tabDatVe->Controls->Add(dtpNgayBay); tabDatVe->Controls->Add(btnTimChuyenBay);
        tabDatVe->Controls->Add(gridChuyenBay);
        tabDatVe->Controls->Add(txtHoTen); tabDatVe->Controls->Add(txtCCCD);
        tabDatVe->Controls->Add(lblThongTinCCCD); tabDatVe->Controls->Add(txtSDT);
        tabDatVe->Controls->Add(txtQuocTich); tabDatVe->Controls->Add(cbHangVe);
        tabDatVe->Controls->Add(lblHMDinhMuc); tabDatVe->Controls->Add(txtKyGui);
        tabDatVe->Controls->Add(txtXachTay); tabDatVe->Controls->Add(btnDatVe);
        // VU
        std::vector<SanBay> dsSB = DuLieuSanBay::layTheHien()->layDanhSach();
        for (size_t i = 0; i < dsSB.size(); i++) {
            String^ item = StrToSys(dsSB[i].iata + " - " + dsSB[i].ten + " (" + dsSB[i].thanhPho + ")");
            cbSanBayDi->Items->Add(item);
            cbSanBayDen->Items->Add(item);
        }
        if (cbSanBayDi->Items->Count > 0) cbSanBayDi->SelectedIndex = 0;
        if (cbSanBayDen->Items->Count > 1) cbSanBayDen->SelectedIndex = 1;

        cbHangVe->Items->AddRange(gcnew cli::array<String^>{L"Economy", L"Business", L"First Class"});
        cbHangVe->SelectedIndex = 0;

        // ======================= TAB DANH SÁCH =======================
        txtTimKiem = gcnew TextBox();
        txtTimKiem->Location = Point(20, 20);
        txtTimKiem->Width = 300;

        btnTimKiem = gcnew Button();
        btnTimKiem->Text = L"Tìm PNR / CCCD";
        btnTimKiem->Location = Point(330, 18);
        btnTimKiem->Click += gcnew EventHandler(this, &MainForm::XuLyTimKiem);

        btnHuyVe = gcnew Button();
        btnHuyVe->Text = L"Hủy Vé Đã Chọn";
        btnHuyVe->Location = Point(450, 18);
        btnHuyVe->BackColor = Color::Crimson;
        btnHuyVe->ForeColor = Color::White;
        btnHuyVe->FlatStyle = FlatStyle::Flat;
        btnHuyVe->Click += gcnew EventHandler(this, &MainForm::XuLyHuyVe);

        btnXoaVeHuy = gcnew Button();
        btnXoaVeHuy->Text = L"Dọn Dẹp Vé Đã Hủy";
        btnXoaVeHuy->Location = Point(580, 18);
        btnXoaVeHuy->BackColor = Color::Gray;
        btnXoaVeHuy->ForeColor = Color::White;
        btnXoaVeHuy->FlatStyle = FlatStyle::Flat;
        btnXoaVeHuy->Click += gcnew EventHandler(this, &MainForm::XuLyXoaVeHuy);

        gridVe = gcnew DataGridView();
        gridVe->Location = Point(20, 60);
        gridVe->Size = Drawing::Size(1280, 680);
        gridVe->AllowUserToAddRows = false;
        gridVe->ReadOnly = true;
        gridVe->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
        gridVe->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;

        // Thêm cột để hiển thị thông tin 1 chiều / Khứ hồi
        gridVe->ColumnCount = 14;
        gridVe->Columns[0]->Name = L"PNR";
        gridVe->Columns[1]->Name = L"Số Hiệu";
        gridVe->Columns[2]->Name = L"Họ Tên";
        gridVe->Columns[3]->Name = L"CCCD";
        gridVe->Columns[4]->Name = L"Hành Trình";
        gridVe->Columns[5]->Name = L"Loại Vé";
        gridVe->Columns[6]->Name = L"Thời Gian Bay";
        gridVe->Columns[7]->Name = L"Ngày Về";
        gridVe->Columns[8]->Name = L"Hãng Bay";
        gridVe->Columns[9]->Name = L"Hạng Vé";
        gridVe->Columns[10]->Name = L"Hành Lý";
        gridVe->Columns[11]->Name = L"Tổng Tiền";
        gridVe->Columns[12]->Name = L"Trạng Thái";
        gridVe->Columns[13]->Name = L"Ngày Đặt";

        tabDanhSach->Controls->Add(txtTimKiem);
        tabDanhSach->Controls->Add(btnTimKiem);
        tabDanhSach->Controls->Add(btnHuyVe);
        tabDanhSach->Controls->Add(btnXoaVeHuy);
        tabDanhSach->Controls->Add(gridVe);
        // thoai
        // ======================= TAB KÝ GỬI HÀNG HÓA =======================
        tabHangHoa->Controls->Add(TaoLabel(L"THIẾT LẬP PHIẾU KÝ GỬI HÀNG HÓA", 20, 20, true));

        tabHangHoa->Controls->Add(TaoLabel(L"Người/Đơn vị gửi:", 20, 70, false));
        txtNguoiGuiHH = TaoTextBox(160, 70, 250);

        tabHangHoa->Controls->Add(TaoLabel(L"Sân bay đi:", 20, 110, false));
        cbSBDiHH = TaoComboBox(160, 110, 250);

        tabHangHoa->Controls->Add(TaoLabel(L"Sân bay đến:", 20, 150, false));
        cbSBDenHH = TaoComboBox(160, 150, 250);

        for (size_t i = 0; i < dsSB.size(); i++) {
            String^ item = StrToSys(dsSB[i].iata + " - " + dsSB[i].ten);
            cbSBDiHH->Items->Add(item);
            cbSBDenHH->Items->Add(item);
        }
        if (cbSBDiHH->Items->Count > 0) cbSBDiHH->SelectedIndex = 0;
        if (cbSBDenHH->Items->Count > 1) cbSBDenHH->SelectedIndex = 1;

        tabHangHoa->Controls->Add(TaoLabel(L"Loại khách hàng:", 450, 70, false));
        cbLoaiKhachHH = TaoComboBox(580, 70, 250);
        cbLoaiKhachHH->Items->AddRange(gcnew cli::array<String^>{L"Cá nhân", L"Doanh nghiệp / Công ty HH", L"Bệnh viện / Cơ sở Y tế"});
        cbLoaiKhachHH->SelectedIndex = 0;

        tabHangHoa->Controls->Add(TaoLabel(L"Loại hàng hóa:", 450, 110, false));
        cbLoaiHangHH = TaoComboBox(580, 110, 250);
        cbLoaiHangHH->Items->AddRange(gcnew cli::array<String^>{L"Hàng hóa thông thường", L"Thiết bị Y tế / Dược phẩm", L"Hàng dễ vỡ"});
        cbLoaiHangHH->SelectedIndex = 0;

        tabHangHoa->Controls->Add(TaoLabel(L"Khối lượng (Kg):", 450, 150, false));
        txtKhoiLuongHH = TaoTextBox(580, 150, 250);
        txtKhoiLuongHH->Text = L"10";

        btnTaoPhieuHH = gcnew Button();
        btnTaoPhieuHH->Text = L"TÍNH GIÁ & TẠO PHIẾU";
        btnTaoPhieuHH->Location = Point(870, 140);
        btnTaoPhieuHH->Size = Drawing::Size(200, 40);
        btnTaoPhieuHH->BackColor = Color::MediumSeaGreen;
        btnTaoPhieuHH->ForeColor = Color::White;
        btnTaoPhieuHH->FlatStyle = FlatStyle::Flat;
        btnTaoPhieuHH->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
        btnTaoPhieuHH->Click += gcnew EventHandler(this, &MainForm::XuLyTaoPhieuHangHoa);
        tabHangHoa->Controls->Add(btnTaoPhieuHH);

        gridHangHoa = gcnew DataGridView();
        gridHangHoa->Location = Point(20, 220);
        gridHangHoa->Size = Drawing::Size(1280, 520);
        gridHangHoa->AllowUserToAddRows = false;
        gridHangHoa->ReadOnly = true;
        gridHangHoa->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
        gridHangHoa->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
        gridHangHoa->ColumnCount = 8;
        gridHangHoa->Columns[0]->Name = L"Mã Phiếu";
        gridHangHoa->Columns[1]->Name = L"Người/Đơn vị gửi";
        gridHangHoa->Columns[2]->Name = L"Hành Trình";
        gridHangHoa->Columns[3]->Name = L"Loại Khách";
        gridHangHoa->Columns[4]->Name = L"Loại Hàng";
        gridHangHoa->Columns[5]->Name = L"Khối lượng (kg)";
        gridHangHoa->Columns[6]->Name = L"Tổng Tiền Cước";
        gridHangHoa->Columns[7]->Name = L"Ngày Lập";

        tabHangHoa->Controls->Add(txtNguoiGuiHH);
        tabHangHoa->Controls->Add(cbSBDiHH);
        tabHangHoa->Controls->Add(cbSBDenHH);
        tabHangHoa->Controls->Add(cbLoaiKhachHH);
        tabHangHoa->Controls->Add(cbLoaiHangHH);
        tabHangHoa->Controls->Add(txtKhoiLuongHH);
        tabHangHoa->Controls->Add(gridHangHoa);

        // ======================= TAB THỐNG KÊ (THÊM TÍNH NĂNG I/O) =======================
        lblTongDoanhThu = gcnew Label();
        lblTongDoanhThu->Location = Point(30, 30);
        lblTongDoanhThu->AutoSize = true;
        lblTongDoanhThu->Font = gcnew Drawing::Font("Segoe UI", 16, FontStyle::Bold);
        lblTongDoanhThu->ForeColor = Color::DarkBlue;
        lblTongDoanhThu->Text = L"DOANH THU VÉ MÁY BAY: 0 VND";

        lblDoanhThuHangHoa = gcnew Label();
        lblDoanhThuHangHoa->Location = Point(30, 80);
        lblDoanhThuHangHoa->AutoSize = true;
        lblDoanhThuHangHoa->Font = gcnew Drawing::Font("Segoe UI", 16, FontStyle::Bold);
        lblDoanhThuHangHoa->ForeColor = Color::DarkGreen;
        lblDoanhThuHangHoa->Text = L"DOANH THU HÀNG HÓA: 0 VND";
        
        gridThongKe = gcnew DataGridView();
        gridThongKe->Location = Point(30, 140);
        gridThongKe->Size = Drawing::Size(800, 200);
        gridThongKe->AllowUserToAddRows = false;
        gridThongKe->ReadOnly = true;
        gridThongKe->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
        gridThongKe->ColumnCount = 3;
        gridThongKe->Columns[0]->Name = L"Hạng Mục / Loại Vé";
        gridThongKe->Columns[1]->Name = L"Số Lượng";
        gridThongKe->Columns[2]->Name = L"Doanh Thu (VND)";

        btnLuuDuLieu = gcnew Button();
        btnLuuDuLieu->Text = L"LƯU DỮ LIỆU RA FILE (.CSV)";
        btnLuuDuLieu->Location = Point(30, 360);
        btnLuuDuLieu->Size = Drawing::Size(250, 45);
        btnLuuDuLieu->BackColor = Color::Teal;
        btnLuuDuLieu->ForeColor = Color::White;
        btnLuuDuLieu->FlatStyle = FlatStyle::Flat;
        btnLuuDuLieu->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
        btnLuuDuLieu->Click += gcnew EventHandler(this, &MainForm::XuLyLuuFile);

        tabThongKe->Controls->Add(lblTongDoanhThu);
        tabThongKe->Controls->Add(lblDoanhThuHangHoa);
        tabThongKe->Controls->Add(gridThongKe);
        tabThongKe->Controls->Add(btnLuuDuLieu);

        tabCtrl->Controls->Add(tabDatVe);
        tabCtrl->Controls->Add(tabDanhSach);
        tabCtrl->Controls->Add(tabHangHoa);
        tabCtrl->Controls->Add(tabThongKe);
        this->Controls->Add(tabCtrl);

        tabCtrl->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::CapNhatTab);
    }

    void XuLyChonLoaiVe(Object^ sender, EventArgs^ e) {
        bool laHaiChieu = rbHaiChieu->Checked;
        lblNgayVe->Visible = laHaiChieu;
        dtpNgayVe->Visible = laHaiChieu;
    }

    void XuLyTimChuyenBay(Object^ sender, EventArgs^ e) {
        if (cbSanBayDi->SelectedIndex == cbSanBayDen->SelectedIndex) {
            MessageBox::Show(L"Sân bay đến không được trùng sân bay đi!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        gridChuyenBay->Rows->Clear();
        dsChuyenBayHienTai->clear();

        std::string iataDi = SysToStr(cbSanBayDi->SelectedItem->ToString()).substr(0, 3);
        std::string iataDen = SysToStr(cbSanBayDen->SelectedItem->ToString()).substr(0, 3);
        std::string ngay = SysToStr(dtpNgayBay->Text);

        SanBay sbDi = DuLieuSanBay::layTheHien()->timKiem(iataDi);
        SanBay sbDen = DuLieuSanBay::layTheHien()->timKiem(iataDen);
        double km = DuLieuSanBay::layTheHien()->tinhKhoangCach(sbDi, sbDen);

        bool laQuocTeXa = (km > 3000);

        std::vector<HangHangKhong> hks = DuLieuHangKhong::layTheHien()->layDanhSach();
        std::vector<HangHangKhong> hksPhuHop;

        for (size_t i = 0; i < hks.size(); i++) {
            if (laQuocTeXa) {
                if (hks[i].maHang == "EK" || hks[i].maHang == "QR" || hks[i].maHang == "SQ" || hks[i].maHang == "VN") {
                    hksPhuHop.push_back(hks[i]);
                }
            }
            else {
                if (hks[i].maHang == "VN" || hks[i].maHang == "VJ" || hks[i].maHang == "QH" || hks[i].maHang == "SQ") {
                    hksPhuHop.push_back(hks[i]);
                }
            }
        }
        if (hksPhuHop.empty()) hksPhuHop = hks;

        std::vector<std::string> mbs;
        if (laQuocTeXa) {
            mbs.push_back("Airbus A350-900");
            mbs.push_back("Airbus A380-800");
            mbs.push_back("Boeing 777-300ER");
            mbs.push_back("Boeing 787-9 Dreamliner");
        }
        else {
            mbs.push_back("Airbus A321neo");
            mbs.push_back("Airbus A320");
            mbs.push_back("Boeing 737 MAX 8");
        }

        int soChuyen = 3 + rand() % 3;
        for (int i = 0; i < soChuyen; i++) {
            ChuyenBay cb;
            cb.hangBay = hksPhuHop[rand() % hksPhuHop.size()];
            cb.maCB = cb.hangBay.maHang + std::to_string(100 + rand() % 900);
            cb.sbDi = sbDi;
            cb.sbDen = sbDen;
            cb.dongMayBay = mbs[rand() % mbs.size()];

            int gioDi = 6 + rand() % 14;
            int phutDi = (rand() % 12) * 5;
            int thoiGianBay = laQuocTeXa ? (300 + rand() % 600) : (60 + rand() % 120);

            int tongPhutDi = gioDi * 60 + phutDi;
            int tongPhutDen = tongPhutDi + thoiGianBay;

            int ngayThem = tongPhutDen / 1440;
            int gioDen = (tongPhutDen % 1440) / 60;
            int phutDen = tongPhutDen % 60;

            std::string ghiChu = "";
            if (ngayThem > 0) {
                ghiChu = " (+" + std::to_string(ngayThem) + " ngay)";
            }

            std::stringstream ssDi, ssDen;
            ssDi << ngay << " " << std::setfill('0') << std::setw(2) << gioDi << ":" << std::setfill('0') << std::setw(2) << phutDi;
            ssDen << ngay << " " << std::setfill('0') << std::setw(2) << gioDen << ":" << std::setfill('0') << std::setw(2) << phutDen << ghiChu;

            cb.thoiGianDi = ssDi.str();
            cb.thoiGianDen = ssDen.str();

            double base;
            if (laQuocTeXa) {
                base = 12000000.0 + (rand() % 200) * 100000.0;
            }
            else {
                base = 1200000.0 + (rand() % 20) * 100000.0;
            }

            cb.giaNiemYet = base;
            dsChuyenBayHienTai->push_back(cb);

            gridChuyenBay->Rows->Add(
                StrToSys(cb.hangBay.tenHang),
                StrToSys(cb.maCB),
                StrToSys(cb.dongMayBay),
                StrToSys(cb.thoiGianDi),
                StrToSys(cb.thoiGianDen),
                cb.giaNiemYet.ToString(L"N0")
            );
        }
    }

    void CapNhatHinhMucHanhLy(Object^ sender, EventArgs^ e) {
        if (cbHangVe->SelectedIndex == 0) lblHMDinhMuc->Text = L"Định mức: 23kg KG / 7kg XT";
        else if (cbHangVe->SelectedIndex == 1) lblHMDinhMuc->Text = L"Định mức: 40kg KG / 14kg XT";
        else lblHMDinhMuc->Text = L"Định mức: 50kg KG / 20kg XT";
    }

    void XuLyTruyXuatCCCD(Object^ sender, EventArgs^ e) {
        std::string cccd = SysToStr(txtCCCD->Text);
        if (cccd.length() == 12) {
            ThongTinCCCD tt = PhanTichCCCD::phanTich(cccd);
            if (tt.hopLe) {
                std::string msg = tt.gioiTinh + " | Sinh nam: " + std::to_string(tt.namSinh) + " (" + std::to_string(tt.tuoi) + " tuoi) | " + tt.tenTinh;
                lblThongTinCCCD->Text = StrToSys(msg);
                lblThongTinCCCD->ForeColor = Color::Green;
            }
            else {
                lblThongTinCCCD->Text = L"Cấu trúc định dạng CCCD không hợp lệ!";
                lblThongTinCCCD->ForeColor = Color::Red;
            }
        }
        else {
            lblThongTinCCCD->Text = L"";
        }
    }
    // THOAI
    void XuLyDatVe(Object^ sender, EventArgs^ e) {
        if (gridChuyenBay->SelectedRows->Count == 0) {
            MessageBox::Show(L"Vui lòng tìm kiếm và chọn một chuyến bay từ danh sách phía trên!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }

        std::string ten = SysToStr(txtHoTen->Text);
        std::string cccd = SysToStr(txtCCCD->Text);
        std::string sdt = SysToStr(txtSDT->Text);

        if (ten.empty() || cccd.empty()) {
            MessageBox::Show(L"Vui lòng nhập đủ Họ tên và CCCD!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        ThongTinCCCD tt = PhanTichCCCD::phanTich(cccd);
        if (!tt.hopLe) {
            MessageBox::Show(L"Dữ liệu căn cước công dân không chính xác!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        int selectedIdx = gridChuyenBay->SelectedRows[0]->Index;
        ChuyenBay cbChon = (*dsChuyenBayHienTai)[selectedIdx];

        double kgKG = 0.0, kgXT = 0.0;
        try { kgKG = Convert::ToDouble(txtKyGui->Text); }
        catch (...) { kgKG = 0.0; }
        try { kgXT = Convert::ToDouble(txtXachTay->Text); }
        catch (...) { kgXT = 0.0; }

        HanhKhach hk;
        hk.thietLapHK(ten, tt, sdt, SysToStr(txtQuocTich->Text));

        HangVe* hv = nullptr;
        if (cbHangVe->SelectedIndex == 0) hv = new PhoThong();
        else if (cbHangVe->SelectedIndex == 1) hv = new ThuongGia();
        else hv = new HangNhat();

        // Xử lý logic 1 chiều hay 2 chiều
        bool laHaiChieu = rbHaiChieu->Checked;
        std::string ngayVeStr = laHaiChieu ? SysToStr(dtpNgayVe->Text) : "";

        double tienPhat = 0;
        double tongTien = TinhGiaVe::tinhToan(cbChon.giaNiemYet, hv, kgKG, kgXT, tienPhat);
        if (laHaiChieu) {
            tongTien *= 2;
        }

        String^ strHanhTrinhVe = laHaiChieu ? L"\nLoại vé: KHỨ HỒI (Ngày về: " + dtpNgayVe->Text + L")" : L"\nLoại vé: MỘT CHIỀU";

        String^ strThongBao = String::Format(L"XÁC NHẬN THÔNG TIN ĐẶT VÉ:\n\n" +
            L"Họ và tên: {0}\n" +
            L"CCCD: {1}\n" +
            L"Sân bay đi: {2}\n" +
            L"Sân bay đến: {3}\n" +
            L"Chuyến bay: {4} ({5})\n" +
            L"Hạng vé: {6}{7}\n\n" +
            L"TỔNG TIỀN DỰ KIẾN: {8} VND\n\nĐồng ý đặt?",
            txtHoTen->Text, txtCCCD->Text, StrToSys(cbChon.sbDi.ten), StrToSys(cbChon.sbDen.ten),
            StrToSys(cbChon.hangBay.tenHang), StrToSys(cbChon.maCB), cbHangVe->Text, strHanhTrinhVe, tongTien.ToString(L"N0"));

        System::Windows::Forms::DialogResult dr = MessageBox::Show(strThongBao, L"Xác Nhận", MessageBoxButtons::YesNo, MessageBoxIcon::Information);

        if (dr == System::Windows::Forms::DialogResult::Yes) {
            std::string maPNR = "V" + std::to_string(10000 + rand() % 90000);

            // Khởi tạo vé và ghi nhận thông tin 2 chiều
            VeMayBay veMoi(maPNR, hk, cbChon, hv, kgKG, kgXT, laHaiChieu, ngayVeStr);

            heThongVe.them(veMoi);
            MessageBox::Show(L"Đặt vé thành công!", L"Thành Công", MessageBoxButtons::OK, MessageBoxIcon::Information);
            txtHoTen->Clear(); txtCCCD->Clear(); txtSDT->Clear();
            gridChuyenBay->Rows->Clear(); dsChuyenBayHienTai->clear();
        }
        delete hv;
    }

    void DoDuLieuVaoGrid(const QuanLyDanhSach<VeMayBay>& ds) {
        gridVe->Rows->Clear();
        for (size_t i = 0; i < ds.kichThuoc(); i++) {
            ThongTinCCCD tt = ds[i].layThongTinCCCD();
            String^ loaiChuyen = ds[i].layLaHaiChieu() ? L"Khứ Hồi" : L"Một Chiều";
            String^ ngayVStr = ds[i].layLaHaiChieu() ? StrToSys(ds[i].layNgayVe()) : L"-";

            gridVe->Rows->Add(
                StrToSys(ds[i].layPNR()), StrToSys(ds[i].laySoHieu()), StrToSys(ds[i].layTenKhach()),
                StrToSys(tt.soCCCD), StrToSys(ds[i].layHanhTrinh()),
                loaiChuyen, // Cột Loại chuyến
                StrToSys(ds[i].layNgayGioBay()),
                ngayVStr,   // Cột Ngày về
                StrToSys(ds[i].layHangBay()), StrToSys(ds[i].layTenHangVe()), StrToSys(ds[i].layChuoiHanhLy()),
                ds[i].layTongTien().ToString(L"N0"), StrToSys(ds[i].layTrangThai()), StrToSys(ds[i].layNgayDat())
            );
        }
    }

    void XuLyTimKiem(Object^ sender, EventArgs^ e) {
        std::string kw = SysToStr(txtTimKiem->Text);
        if (kw.empty()) {
            DoDuLieuVaoGrid(heThongVe); return;
        }
        QuanLyDanhSach<VeMayBay> ketQua;
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            if (heThongVe[i].layPNR().find(kw) != std::string::npos || heThongVe[i].layThongTinCCCD().soCCCD.find(kw) != std::string::npos) {
                ketQua.them(heThongVe[i]);
            }
        }
        DoDuLieuVaoGrid(ketQua);
    }

    void XuLyHuyVe(Object^ sender, EventArgs^ e) {
        if (gridVe->SelectedRows->Count > 0) {
            int idx = gridVe->SelectedRows[0]->Index;
            String^ pnrSys = gridVe->Rows[idx]->Cells[0]->Value->ToString();

            System::Windows::Forms::DialogResult ketQua = MessageBox::Show(L"Bạn có chắc chắn muốn hủy vé: " + pnrSys + L" không?", L"Xác Nhận Hủy", MessageBoxButtons::YesNo, MessageBoxIcon::Warning);

            if (ketQua == System::Windows::Forms::DialogResult::Yes) {
                std::string pnrToCancel = SysToStr(pnrSys);
                for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
                    if (heThongVe[i].layPNR() == pnrToCancel) {
                        heThongVe[i].huyVe();
                        MessageBox::Show(L"Hủy vé thành công!", L"Thông báo", MessageBoxButtons::OK, MessageBoxIcon::Information);
                        DoDuLieuVaoGrid(heThongVe);
                        break;
                    }
                }
            }
        }
    }
    
    void XuLyXoaVeHuy(Object^ sender, EventArgs^ e) {
        System::Windows::Forms::DialogResult dr = MessageBox::Show(L"Hành động này sẽ xóa vĩnh viễn các vé có trạng thái 'Đã Hủy'. Tiếp tục?", L"Cảnh Báo", MessageBoxButtons::YesNo, MessageBoxIcon::Warning);
        if (dr == System::Windows::Forms::DialogResult::Yes) {
            heThongVe.xoaTheoDieuKien(KiemTraVeDaHuy);
            DoDuLieuVaoGrid(heThongVe);
            MessageBox::Show(L"Đã dọn dẹp các vé bị hủy bằng STL Algorithm thành công!", L"Thông Báo", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
    }

    void XuLyTaoPhieuHangHoa(Object^ sender, EventArgs^ e) {
        std::string ten = SysToStr(txtNguoiGuiHH->Text);
        if (ten.empty()) {
            MessageBox::Show(L"Vui lòng nhập tên người/đơn vị gửi!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        if (cbSBDiHH->SelectedIndex == cbSBDenHH->SelectedIndex) {
            MessageBox::Show(L"Sân bay đến không được trùng sân bay đi!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        double kl = 0.0;
        try { kl = Convert::ToDouble(txtKhoiLuongHH->Text); }
        catch (...) {
            MessageBox::Show(L"Khối lượng không hợp lệ!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        if (kl <= 0) {
            MessageBox::Show(L"Khối lượng phải lớn hơn 0!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        // =================================================================
        // ĐOẠN MÃ THÊM MỚI: Giới hạn khối lượng <= 100kg
        // =================================================================
        if (kl > 50) {
            MessageBox::Show(L"Khối lượng hàng hóa gửi không được vượt quá 50kg đối với mọi loại khách hàng!", L"Giới hạn khối lượng", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        // =================================================================

        // TUAN
        std::string iataDi = SysToStr(cbSBDiHH->SelectedItem->ToString()).substr(0, 3);
        std::string iataDen = SysToStr(cbSBDenHH->SelectedItem->ToString()).substr(0, 3);

        SanBay sbDi = DuLieuSanBay::layTheHien()->timKiem(iataDi);
        SanBay sbDen = DuLieuSanBay::layTheHien()->timKiem(iataDen);
        double km = DuLieuSanBay::layTheHien()->tinhKhoangCach(sbDi, sbDen);

        int lk = cbLoaiKhachHH->SelectedIndex;
        int lh = cbLoaiHangHH->SelectedIndex;

        double tienCuoc = TinhGiaHangHoa::tinhToan(km, kl, lk, lh);

        String^ strThongBao = String::Format(L"XÁC NHẬN PHIẾU KÝ GỬI HÀNG HÓA:\n\n" +
            L"Người gửi: {0}\n" +
            L"Hành trình: {1} - {2} ({3} km)\n" +
            L"Loại khách: {4}\n" +
            L"Loại hàng: {5}\n" +
            L"Khối lượng: {6} kg\n\n" +
            L"TỔNG CƯỚC DỰ KIẾN: {7} VND\n\nĐồng ý lập phiếu?",
            txtNguoiGuiHH->Text, StrToSys(sbDi.iata), StrToSys(sbDen.iata), (int)km,
            cbLoaiKhachHH->Text, cbLoaiHangHH->Text, kl, tienCuoc.ToString(L"N0"));

        System::Windows::Forms::DialogResult dr = MessageBox::Show(strThongBao, L"Xác Nhận", MessageBoxButtons::YesNo, MessageBoxIcon::Information);

        if (dr == System::Windows::Forms::DialogResult::Yes) {
            PhieuHangHoa ph;
            ph.maPhieu = "CGO" + std::to_string(100000 + rand() % 900000);
            ph.tenNguoiGui = ten;
            ph.sbDi = sbDi;
            ph.sbDen = sbDen;
            ph.khoangCach = km;
            ph.loaiKhach = SysToStr(cbLoaiKhachHH->Text);
            ph.loaiHang = SysToStr(cbLoaiHangHH->Text);
            ph.khoiLuong = kl;
            ph.tongTien = tienCuoc;

            time_t now = time(0);
            char buf[80];
            strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", localtime(&now));
            ph.ngayTao = std::string(buf);

            heThongHangHoa.them(ph);
            MessageBox::Show(L"Lập phiếu hàng hóa thành công!", L"Thành Công", MessageBoxButtons::OK, MessageBoxIcon::Information);

            DoDuLieuHangHoaVaoGrid();
            txtNguoiGuiHH->Clear();
        }
    }

    void DoDuLieuHangHoaVaoGrid() {
        gridHangHoa->Rows->Clear();
        for (size_t i = 0; i < heThongHangHoa.kichThuoc(); i++) {
            gridHangHoa->Rows->Add(
                StrToSys(heThongHangHoa[i].maPhieu),
                StrToSys(heThongHangHoa[i].tenNguoiGui),
                StrToSys(heThongHangHoa[i].sbDi.iata + " - " + heThongHangHoa[i].sbDen.iata),
                StrToSys(heThongHangHoa[i].loaiKhach),
                StrToSys(heThongHangHoa[i].loaiHang),
                heThongHangHoa[i].khoiLuong.ToString(),
                heThongHangHoa[i].tongTien.ToString(L"N0"),
                StrToSys(heThongHangHoa[i].ngayTao)
            );
        }
    }

    void XuLyLuuFile(Object^ sender, EventArgs^ e) {
        try {
            
            std::ofstream fileOut("DuLieuHeThong.csv", std::ios::out | std::ios::trunc | std::ios::binary);

            if (!fileOut.is_open()) {
                throw std::ios_base::failure("Khong the truy cap file de ghi du lieu (Kiem tra quyen admin)!");
            }

            
            fileOut << "\xEF\xBB\xBF";

            fileOut << "--- DANH SACH VE MAY BAY ---\n";
            fileOut << "PNR,So Hieu,Ho Ten,CCCD,Hanh Trinh,Loai Ve,Ngay Bay,Ngay Ve,Hang Ve,Tong Tien,Trang Thai\n";
            for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
                fileOut << heThongVe[i].layPNR() << ","
                    << heThongVe[i].laySoHieu() << ","
                    << heThongVe[i].layTenKhach() << ","
                    << heThongVe[i].layThongTinCCCD().soCCCD << ","
                    << heThongVe[i].layHanhTrinh() << ","
                    << (heThongVe[i].layLaHaiChieu() ? "Khu Hoi" : "Mot Chieu") << ","
                    << heThongVe[i].layNgayGioBay() << ","
                    << (heThongVe[i].layLaHaiChieu() ? heThongVe[i].layNgayVe() : "-") << ","
                    << heThongVe[i].layTenHangVe() << ","
                    << heThongVe[i].layTongTien() << ","
                    << heThongVe[i].layTrangThai() << "\n";
            }

            fileOut << "\n--- DANH SACH PHIEU HANG HOA ---\n";
            fileOut << "Ma Phieu,Nguoi Gui,Hanh Trinh,Loai Khach,Loai Hang,Khoi Luong,Tong Tien\n";
            for (size_t i = 0; i < heThongHangHoa.kichThuoc(); i++) {
                fileOut << heThongHangHoa[i].maPhieu << ","
                    << heThongHangHoa[i].tenNguoiGui << ","
                    << heThongHangHoa[i].sbDi.iata << "-" << heThongHangHoa[i].sbDen.iata << ","
                    << heThongHangHoa[i].loaiKhach << ","
                    << heThongHangHoa[i].loaiHang << ","
                    << heThongHangHoa[i].khoiLuong << ","
                    << heThongHangHoa[i].tongTien << "\n";
            }

            fileOut.close();
            MessageBox::Show(L"Xuất file CSV thành công!\n(File được lưu tại thư mục chứa mã nguồn: DuLieuHeThong.csv)", L"Thành Công", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (const std::exception& ex) {
            String^ errorMsg = StrToSys(ex.what());
            MessageBox::Show(L"Lỗi File I/O: " + errorMsg, L"Bắt Ngoại Lệ Hệ Thống", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }

    void CapNhatTab(Object^ sender, EventArgs^ e) {
        if (tabCtrl->SelectedTab == tabDanhSach) {
            DoDuLieuVaoGrid(heThongVe);
        }
        else if (tabCtrl->SelectedTab == tabHangHoa) {
            DoDuLieuHangHoaVaoGrid();
        }
        else if (tabCtrl->SelectedTab == tabThongKe) {
            double tongDT = 0, dtY = 0, dtC = 0, dtF = 0;
            int countY = 0, countC = 0, countF = 0;
            int veHuy = 0;
            double tienHuy = 0;

            for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
                double t = heThongVe[i].layTongTien();
                std::string hang = heThongVe[i].layTenHangVe();
                if (heThongVe[i].layTrangThai() == "Da Huy") {
                    veHuy++;
                    tienHuy += t;
                }
                else {
                    tongDT += t;
                    if (hang == "Economy") { countY++; dtY += t; }
                    else if (hang == "Business") { countC++; dtC += t; }
                    else { countF++; dtF += t; }
                }
            }

            double dtHH = 0;
            for (size_t i = 0; i < heThongHangHoa.kichThuoc(); i++) {
                dtHH += heThongHangHoa[i].tongTien;
            }

            lblTongDoanhThu->Text = L"DOANH THU VÉ MÁY BAY: " + tongDT.ToString(L"N0") + L" VND  |  ĐÃ HỦY: " + veHuy.ToString() + L" Vé";
            lblDoanhThuHangHoa->Text = L"DOANH THU HÀNG HÓA: " + dtHH.ToString(L"N0") + L" VND  |  TỔNG PHIẾU: " + heThongHangHoa.kichThuoc().ToString();

            gridThongKe->Rows->Clear();
            gridThongKe->Rows->Add(L"Vé Phổ Thông", countY.ToString(), dtY.ToString(L"N0"));
            gridThongKe->Rows->Add(L"Vé Thương Gia", countC.ToString(), dtC.ToString(L"N0"));
            gridThongKe->Rows->Add(L"Vé Hạng Nhất", countF.ToString(), dtF.ToString(L"N0"));
            gridThongKe->Rows->Add(L"Ký Gửi Hàng Hóa", heThongHangHoa.kichThuoc().ToString(), dtHH.ToString(L"N0"));
        }
    }

public:
    MainForm() {
        KhoiTaoUI();
    }
    ~MainForm() {
        delete dsChuyenBayHienTai;
    }
};

[STAThreadAttribute]
int main(array<String^>^ args) {
    srand((unsigned)time(NULL));
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew MainForm());
    return 0;
}
