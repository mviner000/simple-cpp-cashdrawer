#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

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

int main() {
    char choice;

    do {
        std::cout << "Menu:\n";
        std::cout << "1. Open Drawer\n";
        std::cout << "2. Exit Program\n";

        std::cout << "Enter your choice (1 or 2): ";
        std::cin >> choice;

        if (choice == '1') {
            std::cout << "\nOpening drawer... \nPlease Wait\n";
            std::thread drawerThread(openDrawer);

            // Max of 5 seconds to wait for drawer to open
            const int maxTime = 1;
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

            std::cout << "\nDrawer opened. You can use the menu again.\n";
        } else if (choice == '2') {
            std::cout << "Exiting program...\n";
        } else {
            std::cerr << "Invalid choice. Please enter 1 or 2.\n";
        }

    } while (choice != '2');

    return 0;
}
