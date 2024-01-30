#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>

struct MenuItem {
    std::string name;
    int price;
};

void openDrawer() {
    #ifdef __linux__
        const std::string base = "/dev/ttyS";
    #else
        #ifdef _WIN32
            const std::string base = "COM";
        #else
            #error Unsupported platform
        #endif
    #endif

    for (int i = 3; i <= 9; ++i) {
        try {
            std::ofstream com(base + std::to_string(i));
            com << char(27) << char(112) << char(0) << char(48);
        } catch (const std::ios_base::failure&) {
            // COM/ttyS port likely was wrong
        }
    }
}

void displayReceipt(const std::vector<MenuItem>& order, int totalAmount, int cash, int change) {
    std::cout << "\nReceipt:\n";
    std::cout << "Order #1 - Walk in\n";

    for (size_t i = 0; i < order.size(); ++i) {
        std::cout << order[i].name << " - " << order[i].price << "pcs\n";
    }

    std::cout << "Total Amount Due - Php" << totalAmount << "\n";
    std::cout << "Cash - Php" << cash << "\n";
    std::cout << "Change - Php" << change << "\n";
}

int main() {
    std::vector<MenuItem> menu = {{"Burger", 39}, {"French Fries", 29}};
    std::vector<MenuItem> order;

    char confirm;
    int totalAmount = 0;
    int cash;

    std::cout << "Menu:\n";
    for (size_t i = 0; i < menu.size(); ++i) {
        std::cout << i + 1 << ". " << menu[i].name << " - Php" << menu[i].price << "\n";
    }

    do {
        size_t choice;
        int quantity;

        std::cout << "Enter the item number to order (1-" << menu.size() << "): ";
        std::cin >> choice;

        if (choice < 1 || choice > menu.size()) {
            std::cerr << "Invalid item number. Please enter a valid item number.\n";
            continue;
        }

        std::cout << "Enter the quantity for " << menu[choice - 1].name << ": ";
        std::cin >> quantity;

        order.push_back({menu[choice - 1].name, quantity});
        totalAmount += quantity * menu[choice - 1].price;

        std::cout << "Do you want to order more items? (y/n): ";
        std::cin >> confirm;

    } while (confirm == 'y' || confirm == 'Y');

    std::cout << "Enter the cash amount: Php";
    std::cin >> cash;

    if (cash < totalAmount) {
        std::cerr << "Insufficient cash. Exiting program.\n";
        return 1;
    }

    int change = cash - totalAmount;

    displayReceipt(order, totalAmount, cash, change);

    std::cout << "Confirm to open the drawer? (y/n): ";
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        std::cout << "Opening drawer...\n";
        std::thread drawerThread(openDrawer);

        // Max of 5 seconds to wait for drawer to open
        const int maxTime = 5;
        auto startTime = std::chrono::steady_clock::now();

        while (std::chrono::steady_clock::now() <= startTime + std::chrono::seconds(maxTime)) {
            if (drawerThread.joinable()) {
                std::cout << '.';
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } else {
                break;
            }
        }

        if (drawerThread.joinable()) {
            drawerThread.detach();
        }

        std::cout << "\nDrawer opened. Thank you for your order!\n";
    } else {
        std::cout << "Thank you for your order!\n";
    }

    return 0;
}
