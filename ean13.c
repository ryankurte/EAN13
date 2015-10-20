#include <stdio.h>
#include <stdbool.h>

#define UPCA_MODULE 7
#define UPCA_DIGITS 12
#define UPCA_WIDTH   (sizeof(quiet_zone)*2+sizeof(lead_trailer)*2+sizeof(separator)+UPCA_MODULE*UPCA_DIGITS)

enum parity_e {
	PARITY_ODD = 0,
	PARITY_EVEN = 1
};

static bool quiet_zone[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool lead_trailer[] = {1, 0, 1};
static bool separator[] = {0, 1, 0, 1, 0};

static bool modules_odd_left[10][7] = {
	{0, 0, 0, 1, 1, 0, 1}, {0, 0, 1, 1, 0, 0, 1}, {0, 0, 1, 0, 0, 1, 1}, {0, 1, 1, 1, 1, 0, 1},
	{0, 1, 0, 0, 0, 1, 1}, {0, 1, 1, 0, 0, 0, 1}, {0, 1, 0, 1, 1, 1, 1}, {0, 1, 1, 1, 0, 1, 1},
	{0, 1, 1, 0, 1, 1, 1}, {0, 0, 0, 1, 0, 1, 1}
};

static bool modules_even_left[10][7] = {
	{0, 1, 0, 0, 1, 1, 1}, {0, 1, 1, 0, 0, 1, 1}, {0, 0, 1, 1, 0, 1, 1}, {0, 1, 0, 0, 0, 0, 1},
	{0, 0, 1, 1, 1, 0, 1}, {0, 1, 1, 1, 0, 0, 1}, {0, 0, 0, 0, 1, 0, 1}, {0, 0, 1, 0, 0, 0, 1},
	{0, 0, 0, 1, 0, 0, 1}, {0, 0, 1, 0, 1, 1, 1}
};

static bool modules_right[10][7] = {
	{1, 1, 1, 0, 0, 1, 0}, {1, 1, 0, 0, 1, 1, 0}, {1, 1, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 0, 1, 0},
	{1, 0, 1, 1, 1, 0, 0}, {1, 0, 0, 1, 1, 1, 0}, {1, 0, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 1, 0, 0},
	{1, 0, 0, 1, 0, 0, 0}, {1, 1, 1, 0, 1, 0, 0}
};

static bool parities[10][5] = {
	{0, 0, 0, 0, 0}, {0, 1, 0, 1, 1}, {0, 1, 1, 0, 1}, {0, 1, 1, 1, 0}, {1, 0, 0, 1, 1},
	{1, 1, 0, 0, 1}, {1, 1, 1, 0, 0}, {1, 0, 1, 0, 1}, {1, 0, 1, 1, 0}, {1, 1, 0, 1, 0}
};

static char test[] = "222543841541";

static bool test_code[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,  //quiet
	1, 0, 1,                    //porch
	0, 0, 1, 0, 0, 1, 1,        //2 left_odd
	0, 0, 1, 0, 0, 1, 1,        //2 left_odd
	0, 1, 1, 1, 0, 0, 1,        //5 left_even
	0, 0, 1, 1, 1, 0, 1,        //4 left_even
	0, 1, 1, 1, 1, 0, 1,        //3 left_odd
	0, 0, 0, 1, 0, 0, 1,        //8 left_even
	0, 1, 0, 1, 0,              //center
	1, 0, 1, 1, 1, 0, 0,        //4 right
	1, 1, 0, 0, 1, 1, 0,        //1 right
	1, 0, 0, 1, 1, 1, 0,        //5 right
	1, 0, 1, 1, 1, 0, 0,        //4 right
	1, 1, 0, 0, 1, 1, 0,        //1 right
	1, 1, 1, 0, 1, 0, 0,        //9 right
	1, 0, 1,                    //trailer
	0, 0, 0, 0, 0, 0, 0, 0, 0   //quiet
};

void write_n(bool *dest, bool *src, int n)
{
	int i;
	//printf("Write to: %p\r\n", dest);
	for (i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

void s_print_bool_arr(bool* arr, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		printf("%d", (int)arr[i]);
	}
	printf("\r\n");
}

int build_ean(char *data, bool *code)
{
	int index = 0;
	int num = 0;
	int start = 0;
	int i;

	//Clear code
	for (i = 0; i < UPCA_WIDTH; i++) {
		code[i] = 0;
	}

	//Create leading quiet zone
	write_n(code + index, quiet_zone, sizeof(quiet_zone));
	index += sizeof(quiet_zone);

	//Create leading trailer (porch)
	write_n(code + index, lead_trailer, sizeof(lead_trailer));
	index += sizeof(lead_trailer);

	//Left hand data here
	start = data[0] - 0x30;

	//First char is a special case
	num = data[1] - 0x30;
	write_n(code + index, modules_odd_left[num], UPCA_MODULE);
	index += UPCA_MODULE;

	//Write left hand data
	for (i = 2; i < 7; i++) {
		num = data[i] - 0x30;
		if (parities[start][i - 2] == 0) {
			write_n(code + index, modules_odd_left[num], UPCA_MODULE);
		} else {
			write_n(code + index, modules_even_left[num], UPCA_MODULE);
		}
		index += UPCA_MODULE;
	}

	//Create center separator
	write_n(code + index, separator, sizeof(separator));
	index += sizeof(separator);

	//Right hand data here
	for (i = 7; i < 12; i++) {
		num = data[i] - 0x30;
		write_n(code + index, modules_right[num], 7);
		index += UPCA_MODULE;
	}

	//Calculate Checksum
	int odds = 0;
	int evens = 0;
	for (int i = 0; i < 12; i++) {
		if (i % 2 == 0) {
			evens += data[i] - 0x30;
		} else {
			odds += data[i] - 0x30;
		}
	}

	//Write checksum
	int checksum = 10 - (((odds * 3) + evens) % 10);
	write_n(code + index, modules_right[checksum], UPCA_MODULE);
	index += UPCA_MODULE;

	//Following trailer
	write_n(code + index, lead_trailer, sizeof(lead_trailer));
	index += sizeof(lead_trailer);

	//Trailing quiet zone here
	write_n(code + index, quiet_zone, sizeof(quiet_zone));
	index += sizeof(quiet_zone);

	return 0;
}

int main(int argc, char** argv)
{

	int ean_size = UPCA_WIDTH;
	printf("EAN barcode size: %d\r\n", ean_size);

	bool code[UPCA_WIDTH];

	build_ean(test, code);

	for (int i = 0; i < UPCA_WIDTH; i++) {
		if (code[i] != test_code[i]) {
			printf("Error in barcode at position: %d\r\n", i);
			break;
		}
	}

	printf("Created:  ");
	s_print_bool_arr(code, UPCA_WIDTH);
	printf("Expected: ");
	s_print_bool_arr(test_code, UPCA_WIDTH);

	return 0;
}

