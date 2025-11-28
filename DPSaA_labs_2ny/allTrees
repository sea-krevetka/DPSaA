import random
from typing import Optional, List
import graphviz
import os
from collections import Counter

class Vertex:
    def __init__(self, data: int, char: str = '', weight: int = 1, balance: int = 0):
        self.Data = data  # ASCII код или Unicode код
        self.char = char  # Оригинальный символ
        self.weight = weight
        self.balance = balance
        self.use = False
        self.Left: Optional['Vertex'] = None
        self.Right: Optional['Vertex'] = None

# Глобальные переменные для B-дерева
VR = 1
HR = 1

# ==================== ВИЗУАЛИЗАЦИЯ ====================

def visualize_tree(root: Optional[Vertex], name: str, algorithm: str) -> None:
    """Визуализация дерева с помощью Graphviz"""
    if root is None:
        print("Дерево пустое, визуализация невозможна")
        return
    
    # Создаем граф
    dot = graphviz.Digraph(comment=f'Tree - {algorithm}')
    # Улучшаем качество: DPI для растровых форматов и увеличенные отступы между уровнями
    dot.attr(rankdir='TB', size='8,5')
    dot.graph_attr.update({'dpi': '300', 'nodesep': '0.6', 'ranksep': '0.8'})
    # Дефолтные атрибуты для узлов и ребер (чтобы текст был читаемым)
    dot.node_attr.update({'shape': 'box', 'fontsize': '10', 'fontname': 'Arial'})
    dot.edge_attr.update({'fontsize': '9', 'fontname': 'Arial'})
    
    # Рекурсивно добавляем вершины и ребра
    def add_nodes_edges(vertex: Optional[Vertex]) -> None:
        if vertex is None:
            return
        
        # Создаем метку для вершины с символом, данными и весом
        if vertex.weight != 1:
            # Отображаем символ, код и вес
            if vertex.char and vertex.char.strip():
                label = f"'{vertex.char}'\\n{vertex.Data}\\nw:{vertex.weight}\\nb:{vertex.balance}"
            else:
                label = f"{vertex.Data}\\nw:{vertex.weight}\\nb:{vertex.balance}"
        else:
            # Только символ и код
            if vertex.char and vertex.char.strip():
                label = f"'{vertex.char}'\\n{vertex.Data}\\nb:{vertex.balance}"
            else:
                label = f"{vertex.Data}\\nb:{vertex.balance}"
        
        dot.node(str(id(vertex)), label)

        # Добавляем левого потомка (с меткой L)
        if vertex.Left is not None:
            dot.edge(str(id(vertex)), str(id(vertex.Left)), label='L')
            add_nodes_edges(vertex.Left)

        # Добавляем правого потомка (с меткой R)
        if vertex.Right is not None:
            dot.edge(str(id(vertex)), str(id(vertex.Right)), label='R')
            add_nodes_edges(vertex.Right)
    
    add_nodes_edges(root)
    
    # Сохраняем и показываем граф
    filename = f"tree_{name}_{random.randint(1000, 9999)}"
    # Сохраняем в векторный формат по умолчанию (SVG) для лучшего качества масштабирования.
    try:
        dot.render(filename, format='png', cleanup=True, view=True)
        print(f"\nДерево сохранено как {filename}.svg")
    except Exception:
        # fallback на PNG с высоким DPI
        dot.render(filename, format='png', cleanup=True, view=True)
        print(f"\nДерево сохранено как {filename}.png")

# ==================== ОСНОВНЫЕ ФУНКЦИИ ДЛЯ ВСЕХ АЛГОРИТМОВ ====================

def left_to_right(p: Optional[Vertex]) -> None:
    """Симметричный обход дерева (левый-корень-правый)"""
    if p is not None:
        left_to_right(p.Left)
        if p.char and p.char.strip():
            print(f"'{p.char}'(код:{p.Data},w:{p.weight})", end=" ")
        else:
            print(f"{p.Data}(w:{p.weight})", end=" ")
        left_to_right(p.Right)

def size(p: Optional[Vertex]) -> int:
    """Размер дерева (количество вершин)"""
    if p is None:
        return 0
    return 1 + size(p.Left) + size(p.Right)

def check_sum(p: Optional[Vertex]) -> int:
    """Сумма всех элементов дерева"""
    if p is None:
        return 0
    return p.Data + check_sum(p.Left) + check_sum(p.Right)

def height(p: Optional[Vertex]) -> int:
    """Высота дерева"""
    if p is None:
        return 0
    lh = height(p.Left)
    rh = height(p.Right)
    return 1 + max(lh, rh)

def total_weight(p: Optional[Vertex]) -> int:
    """Общий вес дерева"""
    if p is None:
        return 0
    return p.weight + total_weight(p.Left) + total_weight(p.Right)

def weighted_path_sum(p: Optional[Vertex], level: int) -> int:
    """Взвешенная сумма длин путей"""
    if p is None:
        return 0
    return (p.weight * level) + weighted_path_sum(p.Left, level + 1) + weighted_path_sum(p.Right, level + 1)

def weighted_average_height(root: Optional[Vertex]) -> float:
    """Средняя взвешенная высота дерева"""
    if root is None:
        return 0.0
    total_weight_val = total_weight(root)
    if total_weight_val == 0:
        return 0.0
    weighted_path_sum_val = weighted_path_sum(root, 1)
    return weighted_path_sum_val / total_weight_val

def average_height(root: Optional[Vertex]) -> float:
    """Средняя высота дерева"""
    if root is None:
        return 0.0
    def path_length_sum(p: Optional[Vertex], level: int) -> int:
        if p is None:
            return 0
        return level + path_length_sum(p.Left, level + 1) + path_length_sum(p.Right, level + 1)
    return path_length_sum(root, 1) / size(root)

# ==================== ОБРАБОТКА ВВОДА ====================

def get_input_string() -> str:
    """Получение строки от пользователя"""
    print("\n" + "="*50)
    print("ВВОД ДАННЫХ ДЛЯ ДЕРЕВА")
    print("="*50)
    input_string = input("Введите строку символов: ").strip()
    
    if not input_string:
        print("Строка пустая! Используется строка по умолчанию.")
        return "ПриветМирДерево2024🌳"
    
    print(f"Введенная строка: '{input_string}'")
    print(f"Длина строки: {len(input_string)} символов")
    print(f"Уникальных символов: {len(set(input_string))}")
    return input_string

def process_input_string(input_string: str, use_weights: bool = False) -> tuple[List[int], List[int], List[str]]:
    """Обработка входной строки и преобразование в данные для дерева.

    Из строки выбирается до 10 уникальных символов (по убыванию частоты).
    Для алгоритмов с весами вес символа равен количеству его вхождений в полной строке.
    Для алгоритмов без весов все веса устанавливаются в 1.
    Возвращает кортеж: (data_codes, weights, chars)
    """
    if not input_string:
        return [], [], []

    # Подсчитываем частоту символов во всей строке
    char_counter = Counter(input_string)

    # Выбираем до 10 уникальных символов, сохраняя порядок их первого появления в строке
    seen = set()
    selected: List[str] = []
    for ch in input_string:
        if ch not in seen:
            seen.add(ch)
            selected.append(ch)
            if len(selected) >= 10:
                break

    data_codes: List[int] = []
    weights: List[int] = []
    chars: List[str] = []

    for ch in selected:
        data_codes.append(ord(ch))
        if use_weights:
            weights.append(char_counter[ch])
        else:
            weights.append(1)
        chars.append(ch)

    return data_codes, weights, chars

def create_vertex_list(data: List[int], chars: List[str], weights: List[int]) -> List[Vertex]:
    """Создание списка вершин из данных, символов и весов"""
    return [Vertex(data[i], chars[i], weights[i]) for i in range(len(data))]

def insert_sort(data: List[int], chars: List[str], weights: List[int]) -> None:
    """Сортировка вставками данных, символов и весов"""
    n = len(data)
    for i in range(1, n):
        temp_data = data[i]
        temp_char = chars[i]
        temp_weight = weights[i]
        j = i - 1
        while j >= 0 and temp_data < data[j]:
            data[j + 1] = data[j]
            chars[j + 1] = chars[j]
            weights[j + 1] = weights[j]
            j -= 1
        data[j + 1] = temp_data
        chars[j + 1] = temp_char
        weights[j + 1] = temp_weight

# ==================== АЛГОРИТМЫ ПОСТРОЕНИЯ ДЕРЕВЬЕВ ====================

def add_vertex(root: Optional[Vertex], data: int, char: str, weight: int = 1) -> Vertex:
    """Добавление вершины в дерево поиска"""
    if root is None:
        return Vertex(data, char, weight)
    elif data < root.Data:
        root.Left = add_vertex(root.Left, data, char, weight)
    elif data > root.Data:
        root.Right = add_vertex(root.Right, data, char, weight)
    return root

# 1. Идеально сбалансированное дерево поиска (ISDP)
def build_ISDP(L: int, R: int, data: List[int], chars: List[str], weights: List[int]) -> Optional[Vertex]:
    """Построение идеально сбалансированного дерева поиска"""
    if L > R:
        return None
    m = (L + R) // 2
    p = Vertex(data[m], chars[m], weights[m])
    p.Left = build_ISDP(L, m - 1, data, chars, weights)
    p.Right = build_ISDP(m + 1, R, data, chars, weights)
    return p

# 2. Случайное дерево поиска (рекурсивное)
def add_recursive_SDP(p: Optional[Vertex], data: int, char: str, weight: int = 1) -> Vertex:
    """Рекурсивное добавление в дерево поиска"""
    if p is None:
        return Vertex(data, char, weight)
    elif data < p.Data:
        p.Left = add_recursive_SDP(p.Left, data, char, weight)
    elif data > p.Data:
        p.Right = add_recursive_SDP(p.Right, data, char, weight)
    return p

# 3. Случайное дерево поиска (двойная косвенность)
def add_double_SDP(root: Optional[Vertex], data: int, char: str, weight: int = 1) -> Vertex:
    """Добавление в дерево поиска с двойной косвенностью"""
    if root is None:
        return Vertex(data, char, weight)
    
    current = root
    parent = None
    direction = None
    
    while current is not None:
        parent = current
        if data < current.Data:
            current = current.Left
            direction = 'left'
        elif data > current.Data:
            current = current.Right
            direction = 'right'
        else:
            return root
    
    new_vertex = Vertex(data, char, weight)
    if direction == 'left':
        parent.Left = new_vertex
    else:
        parent.Right = new_vertex
    
    return root

# 4. АВЛ дерево
def ll_rotation(p: Vertex) -> Vertex:
    """LL-поворот для АВЛ дерева"""
    q = p.Left
    p.Left = q.Right
    q.Right = p
    p.balance = 0
    q.balance = 0
    return q

def lr_rotation(p: Vertex) -> Vertex:
    """LR-поворот для АВЛ дерева"""
    q = p.Left
    r = q.Right

    if r.balance < 0:
        p.balance = 1
    else:
        p.balance = 0

    if r.balance > 0:
        q.balance = -1
    else:
        q.balance = 0

    r.balance = 0

    q.Right = r.Left
    p.Left = r.Right
    r.Left = q
    r.Right = p
    return r

def rl_rotation(p: Vertex) -> Vertex:
    """RL-поворот для АВЛ дерева"""
    q = p.Right
    r = q.Left

    if r.balance > 0:
        p.balance = -1
    else:
        p.balance = 0

    if r.balance < 0:
        q.balance = 1
    else:
        q.balance = 0

    r.balance = 0

    q.Left = r.Right
    p.Right = r.Left
    r.Left = p
    r.Right = q
    return r

def rr_rotation(p: Vertex) -> Vertex:
    """RR-поворот для АВЛ дерева"""
    q = p.Right
    p.Right = q.Left
    q.Left = p
    p.balance = 0
    q.balance = 0
    return q

def insert_AVL(D: int, char: str, weight: int, p: Optional[Vertex]) -> tuple[Optional[Vertex], bool]:
    """Вставка в АВЛ дерево"""
    if p is None:
        return Vertex(D, char, weight), True

    growth = False
    if D < p.Data:
        p.Left, growth = insert_AVL(D, char, weight, p.Left)
        if growth:
            if p.balance > 0:
                p.balance = 0
                return p, False
            elif p.balance == 0:
                p.balance = -1
                return p, True
            else:
                if p.Left.balance < 0:
                    p = ll_rotation(p)
                else:
                    p = lr_rotation(p)
                return p, False
    elif D > p.Data:
        p.Right, growth = insert_AVL(D, char, weight, p.Right)
        if growth:
            if p.balance < 0:
                p.balance = 0
                return p, False
            elif p.balance == 0:
                p.balance = 1
                return p, True
            else:
                if p.Right.balance > 0:
                    p = rr_rotation(p)
                else:
                    p = rl_rotation(p)
                return p, False
    return p, False

# 5. Б-дерево (B-tree)
def B2INSERT(D: int, char: str, weight: int, p: Optional[Vertex]) -> tuple[Optional[Vertex], int, int]:
    """Вставка в Б-дерево"""
    global VR, HR
    if p is None:
        new_vertex = Vertex(D, char, weight)
        VR = 1
        return new_vertex, VR, HR

    if D < p.Data:
        p.Left, VR, HR = B2INSERT(D, char, weight, p.Left)
        if VR == 1:
            if p.balance == 0:
                q = p.Left
                p.Left = q.Right
                q.Right = p
                q.balance = 1
                VR = 0
                HR = 1
                return q, VR, HR
            else:
                p.balance = 0
                VR = 1
                HR = 0
        else:
            HR = 0
    elif D > p.Data:
        p.Right, VR, HR = B2INSERT(D, char, weight, p.Right)
        if VR == 1:
            p.balance = 1
            HR = 1
            VR = 0
        elif HR == 1:
            if p.balance == 1:
                q = p.Right
                p.balance = 0
                q.balance = 0
                p.Right = q.Left
                q.Left = p
                VR = 1
                HR = 0
                return q, VR, HR
            else:
                HR = 0
    return p, VR, HR

# 6. Дерево оптимального поиска
def calculate_aw(n: int, weights: List[int]) -> List[List[int]]:
    """Вычисление матрицы весов AW"""
    AW = [[0] * (n+1) for _ in range(n+1)]
    for i in range(n+1):
        for j in range(i+1, n+1):
            AW[i][j] = AW[i][j-1] + weights[j]
    return AW

def calculate_APAR(n: int, AW: List[List[int]]) -> tuple[List[List[int]], List[List[int]]]:
    """Вычисление матриц AP и AR для оптимального дерева"""
    AP = [[0] * (n+1) for _ in range(n+1)]
    AR = [[0] * (n+1) for _ in range(n+1)]
    
    for i in range(n):
        j = i + 1
        AP[i][j] = AW[i][j]
        AR[i][j] = j

    for h in range(2, n+1):
        for i in range(n - h + 1):
            j = i + h
            m = AR[i][j-1]
            min_val = AP[i][m-1] + AP[m][j]
            
            for k in range(m+1, AR[i+1][j] + 1):
                x = AP[i][k-1] + AP[k][j]
                if x < min_val:
                    m = k
                    min_val = x
            
            AP[i][j] = min_val + AW[i][j]
            AR[i][j] = m
    
    return AP, AR

def create_optimal_tree(L: int, R: int, data: List[int], chars: List[str], weights: List[int], AR: List[List[int]]) -> Optional[Vertex]:
    """Создание оптимального дерева поиска"""
    if L < R:
        k = AR[L][R]
        root = Vertex(data[k], chars[k], weights[k])
        root.Left = create_optimal_tree(L, k-1, data, chars, weights, AR)
        root.Right = create_optimal_tree(k, R, data, chars, weights, AR)
        return root
    return None

# 7. Алгоритм A1 (по весам)
def build_tree_by_weight_A1(data: List[int], chars: List[str], weights: List[int]) -> Optional[Vertex]:
    """Построение дерева алгоритмом A1 (сортировка по убыванию весов)"""
    n = len(data)
    Root = None
    V = [Vertex(data[i], chars[i], weights[i]) for i in range(n)]
    
    for _ in range(n):
        max_weight = 0
        index = 0
        for j in range(n):
            if V[j].weight > max_weight and not V[j].use:
                max_weight = V[j].weight
                index = j
        V[index].use = True
        Root = add_vertex(Root, V[index].Data, V[index].char, V[index].weight)
    
    return Root

# 8. Алгоритм A2 (медианный)
def build_A2_tree(L: int, R: int, data: List[int], chars: List[str], weights: List[int]) -> Optional[Vertex]:
    """Построение дерева алгоритмом A2 (медианное разбиение)"""
    if L <= R:
        total_weight = sum(weights[L:R+1])
        if total_weight == 0:
            # if all weights zero, fall back to middle index
            median_index = (L + R) // 2
        else:
            half = total_weight // 2  # use integer division to match C behaviour
            current_sum = 0
            median_index = L
            for i in range(L, R+1):
                if current_sum < half and current_sum + weights[i] >= half:
                    median_index = i
                    break
                current_sum += weights[i]
        
        root = Vertex(data[median_index], chars[median_index], weights[median_index])
        root.Left = build_A2_tree(L, median_index - 1, data, chars, weights)
        root.Right = build_A2_tree(median_index + 1, R, data, chars, weights)
        return root
    return None

# ==================== ИНТЕРФЕЙС И ВЫВОД ====================

def print_statistics(root: Optional[Vertex], name: str) -> None:
    """Вывод статистики по дереву"""
    print(f"\n{name}:")
    print(f"Размер: {size(root)}")
    print(f"Контрольная сумма: {check_sum(root)}")
    print(f"Высота: {height(root)}")
    print(f"Средняя высота: {average_height(root):.2f}")
    if any(node.weight != 1 for node in preorder_traversal(root)):
        print(f"Средняя взвешенная высота: {weighted_average_height(root):.2f}")

def preorder_traversal(root: Optional[Vertex]) -> List[Vertex]:
    """Обход дерева в порядке корень-левый-правый"""
    if root is None:
        return []
    return [root] + preorder_traversal(root.Left) + preorder_traversal(root.Right)

def print_weights_info(input_string: str, chars: List[str], weights: List[int]) -> None:
    """Вывод информации о весах для выбранных символов

    input_string - полная исходная строка (используется для подсчета частот)
    chars - список выбранных символов (до 10)
    weights - соответствующие веса (количества или 1)
    """
    if len(weights) > 0 and any(w != 1 for w in weights):
        char_counter = Counter(input_string)
        print("\nИнформация о весах (на основе повторений символов):")
        print(f"Всего выбранных символов: {len(weights)}")
        print(f"Уникальных символов в исходной строке: {len(char_counter)}")
        print(f"Максимальный вес среди выбранных: {max(weights)}")
        print(f"Минимальный вес среди выбранных: {min(weights)}")
        print(f"Общий вес (выбранных): {sum(weights)}")

        # Показываем выбранные символы и их веса (на основе полной строки)
        print("\nВыбранные символы (символ : количество_вхождений -> вес):")
        for ch, w in zip(chars, weights):
            print(f"  '{ch}' (код {ord(ch)}): {char_counter.get(ch,0)} повторений -> вес {w}")

        # Показываем топ-5 самых частых символов в исходной строке
        most_common = char_counter.most_common(5)
        if most_common:
            print("\nТоп-5 самых частых символов в исходной строке:")
            for char, count in most_common:
                print(f"  '{char}' (код {ord(char)}): {count} повторений")

def print_menu() -> None:
    """Вывод меню алгоритмов"""
    print("\n" + "="*60)
    print("ВЫБЕРИТЕ АЛГОРИТМ ПОСТРОЕНИЯ ДЕРЕВА:")
    print("="*60)
    print("1 - Идеально сбалансированное дерево поиска (ISDP)")
    print("2 - Случайное дерево поиска (рекурсивное)")
    print("3 - Случайное дерево поиска (двойная косвенность)")
    print("4 - АВЛ дерево")
    print("5 - Б-дерево (B-tree)")
    print("6 - Оптимальное дерево поиска")
    print("7 - Алгоритм A1 (по весам)")
    print("8 - Алгоритм A2 (медианный)")
    print("0 - Выход")
    print("="*60)

def main():
    """Основная функция проекта"""
    print("ПРОЕКТ: АЛГОРИТМЫ ПОСТРОЕНИЯ ДЕРЕВЬЕВ ПОИСКА")
    print("✓ Поддержка кириллицы, эмодзи и специальных символов")
    
    # Проверяем наличие Graphviz
    try:
        graphviz.version()
        print("✓ Graphviz доступен для визуализации деревьев")
    except:
        print("⚠ Graphviz не установлен! Визуализация будет недоступна.")
        print("Установите Graphviz: https://graphviz.org/download/")
    
    while True:
        print_menu()
        choice = input("Выберите алгоритм (0-8): ").strip()
        
        if choice == '0':
            print("Выход из программы.")
            break
        
        if choice not in ['1', '2', '3', '4', '5', '6', '7', '8']:
            print("Неверный выбор! Попробуйте снова.")
            continue
        
        # Получаем строку от пользователя
        input_string = get_input_string()
        
        # Обрабатываем строку для создания данных
        use_weights = choice in ['6', '7', '8']  # Эти алгоритмы используют веса
        data, weights, chars = process_input_string(input_string, use_weights)

        if not data:
            print("Ошибка: не удалось создать данные из строки!")
            continue
        
        # Выводим информацию о весах (если используются)
        if use_weights:
            print_weights_info(input_string, chars, weights)
        
        # Сортировка данных для некоторых алгоритмов
        if choice in ['1', '6']:
            sorted_data = data.copy()
            sorted_chars = chars.copy()
            sorted_weights = weights.copy()
            insert_sort(sorted_data, sorted_chars, sorted_weights)
            print(f"\nОтсортировано данных: {len(sorted_data)} элементов")
        else:
            sorted_data = data
            sorted_chars = chars
            sorted_weights = weights
        
        root = None
        algorithm_name = ""
        
        # Построение дерева выбранным алгоритмом
        if choice == '1':
            # ISDP
            root = build_ISDP(0, len(sorted_data)-1, sorted_data, sorted_chars, sorted_weights)
            algorithm_name = "ISDP"
            print("\nПостроено идеально сбалансированное дерево поиска")
            
        elif choice == '2':
            # Рекурсивное SDP (вставляем в порядке появления в исходной строке)
            for i in range(len(data)):
                root = add_recursive_SDP(root, data[i], chars[i], weights[i])
            algorithm_name = "SDP_Recursive"
            print("\nПостроено случайное дерево поиска (рекурсивное)")
            
        elif choice == '3':
            # Двойная косвенность SDP (вставляем в порядке появления в исходной строке)
            for i in range(len(data)):
                root = add_double_SDP(root, data[i], chars[i], weights[i])
            algorithm_name = "SDP_Double"
            print("\nПостроено случайное дерево поиска (двойная косвенность)")
            
        elif choice == '4':
            # АВЛ дерево
            for i in range(len(data)):
                root, _ = insert_AVL(data[i], chars[i], weights[i], root)
            algorithm_name = "AVL"
            print("\nПостроено АВЛ дерево")
            
        elif choice == '5':
            # Б-дерево
            global VR, HR
            for i in range(len(data)):
                root, VR, HR = B2INSERT(data[i], chars[i], weights[i], root)
            algorithm_name = "B_Tree"
            print("\nПостроено Б-дерево")
            
        elif choice == '6':
            # Оптимальное дерево поиска
            optimal_data = [0] + sorted_data
            optimal_chars = [''] + sorted_chars  # Пустой символ для нулевого элемента
            optimal_weights = [0] + sorted_weights
            AW = calculate_aw(len(sorted_data), optimal_weights)
            AP, AR = calculate_APAR(len(sorted_data), AW)
            root = create_optimal_tree(0, len(sorted_data), optimal_data, optimal_chars, optimal_weights, AR)
            calculated_weight = AP[0][len(sorted_data)] / AW[0][len(sorted_data)] if AW[0][len(sorted_data)] != 0 else 0
            algorithm_name = "Optimal"
            print(f"\nПостроено оптимальное дерево поиска")
            print(f"Теоретическая средняя взвешенная высота: {calculated_weight:.2f}")
            
        elif choice == '7':
            # Алгоритм A1
            root = build_tree_by_weight_A1(sorted_data, sorted_chars, sorted_weights)
            algorithm_name = "A1_Weight"
            print("\nПостроено дерево алгоритмом A1 (по весам)")
            
        elif choice == '8':
            # Алгоритм A2
            root = build_A2_tree(0, len(sorted_data)-1, sorted_data, sorted_chars, sorted_weights)
            algorithm_name = "A2_Median"
            print("\nПостроено дерево алгоритмом A2 (медианный)")
        
        # Вывод результатов
        if root:
            print("\nОбход дерева (симметричный) - символы и коды:")
            left_to_right(root)
            
            print_statistics(root, "\nСтатистика дерева")
            
            # Визуализация
            try:
                visualize_choice = input("\nВизуализировать дерево? (y/n, по умолчанию y): ").strip().lower()
                if visualize_choice != 'n':
                    visualize_tree(root, algorithm_name, get_algorithm_full_name(choice))
            except Exception as e:
                print(f"Ошибка при визуализации: {e}")
                print("Убедитесь, что Graphviz установлен и добавлен в PATH")
        else:
            print("Ошибка: дерево не построено!")
        
        input("\nНажмите Enter для продолжения...")

def get_algorithm_full_name(choice: str) -> str:
    """Получить полное название алгоритма"""
    names = {
        '1': 'Идеально сбалансированное дерево поиска',
        '2': 'Случайное дерево поиска (рекурсивное)',
        '3': 'Случайное дерево поиска (двойная косвенность)',
        '4': 'АВЛ дерево',
        '5': 'Б-дерево',
        '6': 'Оптимальное дерево поиска',
        '7': 'Алгоритм A1 (по весам)',
        '8': 'Алгоритм A2 (медианный)'
    }
    return names.get(choice, "Неизвестный алгоритм")

if __name__ == "__main__":
    main()
