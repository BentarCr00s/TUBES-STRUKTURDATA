#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Fungsi untuk menghasilkan QR Code dari teks
void generateQRCode(const string &text)
{
    // Konversi teks menjadi data biner
    vector<bool> data;
    for (char c : text)
    {
        for (int i = 7; i >= 0; --i)
        {
            data.push_back((c >> i) & 1);
        }
    }

    // Menentukan ukuran QR Code
    int size = 21; // Ukuran default
    if (data.size() <= 152)
    {
        size = 21;
    }
    else if (data.size() <= 272)
    {
        size = 25;
    }
    else if (data.size() <= 440)
    {
        size = 29;
    }

    // Menginisialisasi matriks QR Code dengan modul putih
    vector<vector<bool>> qrCode(size, vector<bool>(size, false));

    // Menentukan pola penanda
    for (int i = 0; i < 7; ++i)
    {
        qrCode[i][0] = qrCode[0][i] = qrCode[i][6] = qrCode[6][i] = true;
        qrCode[i][size - 1] = qrCode[size - 1][i] = qrCode[size - 7 + i][0] = true;
        qrCode[0][size - 7 + i] = qrCode[i][size - 7] = qrCode[size - 7][i] = true;
    }

    // Menempatkan data ke dalam QR Code
    int dataIndex = 0;
    for (int y = size - 1; y >= 0; y -= 2)
    {
        for (int x = size - 1; x >= 0; --x)
        {
            if (!qrCode[y][x] && dataIndex < data.size())
            {
                qrCode[y][x] = data[dataIndex++];
            }
        }
        --y;
        if (y >= 0)
        {
            for (int x = 0; x < size; ++x)
            {
                if (!qrCode[y][x] && dataIndex < data.size())
                {
                    qrCode[y][x] = data[dataIndex++];
                }
            }
        }
    }

    // Menampilkan QR Code
    for (const auto &row : qrCode)
    {
        for (bool module : row)
        {
            cout << (module ? "##" : "  "); // Karakter untuk modul hitam dan putih
        }
        cout << endl;
    }
}

int main()
{
    string text = "Hello, World!";
    generateQRCode(text);
    return 0;
}
