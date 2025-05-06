#include <vector>
#include <algorithm>
#include <cmath>

class Rect {
public:
    float x_min, x_max, y_min, y_max;

    // Área del rectángulo
    float area() const {
        return (x_max - x_min) * (y_max - y_min);
    }

    // Centro del rectángulo
    std::pair<float, float> center() const {
        return {(x_min + x_max) / 2, (y_min + y_max) / 2};
    }

    // Distancia euclidiana al centro de otro rect
    float distanceTo(const Rect &other) const {
        auto [cx1, cy1] = center();
        auto [cx2, cy2] = other.center();
        return std::sqrt(std::pow(cx1 - cx2, 2) + std::pow(cy1 - cy2, 2));
    }
};

class RStarNode {
public:
    std::vector<Rect> data;
    bool isLeaf;

    // Constructor
    RStarNode(bool leaf = true) : isLeaf(leaf) {}
};

class RStar {
private:
    const int MAX_ENTRIES = 8;     // Capacidad máxima del nodo
    const float REINSERT_PCT = 0.3f; // % de elementos a reinsertar

public:
    // Función principal
    void OverflowTreatment(RStarNode *node) {
        if (node->isLeaf || node == root) {
            // Aplicamos ReInsert si no es la raíz
            if (node != root) {
                ReInsert(node);
            } else {
                Split(node);
            }
        } else {
            // Nodo interno: aplicar Split directamente
            Split(node);
        }
    }

    // Reinsertar parte de los rectángulos más alejados del centro del nodo
    void ReInsert(RStarNode *node) {
        int n = node->data.size();
        int m = std::ceil(n * REINSERT_PCT); // número de elementos a reinsertar

        // Calcular centro del nodo
        Rect mbr = computeMBR(node->data);
        auto center = mbr.center();

        // Ordenar por distancia al centro del nodo
        std::sort(node->data.begin(), node->data.end(),
            [center](const Rect &a, const Rect &b) {
                Rect c1{center.first, center.first, center.second, center.second};
                return a.distanceTo(c1) > b.distanceTo(c1); // descendente
            });

        // Extraer los m más lejanos para reinsertar
        std::vector<Rect> toReinsert(node->data.begin(), node->data.begin() + m);
        node->data.erase(node->data.begin(), node->data.begin() + m);

        // Reinsertar cada uno
        for (const Rect &r : toReinsert) {
            RStarNode *target = ChooseSubTree(r);
            target->data.push_back(r);
            if (target->data.size() > MAX_ENTRIES) {
                OverflowTreatment(target);
            }
        }
    }

    // División del nodo en dos
    void Split(RStarNode *node) {
        // Supongamos un método simple: dividir por eje X usando orden de centroides
        std::sort(node->data.begin(), node->data.end(), [](const Rect &a, const Rect &b) {
            return a.center().first < b.center().first;
        });

        int mid = node->data.size() / 2;

        // Crear dos nuevos nodos
        RStarNode *left = new RStarNode(node->isLeaf);
        RStarNode *right = new RStarNode(node->isLeaf);

        left->data.insert(left->data.end(), node->data.begin(), node->data.begin() + mid);
        right->data.insert(right->data.end(), node->data.begin() + mid, node->data.end());

        // Reemplazar este nodo por los dos hijos si no es raíz
        if (node != root) {
            node->data.clear();
            // Esto normalmente implicaría que el nodo padre ahora apunta a left y right.
            // Aquí podrías agregar left y right al padre del nodo, suponiendo que tienes acceso.
        } else {
            // Si es raíz, crear una nueva raíz
            root = new RStarNode(false);
            root->data.clear(); // normalmente serían punteros a hijos
            // Esto requiere rediseño si trabajas con punteros a hijos
        }
    }

    // Función para obtener el MBR (bounding box) de un conjunto de rectángulos
    Rect computeMBR(const std::vector<Rect> &rects) {
        float xmin = rects[0].x_min, xmax = rects[0].x_max;
        float ymin = rects[0].y_min, ymax = rects[0].y_max;

        for (const Rect &r : rects) {
            xmin = std::min(xmin, r.x_min);
            xmax = std::max(xmax, r.x_max);
            ymin = std::min(ymin, r.y_min);
            ymax = std::max(ymax, r.y_max);
        }

        return Rect{xmin, xmax, ymin, ymax};
    }

    // Asumimos que esta función ya está implementada
    RStarNode* ChooseSubTree(Rect input);

    // Miembros
    RStarNode *root;
};
