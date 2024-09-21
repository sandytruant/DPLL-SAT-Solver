#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct CNF {
    int num_vars;
    int num_clauses;
    vector<vector<int>> clauses;
};

CNF read_cnf(const string &filename) {
    ifstream file(filename);
    string line;
    CNF cnf;
    while (getline(file, line)) {
        if (line[0] == 'c') {
            continue;
        }
        if (line[0] == 'p') {
            sscanf(line.c_str(), "p cnf %d %d", &cnf.num_vars, &cnf.num_clauses);
            continue;
        }
        vector<int> clause;
        int lit;
        stringstream ss(line);
        while (ss >> lit) {
            if (lit == 0) {
                break;
            }
            clause.push_back(lit);
        }
        cnf.clauses.push_back(clause);
    }
    return cnf;
}

void print_cnf(const CNF &cnf) {
    cout << "p cnf " << cnf.num_vars << " " << cnf.num_clauses << endl;
    for (const auto &clause : cnf.clauses) {
        for (int lit : clause) {
            cout << lit << " ";
        }
        cout << "0" << endl;
    }
}

bool DPLL(CNF &cnf, vector<int> &assignment) {
    // BCP
    while (true) {
        bool changed = false;
        for (const auto &clause : cnf.clauses) {
            int num_unassigned = 0;
            int unassigned_lit = 0;
            for (int lit : clause) {
                if (assignment[abs(lit)] == 0) {
                    num_unassigned++;
                    unassigned_lit = lit;
                }
                else if (assignment[abs(lit)] * lit > 0) {
                    num_unassigned = -1; // clause is satisfied
                    unassigned_lit = 0;
                    break;
                }
            }
            if (num_unassigned == 0) {
                return false;
            }
            if (num_unassigned == 1) {
                assignment[abs(unassigned_lit)] = unassigned_lit > 0 ? 1 : -1;
                changed = true;
            }
        }
        if (!changed) {
            break;
        }
    }

    // Check if all clauses are satisfied
    bool all_satisfied = true;
    for (const auto &clause : cnf.clauses) {
        bool satisfied = false;
        for (int lit : clause) {
            if (assignment[abs(lit)] * lit > 0) {
                satisfied = true;
                break;
            }
        }
        if (!satisfied) {
            all_satisfied = false;
            break;
        }
    }
    if (all_satisfied) {
        cout << "s SATISFIABLE" << endl;
        for (int i = 1; i <= cnf.num_vars; i++) {
            cout << (assignment[i] != 0 ? assignment[i] * i : i) << " ";
        }
        cout << "0" << endl;
        return true;
    }

    // Branch
    for (int i = 1; i <= cnf.num_vars; i++) {
        if (assignment[i] == 0) {
            vector<int> assignment_copy = assignment;
            assignment_copy[i] = 1;
            if (DPLL(cnf, assignment_copy)) {
                return true;
            }
            assignment_copy = assignment;
            assignment_copy[i] = -1;
            if (DPLL(cnf, assignment_copy)) {
                return true;
            }
            return false;
        }
    }
    return false;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <cnf-file>" << endl;
        return 1;
    }
    CNF cnf = read_cnf(argv[1]);
    // print_cnf(cnf);
    vector<int> assignment(cnf.num_vars + 1, 0);
    if (!DPLL(cnf, assignment)) {
        cout << "s UNSATISFIABLE" << endl;
    }
    return 0;
}
