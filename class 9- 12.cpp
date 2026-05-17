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

    // Thu?c tính m?i cho vé Kh? H?i
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
            tongTien *= 2; // Nhân dôi t?ng ti?n cho 2 chi?u
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

