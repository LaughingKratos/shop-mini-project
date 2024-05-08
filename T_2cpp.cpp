#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <ctime>    
#include <unordered_map>

using namespace std;

string GetDate(bool monthforward = true) {
	string date;
	struct tm newtime;
	time_t now = time(0);
	localtime_s(&newtime, &now);
	date = to_string(newtime.tm_mday) + "/" + to_string(monthforward + newtime.tm_mon) + "/" + to_string(1900 + newtime.tm_year);
	return date;
}

class Product {
	long Stock;
	string Name;
	double Price;

	fstream productsFile;

public:
	Product(string name, double price, long stock) : Name(name), Price(price), Stock(stock) 
	{
		productsFile.open("Products.txt", ios::app);
		if (!productsFile.is_open()) {
			cout << "We've got a problem creating/opening the products file!";
			return;
		}

		string line = Name + "," + to_string(Price) + "," + to_string(Stock) + '\n';

		productsFile << line;
		productsFile.close();
	}
	Product(string line, bool findingMode)
	{
		string price, stock;
		stringstream lineS(line);
		getline(lineS, Name, ',');
		getline(lineS, price, ',');
		getline(lineS, stock, ',');
		Price = stod(price);
		Stock = stoi(stock);

		if(!findingMode)
			cout << setw(18) << Name << setw(14) << Stock << setw(14) << Price << setw(10) << '\n';
	}
	Product(const Product& p) : Name(p.Name), Price(p.Price), Stock(p.Stock) {}
	Product(){}

	double get_price() { return Price; }
	string get_name() { return Name; }
	long get_stock() { return Stock; }

	~Product() {};
};

Product* FindProduct(string name) {
	Product* p = new Product();
	fstream productsFile;
	productsFile.open("Products.txt", ios::in);
	if (!productsFile.is_open()) {
		cout << "We've got a problem creating/opening the products file!";
		return p;
	}

	while (!productsFile.eof())
	{
		string fprThing;
		int pointer = productsFile.tellg();
		string line;
		getline(productsFile, line);
		if (line != "")
			productsFile.seekg(pointer);

		getline(productsFile, fprThing, ',');
		if (fprThing != "") {
			productsFile.seekg(pointer);
			getline(productsFile, line);
			if (fprThing == name) {
				p = new Product(line, true);
			}
		}
	}
	if(p->get_name()== "")
		cout << "You dont have this product in the inventory!" << '\n';

	productsFile.close();
	return p;
}

struct Customer
{
	string name;
	string fName;
	string phoneNumber;
};
struct Date_Time 
{
	string date;
	string time;
};

class SubOrder :Product {
	long Amount;
	Product Pro;
	double orderPrice;

public:
	SubOrder(Product product, long amount) : Pro(product), Amount(amount) { orderPrice = Pro.get_price() * Amount; }

	double get_orderprice() { return orderPrice; }
	Product get_orderPro() { return Pro; }
	long get_orderAmount() { return Amount; }

	~SubOrder() {};
};

SubOrder SetSuborder() {
	string prName;
	cout << "Enter the name of product you want to order: ";
	getline(cin, prName);
	Product* p = FindProduct(prName);

	gettingAmount:
	string count;
	cout << "Enter the amount: ";
	getline(cin, count);

	if (p->get_stock() - stol(count) < 0 || stol(count) <= 0) {
		cout << "We dont have this amount of item in the inventory!" << '\n';
		goto gettingAmount;
	}

	string editedProductInfo = p->get_name() + "," + to_string(p->get_price()) + "," + to_string(p->get_stock() - stol(count));

	fstream file;
	file.open("Products.txt", ios::in | ios::out);
	if (!file.is_open()) {
		cout << "We've got a problem creating/opening the products file!";
		exit(0);
	}
	while (!file.eof()) {
		string fprThing;
		int pointer = file.tellg();
		string line2, line3;
		getline(file, fprThing, ',');
		if (fprThing != "") {
			if (fprThing == p->get_name()) {
				getline(file, line2, ',');
				getline(file, line3);
				string size = fprThing + "," + line2 + "," + line3;
				file.seekg(pointer);
				file << fprThing + "," + line2 + "," + to_string(stol(line3)-stol(count)) << endl;
				break;
			}
			else {
				getline(file, fprThing);
			}	
		}
	}

	file.close();
	SubOrder* so = new SubOrder(*p, stol(count));
	return *so;
}


class Order {
	Customer Cus;
	vector<SubOrder> Cart;
	double finalPrice = 0;
	double Discount;
	Date_Time ordDate;
	string Time;

public:
	Order(Customer customer, vector<SubOrder> cart, Date_Time dt, double discount) : Cus(customer), Cart(cart), Discount(discount), ordDate(dt) {
		for (long i = 0; i < cart.size(); i++) {
			finalPrice += cart[i].get_orderprice();
		}
		finalPrice -= (Discount / 100.0) * finalPrice;

		fstream ordFile;
		ordFile.open("Orders.txt", ios::app);
		if (!ordFile.is_open()) {
			cout << "We've got a problem creating/opening the orders file!";
			return;
		}

		string line = "(";
		for (long j = 0; j < cart.size(); j++) {
			line += cart[j].get_orderPro().get_name() + "," + to_string(cart[j].get_orderAmount());
			if (j == (cart.size() - 1))
				line += ")";
			else
				line += '|';
		}
		line += to_string(finalPrice) + "," + customer.name + "," + customer.fName + "," + customer.phoneNumber + "," + dt.date + "," + dt.time + '\n';

		ordFile << line;
		ordFile.close();
	};

	~Order() {};
};

void SetOrder() {
	vector<SubOrder> suborders;
	string sthElse = "";
	string discount = "0";
	Customer cus;
	Date_Time dt;

	while (sthElse == "" || sthElse == "yes") {
		suborders.push_back(SetSuborder());

		cout << "Is there anything else?(yes/no) ";
		getline(cin, sthElse);		
	}

	cout << "Percentage of discount you want to apply?(enter 0 if you dont want) ";
	getline(cin, discount);

	cout << "Now, enter your customer name: ";
	getline(cin, cus.name);

	cout << "Enter his last name: ";
	getline(cin, cus.fName);

	cout << "And his phone number: ";
	getline(cin, cus.phoneNumber);

	string date;
	struct tm newtime;
	time_t now = time(0);
	localtime_s(&newtime, &now);
	dt.time = to_string(newtime.tm_hour) + ":" + to_string(newtime.tm_min) + ":" + to_string(newtime.tm_sec);
	dt.date = GetDate();

	Order* newOrd = new Order(cus, suborders, dt, stod(discount));
	cout << "Order registered successfully!" << '\n';
}

void ShowOrders() {
	ifstream ordFile;
	ordFile.open("Orders.txt");
	if (!ordFile.is_open()) {
		cout << "We've got a problem creating/opening the orders file!";
		return;
	}

	cout << setw(8) << "Id" << setw(25) << "customer name" << setw(16) << "total price" << setw(13) << "date" << setw(10) << '\n';
	cout << string(70, '-') << '\n';

	long id = 0;

	while (!ordFile.eof()) {
		string line, cusName, cusFName, totalPrice, date;
		getline(ordFile, line, ')');
		if (line != "") {
			id++;
			getline(ordFile, totalPrice, ',');
			getline(ordFile, cusName, ',');
			getline(ordFile, cusFName, ',');
			getline(ordFile, line, ',');
			getline(ordFile, date, ',');
			getline(ordFile, line);
			cusName += " " + cusFName;
				
			cout << setw(8) << id << setw(25) << cusName << setw(15) << stod(totalPrice) << setw(14) << date << setw(10) << '\n';
		}
	}

	ordFile.close();
	ordFile.open("Orders.txt");
	int begin = ordFile.tellg();

	long showId = -1;
	do {
		if (showId == 0)
			break;

		string line;
		while (showId > 1) {
			getline(ordFile, line);
			showId--;
		}
		if (showId == 1) {
			int pointer = ordFile.tellg();
			string line, cusName, cusFName, totalPrice, date, time, cusContact;
			cout << '\n' << setw(20) << "customer name" << setw(22) << "customer contact" << setw(15) << "total price" << setw(13) << "date" << setw(10) << "time" << setw(10) << '\n';
			cout << string(90, '-') << '\n';

			getline(ordFile, line, ')');
			getline(ordFile, totalPrice, ',');
			getline(ordFile, cusName, ',');
			getline(ordFile, cusFName, ',');
			getline(ordFile, cusContact, ',');
			getline(ordFile, date, ',');
			getline(ordFile, time);
			cusName += " " + cusFName;

			cout << setw(20) << cusName << setw(22) << cusContact << setw(15) << stod(totalPrice) << setw(13) << date << setw(10) << time << setw(10) << '\n';

			cout << '\n' << setw(35) << "item name" << setw(15) << "amount" << setw(10) << '\n';
			cout << string(65, '-') << '\n';

			string subline;
			for (int i = 1; i <= line.length(); i++) {
				if (line[i] == ',') 
				{
					cout << setw(35) << subline;
					subline = "";
				}
				else if (line[i] == '|' || i == line.length())
				{
					cout << setw(15) << subline << setw(10) << '\n';
					subline = "";
				}
				else
				{
					subline += line[i];
				}
			}
			ordFile.seekg(begin);
		}
		cout << '\n' << "Which one do you want to see the details?(enter Id)(enter 0 to abort) ";
	} while (cin >> showId);

	ordFile.close();
}

struct DateType {
	short day, month, year;
};

DateType StringtoDate(string s, bool dayback = 0) {
	DateType dt;
	string day, month, year;
	stringstream ss(s);
	getline(ss, day, '/');
	getline(ss, month, '/');
	getline(ss, year);

	dt.day = stoi(day)-dayback;
	dt.month = stoi(month);
	dt.year = stoi(year);
	return dt;
}

bool ReachedDate(DateType date, DateType targetDate) {
	if (date.year > targetDate.year)
		return true;
	else if (date.year == targetDate.year && date.month > targetDate.month)
		return true;
	else if (date.year == targetDate.year && date.month == targetDate.month && date.day >= targetDate.day)
		return true;
				
	return false;
}

struct ProductOrders
{
	string name;
	long amount = 0;
};

void Report() {
	string startofp, endofp;
	double revenue = 0;
	vector<ProductOrders> products;
	cout << "Enter the starting day of period(dd/mm/yy form)(use 0 for default): ";
	cin >> startofp;
	if (startofp == "0") {
		startofp = GetDate(false);
		endofp = GetDate();
		goto afterInput;
	}
	cout << "Enter the ending day of period(dd/mm/yy form): ";
	cin >> endofp;

	afterInput:
	ifstream ordFile;
	ordFile.open("Orders.txt");
	if (!ordFile.is_open()) {
		cout << "We've got a problem creating/opening the orders file!";
		return;
	}

	bool flag = false, flag2 = false;

	string line, bullshit, totalPrice, date;
	while (!ordFile.eof()) {
		totalPrice = "0";
		if (flag2)
			break;

		getline(ordFile, line, ')');
		getline(ordFile, totalPrice, ',');
		for (short i = 0; i < 3; i++)
			getline(ordFile, bullshit, ',');
		getline(ordFile, date, ',');
		getline(ordFile, bullshit);

		if (!flag){
			if (ReachedDate(StringtoDate(date), StringtoDate(startofp))) {
				flag = true;
				goto here;
			}
		}
		else {
			here:
			if(ReachedDate(StringtoDate(date,true), StringtoDate(endofp)))
				flag2=true;
			else{
				revenue += stod(totalPrice);
				
				string subline;
				string temp;
				int pos = -1;
				for (int i = 1; i <= line.length(); i++) {
					if (line[i] == ',')
					{
						for (int j = 0; j < products.size();j++) {
							if (products[j].name == subline) {
								pos = j;
							}
						}if (pos == -1) {
							temp = subline;
						}

						subline = "";
					}
					else if (line[i] == '|' || i == line.length())
					{
						if (pos > -1) {
							products[pos].amount += stol(subline);
						}
						else {
							ProductOrders p;
							p.name = temp;
							p.amount = stol(subline);
							products.push_back(p);
						}
						subline = "";
					}
					else
					{
						subline += line[i];
					}
				}
				
			}
		}
		ordFile.close();
	}

	if (revenue == 0) {
		cout << "No records in this period!" << '\n';
	}
	else {
		cout << setw(15) << "revenue" << setw(35) << "item name" << setw(15) << "amount sold" << setw(10) << '\n';
		cout << string(70, '-') << '\n';
		cout << setw(15) << revenue;
		for (int i = 0; i < products.size(); i++) {
			if(i > 0)
				cout << setw(50) << products[i].name << setw(15) << products[i].amount << setw(10) << '\n';
			else
				cout << setw(35) << products[i].name << setw(15) << products[i].amount << setw(10) << '\n';
		}
	}
}

/////////////////////////////////////////////////// Projecting Products

void SetProduct() {
	long stock;
	string name;
	double price;

	cout << "Enter the name of our new product: ";
	getline(cin, name);
	cout << "How many/much of that we,ve got in the inventory: ";
	while (!(cin >> stock)) {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid input; please re-enter.\n";
	}
	cout << "And What'll be the price: ";
	while (!(cin >> price)) {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid input; please re-enter.\n";
	}

	Product* newP = new Product(name, price, stock);

	cout << "Item added successfully!" << '\n';
}

void ShowProducts() {
	fstream productsFile;
	productsFile.open("Products.txt", ios::in);
	if (!productsFile.is_open()) {
		cout << "We've got a problem creating/opening the products file!";
		return;
	}

	cout << setw(18) << "name" << setw(14) << "stock" << setw(14) << "price" << setw(10) << '\n';
	cout << string(50, '-') << '\n';

	string line;
	while (!productsFile.eof()) {
		getline(productsFile, line);
		if (line != "") {			
			Product* pShow = new Product(line, false);
		}
	}
	productsFile.close();
}

void EditProduct() {
	string prName;
	cout << "Enter the name of product you'd like to edit: ";
	getline(cin, prName);
	while (prName == "") {
		cout << "Give me something bro!" << '\n';
		getline(cin, prName);
	}

	fstream productsFile;
	productsFile.open("Products.txt", ios::out | ios::in);
	if (!productsFile.is_open()) {
		cout << "We've got a problem creating/opening the products file!";
		return;
	}

	string fprThing;
	while (!productsFile.eof())
	{
		int pointer = productsFile.tellg();
		getline(productsFile, fprThing, ',');
		if (fprThing != "") {
			if (fprThing == prName) {
				editing:
				cout << "What do you want to edit?(name/stock/price): ";
				string editingOption;
				getline(cin, editingOption);

				string newLine;

				if (editingOption == "name") {
					cout << "Enter a new name for it: ";
					string newName;
					getline(cin, newName);
					getline(productsFile, fprThing, ',');
					newLine = newName + "," + fprThing + ",";
					getline(productsFile, fprThing);
					newLine += fprThing;
				}
				else if (editingOption == "price") {
					cout << "Enter the new price: ";
					string newPrice;
					getline(cin, newPrice);
					newLine = fprThing + "," + newPrice + ",";
					getline(productsFile, fprThing, ',');
					getline(productsFile, fprThing, ',');
					newLine += fprThing;
				}
				else if (editingOption == "stock") {
					cout << "Enter new stock: ";
					string newStock;
					getline(cin, newStock);
					newLine = fprThing + ",";
					getline(productsFile, fprThing, ',');
					newLine += fprThing + "," + newStock;
				}
				else {
					cout << "Invalid input! Try again..." << '\n';
					goto editing;
				}

				productsFile.seekg(pointer);
				productsFile << newLine << endl;
				productsFile.close();
				cout << "Item edited successfully!" << '\n';
				return;
			}
		}
		getline(productsFile, fprThing);
	}
	productsFile.close();
	cout << "Item not found!" << '\n';
}


///////////////////////////////////////////////////


void AnalayzeCommand(string command) {
	if (command == "")
		return;
	
	if (command == "set product")
		SetProduct();
	else if (command == "list of products")
		ShowProducts();
	else if (command == "edit product")
		EditProduct();
	else if (command == "set order")
		SetOrder();
	else if (command == "list of orders")
		ShowOrders();
	else if (command == "report")
		Report();

	cout << '\n' << "What do you want me to do? ";
}

int main() {
	cout << "List of commands:" << '\n' << "set product - list of products - edit product - set order - list of orders - report" << '\n' << '\n';
	string cmd;
	cout << "What do you want me to do? ";
	while (getline(cin, cmd)) {
		AnalayzeCommand(cmd);
	}
}