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
            std::ofstream fileOut("DuLieuHeThong.csv", std::ios::out | std::ios::trunc);

            if (!fileOut.is_open()) {
                throw std::ios_base::failure("Khong the truy cap file de ghi du lieu (Kiem tra quyen admin)!");
            }

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
