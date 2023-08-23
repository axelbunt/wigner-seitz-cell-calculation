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
    float x;  // for equation x=...
};

// reloading the operator to compare atoms
bool operator==(const Atom& left, const Atom& right) {
    return left.x_abs == right.x_abs && left.y_abs == right.y_abs;
}

// reloading the operator to output the coordinates of a point in the console
ostream& operator << (ostream& os, const Point& point) {
    return os << "x: " << point.x << ", y: " << point.y;
}

// check if atom contains in vector
bool is_atom_in_vector(const Atom& atom, const vector<Atom>& arr) {
    if (arr.empty()) {
        return false;
    }
    if (find(arr.begin(), arr.end(), atom) != arr.end()) {
        return true;
    }
    return false;
}

// function for getting equation of line
Line get_line_equation(float x_1, float y_1, float x_2, float y_2) {
    Line ans{};

    if (abs(x_1 - x_2) <= 1e-9) {  // for equation x=...
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

// function for computing polar angle of atom relative to the origin of coordinates
float compute_polar_angle(const Atom& atom) {
    return atan2(atom.y_abs, atom.x_abs);
}

// function for computing area of polygon based on the coordinates of its vertices
float get_polygon_area(const vector<Point>& points) {
    float area = 0;
    float x1 = points[0].x;
    float y1 = points[0].y;
    float x2, y2;
    for (int i = 1; i < points.size(); ++i) {
        x2 = points[i].x;
        y2 = points[i].y;
        area += (x1 + x2) * (y2 - y1);
        x1 = x2;
        y1 = y2;
    }
    area += (points[0].x + x2) * (points[0].y - y2);
    return abs(area) / 2;
}

Point get_lines_interception(Line& line_1, Line& line_2) {
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
    int n;  // number of atoms in cell
    cin >> n;

    if (n == 1) {
        cout << "False value for n";
        return 1;
    }

    vector<Atom> atoms(n);
    for (int i = 0; i < n; ++i) {  // getting absolute coordinates of the atom
        float x_abs, y_abs;
        cin >> x_abs >> y_abs;
        atoms[i].x_abs = x_abs;
        atoms[i].y_abs = y_abs;

        // we consider the first entered atom to be "initial", that is, its relative coordinates are zeros
        if (i == 0) {
            atoms[i].x_rel = 0;
            atoms[i].y_rel = 0;
        } else {  // calculating the relative coordinates of the atom, if it is not the initial one
            atoms[i].x_rel = x_abs - atoms[0].x_abs;
            atoms[i].y_rel = y_abs - atoms[0].y_abs;
        }
    }

    // we reflect the positions of the atoms relative to the "initial" one, in order to obtain all
    // its neighboring atoms
    vector<Atom> neighbor_atoms;
    // adding the "parents" of symmetric atoms to the vector of neighbors
    neighbor_atoms.insert(neighbor_atoms.end(), atoms.begin() + 1, atoms.end());
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

    // this case is an exception when diagonal atoms are not added to vector of neighbors
    if (n == 4) {
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
    }

    // sorting neighbors by polar angle
    sort(neighbor_atoms.begin(), neighbor_atoms.end(), [](const Atom& first, const Atom& second) {
        return compute_polar_angle(first) < compute_polar_angle(second);
    });

    // vector for all equations of straight median perpendiculars
    vector<Line> median_perp_equations;
    float x_1 = atoms[0].x_abs;
    float y_1 = atoms[0].y_abs;
    for (Atom atom : neighbor_atoms) {
        float x_2 = atom.x_abs;
        float y_2 = atom.y_abs;

        float x_m = (x_1 + x_2) / 2;  // calculate the coordinates of the middle of the segment
        float y_m = (y_1 + y_2) / 2;

        if (abs(y_1 - y_2) <= 1e-9) {  // the case if the median perpendicular is parallel to 0Y
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

    // calculate the extreme points of the Wigner-Seitz cell
    vector<Point> cell_points{};
    if (n == 5) {
        for (int i = 0; i < median_perp_equations.size(); ++i) {
            Line current_line = median_perp_equations[i];
            Line next_line{};
            if (i == median_perp_equations.size() - 1) {
                next_line = median_perp_equations[0];
            } else {
                next_line = median_perp_equations[i + 1];
            }

            cell_points.push_back(get_lines_interception(current_line, next_line));
        }
    } else {
        // getting boundaries at the intersection of mid-perpendiculars not to diagonals
        vector<tuple<Point, Line, Line>> first_border_points{};
        for (int i = 1; i < median_perp_equations.size(); i += 2) {
            Line current_line = median_perp_equations[i];
            Line next_line{};
            if (i == median_perp_equations.size() - 1) {
                next_line = median_perp_equations[1];
            } else {
                next_line = median_perp_equations[i + 2];
            }

            first_border_points.emplace_back(get_lines_interception(current_line, next_line),
                                             current_line, next_line);
        }

        int first_border_counter = 0;
        vector<tuple<Point, Point>> cutting_points;
        unordered_map<Point, vector<Point>> point_replacement{};
        int special_key = 0;
        // bypassing mid-perpendiculars built from the diagonal
        for (int i = 2; i < median_perp_equations.size(); i += 2) {
            tuple<Point, Line, Line> top = first_border_points[first_border_counter];

            Line diagonal_line = median_perp_equations[i];
            Point point_1 = get_lines_interception(diagonal_line, get<1>(top));
            Point point_2 = get_lines_interception(diagonal_line, get<2>(top));
            cutting_points.emplace_back(point_1, point_2);

            Point key = get<0>(top);
            vector<Point> to_replace { point_1, point_2};
            if (special_key % 2 == 0) {
                point_replacement[key] = to_replace;
            }

            first_border_counter += 1;
            special_key += 1;
        }

        for (auto point : first_border_points) {
            Point to_check = get<0>(point);

            if (point_replacement.find(to_check) != point_replacement.end()) {
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

    cout << get_polygon_area(cell_points);

    return 0;
}
