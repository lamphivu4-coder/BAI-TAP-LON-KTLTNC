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
//Class 3
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
