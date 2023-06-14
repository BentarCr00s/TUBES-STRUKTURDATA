#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>

using namespace std;

// Struktur data untuk node film
struct FilmNode
{
    int id;
    string title;
    vector<string> genres;
};

// Struktur data untuk node pengguna
struct UserNode
{
    int id;
    vector<pair<int, double>> ratedFilms;
};

// Struktur data untuk Graph film
class FilmGraph
{
private:
    vector<vector<int>> adjacencyList;

public:
    vector<UserNode> users;
    vector<FilmNode> films;
    void addFilm(int id, string title, const vector<string> &genres)
    {
        FilmNode film;
        film.id = id;
        film.title = title;
        film.genres = genres;
        films.push_back(film);
        adjacencyList.resize(films.size());
    }

    void addUser(int id)
    {
        UserNode user;
        user.id = id;
        users.push_back(user);
    }

    void addRating(int userId, int filmId, double rating)
    {
        users[userId].ratedFilms.push_back({filmId, rating});
        adjacencyList[filmId].push_back(userId);
    }

    void displayAllFilms()
    {
        for (const auto &film : films)
        {
            cout << "Film ID: " << film.id << endl;
            cout << "Title: " << film.title << endl;
            cout << "Genres: ";
            for (const auto &genre : film.genres)
            {
                cout << genre << ", ";
            }
            cout << endl
                 << endl;
        }
    }

    void displayAllRatings()
    {
        for (const auto &user : users)
        {
            cout << "User ID: " << user.id << endl;
            cout << "Rated Films: " << endl;
            for (const auto &ratedFilm : user.ratedFilms)
            {
                cout << "Film ID: " << ratedFilm.first << endl;
                cout << "Rating: " << ratedFilm.second << endl;
                cout << endl;
            }
            cout << endl;
        }
    }

    vector<int> getRecommendedFilms(int userId)
    {
        vector<int> recommendedFilms;
        vector<bool> visited(films.size(), false);
        queue<int> filmQueue;

        for (const auto &ratedFilm : users[userId].ratedFilms)
        {
            visited[ratedFilm.first] = true;
            filmQueue.push(ratedFilm.first);
        }

        while (!filmQueue.empty())
        {
            int currentFilm = filmQueue.front();
            filmQueue.pop();

            for (const auto &user : adjacencyList[currentFilm])
            {
                for (const auto &ratedFilm : users[user].ratedFilms)
                {
                    if (!visited[ratedFilm.first])
                    {
                        visited[ratedFilm.first] = true;
                        filmQueue.push(ratedFilm.first);
                        recommendedFilms.push_back(ratedFilm.first);
                    }
                }
            }
        }

        return recommendedFilms;
    }

    void displayMostPopularFilms()
    {
        unordered_map<int, int> filmRatings;

        for (const auto &user : users)
        {
            for (const auto &ratedFilm : user.ratedFilms)
            {
                int filmId = ratedFilm.first;
                if (filmRatings.find(filmId) == filmRatings.end())
                {
                    filmRatings[filmId] = 0;
                }
                filmRatings[filmId]++;
            }
        }

        vector<pair<int, int>> popularFilms(filmRatings.begin(), filmRatings.end());
        sort(popularFilms.begin(), popularFilms.end(), [](const pair<int, int> &a, const pair<int, int> &b)
             { return a.second > b.second; });

        cout << "Most Popular Films:" << endl;
        for (const auto &popularFilm : popularFilms)
        {
            int filmId = popularFilm.first;
            int ratingCount = popularFilm.second;

            cout << "Film ID: " << filmId << endl;
            cout << "Title: " << films[filmId].title << endl;
            cout << "Rating Count: " << ratingCount << endl;
            cout << endl;
        }
    }
};

int main()
{
    // Membaca data dari file movies.csv
    ifstream moviesFile("movies.csv");
    string line;
    FilmGraph filmGraph;

    if (moviesFile.is_open())
    {
        while (getline(moviesFile, line))
        {
            stringstream ss(line);
            string field;
            vector<string> fields;

            while (getline(ss, field, ','))
            {
                fields.push_back(field);
            }

            int filmId = stoi(fields[0]);
            string filmTitle = fields[1];

            stringstream genresStream(fields[2]);
            string genre;
            vector<string> filmGenres;

            while (getline(genresStream, genre, ','))
            {
                filmGenres.push_back(genre);
            }

            filmGraph.addFilm(filmId, filmTitle, filmGenres);
        }

        moviesFile.close();
    }
    else
    {
        cout << "Failed to open movies.csv" << endl;
        return 1;
    }

    // Membaca data dari file ratings.csv
    ifstream ratingsFile("ratings.csv");

    if (ratingsFile.is_open())
    {
        while (getline(ratingsFile, line))
        {
            stringstream ss(line);
            string field;
            vector<string> fields;

            while (getline(ss, field, ','))
            {
                fields.push_back(field);
            }

            int userId = stoi(fields[0]);
            int filmId = stoi(fields[1]);
            double rating = stod(fields[2]);

            filmGraph.addUser(userId);
            filmGraph.addRating(userId, filmId, rating);
        }

        ratingsFile.close();
    }
    else
    {
        cout << "Failed to open ratings.csv" << endl;
        return 1;
    }

    int choice;

    do
    {
        cout << "Menu:" << endl;
        cout << "1. Tampilkan semua film" << endl;
        cout << "2. Tampilkan semua rating pengguna" << endl;
        cout << "3. Rekomendasi film berdasarkan ID Users" << endl;
        cout << "4. Film Terpopuler" << endl;
        cout << "0. Keluar" << endl;
        cout << "Pilih menu: ";
        cin >> choice;
        cout << endl;

        switch (choice)
        {
        case 1:
            filmGraph.displayAllFilms();
            break;
        case 2:
            filmGraph.displayAllRatings();
            break;
        case 3:
            int userId;
            cout << "Masukkan ID Users: ";
            cin >> userId;
            cout << endl;

            if (userId >= 0 && userId < filmGraph.users.size())
            {
                vector<int> recommendedFilms = filmGraph.getRecommendedFilms(userId);

                cout << "Rekomendasi film untuk User ID " << userId << ":" << endl;
                for (const auto &filmId : recommendedFilms)
                {
                    cout << "Film ID: " << filmId << endl;
                    cout << "Title: " << filmGraph.films[filmId].title << endl;
                    cout << endl;
                }
            }
            else
            {
                cout << "ID Users tidak valid!" << endl;
            }
            break;
        case 4:
            filmGraph.displayMostPopularFilms();
            break;
        case 0:
            cout << "Keluar dari program." << endl;
            break;
        default:
            cout << "Pilihan tidak valid!" << endl;
            break;
        }

        cout << endl;
    } while (choice != 0);
    system("pause");
    return 0;
}
