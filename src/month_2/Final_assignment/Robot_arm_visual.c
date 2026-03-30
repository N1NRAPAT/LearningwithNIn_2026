#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

struct Joint
{
    float angle ;
    float length ;
    float x , y ; /* current tip position of this joint */
} ;

struct Arm
{
    struct Joint joints[2] ; /* 2D robot arm with 2 joints */
    float base_x , base_y ; /* base position of the robot arm */
    float max_reach ; /* maximum reach (L1 + L2) */
    float min_reach ; /* minimum reach (|L1 - L2|) */
    int speed ; /* speed selection (1=slow, 2=normal, 3=fast) */
    int count_steps ; /* animation steps based on speed */
} ;

struct Target
{
    float x , y ; /* target position */
    int is_reached ; /* 1 if reachable, 0 if not */
    float distance_to_target ; /* straight line distance from base to target */
} ;

/* --- Function Prototypes --- */
void Arm_setup(struct Arm *arm) ;
void select_speed(struct Arm *arm) ;
float validate_float(char *prompt, float min, float max) ;
int validate_int(char *prompt, int min, int max) ;
void get_target_position(struct Target *target, struct Arm arm) ;
int check_target_reached(struct Target *target, struct Arm arm) ;
void Calculate_Joint_Angles(struct Arm *arm, struct Target target, float *t_angle1, float *t_angle2) ;
void animate_movement(struct Arm *arm, float target_angle1, float target_angle2, struct Target target) ;
void draw_arm(float bx, float by, float j1x, float j1y, float j2x, float j2y,
              float tx, float ty, float scale, int step, int total_steps) ;
void show_default_arm(struct Arm arm) ;

/* ========================== MAIN ========================== */
int main()
{
    struct Arm robot_arm ;
    struct Target target ;
    float target_angle1, target_angle2 ;

    Arm_setup(&robot_arm) ;

    /* Show arm at default position before asking for target */
    show_default_arm(robot_arm) ;

    get_target_position(&target, robot_arm) ;

    printf("\n=== TARGET ACCEPTED ===\n") ;
    printf("Target: (%.1f, %.1f) | Distance: %.1f cm\n",
           target.x, target.y, target.distance_to_target) ;

    Calculate_Joint_Angles(&robot_arm, target, &target_angle1, &target_angle2) ;
    select_speed(&robot_arm) ;
    animate_movement(&robot_arm, target_angle1, target_angle2, target) ;

    return 0 ;
}

/* ========================== FUNCTIONS ========================== */

void Arm_setup(struct Arm *arm)
{
    printf("=== ROBOT ARM SETUP ===\n") ;

    arm->joints[0].length = validate_float("Enter length of joint 1 (cm)", 1.0f, 50.0f) ;
    arm->joints[1].length = validate_float("Enter length of joint 2 (cm)", 1.0f, 50.0f) ;
    arm->joints[0].angle  = 0.0f ;
    arm->joints[1].angle  = 0.0f ;
    arm->joints[0].x      = arm->joints[0].length ;
    arm->joints[0].y      = 0.0f ;
    arm->joints[1].x      = arm->joints[0].x + arm->joints[1].length ;
    arm->joints[1].y      = 0.0f ;
    arm->max_reach        = arm->joints[0].length + arm->joints[1].length ;
    arm->min_reach        = arm->joints[0].length - arm->joints[1].length ;

    if (arm->min_reach < 0) arm->min_reach = -arm->min_reach ;

    arm->base_x = 0.0f ;
    arm->base_y = 0.0f ;

    printf("\n=== ARM SETUP COMPLETE ===\n") ;
    printf("Joint 1 length: %.1f cm | Joint 2 length: %.1f cm\n",
           arm->joints[0].length, arm->joints[1].length) ;
    printf("Max reach: %.1f cm | Min reach: %.1f cm\n",
           arm->max_reach, arm->min_reach) ;
}

void select_speed(struct Arm *arm)
{
    int choice ;
    printf("\n=== SELECT MOVEMENT SPEED ===\n") ;
    choice = validate_int("Select speed (1=slow, 2=normal, 3=fast)", 1, 3) ;
    arm->speed = choice ;

    if(choice == 1)      arm->count_steps = 30 ;
    else if(choice == 2) arm->count_steps = 20 ;
    else                 arm->count_steps = 10 ;

    printf("Speed set to %s\n", choice == 1 ? "slow" : choice == 2 ? "normal" : "fast") ;
}

float validate_float(char *prompt, float min, float max)
{
    float value ;
    int result ;

    do {
        printf("%s (%.1f to %.1f): ", prompt, min, max) ;
        result = scanf("%f", &value) ;

        if(result != 1) {
            printf("Invalid! Please enter a number.\n") ;
            while(getchar() != '\n') ;
            continue ;
        }

        if(value < min || value > max)
            printf("Out of range! Must be between %.1f and %.1f\n", min, max) ;

    } while(result != 1 || value < min || value > max) ;

    return value ;
}

int validate_int(char *prompt, int min, int max)
{
    int value ;
    int result ;

    do {
        printf("%s (%d to %d): ", prompt, min, max) ;
        result = scanf("%d", &value) ;

        if(result != 1) {
            printf("Invalid! Please enter an integer.\n") ;
            while(getchar() != '\n') ;
            continue ;
        }

        if(value < min || value > max)
            printf("Out of range! Must be between %d and %d\n", min, max) ;

    } while(result != 1 || value < min || value > max) ;

    return value ;
}

void get_target_position(struct Target *target, struct Arm arm)
{
    float max_sq, min_sq, y_max, y_min ;

    printf("\n=== ENTER TARGET POSITION ===\n") ;
    printf("Arm reach: %.1f to %.1f cm from base\n", arm.min_reach, arm.max_reach) ;

    do {
        printf("\nX range: (%.1f to %.1f)\n", -arm.max_reach, arm.max_reach) ;
        target->x = validate_float("Enter target X", -arm.max_reach, arm.max_reach) ;

        /* Calculate valid Y range based on X entered */
        max_sq = arm.max_reach * arm.max_reach - target->x * target->x ;
        min_sq = arm.min_reach * arm.min_reach - target->x * target->x ;

        y_max = sqrt(max_sq) ;
        y_min = (min_sq > 0) ? sqrt(min_sq) : 0.0f ;

        printf("For X = %.1f, valid Y range is:\n", target->x) ;
        printf("  Positive Y: %.1f to %.1f\n",  y_min,  y_max) ;
        printf("  Negative Y: %.1f to %.1f\n", -y_max, -y_min) ;

        target->y = validate_float("Enter target Y", -y_max, y_max) ;

        check_target_reached(target, arm) ;

        if(!target->is_reached)
            printf("Still out of reach — please re-enter.\n") ;

    } while(!target->is_reached) ;
}

int check_target_reached(struct Target *target, struct Arm arm)
{
    float d_squared = target->x * target->x + target->y * target->y ;
    float max_sq    = arm.max_reach * arm.max_reach ;
    float min_sq    = arm.min_reach * arm.min_reach ;

    /* Calculate actual distance for display and IK use */
    target->distance_to_target = sqrt(d_squared) ;

    printf("=== CHECKING TARGET REACHABILITY ===\n") ;

    if(d_squared <= max_sq && d_squared >= min_sq)
    {
        target->is_reached = 1 ;
        printf("Target at (%.1f, %.1f) is reachable!\n", target->x, target->y) ;
        printf("Distance from base: %.1f cm\n", target->distance_to_target) ;
    }
    else
    {
        target->is_reached = 0 ;
        printf("Target at (%.1f, %.1f) is not reachable.\n", target->x, target->y) ;
        printf("Distance: %.1f cm | Valid range: %.1f to %.1f cm\n",
               target->distance_to_target, arm.min_reach, arm.max_reach) ;
    }

    return target->is_reached ;
}

void Calculate_Joint_Angles(struct Arm *arm, struct Target target, float *t_angle1, float *t_angle2)
{
    float x  = target.x ;
    float y  = target.y ;
    float L1 = arm->joints[0].length ;
    float L2 = arm->joints[1].length ;
    float d  = target.distance_to_target ; /* reuse already calculated distance */

    float cos_angle2 = (d*d - L1*L1 - L2*L2) / (2.0f * L1 * L2) ;

    /* Clamp to [-1, 1] to prevent acos domain error from float precision */
    if (cos_angle2 >  1.0f) cos_angle2 =  1.0f ;
    if (cos_angle2 < -1.0f) cos_angle2 = -1.0f ;

    *t_angle2 = acos(cos_angle2) ;
    *t_angle1 = atan2(y, x) - atan2(L2 * sin(*t_angle2), L1 + L2 * cos(*t_angle2)) ;

    printf("\n=== JOINT ANGLES CALCULATED ===\n") ;
    printf("Joint 1 target angle: %.2f degrees\n", *t_angle1 * 180.0f / M_PI) ;
    printf("Joint 2 target angle: %.2f degrees\n", *t_angle2 * 180.0f / M_PI) ;
}

void animate_movement(struct Arm *arm, float target_angle1, float target_angle2, struct Target target)
{
    float start_angle1 = arm->joints[0].angle ;
    float start_angle2 = arm->joints[1].angle ;
    float t ;
    int step ;
    float scale ;

    /* Scale grid so arm fills canvas — canvas is 30 units radius, arm max_reach fills it */
    scale = 30.0f / arm->max_reach ;

    printf("\n=== ANIMATING ARM MOVEMENT ===\n") ;

    for(step = 0; step <= arm->count_steps; step++)
    {
        /* t goes from 0.0 to 1.0 across all steps */
        t = (float)step / (float)arm->count_steps ;

        /* Interpolate angles toward target */
        arm->joints[0].angle = start_angle1 + t * (target_angle1 - start_angle1) ;
        arm->joints[1].angle = start_angle2 + t * (target_angle2 - start_angle2) ;

        /* Update joint tip positions from new angles */
        arm->joints[0].x = arm->base_x + arm->joints[0].length * cos(arm->joints[0].angle) ;
        arm->joints[0].y = arm->base_y + arm->joints[0].length * sin(arm->joints[0].angle) ;

        arm->joints[1].x = arm->joints[0].x + arm->joints[1].length * cos(arm->joints[0].angle + arm->joints[1].angle) ;
        arm->joints[1].y = arm->joints[0].y + arm->joints[1].length * sin(arm->joints[0].angle + arm->joints[1].angle) ;

        draw_arm(arm->base_x, arm->base_y,
                 arm->joints[0].x, arm->joints[0].y,
                 arm->joints[1].x, arm->joints[1].y,
                 target.x, target.y, scale,
                 step, arm->count_steps) ;

        printf("Step %2d/%d | J1: %6.1f deg | J2: %6.1f deg | Tip: (%.1f, %.1f)\n",
               step, arm->count_steps,
               arm->joints[0].angle * 180.0f / M_PI,
               arm->joints[1].angle * 180.0f / M_PI,
               arm->joints[1].x, arm->joints[1].y) ;
    }

    printf("\n=== MOVEMENT COMPLETE ===\n") ;
    printf("Final tip position: (%.1f, %.1f)\n", arm->joints[1].x, arm->joints[1].y) ;
}

void draw_arm(float bx, float by,
              float j1x, float j1y,
              float j2x, float j2y,
              float tx,  float ty,
              float scale, int step, int total_steps)
{
    int GRID = 30 ;           /* canvas radius — 30 cells each direction */
    int SIZE = GRID * 2 + 1 ; /* total grid size: 61x61 */
    int PAD  = 4 ;            /* left padding spaces to centre in terminal */
    char grid[61][61] ;
    int i, j, s ;
    float t ;
    int gx, gy ;

    /* Fill grid with spaces for cleaner look */
    for(i = 0; i < SIZE; i++)
        for(j = 0; j < SIZE; j++)
            grid[i][j] = ' ' ;

    /* Draw axes */
    for(i = 0; i < SIZE; i++) {
        grid[GRID][i] = '-' ;
        grid[i][GRID] = '|' ;
    }
    grid[GRID][GRID] = '+' ;

    /* Axis labels at edges */
    grid[GRID][SIZE-1]  = '>' ; /* X positive */
    grid[0][GRID]       = '^' ; /* Y positive */

    /* Convert world coords to grid — scale so arm fills canvas */
    #define GX(x) (GRID + (int)((x) * scale + 0.5f))
    #define GY(y) (GRID - (int)((y) * scale + 0.5f))

    /* Mark target position with * */
    gx = GX(tx) ; gy = GY(ty) ;
    if(gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE)
        grid[gy][gx] = '*' ;

    /* Draw segment 1 — base to elbow */
    for(s = 0; s <= 40; s++) {
        t  = (float)s / 40 ;
        gx = GX(bx + t * (j1x - bx)) ;
        gy = GY(by + t * (j1y - by)) ;
        if(gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE)
            grid[gy][gx] = '=' ;
    }

    /* Draw segment 2 — elbow to tip */
    for(s = 0; s <= 40; s++) {
        t  = (float)s / 40 ;
        gx = GX(j1x + t * (j2x - j1x)) ;
        gy = GY(j1y + t * (j2y - j1y)) ;
        if(gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE)
            grid[gy][gx] = '-' ;
    }

    /* Plot key points on top */
    gx = GX(bx) ; gy = GY(by) ;
    if(gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE)
        grid[gy][gx] = 'B' ;

    gx = GX(j1x) ; gy = GY(j1y) ;
    if(gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE)
        grid[gy][gx] = 'E' ;

    gx = GX(j2x) ; gy = GY(j2y) ;
    if(gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE)
        grid[gy][gx] = 'T' ;

    /* Replot target on top in case arm segment overwrote it */
    gx = GX(tx) ; gy = GY(ty) ;
    if(gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE &&
       grid[gy][gx] != 'T')  /* don't overwrite tip when reached */
        grid[gy][gx] = '*' ;

    /* Print blank lines to push old frame up — works on all platforms */
    for(i = 0; i < 65; i++) printf("\n") ;

    /* Header */
    printf("%*s=== ROBOT ARM SIMULATOR === Step %d/%d\n\n", PAD, "", step, total_steps) ;

    /* Print grid with left padding to centre it */
    for(i = 0; i < SIZE; i++) {
        printf("%*s", PAD, "") ;
        for(j = 0; j < SIZE; j++)
            printf("%c", grid[i][j]) ;
        printf("\n") ;
    }

    /* Legend below grid */
    printf("\n%*sB=base  E=elbow  T=tip  *=target  ===segment1  ---segment2\n",
           PAD, "") ;

    #undef GX
    #undef GY
}

/* Show the arm at its starting position before animation begins */
void show_default_arm(struct Arm arm)
{
    printf("\n") ;
    /* Draw with no target shown yet (target at 0,0 won't be visible on axis) */
    /* Pass dummy target at base so * doesn't appear — use a point off canvas */
    float scale = 30.0f / arm.max_reach ;
    draw_arm(arm.base_x, arm.base_y,
             arm.joints[0].x, arm.joints[0].y,
             arm.joints[1].x, arm.joints[1].y,
             9999.0f, 9999.0f,   /* target off canvas — not visible */
             scale, 0, 0) ;

    printf("\nArm is at default position (pointing right).\n") ;
    printf("Now enter your target coordinates below.\n") ;
}