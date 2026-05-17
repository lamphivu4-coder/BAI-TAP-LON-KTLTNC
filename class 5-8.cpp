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
//Class 7
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
