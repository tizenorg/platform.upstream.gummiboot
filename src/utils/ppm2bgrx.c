#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef NDEBUG
#       define DEBUG for(;0;)
#else
#       define DEBUG
#endif

char* ppm_readline(FILE*, int);

char* ppm_readline(FILE* stream, int skip_ws) {
	int c, skip;
	size_t i, buf_size;
	char* buf;

	buf_size = 128;
	buf = malloc(buf_size);
	if(!buf)
		goto err;

	if(skip_ws) {
	        while(!feof(stream)) {
        		c = fgetc(stream);
	                if(!strchr(" \t\r\n", c)) {
                                break;
                        }
	        }
	}
start_line:
	skip = 0;
	i = 0;
        
	while(!feof(stream)) {
	        if(skip_ws)
	                skip_ws = 0;
	        else
        		c = fgetc(stream);
        	
		if((i == 0) && (c == '#')) {
			skip = 1;
		}
		if(strchr(skip ? "\r\n" : " \t\r\n", c)) {
			if(skip)
				goto start_line;
			break;
		}
		if(i >= buf_size - 1) {
			buf = realloc(buf, buf_size *= 2);
			if(!buf)
				goto err;
		}
		buf[i++] = c;
	}

	if(skip)
		goto err2;
	if(!i)
		goto err;
	buf[i] = 0;
	return buf;
err2:
	free(buf);
err:
	return NULL;
}

int main(int argc, char* argv[]) {
	FILE* input;
	FILE* output;
	char* line;
	int w,h,maxval;
	long npixels;
	unsigned char binary[2];
	int i;

	if(argc < 2 || !strcmp("-", argv[1])) {
		input = stdin;
	}

	if(argc < 3 || !strcmp("-", argv[2])) {
		output = stdout;
	}

	line = ppm_readline(input, 0);
	if(!line)
		goto err;
	if(strcmp(line, "P6")) {
		fprintf(stderr, "ERROR: Not a valid PPM file (wrong header)!\n");
		goto err2;
	}
	fwrite("BGRX", 4, 1, output);
	free(line);

	line = ppm_readline(input, 1);
	if(!line)
		goto err;
	w = atoi(line);
	if(w <= 0) {
		fprintf(stderr, "ERROR: Not a valid PPM file (zero or negative width)!\n");
		goto err2;
	}
	free(line);
	binary[0] = (unsigned char)((w >> 8) & 0xFF);
	binary[1] = (unsigned char)((w >> 0) & 0xFF);
	fwrite(binary, sizeof(binary), 1, output);

	line = ppm_readline(input, 1);
	if(!line)
		goto err;
	h = atoi(line);
	if(h <= 0) {
		fprintf(stderr, "ERROR: Not a valid PPM file (zero or negative height)!\n");
		goto err2;
	}
	free(line);
	binary[0] = (unsigned char)((h >> 8) & 0xFF);
	binary[1] = (unsigned char)((h >> 0) & 0xFF);
	fwrite(binary, sizeof(binary), 1, output);

	line = ppm_readline(input, 1);
	if(!line)
		goto err;
	maxval = atoi(line);
	if(maxval != 0xFF) {
		fprintf(stderr, "ERROR: Not a valid PPM file (not 8-bit)!\n");
		goto err2;
	}
	free(line);

	npixels = (long)w*(long)h;

	for(i = 0; i < npixels; ++i)
	{
		unsigned char pix_in[3];
		unsigned char pix_out[4];
		if(feof(input) || feof(output))
			goto err;
		fread(pix_in, sizeof(pix_in), 1, input);
		pix_out[0] = pix_in[2];
		pix_out[1] = pix_in[1];	
		pix_out[2] = pix_in[0];	
		pix_out[3] = 0;	
		fwrite(pix_out, sizeof(pix_out), 1, output);
	}
        fclose(output);
        fclose(input);
	return EXIT_SUCCESS;
err2:
	free(line);
err:
	return EXIT_FAILURE;
}
