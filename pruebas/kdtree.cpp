#include <vector>
#include <utility>
#include <algorithm>

template<typename ElemType>
class KDTree {
private:
    struct Node {
        std::vector<double> point;
        ElemType value;
        Node* left;
        Node* right;

        Node(const std::vector<double>& pt, const ElemType& val)
            : point(pt), value(val), left(nullptr), right(nullptr) {}
    };

    Node* root;
    int dimensions;

public:
    KDTree(int dims) : root(nullptr), dimensions(dims) {}

    // Construye el árbol desde un vector de puntos y valores
    void build(const std::vector<std::pair<std::vector<double>, ElemType>>& data) {
        root = build_recursive(data, 0, data.size(), 0);
    }

    // Inserta un solo punto
    void insert(const std::vector<double>& point, const ElemType& value) {
        root = insert_recursive(root, point, value, 0);
    }

    // Búsqueda ortogonal en rango
    std::vector<ElemType> ortogonal_range_query(const std::vector<std::pair<double, double>>& ranges) const {
        std::vector<ElemType> result;
        ortogonal_range_query_recursive(root, 0, ranges, result);
        return result;
    }

private:
    // Inserción recursiva
    Node* insert_recursive(Node* node, const std::vector<double>& point, const ElemType& value, int depth) {
        if (!node) return new Node(point, value);

        int axis = depth % dimensions;
        if (point[axis] < node->point[axis])
            node->left = insert_recursive(node->left, point, value, depth + 1);
        else
            node->right = insert_recursive(node->right, point, value, depth + 1);

        return node;
    }

    // Construcción recursiva desde vector
    Node* build_recursive(std::vector<std::pair<std::vector<double>, ElemType>> data,
                          int start, int end, int depth) {
        if (start >= end) return nullptr;

        int axis = depth % dimensions;
        int mid = (start + end) / 2;

        std::nth_element(data.begin() + start, data.begin() + mid, data.begin() + end,
            [axis](const auto& a, const auto& b) {
                return a.first[axis] < b.first[axis];
            });

        Node* node = new Node(data[mid].first, data[mid].second);
        node->left = build_recursive(data, start, mid, depth + 1);
        node->right = build_recursive(data, mid + 1, end, depth + 1);

        return node;
    }

    // Consulta ortogonal en rango
    void ortogonal_range_query_recursive(Node* node, int depth,
                                         const std::vector<std::pair<double, double>>& ranges,
                                         std::vector<ElemType>& result) const {
        if (!node) return;

        bool inside = true;
        for (int i = 0; i < dimensions; ++i) {
            if (node->point[i] < ranges[i].first || node->point[i] > ranges[i].second) {
                inside = false;
                break;
            }
        }

        if (inside) {
            result.push_back(node->value);
        }

        int axis = depth % dimensions;

        if (node->point[axis] >= ranges[axis].first)
            ortogonal_range_query_recursive(node->left, depth + 1, ranges, result);

        if (node->point[axis] <= ranges[axis].second)
            ortogonal_range_query_recursive(node->right, depth + 1, ranges, result);
    }
};
#include <iostream>

int main() {
    KDTree<std::string> tree(2);

    std::vector<std::pair<std::vector<double>, std::string>> puntos = {
        {{3.0, 5.0}, "A"},
        {{7.0, 2.0}, "B"},
        {{5.0, 4.0}, "C"},
        {{9.0, 6.0}, "D"},
        {{4.0, 7.0}, "E"}
    };

    tree.build(puntos);

    std::vector<std::pair<double, double>> rango = {{2.0, 8.0}, {1.0, 6.0}};
    auto resultado = tree.ortogonal_range_query(rango);

    for (const auto& val : resultado) {
        std::cout << "Encontrado: " << val << "\n";
    }

    return 0;
}