/* Copyright (C) 2012 Chol Nhial <ch01.cout@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <getopt.h>
#include "application.h"
#include "global.h"


/* Description of long options for getopt_long.  */

static const struct option long_options[] = {
    { "address",          1, NULL, 'a' },
    { "help",             0, NULL, 'h' },
    { "upload",           0, NULL, 'u' },
    { "download",         0, NULL, 'd' },
    { "listen",           1, NULL, 'l' },
    { "input",            1, NULL, 'i' },
    { "output",           1, NULL, 'o' },
    { "port",             1, NULL, 'p' },
    { "verbose",          0, NULL, 'v' },
};
/* Usage summary text.  */

static const char* const usage_template =
    "Usage: %s [ options ]\n"
    "  -a, --address ADDR        Connecting address.\n"
    "  -h, --help                Print this information.\n"
    "  -u, --upload              Upload a file.\n"
    "  -d, --download            Download a file.\n"
    "  -i, --input FILE          Input file for uploading.\n"
    "  -o, --output DIR          Output directory.\n"
    "  -p, --port PORT           Connecting port.\n"
    "  -l, --listen PORT         Listening port.\n"
    "  -v, --verbose             Print verbose messages.\n";

/* Description of short options for getopt_long.  */

static const char* const short_options = "a:hudl:i:o:p:v";


static void print_usage (int is_error)
{
    fprintf (is_error ? stderr : stdout, usage_template, program_name);
    exit (is_error ? 1 : 0);
}


int main(int argc, char **argv)
{
    const char * port;
    const char * host;
    App app;
    using std::string;

    string inpath;
    bool valid_options[7] =
    {
        false,
        false,
        false,
        false,
        false,
        false,
        false
    };

// Assign program name
    program_name = argv[0];

    verbose = 0;
    outputdir = NULL;
    int next_option;
    /* Parse options.  */
    do {
        next_option =
            getopt_long (argc, argv, short_options, long_options, NULL);
        switch (next_option) {
        case 'a':
            /* User specified -a or --address.  */
        {
            host = optarg;
            valid_options[0] = true;
        }
        break;

        case 'h':
            /* User specified -h or --help.  */
            print_usage (0);
        case 'u':
            /* User specified -u or  --upload  */
            valid_options[1] = true;
            break;
        case 'd': /* User specified -d or --download. */
            valid_options[2] = true;
            break;
        case 'i': /* User specified -i or --input. */
        {
            struct stat file_info;

            /* Check that it exists.  */
            if (access (optarg, F_OK) != 0)
                error (optarg, "File does not exist");
            /* Check that it is accessible.  */
            if (access (optarg, R_OK) != 0)
                error (optarg, "File is not accessible");
            /* Make sure that it is a file  */
            if (stat (optarg, &file_info) != 0 || !S_ISREG (file_info.st_mode))
                error (optarg, "Not a regular file");
            /* It looks OK, so use it.  */
            inpath = optarg;
            if (inpath[inpath.size()] == '/')
                inpath[inpath.size()] = '\0';
            inputdir = inpath.c_str();
            valid_options[3] = true;
        }
        break;
        case 'o':
        {/* User specified -o or --output */
            struct stat dir_info;
            /* Check that it exists.  */
            if (access (optarg, F_OK) != 0)
                error (optarg, "Directory does not exist");
            /* Check that it is accessible.  */

            /* Make sure that it is directory file  */
            if (stat (optarg, &dir_info) != 0 || !S_ISDIR (dir_info.st_mode))
                error (optarg, "Not a directory");
            else
                if (access (optarg, R_OK | W_OK) != 0)
                    error (optarg, "Directory is not accessible");
            /* It looks OK, so use it.  */
            outputdir = optarg;
            valid_options[4] = true;
        }
        break;

        case 'p':
            /* User specified -p or --port.  */
        {
            char* end;

            strtol (optarg, &end, 10);
            if (*end != '\0')
                /* The user specified non-digits in the port number.  */
                print_usage (1);

            port = optarg;
            valid_options[5] = true;
        }
        break;

        case 'l': /* User specified -l or listen. */
        {
            char* end;

            strtol (optarg, &end, 10);
            if (*end != '\0')
                /* The user specified non-digits in the port number.  */
                print_usage (1);

            port = optarg;
            valid_options[6] = true;
        }
        break;
        case 'v':
            /* User specified -v or --verbose.  */
            verbose = 1;
            break;

        case '?':
            /* User specified an nrecognized option.  */
            print_usage (1);

        case -1:
            /* Done with options.  */
            break;

        default:
            abort ();
        }
    } while (next_option != -1);

    /* Does the user want to download a file */
    if (valid_options[2] == true && valid_options[6] == true
            &&  valid_options[4] == true && valid_options[5] == false && valid_options[3] == false && valid_options[1] == false && valid_options[0] == false)
        app.download(port);

    /* Does the user want to upload a file.*/
    else if (valid_options[2] == false && valid_options[6] == false
             &&  valid_options[4] == false && valid_options[5] == true && valid_options[3] == true && valid_options[1] == true && valid_options[0] == true)
        app.upload(host,port,inputdir);
    /* User entered invalid options */
    else
        print_usage(0);

    return 0;
}
