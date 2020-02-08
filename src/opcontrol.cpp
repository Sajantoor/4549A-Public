#include "main.h"
#include "motor_setup.h"
#include "motor_sensor_init.h"
#include "drive.h"
#include "all_used.h"
#include "lift.h"
#include "angler.h"
#include "intake.h"

const int LIFT_HIGH = 1950;
const int LIFT_LOW = 2500;
const int LIFT_DESCORE = 1700;

void opcontrol() {
	// global variables
	bool anglerVal = false;
	bool liftBool = false; // used to check first lift
	int stickArray[4];
	int power[4];
	bool intakeUsed = false;

	while (true) {
		// printf("value %d \n \n", light_sensor_intake.get_value());
		// controller.print(0, 0, "Unlock");
		float armPosition = arm.get_position();
		stickArray[0] = powf(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X), 3) / powf(127, 2);
		stickArray[1] = powf(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y), 3) / powf(127, 2);
		stickArray[2] = powf(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X), 3) / powf(127, 2);
		stickArray[3] = powf(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y), 3) / powf(127, 2);
		// loops through array and removes values under 15 from the calculation
		for (size_t j = 0; j < 4; j++) {
			if (abs(stickArray[j]) < 15) {
				stickArray[j] = 0;
			}
			// for x values remove if they are over 30
			if (j == 0 || j == 2) {
				if (127 - abs(stickArray[j]) < 30) {
					if (stickArray[j] > 0)
						stickArray[j] = 127;
					else
						stickArray[j] = -127;
				}
			}
		}
		// tank drive with mecanum calculations
		power[0] = stickArray[1] + stickArray[0];
		power[1] = stickArray[1] - stickArray[0];
		power[2] = stickArray[3] - stickArray[2];
		power[3] = stickArray[3] + stickArray[2];
		// loops through all power values to check if they are above the limit (127)
		for (size_t i = 0; i < 4; i++) {
			if (abs(power[i]) > 127) {
				if (power[i] > 0) {
					power[i] = 127;
				} else {
					power[i] = -127;
				}
			}
		}
		// sets drive to power
		drive_left = power[0];
		drive_left_b = power[1];
		drive_right = power[2];
		drive_right_b = power[3];
		// check if intake is used in any task, letting driver use it.
		if (intakeTaskBool || !anglerIntakeThreshold) {
			intakeUsed = true;
		} else {
			intakeUsed = false;
		}
		// intake on triggers
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && !intakeUsed) {
			loader_left.move_voltage(12000);
			loader_right.move_voltage(12000);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2) && !intakeUsed) {
			loader_left.move_velocity(-12000);
			loader_right.move_velocity(-12000);
		} else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !intakeUsed) {
			loader_left.move(-94);
			loader_right.move(-94);
		} else if (!intakeUsed) {
			loader_left.move(0);
			loader_right.move(0);
		}

		// autonomous stacking mechanism
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
			if (!anglerVal) {
				anglerHold = false;
				pros::delay(20);
				angler_pid(1020, true, 127, true);
			} else if (anglerVal) {
				anglerHold = false;
				pros::delay(20);
				angler_pid(3400, true, 127, false, 2000);
			}
			// same button to return
			anglerVal ? anglerVal = false : anglerVal = true;
		}

		// if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
		// 	lift(800, 500);
		// }

		// Unlock mech in skills
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
			if (!(LIFT_HIGH + 100 > armPosition && armPosition > LIFT_HIGH - 100)) {
				lift(LIFT_HIGH, 20000);
			}
		}

		// lift high scoring value
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
			if (!(LIFT_HIGH + 100 > armPosition && armPosition > LIFT_HIGH - 100)) {
				lift(LIFT_HIGH, 20000);

				if (!liftBool) {
					liftBool = true;
					sensor_outtake();
				}
			}
		}
		// lift low scoring value
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
			if (!(LIFT_LOW + 100 > armPosition && armPosition > LIFT_LOW - 100)) {
				lift(LIFT_LOW, 20000);

				if (!liftBool) {
					liftBool = true;
					sensor_outtake();
				}
			}
		}

		// lift descore value
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
			if (!(LIFT_DESCORE + 100 > armPosition && armPosition > LIFT_DESCORE - 100)) {
				lift(LIFT_DESCORE, 20000);

				if (!liftBool) {
					liftBool = true;
					sensor_outtake();
				}
			}
		}
		// reset everything incase something goes wrong
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
				angler_pid(3400, true, 127, false, 2000);
				lift(0,0);
				intakeTaskBool = false;
				anglerIntakeThreshold = true;
		}

		// drop lift
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
			lift(0, 1000);
			liftBool = false;
		}

		pros::delay(20);
	}

}
