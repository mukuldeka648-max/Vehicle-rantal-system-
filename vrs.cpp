#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <conio.h>
#include <algorithm>

using namespace std;

#define RESET   "\033[0m"
#define BLUE_BG "\033[44m\033[37m"
#define YELLOW_BG "\033[43m\033[30m"
#define GREEN   "\033[32m"
#define RED    "\033[31m"
#define BOLD   "\033[1m"
#define CYAN   "\033[36m"

void drawBoundary() {
    cout << CYAN << " +-----------------------------------------------------------------------+" << RESET << endl;
}

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

string getPassword() {
    string pass = "";
    char ch;
    while ((ch = _getch()) != 13) {
        if (ch == 8) {
            if (pass.length() > 0) { cout << "\b \b"; pass.pop_back(); }
        } else { pass.push_back(ch); cout << "*"; }
    }
    cout << endl;
    return pass;
}

class Vehicle {
public:
    string plateNumber; 
    string name; 
    string type; 
    float rent; 
    bool available;
    string providerName; 
    string renterName;   

    Vehicle(string pn, string n, string t, float r, string pName, bool a = true, string rName = "NONE") 
        : plateNumber(pn), name(n), type(t), rent(r), available(a), providerName(pName), renterName(rName) {}

    virtual ~Vehicle() {}

    void displayRow() {
        cout << CYAN << " | " << RESET << left << setw(12) << plateNumber << setw(12) << name 
             << setw(10) << type << setw(8) << rent 
             << (available ? GREEN : RED) << setw(10) << (available ? "GARAGE" : "RENTED") 
             << RESET << CYAN << " |" << RESET << endl;
    }
};

class Car : public Vehicle {
public:
    Car(string pn, string n, float r, string pName, bool a = true, string rName = "NONE")
        : Vehicle(pn, n, "Car", r, pName, a, rName) {}
};

class Bike : public Vehicle {
public:
    Bike(string pn, string n, float r, string pName, bool a = true, string rName = "NONE")
        : Vehicle(pn, n, "Bike", r, pName, a, rName) {}
};

class Scooty : public Vehicle {
public:
    Scooty(string pn, string n, float r, string pName, bool a = true, string rName = "NONE")
        : Vehicle(pn, n, "Scooty", r, pName, a, rName) {}
};

class User {
public:
    string username, password, role;
    User() {}
    User(string u, string p, string r) : username(u), password(p), role(r) {}
};

class RentalSystem {
public:
    vector<Vehicle> vehicles;
    vector<User> users;
    User currentUser;

    RentalSystem() { loadData(); }

    void loadData() {
        vehicles.clear();
        ifstream vfile("vehicles.txt");
        string pn, name, type, pName, rName; float rent; int avail;
        while (vfile >> pn >> name >> type >> rent >> avail >> pName >> rName) {
            if (toLower(type) == "car") 
                vehicles.push_back(Car(pn, name, rent, pName, (avail == 1), rName));
            else if (toLower(type) == "bike") 
                vehicles.push_back(Bike(pn, name, rent, pName, (avail == 1), rName));
            else if (toLower(type) == "scooty") 
                vehicles.push_back(Scooty(pn, name, rent, pName, (avail == 1), rName));
            else 
                vehicles.push_back(Vehicle(pn, name, type, rent, pName, (avail == 1), rName));
        }
        vfile.close();

        users.clear();
        ifstream ufile("users.txt");
        string uname, pass, role;
        while (ufile >> uname >> pass >> role)
            users.push_back(User(uname, pass, role));
        ufile.close();
    }

    void saveVehicles() {
        ofstream vfile("vehicles.txt");
        for (auto &v : vehicles)
            vfile << v.plateNumber << " " << v.name << " " << v.type << " " << v.rent << " " 
                  << (v.available ? 1 : 0) << " " << v.providerName << " " << v.renterName << endl;
    }

    void saveRentalLog(string uname, string pn, int days, float total, string phone, string add) {
        ofstream rfile("rentals_log.txt", ios::app);
        rfile << "User: " << uname << " | Plate: " << pn << " | Days: " << days 
              << " | Total: " << total << " | Phone: " << phone << " | Address: " << add << endl;
    }

    void rentVehicle() {
        system("cls");
        showVehicles();
        string pn, phone, add; int days;
        drawBoundary();
        cout << CYAN << " | " << RESET << "Enter Plate Number to Rent: "; cin >> pn;
        
        for (auto &v : vehicles) {
            if (v.plateNumber == pn) {
                if (!v.available) {
                    cout << CYAN << " | " << RED << "Error: Already Rented!" << RESET << endl;
                    drawBoundary(); system("pause"); return;
                }
                cout << CYAN << " | " << RESET << "Enter Rental Days: "; cin >> days;
                cout << CYAN << " | " << RESET << "Enter Drop Address: "; cin >> add;
                cout << CYAN << " | " << RESET << "Enter Phone Number: "; cin >> phone;
                
                v.available = false;
                v.renterName = currentUser.username;
                saveVehicles();
                saveRentalLog(currentUser.username, v.plateNumber, days, (days * v.rent), phone, add);
                
                cout << CYAN << " | " << GREEN << "Success! Total: " << (days * v.rent) << RESET << endl;
                drawBoundary(); system("pause"); return;
            }
        }
        cout << CYAN << " | " << RED << "Error: Plate Number Not Found!" << RESET << endl;
        drawBoundary(); system("pause");
    }

    void returnVehicle() {
        system("cls");
        drawBoundary();
        cout << CYAN << " | " << BOLD << "RETURN VEHICLE" << RESET << endl;
        string pn, pickup, phone;
        cout << CYAN << " | " << RESET << "Enter Plate Number: "; cin >> pn;

        for (auto &v : vehicles) {
            if (v.plateNumber == pn) {
                if (v.available) {
                    cout << CYAN << " | " << RED << "Error: This vehicle is already in garage!" << RESET << endl;
                } else if (v.renterName != currentUser.username) {
                    cout << CYAN << " | " << RED << "Error: You didn't rent this vehicle!" << RESET << endl;
                } else {
                    cout << CYAN << " | " << RESET << "Enter Pickup Location: "; cin >> pickup;
                    cout << CYAN << " | " << RESET << "Enter Phone No: "; cin >> phone;
                    v.available = true;
                    v.renterName = "NONE";
                    saveVehicles();
                    cout << CYAN << " | " << GREEN << "Success! Vehicle returned from " << pickup << RESET << endl;
                }
                drawBoundary(); system("pause"); return;
            }
        }
        cout << CYAN << " | " << RED << "Error: Plate not found!" << RESET << endl;
        drawBoundary(); system("pause");
    }

    void addVehicle() {
        system("cls"); drawBoundary();
        string pn, name, type, addr, ph; float rent;
        cout << CYAN << " | " << BOLD << "ADD VEHICLE (Provider: " << currentUser.username << ")" << RESET << endl;
        cout << CYAN << " | " << RESET << "Plate Number: "; cin >> pn;
        cout << CYAN << " | " << RESET << "Model Name: "; cin >> name;
        cout << CYAN << " | " << RESET << "Type (Car/Bike/Scooty): "; cin >> type;
        cout << CYAN << " | " << RESET << "Daily Rent: "; cin >> rent;
        cout << CYAN << " | " << RESET << "Your Address: "; cin >> addr;
        cout << CYAN << " | " << RESET << "Your Phone: "; cin >> ph;

        if (toLower(type) == "car") vehicles.push_back(Car(pn, name, rent, currentUser.username));
        else if (toLower(type) == "bike") vehicles.push_back(Bike(pn, name, rent, currentUser.username));
        else if (toLower(type) == "scooty") vehicles.push_back(Scooty(pn, name, rent, currentUser.username));
        else vehicles.push_back(Vehicle(pn, name, type, rent, currentUser.username));

        saveVehicles();
        cout << CYAN << " | " << GREEN << "Vehicle registered to your profile!" << RESET << endl;
        drawBoundary(); system("pause");
    }

    void removeVehicle() {
        system("cls");
        showVehicles();
        string pn, addr, ph;
        drawBoundary();
        cout << CYAN << " | " << RESET << "Enter Plate Number to remove: "; cin >> pn;
        for (auto it = vehicles.begin(); it != vehicles.end(); ++it) {
            if (it->plateNumber == pn) {
                if (it->providerName != currentUser.username) {
                    cout << CYAN << " | " << RED << "Error: You do not own this vehicle!" << RESET << endl;
                } else {
                    cout << CYAN << " | " << RESET << "Enter Your Address to Confirm: "; cin >> addr;
                    cout << CYAN << " | " << RESET << "Enter Your Phone to Confirm: "; cin >> ph;
                    vehicles.erase(it);
                    saveVehicles();
                    cout << CYAN << " | " << GREEN << "Vehicle removed successfully." << RESET << endl;
                }
                drawBoundary(); system("pause"); return;
            }
        }
        cout << CYAN << " | " << RED << "Error: Not found!" << RESET << endl;
        drawBoundary(); system("pause");
    }

    void showRentalHistory() {
        system("cls");
        string searchId;
        drawBoundary();
        cout << CYAN << " | " << BOLD << " RENTAL HISTORY SEARCH " << RESET << endl;
        cout << CYAN << " | " << RESET << "Enter Vehicle Plate: "; cin >> searchId; 

        bool existsInSystem = false; 
        bool currentlyInGarage = true;

        for (auto &v : vehicles) { 
            if (v.plateNumber == searchId) { 
                existsInSystem = true; 
                currentlyInGarage = v.available; 
                break; 
            } 
        }

        if (!existsInSystem) {
            cout << CYAN << " | " << RED << "Error: Vehicle not found !" << RESET << endl;
            drawBoundary(); system("pause"); return;
        }

        ifstream file("rentals_log.txt");
        string line, lastMatch = "";

        if (!file) {
            cout << CYAN << " | " << RED << "Error: Log file missing!" << RESET << endl;
        } else {
            while (getline(file, line)) {
                if (line.find(searchId) != string::npos) {
                    lastMatch = line;
                }
            }
            file.close();

            if (currentlyInGarage) cout << CYAN << " | " << GREEN << "Status: In Garage (Available)" << RESET << endl;
            else cout << CYAN << " | " << RED << "Status: Currently Rented" << RESET << endl;

            if (lastMatch == "") {
                cout << CYAN << " | " << RED << "No rental records found in rentals_log.txt" << RESET << endl;
            } else {
                cout << CYAN << " | " << BOLD << "LATEST RENTER DETAILS:" << RESET << endl;
                cout << CYAN << " | " << YELLOW_BG << " " << lastMatch << " " << RESET << endl;
            }
        }
        drawBoundary(); 
        system("pause");
    }

    void showVehicles() {
        string opts[] = {"Bike", "Car", "Scooty", "All"};
        int sel = 0;
        system("cls"); 
        while(true) {
            cout << "\033[H"; 
            drawBoundary();
            cout << CYAN << " | " << BOLD << "FILTER BY TYPE" << RESET << endl;
            for(int i=0; i<4; i++) {
                cout << CYAN << " | " << RESET;
                if(i == sel) cout << YELLOW_BG << " ( " << opts[i] << " ) " << RESET << endl;
                else cout << BLUE_BG << " ( " << opts[i] << " ) " << RESET << endl;
            }
            drawBoundary();
            cout << "\033[J"; 
            int k = _getch();
            if(k == 224) { k=_getch(); if(k==72) sel--; else if(k==80) sel++; if(sel<0) sel=3; if(sel>3) sel=0; }
            else if(k == 13) break;
        }
        string filter = (sel == 3) ? "" : toLower(opts[sel]);
        system("cls"); drawBoundary();
        cout << CYAN << " | " << BOLD << left << setw(12) << "PLATE" << setw(12) << "NAME" 
             << setw(10) << "TYPE" << setw(8) << "RENT" << setw(10) << "STATUS" << RESET << CYAN << " |" << RESET << endl;
        for (auto &v : vehicles) { if (filter == "" || toLower(v.type) == filter) v.displayRow(); }
        drawBoundary();
    }

    void signup(string role) {
        system("cls"); drawBoundary();
        string uname, pass;
        cout << CYAN << " | " << BOLD << "SIGN UP (" << role << ")" << RESET << endl;
        cout << CYAN << " | " << RESET << "Username: "; cin >> uname;
        cout << CYAN << " | " << RESET << "Password: "; pass = getPassword();
        users.push_back(User(uname, pass, role));
        ofstream ufile("users.txt", ios::app);
        ufile << uname << " " << pass << " " << role << "\n";
        cout << CYAN << " | " << GREEN << "Signup Successful!" << RESET << endl;
        drawBoundary(); system("pause");
    }

    bool login(string role) {
        system("cls"); drawBoundary();
        string uname, pass;
        cout << CYAN << " | " << BOLD << "LOGIN (" << role << ")" << RESET << endl;
        cout << CYAN << " | " << RESET << "Username: "; cin >> uname;
        cout << CYAN << " | " << RESET << "Password: "; pass = getPassword();
        for (auto &u : users) {
            if (u.username == uname && u.password == pass && u.role == role) {
                currentUser = u; return true;
            }
        }
        cout << CYAN << " | " << RED << "Invalid Credentials!" << RESET << endl;
        drawBoundary(); system("pause"); return false;
    }

    int menu() {
        int choice = 0;
        string uM[] = {"View Vehicles", "Rent Vehicle", "Return Vehicle", "Logout"};
        string pM[] = {"View Vehicles", "Add Vehicle", "Remove Vehicle", "Vehicles Status", "Logout"};
        int size = (currentUser.role == "user") ? 4 : 5;
        system("cls"); 
        while (true) {
            cout << "\033[H"; 
            drawBoundary();
            cout << CYAN << " | " << BOLD << " WELCOME, " << currentUser.username << " (" << currentUser.role << ")" << RESET << endl;
            for (int i = 0; i < size; i++) {
                cout << CYAN << " | " << RESET;
                string lbl = (currentUser.role == "user") ? uM[i] : pM[i];
                if (i == choice) cout << YELLOW_BG << " ( " << lbl << " ) " << RESET << endl;
                else cout << BLUE_BG << " ( " << lbl << " ) " << RESET << endl;
            }
            drawBoundary();
            cout << "\033[J"; 
            int k = _getch();
            if (k == 224) { k = _getch(); if (k == 72) choice--;
                 else if (k == 80) choice++; if (choice < 0) choice = size - 1; if (choice >= size) choice = 0; }
            else if (k == 13) return choice;
        }
    }

    void mainMenu() {
        while (true) {
            int ch = menu();
            if (currentUser.role == "user") {
                if (ch == 0) { showVehicles(); system("pause"); }
                else if (ch == 1) rentVehicle();
                else if (ch == 2) returnVehicle();
                else break;
            } else {
                if (ch == 0) { showVehicles(); system("pause"); }
                else if (ch == 1) addVehicle();
                else if (ch == 2) removeVehicle();
                else if (ch == 3) showRentalHistory();
                else break;
            }
        }
    }
};

int startMenu(int &roleChoice) {
    int option = 0; roleChoice = 0;
    string roles[] = {"USER", "PROVIDER"};
    string options[] = {"Login", "Sign Up", "Exit"};
    system("cls"); 
    while (true) {
        cout << "\033[H"; 
        drawBoundary();
        cout << CYAN << " | " << BOLD << " VEHICLE RENTAL SYSTEM " << RESET << endl;
        cout << CYAN << " | " << RESET << "Select Role: ";
        for (int i = 0; i < 2; i++) {
            if (i == roleChoice) cout << YELLOW_BG << " " << roles[i] << " " << RESET << " ";
            else cout << BLUE_BG << " " << roles[i] << " " << RESET << " ";
        }
        cout << "\n" << CYAN << " | " << RESET << endl;
        for (int i = 0; i < 3; i++) {
            cout << CYAN << " | " << RESET;
            if (i == option) cout << YELLOW_BG << " ( " << options[i] << " ) " << RESET << endl;
            else cout << BLUE_BG << " ( " << options[i] << " ) " << RESET << endl;
        }
        drawBoundary();
        cout << "\033[J"; 
        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 75) roleChoice--; else if (key == 77) roleChoice++;
            if (key == 72) option--; else if (key == 80) option++;
            if (roleChoice < 0) roleChoice = 1; if (roleChoice > 1) roleChoice = 0;
            if (option < 0) option = 2; if (option > 2) option = 0;
        } else if (key == 13) return option;
    }
}

int main() {
    RentalSystem rs;
    while (true) {
        int roleChoice;
        int action = startMenu(roleChoice);
        string role = (roleChoice == 0) ? "user" : "provider";
        if (action == 0) { if (rs.login(role)) rs.mainMenu(); }
        else if (action == 1) { rs.signup(role); }
        else { break; }
    }
    std::system("cls");
    return 0;
}