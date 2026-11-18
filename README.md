# Inventory and Billing System (C++)

Mini Project 7 | Object-Oriented Programming using C++ (CIC-211)

A console-based shop application to manage products and generate customer bills. It checks stock before billing, calculates discount and GST, updates stock after each sale, and saves all records in a file so data stays available after the program closes.

## Team

- Jatin Anand, Enrollment No: ________
- ________ (partner name), Enrollment No: ________
- Maharaja Agrasen Institute of Technology, Delhi

## Features

- Add products with ID, name, price and stock quantity (duplicate IDs rejected)
- Display all products and search a product by ID
- Create a bill with multiple items and quantities, with stock availability check
- Automatic subtotal, discount, GST and final amount calculation
- Stock updated after billing; data saved to `products.txt` and reloaded on the next run
- Input validation for menu choices, numbers and quantities

## Billing Rules

- Discount: 10% if subtotal is Rs 2000 or more
- GST: 5% on the discounted amount
- Final amount = (Subtotal - Discount) + GST

## OOP Concepts Used

| Concept | Where used |
|---|---|
| Classes, objects, constructors | `Money`, `Product`, `Inventory`, `Bill` |
| Encapsulation | Private data members with public member functions |
| Operator overloading | `Money`: `+`, `-`, `*`, `>=` and `<<` |
| Vectors and iterators | Product list in `Inventory`, items in `Bill` |
| File handling | `Inventory::load()` and `Inventory::save()` |
| Exception handling | `Bill::addItem()` (invalid ID, quantity, insufficient stock) |

## Classes

- **Money**: stores amounts in paise to avoid floating-point errors
- **Product**: ID, name, price and quantity of one product
- **Inventory**: holds all products and handles file save/load
- **Bill**: collects billed items and prints the final bill

## How to Compile and Run

Requires a C++ compiler such as g++ (C++11 or later).

```bash
g++ -std=c++11 inventory_billing.cpp -o ib
./ib          # on Windows: ib
```

## Menu

```
1. Add product
2. Display products
3. Search product by ID
4. Create bill
5. Exit
```

## Sample Output

```
ID    Item            Qty   Price       Total
----------------------------------------------------
101   Notebook        10    Rs 40.00    Rs 400.00
103   Bag             2     Rs 1500.00  Rs 3000.00
102   Pen Pack        5     Rs 120.50   Rs 602.50
----------------------------------------------------
Subtotal      : Rs 4002.50
Discount (10%): Rs 400.25
GST (5%)      : Rs 180.11
FINAL AMOUNT  : Rs 3782.36
```

## Limitations

- Console only, no graphical interface
- No option yet to update or delete a product

## Future Enhancements

- Product update/delete and low-stock alerts
- Sales history file with daily reports

## Sustainable Development Goals

SDG 8 (Decent Work and Economic Growth) and SDG 9 (Industry, Innovation and Infrastructure)
