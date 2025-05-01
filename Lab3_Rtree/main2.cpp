#include <string>
#include <random>
#include <vector>
#include <iostream>
#include "RTree.h"

using namespace std;

void print(const int& command, vector<vector<vector<pair<int, int>>>>& objects_n, string& output) {
    output.resize(0);
    output = to_string(command);

    switch (command) {
    case 0: // OBJECTS
        output += "|" + to_string(objects_n[0].size());
        for (auto& polygon : objects_n[0]) {
            output += "|" + to_string(polygon.size());
            for (auto& point : polygon) {
                output += "|" + to_string(point.first) + "|" + to_string(point.second);
            }
        }
        break;

    case 1: // MBRS:
        cout << " number of (objects) :" << to_string(objects_n.size()) << endl;
        output += "|" + to_string(objects_n.size());
        for (auto& objects : objects_n) {
            output += "|" + to_string(objects.size());
            cout << " number of polygons :" << to_string(objects.size()) << endl;
            for (auto& polygon : objects) {
                output += "|" + to_string(polygon.size());
                cout << " number of points :" << to_string(polygon.size()) << endl;
                for (auto& point : polygon) {
                    output += "|" + to_string(point.first) + "|" + to_string(point.second);
                    cout << "   point:" << to_string(point.first) << " | " << to_string(point.second) << endl;
                }
            }
            cout << endl << "--------------------" << endl;
        }
        break;
    default:
        output += "|0";
        break;
    }

    output += "|END";
}

void print_pair(vector<pair<int, int>> output) {
    for (auto &x : output) {
        cout << " ( " << x.first << " , " << x.second << " ) ";
    }
}

// Generar polígonos aleatorios
vector<vector<pair<int, int>>> generateRandomPolygons(int numPolygons, int minCoord = 0, int maxCoord = 50) {
    vector<vector<pair<int, int>>> polygons;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> coordDist(minCoord, maxCoord);
    uniform_int_distribution<> sizeDist(2, 3);

    for (int i = 0; i < numPolygons; ++i) {
        int numPoints = sizeDist(gen);
        vector<pair<int, int>> polygon;
        for (int j = 0; j < numPoints; ++j) {
            polygon.push_back({coordDist(gen), coordDist(gen)});
        }
        polygons.push_back(polygon);
    }
    return polygons;
}

// Probar inserciones y Search
void testInsertions(int numPolygons, const string& testName) {
    cout << "\n=== " << testName << " (Poligonos: " << numPolygons << ", MAXNODES: " << MAXNODES << ") ===\n";
    
    vector<vector<pair<int, int>>> polygons = generateRandomPolygons(numPolygons);
    RTree rtree;
    
    cout << "\nInsertando poligonos:\n";
    for (const auto& poly : polygons) {
        cout << "Insertando poligono de " << poly.size() << " puntos: ";
        print_pair(poly);
        Rect rect = rtree.MBR(poly);
        vector<pair<int, int>> non_const_poly = poly; // Copia para evitar problemas de const
        rtree.Insert(rect.m_min, rect.m_max, non_const_poly);
        cout << endl;
    }
    
    // Obtener e imprimir MBRs
    cout << "Obteniendo MBRs...\n";
    vector<vector<vector<pair<int, int>>>> objects_n;
    string output;
    rtree.getMBRs(objects_n);
    cout << "\nEstructura del RTree:\n";
    print(1, objects_n, output);
    
    // Prueba de Search con rectángulo (0,0) a (50,50)
    cout << "\nProbando Search en (0,0) a (50,50):\n";
    vector<vector<pair<int, int>>> search_results;
    pair<int, int> search_min = {0, 0};
    pair<int, int> search_max = {50, 50};
    int num_results = rtree.Search(search_min, search_max, search_results);
    cout << "Se encontraron " << num_results << " poligonos (esperados: " << numPolygons << "):\n";
    for (const auto& obj : search_results) {
        cout << "Poligono: ";
        print_pair(obj);
        cout << endl;
    }
    
    // Exportar datos para graficar
    cout << "\nDatos para graficar:\n";
    cout << "Inserted polygons:\n";
    for (const auto& poly : polygons) {
        cout << "Polygon: ";
        for (const auto& point : poly) {
            cout << "(" << point.first << "," << point.second << ") ";
        }
        cout << endl;
    }
    cout << "Query rectangle: (" << search_min.first << "," << search_min.second << ") to (" << search_max.first << "," << search_max.second << ")\n";
    cout << "Search results:\n";
    for (const auto& poly : search_results) {
        cout << "Polygon: ";
        for (const auto& point : poly) {
            cout << "(" << point.first << "," << point.second << ") ";
        }
        cout << endl;
    }
    // Exportar MBRs
    cout << "MBRs:\n";
    for (const auto& objects : objects_n) {
        for (const auto& mbr : objects) {
            // Calcular el MBR de cada polígono en objects
            if (mbr.size() >= 2) { // Asegurarse de que hay al menos 2 puntos
                int min_x = mbr[0].first, max_x = mbr[0].first;
                int min_y = mbr[0].second, max_y = mbr[0].second;
                for (const auto& point : mbr) {
                    min_x = min(min_x, point.first);
                    max_x = max(max_x, point.first);
                    min_y = min(min_y, point.second);
                    max_y = max(max_y, point.second);
                }
                cout << "MBR: (" << min_x << "," << min_y << ") to (" << max_x << "," << max_y << ")\n";
            }
        }
    }
    cout << "Fin de datos para graficar\n";
}

int main(int argc, char *argv[]) {
    cout << "Ejecutando pruebas con MAXNODES = " << MAXNODES << "\n";
    cout << "Para probar otro valor de MAXNODES, edita RTree.h, cambia #define MAXNODES, y recompila.\n";
    
    testInsertions(5, "Prueba con 5 poligonos");
    testInsertions(12, "Prueba con 12 poligonos");
    
    cout << "\nPruebas completadas. Cambia MAXNODES en RTree.h y ejecuta de nuevo para comparar.\n";
    vector<vector<pair<int, int>>> vpoints;

  //   First Test
  float coord[16] = {20, 59, 20, 43, 50, 58 , 48,67, 105, 68, 74, 64, 83, 40, 104, 54};
  vector<pair<int, int>> points;
  int a1=2;
  for(int i =0;i<16;i+=2){
    pair<int,int> A;
    A.first = coord[i];
    A.second = coord[i+1];
    points.push_back(A);          
  }
  for (unsigned int i =0;i<8;i+=a1){       
    vector<pair<int, int>>  sub1(&points[i],&points[i+a1]);
    vpoints.push_back(sub1);
  }
  RTree rtree;

	string output;
	vector<vector<pair<int, int>>> objects;

  vector<vector<vector<pair<int, int>>>> objects_n;

  for(auto &x:vpoints)
    {
      cout<<"inserting "<< x.size()<< ": ";
      print_pair(x);
      Rect rect = rtree.MBR(x);
      rtree.Insert(rect.m_min, rect.m_max, x);
      cout<< endl;
    }  
  //Rect rect = rtree.MBR(vpoints[0]);
  //rtree.Insert(rect.m_min, rect.m_max, vpoints[0]);
  rtree.getMBRs(objects_n);
  print(1, objects_n, output);
  cout << "\n=== Probando Search ===\n";

  // Caso 1: Superposición Parcial (0,0) a (50,50)
  cout << "Caso 1: Buscando en rectángulo (0,0) a (50,50)\n";
  vector<vector<pair<int, int>>> search_results;
  pair<int, int> search_min = {0, 0};
  pair<int, int> search_max = {25, 25};
  int num_results = rtree.Search(search_min, search_max, search_results);
  cout << "Se encontraron " << num_results << " objetos:\n";
  for (const auto& obj : search_results) {
      cout << "Poligono: ";
      print_pair(obj);
      cout << endl;
  }
  cout << "------------------------\n";

  // Caso 2: Sin Superposición (1000,1000) a (2000,2000)
  cout << "Caso 2: Buscando en rectángulo (1000,1000) a (2000,2000)\n";
  search_results.clear();
  search_min = {1000, 1000};
  search_max = {2000, 2000};
  num_results = rtree.Search(search_min, search_max, search_results);
  cout << "Se encontraron " << num_results << " objetos:\n";
  for (const auto& obj : search_results) {
      cout << "Poligono: ";
      print_pair(obj);
      cout << endl;
  }
  cout << "------------------------\n";

  // Caso 3: Superposición Total (0,0) a (200,200)
  cout << "Caso 3: Buscando en rectangulo (0,0) a (200,200)\n";
  search_results.clear();
  search_min = {0, 0};
  search_max = {200, 200};
  num_results = rtree.Search(search_min, search_max, search_results);
  cout << "Se encontraron " << num_results << " objetos:\n";
  for (const auto& obj : search_results) {
      cout << "Poligono: ";
      print_pair(obj);
      cout << endl;
  }
  cout << "------------------------\n";
	
    return 0;
}