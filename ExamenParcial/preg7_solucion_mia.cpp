#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <iomanip>
using namespace std;

// ----------- Definición de estructuras -----------

struct Rect {
    double minX, minY, maxX, maxY;
    Rect(double minX=0, double minY=0, double maxX=0, double maxY=0)
        : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}

    bool overlaps(const Rect& other) const {
        return !(minX > other.maxX || maxX < other.minX ||
                 minY > other.maxY || maxY < other.minY);
    }

    bool contains(const Rect& other) const {
        return (minX <= other.minX && maxX >= other.maxX &&
                minY <= other.minY && maxY >= other.maxY);
    }

    Rect combine(const Rect& other) const {
        return Rect(
            min(minX, other.minX),
            min(minY, other.minY),
            max(maxX, other.maxX),
            max(maxY, other.maxY)
        );
    }

    double area() const {
        return (maxX - minX) * (maxY - minY);
    }
};

const int MAX_ENTRIES = 4;
const int MIN_ENTRIES = 1;

struct Node;

struct Entry {
    Rect rect;
    Node* child;
    Entry(const Rect& r, Node* c = nullptr) : rect(r), child(c) {}
};

struct Node {
    bool isLeaf;
    vector<Entry> entries;
    Node(bool l = true) : isLeaf(l) {}
};

// ----------- Implementación del R-Tree -----------

class RTree {
    Node* root;

    int chooseSubtree(Node* node, const Rect& rect) {
        double minEnlargement = numeric_limits<double>::max();
        int bestIdx = 0;
        for (int i = 0; i < node->entries.size(); i++) {
            Rect combined = node->entries[i].rect.combine(rect);
            double enlargement = combined.area() - node->entries[i].rect.area();
            if (enlargement < minEnlargement) {
                minEnlargement = enlargement;
                bestIdx = i;
            }
        }
        return bestIdx;
    }

    void adjustTree(vector<Node*>& path, vector<int>& indices, Node* child1, Node* child2) {
        int i = path.size()-1;
        while (i >= 0) {
            Node* parent = path[i];
            int idx = indices[i];
            parent->entries[idx].rect = boundingRect(parent->entries[idx].child->entries);
            if (child2) {
                parent->entries.push_back(Entry(boundingRect(child2->entries), child2));
                if (parent->entries.size() > MAX_ENTRIES) {
                    Node* newSibling = splitNode(parent);
                    child2 = newSibling;
                } else {
                    child2 = nullptr;
                }
            }
            --i;
        }
        if (child2) {
            // Crear nueva raíz
            Node* oldRoot = root;
            root = new Node(false);
            root->entries.push_back(Entry(boundingRect(oldRoot->entries), oldRoot));
            root->entries.push_back(Entry(boundingRect(child2->entries), child2));
        }
    }

    Node* splitNode(Node* node) {
        vector<Entry>& es = node->entries;
        sort(es.begin(), es.end(), [](const Entry& a, const Entry& b) {
            return a.rect.minX < b.rect.minX;
        });
        Node* sibling = new Node(node->isLeaf);
        int split = es.size() / 2;
        sibling->entries.assign(es.begin() + split, es.end());
        es.erase(es.begin() + split, es.end());
        return sibling;
    }
    // Función para seleccionar las dos semillas (seeds) más separadas
void LPickSeeds(vector<Entry>& entries, int& seed1, int& seed2) {
    double maxDist = -1;
    for (int i = 0; i < entries.size(); ++i) {
        for (int j = i + 1; j < entries.size(); ++j) {
            double dist = sqrt(pow(entries[i].rect.minX - entries[j].rect.minX, 2) +
                               pow(entries[i].rect.minY - entries[j].rect.minY, 2));
            if (dist > maxDist) {
                maxDist = dist;
                seed1 = i;
                seed2 = j;
            }
        }
    }
}


    void LDistribute(vector<Entry>& entries, vector<Entry>& node1, vector<Entry>& node2) {
    // Asignar las semillas a los nodos
    node1.push_back(entries[seed1]);
    node2.push_back(entries[seed2]);
    entries.erase(entries.begin() + seed2);
    entries.erase(entries.begin() + seed1);

    // Asignar las entradas restantes
    while (!entries.empty()) {
        int next = -1;
        double minOverlap = numeric_limits<double>::max();
        for (int i = 0; i < entries.size(); ++i) {
            double overlap1 = node1.back().rect.combine(entries[i].rect).area() - node1.back().rect.area();
            double overlap2 = node2.back().rect.combine(entries[i].rect).area() - node2.back().rect.area();
            double overlap = min(overlap1, overlap2);
            if (overlap < minOverlap) {
                minOverlap = overlap;
                next = i;
            }
        }
        if (node1.size() < node2.size()) {
            node1.push_back(entries[next]);
        } else {
            node2.push_back(entries[next]);
        }
        entries.erase(entries.begin() + next);
    }
}


    Rect boundingRect(const vector<Entry>& es) {
        double minX = numeric_limits<double>::max();
        double minY = numeric_limits<double>::max();
        double maxX = numeric_limits<double>::lowest();
        double maxY = numeric_limits<double>::lowest();
        for (const auto& e : es) {
            minX = min(minX, e.rect.minX);
            minY = min(minY, e.rect.minY);
            maxX = max(maxX, e.rect.maxX);
            maxY = max(maxY, e.rect.maxY);
        }
        return Rect(minX, minY, maxX, maxY);
    }

public:
    RTree() { root = new Node(true); }

    void insert(const Rect& rect) {
        Node* node = root;
        vector<Node*> path;
        vector<int> indices;
        while (!node->isLeaf) {
            int idx = chooseSubtree(node, rect);
            path.push_back(node);
            indices.push_back(idx);
            node = node->entries[idx].child;
        }
        node->entries.push_back(Entry(rect));
        if (node->entries.size() > MAX_ENTRIES) {
            Node* newSibling = splitNode(node);
            adjustTree(path, indices, node, newSibling);
        } else {
            adjustTree(path, indices, node, nullptr);
        }
    }

    void search(const Rect& query, vector<Rect>& result) {
        search(root, query, result);
    }

    void imprimir() {
        cout << "\nEstructura del R-Tree:\n";
        printNode(root, 0);
    }

private:
    void search(Node* node, const Rect& query, vector<Rect>& result) {
        for (const auto& e : node->entries) {
            if (e.rect.overlaps(query)) {
                if (node->isLeaf)
                    result.push_back(e.rect);
                else
                    search(e.child, query, result);
            }
        }
    }

    void printNode(Node* node, int nivel) {
        string indent(nivel * 4, ' ');
        if (node->isLeaf) {
            cout << indent << "[Hoja] " << node->entries.size() << " rectángulo(s):\n";
            for (auto& e : node->entries) {
                cout << indent << "    ";
                printRect(e.rect);
            }
        } else {
            cout << indent << "[Interno] " << node->entries.size() << " hijo(s):\n";
            int i = 0;
            for (auto& e : node->entries) {
                cout << indent << "    Hijo " << i << " (MBR): ";
                printRect(e.rect);
                printNode(e.child, nivel + 1);
                i++;
            }
        }
    }

    void printRect(const Rect& r) {
        cout << fixed << setprecision(2);
        cout << "[" << r.minX << ", " << r.minY << ", " << r.maxX << ", " << r.maxY << "]\n";
    }
};

// ----------- Ejemplo de uso en main -----------

int main() {
RTree tree;

    srand(time(0));
    for (int i = 0; i < 25; ++i) {
        double x1 = rand() % 95;
        double y1 = rand() % 95;
        double length = 1 + (rand() % 5);
        double x2 = min(100.0, x1 + length);
        double y2 = min(100.0, y1 + length);

        tree.insert(Rect(x1, y1, x2, y2));
    }

    tree.imprimir();

    return 0;
}