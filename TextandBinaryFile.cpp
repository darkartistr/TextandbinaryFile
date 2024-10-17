#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

class Teacher{
public:
    int personalNumber = 0;
    char fullName[51] = "";
    char position[31] = "";
    char academicDegree[31] = "";
    int departmentCode = 0;
    char departmentName[51] = "";

    Teacher() = default;

    Teacher(const Teacher& other)
        : personalNumber(other.personalNumber), departmentCode(other.departmentCode) {
        std::copy(std::begin(other.fullName), std::end(other.fullName), std::begin(fullName));
        std::copy(std::begin(other.position), std::end(other.position), std::begin(position));
        std::copy(std::begin(other.academicDegree), std::end(other.academicDegree), std::begin(academicDegree));
        std::copy(std::begin(other.departmentName), std::end(other.departmentName), std::begin(departmentName));
    }

    Teacher& operator=(const Teacher& other) {
        if (this != &other) {
            personalNumber = other.personalNumber;
            departmentCode = other.departmentCode;
            std::copy(std::begin(other.fullName), std::end(other.fullName), std::begin(fullName));
            std::copy(std::begin(other.position), std::end(other.position), std::begin(position));
            std::copy(std::begin(other.academicDegree), std::end(other.academicDegree), std::begin(academicDegree));
            std::copy(std::begin(other.departmentName), std::end(other.departmentName), std::begin(departmentName));
        }
        return *this;
    }

    bool operator>(const Teacher& other) const {
        return departmentCode > other.departmentCode;
    }

    bool operator==(const Teacher& other) const {
        return personalNumber == other.personalNumber && std::equal(std::begin(fullName), std::end(fullName), std::begin(other.fullName));
    }

    friend std::ifstream& operator>>(std::ifstream& file, Teacher& t) {
        file.read(reinterpret_cast<char*>(&t.personalNumber), sizeof(t.personalNumber));
        file.read(t.fullName, sizeof(t.fullName));
        file.read(t.position, sizeof(t.position));
        file.read(t.academicDegree, sizeof(t.academicDegree));
        file.read(reinterpret_cast<char*>(&t.departmentCode), sizeof(t.departmentCode));
        file.read(t.departmentName, sizeof(t.departmentName));
        return file;
    }

    friend std::ofstream& operator<<(std::ofstream& file, const Teacher& t) {
        file.write(reinterpret_cast<const char*>(&t.personalNumber), sizeof(t.personalNumber));
        file.write(t.fullName, sizeof(t.fullName));
        file.write(t.position, sizeof(t.position));
        file.write(t.academicDegree, sizeof(t.academicDegree));
        file.write(reinterpret_cast<const char*>(&t.departmentCode), sizeof(t.departmentCode));
        file.write(t.departmentName, sizeof(t.departmentName));
        return file;
    }
};


size_t fileSize(std::ifstream& fin) {
    std::streamoff pos = fin.tellg();
    fin.seekg(0, std::ios_base::end);
    size_t n = fin.tellg();
    fin.seekg(pos);
    return n;
}

void TXTtoBIN(const std::string& txtFilename, const std::string& binFilename, bool isDepartment = false) {
    std::ifstream fin(txtFilename);
    std::ofstream fout(binFilename, std::ios::binary | std::ios::out);
    if (fin.is_open() && fout.is_open()) {
        std::string buffer;
        while (std::getline(fin, buffer)) {
            Teacher t;
            std::istringstream sin(buffer);
            std::vector<std::string> data;
            std::string d;
            while (std::getline(sin, d, '\t')) {
                data.push_back(d);
            }

            if (isDepartment) {
                t.departmentCode = std::stoi(data[0]);
                std::copy(data[1].begin(), data[1].end(), std::begin(t.departmentName));
                t.departmentName[std::min(data[1].size(), sizeof(t.departmentName) - 1)] = '\0';
            }
            else {
                t.personalNumber = std::stoi(data[0]);
                std::copy(data[1].begin(), data[1].end(), std::begin(t.fullName));
                t.fullName[std::min(data[1].size(), sizeof(t.fullName) - 1)] = '\0';

                std::copy(data[2].begin(), data[2].end(), std::begin(t.position));
                t.position[std::min(data[2].size(), sizeof(t.position) - 1)] = '\0';

                std::copy(data[3].begin(), data[3].end(), std::begin(t.academicDegree));
                t.academicDegree[std::min(data[3].size(), sizeof(t.academicDegree) - 1)] = '\0';

                t.departmentCode = std::stoi(data[4]);
            }

            fout << t;
        }
        fin.close();
        fout.close();
    }
    else {
        std::cerr << "Error: Cannot open file " << (fin.is_open() ? binFilename : txtFilename) << '\n';
    }

}

void CreateNewSortedFile() {
    std::ifstream finStuff("stuff.bin", std::ios::binary);
    size_t stuffSize = fileSize(finStuff) / sizeof(Teacher);
    std::vector<Teacher> teachers(stuffSize);

    if (finStuff.is_open()) {
        for (size_t i = 0; i < stuffSize; i++) {
            finStuff >> teachers[i];
        }
        finStuff.close();
    }
    else {
        std::cerr << "Error: Cannot open file stuff.bin\n";
        return;
    }

    std::sort(teachers.begin(), teachers.end(), [](const Teacher& a, const Teacher& b) {
        return a.departmentCode < b.departmentCode;
        });

    std::ifstream finDepartment("department.bin", std::ios::binary);
    size_t departmentSize = fileSize(finDepartment) / sizeof(Teacher);
    std::vector<Teacher> departments(departmentSize);

    if (finDepartment.is_open()) {
        for (size_t i = 0; i < departmentSize; i++) {
            finDepartment >> departments[i];
        }
        finDepartment.close();
    }
    else {
        std::cerr << "Error: Cannot open file department.bin\n";
        return;
    }

    std::sort(departments.begin(), departments.end(), [](const Teacher& a, const Teacher& b) {
        return a.departmentCode < b.departmentCode;
        });

    std::ofstream fout("M1.bin", std::ios::binary);

    size_t i = 0, j = 0;
    while (i < stuffSize && j < departmentSize) {
        if (teachers[i].departmentCode == departments[j].departmentCode) {
            std::string departmentNameStr = departments[j].departmentName;
            std::copy(departmentNameStr.begin(), departmentNameStr.end(), std::begin(teachers[i].departmentName));
            teachers[i].departmentName[departmentNameStr.size()] = '\0';
            fout << teachers[i];
            ++i;
        }
        else if (teachers[i].departmentCode < departments[j].departmentCode) {
            ++i;
        }
        else {
            ++j;
        }
    }


    fout.close();
}

void solution() {
    std::ifstream m("M1.bin", std::ios::binary);
    std::vector<Teacher> teachers(fileSize(m) / sizeof(Teacher));

    if (m.is_open()) {
        for (size_t i = 0; i < teachers.size(); i++) {
            m >> teachers[i];
        }
        m.close();
    }
    else {
        std::cerr << "Error: Cannot open file M1.bin\n";
        return;
    }

    int move = -1;
    while (move != 0) {
        std::cout << "Меню:\n"
            << "1. Найти всех сотрудников, имеющих заданную должность.\n"
            << "2. Найти количество сотрудников каждой должности.\n"
            << "3. Показать всех докторов.\n"
            << "4. Отсортировать записи в алфавитном порядке по ФИО.\n"
            << "5. Отобразить список профессоров, не являющихся докторами.\n"
            << "6. Перевести ассистента на должность доцента, если ассистентов больше, чем профессоров.\n"
            << "0. Выход.\n\n";

        std::cin >> move;
        std::cin.ignore();  // Игнорируем оставшийся '\n' после std::cin
        std::cout << '\n';
        switch (move) {
        case 0:
            break;
        case 1: {
            std::string position;
            std::cout << "Введите должность: ";
            std::getline(std::cin, position);

            for (const auto& t : teachers) {
                if (position == t.position) {
                    std::cout << t.fullName << ", " << t.position << ", " << t.academicDegree << ", " << t.departmentName << '\n';
                }
            }
            std::cout << '\n';
            break;
        }
        case 2: {
            std::map<std::string, int> positionCount;
            for (const auto& t : teachers) {
                ++positionCount[t.position];
            }
            for (const auto& p : positionCount) {
                std::cout << p.first << ": " << p.second << '\n';
            }
            std::cout << '\n';
            break;
        }
        case 3: {
            for (const auto& t : teachers) {
                if (std::string(t.academicDegree) == "Доктор") {
                    std::cout << t.fullName << '\n';
                }
            }
            std::cout << '\n';
            break;
        }
        case 4: {
            std::sort(teachers.begin(), teachers.end(), [](const Teacher& a, const Teacher& b) {
                return std::strcmp(a.fullName, b.fullName) < 0;
                });
            std::cout << "Записи отсортированы.\n\n";
            break;
        }
        case 5: {
            for (const auto& t : teachers) {
                if (std::string(t.position) == "Профессор" && std::string(t.academicDegree) != "Доктор") {
                    std::cout << t.fullName << '\n';
                }
            }
            std::cout << '\n';
            break;
        }
        case 6: {
            int assistantCount = std::count_if(teachers.begin(), teachers.end(), [](const Teacher& t) {
                return std::string(t.position) == "Ассистент";
                });
            int professorCount = std::count_if(teachers.begin(), teachers.end(), [](const Teacher& t) {
                return std::string(t.position) == "Профессор";
                });

            if (assistantCount > professorCount) {
                for (auto& t : teachers) {
                    if (std::string(t.position) == "Ассистент") {
                        std::string newPosition = "Доцент";
                        std::copy(newPosition.begin(), newPosition.end(), std::begin(t.position));
                        t.position[newPosition.size()] = '\0';
                        break;
                    }
                }
                std::cout << "Ассистент переведен на должность доцента.\n\n";
            }
            else {
                std::cout << "Количество ассистентов не превышает количество профессоров.\n\n";
            }
            break;
        }

        default:
            std::cout << "Неверный выбор. Попробуйте снова.\n\n";
            break;
        }
    }
}

int main() {
    solution();
    return 0;
}
