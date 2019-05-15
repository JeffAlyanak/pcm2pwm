/* header.c
 *
 * Read and validate header of input wav file.
 * Return errors or success based on what the header says.
 * Pretty simple.
 *
 * code: jeff alyanak
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int header (FILE * fp)
{
	int ret				= 1;
	char c[4]			= { 0 };	// Byte buffer.
	const char riff[]	= "RIFF";
	const char wave[]	= "WAVE";
	const char chan		= 1;
	const char bits		= 8;

	fseek (fp, 0, SEEK_SET);	// Seek the beginning.
	fread (&c, 1, 4, fp);		// Read 4 bytes.
	ret = strncmp (c, riff, 4);	// Compare 4 bytes with RIFF.
	if (ret)
	{
		printf ("\nThis is not a wave file.\n");
		return -1;
	}

	fseek (fp, 8, SEEK_SET);	// Seek byte 8 .
	fread (&c, 1, 4, fp);		// Read 4 bytes.
	ret = strncmp (c, wave, 4);	// Compare 4 bytes with WAVE.
	if (ret)
	{
		printf ("\nThis is not a wave file.\n");
		return -1;
	}

	fseek (fp, 22, SEEK_SET);	// Seek byte 23 .
	fread (&c, 1, 1, fp);		// Read 1 bytes.
	if (c[0] != chan)		// Ensure wave file is mono.
	{
		printf ("\nThis is not a mono wave.\n");
		return -1;
	}

	fseek (fp, 34, SEEK_SET);	// Seek byte 34 .
	fread (&c, 1, 1, fp);		// Read 1 bytes.
	if (c[0] != bits)		// Ensure wave file is 8-bit.
	{
		printf ("\nThis is not an 8-bit wave.\n");
		return -1;
	}

	return 1;
}
