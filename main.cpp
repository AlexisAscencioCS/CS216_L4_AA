#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using std::cin;
using std::cout;
using std::endl;
using std::string;

// ---- Error classification (enum) -------------------------------------------
enum class AccountError {
    InvalidAvailableBelowMin,
    InvalidPresentBelowMin,
    AvailableExceedsPresent,
    Unknown
};

// ---- Exception class --------------------------------------------------------
class BankAccountException : public std::runtime_error {
public:
    explicit BankAccountException(const string& msg, AccountError type = AccountError::Unknown)
        : std::runtime_error(msg), type_(type) {}
    AccountError type() const noexcept { return type_; }
private:
    AccountError type_;
};

// ---- BankAccount class ------------------------------------------------------
class BankAccount {
public:
    // shared minimums
    static constexpr double MIN_DEFAULT_AVAILABLE_BALANCE = 5.00;
    static constexpr double MIN_DEFAULT_PRESENT_BALANCE   = 5.00;

    // constructor/destructor
    BankAccount() : available_(MIN_DEFAULT_AVAILABLE_BALANCE),
                    present_(MIN_DEFAULT_PRESENT_BALANCE) {
        ++s_count;
    }

    BankAccount(double available, double present) {
        try {
            setAccount(available, present);      // may throw
        } catch (const BankAccountException& ex) {
            
            available_ = MIN_DEFAULT_AVAILABLE_BALANCE;
            present_   = MIN_DEFAULT_PRESENT_BALANCE;
            cout << "[Create] " << ex.what()
                 << " -> account set to defaults ($5.00, $5.00)\n";
        }
        ++s_count;
    }

    BankAccount(const BankAccount& other) : available_(other.available_), present_(other.present_) {
        ++s_count;
    }
    BankAccount(BankAccount&& other) noexcept : available_(other.available_), present_(other.present_) {
        ++s_count;
    }

    ~BankAccount() { --s_count; }

    // setter with validation, on error, nothing changes
    void setAccount(double available, double present) {
        // validate before mutating
        if (available < MIN_DEFAULT_AVAILABLE_BALANCE) {
            throw BankAccountException("Available balance below minimum $5.00",
                                       AccountError::InvalidAvailableBelowMin);
        }
        if (present < MIN_DEFAULT_PRESENT_BALANCE) {
            throw BankAccountException("Present balance below minimum $5.00",
                                       AccountError::InvalidPresentBelowMin);
        }
        if (available > present) {
            throw BankAccountException("Available balance cannot exceed present balance",
                                       AccountError::AvailableExceedsPresent);
        }
        // all good
        available_ = available;
        present_   = present;
    }

    // simple accessors
    double available() const { return available_; }
    double present()   const { return present_;   }

    // static accessor for object count
    static int getObjectCount() { return s_count; }

    // print
    void print() const {
        cout << "Account{ available: $" << available_
             << ", present: $" << present_ << " }\n";
    }

private:
    double available_ { MIN_DEFAULT_AVAILABLE_BALANCE };
    double present_   { MIN_DEFAULT_PRESENT_BALANCE   };

    static int s_count;
};

// static definition
int BankAccount::s_count = 0;

// menu -----------------------------------
void printMenu() {
    cout << "\n=== Bank Account Test Menu ===\n"
         << "1) Print number of BankAccount objects in memory\n"
         << "2) Create an account (you choose values)\n"
         << "3) Try to update an existing account (test exceptions)\n"
         << "4) List all accounts\n"
         << "5) Quit\n"
         << "Select: ";
}

int main() {
    std::vector<BankAccount> accounts;

    bool running = true;
    while (running) {
        printMenu();
        int opt; 
        if (!(cin >> opt)) return 0;

        switch (opt) {
        case 1: {
            cout << "Objects currently in memory: "
                 << BankAccount::getObjectCount() << "\n";
            break;
        }
        case 2: {
            cout << "Enter available and present balances: ";
            double a, p; cin >> a >> p;

            // show count before
            cout << "Count before create: " << BankAccount::getObjectCount() << "\n";
            // constructor already handles exceptions internally and sets defaults if needed
            BankAccount acc(a, p);
            accounts.push_back(acc);
            cout << "Created: "; accounts.back().print();
            cout << "Count after create: " << BankAccount::getObjectCount() << "\n";
            break;
        }
        case 3: {
            if (accounts.empty()) {
                cout << "No accounts yet. Create one first (option 2).\n";
                break;
            }
            size_t idx;
            cout << "Choose account index [0.." << (accounts.size()-1) << "]: ";
            cin >> idx;
            if (idx >= accounts.size()) { cout << "Invalid index.\n"; break; }

            cout << "Enter NEW available and present balances: ";
            double a, p; cin >> a >> p;

            cout << "Before update: "; accounts[idx].print();

            try {
                accounts[idx].setAccount(a, p); // will throw on invalid
                cout << "Update OK. After update: ";
                accounts[idx].print();
            } catch (const BankAccountException& ex) {
                cout << "[Update blocked] " << ex.what()
                     << " -> object left unchanged.\n";
                cout << "After failed update: ";
                accounts[idx].print();
            }
            break;
        }
        case 4: {
            if (accounts.empty()) { cout << "(no accounts)\n"; break; }
            for (size_t i = 0; i < accounts.size(); ++i) {
                cout << i << ": "; accounts[i].print();
            }
            break;
        }
        case 5:
            running = false;
            break;
        default:
            cout << "Unknown option.\n";
        }
    }

    cout << "Goodbye!\n";
    return 0;
}
