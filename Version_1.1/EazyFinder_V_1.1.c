#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<limits.h>
#include<ctype.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<dos.h>
#include<unistd.h>

#define filePathLength 200 // the variables used to store the paths are given this length
#define maxCityLength 10 // max size given to city variable
#define maxPlaceLength 20 // This is the max size of the no. of characters allowed in places array (can be changed)
#define noOfVehicles 5 // number of vehicles
#define maxVehicleLength 6 // max size given to vehicles strings
#define couponLength 8 // length of each coupon 7 for coupon code and 1 for '\0'
#define maxUsernameLength 20 // maximum username length accepted
#define maxPasswordLength 16 // maximum password length accepted

void main();
void EazyFinderMenu(); // Shows the Menu and calls corresponding function
bool callSignupLogin(); // Calls SignUp_LogIn recursively based on user input
bool SignUp_LogIn(char ch); // Signup Login code
void passwordInput(char* password, char loginSignupCharacter, char passwordChangeCharacter); // Takes password input
long long int encryptPassword(char* password); // encrypts the password using the ASCII values of characters in password
bool isPasswordAccepted(char* password, char ch); // returns true if password is accepted else returns false
void formCity(); // Calls connectPlaces() function
void connectPlaces(int source, int destination, int distance); // Forms adjacency matrix (cityAdjMat) of the city
void displayMap(); // Displays the map (source destination distance)
void locationInput(); // Takes locations (source & destination) as input from user
void locationCheck(char* source, char* destination); // Checks if our service is available at those locations
void checkForCase(char* source, char* destination, int sourceIndex,
                    int destinationIndex, int case_);
void changeLocation(char source[], char destination[], int case_); // If needed asks for a location change
int SingleSourceShortestPath(int sourceIndex, int destinationIndex, int case_); // Dijkstra's Algorithm
void printRoute(int sourceIndex, int destinationIndex, int shortestPath[], int path[],
                int case_); // Prints the route and displays the cost
int inputID(); // Takes vehicle ID as input
void loadCurrentTime(); // Stores current time in hh:mm:ss string format in 'currentTime'
void loadCurrentDate(); // Stores today's date in dd/mm/yyyy string format in 'today'
void modeOfTransportBasedOnTraffic(int k, char route[][maxPlaceLength]); // Asks for mode of transport
void loadDetails(char source[], char destination[], float cost, char couponName[],
                 int couponDiscount, float totalCost); // Loads all the details of transaction in (username).txt file
void bookings(); // Contains Bookings code
void transactionHistory(); // Displays all the transaction histories of the user
void updateUsername(); // Let's user change the username
void passwordChange(); // Let's user to change the password
void accountDeletion(char case_); // Deletes the user's account as well as transaction history
void switchAccounts(); // Let's user to switch between accounts

// Global Variable Declarations
char username[maxUsernameLength], password[maxPasswordLength + 1]; // Credentials of the user
char city[maxCityLength]; // city variable is used to store the name of the city selected by the user
                    // in EazyFinderMenu(). We Declared it global to use this variable in loadDetails()
                    // function that stores the transaction info in the file

char **places; // places is a 2d array that stores names of places in the city
               // These values are read from (city).txt file
int noOfPlaces; // This value is calculated based on no. of places in (city).txt file
int **cityAdjMat; // cityAdjMat is the Adjaceny Matrix representation of city
int *metro; // metro stores availability of metro at a particular place (These values are stored in (city).txt file)

char vehicles[noOfVehicles][maxVehicleLength] = {"Bus", "Bike", "Auto", "Metro", "Cab"}; // Vehicles available
int costPerKM[noOfVehicles] = {2, 3, 4, 5, 6}; // This is the cost per km per vehicle in order

// Starting and Ending times of vehicles(except bus) are stored in start_time and end_time
// These values are read from availability-times.txt file in CitiesInfo folder
// Bus is available at anytime, so only noOfVehicles-1 size is given
char start_time[noOfVehicles-1][10], end_time[noOfVehicles-1][10];
int extra_cost[noOfVehicles-1]; // cost that must be applied when the user books service 
                         // before start time or after end time. These values are read
                         // from availability-times.txt file in CitiesInfo folder
char currentTime[9], today[11]; // This is the string in which current time is stored

typedef struct modeOfTransport{ // stores the mode of transport selected by the user
     char source[maxPlaceLength], destination[maxPlaceLength];
     char mot[maxVehicleLength];
     float cost;
} modeOfTransport;

// dirname - current working directory, db - database path, th - transaction history file path
char dirname[filePathLength], db[filePathLength], th[filePathLength];

// dirname is already set in main function
void loadDatabasePath(){
     strcpy(db, dirname);
     strcat(db, "\\LogInSignUpDatabase.txt");
}

void loadTransactionHistoryFilePath(){
     strcpy(th, dirname);
     strcat(th, "\\TransactionHistories\\");
     strcat(th, username);
     strcat(th, ".txt");
}

// Stores current time in hh:mm:ss string format in 'currentTime'
void loadCurrentTime(){
     time_t current_time;
     struct tm * time_info;
     time(&current_time);
     time_info = localtime(&current_time);
     strftime(currentTime, sizeof(currentTime), "%H:%M:%S", time_info);
}

// Stores today's date in dd/mm/yyyy string format in 'today'
void loadCurrentDate(){
     time_t current_time;
     time(&current_time);
     struct tm *local = localtime(&current_time);
     int date_ = local->tm_mday;	// get day of month (1 to 31)
	int month_ = local->tm_mon + 1;  // get month of year (0 to 11)
	int year_ = local->tm_year + 1900;   // get year since 1900
     char date[2], month[2], year[4];
    
     // convert date_, month_, year_ from string to integer and store in date, month, year respectively
     sprintf(date, "%d", date_);
     sprintf(month, "%d", month_);
     sprintf(year, "%d", year_);

     strcpy(today, date);
     strcat(today, "/");
     strcat(today, month);
     strcat(today, "/");
     strcat(today, year);
}

void loadDetails(char source[], char destination[], float cost, char couponName[], int couponDiscount, float totalCost){     
     FILE *userFilePointer = fopen(th, "a");
     fprintf(userFilePointer, "City: %s, From: %s, To: %s, Actual Cost: %0.2f, Coupon Code: %s, Discount: %d, Total Cost: %0.2f, Booking Date: %s, Booking Time: %s\n",
                    strupr(city), strupr(source), strupr(destination), cost, couponName, couponDiscount, 
                    totalCost, today, currentTime);
     fclose(userFilePointer);
}

int discount(float cost, modeOfTransport mot_array[], int l){
     int noOfCoupons = 0;
     char ch;
     // Counting no. of Coupons
     FILE *discountFilePointer = fopen("discountCoupons.txt", "r");
     while((ch = fgetc(discountFilePointer)) != EOF){
          if(ch == '\n')
               noOfCoupons++;
     }
     fclose(discountFilePointer);

     char couponName[noOfCoupons][couponLength];
     int couponDiscount[noOfCoupons], couponPrice[noOfCoupons];
     char scannedCouponName[couponLength];
     int scannedCouponDiscount, scannedCouponPrice;
     int i = 0;

     // Open discountCoupons.txt to read all coupon details
     discountFilePointer = fopen("discountCoupons.txt", "r");
     while(fscanf(discountFilePointer, "%s %d %d\n", scannedCouponName, &scannedCouponDiscount, &scannedCouponPrice) != EOF ){
          strcpy(couponName[i], scannedCouponName);
          couponDiscount[i] = scannedCouponDiscount;
          couponPrice[i] = scannedCouponPrice;
          i++;
     }
     fclose(discountFilePointer);

     bool couponApplicable = false, welcome = false;
     for(i = 1 ; i < noOfCoupons ; i++){
          if(cost >= couponPrice[i]){
               couponApplicable = true;
               break;
          }
     }

     FILE *userFilePointer = fopen(th, "r");
     // Checking if there are any previous transactions so that we get to know
     // whether we have to give the user WELCOME code
     if(fgetc(userFilePointer) == EOF)
          welcome = true;
     fclose(userFilePointer);

     // Run the code only if there are any coupons applicable
     if(couponApplicable){
          printf("Coupons Applicable for you are:\n");
          if(welcome){
               printf("Coupon Code: %s\tDiscount: %d%%\ton bookings of Rs.%d or above.\n",
                      couponName[0], couponDiscount[0], couponPrice[0]);
          }
          for(i = 1 ; i < noOfCoupons ; i++){
               if(cost >= couponPrice[i]) // If cost > coupon price required then show coupon
                    printf("Coupon Code: %s\tDiscount: %d%%\ton bookings of Rs.%d or above.\n",
                           couponName[i],couponDiscount[i],couponPrice[i]);
          }
          
          char couponCode[couponLength], applyCoupon;
          printf("Do you want to apply any coupon[Y/N]? ");
          scanf(" %c", &applyCoupon);
          if(tolower(applyCoupon) == 'y'){
               printf("Enter the Coupon Code: ");
               scanf("%s", couponCode);
               if((strcmp(couponCode, "WELCOME") == 0) && !welcome)
                    return 0;
               for(i = 0 ; i < noOfCoupons ; i++){
                    if((strcmp(couponCode, couponName[i]) == 0) && cost >= couponPrice[i]){
                         printf("Do You Want to Proceed[Y/N]? ");
                         scanf(" %c", &applyCoupon);
                         if(tolower(applyCoupon) == 'y'){
                              printf("Coupon Applied successfully\n");
                              float totalCost = cost - ((float)couponDiscount[i]/100)*cost;
                              printf("Total Cost After Applying Coupon: %.1f\n", totalCost);
                              
                              loadDetails(mot_array[0].source, mot_array[l-1].destination,
                                          cost, couponName[i], couponDiscount[i], totalCost);

                              return 1;
                         }
                    }
               }
          }
          else {
               printf("No Coupon Applied\nTotal Cost: %.1f\n", cost);
               return -1;
          }
     } 
     else {
          printf("No Coupons Applicable\n");
          return -1;
     }
     return 0;
}

void generateBill(modeOfTransport mot_array[], int l){
     int i;
     float cost = 0.0;
     char choice;
     printf("Mode of Transport      From\t\t    To\t\t     Price\n");
     for(i = 0 ; i < l ; i++){
          printf("%-20s %-20s %-20s %0.2f\n", mot_array[i].mot, mot_array[i].source, mot_array[i].destination, mot_array[i].cost);
          cost += mot_array[i].cost;
     }
     printf("Total Cost: %0.2f\n", cost);

     int discountSuccess = discount(cost, mot_array, l);
     while(discountSuccess == 0){
          printf("You might have entered an invalid coupon\n");
          char tryAgain;
          printf("Do you want to try again[Y/N]? ");
          scanf(" %c", &tryAgain);
          if(tolower(tryAgain) == 'y')
               discountSuccess = discount(cost, mot_array, l);
          else break;
     }
     if(discountSuccess == -1){
          loadDetails(mot_array[0].source, mot_array[l-1].destination,
                      cost, "-", 0, cost);
     }
}

// Takes the Vehicle ID as Input
int inputID(){
     int id;
     while(id < 1 || id > 5){
          scanf("%d", &id);
          if(id < 1 || id > 5){
               printf("Invalid ID\n");
               printf("Select a Mode of Transportation: ");
          }
     }
     return id;
}

// Asks for mode of transport
void modeOfTransportBasedOnTraffic(int routeLastIndex, char route[][maxPlaceLength]){
     int i, j, l = 0, id, extraCost, extraCostApplied[noOfVehicles], sourceIndex, destinationIndex;
     char trafficChoice, startTimeString[9], endTimeString[9];
     float traffic;
     modeOfTransport mot_array[routeLastIndex-1];

     char path[150];
     getcwd(path, sizeof(path));
     strcat(path, "\\CitiesInfo\\availability-times.txt");

     // Initializing start and end times from file
     i = 1; // As Bus is available at anytime
     FILE *availabilityTimePointer = fopen(path, "r");
     while(fscanf(availabilityTimePointer, "%s %s %d\n", startTimeString, endTimeString, &extraCost) != EOF){
          strcpy(start_time[i-1], startTimeString);
          strcpy(end_time[i-1], endTimeString);
          extra_cost[i-1] = extraCost;
          i++;
     }
     fclose(availabilityTimePointer);
     
     loadCurrentTime(); // Sets currentTime to the current time
     loadCurrentDate(); // Sets today to today's date in dd/mm/yyyy format

     if(routeLastIndex > 1){
          do{
               printf("Do You Want to Select Mode of Transportation based on Traffic [Y/N]: ");
               scanf(" %c", &trafficChoice);
               if(tolower(trafficChoice) != 'y' && tolower(trafficChoice) != 'n'){
                    printf("Please Select a Valid Option\n");
               }
          }while(tolower(trafficChoice) != 'y' && tolower(trafficChoice) != 'n');
     }

     extraCostApplied[0] = 0;
     // Display Cost Per Vehicle Table
     printf("\nCost Per Kilometer of Vehicles Available:\n");
     printf("Id\tVehicle \t Price \t   ExtraCost\n");
     printf("%d \t %s \t\t %d \t\t -\n", 1, vehicles[0], costPerKM[0]);
     for (i = 1 ; i < noOfVehicles; i++){ // 5 Vehicles
          printf("%d \t %s \t\t %d", i + 1, vehicles[i], costPerKM[i]);
          if(strcmp(currentTime, start_time[i-1]) <= 0 || strcmp(currentTime, end_time[i-1]) >= 0){
               printf(" \t\t %d\n", extra_cost[i-1]);
               extraCostApplied[i] = 1;
          } else {
               printf(" \t\t -\n");
               extraCostApplied[i] = 0;
          }
     }

     if(routeLastIndex > 1 && tolower(trafficChoice) == 'y'){
          srand(time(0));
          for(i = routeLastIndex ; i > 0 ; i--){
               traffic = (float)rand()/RAND_MAX;
               if(traffic == 0){
                    printf("\nThe route is Clear from %s to %s\n", route[i], route[i-1]);
               } else if(traffic > 0 && traffic <= 0.5){
                    printf("\nThere is Moderate Traffic from %s to %s\n", route[i], route[i-1]);
               } else if(traffic > 0.5 && traffic <= 1){
                    printf("\nThere is Heavy Traffic from %s to %s\n", route[i], route[i-1]);
               }
               for(j = 0 ; j < noOfPlaces ; j++){
                    if(strcmp(places[j], route[i]) == 0) sourceIndex = j;
                    if(strcmp(places[j], route[i-1]) == 0) destinationIndex = j;
               }
               printf("Enter the ID of Mode of Transport you Prefer: ");
               id = inputID();
               while(id == 4){
                    if(metro[sourceIndex] == 0 && metro[destinationIndex] == 0){
                         printf("Metro not Available at %s and %s\n", route[i], route[i-1]);
                    } else if(metro[sourceIndex] == 0){
                         printf("Metro not Available at %s\n", route[i]);
                    } else if(metro[destinationIndex] == 0){
                         printf("Metro not Available at %s\n", route[i-1]);
                    } else {
                         break;
                    }
                    printf("Please Select another mode of transport: ");
                    id = inputID();
               }
               strcpy(mot_array[l].source, route[i]);
               strcpy(mot_array[l].destination, route[i-1]);
               strcpy(mot_array[l].mot, vehicles[id-1]);
               if(extraCostApplied[id-1] == 0)
                    mot_array[l].cost = (SingleSourceShortestPath(sourceIndex, destinationIndex, 2))*costPerKM[id-1];
               else
                    mot_array[l].cost = (SingleSourceShortestPath(sourceIndex, destinationIndex, 2))*(costPerKM[id-1] + extra_cost[id-2]);
               l++;
          }
     } else {
          l = 1;
          traffic = (float)rand()/RAND_MAX;
          if(traffic == 0){
               printf("\nThe route is Clear from %s to %s\n", route[routeLastIndex], route[0]);
          } else if(traffic > 0 && traffic <= 0.5){
               printf("\nThere is Moderate Traffic from %s to %s\n", route[routeLastIndex], route[0]);
          } else if(traffic > 0.5 && traffic <= 1){
               printf("\nThere is Heavy Traffic from %s to %s\n", route[routeLastIndex], route[0]);
          }
          for(j = 0 ; j < noOfPlaces ; j++){
               if(strcmp(places[j], route[routeLastIndex]) == 0) sourceIndex = j;
               if(strcmp(places[j], route[0]) == 0) destinationIndex = j;
          }
          // TODO
          printf("Select a Mode of Transportation: ");
          id = inputID();
          while(id == 4){
               if(metro[sourceIndex] == 0 && metro[destinationIndex] == 0){
                    printf("Metro not Available at %s and %s\n", route[routeLastIndex], route[0]);
               } else if(metro[sourceIndex] == 0){
                    printf("Metro not Available at %s\n", route[routeLastIndex]);
               } else if(metro[destinationIndex] == 0){
                    printf("Metro not Available at %s\n", route[0]);
               } else {
                    break;
               }
               printf("Please Select another mode of transport: ");
               id = inputID();
          }
          strcpy(mot_array[0].source, route[routeLastIndex]);
          strcpy(mot_array[0].destination, route[0]);
          strcpy(mot_array[0].mot, vehicles[id-1]);
          if(extraCostApplied[id-1] == 0)
               mot_array[0].cost = (SingleSourceShortestPath(sourceIndex, destinationIndex, 2))*costPerKM[id-1];
          else
               mot_array[0].cost = (SingleSourceShortestPath(sourceIndex, destinationIndex, 2))*(costPerKM[id-1] + extra_cost[id-2]);
     }

     printf("\nYour Bill:\n");
     generateBill(mot_array, l);
}

// Prints the Route, displays the cost and case calls modeOfTransportBasedOnTraffic()
void printRoute(int sourceIndex, int destinationIndex, int shortestPath[], int path[], int case_){
     int i, j, k;
     char route[noOfPlaces][maxPlaceLength], trafficChoice;
     j = destinationIndex;
     strcpy(route[0], places[destinationIndex]);
     k = 1;
     while(path[j] != sourceIndex){
          strcpy(route[k], places[path[j]]);
          j = path[j];
          k++;
     }
     strcpy(route[k], places[sourceIndex]);
     
     // Display the Path
     printf("Route: ");
     for(i = k ; i >= 0 ; i--){
          strupr(route[i]);
          printf("%s", route[i]);
          if(i != 0) printf(" -> ");
     }
     printf("\nTotal Distance: %d\n", shortestPath[destinationIndex]);
     if(case_ != 0)
          modeOfTransportBasedOnTraffic(k, route);
}

// Dijkstra's Algorithm
int SingleSourceShortestPath(int sourceIndex, int destinationIndex, int case_){
     int i, j, k, shortestPath[noOfPlaces], visited[noOfPlaces], path[noOfPlaces], min_dist, min_dist_vertex;
    
     // Initializations
     for(i = 0 ; i < noOfPlaces ; i++){
          if(cityAdjMat[sourceIndex][i])
          shortestPath[i] = cityAdjMat[sourceIndex][i];
          else
          shortestPath[i] = INT_MAX;
          visited[i] = 0;
          path[i] = sourceIndex;
     }
     shortestPath[sourceIndex] = 0;

     for(i = 0 ; i < noOfPlaces ; i++){
          min_dist = INT_MAX;
          for(k = 0 ; k < noOfPlaces ; k++){
               // Select the vertices that are unvisited and are nearer to sourceVertex
               if(visited[k] == 0 && min_dist >= shortestPath[k]){
                    min_dist = shortestPath[k];
                    min_dist_vertex = k;
               }
          }

          // min_dist_vertex is at minimum distance to sourceVertex, so put it in solution
          visited[min_dist_vertex] = 1;

          for (j = 0; j < noOfPlaces; j++){
               if(!visited[j] && cityAdjMat[min_dist_vertex][j] && shortestPath[min_dist_vertex] != INT_MAX && 
               shortestPath[j] > shortestPath[min_dist_vertex] + cityAdjMat[min_dist_vertex][j]){
                    shortestPath[j] = shortestPath[min_dist_vertex] + cityAdjMat[min_dist_vertex][j];
                    path[j] = min_dist_vertex;
               }
          }
     }

     // case_ = 1 indicates we are good to go
     if(case_ == 1) {
          printRoute(sourceIndex, destinationIndex, shortestPath, path, case_);
     } else if(case_ == 2){ // When we require the shortest distance from source to destination then case_ = 2
          return shortestPath[destinationIndex];
     }
     return;
}

// Based on case_ prints the corresponding message and
// takes corresponding input
void changeLocation(char *source, char *destination, int case_){
     char choice;
     int i, j, distance;
     do{
          printf("Want to Change the Location(s) (Type 'N' to exit) [Y/N]: ");
          scanf(" %c", &choice);
          if(tolower(choice) != 'y' && tolower(choice) != 'n'){
               printf("Please Select a Valid Option\n");
          }
     }while(tolower(choice) != 'y' && tolower(choice) != 'n');
     if(tolower(choice) == 'y'){
          if(case_ == 1 || case_ == 4) {
               printf("Enter the Source Location Again: ");
               scanf("%s[^\n]",source);
               printf("Enter the Destination Again: ");
               scanf("%s[^\n]",destination);
          } else if(case_ == 2) {
               printf("Enter the Source Location Again: ");
               scanf("%s[^\n]",source);
          } else if(case_ == 3) {
               printf("Enter the Destination Again: ");
               scanf("%s[^\n]",destination);
          }
          // Check the location again after re-entering
          locationCheck(source, destination); // This calls checkForCase()
     } else {
          printf("Have A Great Day Ahead :)\n");
          exit(0);
     }
}

/* 
     Checks for the case based on source and destination:
     case 1: source and destination are wrong
     case 2: source is wrong
     case 3: destination is wrong
     case 4: source and destination same
     case 5: source and destination are correct
*/
void checkForCase(char* source, char* destination, int sourceIndex, int destinationIndex, int case_){
     if(case_ == 5){
          SingleSourceShortestPath(sourceIndex, destinationIndex, 1);
     } else if(case_ == 1) {
          printf("Sorry! Our Services are not available at %s and %s\nOr you have entered wrong locations. Please recheck the spellings\n", source, destination);
     } else if(case_ == 2) {
          printf("Sorry! Our Services are not available at %s\n", source);
     } else if(case_ == 3){
          printf("Sorry! We do not serve from %s to %s\nOr You have eneterd a wrong Destination\n", source, destination);
          printf("\nYou can Reach the following destinations from %s:\n", places[sourceIndex]);
     } else if(case_ == 4){
          printf("Source and Destination cannot be the same\n");
     }
     if(case_ != 5){
          changeLocation(source, destination, case_);
     }
}

// Checks if our service is available at inputted locations
void locationCheck(char *source, char *destination){
     int i, sourceIndex = -1, destinationIndex = -1, case_;

     // Checking for the index of source in places array
     for(i = 0 ; i < noOfPlaces ; i++){
          if(strcmp(source, places[i]) == 0){
               sourceIndex = i;
               break;
          }
     }

     // Checking for the index of destination in places array
     for(i = 0 ; i < noOfPlaces ; i++){
          if(strcmp(destination, places[i]) == 0){
               destinationIndex = i;
               break;
          }
     }

     if (sourceIndex == -1 && destinationIndex == -1) case_ = 1;
     else if (sourceIndex == -1) case_ = 2;
     else if (destinationIndex == -1) case_ = 3;
     else if (sourceIndex == destinationIndex) case_ = 4;
     else case_ = 5;
     checkForCase(source, destination, sourceIndex, destinationIndex, case_);
}

// Input Locations
void locationInput(){
     char source[maxPlaceLength], destination[maxPlaceLength];
     int i, j;
     printf("Enter the Starting Point: ");
     scanf("%s[^\n]",source);
     printf("Enter the Destination: ");
     scanf("%s[^\n]",destination);
     strcpy(source, strlwr(source));
     strcpy(destination, strlwr(destination));
     
     locationCheck(source, destination);
}

// Displays the Map
void displayMap(){
     int i, j;
     printf("\n From\t\t\t To\t\tDistance\n");
     for(i = 0 ; i < noOfPlaces ; i++){
          for(j = 0 ; j < noOfPlaces ; j++){
               if(cityAdjMat[i][j]){
                    printf("%-20s %-20s %-3d\n", places[i], places[j], cityAdjMat[i][j]);
               }
          }
          printf("\n");
     }
}

// Preparing the Adjacency Matrix
void connectPlaces(int source, int destinsation, int distance){
        cityAdjMat[source][destinsation] = cityAdjMat[destinsation][source] = distance;
}

void formCity(){ // As city variable is global, it may be passed as argument or not
     // Connections of Cities will be done here
     int sourceIndex, destinationIndex, distance;
     char path[100], cityTemp[10];
     getcwd(path, sizeof(path));
     strcat(path, "\\CitiesInfo\\");
     strcat(path, city);
     strcat(path, "-connections.txt");

     FILE *cityConnectionsPointer = fopen(path, "r");
     while(fscanf(cityConnectionsPointer, "%d %d %d\n", &sourceIndex, &destinationIndex, &distance ) != EOF)
          connectPlaces(sourceIndex, destinationIndex, distance);
     fclose(cityConnectionsPointer);
}

void bookings(){
     int i, j, cityChoice, metroAvailability;
     char place[maxPlaceLength], path[100], ch;
     FILE *cityPointer;
     printf("\n---------------------Welcome to EazyFinderMenu!!!!---------------------\n");
     printf("Select one of the City IDs:\n1) Hyderabad\n2) Bengaluru\n3) Chennai\n");
     scanf("%d", &cityChoice);
     do{
          switch(cityChoice){
               case 1: strcpy(city, "hyderabad"); // Hyderabad
                    break;
               case 2: strcpy(city, "bengaluru"); // Bengaluru
                    break;
               case 3: strcpy(city, "chennai"); // Chennai
                    break;
               default:printf("Please Select a valid Option\nSelect one of the City IDs: ");
                    scanf("%d", &cityChoice);
          }
     }while(cityChoice < 1 || cityChoice > 3);

     getcwd(path, sizeof(path)); // copies the current working directory into path string
     strcat(path, "\\CitiesInfo\\");
     strcat(path, city);
     strcat(path, ".txt");

     // Memory Allocation
     cityPointer = fopen(path, "r");
     noOfPlaces = 0;
     while((ch = fgetc(cityPointer)) != EOF){
          if(ch == '\n') noOfPlaces++; // Counting number of places in the city from file
     }

     // Allocating Memory for places array
     places = (char**) malloc(noOfPlaces * sizeof(char*));
     for(i = 0 ; i < noOfPlaces ; i++){
          places[i] = (char*) malloc(maxPlaceLength * sizeof(char));
     }

     // Allocating Memory for metro array
     metro = (int*) malloc(noOfPlaces * sizeof(int));

     // Allocating Memory for City Adjacency Matrix
     cityAdjMat = (int**) malloc(noOfPlaces * sizeof(int*));
     for(i = 0 ; i < noOfPlaces ; i++){
          cityAdjMat[i] = (int*) malloc(noOfPlaces * sizeof(int));
     }
     
     fclose(cityPointer); // Closing the File as the pointer reached EOF

     // Initializing places and metro arrays by taking data from file
     cityPointer = fopen(path, "r"); // Opening the file again
     i = 0;
     while(fscanf(cityPointer, "%s %d\n", place, &metroAvailability) != EOF){
          strcpy(places[i], place);
          metro[i] = metroAvailability;
          i++;
     }
     fclose(cityPointer); // Closing the file

     for(i = 0 ; i < noOfPlaces ; i++)
          for(j = 0 ; j < noOfPlaces ; j++)
               cityAdjMat[i][j] = 0; // Initializing the adj_mat

     formCity(); // Connects the Places in form of a graph and forms a city

     // Map of the corresponding city will be shown
     printf("\nMap:\n");
     displayMap();
     printf("\n");

     locationInput(); // There is a chain of function calls from this function
     
     free(metro);
     free(places);
     free(cityAdjMat);
}

// Shows the Transaction History of the user by reading the (usename).txt file
// in TransactionHistories folder
void transactionHistory(){
     char ch;
     FILE *userFilePointer = fopen(th, "r");
     bool empty = false;
     // Checking if their are any transaction histories for the user
     if((ch = fgetc(userFilePointer)) == EOF){
          printf("You have No Transactions yet\n");
          empty = true;
     }

     if(!empty){
          fseek(userFilePointer, 0, SEEK_SET); // Moving file pointer to beginning of file
          printf("Your Transaction History:\n");
          while((ch = fgetc(userFilePointer)) != EOF)
               printf("%c", ch);
          printf("\n");
     }
     fclose(userFilePointer); // Closing file
}

// Let's user to change the username
void updateUsername(){
     char newUsername[maxUsernameLength], scannedUsername[maxUsernameLength];
     long long int scannedPassword;
     bool found;
     FILE* adminFile;
     do {
          found = false;
          printf("Enter new Username: ");
          scanf("%s", newUsername);
          if(strcmp(username, newUsername) == 0){
               printf("New Username cannot be the same as Old one\n");
               found = true;
          } else {
               adminFile = fopen(db, "r");
               // Checking if Username is already present
               while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
                    if(strcmp(strlwr(scannedUsername), strlwr(newUsername)) == 0){
                         found = true;
                         printf("Username already taken, Try with Another one\n");
                         break;
                    }
               }
               fclose(adminFile);
          }
     } while(found); // Only comes out when the username is not used by anyone

     char newTHFileName[filePathLength];
     strcpy(newTHFileName, dirname); 
     strcat(newTHFileName, "\\TransactionHistories\\");
     strcat(newTHFileName, newUsername);
     strcat(newTHFileName, ".txt");

     if(!rename(th, newTHFileName)){
          accountDeletion('U');

          adminFile = fopen(db, "a");
          fprintf(adminFile, "%s %lld\n", newUsername, encryptPassword(password));
          fclose(adminFile);

          strcpy(username, newUsername);
          printf("\nUsername Changed Successfully\n\n");
     } else {
          printf("Error Occurred in Updating Username\n");
     }
}

// Deletes the account and transaction history of the user
void accountDeletion(char case_){
     char ch, scannedUsername[maxUsernameLength];
     long long scannedPassword;
     FILE* loginSignupPointer = fopen(db, "r");
     FILE* tempFilePointer = fopen("tempFile.txt", "w");
     while(fscanf(loginSignupPointer, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
          if(strcmp(scannedUsername, username) != 0)
               fprintf(tempFilePointer, "%s %lld\n", scannedUsername, scannedPassword);
     }
     fclose(loginSignupPointer);
     fclose(tempFilePointer);

     // Remove LogInSignUpDatabase.txt and rename tempFile.txt as LogInSignUpDatabase.txt
     if(!remove(db)){
          rename("tempFile.txt", "LogInSignUpDatabase.txt");
     }

     // If the function is not called from passwordChange() or updateUsername() function
     // then delete the transaction history, elde do not delete
     if(case_ != 'P' && case_ != 'U'){
          if(remove(th) == 0){
               printf("Account Deleted Successfully\nWe are Sorry to See you go :(\n");
          }
          exit(0);
     }
}

// Let's the user to change password
void passwordChange(){
     char newPassword[maxPasswordLength+1];
     bool new;
     do{
          new = true;
          printf("Enter New Password: ");
          passwordInput(newPassword, 'S', 'P');
          // Checks if the previous and new passwords are same
          if(strcmp(newPassword, password) == 0){
               printf("\nPassword Cannot be same as Previous One\n");
               new = false;
          } else {
               printf("\nPassword Changed Successfully\n");
          }
     } while(!new);
     strcpy(password, newPassword);
     accountDeletion('P'); // Delete the account from LogInSignUpDatabase.txt
                           // and add new credentials
     FILE* loginSignupPointer = fopen(db, "a");
     fprintf(loginSignupPointer, "%s %lld\n", username, encryptPassword(newPassword));
     fclose(loginSignupPointer);
     printf("\n");
}

void switchAccounts(){
     char newUsername[maxUsernameLength], newPassword[maxPasswordLength + 1];
     char choice, scannedUsername[maxUsernameLength];
     long long int scannedPassword;
     bool found = false;
     do {
          printf("Username: ");
          scanf("%s", newUsername);

          printf("Password: ");
          passwordInput(newPassword, 'L', 'A');
          printf("\n");

          if(strcmp(username, newUsername) != 0){
               FILE* adminFile = fopen(db, "r");
               while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
                    if((strcmp(scannedUsername, newUsername)) == 0 && scannedPassword == encryptPassword(newPassword)){
                         printf("\nSwitched Accounts Successfully\n");
                         strcpy(username, newUsername);
                         strcpy(password, newPassword);
                         found = true;
                         break;
                    }
               }
               fclose(adminFile);
               if(!found){
                    char newAccountChoice;
                    printf("No User with the Given Username\n");
                    printf("Want to Create an Account with the Username and then Switch [Y/N]? ");
                    scanf(" %c", &newAccountChoice);
                    if(tolower(newAccountChoice) == 'y' && SignUp_LogIn('S')){
                         printf("\nSwitched Accounts Successfully\n");
                         found = true;
                    }
               }
          } else {
               printf("Cannot switch with the same account\n");
          }

          if(!found){
               printf("Want to Try Again [Y/N]? ");
               scanf(" %c", &choice);
          }
     } while (!found && tolower(choice) == 'y');
}

void EazyFinderMenu(){
     char choice;
     int menuChoice;
     byte noOfChoices = 7;
     do{
          printf("\nChoose an Option:\n");
          printf("1) Bookings\n2) Transaction History\n3) Update Username\n4) Password Change\n5) Account Deletion\n6) Switch Accounts\n7) Logout\n");
          
          do{
               scanf("%d", &menuChoice);
               switch(menuChoice){
                    case 1:
                         bookings();
                         break;

                    case 2:
                         transactionHistory();
                         break;

                    case 3:
                         updateUsername();
                         loadTransactionHistoryFilePath();
                         break;

                    case 4:
                         passwordChange();
                         break;
                    
                    case 5:
                         printf("Are You Sure[Y/N]? ");
                         scanf(" %c", &choice);
                         if(tolower(choice) == 'y')
                              accountDeletion('A');
                         break;

                    case 6:
                         switchAccounts();
                         loadTransactionHistoryFilePath();
                         choice = 'y';
                         break;

                    case 7:
                         printf("Logged Out Successfully\n");
                         exit(0);
                    
                    default: printf("Invalid Option\nSelect an Option Again: ");
               } // switch ends here
          } while(menuChoice < 1 || menuChoice > noOfChoices);
          
          if(menuChoice != noOfChoices - 1){ // switch accounts is always kept at last but one place
               printf("Want to Select between Options Again [Y/N]? ");
               scanf(" %c", &choice);
          }

          if(tolower(choice) == 'n'){
               free(metro);
               free(places);
               free(cityAdjMat);
          }
     } while(tolower(choice) == 'y');
}

long long encryptPassword(char* password){
     long long int code = 0, a = 1;
     int i, len = strlen(password);
     for(i = 0 ; i < len ; i++){
          code += ((int)password[i]) * a;
          a *= 10;
     }
     return code;
}

bool isPasswordAccepted(char* password, char ch){
     int len = strlen(password);
     bool passwordAccepted = true;
     if(len < 8){
          printf("\nPassword is too Short. Use atleast 8 Characters");
          passwordAccepted = false;
     } else if(len > maxPasswordLength) {
          printf("\nPassword is too Long. Reduce it to a range of 8 to %d Characters", maxPasswordLength);
          passwordAccepted = false;
     }

     if(passwordAccepted){
          int i;
          bool lowerCaseUsed = false, upperCaseUsed = false, splCharUsed = false, numUsed = false;
          for(i = 0 ; i < len ; i++){
               if(isalpha(password[i])){
                    if(islower(password[i])) lowerCaseUsed = true;
                    else upperCaseUsed = true;
               }
               else if(isalnum(password[i])) numUsed = true;
               else splCharUsed = true;
          }

          if(!lowerCaseUsed) printf("\nAtleast One Lower Case Character must be Used");
          if(!upperCaseUsed) printf("\nAtleast One Upper Case Character must be Used");
          if(!numUsed) printf("\nAtleast One Number must be Used");
          if(!splCharUsed) printf("\nAtleast One Special Character must be Used");

          if(passwordAccepted && lowerCaseUsed && upperCaseUsed && numUsed && splCharUsed)
               passwordAccepted = true;
     }

     if(!passwordAccepted){
          if(ch != 'P')
               printf("\nPassword [Max of %d Characters]: ", maxPasswordLength);
          else {
               printf("\nEnter New Password: ");
          }
     }
     return passwordAccepted;
}

void passwordInput(char* password, char loginSignupCharacter, char passwordChangeCharacter){
     char ch;
     int i;
     do{
          i = 0;
          strcpy(password, "");
          while(true){
               ch = getch(); // getch() reads a key from keyboard without displaying it on screen and returns its ASCCI value
               if(ch == 13){ // if enter key
                    if(i == 0) printf("\nPassword Cannot be Empty");
                    break;
               } else if(ch == 32 || ch == 9){ // if space or tab key
                    continue;
               } else if(ch == 8){ // if backspace
                    if(i > 0){
                         i--;
                         password[i] = '\0';
                         printf("\b \b");
                    }
               } else{
                    if(i < maxPasswordLength){
                         password[i] = ch;
                         i++;
                         printf("*");
                    }
               }
          }
          password[i] = '\0';
          if(loginSignupCharacter == 'L') break;
     }while(!isPasswordAccepted(password, passwordChangeCharacter));
}

// SignUp and Login Code
bool SignUp_LogIn(char ch){
     char scannedUsername[maxUsernameLength];
     long long int scannedPassword;
     FILE *adminFile;
     int i = 0;

     if(ch == 'S'){
          bool found;
          do {
               found = false;
               printf("Username: ");
               scanf("%s", username);
               adminFile = fopen(db, "r");
               // Checking if Username is already present
               while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
                    if(strcmp(strlwr(scannedUsername), strlwr(username)) == 0){
                         found = true;
                         printf("Username already taken, Try with Another one\n");
                         break;
                    }
               }
               fclose(adminFile);
          } while(found); // Only comes out when the username is not used by anyone

          printf("Set Password [Max of %d Characters]: ", maxPasswordLength);
          passwordInput(password, 'S', 'A');

          adminFile = fopen(db , "a");
          fprintf(adminFile, "%s %lld\n", username, encryptPassword(password));
          fclose(adminFile);
          printf("\n\nAccount Created Successfully\n");
          
          loadTransactionHistoryFilePath(); // Loads the transaction history file path

          // Creation of file for user with username
          FILE *userFile = fopen(th, "w");
          fclose(userFile);
     } else {
          bool usernameFound, passwordCorrect;
          char tryAgain;
          do {
               usernameFound = false;
               passwordCorrect = true;
               printf("Username: ");
               scanf("%s", username);

               adminFile = fopen(db, "r");
               while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
                    // Check this if else - TODO
                    if((strcmp(scannedUsername, username)) == 0){
                         printf("Password: ");
                         passwordInput(password, 'L', 'A');
                         usernameFound = true;
                         if(scannedPassword == encryptPassword(password)){
                              passwordCorrect = true;
                              printf("\n\nLogged In Successfully\n");
                         } else if(strcmp(password, "") != 0) {
                              printf("\nPassword Incorrect\n");
                              passwordCorrect = false;
                         }
                    }
               }
               fclose(adminFile);

               if(!usernameFound){
                    printf("No User with given Username\n");
                    fclose(adminFile); // Closing file
               }
               if(!usernameFound || !passwordCorrect){
                    printf("Want to Try Again [Y/N]? ");
                    scanf(" %c", &tryAgain);
               }
               
               if(tolower(tryAgain) == 'n') {
                    printf("Have A Great Day Ahead! :)\n");
                    exit(0);
               }
          } while (tolower(tryAgain) == 'y' && !usernameFound || !passwordCorrect);
     }
}

bool callSignupLogin(){
     char SLChoice[7];
     printf("SignUp or LogIn: ");
     scanf("%s",SLChoice);
     if(strcmp(strlwr(SLChoice), "signup") == 0) {
          SignUp_LogIn('S');
     } else if(strcmp(strlwr(SLChoice), "login") == 0) {
          SignUp_LogIn('L');
     } else {
          printf("It's not a valid choice. Please type again\n");
          callSignupLogin();
     }
}

void main()
{
     getcwd(dirname, sizeof(dirname));
     loadDatabasePath(); // Loads the database's path
     callSignupLogin();
     loadTransactionHistoryFilePath(); // Loads the transaction history file path
     EazyFinderMenu();
     printf("Have A Great Day Ahead! :)\n");
}
