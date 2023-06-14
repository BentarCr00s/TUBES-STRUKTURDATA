#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

using namespace std;

struct Movie {
    int id;
    string title;
    string genre;
};

struct Node {
    string genre;
    vector<Node*> children;
};

// Fungsi untuk merekomendasikan film berdasarkan rating pengguna
vector<pair<string, double>> recommendMovies(int userId, const unordered_map<int, unordered_map<int, double>>& userRatings,
    const vector<Movie>& movies, int numRecs)
{
    unordered_map<int, double> movieScores;
    for (const auto& userRating : userRatings[userId])
    {
        int movieId = userRating.first;
        double rating = userRating.second;
        for (const Movie& movie : movies)
        {
            if (movie.id == movieId)
            {
                movieScores[movieId] += rating;
            }
        }
    }

    vector<pair<string, double>> recommendations;
    for (const auto& score : movieScores)
    {
        int movieId = score.first;
        double totalScore = score.second;
        recommendations.push_back({ movies[movieId - 1].title, totalScore });
    }
    sort(recommendations.begin(), recommendations.end(), [](const pair<string, double>& p1, const pair<string, double>& p2) {
        return p1.second > p2.second;
    });
    vector<pair<string, double>> topRecs(recommendations.begin(), recommendations.begin() + numRecs);

    return topRecs;
}

// Fungsi untuk membangun pohon kategori genre dari daftar film
Node* buildGenreTree(const vector<Movie>& movies)
{
    unordered_map<string, Node*> genreMap;

    Node* root = nullptr;
    for (const Movie& movie : movies)
    {
        istringstream iss(movie.genre);
        string genre;
        while (getline(iss, genre, '|'))
        {
            if (genreMap.find(genre) == genreMap.end())
            {
                Node* newNode = new Node;
                newNode->genre = genre;
                genreMap[genre] = newNode;
                if (root == nullptr)
                {
                    root = newNode;
                }
            }
        }
    }

    // Menghubungkan parent-child dalam pohon
    for (const Movie& movie : movies)
    {
        istringstream iss(movie.genre);
        string genre;
        Node* parentNode = nullptr;
        while (getline(iss, genre, '|'))
        {
            Node* currentNode = genreMap[genre];
            if (parentNode != nullptr)
            {
                parentNode->children.push_back(currentNode);
            }
            parentNode = currentNode;
        }
    }

    return root;
}

// Fungsi untuk menampilkan pohon kategori genre secara rekursif
void printGenreTree(Node* node, int depth = 0)
{
    if (node == nullptr)
    {
        return;
    }

    cout << string(depth, ' ') << "- " << node->genre << endl;

    for (Node* child : node->children)
    {
        printGenreTree(child, depth + 2);
    }
}

// Fungsi untuk memfilter input hanya berupa string
string filterStringInput()
{
    string input;
    getline(cin, input);

    // Menghapus karakter non-alfabet
    input.erase(remove_if(input.begin(), input.end(), [](char c) {
        return !isalpha(c);
    }), input.end());

    return input;
}

// Fungsi untuk memfilter input hanya berupa angka
int filterNumberInput()
{
    string input;
    getline(cin, input);

    // Menghapus karakter non-digit
    input.erase(remove_if(input.begin(), input.end(), [](char c) {
        return !isdigit(c);
    }), input.end());

    return stoi(input);
}

int main()
{
    // Membaca file movies.csv
    ifstream movieFile("movies.csv");
    if (!movieFile)
    {
        cout << "File movies.csv tidak dapat dibuka." << endl;
        return 1;
    }

    vector<Movie> movies;
    string line;
    getline(movieFile, line); // Mengabaikan baris header
    while (getline(movieFile, line))
    {
        istringstream iss(line);
        string movieId, title, genre;
        getline(iss, movieId, ',');
        getline(iss, title, ',');
        getline(iss, genre, ',');
        Movie movie;
        movie.id = stoi(movieId);
        movie.title = title;
        movie.genre = genre;
        movies.push_back(movie);
    }
    movieFile.close();

    // Membaca file ratings.csv
    ifstream ratingFile("ratings.csv");
    if (!ratingFile)
    {
        cout << "File ratings.csv tidak dapat dibuka." << endl;
        return 1;
    }

    unordered_map<int, unordered_map<int, double>> userRatings;
    getline(ratingFile, line); // Mengabaikan baris header
    while (getline(ratingFile, line))
    {
        istringstream iss(line);
        string userId, movieId, rating;
        getline(iss, userId, ',');
        getline(iss, movieId, ',');
        getline(iss, rating, ',');
        int uId = stoi(userId);
        int mId = stoi(movieId);
        double r = stod(rating);
        userRatings[uId][mId] = r;
    }
    ratingFile.close();

    // Membangun pohon kategori genre
    Node* genreTree = buildGenreTree(movies);

    int choice = -1;
    while (choice != 0)
    {
        cout << "Menu:" << endl;
        cout << "1. Tampilkan Genre Tree" << endl;
        cout << "2. Rekomendasikan Film" << endl;
        cout << "3. Tambahkan Data Film" << endl;
        cout << "4. Tambahkan Data Rating" << endl;
        cout << "0. Keluar" << endl;
        cout << "Pilihan Anda: ";
        choice = filterNumberInput();
        cout << endl;

        if (choice == 1)
        {
            cout << "Pohon Kategori Genre:" << endl;
            printGenreTree(genreTree);
            cout << endl;
        }
        else if (choice == 2)
        {
            cout << "Masukkan ID Pengguna: ";
            int userId = filterNumberInput();
            cout << "Masukkan Jumlah Rekomendasi Film: ";
            int numRecs = filterNumberInput();

            // Merekomendasikan film berdasarkan rating pengguna
            vector<pair<string, double>> recommendations = recommendMovies(userId, userRatings, movies, numRecs);

            cout << endl;
            cout << "Rekomendasi Film untuk Pengguna dengan ID " << userId << ":" << endl;
            for (const auto& recommendation : recommendations)
            {
                cout << recommendation.first << endl;
            }
            cout << endl;
        }
        else if (choice == 3)
        {
            cout << "Masukkan ID Film: ";
            int movieId = filterNumberInput();
            cout << "Masukkan Judul Film: ";
            string title = filterStringInput();
            cout << "Masukkan Genre Film (pisahkan dengan '|'): ";
            string genre = filterStringInput();

            Movie movie;
            movie.id = movieId;
            movie.title = title;
            movie.genre = genre;

            movies.push_back(movie);
            cout << "Data Film telah ditambahkan." << endl;
            cout << endl;
        }
        else if (choice == 4)
        {
            cout << "Masukkan ID Pengguna: ";
            int userId = filterNumberInput();
            cout << "Masukkan ID Film: ";
            int movieId = filterNumberInput();
            cout << "Masukkan Rating Pengguna: ";
            double rating = filterNumberInput();

            userRatings[userId][movieId] = rating;
            cout << "Data Rating telah ditambahkan." << endl;
            cout << endl;
        }
        else if (choice == 0)
        {
            cout << "Terima kasih telah menggunakan program ini. Sampai jumpa!" << endl;
        }
        else
        {
            cout << "Pilihan tidak valid. Silakan pilih menu yang tersedia." << endl;
            cout << endl;
        }
    }

    // Menghapus pohon kategori genre
    // ...

    return 0;
}
