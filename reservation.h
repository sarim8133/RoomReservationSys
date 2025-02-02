#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#define Max_rooms 20
#define Max_bookings 20

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <stdlib.h>
typedef struct
{
    char room_no[100];
    int room_id;
    int capacity;
    int available;
} Room;

typedef struct
{
    char user[10];
    char admin_name[50];
    int participants;
    char time_slot[20];
    char date[11];
    Room rooms;
} Booking;

const struct
{
    const char *username;
    const char *password;
} CREDENTIALS[] = {
    {"admin", "admin123"},
    {"faculty", "faculty123"},
    {"student", "student123"}};
#endif
