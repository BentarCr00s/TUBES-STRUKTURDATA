#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

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
    int rating;
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
    cout << "Recommended movies for user " << userId << ":" << endl;
    for (const auto &rec : recommendations)
    {
        int movieId = rec.first;
        double score = rec.second;
        cout << movies[movieId - 1].title << " (" << movies[movieId - 1].genre << "): " << score << endl;
    }
    cout << endl;
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

int main()
{
    vector<Movie> movies;
    vector<Rating> ratings;
    unordered_map<int, unordered_map<int, double>> userRatings;

    // Membaca data film dari file
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
        int rtng = stoi(rating);
        ratings.push_back({uid, mid, rtng});
        userRatings[uid][mid] = rtng;
    }
    ratingFile.close();

    // Menu interaktif
    int choice = 0;
    while (choice != 4)
    {
        cout << "Menu:" << endl;
        cout << "1. Display all movies" << endl;
        cout << "2. Display top movies" << endl;
        cout << "3. Recommend movies" << endl;
        cout << "4. Exit" << endl;
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
            cin.clear();                                         // Menghapus kesalahan pada cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Membersihkan buffer masukan
            cerr << "Error: " << e.what() << endl;
            cout << endl;
        }
    }

    return 0;
}