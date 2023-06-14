#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stack>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

using namespace std;

struct Movie
{
    int id;
    string title;
    string genre;
};

struct Rating
{
    int userId;
    int movieId;
    double rating;
};

// Struktur Node Pohon
struct TreeNode
{
    int movieId;
    double score;
    TreeNode *left;
    TreeNode *right;

    TreeNode(int id, double s) : movieId(id), score(s), left(nullptr), right(nullptr) {}
};

// Fungsi untuk menghitung similarity antar user
double similarity(const unordered_map<int, double> &user1, const unordered_map<int, double> &user2)
{
    double dotProduct = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;

    for (const auto &p1 : user1)
    {
        int movieId = p1.first;
        double rating1 = p1.second;
        if (user2.count(movieId))
        {
            double rating2 = user2.at(movieId);
            dotProduct += rating1 * rating2;
        }
        norm1 += rating1 * rating1;
    }

    for (const auto &p2 : user2)
    {
        double rating2 = p2.second;
        norm2 += rating2 * rating2;
    }

    if (norm1 == 0 || norm2 == 0)
    {
        return 0.0;
    }
    else
    {
        return dotProduct / (sqrt(norm1) * sqrt(norm2));
    }
}

// Fungsi untuk memasukkan skor film ke dalam pohon
void insertMovie(TreeNode *&root, int movieId, double score)
{
    if (root == nullptr)
    {
        root = new TreeNode(movieId, score);
        return;
    }

    if (score > root->score)
    {
        insertMovie(root->right, movieId, score);
    }
    else
    {
        insertMovie(root->left, movieId, score);
    }
}

// Fungsi rekursif untuk mendapatkan film dengan skor tertinggi dari pohon
void getTopMovies(TreeNode *root, vector<pair<int, double>> &topMovies, int numRecs)
{
    if (root == nullptr || topMovies.size() >= static_cast<size_t>(numRecs))
    {
        return;
    }

    getTopMovies(root->right, topMovies, numRecs);
    topMovies.push_back({root->movieId, root->score});
    getTopMovies(root->left, topMovies, numRecs);
}

// Fungsi untuk merekomendasikan film berdasarkan rating dari user-user yang memiliki similarity yang tinggi
vector<pair<int, double>> recommendMovies(int userId, const unordered_map<int, unordered_map<int, double>> &userRatings, int numRecs)
{
    const unordered_map<int, double> &user1 = userRatings.at(userId);

    // Menghitung similarity antar user
    vector<pair<int, double>> similarities;
    for (const auto &p2 : userRatings)
    {
        int userId2 = p2.first;
        if (userId2 != userId)
        {
            double sim = similarity(user1, p2.second);
            similarities.push_back({userId2, sim});
        }
    }

    // Mengurutkan user berdasarkan similarity
    sort(similarities.begin(), similarities.end(), [](const pair<int, double> &p1, const pair<int, double> &p2)
         { return p1.second > p2.second; });

    // Merekomendasikan film
    unordered_map<int, double> movieScores;
    for (const auto &p : similarities)
    {
        int userId2 = p.first;
        double sim = p.second;
        const unordered_map<int, double> &user2 = userRatings.at(userId2);
        for (const auto &p2 : user2)
        {
            int movieId = p2.first;
            double rating = p2.second;
            if (!user1.count(movieId))
            {
                if (!movieScores.count(movieId))
                {
                    movieScores[movieId] = 0.0;
                }
                movieScores[movieId] += rating * sim;
            }
        }
    }

    // Membangun pohon untuk film dan skor
    TreeNode *root = nullptr;
    for (const auto &p : movieScores)
    {
        int movieId = p.first;
        double score = p.second;
        insertMovie(root, movieId, score);
    }

    // Mendapatkan film dengan skor tertinggi dari pohon
    vector<pair<int, double>> recommendations;
    getTopMovies(root, recommendations, numRecs);

    // Menghapus pohon
    // Fungsi untuk menghapus pohon bisa ditambahkan di sini jika diperlukan

    return recommendations;
}

// Fungsi untuk menampilkan semua film
void displayAllMovies(const vector<Movie> &movies)
{
    cout << "All Movies:" << endl;
    for (const Movie &movie : movies)
    {
        cout << movie.id << " - " << movie.title << " (" << movie.genre << ")" << endl;
    }
    cout << endl;
}

// Fungsi untuk menampilkan semua rating pengguna
void displayAllRatings(const vector<Rating> &ratings)
{
    cout << "All Ratings:" << endl;
    for (const Rating &rating : ratings)
    {
        cout << "User ID: " << rating.userId << ", Movie ID: " << rating.movieId << ", Rating: " << rating.rating << endl;
    }
    cout << endl;
}

// Fungsi untuk menampilkan rekomendasi film berdasarkan ID Pengguna
void displayRecommendations(int userId, const vector<pair<int, double>> &recommendations, const vector<Movie> &movies)
{
    string genre;
    cout << "Recommended movies for user " << userId << ":" << endl;
    for (const auto &rec : recommendations)
    {
        int movieId = rec.first;
        double score = rec.second;
        cout << movies[movieId - 1].title << " (" << movies[movieId - 1].genre << "): " << score << endl;
    }
    cin >> genre;
    cout << endl;
    for (const auto &rec : recommendations)
    {
        int movieId = rec.first;
        double score = rec.second;

        if (movies[movieId - 1].genre == genre) // ga boleh sirkuit
        {
            cout << movies[movieId - 1].title << " (" << movies[movieId - 1].genre << "): " << score << endl;
        }
    }
    cout << endl;
}
void addmovie()
{
    string csvFileName = "movies.csv";
    ifstream inputFile(csvFileName);

    // Membaca seluruh isi file CSV ke dalam string
    stringstream buffer;
    buffer << inputFile.rdbuf();
    inputFile.close();
    string fileContents = buffer.str();

    // Menghitung movieid terakhir yang ada dalam file
    int lastMovieId = 0;
    stringstream ss(fileContents);
    string line;
    while (getline(ss, line))
    {
        stringstream lineStream(line);
        string movieIdStr;
        getline(lineStream, movieIdStr, ',');
        int movieId = stoi(movieIdStr);
        if (movieId > lastMovieId)
        {
            lastMovieId = movieId;
        }
    }

    // Menambahkan data baru
    string movie, genre;
    cout << "Masukkan nama film: ";
    cin.ignore();
    getline(cin, movie);
    cout << "Masukkan genre film: ";
    cin.ignore();
    getline(cin, genre);

    // Membuat movieid baru
    int newMovieId = lastMovieId + 1;

    // Menambahkan data baru ke string fileContents
    stringstream newLine;
    newLine << newMovieId << "," << movie << "," << genre << endl;
    fileContents += newLine.str();

    // Menulis kembali fileContents ke file CSV
    ofstream outputFile(csvFileName);
    outputFile << fileContents;
    outputFile.close();

    cout << "Data film berhasil ditambahkan di baris terakhir file CSV." << endl;
}
// Fungsi untuk menampilkan film terpopuler
void displayTopMovies(const vector<Movie> &movies, const vector<Rating> &ratings, int numMovies)
{
    unordered_map<int, int> movieCounts;
    for (const Rating &rating : ratings)
    {
        int movieId = rating.movieId;
        movieCounts[movieId]++;
    }

    vector<pair<int, int>> movieCountsVector(movieCounts.begin(), movieCounts.end());
    sort(movieCountsVector.begin(), movieCountsVector.end(), [](const pair<int, int> &p1, const pair<int, int> &p2)
         { return p1.second > p2.second; });

    cout << "Top " << numMovies << " Movies:" << endl;
    for (int i = 0; i < numMovies && i < movieCountsVector.size(); ++i)
    {
        int movieId = movieCountsVector[i].first;
        int count = movieCountsVector[i].second;
        cout << movies[movieId - 1].title << " (" << movies[movieId - 1].genre << "): " << count << " ratings" << endl;
    }
    cout << endl;
}

void addRating(vector<Rating> &ratings, unordered_map<int, unordered_map<int, double>> &userRatings, int userId, int movieId, float rating)
{
    // Check if the movie has been previously rated by the user
    if (userRatings[userId].count(movieId) > 0)
    {
        // Remove the old rating
        ratings.erase(
            remove_if(ratings.begin(), ratings.end(), [&](const Rating &r)
                      { return r.userId == userId && r.movieId == movieId; }),
            ratings.end());
    }

    // Add the new rating
    Rating newRating = {userId, movieId, rating};
    ratings.push_back(newRating);
    userRatings[userId][movieId] = rating;

    // Save the updated ratings to the file
    ofstream ratingFile("ratings.csv");
    if (ratingFile.is_open())
    {
        // Write all the ratings to the file
        for (const Rating &r : ratings)
        {
            ratingFile << r.userId << "," << r.movieId << "," << r.rating << endl;
        }
        ratingFile.close();
        cout << "Rating added successfully." << endl;
    }
    else
    {
        cerr << "Failed to open ratings.csv for writing." << endl;
    }
}

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

    int attempts = 0;
    const int maxAttempts = 3;

    while (attempts < maxAttempts)
    {
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
            return; // Keluar dari fungsi login setelah berhasil login
        }
        else
        {
            cout << "Username atau password salah. Silakan coba lagi." << endl;
            enteredPassword.clear(); // Menghapus password yang telah dimasukkan
            attempts++;
        }
    }

    cout << "Anda telah mencapai batas maksimal percobaan login. Exit Program." << endl;
    exit(0);
}
int main()
{
    vector<Movie> movies;
    vector<Rating> ratings;
    unordered_map<int, unordered_map<int, double>> userRatings;
    login();

    ifstream movieFile("movies.csv");
    if (!movieFile)
    {
        cerr << "Failed to open movies.csv" << endl;
        return 1;
    }

    string line;
    while (getline(movieFile, line))
    {
        stringstream ss(line);
        string id, title, genre;
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, genre, ',');
        movies.push_back({stoi(id), title, genre});
    }
    movieFile.close();

    // Membaca data rating dari file
    ifstream ratingFile("ratings.csv");
    if (!ratingFile)
    {
        cerr << "Failed to open ratings.csv" << endl;
        return 1;
    }

    while (getline(ratingFile, line))
    {
        stringstream ss(line);
        string userId, movieId, rating;
        getline(ss, userId, ',');
        getline(ss, movieId, ',');
        getline(ss, rating, ',');
        int uid = stoi(userId);
        int mid = stoi(movieId);
        float rtng = stof(rating);           // Mengubah rating menjadi float
        Rating newRating = {uid, mid, rtng}; // Membuat objek Rating baru
        ratings.push_back(newRating);        // Menambahkan objek Rating ke vektor ratings
        userRatings[uid][mid] = rtng;
    }
    ratingFile.close();

    // Menu interaktif
    int choice = 0;
    while (choice != 6)
    {
        cout << "Menu:" << endl;
        cout << "1. Display all movies" << endl;
        cout << "2. Display top movies" << endl;
        cout << "3. Recommend movies" << endl;
        cout << "4. Add rating" << endl;
        cout << "5. Add Movies" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";

        try
        {
            cin >> choice;
            if (cin.fail())
            {
                throw runtime_error("Invalid input! Please enter a valid choice.");
            }

            switch (choice)
            {
            case 1:
                // Menampilkan semua film
                displayAllMovies(movies);
                break;
            case 2:
                // Menampilkan film terpopuler
                {
                    int numMovies = 5;
                    displayTopMovies(movies, ratings, numMovies);
                }
                break;
            case 3:
                // Rekomendasi film
                {
                    int userId, numRecs;
                    cout << "Enter user ID (between 1 and " << userRatings.size() << "): ";
                    cin >> userId;
                    if (cin.fail())
                    {
                        throw runtime_error("Invalid user ID! Please enter a valid ID.");
                    }
                    if (userId < 1 || userId > userRatings.size())
                    {
                        throw runtime_error("User ID is out of range! Please enter a valid user ID.");
                    }

                    numRecs = 99;

                    vector<pair<int, double>> recommendations = recommendMovies(userId, userRatings, numRecs);
                    displayRecommendations(userId, recommendations, movies);
                }
                break;
            case 4:
            {
                string movieIdInput;
                int userId, movieId;
                double rating;
                cout << "Enter user ID: ";
                cin >> userId;
                if (cin.fail())
                {
                    throw runtime_error("Invalid user ID! Please enter a valid ID.");
                }
                if (userId < 1 || userId > userRatings.size())
                {
                    throw runtime_error("User ID is out of range! Please enter a valid user ID.");
                }
                cout << "Enter movie ID: ";
                getline(cin, movieIdInput);

                istringstream iss(movieIdInput);
                if (!(iss >> movieId))
                {
                    throw runtime_error("Invalid movie ID! Please enter a valid ID.");
                }

                // Kode lainnya untuk memeriksa rentang ID yang valid
                if (movieId < 1 || movieId > movies.size())
                {
                    throw runtime_error("Invalid movie ID! Please enter a valid ID.");
                }

                cout << "Enter rating (1-5): ";
                cin >> rating;
                if (cin.fail() || rating < 1 || rating > 5)
                {
                    throw runtime_error("Invalid rating! Please enter a valid rating between 1 and 5.");
                }

                addRating(ratings, userRatings, userId, movieId, rating);
            }
            break;
            case 5:
            {
                addmovie();
            }
            break;
            case 6:
                // Keluar dari program
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice! Please enter a valid choice." << endl;
                break;
            }
            cout << endl;
        }
        catch (const exception &e)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Error: " << e.what() << endl;
            cout << endl;
        }
    }

    return 0;
}