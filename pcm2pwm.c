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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcm2pwm.h"
#include "convert.h"
#include "header.h"

FILE *infile;			// Audio input file pointer.
FILE *outfile;			// Binary output file pointer.

int length = 0;			// File length in bytes.
int highValue = 0xFC;	// Default high crossover.
int lowValue = 0x03;	// Default low crossover
int format = 0;			// Default (0), HT2 format (1), DASM format (2), binary format (4).

int format_h	= 0;
int format_dasm = 0;
int format_bin	= 0;

char *file_name = NULL;
char *bin_name  = NULL;

int main (int argc, char **argv)
{
	// If no arguments are provided, return help info
	if (argc == 1) return(print_usage());

	// getopt argument handling
	int option_index = 0;
	while (( option_index = getopt(argc, argv, "i:H:L:b:hd")) != -1)
	{
		switch(option_index)
		{
			// Option: file name
			case 'i':
				file_name = optarg;
				break;
			// Option: high crossover
			case 'H':
				if (valid_crossover(optarg))
				{
					highValue = atoi(optarg);
				}
				break;
			// Option: low crossover
			case 'L':
				if (valid_crossover(optarg))
				{
					lowValue = atoi(optarg);
				}
				break;
			// Option: HT2 output mode
			case 'h':
				if (format_set())
				{
					printf("Error: Multiple format options selected!\n");
					return 1;
				}
				format_h = 1;
				break;
			// Option: DASM output mode
			case 'd':
				if (format_set())
				{
					printf("Error: Multiple format options selected!\n");
					return 1;
				}
				format_dasm = 1;
				break;
			// Option: binary output mode
			case 'b':
				if (format_set())
				{
					printf("Error: Multiple format options selected!\n");
					return 1;
				}
				format_bin = 1;
				bin_name = optarg;
				break;
			// Handle missing arguments
			// TODO: Contextual error messages
			case '?':
				if (
					optopt == 'i' ||
					optopt == 'H' ||
					optopt == 'L' ||
					optopt == 'b'
					)
				{
					printf("Option -%c requires an argument.\n", optopt);
					return(print_usage());
				}
				return 1;
			default:
				printf("Invalid option\n");
				return 1;
		}
	}

	// Get actual format int
	format =  (format_h | format_dasm << 1 | format_bin << 2);

	// Open output if binary mode
	if (format == 4)
	{
		outfile = fopen (bin_name, "wb");
	}

	// Open file and ensure it opened correctly.
	infile = fopen (file_name, "rb");
	if (infile == NULL)
	{
		fprintf (stderr, "Error opening file.\n");
		perror (file_name);
		return EXIT_FAILURE;
	}

	// Check header info, exit if input file is incompatible.
	if (!(header (infile))) return EXIT_FAILURE;

	// Ensure the end of file can be seeked correctly.
	if (fseek (infile, 0, SEEK_END))
	{
		fclose (infile);
		return EXIT_FAILURE;
	}

	// Determine input file length.
	length = ftell (infile);
	rewind (infile);

	convert (infile, outfile, length, highValue, lowValue, format);

	// Close output if binary mode, add newline if not.
	if (format == 4)
	{
		fclose(outfile);
	}
	else
	{
		printf ("\n");
	}
	
	return (EXIT_SUCCESS);
}

// True if crossover value is valid
int valid_crossover(char value[])
{
	int v = atoi(value);

	if (v > 255 || v < 0)
	{
		printf ("\nWARNING: Crossover cannot be greater than 255 or less than 0. Using default instead\n\n");
		return 0;
	}
	else
	{
		return 1;
	}
}

// True if format already set.
int format_set()
{
	if (format_h == 0 && format_dasm == 0)
	{
		return 0;
	}
	return 1;
}

// Print usage text
int print_usage()
{
	printf ("NAME\n");
	printf ("        pcm2pwm 1.0 - Convert pcm audio to differential pwm.\n");
	printf ("\nUSAGE\n");
	printf ("        pcm2pwm [OPTIONS]... -i [FILE] \n");
	printf ("\nOPTIONS\n");
	printf ("        -i\n");
	printf ("            input wav file (required)\n");
	printf ("        -H\n");
	printf ("            high crossover value, range 0-255 (default: 252)\n");
	printf ("        -L\n");
	printf ("            low crossover value, range 0-255 (default: 3)\n");
	printf ("\nOUTPUT MODES\n");
	// printf ("    only one may be selected\n");
	printf ("        -h\n");
	printf ("            Houston Tracker 2 output format (inverted values)\n");
	printf ("        -d\n");
	printf ("            DASM tabbed output format (\\t before each .byte)\n");
	printf ("        -b\n");
	printf ("            Output to a binary file, must provide filename\n");
	return 1;
}