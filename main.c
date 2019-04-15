/*****************************
 * Flynn Harrison 13/03/2019
 * sort function that sort in
 * assending order 
 ****************************/


#include <stdio.h>
#include <math.h>

// Settings
#define MAX_DELIVERYS   999
#define MAX_PAYLOAD     5.8 // Kg
#define DRONE_MASS      3.4 // Kg

#define STAGE_1         "S1"
#define STAGE_2         "S2"
#define STAGE_3         "S3"

typedef struct 
{
    // Position m
    double x,y;
    // Mass Kg
    double mass;
}delivery_t;

// Prototypes 
/* Prints the first and last elements of delivery_t array */
void print_first_last(char *stage,delivery_t list[], int list_size);
/* Sums up the masses in a delivery_t array */
double delivery_t_mass_sum(delivery_t *list, int list_size);

int main(int argc, char **argv)
{
    // Stage 1, get input
    delivery_t deliverys[MAX_DELIVERYS];
    int lenght = 0;
    double x_buff, y_buff, m_buff;
    // Eat first line
    while(getchar() != '\n'){}

    for(;(scanf("%lf %lf %lf", &x_buff, &y_buff, &m_buff) == 3) 
            && lenght<MAX_DELIVERYS; lenght++)
    {
        deliverys[lenght].x = x_buff;
        deliverys[lenght].y = y_buff;
        deliverys[lenght].mass = m_buff;
    }

    print_first_last(STAGE_1, deliverys, lenght-1);
    printf("%s, total to deliver: %3.2lf Kg\n",
        STAGE_1, delivery_t_mass_sum(deliverys, lenght-1));
}

void print_first_last(char *stage, delivery_t *list, int list_size)
{
    printf("%s, total data lines: %3d\n",stage, list_size);
    // First and last elements of array
    printf("%s, first data line :  x=%4.1lf, y=%4.1lf, kg=%4.1lf\n",
        stage, list[0].x, list[0].y, list[0].mass);
    printf("%s, final data line :  x=%4.1lf, y=%4.1lf, kg=%4.1lf\n",
        stage, list[list_size].x, list[list_size].y, list[list_size].mass);
}

double delivery_t_mass_sum(delivery_t *list, int list_size)
{
    double sum = 0;
    for (int i = 0; i<list_size; i++)
    {
        sum += list[i].mass;
    }
    return sum;
}