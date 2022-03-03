
#include "ComplexityAnalyzer.h"
#include <motion_search/inc/fb_command_line_parser.h>
#include "YUVSequenceReader.h"

#include <algorithm>
#include <chrono>

namespace {

struct CTX {

    const char *inputFile = nullptr;
    const char *outputFile = nullptr;
    int width = 0;
    int height = 0;
    int num_frames = 0;

};

std::unique_ptr<IVideoSequenceReader> getReader(std::string filename,
    int width, int height)
{
    std::unique_ptr<IVideoSequenceReader> reader;

    const size_t pos = filename.find_last_of('.');

    if (std::string::npos != pos)
    {
        std::string ext = filename.substr(pos);

        std::transform(ext.begin(), ext.end(), ext.begin(),
            [](int c){return (char)::tolower(c);});

        if (ext.compare(".yuv") == 0) {
            reader.reset(new YUVSequenceReader(filename, width, height));
        }/* else if (ext.compare(".y4m") == 0) {
            reader.reset(new Y4MSequenceReader(filename));
        }*/
    }

    if ((!reader) || !reader->isOpen()) {
        return nullptr;
    }

    return reader;
}

namespace Options {

const char* const ColorFormat = "c";
const char* const Height = "h";
const char* const NumFrames = "n";
const char* const Width = "w";

} // namespace Options

void PrintUsage(const char * const appName) {

    printf("Usage: %s <input_file> -%s=<width> -%s=<height> -%s=<num_frames> -%s=<color_format> <output_file>\n",
        appName, Options::Width, Options::Height, Options::NumFrames, Options::ColorFormat);
    printf("\n");
    printf("Parameters:\n");
    printf("<input_file> ... The file to be read. It can be a '.yuv' or '.y4m' file containing YUV frames,\n");
    printf("                 At present only these two file types are supported\n");
    printf("\n");
    printf("<width> ........ The width of the picture in the file. This parameter must be passed\n");
    printf("                 in for '.yuv' file types. Ignored for '.y4m' files.\n");
    printf("                 Note the input picture is not resampled. This parameter\n");
    printf("                 is only used to get the picture size for files that do not have any\n");
    printf("                 mechanism to signal it.\n");
    printf("\n");
    printf("<height> ....... The height of the picture in the file. This parameter must be passed\n");
    printf("                 in for '.yuv' file types. Ignored for '.y4m' files.\n");
    printf("                 Note the input picture is not resampled. This parameter\n");
    printf("                 is only used to get the picture size for files that do not have any\n");
    printf("                 mechanism to signal it.\n");
    printf("\n");
    printf("<num_frames> ... The number of frames to procees (default: the whole stream)\n");
    printf("\n");
    printf("<color_format> . The used color format, ignored for '.y4m' file types (default: I420)\n");
    printf("\n");
    printf("<output_file> .. The complexity information is written out to this file.\n");
    printf("\n");
    exit(-1);
}

void parse_options(CTX& ctx, facebook::CommandLineParser &args)
{
    if (2 > args.NumFileNames()) {
        printf("provide 2 files (a source then an output)");
        exit(-1);
    }

    ctx.inputFile = args.GetFileName(0);
    ctx.outputFile = args.GetFileName(1);
    ctx.width = args.Get<int32_t> (Options::Width, 0);
    ctx.height = args.Get<int32_t> (Options::Height, 0);
    ctx.num_frames = args.Get<int32_t> (Options::NumFrames, 0);
}

void print_compl_inf (FILE* const pOut, complexity_info_t *i)
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
    fprintf(pOut, "Frame %6d (%c), I:%6d, P:%6d, B:%6d, MSE = %9d, bits = %7d\n",
        i->picNum, i->picType, i->count_I, i->count_P, i->count_B, i->error, i->bits);
}

} // namespace

int main(int argc, char *argv[])
{
    facebook::CommandLineParser args((size_t) argc, argv);
    CTX ctx;

    if (1 == argc) {
        PrintUsage(argv[0]);
        return 1;
    }

    parse_options(ctx, args);
    auto reader = getReader(ctx.inputFile, ctx.width, ctx.height);
    if (reader == nullptr) {
        fprintf(stderr, "Unsupported input format for %s\n", ctx.inputFile);
        return 1;
    }

    ComplexityAnalyzer analyzer(reader.get());

    const auto begin = std::chrono::high_resolution_clock::now();
    analyzer.analyze();
    const auto end = std::chrono::high_resolution_clock::now();

    std::unique_ptr<FILE, file_closer> out(fopen(ctx.outputFile, "w"));
    if (!out) {
        fprintf(stderr, "can't open output file %s\n", ctx.outputFile);
        return 1;
    }

    fprintf(out.get(), "input_file: '%s'\n", ctx.inputFile);
    fprintf(out.get(), "width %d\n", reader->width());
    fprintf(out.get(), "height %d\n", reader->height());
    fprintf(out.get(), "\n");
    fprintf(out.get(), "complexity info:\n");

    vector<complexity_info_t *> info = analyzer.getInfo();
    for_each(info.begin(), info.end(),
        [&](complexity_info_t *i){print_compl_inf(out.get(), i);});

    const std::chrono::duration<double, std::milli> duration = end - begin;
    fprintf(out.get(), "Execution time = %.2f msec\n", duration.count());

    return 0;
}
