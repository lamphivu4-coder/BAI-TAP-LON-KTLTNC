#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.dll>

#pragma warning(disable: 4996)

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <msclr/marshal_cppstd.h>
#include <cctype>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Net;
using namespace System::Net::Mail;

// ========================== CÁC HÀM TIỆN ÍCH ==========================
String^ StrToSys(const std::string& str) {
    array<Byte>^ bytes = gcnew array<Byte>(str.length());
    for (size_t i = 0; i < str.length(); ++i) bytes[i] = (Byte)str[i];
    return System::Text::Encoding::UTF8->GetString(bytes);
}

std::string SysToStr(String^ str) {
    array<Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(str);
    std::string result = "";
    for (int i = 0; i < bytes->Length; ++i) result += (char)bytes[i];
    return result;
}

double tuyetDoi(double x) { return x < 0 ? -x : x; }

double tinhCanBacHai(double n) {
    if (n <= 0) return 0;
    double x = n, y = 1, e = 0.000001;
    while (x - y > e) { x = (x + y) / 2; y = n / x; }
    return x;
}

// ========================== CLASS DỊCH VỤ EMAIL ==========================
class DichVuEmail {
public:
    static void GuiThongBaoDatVe(String^ emailKhachHang, String^ tenKhach, String^ pnr, String^ thongTinChuyenBay, String^ thoiGianBay, String^ thoiGianDatThuc, String^ tongTien) {
        try {
            MailMessage^ mail = gcnew MailMessage();
            mail->From = gcnew MailAddress("air.connect.notify@gmail.com", L"AirConnect System Alert");
            mail->To->Add(emailKhachHang);
            mail->Subject = L"✈ XÁC NHẬN ĐẶT VÉ THÀNH CÔNG / TICKET CONFIRMATION - PNR: " + pnr;
            mail->IsBodyHtml = false;

            String^ body = L"Kính chào quý khách / Dear " + tenKhach + L",\n\n" +
                L"Cảm ơn quý khách đã tin tưởng và lựa chọn dịch vụ của UTC2 Airlines.\n" +
                L"Thank you for choosing UTC2 Airlines.\n\n" +
                L"Hệ thống đã ghi nhận thông tin đặt giữ chỗ của quý khách với các thông tin chi tiết sau:\n" +
                L"Your reservation has been confirmed with the following details:\n\n" +
                L"---------------------------------------------------\n" +
                L"▶ MÃ ĐẶT CHỖ / PNR      : " + pnr + L"\n" +
                L"▶ THỜI GIAN ĐẶT / BOOKED : " + thoiGianDatThuc + L"\n" + // Thời gian thực tế hệ thống xử lý giao dịch
                L"▶ HÀNH TRÌNH / ITINERARY : " + thongTinChuyenBay + L"\n" +
                L"▶ THỜI GIAN BAY / TIME   : " + thoiGianBay + L"\n" +       // Đã bao gồm khứ hồi nếu có
                L"▶ TỔNG THANH TOÁN / TOTAL: " + tongTien + L" VND\n" +
                L"---------------------------------------------------\n\n" +
                L"Quý khách vui lòng mang theo giấy tờ tùy thân (CCCD/Hộ chiếu) đến quầy thủ tục trước 2 tiếng so với giờ khởi hành.\n" +
                L"Please present your ID/Passport at the check-in counter at least 2 hours before departure.\n\n" +
                L"Kính chúc quý khách có một chuyến bay an toàn và vui vẻ!\n" +
                L"We wish you a safe and pleasant flight!\n\n" +
                L"Trân trọng, / Best regards,\nĐội ngũ UTC2 Airlines / UTC2 Airlines Team.";

            mail->Body = body;

            SmtpClient^ smtp = gcnew SmtpClient("smtp.gmail.com");
            smtp->Port = 587;
            smtp->EnableSsl = true;
            smtp->UseDefaultCredentials = false;
            smtp->DeliveryMethod = SmtpDeliveryMethod::Network;
            smtp->Credentials = gcnew NetworkCredential("air.connect.notify@gmail.com", "ahph mlqi vimv keob");

            smtp->Send(mail);
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Hệ thống đặt vé thành công nhưng việc gửi Email thông báo bị lỗi: " + ex->Message,
                L"Cảnh báo hệ thống Email", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        }
    }
};

// ========================== CLASS KIỂM TRA ==========================
class KiemTra {
public:
    static bool hopLeCCCD(const std::string& cccd) {
        if (cccd.length() != 12) return false;
        for (size_t i = 0; i < cccd.length(); i++)
            if (cccd[i] < '0' || cccd[i] > '9') return false;
        return true;
    }
    static bool hopLeHoChieu(const std::string& hc) {
        if (hc.length() < 6 || hc.length() > 15) return false;
        for (char c : hc) if (!isalnum(c)) return false;
        return true;
    }
    static bool hopLeSDT(const std::string& phone) {
        if (phone.empty()) return false;
        if (phone[0] == '+') {
            if (phone.length() < 11 || phone.length() > 15) return false;
            for (size_t i = 1; i < phone.length(); i++)
                if (phone[i] < '0' || phone[i] > '9') return false;
            return true;
        }
        return false;
    }
};

struct TinhThanh { std::string ma; std::string ten; };

struct ThongTinCCCD {
    std::string soCCCD, maTinh, tenTinh, gioiTinh;
    int namSinh, tuoi;
    bool hopLe;
};

class PhanTichCCCD {
private:
    static std::vector<TinhThanh> danhSachTinh;
    static void khoiTaoDuLieu() {
        if (!danhSachTinh.empty()) return;
        std::ifstream file("tinhthanh.txt");
        if (!file.is_open()) {
            danhSachTinh.push_back({ "079", u8"TP.Hồ Chí Minh" });
            danhSachTinh.push_back({ "001", u8"Hà Nội" });
            danhSachTinh.push_back({ "048", u8"Đà Nẵng" });
            return;
        }
        std::string dong;
        while (std::getline(file, dong)) {
            if (!dong.empty() && dong.back() == '\r') dong.pop_back();
            size_t pos = dong.find(',');
            if (pos != std::string::npos)
                danhSachTinh.push_back({ dong.substr(0, pos), dong.substr(pos + 1) });
        }
        file.close();
    }
public:
    static ThongTinCCCD phanTich(const std::string& cccd) {
        khoiTaoDuLieu();
        ThongTinCCCD tt; tt.soCCCD = cccd; tt.hopLe = false;
        if (!KiemTra::hopLeCCCD(cccd)) return tt;
        tt.maTinh = cccd.substr(0, 3); tt.tenTinh = u8"Không xác định";
        for (size_t i = 0; i < danhSachTinh.size(); i++) {
            if (danhSachTinh[i].ma == tt.maTinh) { tt.tenTinh = danhSachTinh[i].ten; break; }
        }
        int maGT = cccd[3] - '0'; int maNam;
        std::stringstream ss(cccd.substr(4, 2)); ss >> maNam;
        int theKy;
        if (maGT <= 1) { theKy = 1900; tt.gioiTinh = (maGT == 0) ? u8"Nam" : u8"Nữ"; }
        else if (maGT <= 3) { theKy = 2000; tt.gioiTinh = (maGT == 2) ? u8"Nam" : u8"Nữ"; }
        else if (maGT <= 5) { theKy = 2100; tt.gioiTinh = (maGT == 4) ? u8"Nam" : u8"Nữ"; }
        else if (maGT <= 7) { theKy = 2200; tt.gioiTinh = (maGT == 6) ? u8"Nam" : u8"Nữ"; }
        else { theKy = 2300; tt.gioiTinh = (maGT == 8) ? u8"Nam" : u8"Nữ"; }
        tt.namSinh = theKy + maNam;
        time_t t = time(0); struct tm* now = localtime(&t);
        tt.tuoi = (now->tm_year + 1900) - tt.namSinh;
        tt.hopLe = true; return tt;
    }
};
std::vector<TinhThanh> PhanTichCCCD::danhSachTinh;

// ========================== QUỐC GIA / SÂN BAY / HÃNG BAY ==========================
struct QuocGia { std::string ten, maVung; };

class DuLieuQuocGia {
private:
    std::vector<QuocGia> danhSach;
    static DuLieuQuocGia* theHien;
    DuLieuQuocGia() {
        danhSach.push_back({ u8"Việt Nam", "+84" });
        danhSach.push_back({ u8"Mỹ", "+1" });
        danhSach.push_back({ u8"Anh", "+44" });
        danhSach.push_back({ u8"Nhật Bản", "+81" });
        danhSach.push_back({ u8"Hàn Quốc", "+82" });
        danhSach.push_back({ u8"Pháp", "+33" });
    }
public:
    static DuLieuQuocGia* layTheHien() { if (!theHien) theHien = new DuLieuQuocGia(); return theHien; }
    std::vector<QuocGia> layDanhSach() { return danhSach; }
    std::string layMaVung(const std::string& tenQG) {
        for (size_t i = 0; i < danhSach.size(); i++)
            if (danhSach[i].ten == tenQG) return danhSach[i].maVung;
        return "+";
    }
};
DuLieuQuocGia* DuLieuQuocGia::theHien = nullptr;

struct SanBay { std::string iata, ten, thanhPho, quocGia; int khuVuc; double viDo, kinhDo; };

class DuLieuSanBay {
private:
    std::vector<SanBay> danhSach;
    static DuLieuSanBay* theHien;
    DuLieuSanBay() {
        khoiTaoDuLieu();
    }
    void khoiTaoDuLieu() {
        std::ifstream file("sanbay.txt");
        if (!file.is_open()) {
            danhSach.push_back({ "HAN", u8"Nội Bài", u8"Hà Nội", u8"Việt Nam", 1, 21.2212, 105.8072 });
            danhSach.push_back({ "SGN", u8"Tân Sơn Nhất", u8"TP.HCM", u8"Việt Nam", 1, 10.8189, 106.6519 });
            danhSach.push_back({ "DAD", u8"Đà Nẵng", u8"Đà Nẵng", u8"Việt Nam", 1, 16.0439, 108.1993 });
            danhSach.push_back({ "JFK", u8"John F. Kennedy", u8"New York", u8"Mỹ", 2, 40.6413, -73.7781 });
            danhSach.push_back({ "LHR", u8"Heathrow", u8"London", u8"Anh", 2, 51.4700, -0.4543 });
            return;
        }
        std::string dong;
        while (std::getline(file, dong)) {
            if (!dong.empty() && dong.back() == '\r') dong.pop_back();
            std::stringstream ss(dong);
            std::string iata, ten, thanhPho, quocGia, khuVucStr, viDoStr, kinhDoStr;
            if (std::getline(ss, iata, ',') && std::getline(ss, ten, ',') &&
                std::getline(ss, thanhPho, ',') && std::getline(ss, quocGia, ',') &&
                std::getline(ss, khuVucStr, ',') && std::getline(ss, viDoStr, ',') &&
                std::getline(ss, kinhDoStr, ',')) {
                SanBay sb;
                sb.iata = iata; sb.ten = ten; sb.thanhPho = thanhPho; sb.quocGia = quocGia;
                try {
                    sb.khuVuc = std::stoi(khuVucStr);
                    sb.viDo = std::stod(viDoStr);
                    sb.kinhDo = std::stod(kinhDoStr);
                    danhSach.push_back(sb);
                }
                catch (...) {}
            }
        }
        file.close();
    }
public:
    static DuLieuSanBay* layTheHien() { if (!theHien) theHien = new DuLieuSanBay(); return theHien; }
    std::vector<SanBay> layDanhSach() { return danhSach; }
    SanBay timKiem(const std::string& iata) {
        for (size_t i = 0; i < danhSach.size(); i++) if (danhSach[i].iata == iata) return danhSach[i];
        if (!danhSach.empty()) return danhSach[0];
        return { "","","","",0,0.0,0.0 };
    }
    double tinhKhoangCach(const SanBay& sb1, const SanBay& sb2) {
        double dLat = tuyetDoi(sb1.viDo - sb2.viDo) * 111.0;
        double dLon = tuyetDoi(sb1.kinhDo - sb2.kinhDo) * 111.0;
        return tinhCanBacHai(dLat * dLat + dLon * dLon);
    }
};
DuLieuSanBay* DuLieuSanBay::theHien = nullptr;

struct HangHangKhong { std::string maHang, tenHang; int soSao; };
struct ChuyenBay {
    std::string maCB; HangHangKhong hangBay; SanBay sbDi, sbDen;
    std::string dongMayBay, thoiGianDi, thoiGianDen; double giaNiemYet;
};

class DuLieuHangKhong {
private:
    std::vector<HangHangKhong> danhSach;
    static DuLieuHangKhong* theHien;
    DuLieuHangKhong() {
        khoiTaoDuLieu();
    }
    void khoiTaoDuLieu() {
        std::ifstream file("hangbay.txt");
        if (!file.is_open()) {
            danhSach.push_back({ "VN", u8"Vietnam Airlines", 4 });
            danhSach.push_back({ "VJ", u8"Vietjet Air", 3 });
            danhSach.push_back({ "QH", u8"Bamboo Airways", 4 });
            danhSach.push_back({ "EK", u8"Emirates", 5 });
            return;
        }
        std::string dong;
        while (std::getline(file, dong)) {
            if (!dong.empty() && dong.back() == '\r') dong.pop_back();
            std::stringstream ss(dong);
            std::string ma, ten, saoStr;
            if (std::getline(ss, ma, ',') && std::getline(ss, ten, ',') && std::getline(ss, saoStr, ',')) {
                try {
                    danhSach.push_back({ ma, ten, std::stoi(saoStr) });
                }
                catch (...) {}
            }
        }
        file.close();
    }
public:
    static DuLieuHangKhong* layTheHien() { if (!theHien) theHien = new DuLieuHangKhong(); return theHien; }
    std::vector<HangHangKhong> layDanhSach() { return danhSach; }
    HangHangKhong timKiemTheoMa(const std::string& ma) {
        for (size_t i = 0; i < danhSach.size(); i++) if (danhSach[i].maHang == ma) return danhSach[i];
        if (!danhSach.empty()) return danhSach[0];
        return { "","Unknown",0 };
    }
    HangHangKhong timKiemTheoTen(const std::string& ten) {
        for (size_t i = 0; i < danhSach.size(); i++) if (danhSach[i].tenHang == ten) return danhSach[i];
        if (!danhSach.empty()) return danhSach[0];
        return { "","Unknown",0 };
    }
};
DuLieuHangKhong* DuLieuHangKhong::theHien = nullptr;

// ========================== HẠNG VÉ ==========================
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
class PhoThong : public HangVe {
public:
    std::string layTenHang() const override { return u8"Economy"; }
    std::string layMaHang() const override { return "Y"; }
    double layHeSoGia() const override { return 1.0; }
    int layHanhLyKyGui() const override { return 23; }
    int layHanhLyXachTay() const override { return 7; }
    HangVe* taoBanSao() const override { return new PhoThong(*this); }
};
class ThuongGia : public HangVe {
public:
    std::string layTenHang() const override { return u8"Business"; }
    std::string layMaHang() const override { return "C"; }
    double layHeSoGia() const override { return 4.0; }
    int layHanhLyKyGui() const override { return 40; }
    int layHanhLyXachTay() const override { return 14; }
    HangVe* taoBanSao() const override { return new ThuongGia(*this); }
};
class HangNhat : public HangVe {
public:
    std::string layTenHang() const override { return u8"First Class"; }
    std::string layMaHang() const override { return "F"; }
    double layHeSoGia() const override { return 10.0; }
    int layHanhLyKyGui() const override { return 50; }
    int layHanhLyXachTay() const override { return 20; }
    HangVe* taoBanSao() const override { return new HangNhat(*this); }
};

// ========================== NGƯỜI / HÀNH KHÁCH / NHÂN VIÊN ==========================
class Nguoi {
protected:
    std::string hoTen; ThongTinCCCD thongTinCccd; std::string sdt;
public:
    Nguoi() {}
    void thietLap(std::string ten, ThongTinCCCD tt, std::string phone) { hoTen = ten; thongTinCccd = tt; sdt = phone; }
    std::string layHoTen() const { return hoTen; }
    ThongTinCCCD layThongTinCCCD() const { return thongTinCccd; }
    virtual ~Nguoi() {}
};

class HanhKhach : public Nguoi {
private:
    std::string quocTich, soHoChieu;
public:
    HanhKhach() {}
    void thietLapHK(std::string ten, ThongTinCCCD tt, std::string phone, std::string qt, std::string hc) {
        thietLap(ten, tt, phone); quocTich = qt; soHoChieu = hc;
    }
};

class NhanVien : public Nguoi {
private:
    std::string maNV, chucVu, phongBan, matKhau;
    int soKhachPhucVu;
    bool trangThaiTaiKhoan;
public:
    NhanVien() {}
    NhanVien(std::string ma, std::string ten, std::string mk, std::string cv = u8"Nhân viên phục vụ mặt đất", std::string pb = u8"Tiếp thị dịch vụ") {
        maNV = ma; hoTen = ten; matKhau = mk; chucVu = cv; phongBan = pb; soKhachPhucVu = 0; trangThaiTaiKhoan = true;
    }
    std::string layMaNV() const { return maNV; }
    std::string layMatKhau() const { return matKhau; }
    std::string layChucVu() const { return chucVu; }
    std::string layPhongBan() const { return phongBan; }
    int laySoKhachPhucVu() const { return soKhachPhucVu; }
    bool layTrangThai() const { return trangThaiTaiKhoan; }
    void tangSoKhach() { soKhachPhucVu++; }
    void khoaTaiKhoan() { trangThaiTaiKhoan = false; }
    void moTaiKhoan() { trangThaiTaiKhoan = true; }
};

// ========================== TÍNH GIÁ VÉ & HÀNG HÓA ==========================
class TinhGiaVe {
public:
    static double tinhToan(double giaCoBan, HangVe* hv, double kyGui, double xachTay, double& tienPhat) {
        double tong = giaCoBan * hv->layHeSoGia(); tienPhat = 0;
        if (kyGui > hv->layHanhLyKyGui()) tienPhat += (kyGui - hv->layHanhLyKyGui()) * 200000;
        if (xachTay > hv->layHanhLyXachTay()) tienPhat += (xachTay - hv->layHanhLyXachTay()) * 300000;
        return (tong * 1.2) + tienPhat;
    }
};

class TinhGiaHangHoa {
public:
    static double tinhToan(double khoangCach, double khoiLuong, int loaiKhach, int loaiHang, int tocDo) {
        double donGiaCoBan = 25.0;
        double heSoKhach = (loaiKhach == 0) ? 1.2 : (loaiKhach == 2) ? 0.85 : 1.0;
        double heSoHang = (loaiHang == 1) ? 1.5 : (loaiHang == 2) ? 1.3 : 1.0;
        double heSoTocDo = (tocDo == 1) ? 1.5 : (tocDo == 2) ? 2.0 : 1.0;
        double tongTien = khoangCach * khoiLuong * donGiaCoBan * heSoKhach * heSoHang * heSoTocDo;
        if (tongTien < 150000) tongTien = 150000;
        return tongTien;
    }
};

// ========================== VÉ MÁY BAY & PHIẾU HÀNG HÓA ==========================
class VeMayBay {
private:
    std::string pnr, soHieuCB, gheNgoi; HanhKhach hanhKhach; SanBay sbDi, sbDen;
    HangHangKhong hangBay; HangVe* hangVe; std::string ngayGioBay;
    bool laHaiChieu; std::string ngayVe;
    double hanhLyKG, hanhLyXT, tienPhatHanhLy, tongTien;
    std::string ngayDat, trangThai;
public:
    VeMayBay() : hangVe(nullptr), laHaiChieu(false) {}
    VeMayBay(std::string ma, HanhKhach hk, ChuyenBay cb, HangVe* hv, double kg, double xt, bool khamHoi, std::string ngayV, std::string ghe) {
        pnr = ma; soHieuCB = cb.maCB; hanhKhach = hk; sbDi = cb.sbDi; sbDen = cb.sbDen; gheNgoi = ghe;
        hangBay = cb.hangBay; hangVe = hv->taoBanSao(); hanhLyKG = kg; hanhLyXT = xt;
        ngayGioBay = cb.thoiGianDi; laHaiChieu = khamHoi; ngayVe = ngayV; trangThai = u8"Đã Đặt";
        tongTien = TinhGiaVe::tinhToan(cb.giaNiemYet, hv, kg, xt, tienPhatHanhLy);
        if (laHaiChieu) { tongTien *= 2; tienPhatHanhLy *= 2; }
        time_t now = time(0); char buf[80];
        strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", localtime(&now)); ngayDat = std::string(buf);
    }
    VeMayBay(const VeMayBay& k) {
        pnr = k.pnr; soHieuCB = k.soHieuCB; hanhKhach = k.hanhKhach; sbDi = k.sbDi; sbDen = k.sbDen; gheNgoi = k.gheNgoi;
        hangBay = k.hangBay; hanhLyKG = k.hanhLyKG; hanhLyXT = k.hanhLyXT; ngayGioBay = k.ngayGioBay;
        laHaiChieu = k.laHaiChieu; ngayVe = k.ngayVe; tienPhatHanhLy = k.tienPhatHanhLy;
        tongTien = k.tongTien; ngayDat = k.ngayDat; trangThai = k.trangThai;
        hangVe = k.hangVe ? k.hangVe->taoBanSao() : nullptr;
    }
    VeMayBay& operator=(const VeMayBay& k) {
        if (this != &k) {
            delete hangVe;
            pnr = k.pnr; soHieuCB = k.soHieuCB; hanhKhach = k.hanhKhach; sbDi = k.sbDi; sbDen = k.sbDen; gheNgoi = k.gheNgoi;
            hangBay = k.hangBay; hanhLyKG = k.hanhLyKG; hanhLyXT = k.hanhLyXT; ngayGioBay = k.ngayGioBay;
            laHaiChieu = k.laHaiChieu; ngayVe = k.ngayVe; tienPhatHanhLy = k.tienPhatHanhLy;
            tongTien = k.tongTien; ngayDat = k.ngayDat; trangThai = k.trangThai;
            hangVe = k.hangVe ? k.hangVe->taoBanSao() : nullptr;
        }
        return *this;
    }
    ~VeMayBay() { delete hangVe; }
    void setTongTien(double val) { tongTien = val; }
    void capNhatTrangThai(std::string ttMoi) { trangThai = ttMoi; }
    void huyVeLyDo(std::string lydo) { trangThai = u8"Đã Hủy (" + lydo + ")"; }
    void capNhatGheNgoi(std::string gheMoi) { gheNgoi = gheMoi; }
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
    std::string layGheNgoi() const { return gheNgoi; }
    double layTienPhat() const { return tienPhatHanhLy; }
    double layTongTien() const { return tongTien; }
    std::string layNgayDat() const { return ngayDat; }
    std::string layChuoiHanhLy() const {
        std::stringstream ss; ss << std::fixed << std::setprecision(1) << hanhLyKG << "kg KG / " << hanhLyXT << "kg XT"; return ss.str();
    }
};

struct PhieuHangHoa {
    std::string maPhieu, tenNguoiGui; SanBay sbDi, sbDen;
    std::string loaiKhach, loaiHang, tocDoGiao, trangThai;
    double khoiLuong, khoangCach, tongTien;
    std::string ngayTao;
};

struct MaGiamGia {
    std::string maMGG; double phanTramGiam;
    std::string ngayBatDau, gioBatDau, ngayHetHan, gioHetHan;
    int tongSoLuong, soLuongDaDung; std::string loaiVeApDung, moTa; bool dangHoatDong;
};

// ========================== DANH SÁCH TEMPLATE & GLOBAL ==========================
template <typename T>
class QuanLyDanhSach {
private:
    std::vector<T> danhSach;
public:
    void them(const T& item) { danhSach.push_back(item); }
    void xoaTai(size_t idx) { if (idx < danhSach.size()) danhSach.erase(danhSach.begin() + idx); }
    size_t kichThuoc() const { return danhSach.size(); }
    T& operator[](size_t index) { return danhSach[index]; }
    const T& operator[](size_t index) const { return danhSach[index]; }
    void xoaTatCa() { danhSach.clear(); }
    template<typename Predicate>
    void xoaTheoDieuKien(Predicate dk) { danhSach.erase(std::remove_if(danhSach.begin(), danhSach.end(), dk), danhSach.end()); }
};

QuanLyDanhSach<VeMayBay>   heThongVe;
QuanLyDanhSach<PhieuHangHoa> heThongHangHoa;
QuanLyDanhSach<ChuyenBay>  khoChuyenBayToanCuc;
QuanLyDanhSach<MaGiamGia>  heThongMaGiamGia;
QuanLyDanhSach<NhanVien>   danhSachNhanSu;

NhanVien* nhanVienHienTai = nullptr;
bool userIsAdmin = false;

bool KiemTraVeDaHuy(const VeMayBay& v) { return v.layTrangThai().find(u8"Đã Hủy") != std::string::npos; }

// ========================== QUẢN LÝ GHẾ NGỒI ==========================
class QuanLyGhe {
public:
    static std::string SinhGheTuDong(const std::string& soHieu, const std::string& ngayBay, const std::string& hangVe) {
        std::vector<std::string> gheDaDat;
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            if (heThongVe[i].laySoHieu() == soHieu && heThongVe[i].layNgayGioBay() == ngayBay && heThongVe[i].layTrangThai().find(u8"Đã Hủy") == std::string::npos) {
                gheDaDat.push_back(heThongVe[i].layGheNgoi());
            }
        }
        int rowStart = 6, rowEnd = 30;
        if (hangVe == u8"First Class") { rowStart = 1; rowEnd = 2; }
        else if (hangVe == u8"Business") { rowStart = 3; rowEnd = 5; }

        char cols[] = { 'A', 'B', 'C', 'D', 'E', 'F' };
        for (int r = rowStart; r <= rowEnd; r++) {
            for (char c : cols) {
                std::string gheCheck = std::to_string(r) + c;
                if (std::find(gheDaDat.begin(), gheDaDat.end(), gheCheck) == gheDaDat.end()) {
                    return gheCheck;
                }
            }
        }
        return "WL";
    }

    static bool KiemTraGheTrong(const std::string& soHieu, const std::string& ngayBay, const std::string& gheMoi) {
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            if (heThongVe[i].laySoHieu() == soHieu && heThongVe[i].layNgayGioBay() == ngayBay &&
                heThongVe[i].layGheNgoi() == gheMoi && heThongVe[i].layTrangThai().find(u8"Đã Hủy") == std::string::npos) {
                return false;
            }
        }
        return true;
    }
};

void Tao5ChuyenBayMacDinh() {
    std::vector<SanBay> dsSB = DuLieuSanBay::layTheHien()->layDanhSach();
    std::vector<HangHangKhong> dsHK = DuLieuHangKhong::layTheHien()->layDanhSach();
    if (dsSB.size() < 2 || dsHK.empty()) return;

    std::vector<SanBay> sbNoiDia, sbQuocTe;
    for (size_t i = 0; i < dsSB.size(); i++) {
        if (dsSB[i].quocGia == u8"Việt Nam" || dsSB[i].quocGia == "Viet Nam" || dsSB[i].quocGia == "VN" || dsSB[i].quocGia.find("Vi") != std::string::npos) {
            sbNoiDia.push_back(dsSB[i]);
        }
        else {
            sbQuocTe.push_back(dsSB[i]);
        }
    }

    if (sbNoiDia.empty()) return;
    if (sbQuocTe.empty()) sbQuocTe = sbNoiDia;

    time_t now = time(0);

    auto TaoCapChuyenBay = [&](SanBay sb1, SanBay sb2, HangHangKhong hk, int gioBay) {
        struct tm timeinfo = *localtime(&now);
        timeinfo.tm_mday += (1 + rand() % 5);
        timeinfo.tm_hour = 6 + rand() % 12;
        timeinfo.tm_min = (rand() % 12) * 5;
        time_t tDi = mktime(&timeinfo);

        // Đã tích hợp đầy đủ mảng các loại tàu bay hiện đại
        std::vector<std::string> dsMayBay = {
            "Airbus A321neo",
            "Airbus A350-900",
            "Boeing 787-9 Dreamliner",
            "Boeing 777-300ER",
            "Airbus A330-300",
            "Boeing 737 MAX 8"
        };
        std::string mayBayNgauNhien = dsMayBay[rand() % dsMayBay.size()];

        ChuyenBay cbDi;
        cbDi.hangBay = hk;
        cbDi.maCB = hk.maHang + std::to_string(100 + rand() % 900);
        cbDi.sbDi = sb1;
        cbDi.sbDen = sb2;
        cbDi.dongMayBay = mayBayNgauNhien; // Lấy ngẫu nhiên tàu bay
        char bufDi[50], bufDen[50];
        strftime(bufDi, sizeof(bufDi), "%d/%m/%Y %H:%M", localtime(&tDi));
        cbDi.thoiGianDi = std::string(bufDi);
        time_t tDen = tDi + 3600 * gioBay;
        strftime(bufDen, sizeof(bufDen), "%d/%m/%Y %H:%M", localtime(&tDen));
        cbDi.thoiGianDen = std::string(bufDen);
        cbDi.giaNiemYet = (15 + rand() % 30) * 100000.0;
        if (gioBay > 4) cbDi.giaNiemYet *= 2;
        khoChuyenBayToanCuc.them(cbDi);

        ChuyenBay cbVe;
        cbVe.hangBay = hk;
        cbVe.maCB = hk.maHang + std::to_string(100 + rand() % 900);
        cbVe.sbDi = sb2;
        cbVe.sbDen = sb1;
        cbVe.dongMayBay = mayBayNgauNhien; // Chuyến khứ hồi dùng chung tàu bay
        time_t tVeDi = tDen + 3600 * 24 * (1 + rand() % 4);
        strftime(bufDi, sizeof(bufDi), "%d/%m/%Y %H:%M", localtime(&tVeDi));
        cbVe.thoiGianDi = std::string(bufDi);
        time_t tVeDen = tVeDi + 3600 * gioBay;
        strftime(bufDen, sizeof(bufDen), "%d/%m/%Y %H:%M", localtime(&tVeDen));
        cbVe.thoiGianDen = std::string(bufDen);
        cbVe.giaNiemYet = cbDi.giaNiemYet;
        khoChuyenBayToanCuc.them(cbVe);
        };

    // Vòng lặp sinh chuyến bay mà lần trước bị thiếu
    for (int i = 0; i < 2; i++) {
        SanBay sb1 = sbNoiDia[rand() % sbNoiDia.size()];
        SanBay sb2;
        int maxTry = 0;
        do { sb2 = sbNoiDia[rand() % sbNoiDia.size()]; maxTry++; } while (sb1.iata == sb2.iata && maxTry < 10);
        if (sb1.iata != sb2.iata) {
            HangHangKhong hk = dsHK[rand() % dsHK.size()];
            TaoCapChuyenBay(sb1, sb2, hk, 2);
        }
    }

    for (int i = 0; i < 3; i++) {
        SanBay sb1 = sbNoiDia[rand() % sbNoiDia.size()];
        SanBay sb2 = sbQuocTe[rand() % sbQuocTe.size()];
        HangHangKhong hk = dsHK[rand() % dsHK.size()];
        TaoCapChuyenBay(sb1, sb2, hk, 5 + rand() % 4);
    }
}
void KhoiTaoDuLieuHeThong() {
    danhSachNhanSu.them(NhanVien("ADMIN001QL", u8"LÂM PHI VŨ", "123456", u8"Quản trị viên", u8"Ban Quản Lý"));
    danhSachNhanSu.them(NhanVien("NV001SB", u8"MAI ĐOAN KHÁNH THẠCH", "123"));
    danhSachNhanSu.them(NhanVien("NV002SB", u8"BÙI QUANG THOẠI", "123"));
    danhSachNhanSu.them(NhanVien("NV003SB", u8"LƯƠNG NGỌC TUẤN", "123"));

    time_t now = time(0); struct tm* ti = localtime(&now); char bufBD[20], bufHH1[20];
    strftime(bufBD, sizeof(bufBD), "%d/%m/%Y", ti); ti->tm_mon += 6; mktime(ti);
    strftime(bufHH1, sizeof(bufHH1), "%d/%m/%Y", ti);

    MaGiamGia m1; m1.maMGG = "CODUNGXINHDEP"; m1.phanTramGiam = 50.0;
    m1.ngayBatDau = std::string(bufBD); m1.gioBatDau = "00:00";
    m1.ngayHetHan = std::string(bufHH1); m1.gioHetHan = "23:59";
    m1.tongSoLuong = 100; m1.soLuongDaDung = 0;
    m1.loaiVeApDung = u8"Tất cả"; m1.moTa = u8"Mã đặc biệt giảm 50% cho tất cả hạng vé"; m1.dangHoatDong = true;
    heThongMaGiamGia.them(m1);

    Tao5ChuyenBayMacDinh();
}

// ========================== ENGINE VẼ BIỂU ĐỒ ==========================
class TrinhVeBieuDo {
public:
    static void VeBieuDoCotCoBan(Graphics^ g, int w, int h, double dtY, double dtC, double dtF, double dtHH) {
        g->SmoothingMode = SmoothingMode::AntiAlias; g->Clear(Color::WhiteSmoke);
        Pen^ axisPen = gcnew Pen(Color::Black, 2); int baseX = 50, baseY = h - 50;
        g->DrawLine(axisPen, baseX, baseY, w - 20, baseY); g->DrawLine(axisPen, baseX, 40, baseX, baseY);
        double maxVal = Math::Max(Math::Max(dtY, dtC), Math::Max(dtF, dtHH)); if (maxVal == 0) maxVal = 1;
        array<double>^ values = { dtY, dtC, dtF, dtHH }; array<String^>^ labels = { L"Phổ Thông", L"Thương Gia", L"Hạng Nhất", L"Hàng Hóa" };
        array<Brush^>^ colors = { Brushes::SteelBlue, Brushes::Goldenrod, Brushes::Purple, Brushes::MediumSeaGreen };

        for (int i = 0; i < 4; i++) {
            int barHeight = (int)((values[i] / maxVal) * (h - 100)); int barX = baseX + 70 + i * 150, barY = baseY - barHeight;
            g->FillRectangle(colors[i], barX, barY, 80, barHeight);
            g->DrawString(labels[i], gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold), Brushes::Black, barX - 5, baseY + 10);
            String^ valStr = (values[i] > 0) ? (values[i] / 1000000.0).ToString("0.0") + L" Tr" : L"0";
            g->DrawString(valStr, gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold), Brushes::DarkRed, barX, barY - 20);
        }
        g->DrawString(L"BIỂU ĐỒ TỔNG DOANH THU CƠ BẢN (Triệu VND)", gcnew Drawing::Font("Segoe UI", 12, FontStyle::Bold), Brushes::Navy, 220, 10);
    }

    static void VeBieuDoDoanhThuHangBay(Graphics^ g, int w, int h, std::map<std::string, double>& thongKeHang) {
        g->SmoothingMode = SmoothingMode::AntiAlias; g->Clear(Color::AliceBlue);
        Pen^ axisPen = gcnew Pen(Color::Black, 2); int baseX = 50, baseY = h - 50;
        g->DrawLine(axisPen, baseX, baseY, w - 20, baseY); g->DrawLine(axisPen, baseX, 40, baseX, baseY);
        double maxVal = 1; for (auto const& pair : thongKeHang) if (pair.second > maxVal) maxVal = pair.second;
        int i = 0, barWidth = 60, spacing = (w - 100) / (thongKeHang.size() == 0 ? 1 : thongKeHang.size());
        for (auto const& pair : thongKeHang) {
            int barHeight = (int)((pair.second / maxVal) * (h - 100)); int barX = baseX + 30 + i * spacing, barY = baseY - barHeight;
            System::Drawing::Rectangle rectBox(barX, barY, barWidth, barHeight);
            LinearGradientBrush^ lgb = gcnew LinearGradientBrush(rectBox, Color::DarkBlue, Color::LightBlue, 90.0f);
            g->FillRectangle(lgb, barX, barY, barWidth, barHeight);
            g->DrawString(StrToSys(pair.first), gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold), Brushes::Black, barX, baseY + 10);
            String^ valStr = (pair.second / 1000000.0).ToString("0.0") + L" Tr";
            g->DrawString(valStr, gcnew Drawing::Font("Segoe UI", 8, FontStyle::Bold), Brushes::DarkRed, barX - 5, barY - 20);
            i++;
        }
        g->DrawString(L"PHÂN TÍCH DOANH THU THEO HÃNG HÀNG KHÔNG (Triệu VND)", gcnew Drawing::Font("Segoe UI", 12, FontStyle::Bold), Brushes::DarkCyan, 180, 10);
    }

    static void VeBieuDoTronTrangThaiVe(Graphics^ g, int w, int h, int veThanhCong, int veDaHuy) {
        g->SmoothingMode = SmoothingMode::AntiAlias; g->Clear(Color::GhostWhite);
        int total = veThanhCong + veDaHuy;
        if (total == 0) { g->DrawString(L"Chưa có dữ liệu vé", gcnew Drawing::Font("Segoe UI", 14), Brushes::Gray, w / 2 - 80, h / 2); return; }
        float pctThanhCong = (float)veThanhCong / total * 360.0f; float pctHuy = 360.0f - pctThanhCong;
        System::Drawing::Rectangle rect(w / 2 - 120, h / 2 - 100, 200, 200);
        g->FillPie(Brushes::SeaGreen, rect, 0, pctThanhCong); g->FillPie(Brushes::Crimson, rect, pctThanhCong, pctHuy);
        g->FillRectangle(Brushes::SeaGreen, w - 200, h / 2 - 20, 20, 20); g->DrawString(L"Vé Thành Công: " + veThanhCong.ToString(), gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold), Brushes::Black, w - 170, h / 2 - 20);
        g->FillRectangle(Brushes::Crimson, w - 200, h / 2 + 10, 20, 20); g->DrawString(L"Vé Đã Hủy: " + veDaHuy.ToString(), gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold), Brushes::Black, w - 170, h / 2 + 10);
        g->DrawString(L"TỶ LỆ TRẠNG THÁI VÉ", gcnew Drawing::Font("Segoe UI", 12, FontStyle::Bold), Brushes::Indigo, w / 2 - 80, 20);
    }
};

// ========================== FORM ĐĂNG NHẬP ==========================
public ref class LoginForm : public Form {
public:
    TextBox^ txtUser; TextBox^ txtPass;
private:
    Label^ MakeLabel(String^ text, int x, int y) {
        Label^ lbl = gcnew Label(); lbl->Text = text; lbl->Location = Point(x, y); lbl->AutoSize = true; lbl->ForeColor = Color::FromArgb(180, 200, 255); lbl->Font = gcnew Drawing::Font("Segoe UI", 10); return lbl;
    }
public:
    LoginForm() {
        this->Text = L"UTC2 Airlines — Đăng Nhập Nội Bộ"; this->Size = Drawing::Size(480, 340); this->StartPosition = FormStartPosition::CenterScreen; this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle; this->MaximizeBox = false; this->BackColor = Color::FromArgb(12, 20, 50);
        Panel^ pnlHeader = gcnew Panel(); pnlHeader->Dock = DockStyle::Top; pnlHeader->Height = 90; pnlHeader->BackColor = Color::FromArgb(18, 30, 75);
        Label^ lblIcon = gcnew Label(); lblIcon->Text = L"✈"; lblIcon->Font = gcnew Drawing::Font("Segoe UI", 28); lblIcon->ForeColor = Color::Gold; lblIcon->Location = Point(30, 20); lblIcon->AutoSize = true;

        Label^ lblTitle = gcnew Label(); lblTitle->Text = L"UTC2 AIRLINES"; lblTitle->Font = gcnew Drawing::Font("Segoe UI", 18, FontStyle::Bold); lblTitle->ForeColor = Color::White; lblTitle->Location = Point(100, 15); lblTitle->AutoSize = true;
        Label^ lblSub = gcnew Label(); lblSub->Text = L"HỆ THỐNG QUẢN LÝ NỘI BỘ"; lblSub->Font = gcnew Drawing::Font("Segoe UI", 9); lblSub->ForeColor = Color::FromArgb(180, 200, 255); lblSub->Location = Point(102, 52); lblSub->AutoSize = true;

        pnlHeader->Controls->Add(lblIcon); pnlHeader->Controls->Add(lblTitle); pnlHeader->Controls->Add(lblSub);

        Panel^ pnlBody = gcnew Panel(); pnlBody->Location = Point(0, 90); pnlBody->Size = Drawing::Size(480, 250); pnlBody->BackColor = Color::FromArgb(22, 35, 80);
        pnlBody->Controls->Add(MakeLabel(L"Tài khoản", 60, 30));
        txtUser = gcnew TextBox(); txtUser->Location = Point(60, 55); txtUser->Width = 340; txtUser->Font = gcnew Drawing::Font("Segoe UI", 11); txtUser->BackColor = Color::FromArgb(35, 55, 110); txtUser->ForeColor = Color::White; txtUser->BorderStyle = BorderStyle::FixedSingle; pnlBody->Controls->Add(txtUser);
        pnlBody->Controls->Add(MakeLabel(L"Mật khẩu", 60, 95));
        txtPass = gcnew TextBox(); txtPass->Location = Point(60, 120); txtPass->Width = 340; txtPass->PasswordChar = '●'; txtPass->Font = gcnew Drawing::Font("Segoe UI", 11); txtPass->BackColor = Color::FromArgb(35, 55, 110); txtPass->ForeColor = Color::White; txtPass->BorderStyle = BorderStyle::FixedSingle; txtPass->KeyPress += gcnew KeyPressEventHandler(this, &LoginForm::OnEnterKey); pnlBody->Controls->Add(txtPass);

        Button^ btnLogin = gcnew Button(); btnLogin->Text = L"ĐĂNG NHẬP"; btnLogin->Location = Point(60, 170); btnLogin->Size = Drawing::Size(340, 45); btnLogin->BackColor = Color::Gold; btnLogin->ForeColor = Color::FromArgb(12, 20, 50); btnLogin->FlatStyle = FlatStyle::Flat; btnLogin->Font = gcnew Drawing::Font("Segoe UI", 12, FontStyle::Bold); btnLogin->FlatAppearance->BorderSize = 0; btnLogin->Click += gcnew EventHandler(this, &LoginForm::BtnLogin_Click); pnlBody->Controls->Add(btnLogin);
        this->Controls->Add(pnlHeader); this->Controls->Add(pnlBody);
    }
    void OnEnterKey(Object^ sender, KeyPressEventArgs^ e) { if (e->KeyChar == (char)13) BtnLogin_Click(sender, e); }
    void BtnLogin_Click(Object^ sender, EventArgs^ e) {
        std::string usr = SysToStr(txtUser->Text);
        std::string pwd = SysToStr(txtPass->Text);

        for (size_t i = 0; i < danhSachNhanSu.kichThuoc(); i++) {
            if (danhSachNhanSu[i].layMaNV() == usr && danhSachNhanSu[i].layMatKhau() == pwd) {
                if (!danhSachNhanSu[i].layTrangThai()) {
                    MessageBox::Show(L"Tài khoản của bạn đã bị khóa! Vui lòng liên hệ Admin.", L"Từ chối truy cập", MessageBoxButtons::OK, MessageBoxIcon::Error);
                    return;
                }
                nhanVienHienTai = &danhSachNhanSu[i];
                userIsAdmin = (usr == "ADMIN001QL");
                MessageBox::Show(L"XIN CHÀO " + StrToSys(nhanVienHienTai->layHoTen()) + L"!\nCHÚC NGÀY TỐT LÀNH VÀ LÀM VIỆC HIỆU QUẢ.", L"Chào Mừng", MessageBoxButtons::OK, MessageBoxIcon::Information);
                this->DialogResult = System::Windows::Forms::DialogResult::OK;
                this->Close();
                return;
            }
        }
        MessageBox::Show(L"Tài khoản hoặc mật khẩu không chính xác!", L"Lỗi Đăng Nhập", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
};

// ========================== FORM ĐỔI CHỖ NGỒI ==========================
public ref class FormDoiCho : public Form {
public:
    TextBox^ txtGheMoi;
    FormDoiCho(String^ gheHienTai) {
        this->Text = L"Đổi Chỗ Ngồi"; this->Size = Drawing::Size(300, 150); this->StartPosition = FormStartPosition::CenterParent; this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog; this->MaximizeBox = false;
        Label^ lbl = gcnew Label(); lbl->Text = L"Nhập mã ghế mới (VD: 12A):"; lbl->Location = Point(20, 20); lbl->AutoSize = true;
        txtGheMoi = gcnew TextBox(); txtGheMoi->Location = Point(20, 45); txtGheMoi->Width = 240; txtGheMoi->Text = gheHienTai; txtGheMoi->CharacterCasing = CharacterCasing::Upper;
        Button^ btnOK = gcnew Button(); btnOK->Text = L"XÁC NHẬN"; btnOK->Location = Point(20, 80); btnOK->Size = Drawing::Size(110, 30); btnOK->DialogResult = System::Windows::Forms::DialogResult::OK;
        Button^ btnCancel = gcnew Button(); btnCancel->Text = L"HỦY"; btnCancel->Location = Point(150, 80); btnCancel->Size = Drawing::Size(110, 30); btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
        this->Controls->Add(lbl); this->Controls->Add(txtGheMoi); this->Controls->Add(btnOK); this->Controls->Add(btnCancel);
    }
};

// ========================== FORM CHÍNH ==========================
public ref class MainForm : public Form {
private:
    double maGiamGiaHienTai;
    String^ tenMGGDangDung;

    TabControl^ tabCtrl;
    TabPage^ tabDatVe, ^ tabDanhSach, ^ tabHangHoa, ^ tabThongKe;
    TabPage^ tabQuanLyCB, ^ tabDichVuUM, ^ tabHeThong, ^ tabQuanLyMGG, ^ tabNhanSu;

    // Booking tab controls
    ComboBox^ cbSanBayDi, ^ cbSanBayDen; RadioButton^ rbMotChieu, ^ rbHaiChieu;
    DateTimePicker^ dtpNgayBay, ^ dtpNgayVe; Label^ lblNgayVe; Button^ btnTimChuyenBay; DataGridView^ gridChuyenBay;
    TextBox^ txtHoTen; ComboBox^ cbQuocTich; TextBox^ txtCCCD; Label^ lblThongTinCCCD;
    TextBox^ txtHoChieu, ^ txtSDT, ^ txtEmail; ComboBox^ cbHangVe; TextBox^ txtKyGui, ^ txtXachTay;
    Label^ lblHMDinhMuc; TextBox^ txtMaGiamGia; Button^ btnApDungMGG; Label^ lblKetQuaMGG; Button^ btnDatVe;

    // Ticket list tab
    DataGridView^ gridVe; TextBox^ txtTimKiem; Button^ btnTimKiem, ^ btnHuyVe, ^ btnXoaVeHuy, ^ btnDoiCho; ComboBox^ cbLyDoHuy;

    // Cargo tab
    TextBox^ txtNguoiGuiHH; ComboBox^ cbSBDiHH, ^ cbSBDenHH, ^ cbLoaiKhachHH, ^ cbLoaiHangHH, ^ cbTocDoGiaoHH;
    TextBox^ txtKhoiLuongHH; Button^ btnTaoPhieuHH, ^ btnHuyPhieuHH; DataGridView^ gridHangHoa;

    // Stats tab
    DataGridView^ gridThongKe; Label^ lblTongDoanhThu, ^ lblDoanhThuHangHoa; Button^ btnLuuDuLieu, ^ btnXuatDoanhThuHangBay;
    PictureBox^ picBieuDo; ComboBox^ cbLoaiBieuDo; std::vector<ChuyenBay>* dsChuyenBayHienTai;

    // Admin flight plan tab
    TextBox^ txtQ_MaCB, ^ txtQ_MayBay, ^ txtQ_Gia; ComboBox^ cbQ_HangBay, ^ cbQ_SBDi, ^ cbQ_SBDen;
    DateTimePicker^ dtpQ_NgayDi, ^ dtpQ_GioDi, ^ dtpQ_NgayDen, ^ dtpQ_GioDen;
    Button^ btnQ_Them, ^ btnQ_Sua, ^ btnQ_Xoa, ^ btnQ_CapNhat, ^ btnQ_ThemTuDong;
    DataGridView^ gridQuanLyCB;

    // UM service tab
    TextBox^ txtTimPNR_UM; Button^ btnTimUM, ^ btnDangKyUM; Label^ lblThongTinUM;

    // System tab
    Label^ lblQuyenHienTai, ^ lblHoSoNV; Button^ btnDoiQuyen;
    Timer^ tmrMarquee; Label^ lblMarquee; Panel^ pnlMarquee; PictureBox^ picLogo;

    // Admin Discount Code tab
    TextBox^ txtMGG_Ma, ^ txtMGG_PhanTram, ^ txtMGG_MoTa, ^ txtMGG_SoLuong;
    DateTimePicker^ dtpMGG_NgayBD, ^ dtpMGG_NgayHH, ^ dtpMGG_GioBD, ^ dtpMGG_GioHH; ComboBox^ cbMGG_LoaiVe;
    Button^ btnMGG_Them, ^ btnMGG_Xoa, ^ btnMGG_Toggle; DataGridView^ gridMGG; Label^ lblMGGStats;

    // Admin HR tab
    DataGridView^ gridNhanSu; Button^ btnKhoaMoNV;

    // ========================== UI HELPER METHODS ==========================
    Label^ TaoLabel(String^ text, int x, int y, bool isBold) {
        Label^ lbl = gcnew Label(); lbl->Text = text; lbl->Location = Point(x, y); lbl->AutoSize = true;
        if (isBold) lbl->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold); return lbl;
    }
    TextBox^ TaoTextBox(int x, int y, int w) { TextBox^ txt = gcnew TextBox(); txt->Location = Point(x, y); txt->Width = w; return txt; }
    ComboBox^ TaoComboBox(int x, int y, int w) { ComboBox^ cb = gcnew ComboBox(); cb->Location = Point(x, y); cb->Width = w; cb->DropDownStyle = ComboBoxStyle::DropDownList; return cb; }
    void AddPair(Panel^ pnl, String^ lblText, Control^ ctrl, int y) { pnl->Controls->Add(TaoLabel(lblText, 12, y, false)); ctrl->Location = Point(150, y - 3); pnl->Controls->Add(ctrl); }

    Panel^ TaoAdminHeaderPanel(String^ icon, String^ title, Color bgColor) {
        Panel^ pnl = gcnew Panel(); pnl->Location = Point(0, 0); pnl->Size = Drawing::Size(1340, 58); pnl->BackColor = bgColor;
        Panel^ accent = gcnew Panel(); accent->Location = Point(0, 0); accent->Size = Drawing::Size(6, 58); accent->BackColor = Color::Gold; pnl->Controls->Add(accent);
        Label^ lblIcon = gcnew Label(); lblIcon->Text = icon; lblIcon->Location = Point(20, 10); lblIcon->AutoSize = true; lblIcon->Font = gcnew Drawing::Font("Segoe UI", 20); lblIcon->ForeColor = Color::Gold; pnl->Controls->Add(lblIcon);
        Label^ lblTitle = gcnew Label(); lblTitle->Text = title; lblTitle->Location = Point(65, 12); lblTitle->AutoSize = true; lblTitle->Font = gcnew Drawing::Font("Segoe UI", 14, FontStyle::Bold); lblTitle->ForeColor = Color::White; pnl->Controls->Add(lblTitle);
        return pnl;
    }
    Button^ TaoAdminButton(String^ text, int x, int y, int w, int h, Color bg) {
        Button^ btn = gcnew Button(); btn->Text = text; btn->Location = Point(x, y); btn->Size = Drawing::Size(w, h); btn->BackColor = bg; btn->ForeColor = Color::White; btn->FlatStyle = FlatStyle::Flat; btn->FlatAppearance->BorderSize = 0; btn->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold); btn->Cursor = Cursors::Hand; return btn;
    }

    bool KiemTraVaLayMGG(const std::string& ma, const std::string& loaiVe, double& phanTram, std::string& moTa) {
        DateTime now = DateTime::Now; std::string maUpper = ma; std::transform(maUpper.begin(), maUpper.end(), maUpper.begin(), ::toupper);
        for (size_t i = 0; i < heThongMaGiamGia.kichThuoc(); i++) {
            MaGiamGia& mgg = heThongMaGiamGia[i];
            if (!mgg.dangHoatDong) continue;
            std::string maMGGUp = mgg.maMGG; std::transform(maMGGUp.begin(), maMGGUp.end(), maMGGUp.begin(), ::toupper);
            if (maMGGUp != maUpper) continue;
            try {
                DateTime dtStart = DateTime::ParseExact(StrToSys(mgg.ngayBatDau + " " + mgg.gioBatDau), "dd/MM/yyyy HH:mm", System::Globalization::CultureInfo::InvariantCulture);
                DateTime dtEnd = DateTime::ParseExact(StrToSys(mgg.ngayHetHan + " " + mgg.gioHetHan), "dd/MM/yyyy HH:mm", System::Globalization::CultureInfo::InvariantCulture);
                if (now < dtStart || now > dtEnd) continue;
            }
            catch (...) { continue; }
            if (mgg.soLuongDaDung >= mgg.tongSoLuong) continue;
            if (mgg.loaiVeApDung == u8"Tất cả" || mgg.loaiVeApDung == loaiVe || mgg.loaiVeApDung == "Tat ca") {
                phanTram = mgg.phanTramGiam; moTa = mgg.moTa; return true;
            }
        }
        return false;
    }

    void FormatGridWithSTT(DataGridView^ grid) {
        grid->AllowUserToAddRows = false; grid->ReadOnly = true;
        grid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
        grid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
        grid->AlternatingRowsDefaultCellStyle->BackColor = Color::FromArgb(245, 245, 245);
        grid->EnableHeadersVisualStyles = false;
        grid->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(35, 55, 110);
        grid->ColumnHeadersDefaultCellStyle->ForeColor = Color::White;

        DataGridViewTextBoxColumn^ colSTT = gcnew DataGridViewTextBoxColumn();
        colSTT->Name = L"STT"; colSTT->HeaderText = L"STT"; colSTT->Width = 45; colSTT->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        colSTT->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        grid->Columns->Insert(0, colSTT);
    }

    void CapNhatGiaoDienTheoQuyen() {
        this->Text = userIsAdmin ? L"UTC2 Airlines — HỆ THỐNG QUẢN LÝ  [QUYỀN QUẢN TRỊ VIÊN]" : L"UTC2 Airlines — HỆ THỐNG QUẢN LÝ  [NHÂN VIÊN]";
        lblQuyenHienTai->Text = userIsAdmin ? L"Quyền hạn: ADMIN - QUẢN TRỊ VIÊN" : L"Quyền hạn: NHÂN VIÊN - " + StrToSys(nhanVienHienTai->layChucVu());
        lblQuyenHienTai->ForeColor = userIsAdmin ? Color::Crimson : Color::SeaGreen;

        tabCtrl->TabPages->Clear();

        if (!userIsAdmin) {
            tabCtrl->TabPages->Add(tabDatVe);
            tabCtrl->TabPages->Add(tabDichVuUM);
            tabCtrl->TabPages->Add(tabHangHoa);
        }

        tabCtrl->TabPages->Add(tabDanhSach);

        if (userIsAdmin) {
            tabCtrl->TabPages->Add(tabQuanLyCB);
            tabCtrl->TabPages->Add(tabQuanLyMGG);
            tabCtrl->TabPages->Add(tabNhanSu);
        }

        tabCtrl->TabPages->Add(tabThongKe);
        tabCtrl->TabPages->Add(tabHeThong);

        btnXoaVeHuy->Enabled = userIsAdmin; btnXoaVeHuy->Visible = userIsAdmin; cbLyDoHuy->Visible = userIsAdmin;
        cbLoaiBieuDo->Visible = userIsAdmin; btnXuatDoanhThuHangBay->Visible = userIsAdmin;

        if (userIsAdmin) { LoadKhoChuyenBayVaoGrid(); LoadMGGVaoGrid(); LoadNhanSuVaoGrid(); }

        lblHoSoNV->Text = L"HỒ SƠ CÁ NHÂN:\n- Mã NV: " + StrToSys(nhanVienHienTai->layMaNV()) + L"\n- Họ Tên: " + StrToSys(nhanVienHienTai->layHoTen()) + L"\n- Chức Vụ: " + StrToSys(nhanVienHienTai->layChucVu()) + L"\n- Phòng Ban: " + StrToSys(nhanVienHienTai->layPhongBan()) + L"\n- Khách đã phục vụ: " + nhanVienHienTai->laySoKhachPhucVu().ToString();
    }

    void KhoiTaoUI() {
        this->Size = Drawing::Size(1350, 850); this->StartPosition = FormStartPosition::CenterScreen; this->Font = gcnew Drawing::Font("Segoe UI", 10);
        dsChuyenBayHienTai = new std::vector<ChuyenBay>();

        tabCtrl = gcnew TabControl(); tabCtrl->Dock = DockStyle::Fill;
        tabDatVe = gcnew TabPage(L"✈ Đặt Vé Mới"); tabDanhSach = gcnew TabPage(L"📋 Quản Lý Vé"); tabHangHoa = gcnew TabPage(L"📦 Hàng Hóa");
        tabDichVuUM = gcnew TabPage(L"👶 Dịch Vụ UM"); tabThongKe = gcnew TabPage(L"📊 Thống Kê");
        tabQuanLyCB = gcnew TabPage(L"🗓 Kế Hoạch Bay"); tabQuanLyMGG = gcnew TabPage(L"🎫 Mã Giảm Giá"); tabNhanSu = gcnew TabPage(L"👨‍💼 Nhân Sự"); tabHeThong = gcnew TabPage(L"⚙ Hệ Thống");

        // ======================= TAB ĐẶT VÉ =======================
        GroupBox^ gbTimKiem = gcnew GroupBox(); gbTimKiem->Text = L"1. TÌM KIẾM CHUYẾN BAY"; gbTimKiem->Location = Point(20, 20); gbTimKiem->Size = Drawing::Size(1280, 250); tabDatVe->Controls->Add(gbTimKiem);

        // Căn chỉnh lại Row 1: Sân bay và Loại vé
        gbTimKiem->Controls->Add(TaoLabel(L"Sân Bay Đi:", 20, 30, false)); cbSanBayDi = TaoComboBox(130, 30, 200); gbTimKiem->Controls->Add(cbSanBayDi);
        gbTimKiem->Controls->Add(TaoLabel(L"Sân Bay Đến:", 350, 30, false)); cbSanBayDen = TaoComboBox(460, 30, 200); gbTimKiem->Controls->Add(cbSanBayDen);
        rbMotChieu = gcnew RadioButton(); rbMotChieu->Text = L"Một chiều"; rbMotChieu->Location = Point(680, 28); rbMotChieu->Checked = true; rbMotChieu->AutoSize = true;
        rbHaiChieu = gcnew RadioButton(); rbHaiChieu->Text = L"Khứ hồi (2 chiều)"; rbHaiChieu->Location = Point(800, 28); rbHaiChieu->AutoSize = true; rbHaiChieu->CheckedChanged += gcnew EventHandler(this, &MainForm::XuLyChonLoaiVe);
        gbTimKiem->Controls->Add(rbMotChieu); gbTimKiem->Controls->Add(rbHaiChieu);

        // Căn chỉnh lại Row 2: FIX LỖI GIAO DIỆN XỆ NGÀY VỀ
        gbTimKiem->Controls->Add(TaoLabel(L"Ngày Đi:", 680, 60, false));
        dtpNgayBay = gcnew DateTimePicker(); dtpNgayBay->Location = Point(750, 60); dtpNgayBay->Width = 130; dtpNgayBay->Format = DateTimePickerFormat::Custom; dtpNgayBay->CustomFormat = L"dd/MM/yyyy"; dtpNgayBay->MinDate = System::DateTime::Now; dtpNgayBay->ValueChanged += gcnew EventHandler(this, &MainForm::XuLyDoiNgayBay); gbTimKiem->Controls->Add(dtpNgayBay);

        lblNgayVe = TaoLabel(L"Ngày Về:", 900, 60, false); lblNgayVe->Visible = false; gbTimKiem->Controls->Add(lblNgayVe);
        dtpNgayVe = gcnew DateTimePicker(); dtpNgayVe->Location = Point(970, 60); dtpNgayVe->Width = 130; dtpNgayVe->Format = DateTimePickerFormat::Custom; dtpNgayVe->CustomFormat = L"dd/MM/yyyy"; dtpNgayVe->MinDate = System::DateTime::Now; dtpNgayVe->Visible = false; gbTimKiem->Controls->Add(dtpNgayVe);

        btnTimChuyenBay = gcnew Button(); btnTimChuyenBay->Text = L"TÌM CHUYẾN BAY"; btnTimChuyenBay->Location = Point(1115, 58); btnTimChuyenBay->Size = Drawing::Size(145, 30); btnTimChuyenBay->BackColor = Color::DarkOrange; btnTimChuyenBay->ForeColor = Color::White; btnTimChuyenBay->FlatStyle = FlatStyle::Flat; btnTimChuyenBay->Click += gcnew EventHandler(this, &MainForm::XuLyTimChuyenBay); gbTimKiem->Controls->Add(btnTimChuyenBay);

        gridChuyenBay = gcnew DataGridView();
        gridChuyenBay->ColumnCount = 6;
        gridChuyenBay->Columns[0]->Name = L"Hãng Khai Thác"; gridChuyenBay->Columns[1]->Name = L"Số Hiệu"; gridChuyenBay->Columns[2]->Name = L"Dòng Máy Bay"; gridChuyenBay->Columns[3]->Name = L"Khởi Hành"; gridChuyenBay->Columns[4]->Name = L"Hạ Cánh Dự Kiến"; gridChuyenBay->Columns[5]->Name = L"Giá Cơ Bản (VND)";
        FormatGridWithSTT(gridChuyenBay);
        gridChuyenBay->Location = Point(20, 100); gridChuyenBay->Size = Drawing::Size(1240, 130);
        gbTimKiem->Controls->Add(gridChuyenBay);

        GroupBox^ gbHanhKhach = gcnew GroupBox(); gbHanhKhach->Text = L"2. THÔNG TIN HÀNH KHÁCH"; gbHanhKhach->Location = Point(20, 280); gbHanhKhach->Size = Drawing::Size(620, 480); tabDatVe->Controls->Add(gbHanhKhach);
        int inputX = 160, pStepY = 40;
        gbHanhKhach->Controls->Add(TaoLabel(L"Họ và Tên (*):", 20, 40, false)); txtHoTen = TaoTextBox(inputX, 40, 250); gbHanhKhach->Controls->Add(txtHoTen);
        gbHanhKhach->Controls->Add(TaoLabel(L"Quốc Tịch (*):", 20, 40 + pStepY, false)); cbQuocTich = TaoComboBox(inputX, 40 + pStepY, 250);
        std::vector<QuocGia> dsQG = DuLieuQuocGia::layTheHien()->layDanhSach();
        for (size_t i = 0; i < dsQG.size(); i++) cbQuocTich->Items->Add(StrToSys(dsQG[i].ten));
        cbQuocTich->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::XuLyChonQuocGia); gbHanhKhach->Controls->Add(cbQuocTich);
        gbHanhKhach->Controls->Add(TaoLabel(L"Số CCCD (VN):", 20, 40 + pStepY * 2, false)); txtCCCD = TaoTextBox(inputX, 40 + pStepY * 2, 250); txtCCCD->MaxLength = 12; txtCCCD->TextChanged += gcnew EventHandler(this, &MainForm::XuLyTruyXuatCCCD_RealTime); gbHanhKhach->Controls->Add(txtCCCD);
        lblThongTinCCCD = TaoLabel(L"", 20, 40 + pStepY * 3 - 10, false); lblThongTinCCCD->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Italic); gbHanhKhach->Controls->Add(lblThongTinCCCD);
        gbHanhKhach->Controls->Add(TaoLabel(L"Hộ Chiếu (Khác):", 20, 40 + pStepY * 3 + 10, false)); txtHoChieu = TaoTextBox(inputX, 40 + pStepY * 3 + 10, 250); gbHanhKhach->Controls->Add(txtHoChieu);
        gbHanhKhach->Controls->Add(TaoLabel(L"Số ĐT (*):", 20, 40 + pStepY * 4 + 10, false)); txtSDT = TaoTextBox(inputX, 40 + pStepY * 4 + 10, 250); gbHanhKhach->Controls->Add(txtSDT);

        // ---- TextBox Email nhận vé ----
        gbHanhKhach->Controls->Add(TaoLabel(L"Email nhận vé:", 20, 40 + pStepY * 5 + 10, false));
        txtEmail = TaoTextBox(inputX, 40 + pStepY * 5 + 10, 250);
        gbHanhKhach->Controls->Add(txtEmail);

        GroupBox^ gbDichVu = gcnew GroupBox(); gbDichVu->Text = L"3. DỊCH VỤ BỔ SUNG & THANH TOÁN"; gbDichVu->Location = Point(660, 280); gbDichVu->Size = Drawing::Size(640, 480); tabDatVe->Controls->Add(gbDichVu);
        gbDichVu->Controls->Add(TaoLabel(L"Hạng Vé:", 20, 40, false)); cbHangVe = TaoComboBox(160, 40, 200); cbHangVe->Items->AddRange(gcnew cli::array<String^>{L"Economy", L"Business", L"First Class"}); cbHangVe->SelectedIndex = 0; cbHangVe->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::CapNhatHinhMucHanhLy); gbDichVu->Controls->Add(cbHangVe);
        lblHMDinhMuc = TaoLabel(L"Định mức: 23kg KG / 7kg XT", 380, 40, false); lblHMDinhMuc->ForeColor = Color::Blue; gbDichVu->Controls->Add(lblHMDinhMuc);
        gbDichVu->Controls->Add(TaoLabel(L"Ký Gửi (Kg):", 20, 80, false)); txtKyGui = TaoTextBox(160, 80, 200); txtKyGui->Text = L"23"; gbDichVu->Controls->Add(txtKyGui);
        gbDichVu->Controls->Add(TaoLabel(L"Xách Tay (Kg):", 20, 120, false)); txtXachTay = TaoTextBox(160, 120, 200); txtXachTay->Text = L"7"; gbDichVu->Controls->Add(txtXachTay);
        gbDichVu->Controls->Add(TaoLabel(L"Mã Giảm Giá:", 20, 160, false)); txtMaGiamGia = TaoTextBox(160, 160, 120); txtMaGiamGia->CharacterCasing = CharacterCasing::Upper; txtMaGiamGia->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold); txtMaGiamGia->ForeColor = Color::DarkViolet; gbDichVu->Controls->Add(txtMaGiamGia);
        btnApDungMGG = gcnew Button(); btnApDungMGG->Text = L"ÁP DỤNG"; btnApDungMGG->Location = Point(285, 158); btnApDungMGG->Size = Drawing::Size(75, 28); btnApDungMGG->BackColor = Color::FromArgb(110, 0, 160); btnApDungMGG->ForeColor = Color::White; btnApDungMGG->FlatStyle = FlatStyle::Flat; btnApDungMGG->Click += gcnew EventHandler(this, &MainForm::XuLyApDungMaGiam); gbDichVu->Controls->Add(btnApDungMGG);
        lblKetQuaMGG = TaoLabel(L"", 160, 190, false); lblKetQuaMGG->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold | FontStyle::Italic); gbDichVu->Controls->Add(lblKetQuaMGG);

        btnDatVe = gcnew Button(); btnDatVe->Text = L"XÁC NHẬN ĐẶT VÉ (Tự động cấp ghế & Email)"; btnDatVe->Location = Point(160, 250); btnDatVe->Size = Drawing::Size(360, 60); btnDatVe->BackColor = Color::SteelBlue; btnDatVe->ForeColor = Color::White; btnDatVe->FlatStyle = FlatStyle::Flat; btnDatVe->Font = gcnew Drawing::Font("Segoe UI", 12, FontStyle::Bold); btnDatVe->Click += gcnew EventHandler(this, &MainForm::XuLyDatVe); gbDichVu->Controls->Add(btnDatVe);

        std::vector<SanBay> dsSB = DuLieuSanBay::layTheHien()->layDanhSach();
        for (size_t i = 0; i < dsSB.size(); i++) { String^ item = StrToSys(dsSB[i].iata + " - " + dsSB[i].ten + " (" + dsSB[i].thanhPho + ")"); cbSanBayDi->Items->Add(item); cbSanBayDen->Items->Add(item); }
        if (cbSanBayDi->Items->Count > 0) cbSanBayDi->SelectedIndex = 0; if (cbSanBayDen->Items->Count > 1) cbSanBayDen->SelectedIndex = 1; if (cbQuocTich->Items->Count > 0) cbQuocTich->SelectedIndex = 0;

        // ======================= TAB DANH SÁCH VÉ =======================
        txtTimKiem = gcnew TextBox(); txtTimKiem->Location = Point(20, 20); txtTimKiem->Width = 300;
        btnTimKiem = gcnew Button(); btnTimKiem->Text = L"Tìm PNR / CCCD"; btnTimKiem->Location = Point(330, 18); btnTimKiem->Click += gcnew EventHandler(this, &MainForm::XuLyTimKiem);
        btnDoiCho = gcnew Button(); btnDoiCho->Text = L"Đổi Chỗ Ngồi"; btnDoiCho->Location = Point(450, 18); btnDoiCho->BackColor = Color::MediumPurple; btnDoiCho->ForeColor = Color::White; btnDoiCho->FlatStyle = FlatStyle::Flat; btnDoiCho->Click += gcnew EventHandler(this, &MainForm::XuLyDoiCho);
        btnHuyVe = gcnew Button(); btnHuyVe->Text = L"Hủy Vé Chọn"; btnHuyVe->Location = Point(560, 18); btnHuyVe->BackColor = Color::Crimson; btnHuyVe->ForeColor = Color::White; btnHuyVe->FlatStyle = FlatStyle::Flat; btnHuyVe->Click += gcnew EventHandler(this, &MainForm::XuLyHuyVe);
        cbLyDoHuy = TaoComboBox(680, 20, 150); cbLyDoHuy->Items->AddRange(gcnew cli::array<String^>{L"Lỗi Kỹ Thuật", L"Thời Tiết Xấu", L"Khách Yêu Cầu", L"Lý Do Khác"}); if (cbLyDoHuy->Items->Count > 0) cbLyDoHuy->SelectedIndex = 0;
        btnXoaVeHuy = gcnew Button(); btnXoaVeHuy->Text = L"Dọn Vé Đã Hủy"; btnXoaVeHuy->Location = Point(840, 18); btnXoaVeHuy->BackColor = Color::Gray; btnXoaVeHuy->ForeColor = Color::White; btnXoaVeHuy->FlatStyle = FlatStyle::Flat; btnXoaVeHuy->Click += gcnew EventHandler(this, &MainForm::XuLyXoaVeHuy);

        gridVe = gcnew DataGridView();
        gridVe->ColumnCount = 15;
        gridVe->Columns[0]->Name = L"PNR"; gridVe->Columns[1]->Name = L"Số Hiệu"; gridVe->Columns[2]->Name = L"Chỗ Ngồi"; gridVe->Columns[3]->Name = L"Họ Tên"; gridVe->Columns[4]->Name = L"CCCD"; gridVe->Columns[5]->Name = L"Hành Trình"; gridVe->Columns[6]->Name = L"Loại Vé"; gridVe->Columns[7]->Name = L"Thời Gian Bay"; gridVe->Columns[8]->Name = L"Ngày Về"; gridVe->Columns[9]->Name = L"Hãng Bay"; gridVe->Columns[10]->Name = L"Hạng Vé"; gridVe->Columns[11]->Name = L"Hành Lý"; gridVe->Columns[12]->Name = L"Tổng Tiền"; gridVe->Columns[13]->Name = L"Trạng Thái"; gridVe->Columns[14]->Name = L"Ngày Đặt";
        FormatGridWithSTT(gridVe);
        gridVe->Location = Point(20, 60); gridVe->Size = Drawing::Size(1280, 680);
        tabDanhSach->Controls->Add(txtTimKiem); tabDanhSach->Controls->Add(btnTimKiem); tabDanhSach->Controls->Add(btnDoiCho); tabDanhSach->Controls->Add(btnHuyVe); tabDanhSach->Controls->Add(cbLyDoHuy); tabDanhSach->Controls->Add(btnXoaVeHuy); tabDanhSach->Controls->Add(gridVe);

        // ======================= TAB HÀNG HÓA =======================
        tabHangHoa->Controls->Add(TaoLabel(L"THIẾT LẬP PHIẾU KÝ GỬI HÀNG HÓA", 20, 20, true));
        tabHangHoa->Controls->Add(TaoLabel(L"Người/Đơn vị gửi:", 20, 70, false)); txtNguoiGuiHH = TaoTextBox(160, 70, 250);
        tabHangHoa->Controls->Add(TaoLabel(L"Sân bay đi:", 20, 110, false)); cbSBDiHH = TaoComboBox(160, 110, 250);
        tabHangHoa->Controls->Add(TaoLabel(L"Sân bay đến:", 20, 150, false)); cbSBDenHH = TaoComboBox(160, 150, 250);
        for (size_t i = 0; i < dsSB.size(); i++) { String^ item = StrToSys(dsSB[i].iata + " - " + dsSB[i].ten); cbSBDiHH->Items->Add(item); cbSBDenHH->Items->Add(item); }
        if (cbSBDiHH->Items->Count > 0) cbSBDiHH->SelectedIndex = 0; if (cbSBDenHH->Items->Count > 1) cbSBDenHH->SelectedIndex = 1;
        tabHangHoa->Controls->Add(TaoLabel(L"Loại khách hàng:", 450, 70, false)); cbLoaiKhachHH = TaoComboBox(580, 70, 250); cbLoaiKhachHH->Items->AddRange(gcnew cli::array<String^>{L"Cá nhân", L"Doanh nghiệp / Công ty HH", L"Bệnh viện / Cơ sở Y tế"}); cbLoaiKhachHH->SelectedIndex = 0;
        tabHangHoa->Controls->Add(TaoLabel(L"Loại hàng hóa:", 450, 110, false)); cbLoaiHangHH = TaoComboBox(580, 110, 250); cbLoaiHangHH->Items->AddRange(gcnew cli::array<String^>{L"Hàng hóa thông thường", L"Thiết bị Y tế / Dược phẩm", L"Hàng dễ vỡ"}); cbLoaiHangHH->SelectedIndex = 0;
        tabHangHoa->Controls->Add(TaoLabel(L"Khối lượng (Kg):", 450, 150, false)); txtKhoiLuongHH = TaoTextBox(580, 150, 250); txtKhoiLuongHH->Text = L"10";
        tabHangHoa->Controls->Add(TaoLabel(L"Tốc độ giao:", 450, 190, false)); cbTocDoGiaoHH = TaoComboBox(580, 190, 250); cbTocDoGiaoHH->Items->AddRange(gcnew cli::array<String^>{L"Bình thường (Tiêu chuẩn)", L"Chuyển phát Nhanh (x1.5)", L"Hỏa tốc (x2.0)"}); cbTocDoGiaoHH->SelectedIndex = 0;
        btnTaoPhieuHH = gcnew Button(); btnTaoPhieuHH->Text = L"TÍNH GIÁ & TẠO PHIẾU"; btnTaoPhieuHH->Location = Point(870, 180); btnTaoPhieuHH->Size = Drawing::Size(200, 40); btnTaoPhieuHH->BackColor = Color::MediumSeaGreen; btnTaoPhieuHH->ForeColor = Color::White; btnTaoPhieuHH->FlatStyle = FlatStyle::Flat; btnTaoPhieuHH->Click += gcnew EventHandler(this, &MainForm::XuLyTaoPhieuHangHoa); tabHangHoa->Controls->Add(btnTaoPhieuHH);
        btnHuyPhieuHH = gcnew Button(); btnHuyPhieuHH->Text = L"HỦY PHIẾU CHỌN"; btnHuyPhieuHH->Location = Point(1090, 180); btnHuyPhieuHH->Size = Drawing::Size(150, 40); btnHuyPhieuHH->BackColor = Color::Crimson; btnHuyPhieuHH->ForeColor = Color::White; btnHuyPhieuHH->FlatStyle = FlatStyle::Flat; btnHuyPhieuHH->Click += gcnew EventHandler(this, &MainForm::XuLyHuyPhieuHH); tabHangHoa->Controls->Add(btnHuyPhieuHH);

        gridHangHoa = gcnew DataGridView();
        gridHangHoa->ColumnCount = 9;
        gridHangHoa->Columns[0]->Name = L"Mã Phiếu"; gridHangHoa->Columns[1]->Name = L"Người/Đơn vị gửi"; gridHangHoa->Columns[2]->Name = L"Hành Trình"; gridHangHoa->Columns[3]->Name = L"Tốc Độ"; gridHangHoa->Columns[4]->Name = L"Loại KH/Hàng"; gridHangHoa->Columns[5]->Name = L"Khối lượng (kg)"; gridHangHoa->Columns[6]->Name = L"Tổng Cước (VND)"; gridHangHoa->Columns[7]->Name = L"Trạng Thái"; gridHangHoa->Columns[8]->Name = L"Ngày Lập";
        FormatGridWithSTT(gridHangHoa);
        gridHangHoa->Location = Point(20, 240); gridHangHoa->Size = Drawing::Size(1280, 500);
        tabHangHoa->Controls->Add(txtNguoiGuiHH); tabHangHoa->Controls->Add(cbSBDiHH); tabHangHoa->Controls->Add(cbSBDenHH); tabHangHoa->Controls->Add(cbLoaiKhachHH); tabHangHoa->Controls->Add(cbLoaiHangHH); tabHangHoa->Controls->Add(txtKhoiLuongHH); tabHangHoa->Controls->Add(cbTocDoGiaoHH); tabHangHoa->Controls->Add(gridHangHoa);

        // ======================= TAB DỊCH VỤ UM =======================
        tabDichVuUM->Controls->Add(TaoLabel(L"ĐĂNG KÝ DỊCH VỤ TIẾP VIÊN CHĂM SÓC TRẺ EM (UM)", 30, 30, true));
        tabDichVuUM->Controls->Add(TaoLabel(L"Nhập mã PNR:", 30, 80, false)); txtTimPNR_UM = TaoTextBox(150, 78, 200);
        btnTimUM = gcnew Button(); btnTimUM->Text = L"Kiểm tra vé"; btnTimUM->Location = Point(360, 76); btnTimUM->Click += gcnew EventHandler(this, &MainForm::XuLyTimVeUM);
        lblThongTinUM = TaoLabel(L"Vui lòng nhập PNR để kiểm tra độ tuổi hành khách.", 30, 130, false); lblThongTinUM->ForeColor = Color::Blue;
        btnDangKyUM = gcnew Button(); btnDangKyUM->Text = L"XÁC NHẬN ĐĂNG KÝ UM (+500,000 VND)"; btnDangKyUM->Location = Point(30, 180); btnDangKyUM->Size = Drawing::Size(330, 40); btnDangKyUM->BackColor = Color::Orchid; btnDangKyUM->ForeColor = Color::White; btnDangKyUM->Enabled = false; btnDangKyUM->Click += gcnew EventHandler(this, &MainForm::XuLyDangKyUM);
        tabDichVuUM->Controls->Add(txtTimPNR_UM); tabDichVuUM->Controls->Add(btnTimUM); tabDichVuUM->Controls->Add(lblThongTinUM); tabDichVuUM->Controls->Add(btnDangKyUM);

        // ======================= TAB QUẢN LÝ KẾ HOẠCH BAY (ADMIN) =======================
        tabQuanLyCB->Controls->Add(TaoAdminHeaderPanel(L"✈", L"LÊN KẾ HOẠCH CHUYẾN BAY", Color::FromArgb(10, 28, 75)));
        int qX = 40, qY = 70, qStep = 40;
        tabQuanLyCB->Controls->Add(TaoLabel(L"Mã Chuyến Bay:", qX, qY, false)); txtQ_MaCB = TaoTextBox(160, qY, 150);
        tabQuanLyCB->Controls->Add(TaoLabel(L"Hãng Bay:", qX, qY + qStep, false)); cbQ_HangBay = TaoComboBox(160, qY + qStep, 150);
        std::vector<HangHangKhong> dsHK = DuLieuHangKhong::layTheHien()->layDanhSach();
        for (size_t i = 0; i < dsHK.size(); i++) cbQ_HangBay->Items->Add(StrToSys(dsHK[i].tenHang));
        if (cbQ_HangBay->Items->Count > 0) cbQ_HangBay->SelectedIndex = 0;
        tabQuanLyCB->Controls->Add(TaoLabel(L"Máy Bay:", qX, qY + qStep * 2, false)); txtQ_MayBay = TaoTextBox(160, qY + qStep * 2, 150); txtQ_MayBay->Text = L"Airbus A321";
        tabQuanLyCB->Controls->Add(TaoLabel(L"Giá Niêm Yết:", qX, qY + qStep * 3, false)); txtQ_Gia = TaoTextBox(160, qY + qStep * 3, 150); txtQ_Gia->Text = L"1500000";
        int qX2 = 350;
        tabQuanLyCB->Controls->Add(TaoLabel(L"Sân Bay Đi:", qX2, qY, false)); cbQ_SBDi = TaoComboBox(470, qY, 200);
        tabQuanLyCB->Controls->Add(TaoLabel(L"Sân Bay Đến:", qX2, qY + qStep, false)); cbQ_SBDen = TaoComboBox(470, qY + qStep, 200);
        for (size_t i = 0; i < dsSB.size(); i++) { String^ item = StrToSys(dsSB[i].iata + " - " + dsSB[i].ten); cbQ_SBDi->Items->Add(item); cbQ_SBDen->Items->Add(item); }
        if (cbQ_SBDi->Items->Count > 0) cbQ_SBDi->SelectedIndex = 0; if (cbQ_SBDen->Items->Count > 1) cbQ_SBDen->SelectedIndex = 1;
        tabQuanLyCB->Controls->Add(TaoLabel(L"Khởi Hành:", qX2, qY + qStep * 2, false));
        dtpQ_NgayDi = gcnew DateTimePicker(); dtpQ_NgayDi->Format = DateTimePickerFormat::Short; dtpQ_NgayDi->Location = Point(470, qY + qStep * 2); dtpQ_NgayDi->Width = 100; dtpQ_NgayDi->MinDate = System::DateTime::Now;
        dtpQ_GioDi = gcnew DateTimePicker(); dtpQ_GioDi->Format = DateTimePickerFormat::Time; dtpQ_GioDi->ShowUpDown = true; dtpQ_GioDi->Location = Point(580, qY + qStep * 2); dtpQ_GioDi->Width = 90;
        tabQuanLyCB->Controls->Add(TaoLabel(L"Hạ Cánh:", qX2, qY + qStep * 3, false));
        dtpQ_NgayDen = gcnew DateTimePicker(); dtpQ_NgayDen->Format = DateTimePickerFormat::Short; dtpQ_NgayDen->Location = Point(470, qY + qStep * 3); dtpQ_NgayDen->Width = 100; dtpQ_NgayDen->MinDate = System::DateTime::Now;
        dtpQ_GioDen = gcnew DateTimePicker(); dtpQ_GioDen->Format = DateTimePickerFormat::Time; dtpQ_GioDen->ShowUpDown = true; dtpQ_GioDen->Location = Point(580, qY + qStep * 3); dtpQ_GioDen->Width = 90;

        btnQ_Them = TaoAdminButton(L"THÊM MỚI", 700, qY, 120, 35, Color::FromArgb(0, 130, 70));
        btnQ_Sua = TaoAdminButton(L"SỬA CHUYẾN", 700, qY + 45, 120, 35, Color::FromArgb(180, 120, 0));
        btnQ_Xoa = TaoAdminButton(L"XÓA CHUYẾN", 700, qY + 90, 120, 35, Color::Crimson);
        btnQ_ThemTuDong = TaoAdminButton(L"TỰ ĐỘNG (5 Cặp)", 830, qY, 150, 35, Color::BlueViolet);
        btnQ_CapNhat = TaoAdminButton(L"ĐỒNG BỘ", 830, qY + 45, 150, 35, Color::Teal);

        btnQ_Them->Click += gcnew EventHandler(this, &MainForm::XuLyThemCBAdmin);
        btnQ_Sua->Click += gcnew EventHandler(this, &MainForm::XuLySuaCBAdmin);
        btnQ_Xoa->Click += gcnew EventHandler(this, &MainForm::XuLyXoaCBAdmin);
        btnQ_ThemTuDong->Click += gcnew EventHandler(this, &MainForm::XuLyThemTuDongCBAdmin);
        btnQ_CapNhat->Click += gcnew EventHandler(this, &MainForm::XuLyCapNhatHeThongAdmin);

        gridQuanLyCB = gcnew DataGridView();
        gridQuanLyCB->ColumnCount = 8;
        gridQuanLyCB->Columns[0]->Name = L"Mã CB"; gridQuanLyCB->Columns[1]->Name = L"Hãng Bay"; gridQuanLyCB->Columns[2]->Name = L"Sân Bay Đi"; gridQuanLyCB->Columns[3]->Name = L"Sân Bay Đến"; gridQuanLyCB->Columns[4]->Name = L"Máy Bay"; gridQuanLyCB->Columns[5]->Name = L"Khởi Hành"; gridQuanLyCB->Columns[6]->Name = L"Hạ Cánh"; gridQuanLyCB->Columns[7]->Name = L"Giá Niêm Yết";
        FormatGridWithSTT(gridQuanLyCB);
        gridQuanLyCB->Location = Point(20, 240); gridQuanLyCB->Size = Drawing::Size(1280, 490);
        gridQuanLyCB->SelectionChanged += gcnew EventHandler(this, &MainForm::DienThongTinSuaCB);

        tabQuanLyCB->Controls->Add(txtQ_MaCB); tabQuanLyCB->Controls->Add(cbQ_HangBay); tabQuanLyCB->Controls->Add(txtQ_MayBay); tabQuanLyCB->Controls->Add(txtQ_Gia); tabQuanLyCB->Controls->Add(cbQ_SBDi); tabQuanLyCB->Controls->Add(cbQ_SBDen); tabQuanLyCB->Controls->Add(dtpQ_NgayDi); tabQuanLyCB->Controls->Add(dtpQ_GioDi); tabQuanLyCB->Controls->Add(dtpQ_NgayDen); tabQuanLyCB->Controls->Add(dtpQ_GioDen); tabQuanLyCB->Controls->Add(btnQ_Them); tabQuanLyCB->Controls->Add(btnQ_Sua); tabQuanLyCB->Controls->Add(btnQ_Xoa); tabQuanLyCB->Controls->Add(btnQ_ThemTuDong); tabQuanLyCB->Controls->Add(btnQ_CapNhat); tabQuanLyCB->Controls->Add(gridQuanLyCB);

        // ======================= TAB QUẢN LÝ MGG =======================
        tabQuanLyMGG->Controls->Add(TaoAdminHeaderPanel(L"🎫", L"QUẢN LÝ MÃ GIẢM GIÁ — Ban hành & Kiểm soát", Color::FromArgb(60, 0, 100)));
        Panel^ pnlMGGInput = gcnew Panel(); pnlMGGInput->Location = Point(20, 72); pnlMGGInput->Size = Drawing::Size(620, 300); pnlMGGInput->BackColor = Color::FromArgb(248, 245, 255); pnlMGGInput->BorderStyle = BorderStyle::FixedSingle;
        Label^ lblMGGHeader = gcnew Label(); lblMGGHeader->Text = L"  THIẾT LẬP MÃ GIẢM GIÁ MỚI"; lblMGGHeader->Dock = DockStyle::Top; lblMGGHeader->Height = 32; lblMGGHeader->BackColor = Color::FromArgb(85, 0, 130); lblMGGHeader->ForeColor = Color::White; lblMGGHeader->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold); lblMGGHeader->TextAlign = ContentAlignment::MiddleLeft; pnlMGGInput->Controls->Add(lblMGGHeader);
        int mY2 = 42, mSt = 35;
        txtMGG_Ma = TaoTextBox(0, 0, 200); txtMGG_Ma->CharacterCasing = CharacterCasing::Upper; txtMGG_Ma->Font = gcnew Drawing::Font("Consolas", 11, FontStyle::Bold); txtMGG_Ma->ForeColor = Color::FromArgb(80, 0, 120); AddPair(pnlMGGInput, L"Mã Giảm Giá (*):", txtMGG_Ma, mY2);
        txtMGG_PhanTram = TaoTextBox(0, 0, 80); txtMGG_PhanTram->Text = L"10"; AddPair(pnlMGGInput, L"% Giảm (1–100):", txtMGG_PhanTram, mY2 + mSt);
        dtpMGG_NgayBD = gcnew DateTimePicker(); dtpMGG_NgayBD->Width = 110; dtpMGG_NgayBD->Format = DateTimePickerFormat::Custom; dtpMGG_NgayBD->CustomFormat = L"dd/MM/yyyy";
        dtpMGG_GioBD = gcnew DateTimePicker(); dtpMGG_GioBD->Width = 80; dtpMGG_GioBD->Format = DateTimePickerFormat::Custom; dtpMGG_GioBD->CustomFormat = L"HH:mm"; dtpMGG_GioBD->ShowUpDown = true; dtpMGG_GioBD->Location = Point(270, mY2 + mSt * 2 - 3); pnlMGGInput->Controls->Add(dtpMGG_GioBD); AddPair(pnlMGGInput, L"Bắt Đầu:", dtpMGG_NgayBD, mY2 + mSt * 2);
        dtpMGG_NgayHH = gcnew DateTimePicker(); dtpMGG_NgayHH->Width = 110; dtpMGG_NgayHH->Format = DateTimePickerFormat::Custom; dtpMGG_NgayHH->CustomFormat = L"dd/MM/yyyy"; dtpMGG_NgayHH->Value = System::DateTime::Now.AddDays(180);
        dtpMGG_GioHH = gcnew DateTimePicker(); dtpMGG_GioHH->Width = 80; dtpMGG_GioHH->Format = DateTimePickerFormat::Custom; dtpMGG_GioHH->CustomFormat = L"HH:mm"; dtpMGG_GioHH->ShowUpDown = true; dtpMGG_GioHH->Location = Point(270, mY2 + mSt * 3 - 3); pnlMGGInput->Controls->Add(dtpMGG_GioHH); AddPair(pnlMGGInput, L"Kết Thúc:", dtpMGG_NgayHH, mY2 + mSt * 3);
        txtMGG_SoLuong = TaoTextBox(0, 0, 100); txtMGG_SoLuong->Text = L"100"; AddPair(pnlMGGInput, L"Số Lượng:", txtMGG_SoLuong, mY2 + mSt * 4);
        cbMGG_LoaiVe = TaoComboBox(0, 0, 160); cbMGG_LoaiVe->Items->AddRange(gcnew cli::array<String^>{L"Tat ca", L"Economy", L"Business", L"First Class"}); cbMGG_LoaiVe->SelectedIndex = 0; AddPair(pnlMGGInput, L"Áp Dụng Hạng:", cbMGG_LoaiVe, mY2 + mSt * 5);
        txtMGG_MoTa = TaoTextBox(0, 0, 390); AddPair(pnlMGGInput, L"Mô Tả / Ghi Chú:", txtMGG_MoTa, mY2 + mSt * 6);
        tabQuanLyMGG->Controls->Add(pnlMGGInput);
        Panel^ pnlMGGBtns = gcnew Panel(); pnlMGGBtns->Location = Point(655, 72); pnlMGGBtns->Size = Drawing::Size(260, 260); pnlMGGBtns->BackColor = Color::Transparent;
        btnMGG_Them = TaoAdminButton(L"➕  THÊM MÃ MỚI", 0, 8, 250, 52, Color::FromArgb(0, 140, 80)); btnMGG_Them->Click += gcnew EventHandler(this, &MainForm::XuLyThemMGG); pnlMGGBtns->Controls->Add(btnMGG_Them);
        btnMGG_Toggle = TaoAdminButton(L"🔄  KÍCH HOẠT / TẮT", 0, 70, 250, 52, Color::FromArgb(190, 110, 0)); btnMGG_Toggle->Click += gcnew EventHandler(this, &MainForm::XuLyToggleMGG); pnlMGGBtns->Controls->Add(btnMGG_Toggle);
        btnMGG_Xoa = TaoAdminButton(L"🗑  XÓA MÃ ĐÃ CHỌN", 0, 132, 250, 52, Color::Crimson); btnMGG_Xoa->Click += gcnew EventHandler(this, &MainForm::XuLyXoaMGG); pnlMGGBtns->Controls->Add(btnMGG_Xoa);
        lblMGGStats = gcnew Label(); lblMGGStats->Location = Point(0, 196); lblMGGStats->Size = Drawing::Size(250, 55); lblMGGStats->Text = L"Tổng mã: 0  |  Đang hoạt động: 0"; lblMGGStats->Font = gcnew Drawing::Font("Segoe UI", 9, FontStyle::Bold); lblMGGStats->ForeColor = Color::FromArgb(80, 0, 120); pnlMGGBtns->Controls->Add(lblMGGStats);
        tabQuanLyMGG->Controls->Add(pnlMGGBtns);

        gridMGG = gcnew DataGridView();
        gridMGG->ColumnCount = 7;
        gridMGG->Columns[0]->Name = L"Mã Giảm Giá"; gridMGG->Columns[1]->Name = L"% Giảm"; gridMGG->Columns[2]->Name = L"Khung Thời Gian"; gridMGG->Columns[3]->Name = L"Lượt Dùng"; gridMGG->Columns[4]->Name = L"Áp Dụng Hạng"; gridMGG->Columns[5]->Name = L"Mô Tả"; gridMGG->Columns[6]->Name = L"Trạng Thái";
        FormatGridWithSTT(gridMGG);
        gridMGG->Location = Point(20, 385); gridMGG->Size = Drawing::Size(1280, 355);
        tabQuanLyMGG->Controls->Add(gridMGG);

        // ======================= TAB NHÂN SỰ (ADMIN) =======================
        tabNhanSu->Controls->Add(TaoAdminHeaderPanel(L"👨‍💼", L"QUẢN LÝ NHÂN SỰ & HIỆU SUẤT", Color::FromArgb(40, 60, 100)));
        gridNhanSu = gcnew DataGridView();
        gridNhanSu->ColumnCount = 5;
        gridNhanSu->Columns[0]->Name = L"Mã NV"; gridNhanSu->Columns[1]->Name = L"Họ Tên"; gridNhanSu->Columns[2]->Name = L"Chức Vụ"; gridNhanSu->Columns[3]->Name = L"Khách Phục Vụ"; gridNhanSu->Columns[4]->Name = L"Trạng Thái";
        FormatGridWithSTT(gridNhanSu);
        gridNhanSu->Location = Point(20, 80); gridNhanSu->Size = Drawing::Size(1000, 500);
        tabNhanSu->Controls->Add(gridNhanSu);
        btnKhoaMoNV = TaoAdminButton(L"KHÓA / MỞ TÀI KHOẢN", 1040, 80, 200, 45, Color::Crimson); btnKhoaMoNV->Click += gcnew EventHandler(this, &MainForm::XuLyKhoaMoTaiKhoan); tabNhanSu->Controls->Add(btnKhoaMoNV);

        // ======================= TAB THỐNG KÊ =======================
        lblTongDoanhThu = gcnew Label(); lblTongDoanhThu->Location = Point(30, 30); lblTongDoanhThu->AutoSize = true; lblTongDoanhThu->Font = gcnew Drawing::Font("Segoe UI", 16, FontStyle::Bold); lblTongDoanhThu->ForeColor = Color::DarkBlue; lblTongDoanhThu->Text = L"DOANH THU VÉ MÁY BAY: 0 VND";
        lblDoanhThuHangHoa = gcnew Label(); lblDoanhThuHangHoa->Location = Point(30, 80); lblDoanhThuHangHoa->AutoSize = true; lblDoanhThuHangHoa->Font = gcnew Drawing::Font("Segoe UI", 16, FontStyle::Bold); lblDoanhThuHangHoa->ForeColor = Color::DarkGreen; lblDoanhThuHangHoa->Text = L"DOANH THU HÀNG HÓA: 0 VND";
        cbLoaiBieuDo = TaoComboBox(900, 30, 350); cbLoaiBieuDo->Items->AddRange(gcnew cli::array<String^>{L"1. Tổng Doanh Thu Cơ Bản (Cột)", L"2. Doanh Thu Theo Hãng Bay (Cột Gradient)", L"3. Tỷ Lệ Trạng Thái Vé (Tròn)"}); cbLoaiBieuDo->SelectedIndex = 0; cbLoaiBieuDo->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::XuLyChuyenDoiBieuDo);
        gridThongKe = gcnew DataGridView(); gridThongKe->ColumnCount = 3; gridThongKe->Columns[0]->Name = L"Hạng Mục / Loại Vé"; gridThongKe->Columns[1]->Name = L"Số Lượng"; gridThongKe->Columns[2]->Name = L"Doanh Thu (VND)"; FormatGridWithSTT(gridThongKe); gridThongKe->Location = Point(30, 140); gridThongKe->Size = Drawing::Size(800, 200);
        btnLuuDuLieu = gcnew Button(); btnLuuDuLieu->Text = L"LƯU DỮ LIỆU TỔNG RA FILE (.CSV)"; btnLuuDuLieu->Location = Point(30, 360); btnLuuDuLieu->Size = Drawing::Size(280, 45); btnLuuDuLieu->BackColor = Color::Teal; btnLuuDuLieu->ForeColor = Color::White; btnLuuDuLieu->FlatStyle = FlatStyle::Flat; btnLuuDuLieu->Click += gcnew EventHandler(this, &MainForm::XuLyLuuFile);
        btnXuatDoanhThuHangBay = gcnew Button(); btnXuatDoanhThuHangBay->Text = L"XUẤT BÁO CÁO HÃNG BAY (.CSV)"; btnXuatDoanhThuHangBay->Location = Point(330, 360); btnXuatDoanhThuHangBay->Size = Drawing::Size(280, 45); btnXuatDoanhThuHangBay->BackColor = Color::MediumVioletRed; btnXuatDoanhThuHangBay->ForeColor = Color::White; btnXuatDoanhThuHangBay->FlatStyle = FlatStyle::Flat; btnXuatDoanhThuHangBay->Click += gcnew EventHandler(this, &MainForm::XuLyXuatCSVHangBay);
        picBieuDo = gcnew PictureBox(); picBieuDo->Location = Point(450, 420); picBieuDo->Size = Drawing::Size(850, 350); picBieuDo->BorderStyle = BorderStyle::FixedSingle; picBieuDo->BackColor = Color::WhiteSmoke;
        tabThongKe->Controls->Add(lblTongDoanhThu); tabThongKe->Controls->Add(lblDoanhThuHangHoa); tabThongKe->Controls->Add(cbLoaiBieuDo); tabThongKe->Controls->Add(gridThongKe); tabThongKe->Controls->Add(btnLuuDuLieu); tabThongKe->Controls->Add(btnXuatDoanhThuHangBay); tabThongKe->Controls->Add(picBieuDo);

        // ======================= TAB HỆ THỐNG =======================
        Panel^ pnlHeThong = gcnew Panel(); pnlHeThong->Location = Point(0, 0); pnlHeThong->Size = Drawing::Size(1340, 58); pnlHeThong->BackColor = Color::FromArgb(30, 30, 30);
        Label^ lblHTTitle = gcnew Label(); lblHTTitle->Text = L"⚙  QUẢN LÝ HỆ THỐNG & PHÂN QUYỀN"; lblHTTitle->Font = gcnew Drawing::Font("Segoe UI", 13, FontStyle::Bold); lblHTTitle->ForeColor = Color::White; lblHTTitle->Dock = DockStyle::Fill; lblHTTitle->TextAlign = ContentAlignment::MiddleCenter; pnlHeThong->Controls->Add(lblHTTitle); tabHeThong->Controls->Add(pnlHeThong);
        pnlMarquee = gcnew Panel(); pnlMarquee->Location = Point(0, 58); pnlMarquee->Size = Drawing::Size(1340, 30); pnlMarquee->BackColor = Color::Black; pnlMarquee->BorderStyle = BorderStyle::FixedSingle;
        lblMarquee = gcnew Label(); lblMarquee->Text = L"🚀 TRANG THÔNG TIN QUẢN TRỊ NỘI BỘ -  HỆ THỐNG UTC2 AIRLINES ĐANG HOẠT ĐỘNG "; lblMarquee->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold); lblMarquee->ForeColor = Color::Lime; lblMarquee->AutoSize = true; lblMarquee->Location = Point(1340, 4); pnlMarquee->Controls->Add(lblMarquee); tabHeThong->Controls->Add(pnlMarquee);
        tmrMarquee = gcnew Timer(); tmrMarquee->Interval = 25; tmrMarquee->Tick += gcnew EventHandler(this, &MainForm::HieuUngChayChu); tmrMarquee->Start();

        lblQuyenHienTai = TaoLabel(L"Quyền hạn: ", 50, 120, true); lblQuyenHienTai->Font = gcnew Drawing::Font("Segoe UI", 14, FontStyle::Bold);
        lblHoSoNV = TaoLabel(L"", 50, 160, false); lblHoSoNV->Font = gcnew Drawing::Font("Segoe UI", 11); lblHoSoNV->ForeColor = Color::DarkBlue;

        btnDoiQuyen = gcnew Button();
        btnDoiQuyen->Text = L"ĐĂNG XUẤT / ĐỔI TÀI KHOẢN";
        btnDoiQuyen->Location = Point(50, 280);
        btnDoiQuyen->Size = Drawing::Size(280, 50);
        btnDoiQuyen->BackColor = Color::FromArgb(40, 60, 120);
        btnDoiQuyen->ForeColor = Color::White;
        btnDoiQuyen->FlatStyle = FlatStyle::Flat;
        btnDoiQuyen->Font = gcnew Drawing::Font("Segoe UI", 11, FontStyle::Bold);
        btnDoiQuyen->Click += gcnew EventHandler(this, &MainForm::XuLyDoiQuyen);

        picLogo = gcnew PictureBox(); picLogo->Location = Point(500, 150); picLogo->Size = Drawing::Size(600, 500); picLogo->SizeMode = PictureBoxSizeMode::Zoom;
        try { picLogo->Image = Image::FromFile("D:\\logo.png"); }
        catch (...) { picLogo->BackColor = Color::LightGray; }
        tabHeThong->Controls->Add(lblQuyenHienTai); tabHeThong->Controls->Add(lblHoSoNV); tabHeThong->Controls->Add(btnDoiQuyen); tabHeThong->Controls->Add(picLogo);

        this->Controls->Add(tabCtrl);
        CapNhatGiaoDienTheoQuyen();
        tabCtrl->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::CapNhatTab);
    }

    // ========================== HANDLERS ==========================
    void HieuUngChayChu(Object^ sender, EventArgs^ e) { if (lblMarquee != nullptr && pnlMarquee != nullptr) { lblMarquee->Left -= 4; if (lblMarquee->Right < 0) { lblMarquee->Left = pnlMarquee->Width; } } }
    void XuLyChonLoaiVe(Object^ sender, EventArgs^ e) { bool h = rbHaiChieu->Checked; lblNgayVe->Visible = h; dtpNgayVe->Visible = h; }
    void XuLyDoiNgayBay(Object^ sender, EventArgs^ e) { dtpNgayVe->MinDate = dtpNgayBay->Value; if (dtpNgayVe->Value < dtpNgayBay->Value) dtpNgayVe->Value = dtpNgayBay->Value; }
    void CapNhatHinhMucHanhLy(Object^ sender, EventArgs^ e) {
        if (cbHangVe->SelectedIndex == 0) lblHMDinhMuc->Text = L"Định mức: 23kg KG / 7kg XT"; else if (cbHangVe->SelectedIndex == 1) lblHMDinhMuc->Text = L"Định mức: 40kg KG / 14kg XT"; else lblHMDinhMuc->Text = L"Định mức: 50kg KG / 20kg XT";
        maGiamGiaHienTai = 0.0; tenMGGDangDung = ""; lblKetQuaMGG->Text = L""; txtMaGiamGia->Clear();
    }
    void XuLyApDungMaGiam(Object^ sender, EventArgs^ e) {
        maGiamGiaHienTai = 0.0; tenMGGDangDung = ""; std::string ma = SysToStr(txtMaGiamGia->Text);
        if (ma.empty()) { lblKetQuaMGG->Text = L"⚠ Vui lòng nhập mã!"; lblKetQuaMGG->ForeColor = Color::Orange; return; }
        std::string loaiVe = SysToStr(cbHangVe->Text); double pct = 0; std::string moTa;
        if (KiemTraVaLayMGG(ma, loaiVe, pct, moTa)) {
            maGiamGiaHienTai = pct; tenMGGDangDung = txtMaGiamGia->Text; lblKetQuaMGG->Text = L"✔ Giảm " + pct.ToString("0") + L"% — " + StrToSys(moTa); lblKetQuaMGG->ForeColor = Color::SeaGreen;
        }
        else { lblKetQuaMGG->Text = L"✘ Mã hết hạn/lượt hoặc sai hạng!"; lblKetQuaMGG->ForeColor = Color::Crimson; }
    }
    void XuLyChonQuocGia(Object^ sender, EventArgs^ e) {
        std::string tenQG = SysToStr(cbQuocTich->SelectedItem->ToString());
        txtSDT->Text = StrToSys(DuLieuQuocGia::layTheHien()->layMaVung(tenQG));

        if (tenQG == u8"Việt Nam" || tenQG == "Viet Nam" || tenQG == "VN" || tenQG.find("Vi") != std::string::npos) {
            txtCCCD->Enabled = true;
            txtHoChieu->Enabled = false;
            txtHoChieu->Text = L"";
        }
        else {
            txtCCCD->Enabled = false;
            txtCCCD->Text = L"";
            lblThongTinCCCD->Text = L"";
            txtHoChieu->Enabled = true;
        }
    }
    void XuLyTruyXuatCCCD_RealTime(Object^ sender, EventArgs^ e) {
        if (!txtCCCD->Enabled) return;
        std::string cccd = SysToStr(txtCCCD->Text);
        if (cccd.empty()) { lblThongTinCCCD->Text = L""; return; }

        for (char c : cccd) {
            if (c < '0' || c>'9') {
                lblThongTinCCCD->Text = L"⚠ Lỗi: CCCD chỉ được phép chứa chữ số!";
                lblThongTinCCCD->ForeColor = Color::Red;
                return;
            }
        }

        if (cccd.length() < 12) {
            lblThongTinCCCD->Text = L"⚠ Cần nhập đủ 12 chữ số (" + cccd.length().ToString() + L"/12)";
            lblThongTinCCCD->ForeColor = Color::Orange;
        }
        else if (cccd.length() > 12) {
            lblThongTinCCCD->Text = L"⚠ Lỗi: CCCD không được vượt quá 12 chữ số!";
            lblThongTinCCCD->ForeColor = Color::Red;
        }
        else {
            ThongTinCCCD tt = PhanTichCCCD::phanTich(cccd);
            if (tt.hopLe) {
                // FIX LỖI FONT Ở ĐÂY BẰNG CÁCH SỬ DỤNG STRING NATIVE C++/CLI (L"...") KẾT HỢP StrToSys
                lblThongTinCCCD->Text = L"✔ Hợp lệ: " + StrToSys(tt.gioiTinh) + L" | Sinh: " + tt.namSinh.ToString() + L" (" + tt.tuoi.ToString() + L" tuổi) | Cấp tại: " + StrToSys(tt.tenTinh);
                lblThongTinCCCD->ForeColor = Color::Green;
            }
            else {
                lblThongTinCCCD->Text = L"⚠ Lỗi: Cấu trúc mã tỉnh/năm sinh CCCD không hợp lệ!";
                lblThongTinCCCD->ForeColor = Color::Red;
            }
        }
    }
    void XuLyTimChuyenBay(Object^ sender, EventArgs^ e) {
        if (cbSanBayDi->SelectedIndex < 0 || cbSanBayDen->SelectedIndex < 0 || cbSanBayDi->SelectedIndex == cbSanBayDen->SelectedIndex) { MessageBox::Show(L"Sân bay không hợp lệ!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error); return; }
        gridChuyenBay->Rows->Clear(); dsChuyenBayHienTai->clear();
        std::string iataDi = SysToStr(cbSanBayDi->SelectedItem->ToString()).substr(0, 3), iataDen = SysToStr(cbSanBayDen->SelectedItem->ToString()).substr(0, 3);
        std::string ngayCanTim = SysToStr(dtpNgayBay->Text); bool found = false; int stt = 1;
        for (size_t i = 0; i < khoChuyenBayToanCuc.kichThuoc(); i++) {
            ChuyenBay cb = khoChuyenBayToanCuc[i];
            if (cb.sbDi.iata == iataDi && cb.sbDen.iata == iataDen && cb.thoiGianDi.find(ngayCanTim) == 0) {
                dsChuyenBayHienTai->push_back(cb);
                gridChuyenBay->Rows->Add(stt.ToString(), StrToSys(cb.hangBay.tenHang), StrToSys(cb.maCB), StrToSys(cb.dongMayBay), StrToSys(cb.thoiGianDi), StrToSys(cb.thoiGianDen), cb.giaNiemYet.ToString(L"N0"));
                stt++; found = true;
            }
        }
        if (!found) MessageBox::Show(L"Không có chuyến bay trong ngày này!", L"Thông báo", MessageBoxButtons::OK, MessageBoxIcon::Information);
    }

    // ==== NƠI XỬ LÝ ĐẶT VÉ VÀ GỌI DỊCH VỤ EMAIL ====
    void XuLyDatVe(Object^ sender, EventArgs^ e) {
        if (gridChuyenBay->SelectedRows->Count == 0) { MessageBox::Show(L"Vui lòng chọn chuyến bay!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
        std::string ten = SysToStr(txtHoTen->Text), quocTich = SysToStr(cbQuocTich->Text), cccd = SysToStr(txtCCCD->Text), hoChieu = SysToStr(txtHoChieu->Text), sdt = SysToStr(txtSDT->Text);
        if (ten.empty() || sdt.empty()) { MessageBox::Show(L"Điền đủ thông tin bắt buộc!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
        bool laVietNam = false; std::string qtUpper = quocTich; std::transform(qtUpper.begin(), qtUpper.end(), qtUpper.begin(), ::toupper);
        if (qtUpper == u8"VIỆT NAM" || qtUpper == "VIET NAM" || qtUpper == "VN" || quocTich.find("Vi") != std::string::npos) laVietNam = true;

        if (laVietNam && cccd.length() != 12) {
            MessageBox::Show(L"Căn cước công dân bắt buộc phải nhập đủ và đúng 12 chữ số. Vui lòng kiểm tra lại!", L"Lỗi Nhập Liệu", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        if (laVietNam && !KiemTra::hopLeCCCD(cccd)) { MessageBox::Show(L"CCCD sai cấu trúc hoặc chứa chữ cái!", L"Lỗi Nhập Liệu", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
        if (!laVietNam && !KiemTra::hopLeHoChieu(hoChieu)) { MessageBox::Show(L"Hộ chiếu sai cấu trúc!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
        if (!KiemTra::hopLeSDT(sdt)) { MessageBox::Show(L"SĐT sai mã vùng!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }

        ThongTinCCCD tt;
        if (laVietNam) { tt = PhanTichCCCD::phanTich(cccd); if (!tt.hopLe) return; }
        else { tt.soCCCD = "PASS:" + hoChieu; tt.tuoi = 25; tt.hopLe = true; tt.gioiTinh = u8"Không xác định"; tt.tenTinh = u8"Quốc tế"; }
        int selIdx = gridChuyenBay->SelectedRows[0]->Index; ChuyenBay cbChon = (*dsChuyenBayHienTai)[selIdx];
        double kgKG = 0, kgXT = 0; try { kgKG = Convert::ToDouble(txtKyGui->Text); kgXT = Convert::ToDouble(txtXachTay->Text); }
        catch (...) {}

        HanhKhach hk; hk.thietLapHK(ten, tt, sdt, quocTich, hoChieu);
        HangVe* hv = nullptr; std::string hangVeStr = SysToStr(cbHangVe->Text);
        if (cbHangVe->SelectedIndex == 0) hv = new PhoThong(); else if (cbHangVe->SelectedIndex == 1) hv = new ThuongGia(); else hv = new HangNhat();
        bool laHaiChieu = rbHaiChieu->Checked; std::string ngayVeStr = laHaiChieu ? SysToStr(dtpNgayVe->Text) : "";
        double tienPhat = 0; double tongTien = TinhGiaVe::tinhToan(cbChon.giaNiemYet, hv, kgKG, kgXT, tienPhat);
        if (tt.tuoi <= 12) tongTien *= 0.75;
        double pctMGG = 0; std::string moTaMGG; bool coMGG = (maGiamGiaHienTai > 0) && KiemTraVaLayMGG(SysToStr(tenMGGDangDung), hangVeStr, pctMGG, moTaMGG);
        if (coMGG) tongTien *= (1.0 - pctMGG / 100.0); if (laHaiChieu) tongTien *= 2;

        std::string gheDuKien = QuanLyGhe::SinhGheTuDong(cbChon.maCB, cbChon.thoiGianDi, hangVeStr); String^ sysGhe = StrToSys(gheDuKien); if (gheDuKien == "WL") sysGhe = L"Hết chỗ (Waitlist)";
        String^ strLoaiVe = laHaiChieu ? L"\nKhứ hồi về: " + dtpNgayVe->Text : L"\nMột chiều";
        String^ confirm = String::Format(L"XÁC NHẬN:\nTên: {0}\nChuyến: {1}\nHạng: {2}\nGhế: {3}{4}\nTiền: {5} VND", txtHoTen->Text, StrToSys(cbChon.maCB), cbHangVe->Text, sysGhe, strLoaiVe, tongTien.ToString(L"N0"));
        if (MessageBox::Show(confirm, L"Xác Nhận Đặt Vé", MessageBoxButtons::YesNo, MessageBoxIcon::Information) == System::Windows::Forms::DialogResult::Yes) {
            std::string pnr = "V" + std::to_string(10000 + rand() % 90000);
            VeMayBay veMoi(pnr, hk, cbChon, hv, kgKG, kgXT, laHaiChieu, ngayVeStr, gheDuKien); veMoi.setTongTien(tongTien); heThongVe.them(veMoi);
            if (coMGG) {
                std::string maUpper = SysToStr(tenMGGDangDung); std::transform(maUpper.begin(), maUpper.end(), maUpper.begin(), ::toupper);
                for (size_t i = 0; i < heThongMaGiamGia.kichThuoc(); i++) { std::string code = heThongMaGiamGia[i].maMGG; std::transform(code.begin(), code.end(), code.begin(), ::toupper); if (code == maUpper) { heThongMaGiamGia[i].soLuongDaDung++; break; } }
            }
            if (nhanVienHienTai) nhanVienHienTai->tangSoKhach();
            MessageBox::Show(L"Đặt vé thành công!\nPNR: " + StrToSys(pnr), L"Hoàn Tất", MessageBoxButtons::OK, MessageBoxIcon::Information);

            // --- XỬ LÝ GỬI EMAIL NẾU CÓ NHẬP EMAIL ---
          // --- XỬ LÝ GỬI EMAIL NẾU CÓ NHẬP EMAIL ---
            String^ emailNhan = txtEmail->Text;
            if (!String::IsNullOrWhiteSpace(emailNhan) && emailNhan->Contains("@")) {

                // 1. Chuẩn hóa chuỗi hiển thị thời gian bay (Kiểm tra khứ hồi để tự động ghép ngày về)
                String^ thoiGianBayEmail = StrToSys(cbChon.thoiGianDi);
                if (laHaiChieu) {
                    thoiGianBayEmail += L" (Chiều đi) | Khứ hồi về: " + dtpNgayVe->Text;
                }

                // 2. Lấy thời gian thực tại thời điểm bấm nút đặt vé thành công (Giờ hệ thống máy tính)
                time_t tReal = time(0);
                char bufReal[80];
                // Định dạng ngày/tháng/năm giờ:phút:giây chi tiết thời gian thực
                strftime(bufReal, sizeof(bufReal), "%d/%m/%Y %H:%M:%S", localtime(&tReal));
                String^ thoiGianDatThuc = StrToSys(std::string(bufReal));

                // 3. Tiến hành gọi hàm gửi mail với đầy đủ dữ liệu thời gian thực
                DichVuEmail::GuiThongBaoDatVe(
                    emailNhan,
                    txtHoTen->Text,
                    StrToSys(pnr),
                    StrToSys(cbChon.sbDi.iata + " - " + cbChon.sbDen.iata),
                    thoiGianBayEmail,
                    thoiGianDatThuc,
                    tongTien.ToString(L"N0")
                );
            }

            // Dọn dẹp Textbox
            txtHoTen->Clear(); txtCCCD->Clear(); txtHoChieu->Clear(); txtEmail->Clear(); txtMaGiamGia->Clear(); lblKetQuaMGG->Text = L""; maGiamGiaHienTai = 0.0; tenMGGDangDung = ""; gridChuyenBay->Rows->Clear(); dsChuyenBayHienTai->clear();
        }
        delete hv;
    }

    void DoDuLieuVaoGrid(const QuanLyDanhSach<VeMayBay>& ds) {
        gridVe->Rows->Clear();
        for (size_t i = 0; i < ds.kichThuoc(); i++) {
            gridVe->Rows->Add((i + 1).ToString(), StrToSys(ds[i].layPNR()), StrToSys(ds[i].laySoHieu()), StrToSys(ds[i].layGheNgoi()), StrToSys(ds[i].layTenKhach()), StrToSys(ds[i].layThongTinCCCD().soCCCD), StrToSys(ds[i].layHanhTrinh()), ds[i].layLaHaiChieu() ? L"Khứ Hồi" : L"Một Chiều", StrToSys(ds[i].layNgayGioBay()), ds[i].layLaHaiChieu() ? StrToSys(ds[i].layNgayVe()) : L"-", StrToSys(ds[i].layHangBay()), StrToSys(ds[i].layTenHangVe()), StrToSys(ds[i].layChuoiHanhLy()), ds[i].layTongTien().ToString(L"N0"), StrToSys(ds[i].layTrangThai()), StrToSys(ds[i].layNgayDat()));
        }
    }
    void XuLyTimKiem(Object^ sender, EventArgs^ e) {
        std::string kw = SysToStr(txtTimKiem->Text);
        std::transform(kw.begin(), kw.end(), kw.begin(), ::toupper);
        if (kw.empty()) { DoDuLieuVaoGrid(heThongVe); return; }

        QuanLyDanhSach<VeMayBay> res;
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            std::string pnr = heThongVe[i].layPNR();
            std::transform(pnr.begin(), pnr.end(), pnr.begin(), ::toupper);
            std::string cccd = heThongVe[i].layThongTinCCCD().soCCCD;

            if (pnr.find(kw) != std::string::npos || cccd.find(kw) != std::string::npos) {
                res.them(heThongVe[i]);
            }
        }

        if (res.kichThuoc() == 0) {
            MessageBox::Show(L"Không tìm thấy vé với PNR hoặc CCCD: " + StrToSys(kw), L"Thông báo", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        DoDuLieuVaoGrid(res);
    }
    void XuLyDoiCho(Object^ sender, EventArgs^ e) {
        if (gridVe->SelectedRows->Count == 0) return; int idx = gridVe->SelectedRows[0]->Index;
        String^ pnrGrid = gridVe->Rows[idx]->Cells[1]->Value->ToString();
        String^ trangThai = gridVe->Rows[idx]->Cells[14]->Value->ToString();
        if (trangThai->Contains(L"Đã Hủy") || trangThai->Contains(L"Da Huy")) { MessageBox::Show(L"Không thể đổi vé hủy!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
        std::string pnrSua = SysToStr(pnrGrid);
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            if (heThongVe[i].layPNR() == pnrSua) {
                FormDoiCho^ f = gcnew FormDoiCho(StrToSys(heThongVe[i].layGheNgoi()));
                if (f->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                    std::string gheMoi = SysToStr(f->txtGheMoi->Text); if (gheMoi == heThongVe[i].layGheNgoi()) return;
                    if (QuanLyGhe::KiemTraGheTrong(heThongVe[i].laySoHieu(), heThongVe[i].layNgayGioBay(), gheMoi)) { heThongVe[i].capNhatGheNgoi(gheMoi); MessageBox::Show(L"Đổi chỗ thành công!", L"Thành công", MessageBoxButtons::OK, MessageBoxIcon::Information); DoDuLieuVaoGrid(heThongVe); }
                    else { MessageBox::Show(L"Ghế đã có người chọn!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error); }
                } break;
            }
        }
    }
    void XuLyHuyVe(Object^ sender, EventArgs^ e) {
        if (gridVe->SelectedRows->Count == 0) return; String^ pnrSys = gridVe->Rows[gridVe->SelectedRows[0]->Index]->Cells[1]->Value->ToString();
        if (MessageBox::Show(L"Hủy vé: " + pnrSys + L"?", L"Xác Nhận Hủy", MessageBoxButtons::YesNo, MessageBoxIcon::Warning) == System::Windows::Forms::DialogResult::Yes) {
            std::string pnrCancel = SysToStr(pnrSys);
            for (size_t i = 0; i < heThongVe.kichThuoc(); i++) { if (heThongVe[i].layPNR() == pnrCancel) { heThongVe[i].huyVeLyDo(userIsAdmin ? SysToStr(cbLyDoHuy->Text) : u8"Khách yêu cầu"); heThongVe[i].capNhatGheNgoi(""); MessageBox::Show(L"Đã hủy thành công!", L"Thông Báo", MessageBoxButtons::OK, MessageBoxIcon::Information); DoDuLieuVaoGrid(heThongVe); break; } }
        }
    }
    void XuLyXoaVeHuy(Object^ sender, EventArgs^ e) { if (MessageBox::Show(L"Xóa các vé đã hủy khỏi hệ thống?", L"Xóa vĩnh viễn", MessageBoxButtons::YesNo, MessageBoxIcon::Warning) == System::Windows::Forms::DialogResult::Yes) { heThongVe.xoaTheoDieuKien(KiemTraVeDaHuy); DoDuLieuVaoGrid(heThongVe); } }

    void XuLyTaoPhieuHangHoa(Object^ sender, EventArgs^ e) {
        std::string ten = SysToStr(txtNguoiGuiHH->Text); if (ten.empty() || cbSBDiHH->SelectedIndex < 0 || cbSBDenHH->SelectedIndex < 0 || cbSBDiHH->SelectedIndex == cbSBDenHH->SelectedIndex) { MessageBox::Show(L"Vui lòng điền đủ và đúng thông tin!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error); return; }
        double kl = 0.0; try { kl = Convert::ToDouble(txtKhoiLuongHH->Text); }
        catch (...) { MessageBox::Show(L"Khối lượng không hợp lệ!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error); return; } if (kl <= 0 || kl > 50) { MessageBox::Show(L"Khối lượng chỉ cho phép 0-50kg!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
        SanBay sbDi = DuLieuSanBay::layTheHien()->timKiem(SysToStr(cbSBDiHH->SelectedItem->ToString()).substr(0, 3));
        SanBay sbDen = DuLieuSanBay::layTheHien()->timKiem(SysToStr(cbSBDenHH->SelectedItem->ToString()).substr(0, 3));
        double km = DuLieuSanBay::layTheHien()->tinhKhoangCach(sbDi, sbDen);
        double tienCuoc = TinhGiaHangHoa::tinhToan(km, kl, cbLoaiKhachHH->SelectedIndex, cbLoaiHangHH->SelectedIndex, cbTocDoGiaoHH->SelectedIndex);
        if (MessageBox::Show(L"Tổng cước là: " + tienCuoc.ToString(L"N0") + L" VND. Tiến hành lập phiếu?", L"Xác Nhận", MessageBoxButtons::YesNo, MessageBoxIcon::Information) == System::Windows::Forms::DialogResult::Yes) {
            PhieuHangHoa ph; ph.maPhieu = "CGO" + std::to_string(100000 + rand() % 900000); ph.tenNguoiGui = ten; ph.sbDi = sbDi; ph.sbDen = sbDen; ph.khoangCach = km; ph.loaiKhach = SysToStr(cbLoaiKhachHH->Text); ph.loaiHang = SysToStr(cbLoaiHangHH->Text); ph.tocDoGiao = SysToStr(cbTocDoGiaoHH->Text); ph.trangThai = u8"Đã Tiếp Nhận"; ph.khoiLuong = kl; ph.tongTien = tienCuoc;
            time_t now = time(0); char buf[80]; strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", localtime(&now)); ph.ngayTao = std::string(buf);
            heThongHangHoa.them(ph); if (nhanVienHienTai) nhanVienHienTai->tangSoKhach(); MessageBox::Show(L"Đã lập phiếu hàng hóa!", L"Thành công", MessageBoxButtons::OK, MessageBoxIcon::Information); DoDuLieuHangHoaVaoGrid(); txtNguoiGuiHH->Clear();
        }
    }
    void XuLyHuyPhieuHH(Object^ sender, EventArgs^ e) {
        if (gridHangHoa->SelectedRows->Count == 0) return; int idx = gridHangHoa->SelectedRows[0]->Index; String^ maPhieu = gridHangHoa->Rows[idx]->Cells[1]->Value->ToString(); String^ tt = gridHangHoa->Rows[idx]->Cells[8]->Value->ToString();
        if (tt == L"Đã Hủy") return;
        if (MessageBox::Show(L"Hủy phiếu " + maPhieu + L"?", L"Xác nhận hủy", MessageBoxButtons::YesNo, MessageBoxIcon::Warning) == System::Windows::Forms::DialogResult::Yes) {
            std::string maSearch = SysToStr(maPhieu); for (size_t i = 0; i < heThongHangHoa.kichThuoc(); i++) { if (heThongHangHoa[i].maPhieu == maSearch) { heThongHangHoa[i].trangThai = u8"Đã Hủy"; DoDuLieuHangHoaVaoGrid(); break; } }
        }
    }
    void DoDuLieuHangHoaVaoGrid() {
        gridHangHoa->Rows->Clear();
        for (size_t i = 0; i < heThongHangHoa.kichThuoc(); i++) {
            gridHangHoa->Rows->Add((i + 1).ToString(), StrToSys(heThongHangHoa[i].maPhieu), StrToSys(heThongHangHoa[i].tenNguoiGui), StrToSys(heThongHangHoa[i].sbDi.iata + "-" + heThongHangHoa[i].sbDen.iata), StrToSys(heThongHangHoa[i].tocDoGiao), StrToSys(heThongHangHoa[i].loaiKhach + " | " + heThongHangHoa[i].loaiHang), heThongHangHoa[i].khoiLuong.ToString(), heThongHangHoa[i].tongTien.ToString(L"N0"), StrToSys(heThongHangHoa[i].trangThai), StrToSys(heThongHangHoa[i].ngayTao));
        }
    }

    void XuLyTimVeUM(Object^ sender, EventArgs^ e) {
        std::string kw = SysToStr(txtTimPNR_UM->Text); btnDangKyUM->Enabled = false; if (kw.empty()) return;
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            if (heThongVe[i].layPNR() == kw) {
                ThongTinCCCD tt = heThongVe[i].layThongTinCCCD();
                if (tt.soCCCD.find("PASS:") != std::string::npos) { lblThongTinUM->Text = L"Hộ chiếu, vui lòng check thủ công."; lblThongTinUM->ForeColor = Color::Orange; return; }
                if (tt.tuoi < 5) { lblThongTinUM->Text = L"Dưới 5 tuổi KHÔNG được đi UM!"; lblThongTinUM->ForeColor = Color::Red; }
                else if (tt.tuoi <= 12) { lblThongTinUM->Text = L"Hợp lệ UM."; lblThongTinUM->ForeColor = Color::Green; btnDangKyUM->Enabled = true; }
                else { lblThongTinUM->Text = L"Trên 12 tuổi không bắt buộc UM."; lblThongTinUM->ForeColor = Color::Orange; }
                return;
            }
        } lblThongTinUM->Text = L"Không tìm thấy vé!"; lblThongTinUM->ForeColor = Color::Red;
    }
    void XuLyDangKyUM(Object^ sender, EventArgs^ e) {
        std::string kw = SysToStr(txtTimPNR_UM->Text);
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            if (heThongVe[i].layPNR() == kw) { heThongVe[i].capNhatTrangThai(u8"Đã Đặt (+ UM)"); MessageBox::Show(L"Đã đăng ký UM thành công!", L"Thông Báo", MessageBoxButtons::OK, MessageBoxIcon::Information); btnDangKyUM->Enabled = false; return; }
        }
    }

    void LoadKhoChuyenBayVaoGrid() {
        gridQuanLyCB->Rows->Clear();
        for (size_t i = 0; i < khoChuyenBayToanCuc.kichThuoc(); i++) {
            ChuyenBay cb = khoChuyenBayToanCuc[i];
            gridQuanLyCB->Rows->Add((i + 1).ToString(), StrToSys(cb.maCB), StrToSys(cb.hangBay.tenHang), StrToSys(cb.sbDi.iata), StrToSys(cb.sbDen.iata), StrToSys(cb.dongMayBay), StrToSys(cb.thoiGianDi), StrToSys(cb.thoiGianDen), cb.giaNiemYet.ToString());
        }
    }
    void XuLyThemCBAdmin(Object^ sender, EventArgs^ e) {
        if (txtQ_MaCB->Text == "" || cbQ_HangBay->SelectedIndex < 0 || cbQ_SBDi->SelectedIndex < 0 || cbQ_SBDen->SelectedIndex < 0 || cbQ_SBDi->SelectedIndex == cbQ_SBDen->SelectedIndex) {
            MessageBox::Show(L"Thông tin điền không hợp lệ!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        }

        ChuyenBay cb;
        cb.maCB = SysToStr(txtQ_MaCB->Text);
        cb.hangBay = DuLieuHangKhong::layTheHien()->timKiemTheoTen(SysToStr(cbQ_HangBay->SelectedItem->ToString()));
        cb.sbDi = DuLieuSanBay::layTheHien()->timKiem(SysToStr(cbQ_SBDi->SelectedItem->ToString()).substr(0, 3));
        cb.sbDen = DuLieuSanBay::layTheHien()->timKiem(SysToStr(cbQ_SBDen->SelectedItem->ToString()).substr(0, 3));
        cb.dongMayBay = SysToStr(txtQ_MayBay->Text);
        cb.thoiGianDi = SysToStr(dtpQ_NgayDi->Value.ToString(L"dd/MM/yyyy") + " " + dtpQ_GioDi->Value.ToString(L"HH:mm"));
        cb.thoiGianDen = SysToStr(dtpQ_NgayDen->Value.ToString(L"dd/MM/yyyy") + " " + dtpQ_GioDen->Value.ToString(L"HH:mm"));

        try {
            cb.giaNiemYet = Convert::ToDouble(txtQ_Gia->Text);
        }
        catch (...) {
            cb.giaNiemYet = 1000000;
        }

        khoChuyenBayToanCuc.them(cb);
        LoadKhoChuyenBayVaoGrid();
        MessageBox::Show(L"Đã thêm kế hoạch bay mới!", L"Thành công", MessageBoxButtons::OK, MessageBoxIcon::Information);

        // Tự động dọn dẹp ô nhập liệu để nhập thủ công chuyến bay tiếp theo
        txtQ_MaCB->Clear();
        txtQ_MaCB->Enabled = true;
    }
    void XuLyThemTuDongCBAdmin(Object^ sender, EventArgs^ e) { Tao5ChuyenBayMacDinh(); LoadKhoChuyenBayVaoGrid(); MessageBox::Show(L"Đã tạo thêm 5 Cặp chuyến bay (Khứ hồi) ngẫu nhiên!", L"Tự Động Sinh", MessageBoxButtons::OK, MessageBoxIcon::Information); }
    void DienThongTinSuaCB(Object^ sender, EventArgs^ e) {
        if (gridQuanLyCB->SelectedRows->Count > 0) {
            int idx = gridQuanLyCB->SelectedRows[0]->Index;
            txtQ_MaCB->Text = gridQuanLyCB->Rows[idx]->Cells[1]->Value->ToString();
            // Thay vì false, gán lại bằng true để không bao giờ bị khóa khi click vào danh sách
            txtQ_MaCB->Enabled = true;
        }
    }
    void XuLySuaCBAdmin(Object^ sender, EventArgs^ e) { MessageBox::Show(L"Tính năng sửa đang được bảo trì an toàn (Logic cập nhật nội bộ)", L"Thông Báo", MessageBoxButtons::OK, MessageBoxIcon::Information); txtQ_MaCB->Enabled = true; }
    void XuLyXoaCBAdmin(Object^ sender, EventArgs^ e) { if (gridQuanLyCB->SelectedRows->Count > 0) { khoChuyenBayToanCuc.xoaTai(gridQuanLyCB->SelectedRows[0]->Index); LoadKhoChuyenBayVaoGrid(); txtQ_MaCB->Enabled = true; MessageBox::Show(L"Đã xóa chuyến bay khỏi hệ thống!", L"Hoàn tất", MessageBoxButtons::OK, MessageBoxIcon::Information); } }
    void XuLyCapNhatHeThongAdmin(Object^ sender, EventArgs^ e) { MessageBox::Show(L"Đã đồng bộ lại toàn bộ dữ liệu!", L"Đồng bộ", MessageBoxButtons::OK, MessageBoxIcon::Information); }

    void LoadMGGVaoGrid() {
        gridMGG->Rows->Clear(); DateTime now = DateTime::Now; int soHoatDong = 0;
        for (size_t i = 0; i < heThongMaGiamGia.kichThuoc(); i++) {
            MaGiamGia& mgg = heThongMaGiamGia[i]; bool conHan = true;
            try { DateTime dtEnd = DateTime::ParseExact(StrToSys(mgg.ngayHetHan + " " + mgg.gioHetHan), "dd/MM/yyyy HH:mm", System::Globalization::CultureInfo::InvariantCulture); if (now > dtEnd) conHan = false; }
            catch (...) { conHan = false; }
            bool conLuot = (mgg.soLuongDaDung < mgg.tongSoLuong);
            String^ trangThaiSys; if (!mgg.dangHoatDong) trangThaiSys = L"Đã tắt"; else if (!conHan) trangThaiSys = L"Hết hạn"; else if (!conLuot) trangThaiSys = L"Hết lượt"; else { trangThaiSys = L"✔ Hoạt động"; soHoatDong++; }
            int rowIdx = gridMGG->Rows->Add((i + 1).ToString(), StrToSys(mgg.maMGG), mgg.phanTramGiam.ToString("0") + "%", StrToSys(mgg.ngayBatDau + " " + mgg.gioBatDau + " - \n" + mgg.ngayHetHan + " " + mgg.gioHetHan), mgg.soLuongDaDung.ToString() + L"/" + mgg.tongSoLuong.ToString(), StrToSys(mgg.loaiVeApDung), StrToSys(mgg.moTa), trangThaiSys);
            if (!mgg.dangHoatDong || !conHan || !conLuot) { gridMGG->Rows[rowIdx]->DefaultCellStyle->BackColor = Color::FromArgb(240, 220, 220); gridMGG->Rows[rowIdx]->DefaultCellStyle->ForeColor = Color::Crimson; }
            else { gridMGG->Rows[rowIdx]->DefaultCellStyle->BackColor = Color::FromArgb(220, 255, 220); gridMGG->Rows[rowIdx]->DefaultCellStyle->ForeColor = Color::FromArgb(0, 120, 50); }
        } lblMGGStats->Text = L"Tổng mã: " + heThongMaGiamGia.kichThuoc().ToString() + L"  |  Đang hoạt động: " + soHoatDong.ToString();
    }
    void XuLyThemMGG(Object^ sender, EventArgs^ e) {
        std::string ma = SysToStr(txtMGG_Ma->Text); if (ma.empty()) return; double pct = 0; int sl = 0; try { pct = Convert::ToDouble(txtMGG_PhanTram->Text); sl = Convert::ToInt32(txtMGG_SoLuong->Text); }
        catch (...) { MessageBox::Show(L"Lỗi giá trị kiểu số!", L"Lỗi", MessageBoxButtons::OK, MessageBoxIcon::Error); return; }
        MaGiamGia mgg; mgg.maMGG = ma; mgg.phanTramGiam = pct; mgg.ngayBatDau = SysToStr(dtpMGG_NgayBD->Value.ToString(L"dd/MM/yyyy")); mgg.gioBatDau = SysToStr(dtpMGG_GioBD->Value.ToString(L"HH:mm")); mgg.ngayHetHan = SysToStr(dtpMGG_NgayHH->Value.ToString(L"dd/MM/yyyy")); mgg.gioHetHan = SysToStr(dtpMGG_GioHH->Value.ToString(L"HH:mm")); mgg.tongSoLuong = sl; mgg.soLuongDaDung = 0; mgg.loaiVeApDung = SysToStr(cbMGG_LoaiVe->Text); mgg.moTa = SysToStr(txtMGG_MoTa->Text); mgg.dangHoatDong = true;
        heThongMaGiamGia.them(mgg); LoadMGGVaoGrid(); MessageBox::Show(L"Đã phát hành mã giảm giá!", L"Thành công", MessageBoxButtons::OK, MessageBoxIcon::Information);
    }
    void XuLyXoaMGG(Object^ sender, EventArgs^ e) { if (gridMGG->SelectedRows->Count > 0) { heThongMaGiamGia.xoaTai(gridMGG->SelectedRows[0]->Index); LoadMGGVaoGrid(); } }
    void XuLyToggleMGG(Object^ sender, EventArgs^ e) { if (gridMGG->SelectedRows->Count > 0) { int idx = gridMGG->SelectedRows[0]->Index; heThongMaGiamGia[idx].dangHoatDong = !heThongMaGiamGia[idx].dangHoatDong; LoadMGGVaoGrid(); } }

    void LoadNhanSuVaoGrid() {
        gridNhanSu->Rows->Clear();
        for (size_t i = 0; i < danhSachNhanSu.kichThuoc(); i++) {
            gridNhanSu->Rows->Add((i + 1).ToString(), StrToSys(danhSachNhanSu[i].layMaNV()), StrToSys(danhSachNhanSu[i].layHoTen()), StrToSys(danhSachNhanSu[i].layChucVu()), danhSachNhanSu[i].laySoKhachPhucVu().ToString(), danhSachNhanSu[i].layTrangThai() ? L"✔ Hoạt Động" : L"🔒 Bị Khóa");
        }
    }
    void XuLyKhoaMoTaiKhoan(Object^ sender, EventArgs^ e) {
        if (gridNhanSu->SelectedRows->Count > 0) {
            int idx = gridNhanSu->SelectedRows[0]->Index;
            if (danhSachNhanSu[idx].layMaNV() == "ADMIN") { MessageBox::Show(L"Hệ thống từ chối khóa Quản Trị Viên cao nhất!", L"Bảo vệ", MessageBoxButtons::OK, MessageBoxIcon::Warning); return; }
            if (danhSachNhanSu[idx].layTrangThai()) danhSachNhanSu[idx].khoaTaiKhoan(); else danhSachNhanSu[idx].moTaiKhoan();
            LoadNhanSuVaoGrid();
        }
    }

    void VeDoThiKhuVuc() {
        if (picBieuDo->Image != nullptr) delete picBieuDo->Image; Bitmap^ bmp = gcnew Bitmap(picBieuDo->Width, picBieuDo->Height); Graphics^ g = Graphics::FromImage(bmp);
        double dtY = 0, dtC = 0, dtF = 0, dtHH = 0; int dat = 0, huy = 0; std::map<std::string, double> thongKeHang;
        for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
            double t = heThongVe[i].layTongTien(); std::string hang = heThongVe[i].layTenHangVe(); std::string tenHangBay = heThongVe[i].layHangBay();
            if (KiemTraVeDaHuy(heThongVe[i])) { huy++; }
            else { dat++; if (hang == u8"Economy") dtY += t; else if (hang == u8"Business") dtC += t; else dtF += t; thongKeHang[tenHangBay] += t; }
        }
        for (size_t i = 0; i < heThongHangHoa.kichThuoc(); i++) { if (heThongHangHoa[i].trangThai.find(u8"Đã Hủy") == std::string::npos) dtHH += heThongHangHoa[i].tongTien; }
        int selChart = userIsAdmin ? cbLoaiBieuDo->SelectedIndex : 0;
        if (selChart == 0) TrinhVeBieuDo::VeBieuDoCotCoBan(g, bmp->Width, bmp->Height, dtY, dtC, dtF, dtHH);
        else if (selChart == 1) TrinhVeBieuDo::VeBieuDoDoanhThuHangBay(g, bmp->Width, bmp->Height, thongKeHang);
        else if (selChart == 2) TrinhVeBieuDo::VeBieuDoTronTrangThaiVe(g, bmp->Width, bmp->Height, dat, huy);
        picBieuDo->Image = bmp;
    }
    void XuLyChuyenDoiBieuDo(Object^ sender, EventArgs^ e) { VeDoThiKhuVuc(); }

    void CapNhatTab(Object^ sender, EventArgs^ e) {
        if (tabCtrl->SelectedTab == tabDanhSach) DoDuLieuVaoGrid(heThongVe);
        else if (tabCtrl->SelectedTab == tabHangHoa) DoDuLieuHangHoaVaoGrid();
        else if (tabCtrl->SelectedTab == tabQuanLyMGG && userIsAdmin) LoadMGGVaoGrid();
        else if (tabCtrl->SelectedTab == tabNhanSu && userIsAdmin) LoadNhanSuVaoGrid();
        else if (tabCtrl->SelectedTab == tabThongKe) {
            double tongDT = 0; int veHuy = 0; double dtHH = 0;
            for (size_t i = 0; i < heThongVe.kichThuoc(); i++) { if (KiemTraVeDaHuy(heThongVe[i])) veHuy++; else tongDT += heThongVe[i].layTongTien(); }
            for (size_t i = 0; i < heThongHangHoa.kichThuoc(); i++) { if (heThongHangHoa[i].trangThai.find(u8"Đã Hủy") == std::string::npos) dtHH += heThongHangHoa[i].tongTien; }
            lblTongDoanhThu->Text = L"DOANH THU VÉ MÁY BAY: " + tongDT.ToString(L"N0") + L" VND  |  ĐÃ HỦY: " + veHuy.ToString() + L" Vé";
            lblDoanhThuHangHoa->Text = L"DOANH THU HÀNG HÓA: " + dtHH.ToString(L"N0") + L" VND";
            VeDoThiKhuVuc();
        }
    }

    void XuLyLuuFile(Object^ sender, EventArgs^ e) {
        try {
            std::ofstream f("DuLieuHeThong.csv", std::ios::out | std::ios::trunc | std::ios::binary); if (!f.is_open()) throw std::ios_base::failure("Error"); f << "\xEF\xBB\xBF";
            f << "STT,PNR,So Hieu,Ghe Ngoi,Ho Ten,CCCD,Hang Ve,Tong Tien,Trang Thai\n";
            for (size_t i = 0; i < heThongVe.kichThuoc(); i++) f << (i + 1) << "," << heThongVe[i].layPNR() << "," << heThongVe[i].laySoHieu() << "," << heThongVe[i].layGheNgoi() << "," << heThongVe[i].layTenKhach() << "," << heThongVe[i].layThongTinCCCD().soCCCD << "," << heThongVe[i].layTenHangVe() << "," << heThongVe[i].layTongTien() << "," << heThongVe[i].layTrangThai() << "\n";
            f.close(); MessageBox::Show(L"Xuất file thành công!", L"Lưu CSV", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (...) { MessageBox::Show(L"Lỗi truy xuất ghi File!", L"Lỗi I/O", MessageBoxButtons::OK, MessageBoxIcon::Error); }
    }

    void XuLyXuatCSVHangBay(Object^ sender, EventArgs^ e) {
        try {
            std::map<std::string, std::pair<int, double>> thongKe;
            for (size_t i = 0; i < heThongVe.kichThuoc(); i++) {
                if (!KiemTraVeDaHuy(heThongVe[i])) {
                    thongKe[heThongVe[i].layHangBay()].first++;
                    thongKe[heThongVe[i].layHangBay()].second += heThongVe[i].layTongTien();
                }
            }
            std::ofstream f("DoanhThuHangBay.csv", std::ios::out | std::ios::trunc | std::ios::binary); if (!f.is_open()) throw std::ios_base::failure("Error"); f << "\xEF\xBB\xBF";
            f << "STT,Ten Hang,So Ve,Tong Doanh Thu\n"; int stt = 1;
            for (auto const& pair : thongKe) { f << stt++ << "," << pair.first << "," << pair.second.first << "," << std::fixed << std::setprecision(0) << pair.second.second << "\n"; }
            f.close(); MessageBox::Show(L"Đã xuất DoanhThuHangBay.csv!", L"Lưu CSV", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (...) { MessageBox::Show(L"Lỗi truy xuất ghi File!", L"Lỗi I/O", MessageBoxButtons::OK, MessageBoxIcon::Error); }
    }

    void XuLyDoiQuyen(Object^ sender, EventArgs^ e) {
        if (MessageBox::Show(L"Bạn có muốn Đăng Xuất tài khoản hiện tại (" + StrToSys(nhanVienHienTai->layMaNV()) + L") để đổi quyền không?", L"Xác nhận", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
            LoginForm^ login = gcnew LoginForm();
            this->Hide();
            if (login->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                CapNhatGiaoDienTheoQuyen();
                this->Show();
            }
            else {
                Application::Exit();
            }
        }
    }

public:
    MainForm() { KhoiTaoUI(); }
    ~MainForm() { delete dsChuyenBayHienTai; }
};

// ========================== ENTRY POINT ==========================
[STAThreadAttribute]
int main(array<String^>^ args) {
    srand((unsigned)time(NULL));
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    KhoiTaoDuLieuHeThong();

    LoginForm^ login = gcnew LoginForm();
    if (login->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        Application::Run(gcnew MainForm());
    }
    return 0;
}
