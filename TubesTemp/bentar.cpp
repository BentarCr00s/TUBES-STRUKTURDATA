#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <cmath>

using namespace std;

struct Movie
{
    int movieId;
    string movieName;
    string genre;
};

struct Rating
{
    int userId;
    int movieId;
    double rating;
};

vector<Movie> loadMovies(const string &filename)
{
    vector<Movie> movies;
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header line

    while (getline(file, line))
    {
        stringstream ss(line);
        string token;
        Movie movie;

        getline(ss, token, ',');
        movie.movieId = stoi(token);

        getline(ss, movie.movieName, ',');

        getline(ss, movie.genre, ',');

        movies.push_back(movie);
    }

    file.close();
    return movies;
}

vector<Rating> loadRatings(const string &filename)
{
    vector<Rating> ratings;
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header line

    while (getline(file, line))
    {
        stringstream ss(line);
        string token;
        Rating rating;

        getline(ss, token, ',');
        rating.userId = stoi(token);

        getline(ss, token, ',');
        rating.movieId = stoi(token);

        getline(ss, token, ',');
        rating.rating = stod(token);

        ratings.push_back(rating);
    }

    file.close();
    return ratings;
}

map<int, vector<Rating>> buildUserRatings(const vector<Rating> &ratings)
{
    map<int, vector<Rating>> userRatings;

    for (const auto &rating : ratings)
    {
        userRatings[rating.userId].push_back(rating);
    }

    return userRatings;
}

vector<Movie> getTopRatedMovies(const vector<Rating> &ratings, const vector<Movie> &movies, int n)
{
    map<int, double> movieRatings;

    for (const auto &rating : ratings)
    {
        movieRatings[rating.movieId] += rating.rating;
    }

    multimap<double, int, greater<double>> sortedRatings;

    for (const auto &pair : movieRatings)
    {
        sortedRatings.insert(make_pair(pair.second, pair.first));
    }

    vector<Movie> topRatedMovies;

    for (const auto &pair : sortedRatings)
    {
        if (n <= 0)
            break;

        int movieId = pair.second;
        auto it = find_if(movies.begin(), movies.end(), [movieId](const Movie &movie)
                          { return movie.movieId == movieId; });

        if (it != movies.end())
        {
            topRatedMovies.push_back(*it);
            n--;
        }
    }

    return topRatedMovies;
}

vector<Movie> getRecommendedMovies(int userId, const map<int, vector<Rating>> &userRatings, const vector<Movie> &movies, int n)
{
    map<int, double> movieSimilarities;
    map<int, double> movieScores;
    map<int, int> movieRatingsCount;

    // Calculate movie similarities and ratings count
    for (const auto &pair : userRatings)
    {
        int otherUserId = pair.first;
        if (otherUserId == userId)
            continue;

        for (const auto &rating : pair.second)
        {
            for (const auto &userRating : userRatings.at(userId))
            {
                if (userRating.movieId == rating.movieId)
                {
                    movieSimilarities[rating.movieId] += userRating.rating * rating.rating;
                }
            }
            movieRatingsCount[rating.movieId]++;
        }
    }

    // Calculate movie scores
    for (const auto &pair : movieSimilarities)
    {
        int movieId = pair.first;
        double similarity = pair.second;

        for (const auto &rating : userRatings.at(userId))
        {
            if (rating.movieId == movieId)
                continue;

            movieScores[movieId] += rating.rating * similarity;
        }
    }

    // Find top recommended movies
    multimap<double, int, greater<double>> sortedScores;

    for (const auto &pair : movieScores)
    {
        int movieId = pair.first;
        double score = pair.second;
        int ratingsCount = movieRatingsCount[movieId];

        // Calculate average score considering ratings count
        double averageScore = (ratingsCount > 0) ? score / ratingsCount : 0.0;
        sortedScores.insert(make_pair(averageScore, movieId));
    }

    vector<Movie> recommendedMovies;

    for (const auto &pair : sortedScores)
    {
        if (n <= 0)
            break;

        int movieId = pair.second;
        auto it = find_if(movies.begin(), movies.end(), [movieId](const Movie &movie)
                          { return movie.movieId == movieId; });

        if (it != movies.end())
        {
            recommendedMovies.push_back(*it);
            n--;
        }
    }

    return recommendedMovies;
}

int main()
{
    // Load movies and ratings from CSV files
    vector<Movie> movies = loadMovies("movies.csv");
    vector<Rating> ratings = loadRatings("ratings.csv");

    // Build user-ratings map
    map<int, vector<Rating>> userRatings = buildUserRatings(ratings);

    // Get top rated movies
    cout << "Top rated movies:" << endl;
    vector<Movie> topRatedMovies = getTopRatedMovies(ratings, movies, 10);
    for (const auto &movie : topRatedMovies)
    {
        cout << movie.movieName << " (" << movie.genre << ")" << endl;
    }
    cout << endl;

    // Get recommended movies for a user
    int userId = 1; // Change to desired user ID
    cout << "Recommended movies for user " << userId << ":" << endl;
    vector<Movie> recommendedMovies = getRecommendedMovies(userId, userRatings, movies, 5);
    for (const auto &movie : recommendedMovies)
    {
        cout << movie.movieName << " (" << movie.genre << ")" << endl;
    }

    return 0;
}
