#include "YUVSequenceReader.h"
#include "ComplexityAnalyzer.h"

#include <algorithm>

char *inputFile = NULL;
char *outputFile = NULL;
int width = -1;
int height = -1;

IVideoSequenceReader *getReader (string filename, int width, int height)
{
	IVideoSequenceReader *pReader = NULL;

	unsigned int pos = filename.find_last_of('.');

	if (pos < filename.size())
	{
		string ext = filename.substr(pos);

		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		if (ext.compare(".yuv") == 0)
			pReader = new YUVSequenceReader(filename, width, height);
	}

	return pReader;
}

void parse_options (int argc, char *argv[])
{
	if(argc<5)
	{
		printf("Usage: %s <input_file> <width> <height> <output_file>\n", argv[0]);
		printf("\n");
		printf("Parameters:\n");
		printf("<input_file> ... The file to be read. It can be a '.yuv' file containing YUV frames,\n");
		printf("                 or a '.avs' file that can be read via avisynth. At present only these\n");
		printf("                 two file types are supported\n");
		printf("\n");
		printf("<width> ........ The width of the picture in the file. This parameter must be passed\n");
		printf("                 in for all file types. It is used for '.yuv' files, but ignored for\n");
		printf("                 '.avs' files. Note the input picture is not resampled. This parameter\n");
		printf("                 is only used to get the picture size for files that do not have any\n");
		printf("                 mechanism to signal it.\n");
		printf("\n");
		printf("<height> ....... The height of the picture in the file. This parameter must be passed\n");
		printf("                 in for all file types. It is used for '.yuv' files, but ignored for\n");
		printf("                 '.avs' files. Note the input picture is not resampled. This parameter\n");
		printf("                 is only used to get the picture size for files that do not have any\n");
		printf("                 mechanism to signal it.\n");
		printf("\n");
		printf("<output_file> .. The complexity information is written out to this file.\n");
		printf("\n");
		exit(-1);
	}

	inputFile = argv[1];

	width=atoi(argv[2]);
	if(width<=0)
	{
		printf("Illegal width value = %d\n", width);
		exit(-1);
	}

	height=atoi(argv[3]);
	if(height<=0)
	{
		printf("Illegal height value = %d\n", height);
		exit(-1);
	}

	outputFile = argv[4];
}

static FILE *pOut;

void print_compl_inf (complexity_info_t *i)
{
	static int GOP_bits = 0;
	static int SGOP_bits = 0;
	static int GOP_count = 0;
	static int SGOP_size = 5;

	if (i->picType == 'I' && i->picNum>1)
	{
		fprintf(pOut, "GOP: %3d, GOP-bits = %d\n",GOP_count,GOP_bits);
		GOP_count++;
		SGOP_bits += GOP_bits;
		if((GOP_count%SGOP_size)==0)
		{
			if(GOP_count)
			{
				fprintf(pOut, "10-sec GOP: %3d, bits = %d\n",GOP_count/SGOP_size-1,SGOP_bits);
			}
			SGOP_bits = 0;
		}
		GOP_bits = 0;
	}

	GOP_bits += i->bits;
	fprintf(pOut, "Frame %6d (%c), I:%6d, P:%6d, B:%6d, MSE = %9d, bits = %7d\n", i->picNum, i->picType, i->count_I, i->count_P, i->count_B, i->error, i->bits);
}

int main(int argc, char *argv[])
{
    parse_options(argc, argv);
	IVideoSequenceReader *reader = getReader(inputFile, width, height);

    if (reader == nullptr) {
        fprintf(stderr, "Unsupported input format for %s\n", inputFile);
        return 1;
    }

	ComplexityAnalyzer analyzer(reader);

	analyzer.analyze();

	pOut = fopen (outputFile, "w");

	fprintf(pOut, "input_file: '%s'\n", inputFile);
	fprintf(pOut, "width %d\n", reader->width());
	fprintf(pOut, "height %d\n", reader->height());
	fprintf(pOut, "\n");
	fprintf(pOut, "complexity info:\n");

	vector<complexity_info_t *> info = analyzer.getInfo();
	for_each(info.begin(), info.end(), print_compl_inf);
	fclose(pOut);

	return 0;
}