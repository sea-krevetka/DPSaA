import graphviz

def parse_tree_file(filename):
    trees = []
    current_tree = []
    current_tree_name = ""
    
    with open(filename, 'r', encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            if line.startswith('==='):
                if current_tree:
                    trees.append((current_tree_name, current_tree))
                    current_tree = []
                current_tree_name = line.strip('= ')
            elif line and not line.isspace():
                parts = line.split()
                if len(parts) >= 1:
                    node_data = int(parts[0])
                    parent = None
                    direction = None
                    
                    if len(parts) >= 2 and parts[1] != 'null':
                        parent = int(parts[1])
                    if len(parts) >= 3:
                        direction = parts[2]
                    
                    current_tree.append({
                        'data': node_data,
                        'parent': parent,
                        'direction': direction
                    })
    
    if current_tree:
        trees.append((current_tree_name, current_tree))
    
    return trees

def build_graphviz_tree(tree_data, tree_name):
    dot = graphviz.Digraph(comment=tree_name)
    dot.attr('graph', label=tree_name, labelloc='t', fontsize='16')
    
    # Создаем все узлы
    for node in tree_data:
        dot.node(str(node['data']), str(node['data']))
    
    # Создаем связи между узлами
    for node in tree_data:
        if node['parent'] is not None and node['direction'] is not None:
            if node['direction'] == 'left':
                dot.edge(str(node['parent']), str(node['data']), label='L')
            elif node['direction'] == 'right':
                dot.edge(str(node['parent']), str(node['data']), label='R')
    
    return dot

def visualize_trees_from_file(filename):
    trees = parse_tree_file(filename)
    
    for i, (tree_name, tree_data) in enumerate(trees, 1):
        print(f"Обработка {tree_name}...")
        print(f"Количество узлов: {len(tree_data)}")
        
        dot = build_graphviz_tree(tree_data, tree_name)
        
        output_filename = f"tree_{i}"
        dot.render(output_filename, format='png', cleanup=True)
        print(f"Дерево сохранено как {output_filename}.png")
        
        print("DOT код:")
        print(dot.source[:200] + "..." if len(dot.source) > 200 else dot.source)
        print("-" * 50)

if __name__ == "__main__":
    input_filename = "trees.txt"
    
    try:
        visualize_trees_from_file(input_filename)
        print("Визуализация деревьев завершена успешно!")
    except FileNotFoundError:
        print(f"Файл {input_filename} не найден!")
    except Exception as e:
        print(f"Произошла ошибка: {e}")
