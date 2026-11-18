#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits>
#include <stdexcept>
using namespace std;

const string DATA_FILE = "products.txt";
const double GST_RATE = 0.05;          
const long long DISCOUNT_LIMIT = 200000; 
const double DISCOUNT_RATE = 0.10;      


class Money {
    long long paise;
public:
    Money(long long p = 0) : paise(p) {}
    static Money fromRupees(double r) { return Money((long long)(r * 100 + 0.5)); }
    long long raw() const { return paise; }
    double rupees() const { return paise / 100.0; }

    Money operator+(const Money& o) const { return Money(paise + o.paise); }   
    Money operator-(const Money& o) const { return Money(paise - o.paise); }
    Money operator*(int q) const { return Money(paise * q); }
    Money operator*(double f) const { return Money((long long)(paise * f + 0.5)); }
    bool operator>=(const Money& o) const { return paise >= o.paise; }

    string str() const {
        ostringstream ss;
        ss << "Rs " << fixed << setprecision(2) << rupees();
        return ss.str();
    }
    friend ostream& operator<<(ostream& os, const Money& m) { return os << m.str(); }
};

class Product {
    int id;
    string name;
    Money price;
    int quantity;
public:
    Product(int i = 0, string n = "", Money p = Money(), int q = 0)
        : id(i), name(n), price(p), quantity(q) {}

    int getId() const { return id; }
    string getName() const { return name; }
    Money getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void addStock(int q) { quantity += q; }
    void reduceStock(int q) {
        if (q > quantity) throw runtime_error("Insufficient stock");
        quantity -= q;
    }

    void display() const {
        cout << left << setw(6) << id << setw(20) << name
             << setw(12) << price.str() << setw(8) << quantity << '\n';
    }
   
    void save(ostream& out) const {
        out << id << '|' << name << '|' << price.raw() << '|' << quantity << '\n';
    }
};


class Inventory {
    vector<Product> products;

public:
    Inventory() { load(); }
    ~Inventory() { save(); }

    void load() {
        ifstream in(DATA_FILE);
        if (!in) return; 
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            size_t a = line.find('|'), b = line.find('|', a + 1), c = line.find('|', b + 1);
            if (a == string::npos || b == string::npos || c == string::npos) continue;
            try {
                products.push_back(Product(stoi(line.substr(0, a)),
                                           line.substr(a + 1, b - a - 1),
                                           Money(stoll(line.substr(b + 1, c - b - 1))),
                                           stoi(line.substr(c + 1))));
            } catch (...) {  }
        }
    }

    void save() const {
        ofstream out(DATA_FILE);
        if (!out) { cerr << "Warning: could not save data file.\n"; return; }
        for (vector<Product>::const_iterator it = products.begin(); it != products.end(); ++it)
            it->save(out);
    }

    Product* find(int id) {
        for (vector<Product>::iterator it = products.begin(); it != products.end(); ++it)
            if (it->getId() == id) return &(*it);
        return NULL;
    }

    bool add(const Product& p) {
        if (find(p.getId())) return false;
        products.push_back(p);
        return true;
    }

    void displayAll() const {
        if (products.empty()) { cout << "No products in inventory.\n"; return; }
        cout << left << setw(6) << "ID" << setw(20) << "Name"
             << setw(12) << "Price" << setw(8) << "Stock" << '\n'
             << string(46, '-') << '\n';
        for (vector<Product>::const_iterator it = products.begin(); it != products.end(); ++it)
            it->display();
    }
};


struct BillItem {
    int id;
    string name;
    int qty;
    Money price;
};

class Bill {
    vector<BillItem> items;
public:
    void addItem(Inventory& inv, int id, int qty) {
        Product* p = inv.find(id);
        if (!p) throw invalid_argument("Product ID not found");
        if (qty <= 0) throw invalid_argument("Quantity must be positive");
       
        int already = 0;
        for (size_t i = 0; i < items.size(); i++)
            if (items[i].id == id) already += items[i].qty;
        if (already + qty > p->getQuantity())
            throw runtime_error("Insufficient stock (available: " +
                                to_string(p->getQuantity() - already) + ")");
        items.push_back({id, p->getName(), qty, p->getPrice()});
    }

    bool empty() const { return items.empty(); }

   
    void generate(Inventory& inv) {
        Money subtotal;
        cout << "\n=========== BILL ===========\n";
        cout << left << setw(6) << "ID" << setw(16) << "Item" << setw(6) << "Qty"
             << setw(12) << "Price" << setw(12) << "Total" << '\n'
             << string(52, '-') << '\n';
        for (size_t i = 0; i < items.size(); i++) {
            Money line = items[i].price * items[i].qty;
            subtotal = subtotal + line;
            cout << left << setw(6) << items[i].id << setw(16) << items[i].name
                 << setw(6) << items[i].qty << setw(12) << items[i].price.str()
                 << setw(12) << line.str() << '\n';
            inv.find(items[i].id)->reduceStock(items[i].qty);
        }
        Money discount = (subtotal >= Money(DISCOUNT_LIMIT)) ? subtotal * DISCOUNT_RATE : Money(0);
        Money taxable = subtotal - discount;
        Money tax = taxable * GST_RATE;
        Money finalAmt = taxable + tax;
        cout << string(52, '-') << '\n'
             << "Subtotal      : " << subtotal << '\n'
             << "Discount (10%): " << discount << '\n'
             << "GST (5%)      : " << tax << '\n'
             << "FINAL AMOUNT  : " << finalAmt << '\n'
             << "============================\n";
        items.clear();
        inv.save();
    }
};


int readInt(const string& prompt) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x) return x;
        cout << "Invalid input. Enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readDouble(const string& prompt) {
    double x;
    while (true) {
        cout << prompt;
        if (cin >> x) return x;
        cout << "Invalid input. Enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}


void addProduct(Inventory& inv) {
    int id = readInt("Product ID: ");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Name: ";
    string name;
    getline(cin, name);
    double price = readDouble("Price (Rs): ");
    int qty = readInt("Quantity in stock: ");
    if (id <= 0 || name.empty() || price <= 0 || qty < 0) {
        cout << "Invalid details. Product not added.\n";
        return;
    }
    if (inv.add(Product(id, name, Money::fromRupees(price), qty)))
        { inv.save(); cout << "Product added.\n"; }
    else
        cout << "A product with this ID already exists.\n";
}

void searchProduct(Inventory& inv) {
    int id = readInt("Enter product ID: ");
    Product* p = inv.find(id);
    if (!p) { cout << "Product not found.\n"; return; }
    cout << left << setw(6) << "ID" << setw(20) << "Name" << setw(12) << "Price" << setw(8) << "Stock" << '\n';
    p->display();
}

void createBill(Inventory& inv) {
    Bill bill;
    while (true) {
        int id = readInt("Product ID (0 to finish): ");
        if (id == 0) break;
        int qty = readInt("Quantity: ");
        try {
            bill.addItem(inv, id, qty);
            cout << "Item added to bill.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << '\n';
        }
    }
    if (bill.empty()) cout << "No items billed.\n";
    else bill.generate(inv);
}

int main() {
    Inventory inv;
    int choice;
    do {
        cout << "\n--- Inventory & Billing System ---\n"
             << "1. Add product\n2. Display products\n3. Search product by ID\n"
             << "4. Create bill\n5. Exit\n";
        choice = readInt("Choice: ");
        switch (choice) {
            case 1: addProduct(inv); break;
            case 2: inv.displayAll(); break;
            case 3: searchProduct(inv); break;
            case 4: createBill(inv); break;
            case 5: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice. Try 1-5.\n";
        }
    } while (choice != 5);
    return 0;
}
