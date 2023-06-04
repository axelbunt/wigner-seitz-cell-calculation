#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <tuple>
#include <unordered_map>

using namespace std;

struct Point {
    float x;
    float y;
};

bool operator==(const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

template <>
struct std::hash<Point>
{
    size_t operator()(const Point& k) const
    {
        return hash<float>()(k.x) ^ hash<float>()(k.y);
    }
};

//bool operator<(const Point& p1, const Point& p2) {
//    if (p1.x < p2.x ) {
//        return true;
//    }
//    return false;
//}

class Atom {
public:
    float x_abs;
    float y_abs;
    float x_rel;
    float y_rel;
};

class Line {
public:
    float k;
    float b;
    bool is_parallel_y;
    float x;  // если уравнение вида х=...
};

bool operator==(const Atom& left, const Atom& right) {  // перезагрузка оператора для сравнения атомов
    return left.x_abs == right.x_abs && left.y_abs == right.y_abs;
}

ostream& operator << (ostream& os, const Point& point) {  // перезагрузка оператора для вывода координат точки в консоль
    return os << "x: " << point.x << ", y: " << point.y;
}

bool is_atom_in_vector(const Atom& atom, const vector<Atom>& arr) {  // проверка нахождения атома в массиве
    if (arr.empty()) {
        return false;
    }
    if (find(arr.begin(), arr.end(), atom) != arr.end()) {
        return true;
    }
    return false;
}

Line get_line_equation(float x_1, float y_1, float x_2, float y_2) {  // ф-ция для получения ур-ния прямой
    Line ans{};

    if (abs(x_1 - x_2) <= 1e-9) {  // если уравнение вида х=...
        ans.is_parallel_y = true;
        ans.x = x_1;
    } else {
        ans.is_parallel_y = false;
        float k = (y_2 - y_1) / (x_2 - x_1);
        if (k == -0) {
            k = 0;
        }
        float b = y_1 - k * x_1;
        ans.k = k;
        ans.b = b;
    }

    return ans;
}

float compute_polar_angle(const Atom& atom) {  // вычилсение полярного угла атома
    return atan2(atom.y_abs, atom.x_abs);
}

float get_polygon_square(const vector<Point>& points) {  // вычисление площади многоугольника по координатам вершин
    float square = 0;
    float x1 = points[0].x;
    float y1 = points[0].y;
    float x2, y2;
    for (int i = 1; i < points.size(); ++i) {
        x2 = points[i].x;
        y2 = points[i].y;
        square += (x1 + x2) * (y2 - y1);
        x1 = x2;
        y1 = y2;
    }
    square += (points[0].x + x2) * (points[0].y - y2);
    return abs(square) / 2;
}

Point get_line_interception(Line& line_1, Line& line_2) {
    float x, y;
    if (line_1.is_parallel_y || line_2.is_parallel_y) {
        if (line_1.is_parallel_y) {
            x = line_1.x;
            y = line_2.k * x + line_2.b;
        } else {
            x = line_2.x;
            y = line_1.k * x + line_1.b;
        }
    } else {
        x = (line_1.b - line_2.b) / (line_2.k - line_1.k);
        y = line_2.k * x + line_2.b;
    }
    return Point{x, y};
}

int main() {
    int n;  // вводим количество атомов в ячейке
    cin >> n;

    if (n == 1) {
        cout << "False value for n";
        return 1;
    }

    vector<Atom> atoms(n);
    for (int i = 0; i < n; ++i) {  // вводим абсолютные координаты атома
        float x_abs, y_abs;
        cin >> x_abs >> y_abs;
        atoms[i].x_abs = x_abs;
        atoms[i].y_abs = y_abs;

        if (i == 0) {  // считаем первый введенный атом "начальным", то есть его относительные координаты – нули
            atoms[i].x_rel = 0;
            atoms[i].y_rel = 0;
        } else {  // считаем относительные координаты атома, если он не начальный
            atoms[i].x_rel = x_abs - atoms[0].x_abs;
            atoms[i].y_rel = y_abs - atoms[0].y_abs;
        }
    }

    vector<Atom> neighbor_atoms;  // отражаем положения атомов относительно "начального", для получения всех его соседних атомов
    neighbor_atoms.insert(neighbor_atoms.end(), atoms.begin() + 1, atoms.end());    // добавление "родителей" симметричных атомов в общий вектор соседей
    for (int i = 1; i < n; ++i) {
        Atom new_atom{};
        new_atom.x_abs = atoms[0].x_abs + (atoms[0].x_abs - atoms[i].x_abs);
        new_atom.y_abs = atoms[0].y_abs + (atoms[0].y_abs - atoms[i].y_abs);
        new_atom.x_rel = new_atom.x_abs - atoms[0].x_abs;
        new_atom.y_rel = new_atom.y_abs - atoms[0].y_abs;
        if (!is_atom_in_vector(new_atom, neighbor_atoms)) {
            neighbor_atoms.push_back(new_atom);
        }
    }
    if (n == 4) {  // случай исключение, когда не достраиваются диагональные атомы
        Atom first_atom = atoms[1];
        Atom second_atom = atoms[3];

        float d_x_1 = first_atom.x_abs - atoms[0].x_abs;
        float d_y_1 = first_atom.y_abs - atoms[0].y_abs;
        float d_x_2 = second_atom.x_abs - atoms[0].x_abs;

        Atom new_atom_1{};
        Atom new_atom_2{};
        new_atom_1.x_abs = atoms[0].x_abs - d_x_2 + d_x_1;
        new_atom_1.y_abs = atoms[0].y_abs + d_y_1;

        new_atom_2.x_abs = atoms[0].x_abs + d_x_2 - d_x_1;
        new_atom_2.y_abs = atoms[0].y_abs - d_y_1;
        neighbor_atoms.push_back(new_atom_1);
        neighbor_atoms.push_back(new_atom_2);
//        cout << new_atom_1.x_abs << ' ' << new_atom_1.y_abs << '\n';
//        cout << new_atom_2.x_abs << ' ' << new_atom_2.y_abs << '\n';
    }

    // сортирвока соседей по полярному углу
    sort(neighbor_atoms.begin(), neighbor_atoms.end(), [](const Atom& first, const Atom& second) {
        return compute_polar_angle(first) < compute_polar_angle(second);
    });

//    for (int i = 0; i < neighbor_atoms.size(); ++i) {
//        cout << "Atom " + to_string(i) + ": " + to_string(neighbor_atoms[i].x_abs) + " " + to_string(neighbor_atoms[i].y_abs) + '\n';
//    }

    vector<Line> median_perp_equations;  // вектор для всех уравнений прямых серединных перпендикуляров
    float x_1 = atoms[0].x_abs;
    float y_1 = atoms[0].y_abs;
    for (Atom atom : neighbor_atoms) {
        float x_2 = atom.x_abs;
        float y_2 = atom.y_abs;

        float x_m = (x_1 + x_2) / 2;  // вычисляем координаты середины отрезка
        float y_m = (y_1 + y_2) / 2;

        if (abs(y_1 - y_2) <= 1e-9) {  // случай, если сер пер будет параллелен 0Y
            Line perp_equation{0, 0, true, x_m};
            median_perp_equations.push_back(perp_equation);
        } else {
            float k_12 = (y_2 - y_1) / (x_2 - x_1);
            float k_m = -1 / k_12;
            float b_m = y_m - k_m * x_m;
            Line perp_equation{k_m, b_m, false, 0};

            median_perp_equations.push_back(perp_equation);
        }
    }

//    for (Line line : median_perp_equations) {
//        if (line.is_parallel_y) {
//            cout << "x = " << line.x << '\n';
//        } else {
//            cout << "k = " << line.k << ", b = " << line.b << '\n';
//        }
//    }

    vector<Point> cell_points{};  // вычисляем крайние точки ячейки Вигнера-Зейтца
    if (n == 5) {
        for (int i = 0; i < median_perp_equations.size(); ++i) {
            Line current_line = median_perp_equations[i];
            Line next_line{};
            if (i == median_perp_equations.size() - 1) {
                next_line = median_perp_equations[0];
            } else {
                next_line = median_perp_equations[i + 1];
            }

            cell_points.push_back(get_line_interception(current_line, next_line));
        }
    } else {
        vector<tuple<Point, Line, Line>> first_border_points{};  // получение границ при пересечении серперов не к диагоналям
        for (int i = 1; i < median_perp_equations.size(); i += 2) {
            Line current_line = median_perp_equations[i];
            Line next_line{};
            if (i == median_perp_equations.size() - 1) {
                next_line = median_perp_equations[1];
            } else {
                next_line = median_perp_equations[i + 2];
            }

            first_border_points.emplace_back(get_line_interception(current_line, next_line), current_line, next_line);
        }

        int first_border_counter = 0;
        vector<tuple<Point, Point>> cutting_points;
        unordered_map<Point, vector<Point>> point_replacement{};
        int special_key = 0;
        for (int i = 2; i < median_perp_equations.size(); i += 2) {  // обход серперов построенных от диагонали
            tuple<Point, Line, Line> top = first_border_points[first_border_counter];

            Line diagonal_line = median_perp_equations[i];
            Point point_1 = get_line_interception(diagonal_line, get<1>(top));
            Point point_2 = get_line_interception(diagonal_line, get<2>(top));
            cutting_points.emplace_back(point_1, point_2);

            Point key = get<0>(top);
            vector<Point> to_replace { point_1, point_2};
            if (special_key % 2 == 0) {
                point_replacement[key] = to_replace;
            }

            first_border_counter += 1;
            special_key += 1;
        }

//        for (auto smth : point_replacement) {
//            cout << smth.first << "! " << smth.second[0] << "! " << smth.second[1] << '\n';
//        }
//        for (auto smth : first_border_points) {
//            cout << get<0>(smth) << '\n';
//        }
        for (auto point : first_border_points) {
            Point to_check = get<0>(point);

            if (point_replacement.find(to_check) != point_replacement.end()) {
//                cout << to_check << '\n';
                Point to_push_back_1 = point_replacement[to_check][0];
                Point to_push_back_2 = point_replacement[to_check][1];
                if (to_push_back_1 == to_push_back_2) {
                    cell_points.push_back(to_push_back_1);
                } else {
                    cell_points.push_back(to_push_back_1);
                    cell_points.push_back(to_push_back_2);
                }
            } else {
                cell_points.push_back(to_check);
            }
        }
    }

    for (Point point : cell_points) {
        cout << point << '\n';
    }

    cout << get_polygon_square(cell_points);

    return 0;
}
