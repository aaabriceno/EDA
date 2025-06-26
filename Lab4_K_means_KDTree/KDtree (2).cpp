#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
using namespace std;

struct Punto {
    float x, y;
    int indice;

    bool operator==(const Punto& otro) const {
        return x == otro.x && y == otro.y;
    }
};

float distancia(const Punto& a, const Punto& b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

struct NodoKD {
    Punto punto;
    unique_ptr<NodoKD> izquierda;
    unique_ptr<NodoKD> derecha;

    NodoKD(Punto p) : punto(p), izquierda(nullptr), derecha(nullptr) {}
};

class KDTree {
private:
    unique_ptr<NodoKD> raiz;

    void insertarRec(unique_ptr<NodoKD>& nodo, const Punto& punto, int profundidad) {
        if (!nodo) {
            nodo = make_unique<NodoKD>(punto);
            return;
        }

        int eje = profundidad % 2;
        if ((eje == 0 && punto.x < nodo->punto.x) || (eje == 1 && punto.y < nodo->punto.y))
            insertarRec(nodo->izquierda, punto, profundidad + 1);
        else
            insertarRec(nodo->derecha, punto, profundidad + 1);
    }

    void nearestNeighborRec(NodoKD* nodo, const Punto& objetivo, int profundidad,
                            Punto& mejor, double& mejorDistancia) const {
        if (!nodo) return;

        double d = distancia(objetivo, nodo->punto);
        if (d < mejorDistancia) {
            mejorDistancia = d;
            mejor = nodo->punto;
        }

        int eje = profundidad % 2;
        NodoKD* ladoPrimario = nullptr;
        NodoKD* ladoSecundario = nullptr;

        if ((eje == 0 && objetivo.x < nodo->punto.x) || (eje == 1 && objetivo.y < nodo->punto.y)) {
            ladoPrimario = nodo->izquierda.get();
            ladoSecundario = nodo->derecha.get();
        } else {
            ladoPrimario = nodo->derecha.get();
            ladoSecundario = nodo->izquierda.get();
        }

        nearestNeighborRec(ladoPrimario, objetivo, profundidad + 1, mejor, mejorDistancia);

        // Verifica si vale la pena buscar en el otro lado
        double distanciaEje = (eje == 0) ? abs(objetivo.x - nodo->punto.x) : abs(objetivo.y - nodo->punto.y);
        if (distanciaEje < mejorDistancia) {
            nearestNeighborRec(ladoSecundario, objetivo, profundidad + 1, mejor, mejorDistancia);
        }
    }

public:
    void insertar(const Punto& punto) {
        insertarRec(raiz, punto, 0);
    }

    int nearestNeighborIndex(const Punto& objetivo) const {
        if (!raiz) return -1;

        Punto mejor = raiz->punto;
        double mejorDistancia = distancia(objetivo, mejor);

        nearestNeighborRec(raiz.get(), objetivo, 0, mejor, mejorDistancia);

        return mejor.indice;
    }
};

pair<vector<Punto>, vector<int>> kmeans(vector<Punto>& puntos, int k, int max_iter = 100) {
    int n = puntos.size();
    if (k > n) throw invalid_argument("k no puede ser mayor a la cantidad de puntos");

    random_device rd;
    mt19937 gen(rd());
    shuffle(puntos.begin(), puntos.end(), gen);

    vector<Punto> centroides;
    for (int i = 0; i < k; i++) {
        Punto c = puntos[i];
        c.indice = i;
        centroides.push_back(c);
    }

    vector<int> asignaciones(n, -1);

    for (int iter = 0; iter < max_iter; iter++) {
        KDTree kd;
        for (const auto& c : centroides) {
            kd.insertar(c);
        }
        bool hubo_cambio = false;

        for (int i = 0; i < n; i++) {
            int centroide_mas_cercano = kd.nearestNeighborIndex(puntos[i]);
            if (asignaciones[i] != centroide_mas_cercano) {
                asignaciones[i] = centroide_mas_cercano;
                hubo_cambio = true;
            }
        }

        if (!hubo_cambio) break;

        vector<Punto> nuevos_centroides(k, {0, 0, 0});
        vector<int> conteo(k, 0);

        for (int i = 0; i < n; i++) {
            int c = asignaciones[i];
            nuevos_centroides[c].x += puntos[i].x;
            nuevos_centroides[c].y += puntos[i].y;
            conteo[c]++;
        }

        for (int i = 0; i < k; i++) {
            if (conteo[i] > 0) {
                nuevos_centroides[i].x /= conteo[i];
                nuevos_centroides[i].y /= conteo[i];
            } else {
                // Si un centroide no tiene puntos asignados, mantener el anterior
                nuevos_centroides[i] = centroides[i];  // <--- Cambio para evitar división por cero
            }
            nuevos_centroides[i].indice = i;
        }

        centroides = nuevos_centroides;
    }

    return {centroides, asignaciones};
}

// Función para leer puntos desde un archivo CSV
vector<Punto> leerCSV(const string& nombreArchivo) {
    vector<Punto> puntos;
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << nombreArchivo << "\n";  // <--- Verifica apertura
        return puntos;
    }
    string linea;
    int indice = 0;

    getline(archivo, linea); // Salta encabezado

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string xStr, yStr;

        if (getline(ss, xStr, ',') && getline(ss, yStr, ',')) {
            Punto p;
            p.x = stof(xStr);  // Cambié a stof por ser float
            p.y = stof(yStr);
            p.indice = indice++;
            puntos.push_back(p);
        }
    }

    return puntos;
}

int main() {
    vector<Punto> puntos = leerCSV("points2d.csv");
    cout << "Total puntos: " << puntos.size() << "\n";
    for (int i = 0; i < puntos.size(); i++) {
        cout << "Punto " << i << ": (" << puntos[i].x << ", " << puntos[i].y << ") indice: " << puntos[i].indice << "\n";
    }
    auto resultado = kmeans(puntos, 95);

    vector<Punto> centroides_finales = resultado.first;
    vector<int> asignaciones = resultado.second;

    cout << "Centroides finales:\n";
    for (const auto& c : centroides_finales) {
        cout << "Indice: " << c.indice << " Posicion: (" << c.x << ", " << c.y << ")\n";
    }

    cout << "Asignaciones:\n";
    for (int i = 0; i < (int)asignaciones.size(); i++) {
        cout << "Punto " << i << " -> Centroide " << asignaciones[i] << "\n";
    }

    return 0;
}
