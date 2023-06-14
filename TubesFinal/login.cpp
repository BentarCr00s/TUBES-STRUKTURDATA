#include <iostream>
#include <string>
#include <conio.h>
using namespace std;

bool isPasswordCorrect(const string &enteredPassword, const string &storedPassword)
{
    // Memeriksa apakah password yang dimasukkan sesuai dengan yang disimpan
    return enteredPassword == storedPassword;
}

void login()
{
    string username = "admin";
    string password = "password123";

    string enteredUsername;
    string enteredPassword;

    cout << "Username: ";
    cin >> enteredUsername;

    cout << "Password: ";
    // Menggunakan loop untuk memformat input password dengan bintang-bintang
    char c;
    while ((c = _getch()) != '\r')
    { // Menghentikan loop ketika tombol Enter ditekan
        if (c == '\b')
        {
            if (!enteredPassword.empty())
            {
                cout << "\b \b"; // Menghapus satu karakter dari output
                enteredPassword.pop_back();
            }
        }
        else
        {
            cout << "*";
            enteredPassword.push_back(c);
        }
    }
    cout << endl;

    if (enteredUsername == username && isPasswordCorrect(enteredPassword, password))
    {
        cout << "Login berhasil!" << endl;
    }
    else
    {
        cout << "Username atau password salah." << endl;
    }
}

int main()
{
    login();
    return 0;
}
