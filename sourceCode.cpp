#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <map>
#include <algorithm>
#include <random>
#include <sstream>
using namespace std;

map<string, vector<pair<string, float>>> examResults;
void exportExamGrades(const string& code);



class Question {
    public:
    string text;
    float positiveMark;
    float negativeMark;

    Question(string t, float pos, float neg)
    : text(t), positiveMark(pos), negativeMark(neg) {}

    virtual void ask() = 0;
    virtual float grade(string answer) = 0;
    virtual string getType() = 0;
    virtual void save(ofstream& out) = 0;
    virtual void load(ifstream& in) = 0;
    virtual ~Question() {}
};

class MultipleChoiceQuestion : public Question {
    public:
    vector<string> options;
    int correctOptionIndex;
    vector<int> shuffledIndices;

    MultipleChoiceQuestion(string t, float pos, float neg, vector<string> opts, int correctIdx)
    : Question(t, pos, neg), options(opts), correctOptionIndex(correctIdx) {}

    MultipleChoiceQuestion() : Question("", 0, 0), correctOptionIndex(0) {}

    void ask() override {
        cout << text << "\n";


        vector<int> indices = {0, 1, 2, 3};

        mt19937 rng{random_device{}()};
        shuffle(indices.begin(), indices.end(), rng);


        shuffledIndices = indices;

        for (int i = 0; i < 4; i++) {
            cout << i + 1 << ". " << options[indices[i]] << "\n";
        }
    }

    float grade(string answer) override {
        int userChoice = stoi(answer) - 1;

        if (userChoice < 0 || userChoice >= 4) return -negativeMark;


        int originalIndex = shuffledIndices[userChoice];

        if (originalIndex == correctOptionIndex)
        return positiveMark;
        else
        return -negativeMark;
    }

    string getType() override { return "MCQ"; }

    void save(ofstream& out) override {
        out << getType() << "\n" << text << "\n" << positiveMark << "\n" << negativeMark << "\n";
        for (int i = 0; i < 4; i++) out << options[i] << "\n";
        out << correctOptionIndex << "\n";
    }

    void load(ifstream& in) override {
        getline(in, text);
        in >> positiveMark >> negativeMark;
        in.ignore();
        options.resize(4);
        for (int i = 0; i < 4; i++) getline(in, options[i]);
        in >> correctOptionIndex;
        in.ignore();
    }
};


class ShortAnswerQuestion : public Question {
    public:
    string correctAnswer;

    ShortAnswerQuestion(string t, float pos, float neg, string corrAns)
    : Question(t, pos, neg), correctAnswer(corrAns) {}

    ShortAnswerQuestion() : Question("", 0, 0), correctAnswer("") {}

    void ask() override {
        cout << text << "\n";
    }

    float grade(string answer) override {
        return (answer == correctAnswer) ? positiveMark : -negativeMark;
    }

    string getType() override { return "SA"; }

    void save(ofstream& out) override {
        out << getType() << "\n" << text << "\n" << positiveMark << "\n" << negativeMark << "\n" << correctAnswer << "\n";
    }

    void load(ifstream& in) override {
        getline(in, text);
        in >> positiveMark >> negativeMark;
        in.ignore();
        getline(in, correctAnswer);
    }
};

class DescriptiveQuestion : public Question {
    public:
    string correctAnswer;
    string studentAnswer;

    DescriptiveQuestion(string t, float pos, string corrAns)
    : Question(t, pos, 0), correctAnswer(corrAns) {}

    DescriptiveQuestion() : Question("", 0, 0), correctAnswer("") {}

    void ask() override {
        cout << text << "\n";
    }

    float grade(string answer) override {
        studentAnswer = answer;
        return 0;
    }

    string getStudentAnswer() const {
        return studentAnswer;
    }

    string getType() override { return "DESC"; }

    void save(ofstream& out) override {
        out << getType() << "\n" << text << "\n" << positiveMark << "\n" << correctAnswer << "\n";
    }

    void load(ifstream& in) override {
        getline(in, text);
        in >> positiveMark;
        in.ignore();
        getline(in, correctAnswer);
    }
};

class User {
    public:
    string name;
    string id;
    string password;

    User(string name, string id, string password)
    : name(name), id(id), password(password) {}

    virtual void displayMenu() = 0;
};

class Teacher : public User {
    public:
    vector<pair<string, vector<Question*>>> exams;

    vector<string> courses;

    Teacher(string name, string id, string password, vector<string> courses)
    : User(name, id, password), courses(courses) {}

    void displayMenu() override {
        int choice;
        do {
            cout << "\nmenu-ye ostad:\n";
            cout << "1. ejad azmon\n";
            cout << "2. moshahede-ye azmonha\n";
            cout << "3. daryaft liste nomarat\n";
            cout << "4. khoroj az hesab\n";
            cout << "entekhab konid: ";
            cin >> choice;

            switch (choice) {
                case 1:
                createExam();
                break;
                case 2:
                viewExams();
                break;
                case 3: {
                    string code;
                    cout << "Code azmon baraye daryaft liste nomarat: ";
                    cin >> code;
                    exportExamGrades(code);
                    break;
                }
            }
        } while (choice != 4);
    }


    void createExam() {
        string code;
        cout << "Code yektaye azmon ra vared konid: ";
        cin >> code;
        cin.ignore();

        vector<Question*> questions;
        int choice;
        do {
            cout << "\n1. Soal jadid\n2. Etemam sakhte azmon\nEntekhab: ";
            cin >> choice;
            cin.ignore();

            if (choice == 1) {
                int type;
                cout << "Noe soal (1. MCQ - 2. ShortAnswer - 3. Descriptive): ";
                cin >> type;
                cin.ignore();

                string text;
                float pos, neg = 0;

                cout << "Soal: ";
                getline(cin, text);

                cout << "Nomre mosbat: ";
                cin >> pos;
                cin.ignore();

                if (type != 3) {
                    cout << "Nomre manfi: ";
                    cin >> neg;
                    cin.ignore();
                }

                if (type == 1) {
                    vector<string> opts(4);
                    for (int i = 0; i < 4; ++i) {
                        cout << "Gozine " << i + 1 << ": ";
                        getline(cin, opts[i]);
                    }
                    int correct;
                    cout << "Shomare gozine dorost (1-4): ";
                    cin >> correct;
                    cin.ignore();

                    questions.push_back(new MultipleChoiceQuestion(text, pos, neg, opts, correct - 1));

                } else if (type == 2) {
                    string ans;
                    while (true) {
                        cout << "Javab dorost (yek kalame ya adad): ";
                        getline(cin, ans);


                        if (ans.find(' ') != string::npos) {
                            cout << "Javab faghat bayad yek kalame ya adad bashad. Lotfan dobare vared konid.\n";
                        } else {
                            break;
                        }
                    }

                    questions.push_back(new ShortAnswerQuestion(text, pos, neg, ans));


                } else if (type == 3) {
                    string correctAns;
                    cout << "Javab pishnahadi (baraye rahnama ya tas'hih): ";
                    getline(cin, correctAns);
                    questions.push_back(new DescriptiveQuestion(text, pos, correctAns));
                }


            }
        } while (choice != 2);

        exams.push_back({code, questions});
        cout << "Azmon sakhte shod.\n";
    }


    void viewExams() {
        if (exams.empty()) {
            cout << "hich azmoni vojod nadarad.\n";
            return;
        }

        cout << "azmonha:\n";
        for (const auto& exam : exams) {
            cout << "- code: " << exam.first << "\n";
        }

        string selectedExamCode;
        cout << "\ncode-ye azmon ra vared konid (ya '0' baraye bazgasht): ";
        cin >> selectedExamCode;

        if (selectedExamCode == "0") return;

        for (const auto& exam : exams) {
            if (exam.first == selectedExamCode) {
                cout << "\nsoalat azmon " << selectedExamCode << ":\n";
                for (Question* q : exam.second) {
                    q->ask();
                    cout << "----------------\n";
                }
                int subChoice;
                cout << "\n1. bazgasht\n2. export nomarat be file\nentekhab: ";
                cin >> subChoice;
                if (subChoice == 2) {
                    exportExamGrades(selectedExamCode);
                }

                return;
            }
        }

        cout << "azmon yaft nashod.\n";
    }


};

class Student : public User {
    public:
    vector<string> registeredExams;
    vector<Teacher*>& teachers;
    string major;

    Student(string name, string id, string password, vector<Teacher*>& teachers, string major)
    : User(name, id, password), teachers(teachers), major(major) {}

    void displayMenu() override {
        int choice;
        do {
            cout << "\nmenu-ye danesh-amooz:\n";
            cout << "1. sabt-nam dar azmon\n";
            cout << "2. moshahede azmon-ha\n";
            cout << "3. sherkat dar azmon\n";
            cout << "4. moshahede-ye karname\n";
            cout << "5. khoroj az hesab\n";

            cout << "entekhab konid: ";
            cin >> choice;

            switch (choice) {
                case 1:
                addExam();
                break;
                case 2:
                viewRegisteredExams();
                break;
                case 3:
                takeExam();
                break;
                case 4:
                string code;
                cout << "Code azmon baraye karname: ";
                cin >> code;
                generateReportCard(code);
                break;

            }
        } while (choice != 5);
    }

    void addExam() {
        string examCode;
        cout << "code-ye azmon ra vared konid: ";
        cin >> examCode;

        for (const auto& exam : registeredExams)
        if (exam == examCode) {
            cout << "shoma ghablan sabt nam karde-id.\n";
            return;
        }

        for (Teacher* teacher : teachers)
        for (const auto& exam : teacher->exams)
        if (exam.first == examCode) {
            registeredExams.push_back(examCode);
            cout << "sabt-nam ba movafaghiyat anjam shod.\n";
            return;
        }

        cout << "azmon yaft nashod.\n";
    }

    void viewRegisteredExams() {
        if (registeredExams.empty()) {
            cout << "hich azmoni sabt nashode ast.\n";
            return;
        }
        cout << "azmonha:\n";
        for (const auto& exam : registeredExams)
        cout << "- " << exam << "\n";
    }

    void takeExam() {
        string code;
        cout << "Code azmon: ";
        cin >> code;
        cin.ignore();

        bool found = false;

        ofstream out_sheet("./sheets/sheet" + id + "_" + code + ".txt");

        for (Teacher* t : teachers) {
            for (auto& ex : t->exams) {
                if (ex.first == code) {
                    found = true;

                    float total = 0;
                    int questionCount = ex.second.size();
                    cout << "\nShoroo azmon: " << code << "\n";
                    float finalGrade = 0;

                    for (int i = 0; i < questionCount; ++i) {
                        cout << "Soal " << i + 1 << ":\n";
                        ex.second[i]->ask();
                        cout << "Javab: ";
                        string ans;
                        getline(cin, ans);
                        total += ex.second[i]->grade(ans);

                        out_sheet << "Soal " << i+1 << ": " << ex.second[i]->text << "\n";
                        if (ex.second[i]->getType() == "MCQ") {
                            MultipleChoiceQuestion* mq = dynamic_cast<MultipleChoiceQuestion*>(ex.second[i]);
                            out_sheet << "Noe: 4-gozine-i\n";
                            int user_choice=mq->shuffledIndices[std::stoi(ans)-1];
                            out_sheet << "Pasokh dorost: " << mq->options[mq->correctOptionIndex] << "\n";
                            out_sheet << "Pasokh shoma: " << mq->options[user_choice] << "\n";
                            out_sheet << "Vaziyat:"<<(mq->options[mq->correctOptionIndex] == mq->options[user_choice] ? "true" : "false") << std::endl;
                        } else if (ex.second[i]->getType() == "SA") {
                            ShortAnswerQuestion* sa = dynamic_cast<ShortAnswerQuestion*>(ex.second[i]);
                            out_sheet << "Noe: Kootah-pasokh\n";
                            out_sheet << "Pasokh dorost: " << sa->correctAnswer << "\n";
                            out_sheet << "Pasokh shoma: " << ans << "\n";
                            out_sheet << "Vaziyat:"<<(sa->correctAnswer == ans ? "true" : "false") << std::endl;

                        } else if (ex.second[i]->getType() == "DESC") {
                            DescriptiveQuestion* dq = dynamic_cast<DescriptiveQuestion*>(ex.second[i]);
                            out_sheet << "Noe: Tashrihi\n";
                            out_sheet << "Pasokh pishnahadi: " << dq->correctAnswer << "\n";
                            out_sheet << "Pasokh shoma: " << ans << "\n";
                            out_sheet << "Vaziyat: be dast-e ostad tas'hih mishavad.\n";
                        }


                        if (ex.second[i]->getType() == "DESC") {
                            DescriptiveQuestion* dq = dynamic_cast<DescriptiveQuestion*>(ex.second[i]);
                            ofstream descOut("desc_answ/desc_" + id + "_" + code + ".txt", ios::app);
                            if (descOut) {
                                descOut << "Soal " << i + 1 << ": " << dq->text << "\n";
                                descOut << "Javab daneshjoo: " << dq->getStudentAnswer() << "\n";
                                descOut << "--------------------------\n";
                                descOut.close();
                            }
                        }

                        cout << "\n-----------------------\n";
                    }

                    out_sheet.close();

                    finalGrade = total;
                    examResults[code].push_back({id, finalGrade});
                    logExamResult(id, code, finalGrade);


                    cout << "Azmon ba movafaghiyat anjam shod.\n";
                    break;
                }
            }
            if (found) break;
        }

        if (!found) {
            cout << "Azmon yaft nashod.\n";
        }
    }

    void logExamResult(const string& studentId, const string& examCode, float finalGrade) {
        ofstream out("grades_db.csv", ios::app);
        if (!out) {
            cerr << "Couldn't open grades file.\n";
            return;
        }

        out << examCode << "," <<studentId<< "," << finalGrade << "\n";
        out.close();
    }


    void generateReportCard(string code) {
        ofstream out("reports/report_" + id + "_" + code + ".txt");
        if (!out) {
            cout << "Error: couldn’t create report file.\n";
            return;
        }

        vector<Question*>* questions = nullptr;
        for (Teacher* t : teachers) {
            for (auto& ex : t->exams) {
                if (ex.first == code) {
                    questions = &ex.second;
                    break;
                }
            }
            if (questions) break;
        }

        if (!questions) {
            cout << "Azmon yaft nashod.\n";
            return;
        }


        float myScore = -1;
        for (auto& p : examResults[code]) {
            if (p.first == id) {
                myScore = p.second;
                break;
            }
        }

        if (myScore < 0) {
            cout << "Shoma hanuz dar in azmon sherkat nakarde-id.\n";
            return;
        }


        float totalPositive = 0;
        for (Question* q : *questions) {
            totalPositive += q->positiveMark;
        }


        float sum = 0, maxScore = 0;
        int rank = 1;
        for (auto& p : examResults[code]) {
            sum += p.second;
            if (p.second > maxScore) maxScore = p.second;
            if (p.second > myScore) rank++;
        }
        float avg = examResults[code].empty() ? 0 : sum / examResults[code].size();

        out << "Karname baraye danesh-amooz " << name << " (ID: " << id << ")\n";
        out << "Code azmon: " << code << "\n\n";
        out << "Nomre shoma: " << myScore << " az " << totalPositive << "\n";
        out << "Miyangin nomarat: " << avg << "\n";
        out << "Bishine nomre: " << maxScore << "\n";
        out << "Rotbe shoma: " << rank << " az " << examResults[code].size() << "\n\n";

        out << "Joz'iyat soalat:\n";
        out.close();


        ifstream src("sheets/sheet"+ id + "_" + code + ".txt");
        ofstream dest("reports/report_" + id + "_" + code + ".txt", ios::app);
        if (!src.is_open()) {
            cerr << "Error: could not open source file: " << endl;
            return;
        }
        if (!dest.is_open()) {
            cerr << "Error: could not open destination file: " << endl;
            return;
        }
        string line;
        while (getline(src, line)) {
            dest << line << "\n";
        }
        src.close();
        dest.close();



        cout << "Karname dar file «report_" << id << "_" << code << ".txt» zakhire shod.\n";
    }

};
extern vector<Student*> students;

void exportExamGrades(const string& code) {
    if (examResults.find(code) == examResults.end() || examResults[code].empty()) {
        cout << "Hich kas dar in azmon sherkat nakarde.\n";
        return;
    }

    vector<pair<string, float>> list = examResults[code];


    sort(list.begin(), list.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    ofstream out("grades_" + code + ".txt");
    if (!out) {
        cout << "Error: couldn’t create grades file.\n";
        return;
    }

    float maxScore = 0, sum = 0;

    out << "Liste Nomerat baraye azmon: " << code << "\n";
    out << "---------------------------\n";

    for (auto& entry : list) {
        string studentName = "";
        for (Student* s : students)
        if (s->id == entry.first)
        studentName = s->name;

        out << "Name: " << studentName << " | ID: " << entry.first
        << " | Nomre: " << entry.second << "\n";

        if (entry.second > maxScore) maxScore = entry.second;
        sum += entry.second;
    }

    float avg = list.empty() ? 0 : sum / list.size();

    out << "---------------------------\n";
    out << "Bishine nomre: " << maxScore << "\n";
    out << "Miyangin nomarat: " << avg << "\n";

    out.close();
    cout << "File «grades_" << code << ".txt» ba movafaghiyat sakhte shod.\n";
}


vector<Teacher*> teachers;
vector<Student*> students;

bool isIdUnique(const string& id) {
    for (const auto& teacher : teachers)
    if (teacher->id == id) return false;
    for (const auto& student : students)
    if (student->id == id) return false;
    return true;
}

void signup() {
    int type;
    cout << "1. ostad\n2. danesh-amooz\nentekhab konid: ";
    cin >> type;
    cin.ignore();

    string name, id, password, major;
    while (true) {
        cout << "ID ra vared konid: ";
        cin >> id;
        if (isIdUnique(id)) break;
        cout << "ID tekrari ast. dobare vared konid.\n";
    }

    cout << "nam ra vared konid: ";
    cin.ignore();
    getline(cin, name);
    cout << "ramz ra vared konid: ";
    cin >> password;

    if (type == 1) {
        int numCourses;
        cout << "tedad doroos ra vared konid: ";
        cin >> numCourses;
        vector<string> courses(numCourses);
        for (int i = 0; i < numCourses; i++) {
            cout << "name dars " << i + 1 << ": ";
            cin.ignore();
            getline(cin, courses[i]);
        }
        teachers.push_back(new Teacher(name, id, password, courses));
    } else {
        cout << "reshte tahsiliye shoma: ";
        cin.ignore();
        getline(cin, major);
        students.push_back(new Student(name, id, password, teachers, major));
    }

    cout << "sabt-nam ba movafaghiyat anjam shod.\n";
}

void login() {
    string id, password;
    cout << "ID ra vared konid: ";
    cin >> id;
    cout << "ramz ra vared konid: ";
    cin >> password;

    for (Teacher* teacher : teachers)
    if (teacher->id == id && teacher->password == password) {
        cout << "vorood movafagh!\n";
        teacher->displayMenu();
        return;
    }

    for (Student* student : students)
    if (student->id == id && student->password == password) {
        cout << "vorood movafagh!\n";
        student->displayMenu();
        return;
    }

    cout << "ID ya ramz eshtebah ast.\n";
}


void saveData(const vector<Teacher*>& teachers, const vector<Student*>& students) {
    ofstream out("data.txt");
    if (!out) {
        cout << "Error: couldn't open file to save.\n";
        return;
    }

    out << teachers.size() << "\n";
    for (auto t : teachers) {
        out << t->name << "\n" << t->id << "\n" << t->password << "\n";
        out << t->courses.size() << "\n";
        for (auto& c : t->courses)
        out << c << "\n";

        out << t->exams.size() << "\n";
        for (auto& exam : t->exams) {
            out << exam.first << "\n";
            out << exam.second.size() << "\n";
            for (auto* q : exam.second) {
                q->save(out);
            }
        }
    }

    out << students.size() << "\n";
    for (auto s : students) {
        out << s->name << "\n" << s->id << "\n" << s->password << "\n" << s->major << "\n";
        out << s->registeredExams.size() << "\n";
        for (auto& e : s->registeredExams)
        out << e << "\n";
    }

    out.close();
}



void loadData(vector<Teacher*>& teachers, vector<Student*>& students) {
    ifstream in("data.txt");
    if (!in) {
        cout << "No saved data found.\n";
        return;
    }

    int numTeachers;
    in >> numTeachers;
    in.ignore();

    for (int i = 0; i < numTeachers; ++i) {
        string name, id, password;
        getline(in, name);
        getline(in, id);
        getline(in, password);

        int numCourses;
        in >> numCourses;
        in.ignore();
        vector<string> courses(numCourses);
        for (int j = 0; j < numCourses; ++j)
        getline(in, courses[j]);

        Teacher* t = new Teacher(name, id, password, courses);

        int numExams;
        in >> numExams;
        in.ignore();
        for (int j = 0; j < numExams; ++j) {
            string code;
            getline(in, code);

            int numQuestions;
            in >> numQuestions;
            in.ignore();

            vector<pair<string, float>> questions;
            vector<Question*> qList;

            for (int k = 0; k < numQuestions; ++k) {
                string type;
                getline(in, type);
                Question* q = nullptr;

                if (type == "MCQ") q = new MultipleChoiceQuestion();
                else if (type == "SA") q = new ShortAnswerQuestion();
                else if (type == "DESC") q = new DescriptiveQuestion();

                if (q) {
                    q->load(in);
                    qList.push_back(q);
                }
            }
            t->exams.push_back({code, qList});
        }

        teachers.push_back(t);
    }

    int numStudents;
    in >> numStudents;
    in.ignore();
    for (int i = 0; i < numStudents; ++i) {
        string name, id, password, major;
        getline(in, name);
        getline(in, id);
        getline(in, password);
        getline(in, major);

        Student* s = new Student(name, id, password, teachers, major);

        int numRegs;
        in >> numRegs;
        in.ignore();
        for (int j = 0; j < numRegs; ++j) {
            string examCode;
            getline(in, examCode);
            s->registeredExams.push_back(examCode);
        }

        students.push_back(s);
    }

    in.close();
}


void freeMemory(vector<Teacher*>& teachers, vector<Student*>& students) {
    for (auto t : teachers) {
        for (auto& exam : t->exams) {
            for (Question* q : exam.second) {
                delete q;
            }
        }
        delete t;
    }

    for (auto s : students) {
        delete s;
    }

    teachers.clear();
    students.clear();
}


map<string, vector<pair<string, float>>> readExamResults(const string& filename) {
    map<string, vector<pair<string, float>>> examResults;
    ifstream inputFile(filename);
    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string courseID, studentID, gradeStr;
        float grade;

        if (getline(ss, courseID, ',') &&
        getline(ss, studentID, ',') &&
        getline(ss, gradeStr)) {

            try {
                grade = stof(gradeStr);
                examResults[courseID].emplace_back(studentID, grade);
            } catch (const invalid_argument& e) {
                cerr << "Invalid grade in line: " << line << endl;
            }
        }
    }

    inputFile.close();
    return examResults;
}




int main() {
    loadData(teachers, students);
    examResults=readExamResults("grades_db.csv");

    int choice;
    do {
        cout << "\n1. Signup\n2. Login\n3. Exit\nChoice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
            signup();
            break;
            case 2:
            login();
            break;
            case 3:
            saveData(teachers, students);
            break;
            default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 3);

    return 0;
}


