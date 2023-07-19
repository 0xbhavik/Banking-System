#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>      // std::setprecision
#include <math.h> 
#include <vector>
using namespace std;

int _userId = 1;
int _account_no = 1000000;
int _atm_card_no = 1000000;
int _cvv = 100;
int _transaction_id  = 10000000;

class User;
class Account;
class SavingAccount;
class CurrentAccount;


// class loanAccount;


unordered_map<int, Account> acctIdToacc;
unordered_map<int, User> userIdToUser;
unordered_map<int, vector<Account> > userIdToAcc;
// unordered_map<int, int> accIdtoType;
unordered_map<int, SavingAccount> accIdtoSavAcc;
unordered_map<int, CurrentAccount> accIdtoCurrAcc;
// unordered_map<int, loanAccount> accIdtoLoanAcc;





// Declaring Account Class

class Account{
  public:
   int account_no;
   int user_id;
   int balance;    // Assumption: balance will always remain in 'int' format.
   int age_limit;
   string accType; 
   string account_creation_date;

    
   
    void showBalance(){
        cout << "your balance is " << balance <<endl;
    }

    int getBalance(){
        return this->balance;
    }
    void getAccountNumber(){
        cout<< "your Account number is "<< account_no <<endl;
    }
    
    void viewDetails(){
         cout << "Account number is " << this->account_no<<endl;
         cout << "your balance is " << this->balance <<endl;
         cout << "account creation date is " << this->account_creation_date <<endl;
         cout << "account type is " << this->accType<<endl;
    }

};

class Transaction{
  public:
  int transaction_id;
  int withdrawal_amt;
  string date;
  int type;  // 0-atm , 1-direct
  int account_id;
  int user_id;


  Transaction(int withdrawal_amt, string date,int type = -1){
    transaction_id = _transaction_id++;
    this->withdrawal_amt = withdrawal_amt;
    this->date = date;
    this->type = type;

  }
  
  void viewTransaction(){
     cout<<"transaction Id : " << transaction_id << endl;
     cout<<"withdrawal_amt : " << withdrawal_amt<< endl;
     cout<<"date : " << date << endl;
  }


};
class Atm{
   public:
   int card_no;
   int cvv;
   string expiry_date;

   Atm(){
    card_no = _atm_card_no++;
    cvv = _cvv++;
    expiry_date = "07/07/29" ;   // Update Required. hardcoding the date now.
   }
   
   void viewAtmDetails(){
    cout << "card_no : " << card_no << endl;
    cout << "expiry_date : " << expiry_date << endl;
    cout << "cvv : " << cvv << endl;
   }
};

class NonLoanAccount : public Account{
    public:
    int interest_rate;
    int NRV;
    int penaly_amt_NRV;
    Atm atm;
    int min_amt;
    vector<Transaction> transactions;



    void depositAmt(){
        cout<<"Enter amount you want to deposit"<<endl;
        int amt;
        cin>>amt;
        this->balance += amt;
        cout<<"deposited"<<endl;
    }

    void CheckNRV(){
        if(this->balance < NRV){
            cout<<"your NRV has fallen below minimum level" << penaly_amt_NRV << "rs will be deducted from your account"<<endl;
            this->balance -= penaly_amt_NRV;
        }
    }
    void viewAllTransactions(){
        for(int i=0;i<transactions.size();i++){
            transactions[i].viewTransaction();
        }
    }

};

class SavingAccount : public NonLoanAccount{
    public:
    int max_monthly_atm_withdrawal_limit;
    int max_single_transaction_withdrawal_amt;
    int max_daily_withdrawal_amt;
    int penalty_amt_atm_withdrawal;

    SavingAccount() {
        account_no = _account_no++;
        // user = newuser;
        interest_rate=6;
        NRV=100000;
        penaly_amt_NRV=1000;
        age_limit=0;
        // balance = bal;       
        min_amt = 10000;
        accType="saving";
        max_monthly_atm_withdrawal_limit = 5;
        max_single_transaction_withdrawal_amt=20000;
        max_daily_withdrawal_amt = 50000;
        penalty_amt_atm_withdrawal=500;
    }
    

    bool checkAccPossible(int bal){
        if(bal >= min_amt) return 1;
        else return 0;
    }

    bool checkDailyWithdrawalLimit(int amt,string date){
        // int today = getDay(date);
        // int n  = transactions.size();
        // int prevAmt = 0;
        // for(int i=n-1;i>=0;i--){
        //     // if transaction is direct, continue;
        //     if(transactions[i].type==1) continue;

        //     int txnDay = getDay(transactions[i].date);
        //     if(txnDay==today){
        //        prevAmt+=transactions[i].withdrawal_amt;
        //     }
        //     else{
        //         break;
        //     }
        // }
        // return 
        return 1;
    } 
    
    bool monthlyWithdrawalLimit(string date){

    }

    void withdrawAmt(){
        int flag, amt;
        string date;
        cout<<"Enter how would you like to withdraw your account\n press 1 for Atm withdrawal \n press 2 for direct withdrawal \n";
        cin>>flag;
        cout<<"How many amount you want to withdraw ?" <<endl;
        cin>>amt;
        cout<<"Enter Today's Date in 'DD/MM/YYYY' format" << endl;
        cin>>date;
        
        if(amt > max_single_transaction_withdrawal_amt) {
            cout << "can't withdraw entered amount is more than maximum single transaction amount"<<endl;
            return;
        }
        if(!checkDailyWithdrawalLimit(amt,date)){
            cout<< "Can't withdraw. Maximum of Rs 50000 can be withdrawn in a day"<<endl;
            return;
        }
      
      // atm withdrawal
      // Remaining: logic for atm pin authentication
        if(flag == 1){
            bool flag = monthlyWithdrawalLimit(date);
            if(flag){
             cout <<  "You have already made five atm transactions this month. thus this doing this transaction will cost you 500rs" <<endl;
             cout << "Press 1 for making transaction \n Press 2 for exit";
             int _flag; 
             cin>>_flag;
             if(_flag == 2) return;
             this->balance -= penalty_amt_atm_withdrawal;
            }
            this->balance -= amt;
             Transaction newtxn(amt, date, 0);
             transactions.push_back(newtxn);
             cout<<"Amount withdrawn";
        }
        else{
            this->balance -= amt;
             Transaction newtxn(amt, date, 1);
             transactions.push_back(newtxn);
             cout<<"Amount withdrawn";
        }

    }
};

class CurrentAccount : public NonLoanAccount{
   public:
   int transaction_percentage_charge;
   int max_charge__per_transaction;
   int penalty_amt_per_month;

   CurrentAccount(){
     account_no = _account_no++;
    //   this->user= newuser;
    //   this->account_creation_date = date;
    //   this->balance=bal;
      interest_rate=0;
      NRV=500000;
      penaly_amt_NRV=5000;
      accType="current";
      age_limit=18;
      transaction_percentage_charge=1;
      max_charge__per_transaction=500;
      penalty_amt_per_month=500;
   }

   void withdrawAmt(){
     int amt;
     string date;
    cout<<"How many amount you want to withdraw ?" <<endl;
    cin>>amt;
    cout<<"Enter Today's Date in 'DD/MM/YYYY' format" << endl;
    cin>>date;
    int charge = calculateTransactionCharge(amt);
    cout<<charge<<" rs will be deducted from account as transaction charges"<<endl;
    this->balance -= amt;
    this->balance -= charge;
    Transaction newtxn(amt, date);
    transactions.push_back(newtxn);
    cout<<"Amount withdrawn";
   }

      // calculating transaction charge 
   int calculateTransactionCharge(int amt){
      int amt1 = (amt*transaction_percentage_charge)/100;

      // returning whichever is less
      return amt1 <= max_charge__per_transaction ? amt1 : max_charge__per_transaction;
   }

   void checkMonthlyTransactions(){
     

   }

};

class User {
 public: 
  int userId;
  string name;
  string dob;
  int age;
  string phone_no;
  vector<int> savingAccountsId;
  vector<int> currentAccountsId;
//   vector<loanAccount> loanAccounts;

  User(){
    this->userId = _userId++;
  }
  
  void addAccount(){
     
     cout<< "which account type you want to create ?" << endl;
     cout<< "1. savings" << endl;
     cout<< "2. current" << endl;
     cout<< "3. loan" << endl;
     
     int type;
     cin >> type;
     
     cout<< "Enter amount you want to deposit" << endl;

     int amt;
     cin>>amt;

     cout<< "Enter today's date" << endl;
     
     string date;
     cin>>date;
    

     if(type == 1){
        SavingAccount account; 
        account.user_id = this->userId;
        account.balance=amt;
        account.account_creation_date=date;
        accIdtoSavAcc[account.account_no] = account;
          
         savingAccountsId.push_back(account.account_no);
         cout<< "your account no. is " << account.account_no << endl;
         cout<<"hello"<<savingAccountsId.size()<<endl;
        
     }
     else if(type == 2){
        
         CurrentAccount account;
         account.user_id = this->userId;
         account.balance = amt;
        account.account_creation_date=date;
         accIdtoCurrAcc[account.account_no] = account;

         currentAccountsId.push_back(account.account_no);
         cout<< "your account no. is " << account.account_no << endl;
     }
  }

  void viewAllAccountDetails(){

    cout<<"your saving account details are listed below" << endl;
     cout<<savingAccountsId.size();
    //  cout<<savingAccountsId[0];
     
    for(int i=0;i<savingAccountsId.size();i++){
        SavingAccount acc = accIdtoSavAcc[savingAccountsId[i]];
        acc.viewDetails();
    }
    cout<<"your current account numbers are listed below" << endl;
    for(int i=0;i<currentAccountsId.size();i++){
        CurrentAccount acc = accIdtoCurrAcc[currentAccountsId[i]];
        acc.viewDetails();
    }
    cout<<"your loan account numbers are listed below" << endl;
    // for(int i=0;i<loanAccounts.size();i++){
    //     cout<<loanAccounts[i].viewDetails()<<endl;

    // }
  }

  void viewTransactions(){
    int account_no;
    int type;
    cout<<"Enter Type of account \n press 1 for saving \n press 2 for current" << endl;
    cin>>type;
    cout<<"Enter account no. of account you want to view transactions of ";
    cin >> account_no;
    cout<<"your transaction details of account" << account_no << "are as follows" << endl;
    
    if(type == 1){
        // SavingAccount account =  accIdtoSavAcc[account_no];
        // account.viewAllTransactions();
    }
    else{
    //    CurrentAccount account = accIdtoCurrAcc[account_no];
        // account.viewAllTransactions();
    }
  }

  void viewPersonalDetails(){
      
      cout<< "your username is " << this->name << endl;
      cout<< "your phone number is " << this->phone_no << endl;
      cout<< "your date of birth is " << this->dob<<endl;
  }

  int TotalDeposit(){
        int amt = 0;
    
      
    for(int i=0;i<savingAccountsId.size();i++){
         SavingAccount acc = accIdtoSavAcc[savingAccountsId[i]];
        amt+=acc.getBalance();
    }
     for(int i=0;i<currentAccountsId.size();i++){
         CurrentAccount acc = accIdtoCurrAcc[currentAccountsId[i]];
        amt+=acc.getBalance();
    }

   
    return amt;
  }

};

class Loan { 
    
    int loan_type;
    int interest_rate;
    string loan_creation_date;
    int duration;
    int user_id;
    int acc_no;
    int loan_amt;

    Loan(int type, string date, int user_id ,int acc_no, int duration, int loan_amt){
        this->loan_type=type;
        this->loan_creation_date = date;
        this->acc_no = acc_no;
        this->user_id=user_id;
        this->duration = duration;
        // this->loan_amt;

        if(type == 0){
            this->interest_rate = 7;
        }
        else if(type == 1){
            this->interest_rate = 8;
        }
        else if(type == 2){
            this->interest_rate = 12;
        }
        else{
            this->interest_rate = 15;
        }
    }
};


class LoanAccount : public Account {

    int LoanAmt;
    int percent_total_deposit;
    int min_duration_year;
    int min_amt;
    User user;
    vector <Loan> loans;

    LoanAccount(User user,string date){
         this->account_no = _account_no++;
         this->LoanAmt = 0;
         this->percent_total_deposit=40;
         this->min_duration_year=2;
         this->min_amt=500000;
         this->age_limit=25;
         this->user = user;
         this->account_creation_date = date;
    }

    bool checkEligibility(int user_id, int loanAmt){
       
       int total_deposit_amt = userIdToUser[user_id].TotalDeposit();
       return loanAmt < ((total_deposit_amt*40)/100);

    }


    void takeLoan(){
        cout << "Enter loan Amo";
    }
    void RepayLoan(){
        
    }
      
};

// Global Functions

int makeUserId(string user){
    vector<int> v(26);
    v[0]=1;
    for(int i=1;i<26;i++){
        v[i] = (v[i-1]*2) + (i+1);
    }
    int cust_id = 0;
    for(int i = 0;i<user.size();i++){
            char c = tolower(user[i]);
            cust_id+=( v[c-'a']);
    }
    return cust_id;
}


void newUserLogIn(){

    User user;
    string name;
    cout << "enter your name" << endl;
    cin>>name;

    if(name.size()==0){
        return;
    }
    user.name = name;
    cout << "enter your phoneNumber" << endl;
    cin >> user.phone_no;
    cout << "enter your age" << endl;
    cin >> user.age;

    cout << "Note down your user_id: " << user.userId << endl;

    userIdToUser[user.userId] = user;
   
    

}



void ExistingUser(){
    
    while(true){
    int user_id,flag;
       cout << "Enter user id" << endl;
       cin >> user_id;
       User user =  userIdToUser[user_id];
       cout<<"this"<<user.userId<<endl;
       cout << "1. View Accounts \n2. Create Account \n3. Exit";
       cin>>flag;
       if(flag == 2){
          user.addAccount();
          cout<<"name" << userIdToUser[user_id].name;
          cout<<  userIdToUser[user_id].savingAccountsId.size();
          
       }
       else if(flag == 1){
         user.viewAllAccountDetails();
       }
       else { 
        return;
       }
}
}



int main(){
   
    int flag;
   while(true) {
   

        cout<< "1. Existing User \n2. New User";

   
     cin >> flag;
   

    if(flag == 1){
      ExistingUser();
        
   }
   else if(flag == 2){
      newUserLogIn();
   }

   }
   
  
}


