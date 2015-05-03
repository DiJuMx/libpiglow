/* Constants and Enumerations */

/* Enumerations representing the arm of the led triskelion.
 * TOP refers to the arm closest to the word "piGlow"
 * BOTTOM refers to the arm closest to the power socket
 * RIGHT refers to the arm closest to the processor
 */
enum led_arm {TOP, BOTTOM, RIGHT};

/* Enumeration representing the colour of the LED to be lit up 
 */
enum led_colour {RED, ORANGE, YELLOW, GREEN, BLUE, WHITE};

/* Function Definitions */

/* Brightness corrected (gamma) values
 * index from 0 to 31
 */
int piglow_gamma(int index);

/* Initialise the PiGlow (and library)
 * i2c_id should be the number corresponding
 * to /dev/i2c-x for the PiGlow (typically 0 or 1)
 */
int piglow_init(int i2c_id);

/* Set the PWM value for a given LED on a 
 * given arm of the Triskelion
 */
void piglow_set(enum led_arm arm, enum led_colour colour, int value);

/* Reset the PiGlow 
 */
void piglow_reset(void);

/* Apply the changes made with piglow_set()
 */
void piglow_update(void);

/* Shutdown the PiGlow
 */
void piglow_cleanup(void);
