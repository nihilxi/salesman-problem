#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <chrono>

const int INF = std::numeric_limits<int>::max();
const int MAX_CITIES = 25;

void generateMatrix(std::vector<std::vector<int>> &matrix, int numCities)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    for (int i = 0; i < numCities; ++i)
    {
        for (int j = 0; j < numCities; ++j)
        {
            matrix[i][j] = (i == j) ? 0 : dist(gen);
        }
    }
}

void saveMatrixToFile(const std::vector<std::vector<int>> &matrix, int numCities, const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << " for writing!" << std::endl;
        return;
    }

    file << numCities << std::endl;
    for (const auto &row : matrix)
    {
        for (const auto &val : row)
        {
            file << val << " ";
        }
        file << std::endl;
    }
    file.close();
    std::cout << "Matrix saved to file: " << filename << std::endl;
}

void displayMatrix(const std::vector<std::vector<int>> &matrix)
{
    for (const auto &row : matrix)
    {
        for (const auto &val : row)
        {
            std::cout << std::setw(4) << val << " ";
        }
        std::cout << std::endl;
    }
}

bool loadMatrixFromFile(const std::string &filename, std::vector<std::vector<int>> &matrix, int &numCities)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    file >> numCities;
    if (numCities < 2 || numCities > MAX_CITIES)
    {
        std::cerr << "Error: Invalid number of cities in file!" << std::endl;
        return false;
    }

    matrix.assign(numCities, std::vector<int>(numCities, 0));
    for (int i = 0; i < numCities; ++i)
    {
        for (int j = 0; j < numCities; ++j)
        {
            if (!(file >> matrix[i][j]))
            {
                std::cerr << "Error: Invalid data format in file!" << std::endl;
                return false;
            }
        }
    }
    return true;
}

int tsp(int mask, int pos, const std::vector<std::vector<int>> &dist, std::vector<std::vector<int>> &dp, int numCities)
{
    if (mask == (1 << numCities) - 1)
    {
        return dist[pos][0];
    }

    if (dp[mask][pos] != -1)
    {
        return dp[mask][pos];
    }

    int result = INF;
    for (int city = 0; city < numCities; ++city)
    {
        if (!(mask & (1 << city)))
        {
            int newCost = dist[pos][city] + tsp(mask | (1 << city), city, dist, dp, numCities);
            result = std::min(result, newCost);
        }
    }
    dp[mask][pos] = result;
    return result;
}

void massCalculate(int numFiles)
{
    for (int i = 1; i <= numFiles; ++i)
    {
        std::string filename = "matrix_" + std::to_string(i) + ".txt";
        std::vector<std::vector<int>> dist;
        int numCities;

        if (!loadMatrixFromFile(filename, dist, numCities))
        {
            std::cerr << "Skipping file: " << filename << " due to errors.\n";
            continue;
        }

        std::vector<std::vector<int>> dp(1 << numCities, std::vector<int>(numCities, -1));

        auto start = std::chrono::high_resolution_clock::now();
        int shortestPath = tsp(1, 0, dist, dp, numCities);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        std::cout << "File: " << filename << ", Shortest path cost: " << shortestPath
                  << ", Execution time: " << duration.count() << " ns\n";
    }
}

void menu()
{
    std::cout << "\n=== Traveling Salesman Problem (TSP) ===\n";
    std::cout << "1. Generate random distance matrix\n";
    std::cout << "2. Load distance matrix from file\n";
    std::cout << "3. Display distance matrix\n";
    std::cout << "4. Solve TSP\n";
    std::cout << "5. Solve TSP (Multiple files, only generated one)\n";
    std::cout << "6. Exit\n";
    std::cout << "Select an option: ";
}

int main()
{
    int choice;
    int numCities = 0;
    std::vector<std::vector<int>> dist;

    do
    {
        menu();
        std::cin >> choice;
        system("cls");
        switch (choice)
        {
        case 1:
        {
            std::cout << "Enter the number of cities (max " << MAX_CITIES << "): ";
            std::cin >> numCities;

            if (numCities > MAX_CITIES || numCities < 2)
            {
                std::cerr << "Invalid number of cities! Must be between 2 and " << MAX_CITIES << "." << std::endl;
            }
            else
            {
                int numMatrices;
                std::cout << "Enter the number of matrices to generate: ";
                std::cin >> numMatrices;

                for (int i = 0; i < numMatrices; ++i)
                {
                    dist.assign(numCities, std::vector<int>(numCities, 0));
                    generateMatrix(dist, numCities);

                    std::string filename = "matrix_" + std::to_string(i + 1) + ".txt";
                    saveMatrixToFile(dist, numCities, filename);
                }
                std::cout << numMatrices << " matrices generated and saved to files.\n";
            }
            break;
        }
        case 2:
        {
            std::string filename;
            std::cout << "Enter filename: ";
            std::cin >> filename;
            system("cls");
            if (loadMatrixFromFile(filename, dist, numCities))
            {
                std::cout << "Distance matrix loaded successfully.\n";
            }
            break;
        }
        case 3:
        {
            if (dist.empty())
            {
                std::cout << "No distance matrix available. Please generate or load one first.\n";
            }
            else
            {
                std::cout << "\nCurrent distance matrix:\n";
                displayMatrix(dist);
            }
            break;
        }
        case 4:
        {
            if (dist.empty())
            {
                std::cout << "No distance matrix available. Please generate or load one first.\n";
            }
            else
            {
                std::vector<std::vector<int>> dp(1 << numCities, std::vector<int>(numCities, -1));

                auto start = std::chrono::high_resolution_clock::now();
                int shortestPath = tsp(1, 0, dist, dp, numCities);
                auto end = std::chrono::high_resolution_clock::now();

                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                std::cout << "\nShortest path cost: " << shortestPath << std::endl;
                std::cout << "Execution time: " << duration.count() << " ns\n";
            }
            break;
        }
        case 5:
        {
            int numFiles;
            std::cout << "Enter the number of files to process: ";
            std::cin >> numFiles;
            massCalculate(numFiles);
            break;
        }
        case 6:
            std::cout << "Exiting program. Goodbye!\n";
            break;
        default:
            std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}
