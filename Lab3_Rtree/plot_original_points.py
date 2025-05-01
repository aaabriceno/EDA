import matplotlib.pyplot as plt

def plot_original_points():
    # Polígonos de coord
    coord_polygons = [
        [(20, 59), (20, 43)],
        [(50, 58), (48, 67)],
        [(105, 68), (74, 64)],
        [(83, 40), (104, 54)]
    ]
    # Polígonos de coord2
    coord2_polygons = [
        [(12, 28), (19, 15)],
        [(40, 29), (69, 25)]
    ]
    # Polígono adicional
    ad_polygon = [(53, 4), (54, 12)]

    fig, ax = plt.subplots(figsize=(8, 8))
    
    # Graficar polígonos de coord
    for poly in coord_polygons:
        x = [p[0] for p in poly] + [poly[0][0]]
        y = [p[1] for p in poly] + [poly[0][1]]
        ax.plot(x, y, 'bo-', label='coord' if 'coord' not in ax.get_legend_handles_labels()[1] else '')
    
    # Graficar polígonos de coord2
    for poly in coord2_polygons:
        x = [p[0] for p in poly] + [poly[0][0]]
        y = [p[1] for p in poly] + [poly[0][1]]
        ax.plot(x, y, 'go-', label='coord2' if 'coord2' not in ax.get_legend_handles_labels()[1] else '')
    
    # Graficar polígono adicional
    x = [p[0] for p in ad_polygon] + [ad_polygon[0][0]]
    y = [p[1] for p in ad_polygon] + [ad_polygon[0][1]]
    ax.plot(x, y, 'ro-', label='ad' if 'ad' not in ax.get_legend_handles_labels()[1] else '')
    
    ax.set_xlim(0, 110)
    ax.set_ylim(0, 110)
    ax.set_aspect('equal')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_title('Polígonos del Código Original')
    ax.legend()
    plt.savefig('plot_original_points.png')
    plt.close()

if __name__ == "__main__":
    plot_original_points()