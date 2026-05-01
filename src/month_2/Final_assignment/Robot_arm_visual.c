/*
    Assignment name : Kinematic Calculation 
    Name : Ninrapat Suttinual 
    Reg no : 250139990

    Description:
    This program simulates a 2-joint (2DOF) planar robot arm.
    The user sets arm segment lengths, then enters a target (x, y) position.
    The program uses inverse kinematics (IK) to calculate the joint angles
    needed to reach that position, then displays the arm approaching the
    target step by step using interpolated angle calculations.
    The program loops allowing the user to move to multiple targets.

    AI Disclosure: Claude (Anthropic) was used for brainstorming and planning.
    All code was written and understood by the author.
*/


// Library 
#include <stdio.h>   /* printf, scanf, FILE operations */
#include <stdlib.h>  /* general utilities */
#include <string.h>  /* string handling */
#include <math.h>    /* cos, sin, acos, atan2, sqrt */

// Pi constant — defined manually in case compiler does not provide M_PI
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/* ------ Structure ------ */

/* Joint: represents one arm segment with its angle, length, and tip position */
struct Joint
{
    float angle ;    /* current angle of this joint in radians */
    float length ;   /* fixed length of this arm segment in cm */
    float x , y ;    /* current tip position of this joint — updated each step */
} ;

/* Arm: holds both joints and overall arm properties */
struct Arm
{
    struct Joint joints[2] ; /* array of 2 joints — joint[0]=shoulder, joint[1]=elbow */
    float base_x , base_y ; /* fixed base position of the robot arm (always 0,0) */
    float max_reach ; /* maximum reach (L1 + L2) — arm fully extended */
    float min_reach ; /* minimum reach (|L1 - L2|) — arm fully folded */
} ;

/* Target: stores the user's requested position and reachability result */
struct Target
{
    float x , y ; /* target position in cm relative to base */
    int is_reached ; /* 1 if target is within arm's reach, 0 if not */
    float distance_to_target ; /* straight line distance from base to target — used in IK */
} ;

/* --- Function Prototypes --- */
void Arm_setup(struct Arm *arm) ;                /* Initialize arm lengths, reach limits, default angles */
float validate_float(const char *prompt, float min, float max) ; /* Repetition for float input — loops until valid */
int validate_int(const char *prompt, int min, int max) ;         /* Repetition for int input — loops until valid */
void get_target_position(struct Target *target , struct Arm arm) ; /* Get target x,y from user with Y range guidance */
int check_target_reached(struct Target *target, struct Arm arm) ;  /* Check if target distance is within reach limits */
void Calculate_Joint_Angles(struct Arm *arm, struct Target target, float *t_angle1, float *t_angle2) ; /* Solve IK — find joint angles for target position */
void animate_movement(struct Arm *arm, float target_angle1, float target_angle2, struct Target target) ; /* Interpolate and display approach table */

/* ========================== MAIN ========================== */
int main(void)
{
    struct Arm robot_arm ;       /* main arm struct — declared in main and passed to functions */
    struct Target target ;       /* target struct — holds user's requested position */
    float target_angle1, target_angle2 ; /* IK solution angles — passed to animate_movement */
    int check_case ;             /* End loop condition — 1=continue, 0=quit */

    /* Setup arm once — no need to re-enter lengths each loop */
    Arm_setup(&robot_arm) ;

    // Repetition code using Do / While loop — runs at least once then checks if user wants to continue
    do {
        get_target_position(&target, robot_arm) ; /* get position of target and initialize */

        printf("\n=== TARGET ACCEPTED ===\n") ;
        printf("Target: (%.1f, %.1f) | Distance: %.1f cm\n",
               target.x, target.y, target.distance_to_target) ;

        Calculate_Joint_Angles(&robot_arm, target, &target_angle1, &target_angle2) ; /* solve IK for this target */
        animate_movement(&robot_arm, target_angle1, target_angle2, target) ;          /* display step-by-step approach */

        /* Reset arm angles back to 0 for next movement — ensures IK starts from default position */
        robot_arm.joints[0].angle = 0.0f ;                                        /* shoulder back to 0 degrees */
        robot_arm.joints[1].angle = 0.0f ;                                        /* elbow back to 0 degrees */
        robot_arm.joints[0].x = robot_arm.joints[0].length ;                      /* elbow tip pointing right */
        robot_arm.joints[0].y = 0.0f ;                                            /* elbow tip y reset */
        robot_arm.joints[1].x = robot_arm.joints[0].x + robot_arm.joints[1].length ; /* end tip pointing right */
        robot_arm.joints[1].y = 0.0f ;                                            /* end tip y reset */

        // Ask user whether user want to continue or not want to continue
        check_case = validate_int("\nMove to another target?", 0, 1) ;

    } while(check_case == 1) ; /* loop back if user enters 1 */

    printf("\n=== PROGRAM ENDED ===\n") ;
    printf("Thank you for using the robot arm simulator.\n") ;

    return 0 ;
}

/* ========================== FUNCTIONS ========================== */

void Arm_setup(struct Arm *arm)
{
    printf("=== ROBOT ARM SETUP ===\n") ;

    // Setting up all variable
    arm->joints[0].length = validate_float("Enter length of joint 1 (cm)", 1.0f, 50.0f) ; /* segment 1 length */
    arm->joints[1].length = validate_float("Enter length of joint 2 (cm)", 1.0f, 50.0f) ; /* segment 2 length */
    arm->joints[0].angle  = 0.0f ;              /* start pointing right (0 radians) */
    arm->joints[1].angle  = 0.0f ;              /* elbow fully extended at start */
    arm->joints[0].x      = arm->joints[0].length ;           /* elbow tip x at rest */
    arm->joints[0].y      = 0.0f ;                            /* elbow tip y at rest */
    arm->joints[1].x      = arm->joints[0].x + arm->joints[1].length ; /* end tip x at rest */
    arm->joints[1].y      = 0.0f ;                            /* end tip y at rest */
    arm->max_reach        = arm->joints[0].length + arm->joints[1].length ; /* fully extended reach */
    arm->min_reach        = arm->joints[0].length - arm->joints[1].length ; /* fully folded reach */

    if (arm->min_reach < 0) arm->min_reach = -arm->min_reach ; /* ensure min_reach is always positive */

    arm->base_x = 0.0f ;        /* base fixed at origin x */
    arm->base_y = 0.0f ;        /* base fixed at origin y */

    printf("\n=== ARM SETUP COMPLETE ===\n") ;
    printf("Joint 1 length: %.1f cm | Joint 2 length: %.1f cm\n",
           arm->joints[0].length, arm->joints[1].length) ;
    printf("Max reach: %.1f cm | Min reach: %.1f cm\n",
           arm->max_reach, arm->min_reach) ;
}

float validate_float(const char *prompt, float min, float max)
{
/*  Repetition system to validate input float number by sort only correct number 
    to structure variable */
    float value ;   /* stores the user's input */
    int result ;    /* scanf return value — 1 if read succeeded, 0 if failed */

    do {
        printf("%s (%.1f to %.1f): ", prompt, min, max) ;
        result = scanf("%f", &value) ;   /* attempt to read a float */

        if(result != 1) {                /* scanf failed — user typed letters or symbols */
            printf("Invalid! Please enter a number.\n") ;
            while(getchar() != '\n') ;   /* flush bad input from buffer to prevent infinite loop */
            continue ;                   /* skip range check, loop again */
        }

        if(value < min || value > max)   /* number read successfully but outside allowed range */
            printf("Out of range! Must be between %.1f and %.1f\n", min, max) ;

    } while(result != 1 || value < min || value > max) ; /* keep looping until valid */

    return value ; /* return validated value to caller */
}

int validate_int(const char *prompt, int min, int max)
{
    /* Repetition system to validate input integer number by sort only correct number 
     to structure variable */

    int value ;   /* stores the user's input */
    int result ;  /* scanf return value — 1 if read succeeded, 0 if failed */

    do {
        printf("%s (%d to %d): ", prompt, min, max) ;
        result = scanf("%d", &value) ;   /* attempt to read an integer */

        if(result != 1) {                /* scanf failed — user typed letters or symbols */
            printf("Invalid! Please enter an integer.\n") ;
            while(getchar() != '\n') ;   /* flush bad input from buffer to prevent infinite loop */
            continue ;                   /* skip range check, loop again */
        }

        if(value < min || value > max)   /* number read successfully but outside allowed range */
            printf("Out of range! Must be between %d and %d\n", min, max) ;

    } while(result != 1 || value < min || value > max) ; /* keep looping until valid */

    return value ; /* return validated value to caller */
}

void get_target_position(struct Target *target, struct Arm arm)
{
    float max_sq, min_sq, y_max, y_min ; /* intermediate values for Y range calculation */

    printf("\n=== ENTER TARGET POSITION ===\n") ;
    printf("Arm reach: %.1f to %.1f cm from base\n", arm.min_reach, arm.max_reach) ;

    do {
        /* Step 1 — get X coordinate first so we can calculate valid Y range */
        printf("\nX range: (%.1f to %.1f)\n", -arm.max_reach, arm.max_reach) ;
        target->x = validate_float("Enter target X", -arm.max_reach, arm.max_reach) ;

        /* Calculate valid Y range based on X entered — using Pythagoras on the reach ring */
        max_sq = arm.max_reach * arm.max_reach - target->x * target->x ; /* max Y² = max_reach² - x² */
        min_sq = arm.min_reach * arm.min_reach - target->x * target->x ; /* min Y² = min_reach² - x² */

        y_max = sqrt(max_sq) ;                         /* maximum reachable Y for this X */
        y_min = (min_sq > 0) ? sqrt(min_sq) : 0.0f ;  /* minimum reachable Y — 0 if arm can reach axis */

        /* Show user the exact Y values that will work for their chosen X */
        printf("For X = %.1f, valid Y range is:\n", target->x) ;
        printf("  Positive Y: %.1f to %.1f\n",  y_min,  y_max) ;
        printf("  Negative Y: %.1f to %.1f\n", -y_max, -y_min) ;

        /* Step 2 — get Y coordinate within the calculated valid range */
        target->y = validate_float("Enter target Y", -y_max, y_max) ;

        /* Reject Y values inside the unreachable hole when min_reach > 0 */
        if (y_min > 0.0f && fabsf(target->y) < y_min) {
            printf("Y = %.1f is inside the unreachable zone. Must be >= %.1f or <= -%.1f\n",
                   target->y, y_min, y_min) ;
            continue ;
        }

        /* Step 3 — verify combined X,Y distance falls within the reach ring */
        check_target_reached(target, arm) ;

        if(!target->is_reached)             /* combined distance may still be outside reach ring */
            printf("Still out of reach — please re-enter.\n") ;

    } while(!target->is_reached) ; /* repeat until user enters a reachable coordinate pair */
}

int check_target_reached(struct Target *target, struct Arm arm)
{
    float d_squared = target->x * target->x + target->y * target->y ; /* squared distance — avoids sqrt until needed */
    float max_sq    = arm.max_reach * arm.max_reach ; /* squared max reach for comparison — avoids sqrt */
    float min_sq    = arm.min_reach * arm.min_reach ; /* squared min reach for comparison — avoids sqrt */

    /* Calculate actual distance for display and IK use */
    target->distance_to_target = sqrt(d_squared) ; /* actual straight line distance from base to target */

    printf("=== CHECKING TARGET REACHABILITY ===\n") ;

    if(d_squared <= max_sq && d_squared >= min_sq) /* target is within the reachable ring */
    {
        target->is_reached = 1 ;                   /* mark as reachable */
        printf("Target at (%.1f, %.1f) is reachable!\n", target->x, target->y) ;
        printf("Distance from base: %.1f cm\n", target->distance_to_target) ;
    }
    else                                           /* target is outside the reachable ring */
    {
        target->is_reached = 0 ;                   /* mark as not reachable */
        printf("Target at (%.1f, %.1f) is not reachable.\n", target->x, target->y) ;
        printf("Distance: %.1f cm | Valid range: %.1f to %.1f cm\n",
               target->distance_to_target, arm.min_reach, arm.max_reach) ;
    }

    return target->is_reached ; /* return flag so calling function can decide what to do */
}

void Calculate_Joint_Angles(struct Arm *arm, struct Target target, float *t_angle1, float *t_angle2)
{
    /* Inverse kinematics using law of cosines — finds joint angles for a given (x,y) target */
    float x  = target.x ;                   /* target x coordinate */
    float y  = target.y ;                   /* target y coordinate */
    float L1 = arm->joints[0].length ;      /* length of segment 1 (shoulder to elbow) */
    float L2 = arm->joints[1].length ;      /* length of segment 2 (elbow to tip) */
    float d  = target.distance_to_target ;  /* reuse already calculated distance — avoids redundant sqrt */

    /* Step 1 — law of cosines to find elbow angle theta2 */
    float cos_angle2 = (d*d - L1*L1 - L2*L2) / (2.0f * L1 * L2) ;

    /* Clamp to [-1, 1] to prevent acos domain error from floating point precision loss */
    if (cos_angle2 >  1.0f) cos_angle2 =  1.0f ; /* clamp upper bound */
    if (cos_angle2 < -1.0f) cos_angle2 = -1.0f ; /* clamp lower bound */

    *t_angle2 = acos(cos_angle2) ;           /* elbow angle in radians */

    /* Step 2 — atan2 formula to find shoulder angle theta1 */
    *t_angle1 = atan2(y, x) - atan2(L2 * sin(*t_angle2), L1 + L2 * cos(*t_angle2)) ;

    printf("\n=== JOINT ANGLES CALCULATED ===\n") ;
    printf("Joint 1 target angle: %.2f degrees\n", *t_angle1 * 180.0f / M_PI) ; /* radians to degrees for display */
    printf("Joint 2 target angle: %.2f degrees\n", *t_angle2 * 180.0f / M_PI) ; /* radians to degrees for display */
}

void animate_movement(struct Arm *arm, float target_angle1, float target_angle2, struct Target target)
{
    float start_angle1 = arm->joints[0].angle ; /* store shoulder starting angle before interpolation begins */
    float start_angle2 = arm->joints[1].angle ; /* store elbow starting angle before interpolation begins */
    float t, dist ;                             /* t = interpolation factor (0.0 to 1.0), dist = distance to target */
    int step, num_steps ;                       /* loop counter and user-chosen step count */

    num_steps = validate_int("Enter number of steps to show between positions", 5, 10) ; /* user chooses table resolution */

    printf("\n=== APPROACH CALCULATION ===\n") ;
    printf("From: (%.1f, %.1f) -> To: (%.1f, %.1f)\n\n",
           arm->joints[1].x, arm->joints[1].y, target.x, target.y) ; /* show start and end positions */

    /* Print table header */
    printf("%-6s %-12s %-12s %-12s %-12s %-16s\n",
           "Step", "J1 (deg)", "J2 (deg)", "Tip X", "Tip Y", "Dist to target") ;
    printf("--------------------------------------------------------------\n") ;

    for(step = 0; step <= num_steps; step++)
    {
        t = (float)step / (float)num_steps ; /* interpolation factor: 0.0 at start, 1.0 at target */

        /* Linearly blend from start angle to target angle for each joint */
        arm->joints[0].angle = start_angle1 + t * (target_angle1 - start_angle1) ; /* shoulder angle at this step */
        arm->joints[1].angle = start_angle2 + t * (target_angle2 - start_angle2) ; /* elbow angle at this step */

        /* Recalculate elbow tip position from updated shoulder angle */
        arm->joints[0].x = arm->base_x + arm->joints[0].length * cos(arm->joints[0].angle) ;
        arm->joints[0].y = arm->base_y + arm->joints[0].length * sin(arm->joints[0].angle) ;

        /* Recalculate end tip position from combined shoulder + elbow angle */
        arm->joints[1].x = arm->joints[0].x + arm->joints[1].length * cos(arm->joints[0].angle + arm->joints[1].angle) ;
        arm->joints[1].y = arm->joints[0].y + arm->joints[1].length * sin(arm->joints[0].angle + arm->joints[1].angle) ;

        /* Calculate remaining straight line distance from current tip to target */
        dist = sqrt((arm->joints[1].x - target.x) * (arm->joints[1].x - target.x) +
                    (arm->joints[1].y - target.y) * (arm->joints[1].y - target.y)) ;

        /* Print one row of the approach table — shows convergence toward target */
        printf("%-6d %-12.2f %-12.2f %-12.2f %-12.2f %.3f cm\n",
               step,
               arm->joints[0].angle * 180.0f / M_PI, /* shoulder angle in degrees */
               arm->joints[1].angle * 180.0f / M_PI, /* elbow angle in degrees */
               arm->joints[1].x,                     /* tip x at this step */
               arm->joints[1].y,                     /* tip y at this step */
               dist) ;                               /* remaining distance — should reach 0.000 at final step */
    }

    printf("--------------------------------------------------------------\n") ;

    /* Final summary — confirms arm reached the target */
    printf("\n=== MOVEMENT COMPLETE ===\n") ;
    printf("Target was:          (%.2f, %.2f)\n", target.x, target.y) ;
    printf("Final tip position:  (%.2f, %.2f)\n", arm->joints[1].x, arm->joints[1].y) ;
    printf("Joint 1 final angle: %.2f degrees\n", arm->joints[0].angle * 180.0f / M_PI) ;
    printf("Joint 2 final angle: %.2f degrees\n", arm->joints[1].angle * 180.0f / M_PI) ;
}
