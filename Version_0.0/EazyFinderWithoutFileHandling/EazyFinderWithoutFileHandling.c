#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>
#include<windows.h>
#include<conio.h>
#include<time.h>

#define maxPlaceLength 20
#define noOfPlaces sizeof(places)/sizeof(*places)
#define noOfVehicles 5

char SignUp_LogIn(char ch);
void formCity();
void connect_places(int src, int dest, int distance);
void displayMap();
void EazyFinder();
void location_check(char* source, char* destination);
void check_for_case(char* source, char* destination, int source_index, int destination_index, int case_);
void change_location(char source[], char destination[], int case_);
int SingleSourceShortestPath(int source_index, int destination_index, int case_);
void printRoute(int source_index, int destination_index, int shortestPath[], int path[], int case_);
int inputID();
void modeOfTransportBasedOnTraffic(int k, char route[][maxPlaceLength]);
void generate_bill(mode_of_transport mst_array[], int l);

//Take values from file
char places[][maxPlaceLength]={"lbnagar", "kothapet", "ameerpet", "chaithanyapuri", "vanasthalipuram"};
char vehicles[][6] = {"Bus", "Bike", "Auto", "Metro", "Cab"};
int cost_per_km[] = {5, 10, 20, 30, 50}, metro[noOfPlaces] = {1, 0, 1, 0, 1};
int city_adj_mat[noOfPlaces][noOfPlaces];

// Preparing the Adjacency Matrix
void connect_places(int source, int destinsation, int distance){
        city_adj_mat[source][destinsation] = city_adj_mat[destinsation][source] = distance;
}

void formCity(){
     // Connections of Cities will be done here
     connect_places(0, 1, 10);
	connect_places(0, 3, 30);
	connect_places(0, 4, 100);
	connect_places(1, 2, 50);
	connect_places(2, 3, 20);
	connect_places(2, 4, 10);
	connect_places(3, 4, 60);
}

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

void modeOfTransportBasedOnTraffic(int k, char route[][maxPlaceLength]){
     int i, j, l = 0, id;
     char trafficChoice;
     float traffic;
     int source_index, destination_index;
     mode_of_transport mst_array[k-1];

     printf("Want to Check the Traffic [Y/N]: ");
     scanf(" %c", &trafficChoice);

     // Display Cost Per Vehicle Table
     printf("\nCost Per Kilometer of Vehicles Available:\n");
     printf("Id \t Vehicle \t Price\n");
     for (i = 0; i < noOfVehicles; i++){ // 5 Vehicles
          printf("%d \t %s \t\t %d\n", i + 1, vehicles[i], cost_per_km[i]); 
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
                    printf("Select a Mode of Transportation: ");
                    id = inputID();
               }
               strcpy(mst_array[l].source, route[i]);
               strcpy(mst_array[l].destination, route[i-1]);
               strcpy(mst_array[l].mst, vehicles[id-1]);
               for(j = 0 ; j < noOfPlaces ; j++){
                    if(strcmp(places[j], route[i]) == 0) source_index = j;
                    if(strcmp(places[j], route[i-1]) == 0) destination_index = j;
               }
               mst_array[l].cost = (SingleSourceShortestPath(source_index, destination_index, 2))*cost_per_km[id-1];
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
          mst_array[0].cost = (SingleSourceShortestPath(source_index, destination_index, 2))*cost_per_km[id-1];
     }

     printf("Your Bill:\n");
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

// Signup and Login Code
char SignUp_LogIn(char ch){
     const int maxPasswordLength = 16;
     char username[20], password[maxPasswordLength+1], scannedUsername[20], scannedPassword[maxPasswordLength+1], c, found = '0';
     FILE *adminFile;
     int i = 0;
     if(ch == 'S'){
          do{
               printf("Username: ");
               scanf("%s", username);
               adminFile = fopen("LogInSignUpDatabase.txt", "r");
               while(fscanf(adminFile, "%s %s\n", scannedUsername, scannedPassword) != EOF){
                    if(strcmp(scannedUsername, username) == 0){
                         found = '1';
                         printf("Username already taken\n");
                         break;
                    } else {
                         found = '0';
                    }
               }
          } while(found != '0'); // Only comes out when the username is a different one

          printf("Set Password: ");
          while(1){
               ch = getch();
               if(ch == 13){ // if enter key
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
          printf("\n");

          fclose(adminFile);
          adminFile = fopen("LogInSignUpDatabase.txt" , "a");
          fprintf(adminFile, "%s %s\n", username, password);
          fclose(adminFile);
          printf("Account Created Successfully\n");
          return '1';
     }

     else{
          printf("Username: ");
          scanf("%s", username);
          printf("Password: ");
          while(1){
               ch = getch();
               if(ch == 13){ // if enter key
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
          printf("\n");
          adminFile = fopen("LogInSignUpDatabase.txt", "r");
          while(fscanf(adminFile, "%s %s\n", scannedUsername, scannedPassword) != EOF){
               if((strcmp(scannedUsername, username)) == 0 && strcmp(scannedPassword, password) == 0){
                    found = '1';
                    printf("Logged In Successfully\n");
                    return '1';
               }
          }
          fclose(adminFile);
          if(found == '0'){
               printf("No User with the Given Credentials\n");
               return '0';
          }
     }
}

char callSignupLogin(){
     char SLChoice[7], ret;
     printf("SignUp or LogIn: ");
     scanf("%s",SLChoice);
     if(strcmp(strlwr(SLChoice), "signup") == 0)
          return SignUp_LogIn('S');
     return SignUp_LogIn('L');
}

void main()
{
     char returned, choice;
     int i, j;
     char mapChoice;
     do{
          returned = callSignupLogin();
          if(returned == '1'){
               for(i = 0 ; i < noOfPlaces ; i++)
                    for(j = 0 ; j < noOfPlaces ; j++)
                         city_adj_mat[i][j] = 0; // Initializing the adj_mat
               formCity();    // Connects the Places in form of a graph and forms a city
               printf("Want To See the Map [Y/N]: ");
               scanf(" %c", &mapChoice);
               if(tolower(mapChoice) == 'y'){
                    printf("\nMap:\n");
                    displayMap();
               }
               printf("\n");
               EazyFinder();
               choice = 'N';
          } else {
               printf("Want to Try Again [Y/N]: ");
               scanf(" %c", &choice); // If 'Y' the user will get a chance to again signup/login
          }
          if(choice == 'N' || choice == 'n')
               printf("Have A Great Day Ahead! :)\n");
     }while(choice == 'Y' || choice == 'y');
}
