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

struct TreeNode
{
    Movie movie;
    vector<TreeNode *> children;
};

struct Graph
{
    unordered_map<int, unordered_map<int, double>> userRatings;
    unordered_map<int, unordered_map<int, double>> edges;

    void addVertex(int userId, unordered_map<int, double> ratings)
    {
        userRatings[userId] = ratings;
    }

    void addEdge(int userId1, int userId2, double similarity)
    {
        edges[userId1][userId2] = similarity;
    }

    unordered_map<int, double> getNeighbors(int userId)
    {
        return edges[userId];
    }
};

// Function to calculate similarity between users
double similarity(unordered_map<int, double> *user1, unordered_map<int, double> *user2)
{
    double dotProduct = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;

    for (auto &p1 : *user1)
    {
        int movieId = p1.first;
        double rating1 = p1.second;
        if (user2->count(movieId))
        {
            double rating2 = (*user2)[movieId];
            dotProduct += rating1 * rating2;
        }
        norm1 += rating1 * rating1;
    }

    for (auto &p2 : *user2)
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

// Function to recommend movies based on ratings from users with high similarity
vector<pair<int, double>> recommendMovies(int userId, Graph &userGraph, vector<Movie> &movies, int numRecs)
{
    unordered_map<int, double> *user1 = &(userGraph.userRatings[userId]);

    // Calculate similarity between users
    vector<pair<int, double>> similarities;
    for (auto &p2 : userGraph.userRatings)
    {
        int userId2 = p2.first;
        if (userId2 != userId)
        {
            double sim = similarity(user1, &(p2.second));
            similarities.push_back({userId2, sim});
        }
    }

    // Sort users based on similarity
    sort(similarities.begin(), similarities.end(), [](const pair<int, double> &p1, const pair<int, double> &p2)
         { return p1.second > p2.second; });

    // Recommend movies
    unordered_map<int, double> movieScores;
    for (auto &p : similarities)
    {
        int userId2 = p.first;
        double sim = p.second;
        unordered_map<int, double> *user2 = &(userGraph.userRatings[userId2]);
        for (auto &p2 : *user2)
        {
            int movieId = p2.first;
            double rating = p2.second;
            if (!user1->count(movieId))
            {
                if (!movieScores.count(movieId))
                {
                    movieScores[movieId] = 0.0;
                }
                movieScores[movieId] += rating * sim;
            }
        }
    }

    // Sort movies based on scores
    vector<pair<int, double>> recommendations;
    for (auto &p : movieScores)
    {
        int movieId = p.first;
        double score = p.second;
        recommendations.push_back({movieId, score});
    }
    sort(recommendations.begin(), recommendations.end(), [](const pair<int, double> &p1, const pair<int, double> &p2)
         { return p1.second > p2.second; });
    vector<pair<int, double>> topRecs(recommendations.begin(), recommendations.begin() + numRecs);

    return topRecs;
}

// Function to create a movie tree based on movie data
TreeNode *createMovieTree(vector<Movie> &movies)
{
    unordered_map<int, TreeNode *> movieMap;
    TreeNode *root = nullptr;

    for (auto &movie : movies)
    {
        TreeNode *node = new TreeNode;
        node->movie = movie;
        movieMap[movie.id] = node;

        if (movie.id == 1)
        {
            root = node;
        }
    }

    for (auto &movie : movies)
    {
        if (movie.id != 1)
        {
            TreeNode *parent = movieMap[movie.id / 10];
            parent->children.push_back(movieMap[movie.id]);
        }
    }

    return root;
}

// Function to print the movie tree
void printMovieTree(TreeNode *root, int level = 0)
{
    if (root == nullptr)
    {
        return;
    }

    for (int i = 0; i < level; ++i)
    {
        cout << "  ";
    }
    cout << "- " << root->movie.title << " (" << root->movie.genre << ")" << endl;

    for (auto &child : root->children)
    {
        printMovieTree(child, level + 1);
    }
}

int main()
{
    // Read movie file
    ifstream movieFile("movies.csv");
    vector<Movie> movies;
    string line;
    getline(movieFile, line); // skip header
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

    // Read rating file
    ifstream ratingFile("ratings.csv");
    vector<Rating> ratings;
    getline(ratingFile, line); // skip header
    while (getline(ratingFile, line))
    {
        stringstream ss(line);
        string userId, movieId, rating;
        getline(ss, userId, ',');
        getline(ss, movieId, ',');
        getline(ss, rating, ',');
        ratings.push_back({stoi(userId), stoi(movieId), stoi(rating)});
    }
    ratingFile.close();

    // Create mapping from user to rating
    Graph userGraph;
    for (auto &rating : ratings)
    {
        userGraph.userRatings[rating.userId][rating.movieId] = rating.rating;
    }

    // Create movie tree
    TreeNode *movieTree = createMovieTree(movies);

    // Print movie tree
    cout << "Movie Tree:" << endl;
    printMovieTree(movieTree);
    cout << endl;

    // Recommend movies for each user
    for (int userId = 1; userId <= 10; ++userId)
    {
        vector<pair<int, double>> recommendations = recommendMovies(userId, userGraph, movies, 5);
        cout << "Recommended movies for user " << userId << ":" << endl;
        for (auto &rec : recommendations)
        {
            int movieId = rec.first;
            double score = rec.second;
            cout << movies[movieId - 1].title << " (" << movies[movieId - 1].genre << "): " << score << endl;
        }
        cout << endl;
    }

    return 0;
}
