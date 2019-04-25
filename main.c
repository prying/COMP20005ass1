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

// Settings
#define MAX_DELIVERYS       999

#define DRONE_MAX_PAYLOAD   5.8 // Kg
#define DRONE_MASS          3.8 // Kg
#define DRONE_SPEED         4.2 // m/s

#define FULL_CHARGE         100.0 // %

#define ORIGIN_X            0
#define ORIGIN_Y            0

#define STAGE_1             "S1"
#define STAGE_2             "S2"
#define STAGE_3             "S3"
#define STAGE_4             "S4"

typedef struct 
{
    // Position m
    double x,y;
    // Mass Kg
    double mass;
    // Is delivered flag
    bool is_delivered;
}item_t;

// Prototypes 
double  item_mass_sum(item_t *list, size_t size);
void    item_stats(item_t *list, int i, double dis, 
                    double *b_out, double *b_ret);
double  distance_ptp(double _x1, double _y1, double _x2, double _y2);
double  distance_o(double _x, double _y);
void    stage_output(char *s, int i, double dis, double b_out, double b_ret);  
void    bat_change(char *s, double *charge);
int     bat_check(double charge, double b_out, double b_ret);
int     bat_check_change(char *s, double *charge, 
                    double b_out, double b_ret);
double  bat_charge_used(double distance, double mass);
void    weight_check(double mass, double max_mass);
void    print_first_last(char *s, item_t list[], size_t size);
void    print_stage_stats(char *s, int num_bat, double dis, double velocity);
void    print_seq_order(char*s, item_t *list, size_t size);
void    print_non_seq_order(char* s, item_t *list, 
                    size_t size, double _x, double _y);
double  centroid_x(item_t *list,size_t size);
double  centroid_y(item_t *list,size_t size);

int main(int argc, char **argv)
{
    // Stage 1
    item_t  items[MAX_DELIVERYS];
    int     num_items = 0;
    double  x_buff = 0.0;
    double  y_buff = 0.0;
    double  m_buff = 0.0;

    // Eat first line of x y KG
    while(getchar() != '\n'){}

    for(;(scanf("%lf %lf %lf", &x_buff, &y_buff, &m_buff) == 3) 
            && num_items<MAX_DELIVERYS; num_items++)
    {
        items[num_items].x = x_buff;
        items[num_items].y = y_buff;
        items[num_items].mass = m_buff;
        items[num_items].is_delivered = false;
    }
    
    print_first_last(STAGE_1, items, num_items);
    printf("%s, total to deliver: %3.2lf kg\n",
        STAGE_1, item_mass_sum(items, num_items));

    puts("");
    // Stage 2
    print_seq_order(STAGE_2, items, num_items);

    puts("");
    // Stage 3
    print_non_seq_order(STAGE_3, items, num_items, ORIGIN_X, ORIGIN_Y);

    puts("");
    // Stage 4
    // Get centroid
    double x_h = centroid_x(items, num_items);
    double y_h = centroid_y(items, num_items);
    printf("%s, centroid location x=%6.1lfm, y= %6.1lfm\n",STAGE_4, x_h, y_h); 

    // Reset is_delivered flag
    int i;
    for (i = 0; i<num_items;i++)
    {
        items[i].is_delivered = false;
    }

    print_non_seq_order(STAGE_4, items, num_items, x_h, y_h);

    return 0;
}

/* Sums up the masses in a item_t array */
double item_mass_sum(item_t *list, size_t size)
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

/* Gets item_t stats from element i of the list */
void item_stats(item_t *list, int i, double dis, double *b_out, double *b_ret)
{
    *b_out = bat_charge_used(dis, list[i].mass);
    *b_ret = bat_charge_used(dis, 0);
    weight_check(list[i].mass, DRONE_MAX_PAYLOAD);
}
/* Distance from point to point */
double distance_ptp(double _x1, double _y1, double _x2, double _y2)
{ 
    return sqrt(pow(_x2-_x1, 2) + pow(_y2-_y1, 2));
}

/* Distance from origin */
double distance_o(double _x, double _y)
{
    return distance_ptp(ORIGIN_X, ORIGIN_Y, _x, _y);
}

/* Stage ouput */
void stage_output(char *s, int i, double dis, double b_out, double b_ret)
{
    printf("%s, package=%3d, distance=%6.1lfm, ", s, i, dis);
    printf("battery out=%4.1lf%%, battery ret=%4.1lf%%\n", b_out, b_ret);
}

/* Gets new battery */
void bat_change(char *s, double *charge)
{
    printf("%s, change the battery\n", s);
    *charge = FULL_CHARGE;
}

/* Checks if charge can be drawn out of battery, returns 1 if enough charge.
 100% usage will return 1*/
int bat_check(double charge, double b_out, double b_ret)
{
    // does the battery have enough charge to complete the opertation
    if (charge - (b_out + b_ret) < 0)
    {
        return 0;
    }
    return 1;
}

/* Battery check, returns 1 if change battery */
int bat_check_change(char *s, double *charge, double b_out, double b_ret)
{
    // make sure its a valid package
    if (b_out + b_ret > FULL_CHARGE)
    {
        printf("delivery to heavy and/or to far!\n");
        exit(0);
    }

    *charge -= b_out + b_ret;
    if (*charge <= 0)
    {
        bat_change(s, charge);
        //update with current charge
        *charge -= b_out + b_ret;
        return 1;
    }

    return 0;
}

/* Returns charge used as decimal. mass is not including drone weight*/
double bat_charge_used(double distance, double mass)
{
    double charge = 0.0;
    charge = 6300.0/(DRONE_MASS + mass);
    charge = (distance/charge) *100.0;
    return charge;
}

/* Vailidate package */
void weight_check(double mass, double max_mass)
{
    if (mass > max_mass)
    {
        printf("Item mass over the max mass!\n");
        exit(0); 
    }
}

/* Prints the first and last elements of item_t array*/
void print_first_last(char *s, item_t *list, size_t size)
{
    printf("%s, total data lines: %3lu\n",s, size);
    // First and last elements of array
    printf("%s, first data line :  x=%6.1lf, y=%6.1lf, kg=%.2lf\n",
        s, list[0].x, list[0].y, list[0].mass);
    printf("%s, final data line :  x=%6.1lf, y=%6.1lf, kg=%.2lf\n",
        s, list[size-1].x, list[size-1].y, list[size-1].mass);
}

/* Prints out Stage/fligh stats */
void print_stage_stats(char *s, int num_bat, double dis, double velocity)
{
    int time = round(dis/velocity);
    printf("%s, total batteries required:%4d\n",s, num_bat);
    printf("%s, total flight distance=%.1lf meters, ", s, dis);
    printf("total flight time=%4d seconds\n", time);
}

/* Outputs list in order for delivery in sequential order*/
void print_seq_order(char*s, item_t *list, size_t size)
{
    double  charge  = FULL_CHARGE;
    double  total_d = 0.0;
    int     num_bat = 1;

    int i;
    for (i=0; i<size; i++)
    {
        // Find distance from origin & power usage
        double distance = 0.0;
        double bat_out  = 0.0;
        double bat_ret  = 0.0;

        distance = distance_o(list[i].x, list[i].y);
        item_stats(list, i, distance, &bat_out, &bat_ret);
        
        // Check remaining charge & distance stats
        num_bat += bat_check_change(s, &charge, bat_out, bat_ret);
        total_d += 2*distance;

        stage_output(s, i, distance, bat_out, bat_ret);
    }
    print_stage_stats(s, num_bat, total_d, DRONE_SPEED);    
}

/* Outputs list in order for delivery in non-sequential order*/
void print_non_seq_order(char *s,item_t *list, size_t size, 
                double _x, double _y)
{
    double  charge  = FULL_CHARGE;
    double  total_d = 0.0;
    int     num_bat = 1;

    int i;
    for (i = 0; i < size; i++)
    {
        bool new_bat_flag = false;
        int j;
        for (j=i; j<size; j++)
        {
            if (list[j].is_delivered == false)
            {
                double distance = 0.0;
                double bat_out = 0.0;
                double bat_ret = 0.0;

                distance = distance_ptp(list[j].x, list[j].y, _x, _y);
                item_stats(list, j, distance, &bat_out, &bat_ret);      
                // Check if the drone can also take items
                if (bat_check(charge, bat_out, bat_ret))
                {
                    list[j].is_delivered = true;
                    total_d += 2*distance;
                    // just used to increment charge 
                    bat_check_change(s, &charge, bat_out, bat_ret); 
                    stage_output(s, j, distance, bat_out, bat_ret);
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
            bat_change(s, &charge);
            num_bat ++;            
        }
    }
    print_stage_stats(s, num_bat, total_d, DRONE_SPEED);
}

/* Centroid functions */
double centroid_x(item_t *list,size_t size)
{
    double sum = 0.0;
    int i;
    for (i = 0; i < size; i++)
    {
        sum += list[i].x;
    }
    return (1.0/size)*sum;
}

/* Centroid functions */
double centroid_y(item_t *list,size_t size)
{
    double sum = 0.0;
    int i;
    for (i = 0; i < size; i++)
    {
        sum += list[i].y;
    }
    return (1.0/size)*sum;
}

// One of the enjoyable assignments ive had