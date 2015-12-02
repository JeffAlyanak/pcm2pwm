/* convert.c
 *
 * Convert function takes a file pointer
 * 
 * routines.
 *
 * See readme for basic use case.
 *
 *
 * code: jeff alyanak
*/

#include <stdio.h>
#include <stdlib.h>
#include "convert.h"


int
convert (FILE * fp, int length, int highValue, int lowValue)
{

  int count = 0;		// Count period between zero cross-overs.
  int flipState = 1;		// High or low state.
  int oldState = 1;		// Previous state.
  int newLine = 0;		// Used to limit each output line to 16 bytes.
  int i = 1;			// Byte position counter.
  unsigned char c = 0;		// Byte buffer.

  while (i < length - 44)
    {				// Move through each byte in the file. Hardcoded to skip past the header.
      fseek (fp, (44 + i), SEEK_SET);	// TO-DO: Look at how wav headers are formed and programmatically skip header.
      fread (&c, 1, 1, fp);

      if (c >= highValue)
	{			// High state is any sample above 252.

	  flipState = 1;	// Set state to high (1).
	  if (flipState != oldState)
	    {			// This is a zero cross-over, write out the time since last crossover (count) and reset it.
	      if (newLine == 0)
		{
		  printf ("\n");
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

	      if (count > 0xFF)
		count = 0xFF;	// This hardcoded truncation will fuck with any extremely low frequncy sounds you have by preventing the period time from being any higher than 255. This frequency floor will be different depending on your playback routine rate.
	      printf ("$%2.2X", count);	// Print the period value in the correct format ($00-$FF).
	      count = 0;
	      oldState = flipState;
	    }
	}
      else if (c <= lowValue)
	{			// Low state is any sample below 3.
	  flipState = 2;	// Set state to low (2).
	  if (flipState != oldState)
	    {			// This is a zero cross-over, write out the time since last crossover (count) and reset it.
	      if (newLine == 0)
		{		// My purpose for this utility is for use in 6800 assembly software, this output formatting is purely to aid in that.
		  printf ("\n");
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

	      if (count > 0xFF)
		count = 0xFF;	// This hardcoded truncation will fuck with any extremely low frequency sounds by preventing the period from being any higher than 255. This frequency floor will depending on your playback routine rate.
	      printf ("$%2.2X", count);	// Print the period value in the correct format ($00-$FF).
	      count = 0;
	      oldState = flipState;
	    }
	}
      count++;
      i++;
    }
  return 1;

}
