/* convert.c
 *
 * Convert function takes a file pointer, file length, high and low crossover values. Outputs the pulse width
 * intervals in a format designed for use in assembly language latch audio routines.
 *
 * code: jeff alyanak
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "convert.h"

int convert (FILE *infile, FILE *outfile, int length, int highValue, int lowValue, int format)
{
	int count = 0;			// Count period between zero cross-overs.
	int flipState = 1;		// High or low state.
	int oldState = 1;		// Previous state.
	int newLine = 0;		// Used to limit each output line to 16 bytes.
	int i = 1;				// Byte position counter.
	unsigned char c = 0;	// Byte buffer.

	// This hardcoded truncation will mess with any extremely low frequnc
	// sounds you have by preventing the period time from being any higher
	// than 255. This frequency floor will be different depending on your
	// playback routine rate.
	if (count > 0xFF) count = 0xFF;

	// Move through each byte in the file. Hardcoded to skip past the header.
	while (i < length - 44)
	{
		fseek (infile, (44 + i), SEEK_SET);	// Skip the 44 bytes of the wav header.
		fread (&c, 1, 1, infile);

		if (c >= highValue || c <= lowValue)	// High state defaults to any sample above 252. Low state is any sample below 3.
		{
			if (flipState == 1 && c <= lowValue) flipState = 2;
			if (flipState == 2 && c >= highValue) flipState = 1;

			if (flipState != oldState)	// This is a zero cross-over, write out the time since last crossover (count) and reset it.
			{
				if (format != 4)
				{
					if (newLine == 0)
						{
							printf ("\n");
							if (format == 2) printf("\t");	// DASM tab format
							printf (".byte ");
							newLine++;
						}
						else if (newLine == 15)
						{
							printf (",");
							newLine = 0;
						}
						else
						{
							printf (",");
							newLine++;
						}

						// If HT2 format (inverted)
						if (format == 1)
						{
							printf ("$%02X", (1*((INT_MAX - count) - (INT_MAX - 0xFF)))); 	// Print the period value in the inverted format ($FF-$00).
						}
						else
						{
							printf ("$%02X", (count));	// Print the period value in the standard format ($00-$FF).
						}
				}
				else
				{
					fwrite(&count,1,1,outfile);
				}

				count = 0;
				oldState = flipState;
			}
		}
		count++;
		i++;
	}
	return 1;
}
