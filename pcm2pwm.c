/* pcm2pwm.c
 *
 * Simple converter opens any mono, 8-bit wav file and outputs a
 * byte stream for use in assembly language 1-bit latch audio
 * routines.
 *
 * See readme for basic use case.
 *
 *
 * code: jeff alyanak
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "convert.h"
#include "header.h"

FILE *fp;				// File pointer.
int length = 0;			// File length in bytes.
int highValue = 0xFC;	// Default high crossover.
int lowValue = 0x03;	// Default low crossover
int format = 0;			// Default (0) or HT2 format (1).

int main (int argc, char *argv[])
{
	if (argc == 1) // If no arguments are provided, return help info.
	{
		printf ("\nBasic usage: pcm2pwm input.wav\n\n");
		printf ("Advanced usage: pcm2pwm input.wav X Y\n");
		printf ("     Where X and Y represent high and low\n");
		printf ("     crossovers. Defaults are 252 and 3.\n");
		printf ("     (Values must be between 0 and 255.)\n");
		printf ("ex: pcm2pwm sound.wav 230 20.\n\n");
		printf ("You may also specify output formatted for\n");
		printf ("Houston Tracker 2 using -h.\n");
		printf ("For example: pcm2pwm -h input.wav\n");
		printf ("     This can be used with or without\n");
		printf ("     crossover values being specified.\n\n");
		return 1;
	}

	if ((strncmp (argv[1], "-h", 2) == 0))
	{
		format = 1;
	}
	else if ((strncmp (argv[1], "-", 1) == 0))
	{
		printf ("Invalid argument. Refer to README.\n");
		return 1;
	}
	// If crossover values are provided, convert them to integers and ensure they're between 0 and 255.
	else if (argc > 3) 
	{
		highValue = atoi (argv[(2 + format)]);
		lowValue = atoi (argv[(3 + format)]);

		printf ("; The format is %d\n", format);
		printf ("; High value is %d\n", highValue);
		printf ("; Low value is %d\n", lowValue);

		if (highValue > 255 || highValue < 0)
		{
			printf ("\nHigh crossover cannot be greater than 255 or lower than 0.\n\n");
			return 1;
		}
		else if (lowValue < 0 || lowValue > 255)
		{
			printf ("\nLow crossover cannot be greater than 255 or lower than 0.\n\n");
			return 1;
		}
	}

	// Open file and ensure it opened correctly.
	fp = fopen (argv[1 + format], "rb");
	if (fp == NULL)
	{
		fprintf (stderr, "Error opening file.\n");
		perror (argv[1 + format]);
		return EXIT_FAILURE;
	}

	// Check header info, exit if input file is incompatible.
	if (!(header (fp))) return EXIT_FAILURE;

	// Ensure the end of file can be seeked correctly.
	if (fseek (fp, 0, SEEK_END))
	{
		fclose (fp);
		return EXIT_FAILURE;
	}

	// Determine input file length.
	length = ftell (fp);
	rewind (fp);

	convert (fp, length, highValue, lowValue, format);
	printf ("\n");

	return (EXIT_SUCCESS);
}
