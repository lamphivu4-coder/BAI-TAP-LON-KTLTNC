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
