#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//Bank account definition
class BankAccount
{
    private:
        string accountnumber;
        string accountHolderName;
        double balance;

    public:
        
        //create constructor to initialize acc details
        BankAccount(string accnum, string accname, double initbalance)
        {
            accountnumber = accnum;
            accountHolderName = accname;
            balance = initbalance;
        }

        //create default constructor for creating an empty account object

        BankAccount()
        {
            accountnumber = "";
            accountHolderName = "";
            balance = 0.0;

        }

        //method to deposit money into  account
        void deposit(double amount)
        {
            if(amount > 0)
            {
                balance += amount;
                cout<< "deposited: "<< amount << " | New Balance is :" << balance << endl;
            }
            else
            {
                cout << "deposit amount must be positive!" << endl;

            }
        }


        //method to withdraw amount from account
        void withdraw(double amount)
        {
            if(amount > 0 && amount <= balance)
            {
                balance -= amount;
                cout << "Withdrawn :" << amount << " | New Balance is :" << balance << endl;
            }
            else
            {
                cout << "Insufficient balance!" << endl;

            }
        }

        //method to check balance in account

        void checkBalance() const
        {
            cout << "Account Balance :" << balance << endl;
        }

        //methods to save account details to a file
        void SaveToFile() const
        {
            ofstream file(accountnumber + ".txt");

            if(file.is_open())
            {
                file << accountnumber << endl;          //save the accountnumber
                file << accountHolderName << endl;     //save the holdername
                file << balance << endl;               //save the balance
                file.close();                          // close the file

                cout << "Account details saved to " << accountnumber << ".txt" << endl;
            }
            else
            {
                cout << "unable to open a file" << endl;

            }
        }

        //static method to load account details from the file

        static BankAccount loadFromFile(string accnum)
        {
            ifstream file(accnum + ".txt");
            string accname;
            double balance;

            if(file.is_open())
            {   
                getline(file, accnum);          //read accnum from the file
                getline(file, accname);         //read account holder name from the file
                file >> balance;                //read bal from the file
                file.close();                   //close the file

                //return new bankaccount object initialized with the loaded data
                return BankAccount(accnum, accname, balance);
            }
            else
            {
                cout << " No existing account found with that number.To need to create a new account!" << endl;

                //return an empty bank account object leads to failure
                return BankAccount();

            }
        }


        //method to check if the acc is valid

        bool isvalid() const
        {
            return !accountnumber.empty();

        }
};

int main()
{

    string accnum;

    //prompt from user
    cout << "Enter Account Number:";

    cin >> accnum;


    //attempt to load existing account

    BankAccount account = BankAccount::loadFromFile(accnum);

    //if the account does not exist, prompt from user to create a new one
    if(!account.isvalid())
    {
        string accname;
        double initialbalance;

        cout << "\nCreating a new account..\n";
        
        cout << "Enter Account Holder name:";

        //ignore the enter key left previous cin, before reading full name
        cin.ignore();
        getline(cin, accname);

        cout << "Enter initial balnace :";
        cin >> initialbalance;

        account = BankAccount(accnum, accname, initialbalance);
    }


    int choice;

    do
    {

        //display menu operation for menu interaction

        cout << "\nBank Menu : \n";
        cout << "1. Deposit Amount\n";
        cout << "2. Withdraw Amount\n";
        cout << "3. Check Balance\n";
        cout << "4. Save Account\n";
        cout << "5. Exit\n";
        

        cout << "choose an option: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {

                double amount;
                cout << "Enter amount to deposit :";
                cin >> amount;

                //call deposit function
                account.deposit(amount);
                break;
            }
            case 2:
            {
                double amount;
                cout << "Enter amount to Withdraw :";
                cin >> amount;

                //call withdraw function
                account.withdraw(amount);
                break;
            }
            case 3:
            {
                //call check balance function to display the current balance
                account.checkBalance();
                break;

            }

            case 4:
            {
                //call save to file function to save account details to a file
                account.SaveToFile();
                break;

            }
            case 5:
            {
                //exit the program
                cout << "Exit" << endl;
                break;

            }
            default:
            {
                //if user enter other than this choices
                cout << "Invalid option! Try again" << endl;

            }




        }

    }while(choice != 5);

    return 0;


}