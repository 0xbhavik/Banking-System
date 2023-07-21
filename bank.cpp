#include <iostream>
#include <string>
#include <sstream>
#include <iomanip> // std::setprecision
#include <math.h>
#include <vector>
#include <ctime>
#include <chrono>
#include <set>

using namespace std;

int _userId = 1;
int _account_no = 1000000;
int _atm_card_no = 1000000;
int _cvv = 100;
int _transaction_id = 10000000;
int _loan_id = 1;

class Account;
class SavingAccount;
class CurrentAccount;
class LoanAccount;
class User;

vector<int> getdate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    vector<int> date;
    date.push_back(1900 + ltm->tm_year);
    date.push_back(1 + ltm->tm_mon);
    date.push_back(ltm->tm_mday);
    return date;
}

string getDateInString(vector<int> date)
{
    string day = to_string(date[0]);
    string month = to_string(date[1]);
    string year = to_string(date[2]);

    string outputDate = day + "-" + month + "-" + year;
    return outputDate;
}

unordered_map<int, Account> acctIdToacc;
unordered_map<int, User> userIdToUser;
unordered_map<int, SavingAccount> accIdtoSavAcc;
unordered_map<int, CurrentAccount> accIdtoCurrAcc;
unordered_map<int, LoanAccount> accIdtoLoanAcc;
unordered_map<int, int> accTypes;
set<int> allAccounts;
set<int> userIdSet;

// Declaring Account Class

class Account
{
public:
    int account_no;
    int user_id;
    int balance; // Assumption: balance will always remain in 'int' format.
    int age_limit;
    string accType;
    vector<int> account_creation_date;

    void showBalance()
    {
        cout << "your balance is " << balance << endl;
    }

    int getBalance()
    {
        return this->balance;
    }

    void viewDetails()
    {
        cout << "Account number is " << this->account_no << endl;
        cout << "your balance is " << this->balance << endl;
        cout << "account type is " << this->accType << endl;
    }
};

class Transaction
{
public:
    int transaction_id;
    int withdrawal_amt;
    vector<int> date;
    int type = -1; // 0-atm , 1-direct
    int account_id;
    int user_id;

    Transaction(int withdrawal_amt, vector<int> date, int type)
    {
        transaction_id = _transaction_id++;
        this->withdrawal_amt = withdrawal_amt;
        this->date = date;
        this->type = type;
    }
    string getType(int type)
    {
        if (type == 0)
            return "ATM";
        if (type == 1)
            return "Direct";
        return "Current";
    }
    void viewTransaction()
    {
        cout << "transaction Id : " << transaction_id << endl;
        cout << "withdrawal_amt : " << withdrawal_amt << endl;
        cout << "Date: " << getDateInString(date) << endl;
        cout << "type " << getType(type) << endl;
        cout << endl;
    }
};

class Atm
{
public:
    int card_no;
    int cvv;
    vector<int> expiry_date;

    Atm()
    {
        card_no = _atm_card_no++;
        cvv = _cvv++;
        expiry_date = getExpiryDate();
    }

    void viewAtmDetails()
    {
        cout << "card_no : " << card_no << endl;
        cout << "expiry_date : " << getDateInString(expiry_date) << endl;
        cout << "cvv : " << cvv << endl;
    }

    vector<int> getExpiryDate()
    {

        vector<int> date = getdate();

        date[2] += 4;
        return date;
    }
};

class NonLoanAccount : public Account
{
public:
    int interest_rate;
    int NRV;
    int penaly_amt_NRV;
    Atm atm;
    int min_amt;
    vector<Transaction> transactions;

    void depositAmt()
    {
        cout << "Enter amount you want to deposit" << endl;
        int amt;
        cin >> amt;
        this->balance += amt;
        cout << "deposited" << endl;
    }

    void CheckNRV()
    {
        if (this->balance < NRV)
        {
            cout << "your NRV has fallen below minimum level" << penaly_amt_NRV << "rs will be deducted from your account" << endl;
            this->balance -= penaly_amt_NRV;
        }
    }
    void viewAllTransactions()
    {
        for (int i = 0; i < transactions.size(); i++)
        {
            transactions[i].viewTransaction();
        }
    }
    void viewAtmDetails()
    {
        atm.viewAtmDetails();
        cout << endl;
    }
};

class SavingAccount : public NonLoanAccount
{
public:
    int max_monthly_atm_withdrawal_limit;
    int max_single_transaction_withdrawal_amt;
    int max_daily_withdrawal_amt;
    int penalty_amt_atm_withdrawal;

    SavingAccount()
    {
        account_no = _account_no++;
        interest_rate = 6;
        NRV = 100000;
        penaly_amt_NRV = 1000;
        age_limit = 0;
        min_amt = 10000;
        accType = "saving";
        max_monthly_atm_withdrawal_limit = 5;
        max_single_transaction_withdrawal_amt = 20000;
        max_daily_withdrawal_amt = 50000;
        penalty_amt_atm_withdrawal = 500;
    }

    // bool validateCVV(int _cvv){
    //     return _cvv == atm.cvv;
    // }

    bool checkAgeEligibility(int age)
    {
        return age >= age_limit;
    }

    bool checkAccPossible(int bal)
    {
        if (bal >= min_amt)
            return 1;
        else
            return 0;
    }

    bool checkDailyWithdrawalLimit(int amt, vector<int> date)
    {

        int curr_day = date[0];
        int prevAmt = 0;
        int n = transactions.size();
        for (int i = n - 1; i >= 0; i--)
        {
            int txnDay = transactions[i].date[0];
            if (txnDay == curr_day)
            {
                prevAmt += transactions[i].withdrawal_amt;
            }
            else
            {
                break;
            }
        }
        if (prevAmt + amt <= max_daily_withdrawal_amt)
            return 1;
        return 0;
    }

    bool monthlyWithdrawalLimit(vector<int> date)
    {
        int curr_month = date[1];
        int n = transactions.size();
        int withdrawals = 0;
        for (int i = n - 1; i >= 0; i--)
        {
            if (transactions[i].type == 1 || transactions[i].type == -1)
                continue;
            int txnMonth = transactions[i].date[1];
            if (txnMonth == curr_month)
            {
                withdrawals++;
            }
            else
            {
                break;
            }
        }
        return withdrawals >= 5;
    }

    void addInterestAmt()
    {
        balance += ((balance * interest_rate) / 100);
    }

    void withdrawAmt()
    {
        int flag, amt;

        cout << "Enter how would you like to withdraw your account\n press 1 for Atm withdrawal \n press 2 for direct withdrawal \n";
        cin >> flag;
        cout << "How many amount you want to withdraw ?" << endl;
        cin >> amt;
        if (getBalance() < amt)
        {
            cout << " Insufficient Balance " << endl;
            return;
        }
        if (amt > max_single_transaction_withdrawal_amt)
        {
            cout << "can't withdraw entered amount is more than maximum single transaction amount" << endl;
            return;
        }
        vector<int> curr_date(3);
        cout << "Enter current day" << endl;
        cin >> curr_date[0];
        cout << "Enter current month" << endl;
        cin >> curr_date[1];
        cout << "Enter current Year" << endl;
        cin >> curr_date[2];

        if (!checkDailyWithdrawalLimit(amt, curr_date))
        {
            cout << "Can't withdraw. Maximum of Rs 50000 can be withdrawn in a day" << endl;
            return;
        }

        // atm withdrawal
        // Remaining: logic for atm pin authentication
        if (flag == 1)
        {
            // int _cvv;
            // cout << "Enter CVV" << endl;
            // cin >> _cvv;
            
            bool flag = monthlyWithdrawalLimit(curr_date);
            if (flag)
            {
                cout << "You have already made five atm transactions this month. thus this doing this transaction will cost you 500rs" << endl;
                cout << "Press 1 for making transaction \n Press 2 for exit";
                int _flag;
                cin >> _flag;
                if (_flag == 2)
                    return;
                this->balance -= penalty_amt_atm_withdrawal; //todo
            }
            this->balance -= amt;
            Transaction newtxn(amt, curr_date, -1);
            transactions.push_back(newtxn);
            cout << "Amount withdrawn";
        }
        else
        {
            this->balance -= amt;
            Transaction newtxn(amt, curr_date, -1);
            transactions.push_back(newtxn);
            cout << "Amount withdrawn";
        }
    }
};

class CurrentAccount : public NonLoanAccount
{
public:
    int transaction_percentage_charge;
    int max_charge__per_transaction;
    int penalty_amt_per_month;

    CurrentAccount()
    {
        account_no = _account_no++;
        interest_rate = 0;
        NRV = 500000;
        penaly_amt_NRV = 5000;
        accType = "current";
        age_limit = 18;
        transaction_percentage_charge = 1;
        max_charge__per_transaction = 500;
        penalty_amt_per_month = 500;
        min_amt = 100000;
    }

    bool checkAccPossible(int bal)
    {
        if (bal >= min_amt)
            return 1;
        else
            return 0;
    }

    bool checkAgeEligibility(int age)
    {
        return age >= age_limit;
    }

    void withdrawAmt()
    {
        int amt;
        vector<int> date;
        cout << "How many amount you want to withdraw ?" << endl;
        cin >> amt;
        if (this->getBalance() < amt)
        {
            cout << " Insufficient Balance" << endl;
            return;
        }
        vector<int> curr_date(3);
        cout << "Enter current day" << endl;
        cin >> curr_date[0];
        cout << "Enter current month" << endl;
        cin >> curr_date[1];
        cout << "Enter current Year" << endl;
        cin >> curr_date[2];
        int charge = calculateTransactionCharge(amt);
        cout << charge << " rs will be deducted from account as transaction charges" << endl;
        this->balance -= amt;
        this->balance -= charge;
        Transaction newtxn(amt, date, 1);
        transactions.push_back(newtxn);
        cout << "Amount withdrawn";
    }

    // calculating transaction charge
    int calculateTransactionCharge(int amt)
    {
        int amt1 = (amt * transaction_percentage_charge) / 100;

        // returning whichever is less
        return amt1 <= max_charge__per_transaction ? amt1 : max_charge__per_transaction;
    }

    void checkMonthlyTransactions()
    {
        vector<int> curr_date = getdate();
        int curr_month = curr_date[1];
        int no_of_txns = 0;
        for (int i = transactions.size() - 1; i >= 0; i--)
        {
            Transaction txn = transactions[i];
            if (txn.date[1] == curr_month)
            {
                no_of_txns++;
            }
            else
            {
                break;
            }
        }
        if (no_of_txns < 3)
        {
            cout << "No. of transactions is less than 3 in this month, 500 rs is deducted as penalty";
            this->balance -= 500;
        }
    }
};

class LoanAccount : public Account
{
public:
    int initialLoanAmt;
    int loanRemaining;
    int percent_total_deposit;
    int min_duration_year;
    int min_amt;
    int type;
    int interest_rate;
    int duration;

    LoanAccount()
    {
        this->user_id = user_id;
        this->account_no = _account_no++;
        this->percent_total_deposit = 40;
        this->min_duration_year = 2;
        this->min_amt = 500000;
        this->age_limit = 25;
        this->account_creation_date = getdate();
        this->balance = 0;
    }

    bool checkEligibility(int, int);

    bool checkValidLoanAmt(int amt)
    {
        return amt >= min_amt;
    }

    void setInterestRate(int type)
    {
        if (type == 0)
        {
            this->interest_rate = 7;
        }
        else if (type == 1)
        {
            this->interest_rate = 8;
        }
        else if (type == 2)
        {
            this->interest_rate = 12;
        }
        else
        {
            this->interest_rate = 15;
        }
    }

    bool takeLoan(int user_id)
    {

        cout << "which loan you want to take \n 1. Home \n 2. Car \n 3. Personal \n 4. Business \n ";
        int type;
        cin >> type;

        cout << "Enter loan amount \n";
        int amt;
        cin >> amt;

        if (!checkValidLoanAmt(amt))
        {
            cout << " \n Error : Minimum amount to take a loan is 5,00,000 \n";
            return 0;
        }

        bool check = checkEligibility(user_id, amt);
        if (!check)
        {
            cout << "\n can't give loan. The bank can only give 40 percent of total deposits as loan \n";
            return 0;
        }

        int loan_duration;
        cout << "Enter loan duration (in years) \n";
        cin >> loan_duration;

        if (loan_duration < min_duration_year)
        {
            cout << " \n Error : min loan duration is 2 years \n";
            return 0;
        }
        if (loan_duration > 40)
        {
            cout << " \n Error : max loan duration is 40 years \n";
            return 0;
        }
        this->initialLoanAmt = amt;
        this->loanRemaining = amt;
        this->duration = loan_duration;
        this->type = type;

        this->setInterestRate(type);
        this->user_id = user_id;

        this->account_creation_date = getdate();
        allAccounts.insert(this->account_no);
        accTypes[this->account_no] = 3;

        cout << "your loan account is created \n here are the details" << endl;
        this->loanDetails();

        return 1;
    }
    string getloanType(int type)
    {
        if (type == 1)
            return "Home loan";
        else if (type == 2)
            return "Car loan";
        else if (type == 3)
            return "Personal loan";
        else
            return "Business loan";
    }

    void loanDetails()
    {
        cout << "loan account no. : " << account_no << endl;
        cout << "loan type : " << getloanType(this->type) << endl;
        cout << "loan interest rate : " << this->interest_rate << endl;
        cout << "loan creation date : " << getDateInString(this->account_creation_date) << endl;
        cout << "loan duration : " << this->duration << endl;
        cout << "Total loan Amount : " << this->initialLoanAmt << endl;
        cout << "loan Amount remaining : " << this->loanRemaining << endl;
    }
    void RepayLoan()
    {
        cout << "\n how much you want to repay \n"
             << endl;
        int amt;
        cin >> amt;
        if (!validityCheck(amt))
        {
            cout << "Error : deposit amount should not exceeding 10% of total loan amount. \n ";
            return;
        }
        this->loanRemaining -= amt;
    }

    void compoundInterest()
    {
        loanRemaining = loanRemaining + (((loanRemaining * interest_rate) / 100) / 2);
    }

    bool validityCheck(int amt)
    {
        return amt < ((initialLoanAmt * 10) / 100);
    }
};

class User
{
public:
    int userId;
    string name;
    string dob;
    int age;
    string phone_no;
    vector<int> savingAccountsId;
    vector<int> currentAccountsId;
    vector<int> loanAccountsId;

    User()
    {
        this->userId = _userId++;
    }

    void addAccount()
    {

        cout << "which account type you want to create ?" << endl;
        cout << "1. savings" << endl;
        cout << "2. current" << endl;
        cout << "3. loan" << endl;

        int type;
        cin >> type;
        if (type == 1)
        {
            SavingAccount account;
            cout << "Enter amount you want to deposit" << endl;
            int amt;
            cin >> amt;
            bool flag = account.checkAccPossible(amt);
            if (!flag)
            {
                cout << "you have to deposit more amount to open this account" << endl;
                return;
            }

            account.user_id = this->userId;
            account.balance = amt;
            account.account_creation_date = getdate();
            accIdtoSavAcc[account.account_no] = account;
            savingAccountsId.push_back(account.account_no);
            accTypes[account.account_no] = 1;
            allAccounts.insert(account.account_no);
            cout << "Amount Created with account no. " << account.account_no << endl;
        }
        else if (type == 2)
        {
            CurrentAccount account;
            cout << "Enter amount you want to deposit" << endl;
            int amt;
            cin >> amt;
            bool flag = account.checkAccPossible(amt);
            if (!flag)
            {
                cout << "you have to deposit more amount to open this account" << endl;
                return;
            }

            if (!account.checkAgeEligibility(this->age))
            {
                cout << "Your age is less than 18";
            }

            account.user_id = this->userId;
            account.balance = amt;
            account.account_creation_date = getdate();
            accIdtoCurrAcc[account.account_no] = account;
            this->currentAccountsId.push_back(account.account_no);
            allAccounts.insert(account.account_no);

            accTypes[account.account_no] = 2;

            cout << "Amount Created with account no. " << account.account_no << endl;
        }
        else if (type == 3)
        {
            if (savingAccountsId.size() == 0 && currentAccountsId.size() == 0)
            {
                cout << "you must have savings or current account to open loan account" << endl;
                return;
            }

            if (this->age < 25)
            {
                cout << "Minimum age should be 25 to take loan" << endl;
                return;
            }

            LoanAccount account;
            bool check = account.takeLoan(this->userId);
            if (check == 1)
            {
                this->loanAccountsId.push_back(account.account_no);
                accIdtoLoanAcc[account.account_no] = account;
            }
        }
    }

    void viewAllAccountDetails()
    {

        if (savingAccountsId.size() > 0)
        {
            cout << "your saving account details are listed below" << endl;

            for (int i = 0; i < savingAccountsId.size(); i++)
            {
                SavingAccount acc = accIdtoSavAcc[savingAccountsId[i]];
                acc.viewDetails();
            }
        }
        cout << endl;
        if (currentAccountsId.size() > 0)
        {
            cout << "your current account numbers are listed below" << endl;
            for (int i = 0; i < currentAccountsId.size(); i++)
            {
                CurrentAccount acc = accIdtoCurrAcc[currentAccountsId[i]];
                acc.viewDetails();
            }
        }
        cout << endl;
        if (loanAccountsId.size() > 0)
        {
            cout << "your loan account numbers are listed below" << endl;
            for (int i = 0; i < loanAccountsId.size(); i++)
            {

                LoanAccount acc = accIdtoLoanAcc[loanAccountsId[i]];
                acc.viewDetails();
            }
        }
        cout << endl;
    }

    void getAllLoanAmount()
    {
        int total_loan_amt = 0;
        if (loanAccountsId.size() == 0)
        {
            cout << "user don't have any loan" << endl;
        }
        else
        {
            for (int i = 0; i < loanAccountsId.size(); i++)
            {
                total_loan_amt += accIdtoLoanAcc[loanAccountsId[i]].loanRemaining;
            }
        }
        cout << "user total loan remaining : " << total_loan_amt << endl;
    }

    void viewTransactions()
    {
        int account_no;
        int type;
        cout << "Enter Type of account \n press 1 for saving \n press 2 for current" << endl;
        cin >> type;
        cout << "Enter account no. of account you want to view transactions of ";
        cin >> account_no;
        cout << "your transaction details of account" << account_no << "are as follows" << endl;

        if (type == 1)
        {
            SavingAccount account = accIdtoSavAcc[account_no];
            account.viewAllTransactions();
        }
        else
        {
            CurrentAccount account = accIdtoCurrAcc[account_no];
            account.viewAllTransactions();
        }
    }

    void viewPersonalDetails()
    {

        cout << "your username is " << this->name << endl;
        cout << "your phone number is " << this->phone_no << endl;
        cout << "your date of birth is " << this->dob << endl;
    }

    int TotalDeposit()
    {
        int amt = 0;

        for (int i = 0; i < savingAccountsId.size(); i++)
        {
            SavingAccount acc = accIdtoSavAcc[savingAccountsId[i]];
            amt += acc.getBalance();
        }
        for (int i = 0; i < currentAccountsId.size(); i++)
        {
            CurrentAccount acc = accIdtoCurrAcc[currentAccountsId[i]];
            amt += acc.getBalance();
        }

        return amt;
    }
};

bool LoanAccount ::checkEligibility(int user_id, int loanAmt)
{

    int total_deposit_amt = userIdToUser[user_id].TotalDeposit();
    return loanAmt < ((total_deposit_amt * 40) / 100);
}
// Global Functions

int makeUserId(string user)
{
    vector<int> v(26);
    v[0] = 1;
    for (int i = 1; i < 26; i++)
    {
        v[i] = (v[i - 1] * 2) + (i + 1);
    }
    int cust_id = 0;
    for (int i = 0; i < user.size(); i++)
    {
        char c = tolower(user[i]);
        cust_id += (v[c - 'a']);
    }
    return cust_id;
}

void newUserLogIn()
{

    User user;
    string name;
    cout << "enter your name" << endl;
    cin >> name;

    if (name.size() == 0)
    {
        return;
    }
    user.name = name;
    cout << "enter your phoneNumber" << endl;
    cin >> user.phone_no;
    cout << "enter your age" << endl;
    cin >> user.age;
    userIdToUser[user.userId] = user;
    cout << "Note down your user_id: " << user.userId << endl;
}

void accountDashboard(int acc_no)
{

    if (allAccounts.find(acc_no) == allAccounts.end())
    {
        cout << "account not found \n";
        return;
    }

    while (true)
    {

        int type = accTypes[acc_no];
        int flag;
        cout << endl;
        cout << "1. Deposit Amount \n 2. Withdraw Amount \n 3. Show Transaction History \n 4. View Account Details \n 5. View Atm details \n 6. Exit \n";
        cin >> flag;

        switch (flag)
        {

        case 1:
            if (type == 1)
                accIdtoSavAcc[acc_no].depositAmt();
            else if (type == 2)
                accIdtoCurrAcc[acc_no].depositAmt();
            break;

        case 2:
            if (type == 1)
                accIdtoSavAcc[acc_no].withdrawAmt();
            else if (type == 2)
                accIdtoCurrAcc[acc_no].withdrawAmt();
            break;

        case 3:
            if (type == 1)
                accIdtoSavAcc[acc_no].viewAllTransactions();
            else if (type == 2)
                accIdtoCurrAcc[acc_no].viewAllTransactions();
            break;

        case 4:
            if (type == 1)
                accIdtoSavAcc[acc_no].viewDetails();
            else if (type == 2)
                accIdtoCurrAcc[acc_no].viewDetails();

            break;
        case 5:
            if (type == 1)
                accIdtoSavAcc[acc_no].viewAtmDetails();
            else if (type == 2)
                accIdtoCurrAcc[acc_no].viewAtmDetails();

        case 6:
            break;
        }
    }
}

void LoanAccountDashboard(int acc_no)
{

    LoanAccount acc = accIdtoLoanAcc[acc_no];

    while (true)
    {
        cout << "\n 1. repay loan \n 2. show loan details \n 3. exit";
        int flag;
        cin >> flag;
        switch (flag)
        {
        case 1:
            acc.RepayLoan();
            break;
        case 2:
            acc.loanDetails();
            break;
        case 3:
            return;
        }
    }
}

void ExistingUser()
{
    while (true)
    {
        int user_id, flag;
        cout << "Enter user id" << endl;
        cin >> user_id;
        User user = userIdToUser[user_id];
        cout << "1. View Accounts \n2. Create Account \n3. Go to Accounts \n4. View personal Details \n5. Show totalLoanAmt \n6. exit \n";
        cin >> flag;
        if (flag == 1)
        {
            user.viewAllAccountDetails();

            cout << endl;
        }
        else if (flag == 2)
        {
            user.addAccount();
            userIdToUser[user_id] = user;
            cout << endl;
        }

        else if (flag == 3)
        {
            user.viewAllAccountDetails();

            cout << endl;

            int acc_no;
            cout << "Enter Account number " << endl;
            cin >> acc_no;
            if (accTypes[acc_no] == 3)
            {
                LoanAccountDashboard(acc_no);
            }
            else
            {
                accountDashboard(acc_no);
            }
        }
        else if (flag == 4)
        {
            user.viewPersonalDetails();
            cout << endl;
        }
        else if (flag == 5)
        {
            user.getAllLoanAmount();
            cout << endl;
        }
        else
        {
            return;
        }
    }
}

int main()
{
    int flag;

    while (true)
    {

        cout << "1. Existing User \n2. New User \n";

        cin >> flag;

        if (flag == 1)
        {
            ExistingUser();
        }
        else if (flag == 2)
        {
            newUserLogIn();
        }
    }
}
