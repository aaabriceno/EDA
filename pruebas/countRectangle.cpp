#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Complejidad: O(r log r + p log r)
vector<int> countRectangles(vector<vector<int>>& rectangles, vector<vector<int>>& points) {
    vector<vector<int>> buckets(101); // buckets[h] guarda longitudes li de rectángulos con altura h

    // Agrupar longitudes por altura
    for (const auto& rect : rectangles) {
        int li = rect[0], hi = rect[1];
        buckets[hi].push_back(li);
    }

    // Ordenar cada grupo para búsqueda binaria
    for (int h = 1; h <= 100; ++h) {
        sort(buckets[h].begin(), buckets[h].end());
    }

    vector<int> result;
    for (const auto& point : points) {
        int xj = point[0], yj = point[1];
        int count = 0;

        // Solo revisar alturas ≥ yj
        for (int h = yj; h <= 100; ++h) {
            const auto& bucket = buckets[h];
            count += bucket.end() - lower_bound(bucket.begin(), bucket.end(), xj);
        }

        result.push_back(count);
    }

    return result;
}

// Función para imprimir vector (no debe modificarse)
void printVector(const vector<int>& v) {
    cout << '[';
    for (int num : v) {
        cout << num << " ";
    }
    cout << ']' << endl;
}

int main() {
    // Ejemplo 1
    vector<vector<int>> rectangles1 = {{1, 2}, {2, 3}, {2, 5}};
    vector<vector<int>> points1 = {{2, 1}, {1, 4}};
    vector<int> result1 = countRectangles(rectangles1, points1);
    cout << "Output para el Ejemplo 1: ";
    printVector(result1); // Output esperado: [2, 1]

    // Ejemplo 2
    vector<vector<int>> rectangles2 = {{1, 1}, {2, 2}, {3, 3}};
    vector<vector<int>> points2 = {{1, 3}, {1, 1}};
    vector<int> result2 = countRectangles(rectangles2, points2);
    cout << "Output para el Ejemplo 2: ";
    printVector(result2); // Output esperado: [1, 3]

    return 0;
}
