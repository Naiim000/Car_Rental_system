#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include<conio.h>

using namespace std;


class Car {
public:
    string model;
    string description;
    double pricePerDay;

    Car(string model, string description, double pricePerDay)
        : model(model), description(description), pricePerDay(pricePerDay) {}
};

class User {
protected:
    string username;
    string password;

public:
    User(string u, string p) : username(u), password(p) {}
    virtual void displayOptions() = 0;
    string getUsername() { return username; }
    bool authenticate(string p) { return p == password; }
    string getPassword() { return password; }
    void setPassword(string newPassword) { password = newPassword; }
};

class Customer : public User {
private:
    vector<string> rentalHistory; // Stores customer's rental history

public:
    Customer(string u, string p) : User(u, p) {}

    void displayOptions() override {
        cout << "Customer: " << username << endl;
        cout << "1. Rent a car" << endl;
        cout << "2. View rental history" << endl;
        cout << "3. Search available cars" << endl;
        cout << "4. Add review" << endl;
        cout << "5. view review" << endl;
        cout << "6. Update profile" << endl;
        cout << "7. Exit" << endl;
    }

    void rentCar(vector<Car>& cars) {
        cout << "\nAvailable cars:\n";
        for (int i = 0; i < cars.size(); i++) {
            cout << i + 1 << ". Model: " << cars[i].model << ", Description: "
                 << cars[i].description << ", Price/Day: $" << cars[i].pricePerDay << endl;
        }

        int choice;
        cout << "\nEnter the number of the car you want to rent: ";
        cin >> choice;
        if (choice < 1 || choice > cars.size()) {
            cout << "Invalid choice!" << endl;
            return;
        }

        int days;
        cout << "Enter the number of days you want to rent the car: ";
        cin >> days;

        double totalCost = cars[choice - 1].pricePerDay * days;
        cout << "\n--- Customer Invoice ---\n";
        cout << "Customer Name: " << username << endl;
        cout << "Car Model: " << cars[choice - 1].model << endl;
        cout << "Number of Days: " << days << endl;
        cout << "Total Rental Amount: $" << totalCost << endl;
        cout << "------------------------\n";

        // Record rental in history
        rentalHistory.push_back("Rented " + cars[choice - 1].model + " for " + to_string(days) + " days. Total Cost: $" + to_string(totalCost));
    }

    void viewRentalHistory() {
        cout << "\n--- Rental History ---\n";
        if (rentalHistory.empty()) {
            cout << "No rental history available.\n";
        } else {
            for (const auto& record : rentalHistory) {
                cout << record << endl;
            }
        }
        cout << "----------------------\n";
    }


    //;;;;;;;;;;;;;;;;;;;;;;;;;





    void updateProfile() {
        string newPassword;
        cout << "Enter a new password: ";
        cin >> newPassword;
        setPassword(newPassword);
        cout << "Password updated successfully!\n";
    }
};

class Admin : public User {
public:
    Admin(string u, string p) : User(u, p) {}

    void displayOptions() override {
        cout << "Admin: " << username << endl;
        cout << "1. Add a new car" << endl;
        cout << "2. Remove a car" << endl;
        cout << "3. Change car price" << endl;
        cout << "4. View all rentals" << endl;
        cout << "5. Update profile" << endl;
        cout << "6. Exit" << endl;
    }

    void addCar(vector<Car>& cars) {
        string model, description;
        double pricePerDay;
        cout << "Enter car model: ";
        cin.ignore();
        getline(cin, model);
        cout << "Enter car description: ";
        getline(cin, description);
        cout << "Enter car price per day: ";
        cin >> pricePerDay;

        cars.push_back(Car(model, description, pricePerDay));
        cout << "Car added successfully!\n";

        // Save updated cars to file
        saveCarsToFile(cars);
    }

    void removeCar(vector<Car>& cars) {
        cout << "Available cars:\n";
        for (int i = 0; i < cars.size(); i++) {
            cout << i + 1 << ". " << cars[i].model << endl;
        }

        int choice;
        cout << "Enter the number of the car to remove: ";
        cin >> choice;
        if (choice < 1 || choice > cars.size()) {
            cout << "Invalid choice!" << endl;
            return;
        }

        cars.erase(cars.begin() + choice - 1);
        cout << "Car removed successfully!\n";

        // Save updated cars to file
        saveCarsToFile(cars);
    }

    void changeCarPrice(vector<Car>& cars) {
        cout << "Available cars:\n";
        for (int i = 0; i < cars.size(); i++) {
            cout << i + 1 << ". " << cars[i].model << " ($" << cars[i].pricePerDay << "/day)\n";
        }

        int choice;
        cout << "Enter the number of the car to change the price: ";
        cin >> choice;
        if (choice < 1 || choice > cars.size()) {
            cout << "Invalid choice!" << endl;
            return;
        }

        double newPrice;
        cout << "Enter the new price per day: ";
        cin >> newPrice;
        cars[choice - 1].pricePerDay = newPrice;
        cout << "Car price updated successfully!\n";

        // Save updated cars to file
        saveCarsToFile(cars);
    }

    void viewAllRentals(const map<string, unique_ptr<Customer>>& customers) {
        cout << "\n--- All Customer Rentals ---\n";
        for (const auto& pair : customers) {
            const auto& customer = pair.second;
            cout << "Customer: " << customer->getUsername() << endl;
            customer->viewRentalHistory();
            cout << "----------------------------\n";
        }
    }

    void updateProfile() {
        string newPassword;
        cout << "Enter a new password: ";
        cin >> newPassword;
        setPassword(newPassword);
        cout << "Password updated successfully!\n";
    }

    void saveCarsToFile(const vector<Car>& cars) {
        ofstream outFile("cars.txt");
        if (outFile.is_open()) {
            for (const auto& car : cars) {
                outFile << car.model << "|" << car.description << "|" << car.pricePerDay << "\n";
            }
            outFile.close();
            //cout << "Cars data saved successfully!\n";
        } else {
            cout << "Error opening file for writing.\n";
        }
    }
};

class CarRentalSystem {
private:
    vector<Car> cars;
    map<string, unique_ptr<Customer>> customers;
    map<string, unique_ptr<Admin>> admins;
    string signedInUser;

    void loadCarsFromFile() {
        ifstream inFile("cars.txt");
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos1 = line.find('|');
                size_t pos2 = line.rfind('|');
                if (pos1 != string::npos && pos2 != string::npos) {
                    string model = line.substr(0, pos1);
                    string description = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    double pricePerDay = stod(line.substr(pos2 + 1));
                    cars.push_back(Car(model, description, pricePerDay));
                }
            }
            inFile.close();
            //cout << "Cars loaded successfully!\n";
        } else {
            cout << "Error opening file for reading.\n";
        }
    }

    void loadUsersFromFile() {
        ifstream inFile("users.txt");
        if (inFile.is_open()) {
            string username, password;
            while (inFile >> username >> password) {
                customers[username] = make_unique<Customer>(username, password);
            }
            inFile.close();
            //cout << "Users loaded successfully!\n";
        } else {
            cout << "Error opening file for reading.\n";
        }
    }

    void saveUsersToFile() {
        ofstream outFile("users.txt");
        if (outFile.is_open()) {
            for (const auto& customer : customers) {
                outFile << customer.first << " " << customer.second->getPassword() << "\n";
            }
            outFile.close();
            cout << "Users data saved successfully!\n";
        } else {
            cout << "Error opening file for writing.\n";
        }
    }

public:
    CarRentalSystem() {
        loadCarsFromFile();
        loadUsersFromFile();

        // Sample admin for testing
        admins["admin"] = make_unique<Admin>("admin", "admin123");
    }

    // Add review to the file
void add_review() {
    ofstream file("reviews.txt", ios::app); // Open file in append mode
    if (file.is_open()) { // Check if the file opened successfully
        string carModel, reviewText;
        string customerName = signedInUser; // Automatically fetch signed-in username

        cout << "Enter car model name: ";
        cin.ignore(); // Clear the input buffer before taking input
        getline(cin, carModel); // Read the car model name

        cout << "Enter your review: ";
        getline(cin, reviewText); // Read the review text

        // Write review data to the file
        file << "Customer: " << customerName << "\n";
        file << "Car Model: " << carModel << "\n";
        file << "Review: " << reviewText << "\n";
        file << "---------------------------------------\n";

        file.close(); // Close the file after writing
        cout << "Review added successfully!\n";
    } else {
        cout << "Error opening file for writing!\n";
    }
}

// View reviews from the file
/*void view_review() {
    ifstream file("reviews.txt"); // Open file in read mode
    if (file.is_open()) { // Check if the file opened successfully
        string carModelToSearch;
        cout << "Enter car model name to view reviews: ";
        cin.ignore(); // Clear the input buffer before taking input
        getline(cin, carModelToSearch); // Read the car model name

        string line;
        bool found = false;
        cout << "\n--- Reviews for " << carModelToSearch << " ---\n";

        while (getline(file, line)) { // Read each line of the file
            if (line.find("Car Model: " + carModelToSearch) != string::npos) {
                found = true;
                cout << line << "\n"; // Print car model line
                getline(file, line);  // Print next line (Review)
                cout << line << "\n";
                getline(file, line);  // Print next line (Separator)
                cout << line << "\n";
            }
        }

        if (!found) {
            cout << "No reviews found for the car model " << carModelToSearch << ".\n";
        }

        file.close(); // Close the file after reading
    } else {
        cout << "Error opening file for reading!\n";
    }
}*/

// View reviews from the file
void view_review() {
    ifstream file("reviews.txt"); // Open file in read mode
    if (file.is_open()) { // Check if the file opened successfully
        string carModelToSearch;
        cout << "Enter car model name to view reviews: ";
        cin.ignore(); // Clear the input buffer before taking input
        getline(cin, carModelToSearch); // Read the car model name

        struct Review {
            string customerName;
            string reviewText;
        };

        vector<Review> reviews; // To store matching reviews
        string line, customerName, carModel, reviewText;

        while (getline(file, line)) { // Read the "Customer" line
            // Check if the line starts with "Customer: "
            if (line.rfind("Customer: ", 0) != 0) {
                continue; // Skip if format is invalid
            }
            customerName = line.substr(10); // Extract the customer name

            // Read the next three lines for the review block
            if (!getline(file, line) || line.rfind("Car Model: ", 0) != 0) continue;
            carModel = line.substr(11); // Extract the car model

            if (!getline(file, line) || line.rfind("Review: ", 0) != 0) continue;
            reviewText = line.substr(8); // Extract the review text

            if (!getline(file, line) || line != "---------------------------------------") continue;

            // Check if the car model matches the search term
            if (carModel == carModelToSearch) {
                reviews.push_back(Review{customerName, reviewText});
            }
        }

        if (!reviews.empty()) {
            // Display car model title
            cout << "\n--- Reviews for " << carModelToSearch << " ---\n";

            // Display each customer's name and their review
            for (const auto& review : reviews) {
                cout << "Customer Name: " << review.customerName << "\n";
                cout << "Review: " << review.reviewText << "\n";
                cout << "---------------------------------------\n";
            }
        } else {
            cout << "No reviews found for the car model \"" << carModelToSearch << "\".\n";
        }

        file.close(); // Close the file after reading
    } else {
        cout << "Error opening file for reading!\n";
    }
}

void guestSignIn() {
    int choice;
    do {
        cout << "\n--- Guest Mode ---\n";
        cout << "1. View Available Cars\n";
        cout << "2. See Reviews\n";
        cout << "3. Exit Guest Mode\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                        cout << "\nAvailable cars:\n";
        for (int i = 0; i < cars.size(); i++) {
            cout << i + 1 << ". Model: " << cars[i].model << ", Description: "
                 << cars[i].description << ", Price/Day: $" << cars[i].pricePerDay << endl;
        }   // Function to display all cars
                break;
            case 2:
           // Function to display reviews
           view_review() ;
                break;
            case 3:
                cout << "Exiting Guest Mode.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 3);
}





    void run() {
        while (true) {
            int userType;
            cout << "\n--- Welcome to Car Rental System ---\n";
            cout << "1. Sign In (Customer)\n";
            cout << "2. Sign In (Admin)\n";
            cout << "3. Sign In (Guest)\n";
            cout << "4. Sign Up (Customer)\n";
            cout << "5. Exit\n";
            cout << "Choose an option: ";
            cin >> userType;

            if (userType == 1) {
                customerSignIn();
            } else if (userType == 2) {
                adminSignIn();
            } else if (userType == 3) {
               guestSignIn();
            } else if (userType == 4) {
                customerSignUp();
            } else if (userType == 5) {
                cout << "Exiting system...\n";
                break;
            } else {
                cout << "Invalid choice. Try again.\n";
            }
        }
    }

    /*void customerSignIn() {
        string username, password;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        if (customers.find(username) != customers.end() && customers[username]->authenticate(password)) {
            cout << "Welcome, " << username << "!\n";
            customerMenu(customers[username].get());
        } else {
            cout << "Invalid username or password.\n";
        }
    }*/

    void customerSignIn() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    char ch;
    password = "";

    // Loop to read password character by character
    while ((ch = _getch()) != '\r') { // '\r' is Enter key
        if (ch == '\b') { // Handle backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b"; // Remove the last '*' from the console
            }
        } else {
            password += ch;
            cout << '*'; // Display '*' for each typed character
        }
    }
    cout << endl; // Move to the next line after Enter

    if (customers.find(username) != customers.end() && customers[username]->authenticate(password)) {
          signedInUser = username;
        cout << "Welcome, " << username << "!\n";
        customerMenu(customers[username].get());
    } else {
        cout << "Invalid username or password.\n";
    }
}

    void customerSignUp() {
        string username, password;
        cout << "Enter new username: ";
        cin >> username;
        cout << "Enter new password: ";

    char ch;
    password = "";

    // Loop to read password character by character
    while ((ch = _getch()) != '\r') { // '\r' is Enter key
        if (ch == '\b') { // Handle backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b"; // Remove the last '*' from the console
            }
        } else {
            password += ch;
            cout << '*'; // Display '*' for each typed character
        }
    }
    cout << endl;

        if (customers.find(username) == customers.end()) {
            customers[username] = make_unique<Customer>(username, password);
            saveUsersToFile();
            cout << "Customer account created successfully!\n";
        } else {
            cout << "Username already exists.\n";
        }
    }






    void customerMenu(Customer* customer) {
        while (true) {
            customer->displayOptions();
            int choice;
            cout << "Choose an option: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    customer->rentCar(cars);
                    break;
                case 2:
                    customer->viewRentalHistory();
                    break;

                case 3:
                    searchCars();
                    break;
                case 4:
                    add_review();
                    break;
                case 5:
                    view_review();
                    break;
                case 6:
                    customer->updateProfile();
                    saveUsersToFile();
                    break;
                default:
                   return;
            }
        }
    }


    //void adminSignIn() {
       // string username, password;
       // cout << "Enter admin username: ";
       // cin >> username;
       // cout << "Enter admin password: ";
       // cin >> password;

        //if (admins.find(username) != admins.end() && admins[username]->authenticate(password)) {
            //cout << "Welcome, " << username << "!\n";
           // adminMenu(admins[username].get());
        //} else {
            //cout << "Invalid admin credentials.\n";
        //}
    //}
    void adminSignIn() {
    string username, password;
    cout << "Enter admin username: ";
    cin >> username;

    cout << "Enter admin password: ";
    char ch;
    password = "";
    while ((ch = _getch()) != '\r') { // '\r' is the Enter key
        if (ch == '\b') { // Handle backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b"; // Remove the last '*' from console
            }
        } else {
            password += ch;
            cout << '*'; // Display '*' for each character typed
        }
    }
    cout << endl; // Move to the next line after Enter

    // Assuming `admins` is a map of admin objects
    if (admins.find(username) != admins.end() && admins[username]->authenticate(password)) {
        cout << "Welcome, " << username << "!\n";
        adminMenu(admins[username].get());
    } else {
        cout << "Invalid admin credentials.\n";
    }
}

    void adminMenu(Admin* admin) {
        while (true) {
            admin->displayOptions();
            int choice;
            cout << "Choose an option: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    admin->addCar(cars);
                    break;
                case 2:
                    admin->removeCar(cars);
                    break;
                case 3:
                    admin->changeCarPrice(cars);
                    break;
                case 4:
                    admin->viewAllRentals(customers);
                    break;
                case 5:
                    admin->updateProfile();
                    break;
                case 6:
                    return;
                default:
                    cout << "Invalid choice. Try again.\n";
                    break;
            }
        }
    }

    void searchCars() {
        string keyword;
        cout << "Enter a keyword to search for cars (by model/description): ";
        cin.ignore();
        getline(cin, keyword);

        cout << "\n--- Search Results ---\n";
        for (const auto& car : cars) {
            if (car.model.find(keyword) != string::npos || car.description.find(keyword) != string::npos) {
                cout << "Model: " << car.model << ", Description: " << car.description
                     << ", Price/Day: $" << car.pricePerDay << endl;
            }
        }
        cout << "----------------------\n";
    }
};

int main() {
    CarRentalSystem system;
    system.run();
    return 0;
}
