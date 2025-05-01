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

def plot_vpoints_data(content, title, filename):
    # Depuración: Imprimir el contenido procesado
    print(f"Procesando sección para '{title}'")
    print(f"Contenido:\n{content[:500]}...")  # Primeros 500 caracteres

    # Extraer polígonos insertados (vpoints)
    start = content.find("Inserted polygons (vpoints):")
    end = content.find("Query rectangle:")
    if start == -1 or end == -1:
        print("Error: No se encontraron 'Inserted polygons (vpoints):' o 'Query rectangle:'")
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
    start = content.find("Search results (Caso 1):")
    end = content.find("MBRs:")
    if start == -1 or end == -1:
        print("Error: No se encontraron 'Search results (Caso 1):' o 'MBRs:'")
        return
    search_text = content[start:end]
    search_polygons = parse_polygons(search_text)
    print(f"Polígonos encontrados: {len(search_polygons)}")

    # Definir los 6 rectángulos específicos proporcionados
    mbrs = [
        (20, 43, 20, 59),  # Rectángulo 1: (20,59) (20,43) → MBR: (20,43) a (20,59)
        (48, 58, 50, 67),  # Rectángulo 2: (50,58) (48,67) → MBR: (48,58) a (50,67)
        (74, 64, 105, 68), # Rectángulo 3: (105,68) (74,64) → MBR: (74,64) a (105,68)
        (83, 40, 104, 54), # Rectángulo 4: (83,40) (104,54) → MBR: (83,40) a (104,54)
        (20, 43, 20, 59),  # Rectángulo 5: (20,59) (20,43) → MBR: (20,43) a (20,59)
        (48, 67, 105, 68)  # Rectángulo 6: (48,67) (105,68) → MBR: (48,67) a (105,68)
    ]

    # Graficar
    fig, ax = plt.subplots(figsize=(8, 8))
    
    # Graficar los rectángulos específicos (MBRs)
    for i, (min_x, min_y, max_x, max_y) in enumerate(mbrs):
        # Usar verde para los primeros 3 y azul para los últimos 3
        color = 'green' if i < 3 else 'blue'
        ax.add_patch(plt.Rectangle((min_x, min_y), max_x - min_x, max_y - min_y, fill=None, edgecolor=color, linewidth=1))
    
    # Graficar puntos y líneas de los polígonos insertados
    for poly in inserted_polygons:
        x = [p[0] for p in poly]
        y = [p[1] for p in poly]
        # Graficar puntos como marcadores
        ax.plot(x, y, 'ko', markersize=3)  # Puntos negros
        # Graficar líneas entre puntos
        x.append(poly[0][0])  # Cerrar el polígono
        y.append(poly[0][1])
        ax.plot(x, y, 'b-', label='Insertados' if 'Insertados' not in ax.get_legend_handles_labels()[1] else '')
    
    # Graficar puntos y líneas de los polígonos encontrados
    for poly in search_polygons:
        x = [p[0] for p in poly]
        y = [p[1] for p in poly]
        # Graficar puntos como marcadores
        ax.plot(x, y, 'ko', markersize=3)  # Puntos negros
        # Graficar líneas entre puntos
        x.append(poly[0][0])
        y.append(poly[0][1])
        ax.plot(x, y, 'r-', label='Encontrados' if 'Encontrados' not in ax.get_legend_handles_labels()[1] else '')
    
    # Graficar rectángulo de consulta
    ax.add_patch(plt.Rectangle((min_x, min_y), max_x - min_x, max_y - min_y, fill=None, hatch='/', edgecolor='black', label='Consulta'))
    
    ax.set_xlim(-5, 110)
    ax.set_ylim(-5, 75)
    ax.set_aspect('equal')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_title(title)
    ax.legend()
    plt.savefig(filename)
    print(f"Gráfica guardada en: {filename}")
    plt.close()

# Procesar archivo de salida
with open('grafica_6_2_rect.txt', 'r') as f:
    content = f.read()

start = content.find("Datos para graficar vpoints:")
if start == -1:
    print("Error: No se encontró 'Datos para graficar vpoints:'")
else:
    plot_vpoints_data(content[start:], "Rectángulos Específicos de vpoints", "plot_vpoints_mbrs.png")