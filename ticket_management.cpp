#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX_SEATS = 125; // Update the maximum number of seats

bool bookedSeats[MAX_SEATS] = {false};
const int PLANE_CHARGE_PER_KM = 100; // 100 rupees every 20 kms for planes
const int TRAIN_CHARGE_PER_KM = 100; // 100 rupees every 60 kms for trains
const int KMS_PER_PLANE_CHUNK = 20;   // 20 kms per chunk for planes
const int KMS_PER_TRAIN_CHUNK = 60;   // 60 kms per chunk for trains
const int VEG_MEAL_PRICE = 150;      // Veg meal price
const int NON_VEG_MEAL_PRICE = 200;  // Non-veg meal price

struct Ticket {
    string name;
    int age;
    string fromLocation;
    string toLocation;
    string dateOfTravel;
    int numSeats;
    int seatNumbers[MAX_SEATS];
    string modeOfTransport;
    int totalDistance;
    int ticketAmount;
    int mealType; // 1 for veg, 2 for non-veg
};

// Function to generate a random seat number
int generateRandomSeat() {
    return rand() % MAX_SEATS + 1;
}

// Function to book a new ticket and save it to the CSV file
void bookTicket(Ticket &ticket) {
    cout << "Enter passenger name: ";
    cin.ignore();
    getline(cin, ticket.name);
    cout << "Enter passenger age: ";
    cin >> ticket.age;
    cout << "Enter from location: ";
    cin.ignore();
    getline(cin, ticket.fromLocation);
    cout << "Enter to location: ";
    getline(cin, ticket.toLocation);
    cout << "Enter date of travel (dd/mm/yyyy): ";
    getline(cin, ticket.dateOfTravel);

    do {
        cout << "Enter the mode of transport (plane or train): ";
        getline(cin, ticket.modeOfTransport);
    } while (ticket.modeOfTransport != "plane" && ticket.modeOfTransport != "train");

    cout << "Enter the total distance of travel (in kilometers): ";
    cin >> ticket.totalDistance;

    // Calculate the ticket amount based on total distance and mode of transport
    if (ticket.modeOfTransport == "plane") {
        ticket.ticketAmount = (ticket.totalDistance / KMS_PER_PLANE_CHUNK) * PLANE_CHARGE_PER_KM;
        if (ticket.totalDistance % KMS_PER_PLANE_CHUNK != 0) {
            ticket.ticketAmount += PLANE_CHARGE_PER_KM;
        }
    } else if (ticket.modeOfTransport == "train") {
        ticket.ticketAmount = (ticket.totalDistance / KMS_PER_TRAIN_CHUNK) * TRAIN_CHARGE_PER_KM;
        if (ticket.totalDistance % KMS_PER_TRAIN_CHUNK != 0) {
            ticket.ticketAmount += TRAIN_CHARGE_PER_KM;
        }
    } else {
        cout << "Invalid mode of transport. Please try again.\n";
        return;
    }

    cout << "Ticket amount per seat: " << ticket.ticketAmount << " rupees\n";

    cout << "Enter the number of seats: ";
    cin >> ticket.numSeats;

    if (ticket.numSeats > MAX_SEATS) {
        cout << "Error: The maximum number of seats available is " << MAX_SEATS << ".\n";
        return;
    }

    // Ask for meal choice
    cout << "Choose meal type: (1) Veg, (2) Non-Veg: ";
    cin >> ticket.mealType;
    if (ticket.mealType != 1 && ticket.mealType != 2) {
        cout << "Invalid meal choice. Please choose either (1) Veg or (2) Non-Veg.\n";
        return;
    }

    // Calculate the meal cost
    int mealCost = (ticket.mealType == 1) ? (VEG_MEAL_PRICE * ticket.numSeats) : (NON_VEG_MEAL_PRICE * ticket.numSeats);
    cout << "Total meal cost: " << mealCost << " rupees\n";

    // Update the total ticket amount with meal cost
    ticket.ticketAmount += mealCost;

    cout << "Total ticket amount for " << ticket.numSeats << " seats with meal: " << ticket.ticketAmount << " rupees\n";

    int bookedCount = 0;
    for (int i = 0; i < MAX_SEATS && bookedCount < ticket.numSeats; i++) {
        if (!bookedSeats[i]) {
            bookedSeats[i] = true;
            ticket.seatNumbers[bookedCount] = i + 1;
            bookedCount++;
        }
    }

    // Save the ticket data to the CSV file
    ofstream outFile("tickets.csv", ios::app);
    if (outFile) {
        outFile << ticket.name << "," << ticket.age << "," << ticket.fromLocation << ","
                << ticket.toLocation << "," << ticket.dateOfTravel << "," << ticket.numSeats << ","
                << ticket.modeOfTransport << "," << ticket.totalDistance << "," << ticket.ticketAmount << ","
                << ticket.mealType << ",";
        for (int i = 0; i < ticket.numSeats; i++) {
            outFile << ticket.seatNumbers[i] << ",";
        }
        outFile << "\n";
        cout << "Ticket(s) booked successfully!\n";
    } else {
        cout << "Error: Unable to book ticket!\n";
    }
    outFile.close();
}

// Function to cancel a booked ticket based on the passenger name
void cancelTicket() {
    string passengerName;
    cout << "Enter passenger name to cancel the ticket: ";
    cin.ignore();
    getline(cin, passengerName);

    ifstream inFile("tickets.csv");
    ofstream tempFile("temp.csv", ios::app);
    if (inFile && tempFile) {
        string line;
        bool ticketFound = false;

        while (getline(inFile, line)) {
            size_t pos = line.find(passengerName);
            if (pos == string::npos) {
                tempFile << line << "\n";
            } else {
                ticketFound = true;
                // Free the booked seats
                int seat;
                int i = 0;
                while (line[pos + passengerName.length() + 1 + i] != ',') {
                    seat = line[pos + passengerName.length() + 1 + i] - '0';
                    bookedSeats[seat - 1] = false;
                    i++;
                }
            }
        }

        if (ticketFound) {
            cout << "Ticket for " << passengerName << " has been canceled.\n";
        } else {
            cout << "Ticket not found for " << passengerName << ".\n";
        }

        inFile.close();
        tempFile.close();
        remove("tickets.csv");
        rename("temp.csv", "tickets.csv");
    } else {
        cout << "Error: Unable to cancel ticket!\n";
    }
}

// Function to change the location and date of a previously booked ticket
void changeTicket() {
    string passengerName;
    cout << "Enter passenger name to change ticket details: ";
    cin.ignore();
    getline(cin, passengerName);

    ifstream inFile("tickets.csv");
    ofstream tempFile("temp.csv", ios::app);
    if (inFile && tempFile) {
        string line;
        bool ticketFound = false;

        while (getline(inFile, line)) {
            size_t pos = line.find(passengerName);
            if (pos == string::npos) {
                tempFile << line << "\n";
            } else {
                Ticket ticket;
                cout << "Enter new from location: ";
                getline(cin, ticket.fromLocation);
                cout << "Enter new to location: ";
                getline(cin, ticket.toLocation);
                cout << "Enter the new total distance of travel (in kilometers): ";
                cin >> ticket.totalDistance;

                // Calculate the new ticket amount based on total distance and mode of transport
                if (ticket.modeOfTransport == "plane") {
                    ticket.ticketAmount = (ticket.totalDistance / KMS_PER_PLANE_CHUNK) * PLANE_CHARGE_PER_KM;
                    if (ticket.totalDistance % KMS_PER_PLANE_CHUNK != 0) {
                        ticket.ticketAmount += PLANE_CHARGE_PER_KM;
                    }
                } else if (ticket.modeOfTransport == "train") {
                    ticket.ticketAmount = (ticket.totalDistance / KMS_PER_TRAIN_CHUNK) * TRAIN_CHARGE_PER_KM;
                    if (ticket.totalDistance % KMS_PER_TRAIN_CHUNK != 0) {
                        ticket.ticketAmount += TRAIN_CHARGE_PER_KM;
                    }
                } else {
                    cout << "Invalid mode of transport. Please try again.\n";
                    inFile.close();
                    tempFile.close();
                    remove("temp.csv");
                    return;
                }

                cout << "Ticket amount per seat: " << ticket.ticketAmount << " rupees\n";

                cout << "Enter the new number of seats: ";
                cin >> ticket.numSeats;

                if (ticket.numSeats > MAX_SEATS) {
                    cout << "Error: The maximum number of seats available is " << MAX_SEATS << ".\n";
                    inFile.close();
                    tempFile.close();
                    remove("temp.csv");
                    return;
                }

                // Ask for meal choice
                cout << "Choose meal type: (1) Veg, (2) Non-Veg: ";
                cin >> ticket.mealType;
                if (ticket.mealType != 1 && ticket.mealType != 2) {
                    cout << "Invalid meal choice. Please choose either (1) Veg or (2) Non-Veg.\n";
                    inFile.close();
                    tempFile.close();
                    remove("temp.csv");
                    return;
                }

                // Calculate the meal cost
                int mealCost = (ticket.mealType == 1) ? (VEG_MEAL_PRICE * ticket.numSeats) : (NON_VEG_MEAL_PRICE * ticket.numSeats);
                cout << "Total meal cost: " << mealCost << " rupees\n";

                // Update the total ticket amount with meal cost
                ticket.ticketAmount += mealCost;

                cout << "Total ticket amount for " << ticket.numSeats << " seats with meal: " << ticket.ticketAmount << " rupees\n";

                // Free the previously booked seats
                int seat;
                int i = 0;
                while (line[pos + passengerName.length() + 1 + i] != ',') {
                    seat = line[pos + passengerName.length() + 1 + i] - '0';
                    bookedSeats[seat - 1] = false;
                    i++;
                }

                // Book the new seats
                int bookedCount = 0;
                for (int i = 0; i < MAX_SEATS && bookedCount < ticket.numSeats; i++) {
                    if (!bookedSeats[i]) {
                        bookedSeats[i] = true;
                        ticket.seatNumbers[bookedCount] = i + 1;
                        bookedCount++;
                    }
                }

                tempFile << ticket.name << "," << ticket.age << "," << ticket.fromLocation << ","
                         << ticket.toLocation << "," << ticket.dateOfTravel << "," << ticket.numSeats << ","
                         << ticket.modeOfTransport << "," << ticket.totalDistance << "," << ticket.ticketAmount << ","
                         << ticket.mealType << ",";
                for (int i = 0; i < ticket.numSeats; i++) {
                    tempFile << ticket.seatNumbers[i] << ",";
                }
                tempFile << "\n";
                ticketFound = true;
            }
        }

        if (ticketFound) {
            cout << "Ticket details for " << passengerName << " have been updated.\n";
        } else {
            cout << "Ticket not found for " << passengerName << ".\n";
        }

        inFile.close();
        tempFile.close();
        remove("tickets.csv");
        rename("temp.csv", "tickets.csv");
    } else {
        cout << "Error: Unable to update ticket details!\n";
    }
}

// Function to display all the booked tickets with seat numbers
void displayBookedTickets() {
    ifstream inFile("tickets.csv");
    if (inFile) {
        string line;

        while (getline(inFile, line)) {
            size_t pos = line.find(',');
            cout << "Passenger Name: " << line.substr(0, pos) << ", ";
            size_t nextPos = line.find(',', pos + 1);
            cout << "Age: " << line.substr(pos + 1, nextPos - pos - 1) << ", ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "From: " << line.substr(pos + 1, nextPos - pos - 1) << ", ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "To: " << line.substr(pos + 1, nextPos - pos - 1) << ", ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "Date of Travel: " << line.substr(pos + 1, nextPos - pos - 1) << ", ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "Mode of Transport: " << line.substr(pos + 1, nextPos - pos - 1) << ", ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "Total Distance: " << line.substr(pos + 1, nextPos - pos - 1) << " km, ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "Ticket Amount: " << line.substr(pos + 1, nextPos - pos - 1) << " rupees, ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "Meal Type: " << (line.substr(pos + 1, nextPos - pos - 1) == "1" ? "Veg" : "Non-Veg") << ", ";
            pos = nextPos;
            nextPos = line.find(',', pos + 1);
            cout << "Seat Numbers: ";
            int seat;
            int i = pos + 1;
            while (line[i] != ',') {
                seat = line[i] - '0';
                cout << seat << " ";
                i++;
            }
            cout << "\n";
        }
        inFile.close();
    } else {
        cout << "Error: Unable to read booked tickets!\n";
    }
}

// Function to display popular travel locations
void displayPopularLocations() {
    ifstream inFile("popular_locations.txt");
    if (inFile) {
        string line;

        cout << "********** Popular Travel Locations **********\n";
        while (getline(inFile, line)) {
            size_t pos = line.find(',');
            cout << "Location: " << line.substr(0, pos) << ", ";
            size_t nextPos = line.find(',', pos + 1);
            cout << "Distance from Bangalore: " << line.substr(pos + 1, nextPos - pos - 1) << " km\n";
        }
        inFile.close();
    } else {
        cout << "Error: Unable to read popular locations file!\n";
    }
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Initialize random seed

    int choice;

    do {
        cout << "\n**********  Ticket Management System **********\n";
        cout << "1. Book Ticket\n";
        cout << "2. Cancel Ticket\n";
        cout << "3. Change Ticket Details\n";
        cout << "4. Display Booked Tickets\n";
        cout << "5. Display Popular Locations\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                Ticket ticket;
                bookTicket(ticket);
                break;
            }
            case 2: {
                cancelTicket();
                break;
            }
            case 3: {
                changeTicket();
                break;
            }
            case 4: {
                displayBookedTickets();
                break;
            }
            case 5: {
                displayPopularLocations();
                break;
            }
            case 6:
                cout << "Thank you for using our Ticket Management System.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 6);

    return 0;
}
