import matplotlib.pyplot as plt
import re

def parse_polygons(text):
    polygons = []
    for line in text.split('\n'):
        if line.startswith("Polygon: "):
            points_str = line[len("Polygon: "):].strip()
            points = [tuple(map(int, re.findall(r'\d+', p))) for p in points_str.split()]
            polygons.append(points)
    return polygons

def plot_rtree_data(content, title, filename):
    # Depuración: Imprimir el contenido procesado
    print(f"Procesando sección para '{title}'")
    print(f"Contenido:\n{content[:500]}...")  # Primeros 500 caracteres

    # Extraer polígonos insertados
    start = content.find("Inserted polygons:")
    end = content.find("Query rectangle:")
    if start == -1 or end == -1:
        print("Error: No se encontraron 'Inserted polygons:' o 'Query rectangle:'")
        return
    inserted_text = content[start:end]
    inserted_polygons = parse_polygons(inserted_text)
    print(f"Polígonos insertados encontrados: {len(inserted_polygons)}")

    # Extraer rectángulo de consulta
    query_match = re.search(r"Query [Rr]ectangle: \((-?\d+),\s*(-?\d+)\)\s*to\s*\((-?\d+),\s*(-?\d+)\)", content)
    if query_match:
        min_x, min_y, max_x, max_y = map(int, query_match.groups())
        print(f"Rectángulo de consulta: ({min_x},{min_y}) to ({max_x},{max_y})")
    else:
        print("Error: Query rectangle not found")
        return

    # Extraer resultados de búsqueda
    start = content.find("Search results:")
    if start == -1:
        print("Error: No se encontraron 'Search results:'")
        return
    search_text = content[start:]
    search_polygons = parse_polygons(search_text)
    print(f"Polígonos encontrados: {len(search_polygons)}")

    # Graficar
    fig, ax = plt.subplots(figsize=(8, 8))
    
    # Graficar polígonos insertados
    for poly in inserted_polygons:
        x = [p[0] for p in poly] + [poly[0][0]]  # Cerrar el polígono
        y = [p[1] for p in poly] + [poly[0][1]]
        ax.plot(x, y, 'bo-', label='Insertados' if 'Insertados' not in ax.get_legend_handles_labels()[1] else '')
    
    # Graficar resultados de búsqueda
    for poly in search_polygons:
        x = [p[0] for p in poly] + [poly[0][0]]
        y = [p[1] for p in poly] + [poly[0][1]]
        ax.plot(x, y, 'ro-', label='Encontrados' if 'Encontrados' not in ax.get_legend_handles_labels()[1] else '')
    
    # Graficar rectángulo de consulta
    ax.add_patch(plt.Rectangle((min_x, min_y), max_x - min_x, max_y - min_y, fill=None, hatch='/', edgecolor='black', label='Consulta'))
    
    ax.set_xlim(-5, 55)
    ax.set_ylim(-5, 55)
    ax.set_aspect('equal')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_title(title)
    ax.legend()
    plt.savefig(filename)
    print(f"Gráfica guardada en: {filename}")
    plt.close()

# Procesar archivos de salida
files = [
    ('output_maxnodes2.txt', '5 Poligonos, MAXNODES=2', 'plot_5_maxnodes2.png'),
    ('output_maxnodes2.txt', '12 Poligonos, MAXNODES=2', 'plot_12_maxnodes2.png'),
    ('output_maxnodes3.txt', '5 Poligonos, MAXNODES=3', 'plot_5_maxnodes3.png'),
    ('output_maxnodes3.txt', '12 Poligonos, MAXNODES=3', 'plot_12_maxnodes3.png')
]

for file_path, title, output_file in files:
    print(f"\nAbriendo archivo: {file_path}")
    with open(file_path, 'r') as f:
        content = f.read()
    if '5 Poligonos' in title:
        start = content.find("Prueba con 5 poligonos")
        end = content.find("Prueba con 12 poligonos")
        if start == -1 or end == -1:
            print("Error: No se encontraron las secciones esperadas para 5 poligonos")
            continue
        plot_rtree_data(content[start:end], title, output_file)
    else:
        start = content.find("Prueba con 12 poligonos")
        if start == -1:
            print("Error: No se encontró la sección para 12 poligonos")
            continue
        plot_rtree_data(content[start:], title, output_file)