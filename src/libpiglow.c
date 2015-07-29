#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "piglow.h"

/* Values taken from SN3218 data sheet */
int pi_gamma[32] = {
    0x00, 0x01, 0x02, 0x04, 0x06, 0x0A, 0x0D, 0x12,
    0x16, 0x1C, 0x21, 0x27, 0x2E, 0x35, 0x3D, 0x45,
    0x4E, 0x56, 0x60, 0x6A, 0x74, 0x7E, 0x8A, 0x95,
    0xA1, 0xAD, 0xBA, 0xC7, 0xD4, 0xE2, 0xF0, 0xFF
};

/* Necessary as registers on chip cannot be read */
char pi_enabledReg1 = 0x00;
char pi_enabledReg2 = 0x00;
char pi_enabledReg3 = 0x00;

/* File descriptor to /dev/i2c-x device */
int pi_i2cDevice = -1;

/* internal helper function to write bytes */
int piglow_write(char reg, char value)
{
    if(pi_i2cDevice < 0)
    {
        fputs("PiGlow not initialised\n",stderr);
        return (-1);
    }
    else
    {
        if(write(pi_i2cDevice, (char[2]){reg,value}, 2) != 2)
        {
            perror("Unable to write to PiGlow");
            return (-1);
        }
    }

    return (0);
}


/* return a value from the gamma array,
 * but make sure we don't go out of bounds
 */
int piglow_gamma(int index)
{
    if(index < 0)
        return (pi_gamma[0]);
    else if(index > 31)
        return (pi_gamma[31]);
    else
        return (pi_gamma[index]);
}


int piglow_init(int i2c_id)
{
    char filename[30];

    snprintf(filename,29, "/dev/i2c-%d", i2c_id);

    /* Open Device */
    pi_i2cDevice = open(filename, O_RDWR);
    if(pi_i2cDevice < 0)
    {
        perror("Couldn't access PiGlow");
        return (-1);
    }

    /* Set I2C on device */
    if(ioctl(pi_i2cDevice, I2C_SLAVE, 0x54) < 0)
    {
        perror("Couldn't access PiGlow");
        close(pi_i2cDevice);
        return (-1);
    }

    /* Clear the registers on the PiGlow */
    piglow_reset();

    /* Enable the piglow */
    piglow_write(0x00, 0x01);

    return (0);
}

void piglow_reset(void)
{
    /* Clear our local copy of the enable registers*/
    pi_enabledReg1 = 0;
    pi_enabledReg2 = 0;
    pi_enabledReg3 = 0;

    /* Write to the reset register */
    if(piglow_write(0x17, 0x01) != 0)
    {
        fputs("Couldn't reset PiGlow\n",stderr);
    }
}

void piglow_set(enum led_arm arm, enum led_colour colour, int value)
{
    char enabled;
    char reg;

    /* limit values */
    if(value < 0) value = 0;
    if(value > 255) value = 255;

    /* Is teh LED enabled? */
    if(value == 0) enabled = 0x00;
    else enabled = 0x01;

    /* Select the correct register to write to */
    if(arm == BOTTOM)
    {
        switch(colour)
        {
            case RED:   reg = 0x01; break;
            case ORANGE:reg = 0x02; break;
            case YELLOW:reg = 0x03; break;
            case GREEN: reg = 0x04; break;
            case BLUE:  reg = 0x0F; break;
            case WHITE: reg = 0x0D; break;
            default:
                fputs("Invalid Colour\n", stderr);
                reg = 0xFF;
        }
    }
    else if(arm == TOP)
    {
        switch(colour)
        {
            case RED:   reg = 0x07; break;
            case ORANGE:reg = 0x08; break;
            case YELLOW:reg = 0x09; break;
            case GREEN: reg = 0x06; break;
            case BLUE:  reg = 0x05; break;
            case WHITE: reg = 0x0A; break;
            default:
                fputs("Invalid Colour\n", stderr);
                reg = 0xFF;
        }
    }
    else if(arm == RIGHT)
    {
        switch(colour)
        {
            case RED:   reg = 0x12; break;
            case ORANGE:reg = 0x11; break;
            case YELLOW:reg = 0x10; break;
            case GREEN: reg = 0x0E; break;
            case BLUE:  reg = 0x0C; break;
            case WHITE: reg = 0x0B; break;
            default:
                fputs("Invalid Colour\n", stderr);
                reg = 0xFF;
        }
    }
    else
    {
        fputs("Invalid arm\n", stderr);
        return;
    }    

    /* Bit-wise magic to set the enable flags */
    switch((reg-1) / 6)
    {
        case 0:
            if(enabled) pi_enabledReg1 |= 1 << ((reg-1)%6);
            else pi_enabledReg1 &= ~(1<<(reg-1)%6);
        case 1:
            if(enabled) pi_enabledReg2 |= 1 << ((reg-1)%6);
            else pi_enabledReg2 &= ~(1<<(reg-1)%6);
        case 2:
            if(enabled) pi_enabledReg3 |= 1 << ((reg-1)%6);
            else pi_enabledReg3 &= ~(1<<(reg-1)%6);
    }

    /* Write the PWM value */
    if(piglow_write(reg, value) != 0)
    {
        fputs("Couldn't set LED value\n", stderr);
    }

}

void piglow_update(void)
{
    /* Need to write all three enable registers */
    if(piglow_write(0x13, pi_enabledReg1) != 0
     ||piglow_write(0x14, pi_enabledReg2) != 0
     ||piglow_write(0x15, pi_enabledReg3) != 0 )
    {
        fputs("Couldn't enable LEDs\n", stderr);
    }

    /* Also write to the update register */
    if(piglow_write(0x16, 0x01) != 0)
    {
        fputs("Couldn't update PiGlow\n",stderr);
    }
}

void piglow_cleanup(void)
{
    /* Only do this if we have a file descriptor */
    if(!(pi_i2cDevice < 0))
    {
        /* Reset the PiGlow */
        piglow_reset();

        /* Close the File Descriptor */
        close(pi_i2cDevice);
    }
}
