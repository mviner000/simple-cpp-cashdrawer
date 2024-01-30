#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

struct MenuItem {
    string name;
    int price;
};

void openDrawer() {
    #ifdef __linux__
        const string base = "/dev/ttyS";
    #else
        #ifdef _WIN32
            const string base = "COM";
        #else
            #error Unsupported platform
        #endif
    #endif

    for (int i = 3; i <= 9; ++i) {
        try {
            ofstream com(base + to_string(i));
            com << char(27) << char(112) << char(0) << char(48);
        } catch (const ios_base::failure&) {
            // COM/ttyS port likely was wrong
        }
    }
}

void displayReceipt(int orderNumber, const vector<MenuItem>& order, int totalAmount, int cash, int change) {
    cout << "\nReceipt:\n";
    cout << "Order #" << orderNumber << " - Walk in\n";

    for (size_t i = 0; i < order.size(); ++i) {
        cout << order[i].name << " - " << order[i].price << "pcs\n";
    }

    cout << "Total Amount Due - Php" << totalAmount << "\n";
    cout << "Cash - Php" << cash << "\n";
    cout << "Change - Php" << change << "\n";
}

int main() {
    vector<MenuItem> menu = {{"Burger", 39}, {"French Fries", 29}};
    vector<MenuItem> order;

    char option;
    char confirm;
    int totalAmount;
    int cash;
    int orderNumber = 1;
    int totalCashInDrawer = 0;

    do {
        cout << "\nMain Menu:\n";
        cout << "1. Place Order\n";
        cout << "2. Settings\n";
        cout << "3. Exit\n";

        cout << "Enter your choice (1, 2, or 3): ";
        cin >> option;

        if (option == '1') {
            totalAmount = 0;
            order.clear();

            cout << "\nMenu:\n";
            for (size_t i = 0; i < menu.size(); ++i) {
                cout << i + 1 << ". " << menu[i].name << " - Php" << menu[i].price << "\n";
            }

            do {
                size_t choice;
                int quantity;

                cout << "Enter the item number to order (1-" << menu.size() << "): ";
                cin >> choice;

                if (choice < 1 || choice > menu.size()) {
                    cerr << "Invalid item number. Please enter a valid item number.\n";
                    continue;
                }

                cout << "Enter the quantity for " << menu[choice - 1].name << ": ";
                cin >> quantity;

                order.push_back({menu[choice - 1].name, quantity});
                totalAmount += quantity * menu[choice - 1].price;

                cout << "Do you want to order more items? (y/n): ";
                cin >> confirm;

            } while (confirm == 'y' || confirm == 'Y');

            cout << "Enter the cash amount: Php";
            cin >> cash;

            if (cash < totalAmount) {
                cerr << "Insufficient cash. Exiting program.\n";
                return 1;
            }

            int change = cash - totalAmount;
            totalCashInDrawer += totalAmount;

            displayReceipt(orderNumber, order, totalAmount, cash, change);

            cout << "Confirm to open the drawer? (y/n): ";
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y') {
                cout << "Opening drawer...\n";
                thread drawerThread(openDrawer);

                // Max of 5 seconds to wait for drawer to open
                const int maxTime = 5;
                auto startTime = chrono::steady_clock::now();

                while (chrono::steady_clock::now() <= startTime + chrono::seconds(maxTime)) {
                    if (drawerThread.joinable()) {
                        cout << '.';
                        this_thread::sleep_for(chrono::milliseconds(100));
                    } else {
                        break;
                    }
                }

                if (drawerThread.joinable()) {
                    drawerThread.detach();
                }

                cout << "\nDrawer opened. Thank you for your order!\n";
            } else {
                cout << "Thank you for your order!\n";
            }

            orderNumber++;
        } else if (option == '2') {
            char settingOption;

            cout << "\nSettings:\n";
            cout << "1. Display Total Cash in Drawer\n";
            cout << "X. Back to Main Menu\n";

            cout << "Enter your choice (1 or X): ";
            cin >> settingOption;

            if (settingOption == '1') {
                cout << "Cash in Drawer.\n";
                cout << "Total Amount in Drawer - Php" << totalCashInDrawer << "\n";
                cout << "Press X to back to Main Menu\n";

                char backToMainMenu;
                cin >> backToMainMenu;

                if (backToMainMenu == 'X' || backToMainMenu == 'x') {
                    // Continue to the next iteration of the loop
                } else {
                    cerr << "Invalid option. Returning to Main Menu.\n";
                }
            } else {
                cerr << "Invalid option. Returning to Main Menu.\n";
            }
        } else if (option == '3') {
            cout << "Exiting program...\n";
            return 0;
        } else {
            cerr << "Invalid option. Please enter a valid option.\n";
        }

    } while (true);

    return 0;
}
