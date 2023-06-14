#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

using namespace std;

// Struktur data untuk merepresentasikan informasi film
struct Movie
{
    string title;
    string genre;
    unordered_map<int, double> ratings;

    Movie(string t, string g) : title(t), genre(g) {}
};

// Fungsi untuk memisahkan string berdasarkan delimiter
vector<string> split(const string &s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);

    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

// Fungsi untuk membaca data film dari file CSV
unordered_map<int, Movie *> readMoviesFromFile(const string &filename)
{
    unordered_map<int, Movie *> movies;
    ifstream file(filename);

    if (file)
    {
        string line;

        while (getline(file, line))
        {
            vector<string> tokens = split(line, ',');
            if (tokens.size() >= 3)
            {
                int movieId = stoi(tokens[0]);
                string title = tokens[1];
                string genre = tokens[2];

                Movie *movie = new Movie(title, genre);
                movies[movieId] = movie;
            }
        }

        file.close();
    }
    else
    {
        cerr << "Gagal membuka file " << filename << endl;
    }

    return movies;
}

// Fungsi untuk membaca data rating pengguna dari file CSV
unordered_map<int, unordered_map<int, double>> readRatingsFromFile(const string &filename)
{
    unordered_map<int, unordered_map<int, double>> ratings;
    ifstream file(filename);

    if (file)
    {
        string line;

        while (getline(file, line))
        {
            vector<string> tokens = split(line, ',');
            if (tokens.size() >= 3)
            {
                int userId = stoi(tokens[0]);
                int movieId = stoi(tokens[1]);
                double rating = stod(tokens[2]);

                ratings[userId][movieId] = rating;
            }
        }

        file.close();
    }
    else
    {
        cerr << "Gagal membuka file " << filename << endl;
    }

    return ratings;
}

// Fungsi untuk mendapatkan rekomendasi film berdasarkan kesamaan rating
// Fungsi untuk mendapatkan rekomendasi film berdasarkan film yang dipilih
vector<Movie *> getRecommendedMovies(const Movie *movie, const unordered_map<int, Movie *> &movies, int numRecommendations)
{
    vector<pair<Movie *, double>> movieScores;

    // Menghitung skor kesamaan rating untuk setiap film
    for (const auto &pair : movies)
    {
        const Movie *otherMovie = pair.second;

        if (otherMovie != movie)
        {
            double score = calculateRatingSimilarity(movie, otherMovie);
            movieScores.push_back(make_pair(otherMovie, score));
        }
    }

    // Mengurutkan film-film berdasarkan skor kesamaan rating
    sort(movieScores.begin(), movieScores.end(), [](const pair<Movie *, double> &a, const pair<Movie *, double> &b)
         { return a.second > b.second; });

    // Mengambil sejumlah film teratas sebagai rekomendasi
    vector<Movie *> recommendations;
    for (int i = 0; i < min(numRecommendations, static_cast<int>(movieScores.size())); ++i)
    {
        recommendations.push_back(movieScores[i].first);
    }

    return recommendations;
}

// Fungsi untuk menghitung kesamaan rating antara dua film
double calculateRatingSimilarity(const Movie *movie1, const Movie *movie2)
{
    double similarity = 0.0;
    int commonRatings = 0;

    for (const auto &pair : movie1->ratings)
    {
        int userId = pair.first;
        double rating1 = pair.second;

        if (movie2->ratings.find(userId) != movie2->ratings.end())
        {
            double rating2 = movie2->ratings.at(userId);
            similarity += abs(rating1 - rating2);
            commonRatings++;
        }
    }

    if (commonRatings > 0)
    {
        similarity /= commonRatings;
    }

    return similarity;
}

// Fungsi untuk menampilkan menu
void displayMenu()
{
    cout << "==== Menu ====" << endl;
    cout << "1. Tampilkan semua film" << endl;
    cout << "2. Tampilkan semua rating pengguna" << endl;
    cout << "3. Rekomendasi film berdasarkan ID film" << endl;
    cout << "0. Keluar" << endl;
}

int main()
{
    // Membaca data film dari file CSV
    unordered_map<int, Movie *> movies = readMoviesFromFile("movies.csv");

    // Membaca data rating pengguna dari file CSV
    unordered_map<int, unordered_map<int, double>> ratings = readRatingsFromFile("ratings.csv");

    // Menambahkan rating pengguna ke film-film yang sesuai
    for (const auto &userPair : ratings)
    {
        int userId = userPair.first;
        const auto &userRatings = userPair.second;

        for (const auto &ratingPair : userRatings)
        {
            int movieId = ratingPair.first;
            double rating = ratingPair.second;

            if (movies.find(movieId) != movies.end())
            {
                Movie *movie = movies[movieId];
                movie->ratings[userId] = rating;
            }
        }
    }

    int choice;

    do
    {
        displayMenu();
        cout << "Pilih menu: ";
        cin >> choice;
        cout << endl;

        switch (choice)
        {
        case 1:
        {
            cout << "Data Film:" << endl;
            for (const auto &pair : movies)
            {
                int movieId = pair.first;
                Movie *movie = pair.second;

                cout << "Movie ID: " << movieId << ", Title: " << movie->title << ", Genre: " << movie->genre << endl;
            }

            cout << endl;
            break;
        }
        case 2:
        {
            cout << "Data Rating Pengguna:" << endl;
            for (const auto &userPair : ratings)
            {
                int userId = userPair.first;
                const auto &userRatings = userPair.second;

                for (const auto &ratingPair : userRatings)
                {
                    int movieId = ratingPair.first;
                    double rating = ratingPair.second;

                    cout << "User ID: " << userId << ", Movie ID: " << movieId << ", Rating: " << rating << endl;
                }
            }

            cout << endl;
            break;
        }
        case 3:
        {
            int movieIdToRecommend;
            int numRecommendations;

            cout << "Masukkan ID film yang ingin direkomendasikan: ";
            cin >> movieIdToRecommend;
            cout << "Masukkan jumlah rekomendasi yang diinginkan: ";
            cin >> numRecommendations;

            if (movies.find(movieIdToRecommend) != movies.end())
            {
                Movie *movieToRecommend = movies[movieIdToRecommend];
                vector<Movie *> recommendations = getRecommendedMovies(movieToRecommend, movies, numRecommendations);

                cout << "Rekomendasi Film berdasarkan Kesamaan Rating untuk Film dengan ID " << movieIdToRecommend << ":" << endl;
                for (const auto &recommendation : recommendations)
                {
                    cout << "Movie ID: " << movieIdToRecommend << ", Title: " << recommendation->title << ", Genre: " << recommendation->genre << endl;
                }
            }

            cout << endl;
            break;
        }
        case 0:
            cout << "Terima kasih. Program selesai." << endl;
            break;
        default:
            cout << "Menu tidak valid. Silakan pilih menu yang valid." << endl;
            cout << endl;
            break;
        }
    } while (choice != 0);

    // Menghapus data film
    for (const auto &pair : movies)
    {
        delete pair.second;
    }

    return 0;
}
