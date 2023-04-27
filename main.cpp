#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <queue>
#include <random>
#include <ctime>

using namespace std;
using namespace std::chrono;

struct Movie {
    string title;
    string genres;
    int runtime;
    double popularity;
};

// Node structure
struct Node {
    Movie movie;
    shared_ptr<Node> left;
    shared_ptr<Node> right;

    Node(const Movie& m) : movie(m), left(nullptr), right(nullptr) {}
};

// BST class
class BST {
private:
    shared_ptr<Node> root;

    // Helper function for inserting a movie into the BST
    void insert(shared_ptr<Node>& node, const Movie& movie) {
        if (!node) {
            node = make_shared<Node>(movie);
        } else if (movie.runtime < node->movie.runtime) {
            insert(node->left, movie);
        } else {
            insert(node->right, movie);
        }
    }

    // Helper function for filtering movies by maximum runtime and minimum popularity
    void in_order_traversal(shared_ptr<Node> node, const string &chosen_genre, int max_runtime, BST &popularity_filtered_tree, double min_popularity) {
        if (!node) {
            return;
        }

        in_order_traversal(node->left, chosen_genre, max_runtime, popularity_filtered_tree, min_popularity);

        // Check if the movie belongs to the chosen genre
        if (node->movie.genres.find(chosen_genre) != string::npos) {
            if (node->movie.runtime <= max_runtime && node->movie.popularity >= min_popularity) {
                popularity_filtered_tree.insert(node->movie);
            } else {
                return;
            }
        }
        in_order_traversal(node->right, chosen_genre, max_runtime, popularity_filtered_tree, min_popularity);
    }

// Helper function for inserting movies into a vector using in-order traversal
    void in_order_traversal_to_vector(shared_ptr<Node> node, vector<Movie> &movies) {
        if (!node) {
            return;
        }

        in_order_traversal_to_vector(node->left, movies);
        movies.push_back(node->movie);
        in_order_traversal_to_vector(node->right, movies);
    }

    // Helper function for DFS traversal
    void dfs(shared_ptr<Node> node) {
        if (!node) {
            return;
        }

        dfs(node->left);
        dfs(node->right);
    }

    // Helper function for choosing a random movie using DFS traversal
    Movie choose_random_movie_dfs(shared_ptr<Node> node, int &count, int target_count) {
        if (!node) {
            return Movie{"", "", 0, 0.0};
        }

        Movie left_movie = choose_random_movie_dfs(node->left, count, target_count);
        if (!left_movie.title.empty()) {
            return left_movie;
        }

        if (count == target_count) {
            return node->movie;
        }
        count++;

        return choose_random_movie_dfs(node->right, count, target_count);
    }

    // Helper function for choosing a random movie using BFS traversal
    Movie choose_random_movie_bfs(int target_count) {
        if (!root) {
            return Movie{"", "", 0, 0.0};
        }

        int count = 0;
        queue<shared_ptr<Node>> q;
        q.push(root);

        while (!q.empty()) {
            shared_ptr<Node> current = q.front();
            q.pop();

            if (count == target_count) {
                return current->movie;
            }
            count++;

            if (current->left) {
                q.push(current->left);
            }
            if (current->right) {
                q.push(current->right);
            }
        }

        return Movie{"", "", 0, 0.0};
    }

public:
    BST() : root(nullptr) {}
    // Public function for inserting a movie
    void insert(const Movie& movie) {
        insert(root, movie);
    }
    // Public function for filtering movies by maximum runtime and minimum popularity
    void filter_movies_by_popularity(const string &chosen_genre, BST &popularity_filtered_tree, int max_runtime, double min_popularity) {
        in_order_traversal(root, chosen_genre, max_runtime, popularity_filtered_tree, min_popularity);
    }
    // Public function for inserting movies into a vector using in-order traversal
    void filtered_movies_to_vector(vector<Movie> &movies) {
        in_order_traversal_to_vector(root, movies);
    }

    // DFS traversal
    void dfs_traversal() {
        dfs(root);
    }

    // BFS traversal
    void bfs_traversal() {
        if (!root) {
            return;
        }

        queue<shared_ptr<Node>> q;
        q.push(root);

        while (!q.empty()) {
            shared_ptr<Node> current = q.front();
            q.pop();

            if (current->left) {
                q.push(current->left);
            }
            if (current->right) {
                q.push(current->right);
            }
        }
    }
};

void read_csv_file(const string &filename, unordered_map<string, vector<Movie>> &genre_to_movies) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        try {
            stringstream ss(line);
            string token;
            vector<string> row;

            while (getline(ss, token, ',')) {
                row.push_back(token);
            }

            string movie_title = row[0];      // original_title
            string genres = row[4];           // genres
            int runtime = stoi(row[2]);       // runtime
            double popularity = stod(row[3]); // popularity
            Movie movie{movie_title, genres, runtime, popularity};

            genre_to_movies[genres].push_back(movie);
        } catch (const exception &e) {
            // Ignore any lines that can't be processed
            continue;
        }
    }
}

// Function for reading movie data from a CSV file and storing it in a map
void read_csv_file_map(const string &filename, unordered_map<string, Movie> &movies) {
    ifstream file(filename);
    string line;
    // Skip the header line
    getline(file, line);

    while (getline(file, line)) {
        try {
            stringstream ss(line);
            string token;
            vector<string> row;
            // Split the CSV row into tokens
            while (getline(ss, token, ',')) {
                row.push_back(token);
            }
            // Extract movie information from the tokens
            string movie_title = row[0];      // original_title
            string genres = row[4];           // genres
            int runtime = stoi(row[2]);       // runtime
            double popularity = stod(row[3]); // popularity
            Movie movie{movie_title, genres, runtime, popularity};
            // Add the movie to the map using its title as the key
            movies[movie_title] = movie;
        } catch (const exception &e) {
            // Ignore any lines that can't be processed
            continue;
        }
    }
}

// Function for printing a list of genre options to the console
void printGenres() {
    cout << endl << "Enter 1 for Action" << endl;
    cout << "Enter 2 for Adventure" << endl;
    cout << "Enter 3 for Animation" << endl;
    cout << "Enter 4 for Comedy" << endl;
    cout << "Enter 5 for Crime" << endl;
    cout << "Enter 6 for Documentary" << endl;
    cout << "Enter 7 for Drama" << endl;
    cout << "Enter 8 for Family" << endl;
    cout << "Enter 9 for Fantasy" << endl;
    cout << "Enter 10 for Fiction" << endl;
    cout << "Enter 11 for Foreign" << endl;
    cout << "Enter 12 for History" << endl;
    cout << "Enter 13 for Horror" << endl;
    cout << "Enter 14 for Music" << endl;
    cout << "Enter 15 for Mystery" << endl;
    cout << "Enter 16 for Romance" << endl;
    cout << "Enter 17 for Science" << endl;
    cout << "Enter 18 for Thriller" << endl;
    cout << "Enter 19 for TV Movie" << endl;
    cout << "Enter 20 for War" << endl;
    cout << "Enter 21 for Western" << endl;
}

// Function for converting a genre number to its corresponding string representation
string numToGenre(int genreNum) {
    if (genreNum == 1) {
        return "Action";
    }
    else if (genreNum == 2) {
        return "Adventure";
    }
    else if (genreNum == 3) {
        return "Animation";
    }
    else if (genreNum == 4) {
        return "Comedy";
    }
    else if (genreNum == 5) {
        return "Crime";
    }
    else if (genreNum == 6) {
        return "Documentary";
    }
    else if (genreNum == 7) {
        return "Drama";
    }
    else if (genreNum == 8) {
        return "Family";
    }
    else if (genreNum == 9) {
        return "Fantasy";
    }
    else if (genreNum == 10) {
        return "Fiction";
    }
    else if (genreNum == 11) {
        return "Foreign";
    }
    else if (genreNum == 12) {
        return "History";
    }
    else if (genreNum == 13) {
        return "Horror";
    }
    else if (genreNum == 14) {
        return "Music";
    }
    else if (genreNum == 15) {
        return "Mystery";
    }
    else if (genreNum == 16) {
        return "Romance";
    }
    else if (genreNum == 17) {
        return "Science";
    }
    else if (genreNum == 18) {
        return "Thriller";
    }
    else if (genreNum == 19) {
        return "TV Movie";
    }
    else if (genreNum == 20) {
        return "War";
    }
    else if (genreNum == 21) {
        return "Western";
    }
}

// Function for checking if a genre number is within the valid range of 1-21
bool checkGenre(int genreNum) {
    if (genreNum <= 21 && genreNum > 0) {
        return true;
    }
    else {
        return false;
    }
}

// Function for printing all movies in a map to the console
void printAllMovies(unordered_map<string, Movie> movies){
    for (const auto &movie : movies) {
        cout << "Title: " << movie.second.title << endl;
        cout << "Genre: " << movie.second.genres << endl;
        cout << "Runtime: " << movie.second.runtime << endl;
        cout << "Popularity: " << movie.second.popularity << endl;
        cout << endl;
    }
}

// Function for selecting a random movie from a map
Movie select_random_movie_from_map(const unordered_map<string, Movie> &movies) {
    if (movies.empty()) {
        return Movie{"", "", 0, 0.0};
    }

    // Seed random number generator
    srand(time(0));
    int random_index = rand() % movies.size();

    int current_index = 0;
    for (const auto &movie_pair : movies) {
        if (current_index == random_index) {
            return movie_pair.second;
        }
        current_index++;
    }

    return Movie{"", "", 0, 0.0};
}

int main() {
    unordered_map<string, vector<Movie>> genre_to_movies;
    read_csv_file("movies_metadata.csv", genre_to_movies);
    unordered_map<string, Movie> movies;
    read_csv_file_map("movies_metadata.csv", movies);
    cout << "Welcome to Film Finder!" << endl;
    cout << "More time watching, and less time searching, what is your name?" << endl;
    string name = "";
    cin >> name;
    int choice;
    bool runAgain = true;
    while (runAgain) {
        cout << "Hi " << name << ", choose an option:" << endl;
        cout << "1. Find movie using filters via DFS and BFS" << endl;
        cout << "2. Surprise me!" << endl;
        cout << "3. Print all movies utilizing unordered map" << endl;
        cin >> choice;
        if (choice == 1) {
            int genreNum;
            string chosen_genre;
            int max_runtime;
            double min_popularity;

            // User input
            cout << "Which genre movie from the menu below would you like to select?" << endl;
            printGenres();
            cin >> genreNum;
            while (!checkGenre(genreNum)) {
                cout << "Invalid genre type";
                cout << "Try again!" << endl;
                genreNum = 0;
                cin >> genreNum;
            }
            chosen_genre = numToGenre(genreNum);
            chosen_genre += "\r";
            cout << "What is the longest length you want the movie (in minutes)?: " << endl;
            cin >> max_runtime;
            cout << "What is the minimum IMBD popularity rating you want this movie to have (5 - 20+): " << endl;
            cout << "(Think very popular movies to be closer to 20 or above i.e. Star Wars, Transformers, Jurassic World etc.)" << endl;
            cin >> min_popularity;
            // Create BST and insert movies of chosen genre
            BST movie_bst;
            for (const auto &movie: genre_to_movies[chosen_genre]) {
                movie_bst.insert(movie);
            }

            // Create BST for popularity filtered movies
            BST popularity_filtered_tree;

            // Filter movies by max runtime and min popularity
            movie_bst.filter_movies_by_popularity(chosen_genre, popularity_filtered_tree, max_runtime, min_popularity);

            // Store the filtered movies in a vector
            vector<Movie> final_movies;
            popularity_filtered_tree.filtered_movies_to_vector(final_movies);

            // Measure time for DFS and BFS traversals on movie_bst
            auto start_dfs_1 = high_resolution_clock::now();
            movie_bst.dfs_traversal();
            // auto stop_d;
            auto stop_dfs_1 = high_resolution_clock::now();
            auto duration_dfs_1 = duration_cast<microseconds>(stop_dfs_1 - start_dfs_1);

            auto start_bfs_1 = high_resolution_clock::now();
            movie_bst.bfs_traversal();
            auto stop_bfs_1 = high_resolution_clock::now();
            auto duration_bfs_1 = duration_cast<microseconds>(stop_bfs_1 - start_bfs_1);

            // Measure time for DFS and BFS traversals on popularity_filtered_tree
            auto start_dfs_2 = high_resolution_clock::now();
            popularity_filtered_tree.dfs_traversal();
            auto stop_dfs_2 = high_resolution_clock::now();
            auto duration_dfs_2 = duration_cast<microseconds>(stop_dfs_2 - start_dfs_2);

            auto start_bfs_2 = high_resolution_clock::now();
            popularity_filtered_tree.bfs_traversal();
            auto stop_bfs_2 = high_resolution_clock::now();
            auto duration_bfs_2 = duration_cast<microseconds>(stop_bfs_2 - start_bfs_2);

            // Calculate total time for DFS and BFS traversals
            auto total_dfs_time = duration_dfs_1 + duration_dfs_2;
            auto total_bfs_time = duration_bfs_1 + duration_bfs_2;
            // Print the final_movies vector
            if (final_movies.empty()) {
                cout << "No movies found with the specified filters." << endl;
                cout << "Suggesting an alternative movie by relaxing the filters..." << endl;

                // Relax the filters
                int relaxed_max_runtime = max_runtime + 30;
                double relaxed_min_popularity = min_popularity - 5.0;
                if (relaxed_min_popularity < 0.0) {
                    relaxed_min_popularity = 0.0;
                }

                // Filter movies with relaxed filters
                BST relaxed_popularity_filtered_tree;
                movie_bst.filter_movies_by_popularity(chosen_genre, relaxed_popularity_filtered_tree, relaxed_max_runtime,
                                                      relaxed_min_popularity);

                // Store the filtered movies in a vector
                vector<Movie> relaxed_final_movies;
                relaxed_popularity_filtered_tree.filtered_movies_to_vector(relaxed_final_movies);

                // Print the relaxed_final_movies vector
                for (const Movie &movie: relaxed_final_movies) {
                    cout << movie.title << " (" << movie.runtime << " min, " << movie.popularity << " popularity)"
                         << endl;
                }
            } else {
                for (const Movie &movie: final_movies) {
                    cout << movie.title << " (" << movie.runtime << " min, " << movie.popularity << " popularity)"
                         << endl;
                }
            }
           /* // Print the final_movies vector
            for (const Movie &movie: final_movies) {
                cout << movie.title << " (" << movie.runtime << " min, " << movie.popularity << " popularity)" << endl;
            }*/

            // Print time measurements
            cout << endl << "Total DFS traversal time: " << total_dfs_time.count() << " microseconds" << endl;
            cout << "Total BFS traversal time: " << total_bfs_time.count() << " microseconds" << endl;
        } else if (choice == 2) {
            Movie random_movie = select_random_movie_from_map(movies);
            cout << "Title: " << random_movie.title << endl;
            cout << "Genre: " << random_movie.genres << endl;
            cout << "Runtime: " << random_movie.runtime << endl;
            cout << "Popularity: " << random_movie.popularity << endl;
            cout << endl;
        } else if (choice == 3) {
            printAllMovies(movies);
        }
        int continueChoice;
        cout << endl <<"Would you like to start over or exit?" << endl;
        cout << "1. Start over" << endl;
        cout << "2. Exit" << endl;
        cin >> continueChoice;

        if (continueChoice == 2) {
            runAgain = false;
        }
    }
    return 0;
}
