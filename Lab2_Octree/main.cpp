#include "Octree_alex.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

void leerCSV(const std::string &archivo, std::vector<int> &columna1,
             std::vector<int> &columna2, std::vector<int> &columna3) {

  std::ifstream file(archivo);

  if (!file.is_open()) {
    std::cerr << "Error to open file." << std::endl;
    return;
  }

  std::string linea;
  while (std::getline(file, linea)) {
    std::istringstream ss(linea);
    std::string valor;
    std::getline(ss, valor, ',');
    int valor1 = std::stoi(valor);
    columna1.push_back(valor1);

    std::getline(ss, valor, ',');
    int valor2 = std::stoi(valor);
    columna2.push_back(valor2);

    std::getline(ss, valor, ',');
    int valor3 = std::stoi(valor);
    columna3.push_back(valor3);
  }

  file.close();
}

int max_num(int a,int b,int c){
  return a>b ? (a>c? a:c) : (b>c? b:c);
}

int get_h(vector<int> X, vector<int> Y, vector<int> Z, Point &p_aux) {
  p_aux = Point(
    *std::min_element(X.begin(),X.end()),
    *std::min_element(Y.begin(),Y.end()),
    *std::min_element(Z.begin(),Z.end())
  );

  int max_x = *std::max_element(X.begin(),X.end()), 
  max_y = *std::max_element(Y.begin(),Y.end()), 
  max_z = *std::max_element(Z.begin(),Z.end());

//   cout<<max_x<<" "<<p_aux->x<<endl;
//   cout<<max_y<<" "<<p_aux->y<<endl;
//   cout<<max_z<<" "<<p_aux->z<<endl;
  
  return max_num(
    max_x >= 0 ? max_x - p_aux.x : max_x - p_aux.x, 
    max_y >= 0 ? max_y - p_aux.y : max_x - p_aux.y, 
    max_z >= 0 ? max_z - p_aux.z : max_x - p_aux.z
  );
}


int main() {
  // vector<int> X({9,12,3,-10,5,11,13,14,-14,-13,-11}),
  // Y({3,6,8,7,4,9,12,3,-9,11,6}), Z({10,7,5,1,9,3,9,15,-12,2,8});
  std::vector<int> X;
  std::vector<int> Y;
  std::vector<int> Z;

  leerCSV("points2.csv", X, Y, Z);
  // leerCSV("points2.csv", X, Y, Z);
  Point p_aux;
  int h = get_h(X, Y, Z, p_aux), N;
  cout<<"BottomLeft: "<<p_aux<<" h: "<<h<<endl;
  cout<<"Enter N points in the node: ";cin>>N;

  Octree octree(p_aux, h, N);

  for (int i = 0; i < X.size(); i++) {
    octree.insert(Point(X[i], Y[i], Z[i]));
  }

  std::cout << "Estructura del Octree:" << std::endl;
  octree.printTree();

  Point p_2find=Point(0,0,0);
  int radius=500;
  cout<<"Point exist in the tree? ";
  cout<<octree.exist(p_2find)<<endl;

  std::cout << "\nFind closest del Octree:" << std::endl;
  Point found = octree.find_closest(p_2find, radius);

  octree.get_h_bottom(found);

  while (1){
    int a,b,c, radius;
    cin>>a;
    cin>>b;
    cin>>c;
    Point p_find=Point(a,b,c);
    cin>>radius;
    cout<<"Point exist in the tree? ";
    cout<<octree.exist(p_find)<<endl;

    std::cout << "\nFind closest del Octree:" << std::endl;
    Point found2 = octree.find_closest(p_find, radius);
    octree.get_h_bottom(found2);
  }
  return 0;
};
