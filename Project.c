#include "reservation.h"

/*
admin login: admin , admin123
faculty login: faculty , faculty123
student login:student , student123
*/
void SaveRoomsToFile();
void SaveBookingsToFile();
Room *rooms = NULL;
int num_rooms = 0;
int bookingcount = 0;
Booking *bookings = NULL;

int adminLoggedIn = 0;
int facultyLoggedIn = 0;
int studentLoggedIn = 0;

void clearScreen()
{
    Sleep(1800);
    system("cls");
}

int login(const char *type, const char *username, const char *password)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char input_user[50], input_pass[50];

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("\n%s Login\n", type);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Username: ");
    scanf("%s", input_user);
    printf("Password: ");
    scanf("%s", input_pass);

    if (strcmp(input_user, username) == 0 && strcmp(input_pass, password) == 0)
    {
        printf("\n%s login successful!\n", type);
        clearScreen();
        return 1;
    }

    printf("\nInvalid %s credentials!\n", type);
    clearScreen();
    return 0;
}

int adminLogin()
{
    return login("Admin", CREDENTIALS[0].username, CREDENTIALS[0].password);
}

int facultyLogin()
{
    return login("Faculty", CREDENTIALS[1].username, CREDENTIALS[1].password);
}

int studentLogin()
{
    return login("Student", CREDENTIALS[2].username, CREDENTIALS[2].password);
}

void displayRoomHeader(HANDLE hConsole)
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("\n");
    printf(" _________________________________________________________________________________\n");
    printf("|                             AVAILABLE ROOM DETAILS                           	  |\n");
    printf("|_________________|_________________|_________________|___________________________|\n");
    printf("| %-15s | %-15s | %-15s | %-25s |\n", "Room Number", "Room ID", "Room Capacity", "Availability Status");
    printf("|_________________|_________________|_________________|___________________________|\n");
}

void displayAvailableRooms(const char *timeSlot, HANDLE hConsole)
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("\n%s SLOTS:\n", timeSlot);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf(" _________________________________________________________________________________\n");
    for (int i = 0; i < num_rooms; i++)
    {
        int isAvailable = 1;
        for (int j = 0; j < bookingcount; j++)
        {
            if (bookings[j].rooms.room_id == rooms[i].room_id && strcmp(bookings[j].time_slot, timeSlot) == 0)
            {
                isAvailable = 0;
                break;
            }
        }
        if (isAvailable)
        {
            printf("| %-15s | %-15d | %-15d | %-25s |\n",
                   rooms[i].room_no, rooms[i].room_id, rooms[i].capacity, "Available");
        }
    }
    printf("|_________________|_________________|_________________|___________________________|\n");
}

void display()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    displayRoomHeader(hConsole);

    const char *timeSlots[] = {"Morning", "Afternoon", "Evening"};
    for (int i = 0; i < 3; i++)
    {
        displayAvailableRooms(timeSlots[i], hConsole);
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

void displayBookingHeader()
{
    printf("%-5s | %-10s | %-20s | %-10s | %-12s\n",
           "ID", "Room ID", "User Name", "User Type", "Date");
    printf("________________________________________________________________\n");
}

void displayBookingsForTimeSlot(const char *timeSlot, HANDLE hConsole)
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("\n%s Bookings:\n", timeSlot);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    displayBookingHeader();

    for (int i = 0; i < bookingcount; i++)
    {
        if (strcmp(bookings[i].time_slot, timeSlot) == 0)
        {
            printf("%-5d | %-10d | %-20s | %-10s | %-12s\n",
                   i + 1,
                   bookings[i].rooms.room_id,
                   bookings[i].admin_name,
                   bookings[i].user,
                   bookings[i].date);
        }
    }
}

void displayAllBookings()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    if (!adminLoggedIn)
    {
        printf("\nOnly admin can view all bookings!\n");
        clearScreen();
        return;
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("\nAll Bookings:\n");

    const char *timeSlots[] = {"Morning", "Afternoon", "Evening"};
    for (int i = 0; i < 3; i++)
    {
        displayBookingsForTimeSlot(timeSlots[i], hConsole);
    }
}

void cancelBooking()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    if (bookingcount == 0)
    {
        printf("\nNo bookings to cancel.\n");
        clearScreen();
        return;
    }

    displayAllBookings();

    int bookingId;
    printf("\nEnter booking ID to cancel (1-%d): ", bookingcount);
    scanf("%d", &bookingId);
    getchar();

    if (bookingId < 1 || bookingId > bookingcount)
    {
        printf("Invalid booking ID!\n");
        clearScreen();
        return;
    }

    int idx = bookingId - 1;
    memmove(&bookings[idx], &bookings[idx + 1], (bookingcount - idx - 1) * sizeof(Booking));
    bookingcount--;

    bookings = realloc(bookings, bookingcount * sizeof(Booking));

    SaveRoomsToFile();
    SaveBookingsToFile();

    printf("Booking cancelled successfully!\n");
    clearScreen();
}

void SaveRoomsToFile()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    FILE *f_room = fopen("room.txt", "w");
    if (!f_room)
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("Unable to save room data");
        return;
    }

    for (int i = 0; i < num_rooms; i++)
    {
        fprintf(f_room, "%s %d %d %d\n", rooms[i].room_no, rooms[i].room_id,
                rooms[i].capacity, rooms[i].available);
    }
    fclose(f_room);
}

void LoadRoomsFromFile()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    FILE *f_room = fopen("room.txt", "r");
    if (!f_room)
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("Unable to load room data");
        return;
    }

    char line[256];
    num_rooms = 0;
    while (fgets(line, sizeof(line), f_room))
        num_rooms++;
    rewind(f_room);

    rooms = malloc(num_rooms * sizeof(Room));
    if (!rooms)
    {
        printf("Memory allocation failed!\n");
        fclose(f_room);
        return;
    }

    for (int i = 0; i < num_rooms; i++)
    {
        fscanf(f_room, "%s %d %d %d", rooms[i].room_no, &rooms[i].room_id,
               &rooms[i].capacity, &rooms[i].available);
    }

    fclose(f_room);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Rooms loaded successfully. Total rooms: %d\n", num_rooms);
}

void SaveBookingsToFile()
{
    FILE *file = fopen("bookings.txt", "w");
    if (!file)
    {
        printf("Error opening bookings file.\n");
        return;
    }

    for (int i = 0; i < bookingcount; i++)
    {
        fprintf(file, "%d %s %s %d %s %s\n",
                bookings[i].rooms.room_id,
                bookings[i].admin_name,
                bookings[i].user,
                bookings[i].participants,
                bookings[i].date,
                bookings[i].time_slot);
    }
    fclose(file);
}

int CheckBookingConflicts(int roomid, const char *date, const char *timeslot, const char *usertype)
{
    for (int i = 0; i < bookingcount; i++)
    {
        if (bookings[i].rooms.room_id == roomid &&
            strcmp(bookings[i].date, date) == 0 &&
            strcmp(bookings[i].time_slot, timeslot) == 0)
        {

            if (strcmp(usertype, "Faculty") == 0 && strcmp(bookings[i].user, "Student") == 0)
            {
                printf("Faculty has priority - overriding student booking.\n");
                memmove(&bookings[i], &bookings[i + 1], (bookingcount - i - 1) * sizeof(Booking));
                bookingcount--;
                bookings = realloc(bookings, bookingcount * sizeof(Booking));
                return 0;
            }

            if (strcmp(usertype, "Student") == 0 && strcmp(bookings[i].user, "Faculty") == 0)
            {
                printf("Cannot override faculty booking - faculty has priority.\n");
                return 1;
            }

            printf("Conflict detected: Room %d is already booked on %s during %s.\n",
                   roomid, date, timeslot);
            return 1;
        }
    }
    return 0;
}

int validateDate(const char *date)
{
    // Check basic format (DD-MM-YYYY)
    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-')
    {
        printf("Invalid date format. Please use DD-MM-YYYY format.\n");
        return 0;
    }

    // Extract day, month and year
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
               (date[8] - '0') * 10 + (date[9] - '0');

    // Basic range checks
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2023)
    {
        printf("Invalid date values. Day should be 1-31, month 1-12, and year 2023 or later.\n");
        return 0;
    }

    // Check days in month
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Adjust February for leap years
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        daysInMonth[2] = 29;

    if (day > daysInMonth[month])
    {
        printf("Invalid number of days for the given month.\n");
        return 0;
    }

    return 1;
}

void MakeBooking()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    if (adminLoggedIn)
    {
        printf("\nAdmin cannot make bookings. Only faculty and students can make bookings.\n");
        clearScreen();
        return;
    }

    char usertype[10], username[50], timeslot[20], date[11];
    int participants, roomid;

    strcpy(usertype, facultyLoggedIn ? "Faculty" : "Student");
    strcpy(username, facultyLoggedIn ? CREDENTIALS[1].username : CREDENTIALS[2].username);

    printf("Enter the number of participants: ");
    scanf("%d", &participants);
    getchar();

    do
    {
        printf("Enter the time slot (Morning/Afternoon/Evening): ");
        fgets(timeslot, sizeof(timeslot), stdin);
        timeslot[strcspn(timeslot, "\n")] = '\0';

        // Convert input to lowercase for case-insensitive
        for (int i = 0; timeslot[i]; i++)
        {
            timeslot[i] = tolower(timeslot[i]);
        }
    } while (strcmp(timeslot, "morning") &&
             strcmp(timeslot, "afternoon") &&
             strcmp(timeslot, "evening"));

    // Convert first letter to uppercase for consistency
    timeslot[0] = toupper(timeslot[0]);

    do
    {
        printf("Enter the date (format DD-MM-YYYY): ");
        fgets(date, sizeof(date), stdin);
        date[strcspn(date, "\n")] = '\0';
    } while (!validateDate(date));

    printf("Enter the room id: ");
    scanf("%d", &roomid);
    getchar();

    if (!CheckBookingConflicts(roomid, date, timeslot, usertype))
    {
        int roomIndex = -1;
        for (int i = 0; i < num_rooms; i++)
        {
            if (rooms[i].room_id == roomid)
            {
                roomIndex = i;
                break;
            }
        }

        if (roomIndex != -1 && rooms[roomIndex].capacity >= participants)
        {
            bookings = realloc(bookings, (bookingcount + 1) * sizeof(Booking));
            if (!bookings)
            {
                printf("Memory allocation failed!\n");
                return;
            }

            Booking *newBooking = &bookings[bookingcount];
            strcpy(newBooking->user, usertype);
            strcpy(newBooking->admin_name, username);
            newBooking->participants = participants;
            strcpy(newBooking->time_slot, timeslot);
            strcpy(newBooking->date, date);
            newBooking->rooms = rooms[roomIndex];
            bookingcount++;

            SaveBookingsToFile();
            printf("Booking successful!\n");
        }
        else
        {
            printf("Room does not exist or does not have enough capacity.\n");
        }
    }
    else
    {
        printf("Unable to make the booking due to a conflict.\n");
    }
    clearScreen();
}

int main()
{
    LoadRoomsFromFile();

    bookings = malloc(sizeof(Booking));
    if (!bookings)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    

    int choice;

    do
    {
        if (!adminLoggedIn && !facultyLoggedIn && !studentLoggedIn)
        {	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("\t\t  _____   ____   ____  __  __   _____  ______  _____ ______ _______      __  _______ _____ ____  _   _\n");
    printf("\t\t |  __ \\ / __ \\ / __ \\|  \\/  | |  __ \\|  ____|/ ____|  ____|  __ \\ \\    / /\\|__   __|_   _/ __ \\| \\ | |\n");
    printf("\t\t | |__) | |  | | |  | | \\  / | | |__) | |__  | (___ | |__  | |__) \\ \\  / /  \\  | |    | || |  | |  \\| |\n");
    printf("\t\t |  _  /| |  | | |  | | |\\/| | |  _  /|  __|  \\___ \\|  __| |  _  / \\ \\/ / /\\ \\ | |    | || |  | | . ` |\n");
    printf("\t\t | | \\ \\| |__| | |__| | |  | | | | \\ \\| |____ ____) | |____| | \\ \\  \\  / ____ \\| |   _| || |__| | |\\  |\n");
    printf("\t\t |_|__\\_\\\\____/_\\____/|_|__|_| |_|__\\_\\______|_____/|______|_|  \\_\\  \\/_/    \\_\\_|  |_____\\____/|_| \\_|\n");
    printf("\t\t  / ____\\ \\   / / ____|__   __|  ____|  \\/  |\n");
    printf("\t\t | (___  \\ \\_/ / (___    | |  | |__  | \\  / |\n");
    printf("\t\t  \\___ \\  \\   / \\___ \\   | |  |  __| | |\\/| |\n");
    printf("\t\t  ____) |  | |  ____) |  | |  | |____| |  | |\n");
    printf("\t\t |_____/   |_| |_____/   |_|  |______|_|  |_|\n\n\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("\nMAIN MENU\n");

            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("1. Admin Login\n2. Faculty Login\n3. Student Login\n4. Exit\n");

            printf("Enter your choice: ");
            scanf("%d", &choice);

            getchar();

            switch (choice)
            {
            case 1:
                adminLoggedIn = adminLogin();
                break;
            case 2:
                facultyLoggedIn = facultyLogin();
                break;
            case 3:
                studentLoggedIn = studentLogin();
                break;
            case 4:
                printf("Exiting program...\n");
                free(rooms);
                free(bookings);
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                clearScreen();
            }
        }
        else
        {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("\nOPTIONS MENU\n");
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("1. Display Available Rooms\n");
            if (!adminLoggedIn)
            {
                printf("2. Make a Booking\n");
            }
            if (adminLoggedIn)
            {
                printf("3. View All Bookings\n4. Cancel Booking\n");
            }
            printf("5. Logout\n6. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            getchar();

            switch (choice)
            {
            case 1:
                display();
                break;
            case 2:
                if (!adminLoggedIn)
                    MakeBooking();
                else
                {
                    printf("Invalid choice. Please try again.\n");
                    clearScreen();
                }
                break;
            case 3:
                if (adminLoggedIn)
                    displayAllBookings();
                else
                {
                    printf("Invalid choice. Please try again.\n");
                    clearScreen();
                }
                break;
            case 4:
                if (adminLoggedIn)
                    cancelBooking();
                else
                {
                    printf("Invalid choice. Please try again.\n");
                    clearScreen();
                }
                break;
            case 5:
                adminLoggedIn = facultyLoggedIn = studentLoggedIn = 0;
                printf("Logged out successfully!\n");
                clearScreen();
                break;
            case 6:
                printf("Exiting program...\n");
                free(rooms);
                free(bookings);
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                clearScreen();
            }
        }
    } while (1);

    free(rooms);
    free(bookings);
    return 0;
}
