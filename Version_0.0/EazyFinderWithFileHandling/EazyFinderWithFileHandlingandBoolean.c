#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<limits.h>
#include<ctype.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<unistd.h>

#define maxPlaceLength 20 // This is the max size of the no. of characters in places
#define noOfVehicles 5

bool callSignupLogin();
bool SignUp_LogIn(char ch);
void passwordInput(char* password, const int maxPasswordLength, char c);
long long int encryptPassword(char* password);
bool isPasswordAccepted(char* password, const int maxPasswordLength);
void formCity(char city[]);
void connect_places(int src, int dest, int distance);
void displayMap();
void EazyFinder();
void location_check(char* source, char* destination);
void check_for_case(char* source, char* destination, int source_index, int destination_index, int case_);
void change_location(char source[], char destination[], int case_);
int SingleSourceShortestPath(int source_index, int destination_index, int case_);
void printRoute(int source_index, int destination_index, int shortestPath[], int path[], int case_);
int inputID();
void currentTime();
void modeOfTransportBasedOnTraffic(int k, char route[][maxPlaceLength]);

int noOfPlaces;
char **places;
int **city_adj_mat, *metro;
char vehicles[][6] = {"Bus", "Bike", "Auto", "Metro", "Cab"};
int cost_per_km[] = {2, 3, 4, 5, 6};
char start_time[noOfVehicles-1][10], end_time[noOfVehicles-1][10]; // Bus is available everytime
int extra_cost[noOfVehicles];

typedef struct mode_of_transport{
     char source[maxPlaceLength], destination[maxPlaceLength], mst[6];
     int cost;
} mode_of_transport;

void generate_bill(mode_of_transport mst_array[], int l){
     int i, total_cost = 0;
     char choice;
     printf("Mode of Transport      From\t\t    To\t\t     Price\n");
     for(i = 0 ; i < l ; i++){
          printf("%-20s %-20s %-20s %d\n", mst_array[i].mst, mst_array[i].source, mst_array[i].destination, mst_array[i].cost);
          total_cost += mst_array[i].cost;
     }
     printf("Total Cost: %d\n", total_cost);
     printf("Do You Want to Continue [Y/N]: ");
     scanf(" %c", &choice);
     if(tolower(choice) == 'y'){
          EazyFinder();
     }
}

int inputID(){ // Takes the Vehicle ID as Input
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

char timeString[9];
void currentTime(){
     time_t current_time;
     struct tm * time_info;
     time(&current_time);
     time_info = localtime(&current_time);
     strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
}

void modeOfTransportBasedOnTraffic(int k, char route[][maxPlaceLength]){
     int i, j, l = 0, id, extraCost, extraCostApplied[noOfVehicles-1];
     char trafficChoice, startTimeString[9], endTimeString[9];
     float traffic;
     int source_index, destination_index;
     mode_of_transport mst_array[k-1];

     // Initializing start and end times from file
     extra_cost[0] = 0;
     i = 1; // As Bus is available all of the time
     FILE *availabilityTimePointer = fopen("P:\\EazyFinderProject\\CitiesInfo\\availability-times.txt", "r");
     while(fscanf(availabilityTimePointer, "%s %s %d\n", startTimeString, endTimeString, &extraCost) != EOF){
          //printf("Start: %s End: %s ExtraCost: %d\n",startTimeString,endTimeString,extraCost);
          strcpy(start_time[i-1], startTimeString);
          strcpy(end_time[i-1], endTimeString);
          extra_cost[i] = extraCost;
          i++;
     }
     fclose(availabilityTimePointer);
     
     currentTime(); // Sets timeString to the current time

     printf("Do You Want to Select Mode of Transportation based on Traffic [Y/N]: ");
     scanf(" %c", &trafficChoice);

     // Display Cost Per Vehicle Table
     printf("\nCost Per Kilometer of Vehicles Available:\n");
     printf("Id \t Vehicle \t Price \t   ExtraCost\n");
     printf("%d \t %s \t\t %d \t\t -\n", 1, vehicles[0], cost_per_km[0]);
     extraCostApplied[0] = 0;
     for (i = 1 ; i < noOfVehicles; i++){ // 5 Vehicles
          printf("%d \t %s \t\t %d", i + 1, vehicles[i], cost_per_km[i]);
          if(strcmp(timeString, start_time[i-1]) <= 0
             || strcmp(timeString, end_time[i-1]) >= 0){
                  printf(" \t\t %d\n", extra_cost[i]);
                  extraCostApplied[i] = 1;
          } else {
               printf(" \t\t -\n");
               extraCostApplied[i] = 0;
          }
     }

     if(tolower(trafficChoice) == 'y'){
          srand(time(0));
          for(i = k ; i > 0 ; i--){
               traffic = (float)rand()/RAND_MAX;
               if(traffic == 0){
                    printf("\nThe route is Clear from %s to %s\n", route[i], route[i-1]);
               } else if(traffic > 0 && traffic <= 0.5){
                    printf("\nTheir is Moderate Traffic from %s to %s\n", route[i], route[i-1]);
               } else if(traffic > 0.5 && traffic <= 1){
                    printf("\nTheir is Heavy Traffic from %s to %s\n", route[i], route[i-1]);
               }
               for(j = 0 ; j < noOfPlaces ; j++){
                    if(strcmp(places[j], route[i]) == 0) source_index = j;
                    if(strcmp(places[j], route[i-1]) == 0) destination_index = j;
               }
               printf("Enter the ID of Mode of Transport you Prefer: ");
               id = inputID();
               while(id == 4){
                    if(metro[source_index] == 0 && metro[destination_index] == 0){
                         printf("Metro not Available at %s and %s\n", route[i], route[i-1]);
                    } else if(metro[source_index] == 0){
                         printf("Metro not Available at %s\n", route[i]);
                    } else if(metro[destination_index] == 0){
                         printf("Metro not Available at %s\n", route[i-1]);
                    } else {
                         break;
                    }
                    printf("Please Select another mode of transport: ");
                    id = inputID();
               }
               strcpy(mst_array[l].source, route[i]);
               strcpy(mst_array[l].destination, route[i-1]);
               strcpy(mst_array[l].mst, vehicles[id-1]);
               if(extraCostApplied[id-1] == 0)
                    mst_array[l].cost = (SingleSourceShortestPath(source_index, destination_index, 2))*cost_per_km[id-1];
               else
                    mst_array[l].cost = (SingleSourceShortestPath(source_index, destination_index, 2))*(cost_per_km[id-1] + extra_cost[id-1]);
               l++;
          }
     } else {
          l = 1;
          for(j = 0 ; j < noOfPlaces ; j++){
               if(strcmp(places[j], route[k]) == 0) source_index = j;
               if(strcmp(places[j], route[0]) == 0) destination_index = j;
          }
          printf("Select a Mode of Transportation: ");
          id = inputID();
          strcpy(mst_array[0].source, route[k]);
          strcpy(mst_array[0].destination, route[0]);
          strcpy(mst_array[0].mst, vehicles[id-1]);
          if(extraCostApplied[id-1] == 0)
               mst_array[0].cost = (SingleSourceShortestPath(source_index, destination_index, 2))*cost_per_km[id-1];
          else
               mst_array[0].cost = (SingleSourceShortestPath(source_index, destination_index, 2))*(cost_per_km[id-1] + extra_cost[id-1]);
     }

     printf("\nYour Bill:\n");
     generate_bill(mst_array, l);
}

void printRoute(int source_index, int destination_index, int shortestPath[], int path[], int case_){
     int i, j, k;
     char route[noOfPlaces][maxPlaceLength], trafficChoice;
     j = destination_index;
     strcpy(route[0], places[destination_index]);
     k = 1;
     while(path[j] != source_index){
          strcpy(route[k], places[path[j]]);
          j = path[j];
          k++;
     }
     strcpy(route[k], places[source_index]);
     float traffic[k];
     
     // Display the Path
     printf("Route: ");
     for(i = k ; i >= 0 ; i--){
          printf("%s ", route[i]);
          if(i != 0) printf("->");
     }
     printf("\nTotal Distance: %d\n", shortestPath[destination_index]);
     if(case_ != 0)
          modeOfTransportBasedOnTraffic(k, route);
}

int SingleSourceShortestPath(int source_index, int destination_index, int case_){
     int i, j, k, shortestPath[noOfPlaces], visited[noOfPlaces], path[noOfPlaces], min_dist, min_dist_vertex;
     // Initializations
     for(i = 0 ; i < noOfPlaces ; i++){
          if(city_adj_mat[source_index][i])
            shortestPath[i] = city_adj_mat[source_index][i];
          else
            shortestPath[i] = INT_MAX;
          visited[i] = 0;
          path[i] = source_index;
     }
     shortestPath[source_index] = 0;

     for(i = 0 ; i < noOfPlaces ; i++){
          min_dist = INT_MAX;
          for(k = 0 ; k < noOfPlaces ; k++){
               // Select the vertices that are unvisited and are nearer to sourceVertex
               if(visited[k] == 0 && min_dist >= shortestPath[k]){
                    min_dist = shortestPath[k];
                    min_dist_vertex = k;
               }
          }

          // min_dist_vertex is at minimum distance to sourceVertex. So put it in solution
          visited[min_dist_vertex] = 1;

          for (j = 0; j < noOfPlaces; j++){
               if(!visited[j] && city_adj_mat[min_dist_vertex][j] && shortestPath[min_dist_vertex] != INT_MAX && 
                  shortestPath[j] > shortestPath[min_dist_vertex] + city_adj_mat[min_dist_vertex][j]){
                    shortestPath[j] = shortestPath[min_dist_vertex] + city_adj_mat[min_dist_vertex][j];
                    path[j] = min_dist_vertex;
               }
          }
    }
     if(case_ == 0){
          for (i = 0; i < noOfPlaces; i++){
               if(i != source_index)
                    printRoute(source_index, i, shortestPath, path, case_);
          }
     } else if(case_ == 1) {
          printRoute(source_index, destination_index, shortestPath, path, case_);
     } else if(case_ == 2){
          return shortestPath[destination_index];
     }
     return;
}

void change_location(char *source, char *destination, int case_){
     char choice;
     int i, j, distance;
     printf("Want to Change the Location(s) (Type 'N' to exit) [Y/N]: ");
     scanf(" %c", &choice);
     if(choice == 'Y' || choice == 'y'){
          if(case_ == 1) {
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
          location_check(source, destination); // This calls check_for_case
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
     case 4: source and destination are correct
*/
void check_for_case(char* source, char* destination, int source_index, int destination_index, int case_){
     if(case_ == 4){
          SingleSourceShortestPath(source_index, destination_index, 1);
     } else if(case_ == 1) {
          printf("Sorry! Our Services are not available at %s and %s\nOr you have entered wrong locations. Please recheck the spellings\n", source, destination);
     } else if(case_ == 2) {
          printf("Sorry! Our Services are not available at %s\n", source);
     } else if(case_ == 3){
          printf("Sorry! We do not serve from %s to %s\nOr You have eneterd a wrong Destination\n", source, destination);
          printf("\nYou can Reach the following destinations from %s:\n", places[source_index]);
          SingleSourceShortestPath(source_index, destination_index, 0);
     }
     if(case_ != 4){
          change_location(source, destination, case_);
     }
}

void location_check(char *source, char *destination){
     int i, source_index = -1, destination_index = -1, case_;

     // Checking for the index of source in places array
     for(i = 0 ; i < noOfPlaces ; i++){
          if(strcmp(source, places[i]) == 0){
               source_index = i;
               break;
          }
     }

     // Checking for the index of destination in places array
     for(i = 0 ; i < noOfPlaces ; i++){
          if(strcmp(destination, places[i]) == 0){
               destination_index = i;
               break;
          }
     }

     if (source_index == -1 && destination_index == -1) case_ = 1;
     else if (source_index == -1) case_ = 2;
     else if (destination_index == -1) case_ = 3;
     else case_ = 4;
     check_for_case(source, destination, source_index, destination_index, case_);
}

void EazyFinder(){
     char source[maxPlaceLength], destination[maxPlaceLength];
     int i, j;
     printf("Enter the Starting Point: ");
     scanf("%s[^\n]",source);
     printf("Enter the Destination: ");
     scanf("%s[^\n]",destination);
     strcpy(source, strlwr(source));
     strcpy(destination, strlwr(destination));
     
     location_check(source, destination);
}

void displayMap(){
     int i, j;
     printf(" From\t\t\t To\t\tDistance\n");
     for(i = 0 ; i < noOfPlaces ; i++){
          for(j = 0 ; j < noOfPlaces ; j++){
               if(city_adj_mat[i][j]){
                    printf("%-20s %-20s %-3d\n", places[i], places[j], city_adj_mat[i][j]);
               }
          }
          printf("\n");
     }
}

// Preparing the Adjacency Matrix
void connect_places(int source, int destinsation, int distance){
        city_adj_mat[source][destinsation] = city_adj_mat[destinsation][source] = distance;
}

void formCity(char city[]){
     // Connections of Cities will be done here
     int source_index, destination_index, distance;
     char path[100];
     getcwd(path, sizeof(path));
     strcat(path, "\\CitiesInfo\\");
     strcat(city, "-connections.txt");
     strcat(path, city);
     FILE *cityConnectionsPointer = fopen(path, "r");
     while(fscanf(cityConnectionsPointer, "%d %d %d\n", &source_index, &destination_index, &distance ) != EOF){
          connect_places(source_index, destination_index, distance);
     }
     fclose(cityConnectionsPointer);
}

bool isPasswordAccepted(char* password, const int maxPasswordLength){
     int len = strlen(password);
     bool ret;
     if(len < 8){
          printf("\nPassword is too Short. Use atleast 8 Characters");
          ret = false;
     } else if(len > maxPasswordLength) {
          printf("\nPassword is too Long. Reduce it to a range of 8 to %d Characters", maxPasswordLength);
          ret = false;
     }
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

     if(ret && lowerCaseUsed && upperCaseUsed && numUsed && splCharUsed)
          ret = true;
     if(!ret)
          printf("\nPassword [Max of %d Characters]: ", maxPasswordLength);
     return ret;
}

long long encryptPassword(char* password){
     long long int code = 0, a = 1;
     int i, len = strlen(password);
     for(i = 0 ; i < len ; i++){
          code += ((int)password[i]) * a;
          a *= 100;
     }
     return code;
}

void passwordInput(char* password, const int maxPasswordLength, char c){
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
          if(c == 'L') break;
     }while(!isPasswordAccepted(password, maxPasswordLength));
}

// SignUp and Login Code
bool SignUp_LogIn(char ch){
     const int maxPasswordLength = 16;
     char username[20], password[maxPasswordLength+1], scannedUsername[20];
     long long int scannedPassword;
     bool found = false;
     FILE *adminFile;
     int i = 0;

     if(ch == 'S'){
          do{
               printf("Username: ");
               scanf("%s", username);
               adminFile = fopen("LogInSignUpDatabase_Boolean.txt", "r");
               // Checking if Username is already present
               while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
                    //printf("\n\n%s %lld\n\n", scannedUsername, scannedPassword);
                    if(strcmp(scannedUsername, username) == 0){
                         found = true;
                         printf("Username already taken, Try with Another one\n");
                         break;
                    } else {
                         found = false;
                    }
               }
               fclose(adminFile);
          } while(found); // Only comes out when the username is not used by anyone

          printf("Set Password [Max of %d Characters]: ", maxPasswordLength);
          passwordInput(password, maxPasswordLength, 'S');
          printf("\n");

          adminFile = fopen("LogInSignUpDatabase_Boolean.txt" , "a");
          fprintf(adminFile, "%s %lld\n", username, encryptPassword(password));
          fclose(adminFile);
          printf("Account Created Successfully\n");
          return true;
     }

     else{
          printf("Username: ");
          scanf("%s", username);

          printf("Password: ");
          passwordInput(password, maxPasswordLength, 'L');
          printf("\n");

          adminFile = fopen("LogInSignUpDatabase_Boolean.txt", "r");
          while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
               if((strcmp(scannedUsername, username)) == 0 && scannedPassword == encryptPassword(password)){
                    printf("Logged In Successfully\n");
                    fclose(adminFile); // Closing file
                    return true;
               }
          }
          printf("No User with the Given Credentials\n");
          return false;
     }
}

bool callSignupLogin(){
     char SLChoice[7];
     printf("SignUp or LogIn: ");
     scanf("%s",SLChoice);
     if(strcmp(strlwr(SLChoice), "signup") == 0)
          return SignUp_LogIn('S');
     else if(strcmp(strlwr(SLChoice), "login") == 0)
          return SignUp_LogIn('L');
     else{
          printf("It's not a valid choice. Please type again\n");
          callSignupLogin();
     }
}

void main()
{
     int i, j, cityChoice, metroAvailability;
     char city[10], city1[10], place[maxPlaceLength], path[100];
     FILE *cityPointer;
     getcwd(path, sizeof(path)); // copies the current working directory into path string
     strcat(path, "\\CitiesInfo\\");

     // Variables used for Login Signup Code
     char choice, ch;
     bool returned;

     do{
          returned = callSignupLogin();
          if(returned){
               printf("\n---------------------Welcome to EazyFinder!!!!---------------------\n");
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

               strcpy(city1, city); //city1 is used as a parameter for formCity function, just like a container for city string
               strcat(city, ".txt"); // .txt is concatenated to get the full file name
               strcat(path, city); // city file name is concatenated to the path (cwd)

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
               city_adj_mat = (int**) malloc(noOfPlaces * sizeof(int*));
               for(i = 0 ; i < noOfPlaces ; i++){
                    city_adj_mat[i] = (int*) malloc(noOfPlaces * sizeof(int));
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
                         city_adj_mat[i][j] = 0; // Initializing the adj_mat
          
               formCity(city1); // Connects the Places in form of a graph and forms a city
               
               // Map of the corresponding city will be shown
               printf("\nMap:\n");
               displayMap();
               printf("\n");

               EazyFinder();

               choice = 'N';
          } else {
               printf("Want to Try Again [Y/N]? ");
               scanf(" %c", &choice); // If 'Y' the user will get a chance to again signup/login
          }
          if(choice == 'N' || choice == 'n')
               printf("Have A Great Day Ahead! :)\n");
     }while(choice == 'Y' || choice == 'y');
}
