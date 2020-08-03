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
				// This hardcoded truncation will mess with any extremely low frequency
				// sounds you have by preventing the period time from being any higher
				// than 255. This frequency floor will be different depending on your
				// playback routine rate.

				if (count > 0xFF) count = 0xFF;
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

					// If HT2 format
					if (format == 1)
					{
						// Zero bytes change can't be represented
						if (count == 0) count=1;

						// Print the period value in the frequency factor format ($01-$FF), rounded to nearest int.
						printf ("$%02X", ((0xFF-(count+1)/2)/count)+1);
					}
					else
					{
						// Print the period value in the standard format ($00-$FF).
						printf ("$%02X", (count));
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
