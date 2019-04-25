/* Solution to comp20005 Assignment 1, 2019 semester 1.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Flynn Harrison 992559
   Dated:     25/04/2019

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// Errors
/*Custom Error */
#define CU_ERROR        -1.0
/*Not Enough Charge*/
#define NE_CHARGE       -2.0 

// Settings
#define MAX_DELIVERYS   999
#define MAX_PAYLOAD     5.8 // Kg
#define DRONE_MASS      3.8 // Kg
#define DRONE_SPEED     4.2 // m/s

#define FULL_CHARGE     100.0 // %

#define ORIGIN_X        0
#define ORIGIN_Y        0

#define STAGE_1         "S1"
#define STAGE_2         "S2"
#define STAGE_3         "S3"

typedef struct 
{
    // Position m
    double x,y;
    // Mass Kg
    double mass;
    // Is delivered
    bool is_delivered;
}item_t;

// Prototypes 
/* Prints the first and last elements of item_t array*/
void print_first_last(char *stage,item_t list[], int size);
/* Sums up the masses in a item_t array */
double item_m_sum(item_t *list, int size);
/* distance from point to point */
double distance_ptp(double _x1, double _y1, double _x2, double _y2);
/* distance from origin */
double distance_o(double _x, double _y);
/* Returns charge used as decimal.
 mass is not including drone weight*/
double charge_used(double distance, double mass);
/* Stage ouput */
void stage_output(char *s, int i, double distance, 
    double bat_out, double bat_ret);
/* gets new battery */
void bat_new(char *s, double *charge);
/* checks if charge can be drawn out of battery */
int bat_check(double charge, double bat_out, double bat_ret);
/* battery check, returns 1 if change battery */
int bat_check_change(char *s, double *charge, double bat_out, double bat_ret);
/* vailidate package */
void package_check(double mass, double max_mass);
/* Gets item_t stats from element i of the list */
void item_stats(item_t *list, int i, double *dis, double *b_out, double *b_ret);
/* Prints out Stage/fligh stats */
void print_stage_stats(char *s, int num_bat, double dis, double velocity);

int main(int argc, char **argv)
{
    // Stage 1, get input
    item_t load[MAX_DELIVERYS];
    int lenght = 0;
    double x_buff, y_buff, m_buff;
    // Eat first line
    while(getchar() != '\n'){}

    for(;(scanf("%lf %lf %lf", &x_buff, &y_buff, &m_buff) == 3) 
            && lenght<MAX_DELIVERYS; lenght++)
    {
        load[lenght].x = x_buff;
        load[lenght].y = y_buff;
        load[lenght].mass = m_buff;
        load[lenght].is_delivered = false;
    }
    
    print_first_last(STAGE_1, load, lenght);
    printf("%s, total to deliver: %3.2lf kg\n",
        STAGE_1, item_m_sum(load, lenght));
    puts("");

    // Stage 2
    // Loop through all the packages
    double charge = FULL_CHARGE;
    double total_d = 0.0;
    int num_bat = 1;
    int i;
    for (i=0; i<lenght; i++)
    {
        // Find distance from origin & power usage
        double distance = 0.0;
        double bat_out = 0.0;
        double bat_ret = 0.0;

        item_stats(load, i, &distance, &bat_out, &bat_ret);
        
        // Check remaining charge & stage stats
        num_bat += bat_check_change(STAGE_2, &charge, bat_out, bat_ret);
        total_d += 2*distance;

        // Print S2
        stage_output(STAGE_2, i, distance, bat_out, bat_ret);
    }
    print_stage_stats(STAGE_2, num_bat, total_d, DRONE_SPEED);

    puts("");
    // Stage 3
    charge = FULL_CHARGE;
    total_d = 0.0;
    num_bat = 1;
    for (i = 0; i < lenght; i++)
    {
        int j;
        bool new_bat_flag = false;
        for (j=i; j<lenght; j++)
        {
            if (load[j].is_delivered == false)
            {
                double distance = 0.0;
                double bat_out = 0.0;
                double bat_ret = 0.0;

                item_stats(load, j, &distance, &bat_out, &bat_ret);      

                // Check if the drone can also take load
                if (bat_check(charge, bat_out, bat_ret))
                {
                    load[j].is_delivered = true;
                    total_d += 2*distance;
                    // just used to increment charge 
                    bat_check_change(STAGE_3, &charge, bat_out, bat_ret); 
                    stage_output(STAGE_3, j, distance, bat_out, bat_ret);
                    new_bat_flag = false;
                }
                else
                {
                    new_bat_flag = true;
                }
                
            }
        }
        // Get new battery
        if (new_bat_flag)
        {
            bat_new(STAGE_3, &charge);
            num_bat ++;            
        }
    }
    print_stage_stats(STAGE_3, num_bat, total_d, DRONE_SPEED);
    
    return 1;
}

void print_first_last(char *stage, item_t *list, int size)
{
    printf("%s, total data lines: %3d\n",stage, size);
    // First and last elements of array
    printf("%s, first data line :  x=%6.1lf, y=%6.1lf, kg=%.2lf\n",
        stage, list[0].x, list[0].y, list[0].mass);
    printf("%s, final data line :  x=%6.1lf, y=%6.1lf, kg=%.2lf\n",
        stage, list[size-1].x, list[size-1].y, list[size-1].mass);
}

double item_m_sum(item_t *list, int size)
{
    double sum = 0;
    // Sum up masses from .mass element
    int i;
    for (i = 0; i<size; i++)
    {
        sum += list[i].mass;
    }
    return sum;
}

double distance_ptp(double _x1, double _y1, double _x2, double _y2)
{ 
    // Pythag
    return sqrt(pow(_x2-_x1, 2) + pow(_y2-_y1, 2));
}

double distance_o(double _x, double _y)
{
    // distance from origin
    return distance_ptp(ORIGIN_X, ORIGIN_Y, _x, _y);
}

double charge_used(double distance, double mass)
{
    double charge = 0.0;
    charge = 6300.0/(DRONE_MASS + mass);
    charge = (distance/charge) *100.0;
    return charge;
}

void stage_output(char *s, int i, double distance, 
    double bat_out, double bat_ret)
{
    printf("%s, package=%3d, distance=%6.1lfm, ", s, i, distance);
    printf("battery out=%4.1lf%%, battery ret=%4.1lf%%\n", bat_out, bat_ret);
}

int bat_check_change(char *s, double *charge, double bat_out, double bat_ret)
{
    // make sure its a valid package
    if (bat_out + bat_ret > FULL_CHARGE)
    {
        printf("delivery to heavy and/or to far!\n");
        exit(0);
    }

    *charge -= bat_out + bat_ret;
    if (*charge <= 0)
    {
        // need new battery
        bat_new(s, charge);
        //update with current charge
        *charge -= bat_out + bat_ret;
        return 1;
    }

    return 0;
}

void package_check(double mass, double max_mass)
{
    if (mass > max_mass)
    {
        printf("delivery to heavy and/or to far!\n");
        exit(0); 
    }
}

void print_stage_stats(char *s, int num_bat, double dis, double velocity)
{
    int time = round(dis/velocity);
    printf("%s, total batteries required:%4d\n",s, num_bat);
    printf("%s, total flight distance=%.1lf meters, ", s, dis);
    printf("total flight time=%4d seconds\n", time);
}

void bat_new(char *s, double *charge)
{
    printf("%s, change the battery\n", s);
    *charge = FULL_CHARGE;
}

int bat_check(double charge, double bat_out, double bat_ret)
{
    // does the battery have enough charge to complete
    // the opertation
    if (charge - (bat_out + bat_ret) < 0)
    {
        return 0;
    }
    return 1;
}

void item_stats(item_t *list, int i, double *dis, double *b_out, double *b_ret)
{
    *dis = distance_o(list[i].x, list[i].y);
    *b_out = charge_used(*dis, list[i].mass);
    *b_ret = charge_used(*dis, 0);
    package_check(list[i].mass, MAX_PAYLOAD);
}